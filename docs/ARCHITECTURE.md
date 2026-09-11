# ARCHITECTURE.md — WazeHUD Firmware Architecture

> Read this before implementing any new module. Layer boundaries are enforced in GEMINI.md §8.

---

## Layer Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                    APPLICATION LAYER                            │
│   main.cpp — init, task creation, system orchestration          │
├────────────────────────┬────────────────────────────────────────┤
│   TRANSPORT LAYER      │   DISPLAY LAYER                        │
│                        │                                        │
│  spp_transport.cpp     │  display_driver.cpp  (LVGL + TFT_eSPI) │
│  wifi_transport.cpp    │  hud_screen.cpp      (main HUD UI)     │
│                        │  connect_screen.cpp  (pairing screen)   │
│  Both feed bytes into  │  alert_screen.cpp    (overlay popups)  │
│  g_hlp_rx_queue        │  turn_icons.h        (bitmap arrays)   │
├────────────────────────┴────────────────────────────────────────┤
│                    HLP/1 PROTOCOL LAYER                         │
│                                                                 │
│  hlp_core.cpp        — frame receiver, UTF-8 validator          │
│  hlp_messages.cpp    — dev/hi/ping/pong handler                 │
│  hlp_state.cpp       — JSON → HudState decoder                  │
│  hlp_device_config.cpp — dynamic config (cfg_* messages)        │
├─────────────────────────────────────────────────────────────────┤
│                    UTILITY LAYER                                 │
│  nvs_config.cpp  — Arduino Preferences NVS wrapper              │
│  logger.h        — LOG_E/W/I/D macros                           │
│  config.h        — ALL constexpr pin/constant definitions        │
└─────────────────────────────────────────────────────────────────┘
```

---

## Data Flow

```
              Waze Mod (Android)
                    │
         ┌──────────┴──────────┐
    BT Classic SPP        WiFi WebSocket
         │                    │
  spp_transport           wifi_transport
         │                    │
         └────────┬───────────┘
                  │  raw bytes (chunk_t)
                  ▼
           g_hlp_rx_queue (FreeRTOS queue, 16 slots)
                  │
                  ▼
          protocol_task (Core 0, pri 5)
                  │
         hlp_core: assemble frames
                  │
         hlp_messages: classify by "t"
                  │
         ┌────────┴────────────┐
         │                     │
   t="ping"               t="s"
   send pong          hlp_state: decode JSON
                            │
                            ▼
                     g_hud_state (HudState)
                     protected by g_state_mutex
                            │
                            ▼
                   render_task (Core 1, pri 4)
                            │
                   copy HudState under mutex
                            │
                   LVGL: update widgets
                            │
                   TFT_eSPI: flush to ILI9341
```

---

## FreeRTOS Task Model

| Task | Core | Priority | Stack | Owner |
|------|------|----------|-------|-------|
| `spp_rx_task` | 0 | 6 | 4096 | spp_transport.cpp |
| `wifi_rx_task` | 0 | 6 | 4096 | wifi_transport.cpp |
| `protocol_task` | 0 | 5 | 4096 | main.cpp |
| `render_task` | 1 | 4 | 8192 | display_driver.cpp |

### Task Details

#### spp_rx_task (Core 0, priority 6)
- Reads BluetoothSerial in polling loop.
- Packs bytes into chunk_t → sends to g_hlp_rx_queue.
- Must NOT block >10 ms. Must NOT call LVGL or Preferences.

#### wifi_rx_task (Core 0, priority 6)
- Accepts WebSocket connections from Waze Mod.
- Each WS text message → chunk_t → g_hlp_rx_queue.
- Handles WebSocket-level ping/pong automatically.

#### protocol_task (Core 0, priority 5)
- Receives chunk_t from g_hlp_rx_queue.
- Feeds bytes into HlpRx framer.
- On complete frame:
  - ping → send pong immediately (before anything else)
  - hi → update session, clear stale state
  - s → hlp_state_decode() → lock g_state_mutex → update g_hud_state → unlock
  - cfg_* → hlp_device_config_handle()
- Sends dev after link-up (via g_link_event_group).
- Detects stale: millis() - g_hud_state.last_rx_ms > 3000 → set NO_SIGNAL display state.

#### render_task (Core 1, priority 4)
- Calls lv_timer_handler() every 5 ms.
- Every 50 ms: lock g_state_mutex → copy HudState → unlock.
- Updates LVGL widgets from local copy.
- Owns all screen state transitions.

---

## Shared State & Synchronization

```cpp
// Declared in main.cpp, extern'd in headers

HudState           g_hud_state;        // protocol writes, render reads
SemaphoreHandle_t  g_state_mutex;      // protects g_hud_state
SemaphoreHandle_t  g_lvgl_mutex;       // protects all lv_* API
QueueHandle_t      g_hlp_rx_queue;     // transport → protocol, 16 x chunk_t
EventGroupHandle_t g_link_event_group; // LINK_SPP_UP, LINK_WIFI_UP, etc.
DisplayState       g_display_state;    // atomic enum, render_task owns writes
```

### Event group bits
```cpp
constexpr EventBits_t LINK_SPP_UP     = BIT0;
constexpr EventBits_t LINK_WIFI_UP    = BIT1;
constexpr EventBits_t LINK_HLP_ACTIVE = BIT2;  // after successful handshake
constexpr EventBits_t LINK_STALE      = BIT3;  // set by protocol_task on timeout
```

---

## Initialization Sequence

```
1.  Serial.begin(115200)
2.  logger_init()
3.  nvs_config_load()              ← load settings from Preferences
4.  display_driver_init()          ← TFT_eSPI + LVGL + show BOOT screen
5.  hlp_device_config_init()       ← load HLP cfg from Preferences
6.  g_hlp_rx_queue = xQueueCreate(16, sizeof(chunk_t))
7.  g_state_mutex  = xSemaphoreCreateMutex()
8.  g_lvgl_mutex   = xSemaphoreCreateMutex()
9.  g_link_event_group = xEventGroupCreate()
10. hlp_rx_init(&g_hlp_receiver, on_hlp_line, nullptr)
11. spp_transport_start("WazeHUD")
12. if (nvs_config.wifi_enabled): wifi_transport_start()
13. xTaskCreatePinnedToCore(render_task, ..., 1)   ← Core 1
14. xTaskCreatePinnedToCore(protocol_task, ..., 0)  ← Core 0
```

---

## Memory Budget

| Region | Size | Notes |
|--------|------|-------|
| FreeRTOS overhead | ~30 KB | |
| spp_rx_task stack | 4 KB | |
| wifi_rx_task stack | 4 KB | |
| protocol_task stack | 4 KB | |
| render_task stack | 8 KB | |
| LVGL heap (LV_MEM_SIZE) | 32 KB | Static pool |
| HLP frame buffer | 512 B | Static in hlp_core |
| HLP queue | ~8 KB | 16 × sizeof(chunk_t) |
| HudState struct | ~256 B | Static global |
| TFT_eSPI draw buffer | ~960 B | 1-line DMA |
| **Total estimated** | **~93 KB** | Well within 520 KB |

---

## Module Dependency Rules

- Transport layer → HLP layer (hlp_core only) ✓
- HLP layer → Utility layer ✓
- Display layer → HLP state (hlp_state.h, read-only) ✓
- **Transport ↔ Display: FORBIDDEN**
- **HLP layer → Display: FORBIDDEN**
- **Circular imports: FORBIDDEN**

---

## Transport Selection Logic

```cpp
// chunk_t carries transport_id so protocol_task can filter
typedef struct {
    uint8_t  transport_id;    // 0 = SPP, 1 = WiFi
    uint16_t length;
    uint8_t  bytes[HLP_MAX_FRAME];
} chunk_t;

// Active transport priority: SPP > WiFi
// protocol_task drops chunks from non-active transport
```

---

## Screen State Machine

```
     ┌──────────┐
     │   BOOT   │  (logo + "Đang khởi động...")
     └────┬─────┘
          │ transports ready
     ┌────▼─────────┐
     │  CONNECTING  │  (BT icon + "Đang tìm Waze...")
     └────┬─────────┘
          │ LINK_HLP_ACTIVE set
     ┌────▼─────────┐◄──── reconnect ────┐
     │  HUD_ACTIVE  │                    │
     └────┬─────────┘                    │
          │ stale > 3 s              disconnect
     ┌────▼─────────┐                    │
     │  NO_SIGNAL   │────────────────────┘
     └──────────────┘
```

Alert overlays appear on top of HUD_ACTIVE as LVGL popups (not separate screens).

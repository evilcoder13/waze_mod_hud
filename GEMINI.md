# GEMINI.md — WazeHUD CYD Project Rules

> This file is the **authoritative architecture and coding contract** for Gemini (and any AI agent)
> working on the WazeHUD project. Read this file **before** modifying any source file.
> All proposed changes must conform to the rules below.

---

## 1. Project Identity

| Property | Value |
|---|---|
| **Project name** | WazeHUD |
| **Target board** | ESP32-2432S028 (CYD — Cheap Yellow Display) |
| **SoC** | ESP32 dual-mode (Xtensa LX6, 240 MHz, 520 KB SRAM, 4 MB Flash) |
| **Display** | ILI9341 TFT 2.8 inch, 240 × 320, SPI |
| **Touch** | XPT2046 resistive, shares SPI bus with display |
| **Framework** | Arduino (via PlatformIO) |
| **Primary transport** | Bluetooth Classic SPP (Bluedroid) |
| **Secondary transport** | WiFi WebSocket (`ws://<ip>:8765/hlp`) |
| **HUD data source** | Waze Mod via HLP/1 protocol |
| **Language** | C++17 (firmware), Markdown (docs) |

---

## 2. Key Reference Documents

Always read these before implementing any feature:

| File | Purpose |
|---|---|
| [`waze-hud-link-sdk-ai-bundle.md`](./waze-hud-link-sdk-ai-bundle.md) | **Normative** HLP/1 spec, all UUIDs, enum tables, handshake rules |
| [`HARDWARE.md`](./HARDWARE.md) | Pin definitions — never hard-code GPIO numbers in source |
| [`ARCHITECTURE.md`](./ARCHITECTURE.md) | Layer boundaries, task model, data flow — do not violate |
| [`DISPLAY_LAYOUT.md`](./DISPLAY_LAYOUT.md) | Screen wireframe and widget coordinates |
| [`PROTOCOL_NOTES.md`](./PROTOCOL_NOTES.md) | Edge cases and implementation notes for HLP/1 |
| [`BUILD.md`](./BUILD.md) | Build toolchain, dependencies, flash procedure |
| [`TEST_PLAN.md`](./TEST_PLAN.md) | Test matrix — implement tests from here when asked |

---

## 3. Directory Layout (Immutable)

```
d:\Projects\WazeHUD\
├── GEMINI.md                    ← THIS FILE — read first
├── HARDWARE.md
├── ARCHITECTURE.md
├── BUILD.md
├── DISPLAY_LAYOUT.md
├── PROTOCOL_NOTES.md
├── TEST_PLAN.md
├── waze-hud-link-sdk-ai-bundle.md
└── firmware/
    ├── platformio.ini
    ├── src/
    │   ├── main.cpp
    │   ├── config.h             ← ALL pin/constant definitions live here
    │   ├── transport/
    │   │   ├── spp_transport.h
    │   │   ├── spp_transport.cpp
    │   │   ├── wifi_transport.h
    │   │   └── wifi_transport.cpp
    │   ├── hlp/
    │   │   ├── hlp_core.h
    │   │   ├── hlp_core.cpp
    │   │   ├── hlp_messages.h
    │   │   ├── hlp_messages.cpp
    │   │   ├── hlp_device_config.h
    │   │   ├── hlp_device_config.cpp
    │   │   ├── hlp_state.h
    │   │   └── hlp_state.cpp
    │   ├── display/
    │   │   ├── display_driver.h
    │   │   ├── display_driver.cpp
    │   │   ├── hud_screen.h
    │   │   ├── hud_screen.cpp
    │   │   ├── connect_screen.h
    │   │   ├── connect_screen.cpp
    │   │   ├── alert_screen.h
    │   │   ├── alert_screen.cpp
    │   │   ├── fonts/           ← Vietnamese UTF-8 LVGL fonts
    │   │   └── turn_icons.h     ← const bitmap arrays, 20 turn types
    │   └── utils/
    │       ├── nvs_config.h
    │       ├── nvs_config.cpp
    │       └── logger.h
    └── lib/                     ← managed by PlatformIO
```

**Rules:**
- Do **not** create files outside this layout without updating GEMINI.md first.
- Do **not** merge separate concerns into one file.
- All GPIO numbers must be defined as `constexpr int` in `config.h`.

---

## 4. HLP/1 Protocol Rules

### 4.1 Frame format
- Each frame is one UTF-8 JSON object terminated by `\n` (0x0A).
- Maximum frame size: **512 bytes** including the trailing `\n`.
- Strip trailing `\r` before null-terminating the buffer.
- On overflow or malformed UTF-8: **drop the frame, resync at the next `\n`**.

### 4.2 Handshake sequence
```
ESP32 (us)          Android (Waze Mod)
   |                      |
   | <── connectGatt/SPP ─|
   |─── dev ─────────────>|   send within 500 ms of link-up
   |<── hi ───────────────|   contains negotiated rate + fields
   |<── s  (stream) ──────|
   |<── ping (5 s) ───────|
   |─── pong ────────────>|   send BEFORE any render/NVS operation
```
- **Send `dev` immediately** after SPP link-up or BLE CCCD subscribe.
- Respond to `ping` with `pong` before any other slow work.
- Accept `hi` as idempotent — it may arrive more than once per session.
- When `sess` in `hi` changes, treat it as a **new session**: clear cached state.

### 4.3 `dev` declaration we send
```json
{
  "v": 1,
  "t": "dev",
  "name": "WazeHUD-CYD",
  "fw": "<FIRMWARE_VERSION from config.h>",
  "proto": [1],
  "disp": { "w": 240, "h": 320, "color": 1 },
  "can": ["speed","limit","turn","lanes","street","eta","avgzone","alerts","device_config"],
  "want": {
    "rate": 8,
    "fields": ["nav","spd","lim","over","trn","trn2","dst","exit","lan",
               "st","st2","eta","rmin","rm","rkm",
               "avg","avgL","avgR","avgP",
               "alr","alrD","alrV","alrs","ts"]
  }
}
```
- `rate: 8` for SPP, `rate: 4` for BLE/WiFi.
- Always include `alrs` in `want.fields`.

### 4.4 Turn enum (trn / trn2)
| Code | Meaning |
|---:|---|
| 0 | NONE |
| 1 | CONTINUE (straight) |
| 2 | LEFT |
| 3 | RIGHT |
| 4 | SLIGHT_LEFT |
| 5 | SLIGHT_RIGHT |
| 6 | SHARP_LEFT |
| 7 | SHARP_RIGHT |
| 8 | U_TURN |
| 10 | ROUNDABOUT (use `exit`) |
| 11 | ROUNDABOUT_LEFT |
| 12 | ROUNDABOUT_RIGHT |
| 13 | KEEP_LEFT |
| 14 | KEEP_RIGHT |
| 15 | EXIT_LEFT |
| 16 | EXIT_RIGHT |
| 17 | ARRIVE |
| 19 | ROUNDABOUT_STRAIGHT |
| 20 | ROUNDABOUT_U_TURN |
- Unknown codes → render nothing (treat as 0).

### 4.5 Alert enum (alr)
- Codes 0–9: stable legacy. Codes 10–74: additive extensions.
- Unknown codes must render the generic hazard icon (code 4).
- `alrV` present only for `SPEED_DROP (8)` and `END_SPEED_RESTRICTION (21)`.
- `avg=1` in Vietnam = **no-passing zone** — render no-passing sign + `avgL` countdown. **NEVER render a speed camera for `avg`.**

### 4.6 Stale detection
- Consider link stale after **3000 ms** without any message.
- Show "Mất tín hiệu" screen when stale.
- Each message `s` is a **full snapshot** (not delta).

---

## 5. Transport Layer Rules

### 5.1 Bluetooth Classic SPP
- Device name: **`"WazeHUD"`** (must match exactly).
- Use `BluetoothSerial` library from Arduino ESP32 core.
- Feed received bytes into queue from SPP callback — **exit the callback immediately**.
- **Must not** call LVGL or Preferences from the SPP callback.

### 5.2 WiFi WebSocket
- ESP32 is the **WebSocket server**; Waze Mod is the client.
- Listen on port **8765**, path **`/hlp`**, subprotocol `hlp.v1`.
- Each WebSocket text message = one HLP JSON frame.
- Accept **only LAN/private IP** connections.
- Do not enable if WiFi SSID is not configured.

### 5.3 Transport selection
- Run **both** SPP and WiFi transports concurrently.
- Only **one** transport drives the HLP session at a time.
- If both connect simultaneously, **prefer SPP**.

---

## 6. Display Layer Rules

### 6.1 LVGL version
- **LVGL v8.3.x** — do not upgrade to v9 without updating this file.
- LVGL mutex: always take `g_lvgl_mutex` before calling any `lv_*` API from outside `render_task`.

### 6.2 Display driver
- Library: **TFT_eSPI** configured for ILI9341.
- All SPI pins defined in `config.h` and mirrored in `User_Setup.h`.
- Backlight: GPIO 21, PWM channel 0, 5000 Hz, 8-bit.
- Touch: XPT2046, CS on GPIO 33.

### 6.3 Screen states
```
BOOT → CONNECTING → HUD_ACTIVE
                 ↘ NO_SIGNAL (stale > 3 s) → HUD_ACTIVE (on reconnect)
```

### 6.4 Font rules — Vietnamese support
- **MUST support Vietnamese UTF-8** (Latin Extended Additional, U+1E00–U+1EFF).
- Generate LVGL font with codepoints:
  - U+0020–U+007E (basic Latin)
  - U+00C0–U+024F (Latin Extended A & B)
  - U+1E00–U+1EFF (Latin Extended Additional — Vietnamese diacritics)
- Font sizes: 48px (speed), 24px (street name), 16px (ETA/distance), 14px (labels).
- Font files go in `firmware/src/display/fonts/`.
- Never render raw bytes as font — always pass UTF-8 strings through LVGL labels.

### 6.5 Layout constraints (240 × 320, portrait)
```
y=   0..31   : Alert bar (32 px)
y=  32..223  : Main — speed left (120px) + turn right (120px)
y= 224..251  : Current street name (28 px)
y= 252..279  : ETA + remaining distance (28 px)
y= 280..319  : Avg-zone bar (40 px, hidden when avg=0)
```

---

## 7. HLP State Data Model

`HudState` struct in `hlp_state.h` is the **single source of truth**.

```cpp
struct HudState {
    bool  nav;
    int   spd, lim;
    bool  over;
    int   trn, trn2, dst, exit_num;
    char  st[41], st2[41];   // UTF-8, Vietnamese
    char  eta[6];             // "HH:MM"
    int   rmin, rm;
    float rkm;
    bool  avg;
    int   avgL, avgR, avgP;
    int   alr, alrD, alrV;
    struct AlertEntry { int k; int d; int v; } alrs[4];
    int   alrs_count;
    uint32_t ts, sess;
    uint32_t last_rx_ms;
};
```

- Written **only** by `protocol_task`.
- Read by `render_task` via a **copy** protected by `g_state_mutex`.
- Never store pointers into `HudState` from the display layer.

---

## 8. FreeRTOS Task Rules

| Task name | Core | Priority | Stack |
|---|---|---|---|
| `spp_rx` | 0 | 6 | 4096 |
| `wifi_rx` | 0 | 6 | 4096 |
| `protocol` | 0 | 5 | 4096 |
| `render` | 1 | 4 | 8192 |

- **Core 1 is reserved for `render_task`**.
- Never call `delay()` or `vTaskDelay()` in a callback.

---

## 9. Dynamic Device Config Fields

| ID | Kind | Label (VI) | Range | Default |
|---|---|---|---|---|
| `brightness` | slider | Độ sáng | 10–100, step 5 | 70 |
| `show_eta` | toggle | Hiện giờ đến | — | true |
| `show_alerts` | toggle | Hiện cảnh báo | — | true |
| `theme` | selection | Giao diện | auto/day/night | auto |
| `wifi_enabled` | toggle | WiFi HUD | — | false |
| `label` | text | Tên hiển thị | maxLength 20 | "WazeHUD" |

- NVS namespace: `"wazehud_cfg"` (Arduino Preferences).
- Follow atomic staging pattern from waze-hud-link-sdk-ai-bundle.md §2.6.
- Apply `brightness` to backlight PWM immediately after `cfg_ack`.
- Apply `theme` to LVGL palette immediately after `cfg_ack`.
- `wifi_enabled` requires restart.

---

## 10. Coding Standards

- C++17. Use `constexpr`, `enum class`, `nullptr`.
- No exceptions, no dynamic allocation after `setup()`.
- Max line length: 100 chars. Encoding: UTF-8 without BOM.
- Use `char[]` arrays and `strlcpy/snprintf` — **never Arduino `String` class**.
- `HLP_MAX_FRAME = 512` bytes.

### Naming
| Entity | Convention | Example |
|---|---|---|
| Global variable | `g_` prefix, snake_case | `g_hud_state` |
| Constant | UPPER_SNAKE | `MAX_FRAME_SIZE` |
| Class | PascalCase | `SppTransport` |
| Method | camelCase | `sendDev()` |
| File | snake_case | `spp_transport.cpp` |

---

## 11. What Gemini Must NOT Do

- ❌ Hard-code GPIO pin numbers outside `config.h`.
- ❌ Call LVGL APIs from SPP callbacks or ISRs.
- ❌ Use Arduino `String` class — use `char[]` and `strlcpy`.
- ❌ Use `malloc/free` after `setup()` completes.
- ❌ Render a speed camera icon for `avg=1` (Vietnam no-passing zone).
- ❌ Ignore unknown `trn`/`alr` enum values — must fallback gracefully.
- ❌ Upgrade LVGL to v9 without explicit user approval.
- ❌ Add config field without updating `CFG_COUNT` and §9 table.
- ❌ Log raw HLP payload (contains navigation/personal data).
- ❌ Create files outside the directory layout in §3.
- ❌ Import display layer from transport layer or vice versa.

---

## 12. Quick Reference — BLE UUIDs

| UUID | Role |
|---|---|
| `8a7e0001-4d6e-4c48-9a9d-484c504c0001` | HLP Service |
| `8a7e0002-4d6e-4c48-9a9d-484c504c0001` | TX (Android→device), write with response |
| `8a7e0003-4d6e-4c48-9a9d-484c504c0001` | RX (device→Android), notify + CCCD |
| `8a7e0004-4d6e-4c48-9a9d-484c504c0001` | Capabilities (optional) |

*(SPP uses SDP UUID `0x1101` automatically.)*

---

## 13. Changelog

| Date | Change |
|---|---|
| 2026-09-11 | Initial version — created from SDK analysis |

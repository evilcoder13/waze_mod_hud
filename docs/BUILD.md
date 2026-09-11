# BUILD.md — WazeHUD Build & Flash Guide

> Prerequisites: Windows 10/11, VS Code + PlatformIO extension, Python 3.x (for esptool).

---

## 1. Toolchain Setup

### Install VS Code + PlatformIO
```
1. Download VS Code: https://code.visualstudio.com/
2. Install PlatformIO IDE extension from VS Code Marketplace.
3. Restart VS Code — PlatformIO will install its core tools automatically.
```

### Install CP2102 / CH340 Drivers
- Download CH340 driver: http://www.wch-ic.com/downloads/CH341SER_EXE.html
- Install and restart Windows if prompted.
- Verify in Device Manager: Ports (COM & LPT) → USB-SERIAL CH340 (COMx)

---

## 2. Clone / Open Project

```powershell
# Open terminal in VS Code
cd d:\Projects\WazeHUD\firmware
# Open in PlatformIO
code .
```

PlatformIO will detect `platformio.ini` and download all dependencies automatically on first build.

---

## 3. platformio.ini Reference

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
board_build.partitions = min_spiffs.csv

lib_deps =
  lvgl/lvgl @ ^8.3.11
  bblanchon/ArduinoJson @ ^7.0.0
  bodmer/TFT_eSPI @ ^2.5.43
  links2004/WebSockets @ ^2.4.1

build_flags =
  -DCORE_DEBUG_LEVEL=3
  -DLV_CONF_INCLUDE_SIMPLE
  -DBOARD_HAS_PSRAM
  -std=c++17
  -I src

[env:esp32dev-release]
extends = env:esp32dev
build_flags =
  -DCORE_DEBUG_LEVEL=0
  -DLV_CONF_INCLUDE_SIMPLE
  -DWAZEHUD_LOG_LEVEL=0
  -Os
```

---

## 4. LVGL Configuration

Create `firmware/src/lv_conf.h` with these critical settings:

```c
// Key LVGL v8 settings for ESP32-2432S028
#define LV_MEM_SIZE (32 * 1024U)         // 32 KB heap
#define LV_COLOR_DEPTH 16                 // RGB565
#define LV_COLOR_16_SWAP 1                // TFT_eSPI byte order
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_FONT_MONTSERRAT_16 1           // fallback font
#define LV_FONT_DEFAULT &lv_font_montserrat_16
#define LV_USE_LABEL 1
#define LV_USE_IMG   1
#define LV_USE_BAR   1
#define LV_USE_ARC   1
```

---

## 5. TFT_eSPI Configuration

Create `firmware/src/User_Setup.h` (TFT_eSPI reads this automatically):

```c
// User_Setup.h for ESP32-2432S028
#define ILI9341_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

#define TFT_MOSI 13
#define TFT_MISO 12
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  -1   // tied to EN

#define TOUCH_CS 33

#define SPI_FREQUENCY       40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_GFXFF
#define SMOOTH_FONT
```

---

## 6. Build

### Via VS Code PlatformIO toolbar:
- Click **Build** (checkmark icon) in the bottom status bar.
- Or: `Ctrl+Alt+B`

### Via terminal:
```powershell
cd d:\Projects\WazeHUD\firmware
platformio run
```

Expected output:
```
Building .pio\build\esp32dev\firmware.elf
Linking .pio\build\esp32dev\firmware.elf
Checking size .pio\build\esp32dev\firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [==        ]  18.5% (used 96132 bytes from 520192 bytes)
Flash: [====      ]  40.2% (used 1327600 bytes from 3309568 bytes)
```

---

## 7. Flash

```powershell
# Replace COMx with your actual port (check Device Manager)
platformio run --target upload --upload-port COMx
```

Or via PlatformIO toolbar: **Upload** (right-arrow icon).

### Finding the COM port:
```powershell
Get-WmiObject Win32_SerialPort | Select-Object Name, DeviceID
```

---

## 8. Serial Monitor

```powershell
platformio device monitor --port COMx --baud 115200
```

Or: `Ctrl+Alt+S` in VS Code.

Expected boot output:
```
[I] WazeHUD v1.0.0 starting...
[I] Display initialized: 240x320 ILI9341
[I] LVGL initialized
[I] Bluetooth SPP started as WazeHUD
[I] WiFi: disabled (set wifi_enabled=true to enable)
[I] Tasks created. Waiting for connection...
```

---

## 9. Pairing with Android

1. Flash firmware and confirm `WazeHUD` appears in Serial monitor.
2. On Android: **Settings → Bluetooth → Pair new device**.
3. Select **WazeHUD** from the list. Accept pairing PIN if prompted (default: `1234`).
4. Open **Waze → Mod Settings → Device → HUD Link**.
5. Select transport: **Bluetooth Classic**.
6. Select device: **WazeHUD**.
7. Enable **HUD Link** toggle.
8. Watch Serial monitor for:
   ```
   [I] SPP connected
   [I] Sending dev declaration
   [I] HLP hi received, rate=8 Hz
   [I] HLP stream active
   ```

---

## 10. WiFi Setup (Optional)

1. Enable `wifi_enabled` via Waze Mod config panel (after initial BT connection).
2. Restart ESP32 (board reset button).
3. Serial monitor shows:
   ```
   [I] WiFi connecting to <SSID>...
   [I] WiFi connected. IP: 192.168.1.xxx
   [I] WebSocket server started on ws://192.168.1.xxx:8765/hlp
   ```
4. In Waze Mod: **HUD Link → Transport: WiFi WebSocket → IP: 192.168.1.xxx**.

Note: WiFi SSID and password are configured via the dynamic config system.

---

## 11. Vietnamese Font Generation

LVGL fonts must be generated with Vietnamese codepoints:

```bash
# Install lv_font_conv (Node.js required)
npm install -g lv_font_conv

# Generate Roboto 24px with Vietnamese support
lv_font_conv --font Roboto-Regular.ttf \
  --size 24 \
  --format lvgl \
  --bpp 4 \
  --range 0x0020-0x007E \
  --range 0x00C0-0x024F \
  --range 0x1E00-0x1EFF \
  -o src/display/fonts/roboto_24_vi.c

# Generate sizes: 14, 16, 24, 48px
# Place all .c files in firmware/src/display/fonts/
```

---

## 12. Troubleshooting

| Symptom | Solution |
|---------|----------|
| COM port not found | Install CH340 driver; check USB cable (data, not charge-only) |
| Build error: LVGL not found | Run `pio pkg install` in firmware/ folder |
| Screen shows garbage | Verify TFT_CS=15, TFT_DC=2 in User_Setup.h |
| BT not visible on Android | Confirm `BluetoothSerial.begin("WazeHUD")` in Serial monitor |
| HLP stream not starting | Pair BT first; check Waze Mod transport = Classic SPP |
| Vietnamese chars show boxes | Regenerate font with Vietnamese codepoints; check lv_conf.h |
| WiFi WebSocket not connecting | Ensure phone and ESP32 on same LAN/hotspot |

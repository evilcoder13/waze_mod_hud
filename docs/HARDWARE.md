# HARDWARE.md — ESP32-2432S028 (CYD) Pinout

> Reference document for `config.h`. **Never hard-code GPIO numbers in source files.**
> All constants here must be mirrored in `firmware/src/config.h`.

---

## Board Overview

ESP32-2432S028 is a development board featuring:
- ESP32 dual-core SoC (240 MHz, 520 KB SRAM, 4 MB Flash)
- 2.8" ILI9341 TFT LCD, 240×320 portrait, SPI
- XPT2046 resistive touchscreen, shares SPI bus
- Onboard RGB LED (active LOW)
- Micro SD card slot
- CH340C USB-UART bridge
- USB-C or Micro-USB power/programming

---

## SPI Bus — Display (ILI9341)

| Signal | GPIO | Notes |
|--------|------|-------|
| MOSI | 13 | Shared with touch |
| MISO | 12 | Shared with touch |
| SCK | 14 | Shared with touch |
| CS (display) | 15 | Active LOW |
| DC / RS | 2 | Data/Command select |
| RST | — | Tied to EN (hardware reset) |
| Backlight | **21** | PWM via transistor, active HIGH |

**TFT_eSPI `User_Setup.h` settings:**
```cpp
#define ILI9341_DRIVER
#define TFT_MOSI  13
#define TFT_MISO  12
#define TFT_SCLK  14
#define TFT_CS    15
#define TFT_DC     2
#define TFT_RST   -1
#define SPI_FREQUENCY  40000000
```

---

## SPI Bus — Touch (XPT2046)

| Signal | GPIO | Notes |
|--------|------|-------|
| MOSI | 13 | Shared with display |
| MISO | 12 | Shared with touch |
| SCK | 14 | Shared with display |
| CS (touch) | **33** | Active LOW |
| IRQ | **36** | Touch interrupt (INPUT_PULLUP) |

---

## RGB LED (Onboard, Active LOW)

| Color | GPIO | Logic |
|-------|------|-------|
| Red | **4** | LOW = ON |
| Green | **16** | LOW = ON |
| Blue | **17** | LOW = ON |

Status meanings in WazeHUD:
- 🔵 Blue: Connecting to Waze Mod
- 🟢 Green: Connected, HLP stream active
- 🔴 Red: Error / signal lost
- Off: Boot / idle

---

## SD Card (SPI)

| Signal | GPIO |
|--------|------|
| MOSI | 13 |
| MISO | 12 |
| SCK | 14 |
| CS | **5** |

> Not used in v1.0. GPIO 5 reserved — do not repurpose.

---

## P3 Expansion Connector

| Pin | GPIO | Default use |
|-----|------|-------------|
| 1 | GND | Ground |
| 2 | VCC (3.3 V) | Power |
| 3 | **IO22** | I2C SDA / spare GPIO |
| 4 | **IO35** | ADC input / spare GPIO |

---

## UART / USB

| Signal | GPIO | Notes |
|--------|------|-------|
| TX0 | 1 | CH340C → USB Serial (debug) |
| RX0 | 3 | CH340C → USB Serial |

- Serial speed: **115200 baud**
- Programming: CH340C auto-reset via EN + GPIO0

---

## Power

| Source | Voltage | Max current |
|--------|---------|-------------|
| USB | 5 V | 500 mA (USB 2.0) |
| 3.3 V rail | 3.3 V | ~300 mA |
| Backlight LED | ~3.3 V | ~80 mA (full brightness) |

> Use a powered USB hub or car USB charger (>= 1 A) for reliable operation.

---

## Bluetooth

- **Classic BT (BR/EDR)**: SPP, device name `"WazeHUD"`
- **BLE**: Available, secondary transport
- Antenna: PCB trace — keep clear of metal enclosures

---

## config.h Pin Reference

```cpp
// All GPIO definitions — firmware/src/config.h

// SPI shared bus
constexpr int PIN_SPI_MOSI = 13;
constexpr int PIN_SPI_MISO = 12;
constexpr int PIN_SPI_SCK  = 14;

// ILI9341 display
constexpr int PIN_TFT_CS   = 15;
constexpr int PIN_TFT_DC   =  2;
constexpr int PIN_TFT_BL   = 21;   // backlight PWM

// XPT2046 touch
constexpr int PIN_TOUCH_CS  = 33;
constexpr int PIN_TOUCH_IRQ = 36;

// RGB LED (active LOW)
constexpr int PIN_LED_RED   =  4;
constexpr int PIN_LED_GREEN = 16;
constexpr int PIN_LED_BLUE  = 17;

// SD card (reserved)
constexpr int PIN_SD_CS     =  5;

// UART debug
constexpr int SERIAL_BAUD   = 115200;
```

---

## Board Variants

| Variant | Backlight GPIO | Touch CS |
|---------|---------------|----------|
| 2432S028 (common) | 21 | 33 |
| 2432S028R (red PCB) | 21 | 33 |
| 2424S012 (2.4 inch) | 27 | 33 |

> This firmware targets **2432S028** only. Update `config.h` and `User_Setup.h` for other variants.

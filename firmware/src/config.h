#pragma once

#include <cstdint>
#include <cstddef>
#include <freertos/FreeRTOS.h>

// ==========================================
// Firmware Metadata
// ==========================================
constexpr const char* FIRMWARE_VERSION = "1.3.1";
constexpr const char* DEVICE_NAME      = "WazeHUD";
constexpr const char* HLP_DEVICE_MODEL = "WazeHUD-CYD";

// ==========================================
// Hardware Pin Definitions (ESP32-2432S028 CYD)
// ==========================================
// Display ILI9341 SPI (Shared SPI bus)
constexpr int PIN_SPI_MOSI = 13;
constexpr int PIN_SPI_MISO = 12;
constexpr int PIN_SPI_SCK  = 14;
constexpr int PIN_TFT_CS   = 15;
constexpr int PIN_TFT_DC   = 2;
constexpr int PIN_TFT_RST  = -1; // Connected to EN
constexpr int PIN_TFT_BL   = 21; // Backlight PWM (active HIGH)

// Touch XPT2046 SPI
constexpr int PIN_TOUCH_CS  = 33;
constexpr int PIN_TOUCH_IRQ = 36;

// Physical Button (BOOT button on ESP32 CYD - GPIO 0)
constexpr int PIN_BUTTON_BOOT = 0;

// RGB LED Onboard (Active LOW)
constexpr int PIN_LED_RED   = 4;
constexpr int PIN_LED_GREEN = 16;
constexpr int PIN_LED_BLUE  = 17;

// SD Card (Reserved)
constexpr int PIN_SD_CS     = 5;

// UART Debug
constexpr int SERIAL_BAUD   = 115200;

// ==========================================
// Display Specifications
// ==========================================
constexpr int SCREEN_WIDTH      = 320;
constexpr int SCREEN_HEIGHT     = 240;
constexpr int DISPLAY_ROTATION  = 1; // 1 = Landscape (USB on bottom/left)

// Backlight PWM configuration
constexpr int BL_PWM_CHANNEL   = 0;
constexpr int BL_PWM_FREQ      = 5000;
constexpr int BL_PWM_RES_BITS  = 8;

// ==========================================
// HLP/1 Protocol Constants
// ==========================================
constexpr size_t HLP_MAX_FRAME = 512;
constexpr uint32_t HLP_STALE_TIMEOUT_MS = 3000;
constexpr uint8_t HLP_WANT_RATE_SPP = 8;
constexpr uint8_t HLP_WANT_RATE_WIFI = 4;

// ==========================================
// WiFi & WebSocket Configuration
// ==========================================
constexpr uint16_t WS_SERVER_PORT = 8765;
constexpr const char* WS_PATH     = "/hlp";
constexpr const char* WS_SUBPROTO = "hlp.v1";

// ==========================================
// FreeRTOS Task Configurations
// ==========================================
constexpr uint32_t STACK_SPP_RX    = 4096;
constexpr uint32_t STACK_WIFI_RX   = 4096;
constexpr uint32_t STACK_PROTOCOL  = 4096;
constexpr uint32_t STACK_RENDER    = 8192;

constexpr UBaseType_t PRIO_SPP_RX   = 6;
constexpr UBaseType_t PRIO_WIFI_RX  = 6;
constexpr UBaseType_t PRIO_PROTOCOL = 5;
constexpr UBaseType_t PRIO_RENDER   = 4;

constexpr BaseType_t CORE_TRANSPORT = 0;
constexpr BaseType_t CORE_PROTOCOL  = 0;
constexpr BaseType_t CORE_RENDER    = 1;

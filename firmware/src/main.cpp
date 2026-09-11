#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include "config.h"
#include "utils/logger.h"
#include "utils/nvs_config.h"
#include "hlp/hlp_core.h"
#include "hlp/hlp_messages.h"
#include "hlp/hlp_state.h"
#include "transport/spp_transport.h"
#include "transport/wifi_transport.h"
#include "display/display_driver.h"
#include "display/hud_screen.h"

// ==========================================
// Globals & Handles
// ==========================================
static QueueHandle_t s_hlp_queue = nullptr;
static SemaphoreHandle_t s_state_mutex = nullptr;
static SemaphoreHandle_t s_lvgl_mutex = nullptr;

static HudState s_active_hud_state;
static HlpRx s_hlp_rx;
static uint8_t s_active_transport = 0; // 0 = SPP, 1 = WiFi

static void sendLineToActiveTransport(const char* line, void* user) {
    if (!line) return;
    if (s_active_transport == 0) {
        SppTransport::sendLine(line);
    } else {
        WifiTransport::sendLine(line);
    }
}

static void onHlpStateUpdate(const HudState& state, void* user) {
    if (s_state_mutex && xSemaphoreTake(s_state_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        s_active_hud_state = state;
        xSemaphoreGive(s_state_mutex);
    }
}

static void onHlpLineParsed(const char* line, size_t length, void* user) {
    hlp_handle_line(line, length, sendLineToActiveTransport, onHlpStateUpdate, user);
}

// Protocol Task: handles framing, handshake, stale detection, UI dispatch
static void protocolTask(void* param) {
    LOG_I("protocolTask started on Core %d", xPortGetCoreID());

    hlp_rx_init(&s_hlp_rx, onHlpLineParsed, nullptr);

    bool was_spp_connected = false;
    bool was_wifi_connected = false;
    ScreenState current_screen = ScreenState::CONNECTING;

    if (s_lvgl_mutex && xSemaphoreTake(s_lvgl_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        HudScreen::setScreenState(ScreenState::CONNECTING);
        xSemaphoreGive(s_lvgl_mutex);
    }

    TransportChunk chunk;

    while (true) {
        bool spp_up = SppTransport::isConnected();
        bool wifi_up = WifiTransport::isConnected();

        // 1. Detect Connection Changes
        if (spp_up && !was_spp_connected) {
            LOG_I("SPP connected, initiating HLP handshake");
            s_active_transport = 0;
            vTaskDelay(pdMS_TO_TICKS(100));
            hlp_send_dev(sendLineToActiveTransport, nullptr, "spp", HLP_WANT_RATE_SPP);
        } else if (!spp_up && was_spp_connected) {
            LOG_W("SPP disconnected");
        }
        was_spp_connected = spp_up;

        if (wifi_up && !was_wifi_connected) {
            if (!spp_up) {
                LOG_I("WiFi WS connected and SPP inactive, switching to WiFi transport");
                s_active_transport = 1;
                hlp_send_dev(sendLineToActiveTransport, nullptr, "wifi", HLP_WANT_RATE_WIFI);
            }
        }
        was_wifi_connected = wifi_up;

        // 2. Consume incoming bytes from transports
        if (xQueueReceive(s_hlp_queue, &chunk, pdMS_TO_TICKS(20)) == pdTRUE) {
            // Prioritize SPP over WiFi
            if (spp_up && chunk.transport_id == 1) {
                // Drop WiFi packet if SPP is active
            } else {
                s_active_transport = chunk.transport_id;
                hlp_rx_feed(&s_hlp_rx, chunk.bytes, chunk.length);
            }
        }

        // 3. Stale & State Machine Handling
        uint32_t now = millis();
        bool link_up = spp_up || wifi_up;

        if (!link_up) {
            if (current_screen != ScreenState::CONNECTING) {
                current_screen = ScreenState::CONNECTING;
                if (s_lvgl_mutex && xSemaphoreTake(s_lvgl_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
                    HudScreen::setScreenState(ScreenState::CONNECTING);
                    xSemaphoreGive(s_lvgl_mutex);
                }
            }
        } else {
            // Connected: check stale condition (3000ms timeout)
            if (s_active_hud_state.last_rx_ms > 0 && (now - s_active_hud_state.last_rx_ms > HLP_STALE_TIMEOUT_MS)) {
                if (current_screen != ScreenState::NO_SIGNAL) {
                    current_screen = ScreenState::NO_SIGNAL;
                    if (s_lvgl_mutex && xSemaphoreTake(s_lvgl_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
                        HudScreen::setScreenState(ScreenState::NO_SIGNAL);
                        xSemaphoreGive(s_lvgl_mutex);
                    }
                }
            } else if (s_active_hud_state.last_rx_ms > 0) {
                if (current_screen != ScreenState::HUD_ACTIVE) {
                    current_screen = ScreenState::HUD_ACTIVE;
                    if (s_lvgl_mutex && xSemaphoreTake(s_lvgl_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
                        HudScreen::setScreenState(ScreenState::HUD_ACTIVE);
                        xSemaphoreGive(s_lvgl_mutex);
                    }
                }

                // Update UI data
                if (s_lvgl_mutex && xSemaphoreTake(s_lvgl_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
                    HudState state_copy;
                    if (s_state_mutex && xSemaphoreTake(s_state_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                        state_copy = s_active_hud_state;
                        xSemaphoreGive(s_state_mutex);
                    }
                    HudScreen::updateData(state_copy);
                    xSemaphoreGive(s_lvgl_mutex);
                }
            }
        }
    }
}

// ==========================================
// Arduino Setup & Loop
// ==========================================
void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(200);

    LOG_I("=================================");
    LOG_I(" WazeHUD CYD v%s Starting... ", FIRMWARE_VERSION);
    LOG_I("=================================");

    // 1. Load NVS Settings
    NvsConfig::init();

    // 2. Initialize Queues and Synchronization Mutexes
    s_hlp_queue   = xQueueCreate(16, sizeof(TransportChunk));
    s_state_mutex = xSemaphoreCreateMutex();
    s_lvgl_mutex  = xSemaphoreCreateMutex();

    // 3. Initialize Display & LVGL
    DisplayDriver::init(s_lvgl_mutex);

    // 4. Start Transports
    SppTransport::init(s_hlp_queue, DEVICE_NAME);
    WifiTransport::init(s_hlp_queue);

    // 5. Start Core 1 Render Task & Core 0 Protocol Task
    DisplayDriver::startRenderTask();

    xTaskCreatePinnedToCore(
        protocolTask,
        "protocol",
        STACK_PROTOCOL,
        nullptr,
        PRIO_PROTOCOL,
        nullptr,
        CORE_PROTOCOL
    );

    LOG_I("System initialization complete, running FreeRTOS tasks");
}

void loop() {
    // Everything handled by FreeRTOS tasks
    vTaskDelay(pdMS_TO_TICKS(1000));
}

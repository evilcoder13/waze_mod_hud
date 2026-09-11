#include "wifi_transport.h"
#include <WiFi.h>
#include <WebSocketsServer.h>
#include "config.h"
#include "utils/nvs_config.h"
#include "utils/logger.h"

static WebSocketsServer s_ws_server(WS_SERVER_PORT);
static QueueHandle_t s_rx_queue = nullptr;
static volatile bool s_ws_client_connected = false;
static uint8_t s_active_client_num = 0;

static void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            if (s_ws_client_connected && num == s_active_client_num) {
                s_ws_client_connected = false;
                LOG_W("WebSocket client #%u disconnected", num);
            }
            break;
        case WStype_CONNECTED:
            s_ws_client_connected = true;
            s_active_client_num = num;
            LOG_I("WebSocket client #%u connected from %s", num, s_ws_server.remoteIP(num).toString().c_str());
            break;
        case WStype_TEXT:
            if (length > 0 && s_rx_queue) {
                TransportChunk chunk;
                chunk.transport_id = 1; // WiFi
                size_t to_copy = (length < HLP_MAX_FRAME - 1) ? length : (HLP_MAX_FRAME - 2);
                memcpy(chunk.bytes, payload, to_copy);
                chunk.bytes[to_copy] = '\n'; // Ensure newline delimiter
                chunk.length = to_copy + 1;
                xQueueSend(s_rx_queue, &chunk, 0);
            }
            break;
        default:
            break;
    }
}

static void wifiRxTask(void* param) {
    LOG_I("wifiRxTask started on Core %d", xPortGetCoreID());
    while (true) {
        s_ws_server.loop();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

bool WifiTransport::init(QueueHandle_t rx_queue) {
    s_rx_queue = rx_queue;
    const NvsSettings& cfg = NvsConfig::get();
    if (!cfg.wifi_enabled || strlen(cfg.wifi_ssid) == 0) {
        LOG_I("WiFi is disabled in NVS settings");
        return false;
    }

    LOG_I("Connecting to WiFi SSID: %s", cfg.wifi_ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(cfg.wifi_ssid, cfg.wifi_pass);

    // Wait up to 8 seconds
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 16) {
        vTaskDelay(pdMS_TO_TICKS(500));
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        LOG_I("WiFi connected! IP address: %s", WiFi.localIP().toString().c_str());
        s_ws_server.begin();
        s_ws_server.onEvent(webSocketEvent);

        BaseType_t res = xTaskCreatePinnedToCore(
            wifiRxTask,
            "wifi_rx",
            STACK_WIFI_RX,
            nullptr,
            PRIO_WIFI_RX,
            nullptr,
            CORE_TRANSPORT
        );
        return (res == pdPASS);
    } else {
        LOG_W("WiFi connection failed or timed out");
        return false;
    }
}

void WifiTransport::sendLine(const char* line) {
    if (!line || !s_ws_client_connected) return;
    s_ws_server.sendTXT(s_active_client_num, line);
}

bool WifiTransport::isConnected() {
    return s_ws_client_connected;
}

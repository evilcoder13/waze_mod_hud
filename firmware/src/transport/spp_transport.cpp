#include "spp_transport.h"
#include <BluetoothSerial.h>
#include <cstring>
#include "utils/logger.h"

static BluetoothSerial s_bt_serial;
static QueueHandle_t s_rx_queue = nullptr;
static volatile bool s_spp_connected = false;

static void btCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch (event) {
        case ESP_SPP_SRV_OPEN_EVT:
            s_spp_connected = true;
            LOG_I("Bluetooth SPP client connected");
            break;
        case ESP_SPP_CLOSE_EVT:
            s_spp_connected = false;
            LOG_W("Bluetooth SPP client disconnected");
            break;
        default:
            break;
    }
}

void SppTransport::sppRxTask(void* param) {
    LOG_I("sppRxTask started on Core %d", xPortGetCoreID());
    TransportChunk chunk;
    chunk.transport_id = 0; // SPP

    while (true) {
        if (s_bt_serial.available()) {
            size_t count = 0;
            while (s_bt_serial.available() && count < HLP_MAX_FRAME) {
                chunk.bytes[count++] = (uint8_t)s_bt_serial.read();
            }
            if (count > 0 && s_rx_queue) {
                chunk.length = count;
                if (xQueueSend(s_rx_queue, &chunk, pdMS_TO_TICKS(10)) != pdTRUE) {
                    LOG_W("sppRxTask: queue is full, dropped %u bytes", count);
                }
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

bool SppTransport::init(QueueHandle_t rx_queue, const char* device_name) {
    s_rx_queue = rx_queue;

    s_bt_serial.register_callback(btCallback);
    if (!s_bt_serial.begin(device_name)) {
        LOG_E("Failed to start BluetoothSerial as %s", device_name);
        return false;
    }

    LOG_I("BluetoothSerial started successfully as %s", device_name);

    BaseType_t res = xTaskCreatePinnedToCore(
        sppRxTask,
        "spp_rx",
        STACK_SPP_RX,
        nullptr,
        PRIO_SPP_RX,
        nullptr,
        CORE_TRANSPORT
    );

    return (res == pdPASS);
}

void SppTransport::sendLine(const char* line) {
    if (!line || !s_spp_connected) return;
    s_bt_serial.print(line);
    s_bt_serial.print("\n");
}

bool SppTransport::isConnected() {
    return s_spp_connected;
}

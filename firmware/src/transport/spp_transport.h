#pragma once

#include <cstdint>
#include <cstddef>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "config.h"

struct TransportChunk {
    uint8_t transport_id; // 0 = SPP, 1 = WiFi
    uint16_t length;
    uint8_t bytes[HLP_MAX_FRAME];
};

class SppTransport {
public:
    static bool init(QueueHandle_t rx_queue, const char* device_name = DEVICE_NAME);
    static void sendLine(const char* line);
    static bool isConnected();

private:
    static void sppRxTask(void* param);
};

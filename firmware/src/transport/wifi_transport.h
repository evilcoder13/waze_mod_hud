#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "transport/spp_transport.h"

class WifiTransport {
public:
    static bool init(QueueHandle_t rx_queue);
    static void sendLine(const char* line);
    static bool isConnected();
};

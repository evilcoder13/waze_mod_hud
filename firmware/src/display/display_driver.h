#pragma once

#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class DisplayDriver {
public:
    static bool init(SemaphoreHandle_t lvgl_mutex);
    static void setBrightness(uint8_t percent);
    static void toggleMirror();
    static bool isMirrored();
    static void startRenderTask();
};

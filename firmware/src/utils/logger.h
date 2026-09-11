#pragma once

#include <Arduino.h>

#ifndef WAZEHUD_LOG_LEVEL
    #define WAZEHUD_LOG_LEVEL 3 // 0: None, 1: Error, 2: Warn, 3: Info, 4: Debug
#endif

#define LOG_TAG "WazeHUD"

#if WAZEHUD_LOG_LEVEL >= 1
    #define LOG_E(format, ...) Serial.printf("[E][%s] " format "\n", LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_E(format, ...)
#endif

#if WAZEHUD_LOG_LEVEL >= 2
    #define LOG_W(format, ...) Serial.printf("[W][%s] " format "\n", LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_W(format, ...)
#endif

#if WAZEHUD_LOG_LEVEL >= 3
    #define LOG_I(format, ...) Serial.printf("[I][%s] " format "\n", LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_I(format, ...)
#endif

#if WAZEHUD_LOG_LEVEL >= 4
    #define LOG_D(format, ...) Serial.printf("[D][%s] " format "\n", LOG_TAG, ##__VA_ARGS__)
#else
    #define LOG_D(format, ...)
#endif

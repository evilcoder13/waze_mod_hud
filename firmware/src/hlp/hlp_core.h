#pragma once

#include <cstdint>
#include <cstddef>
#include <stdbool.h>
#include "config.h"

typedef void (*hlp_line_cb_t)(const char* line, size_t length, void* user);

struct HlpRx {
    uint8_t buffer[HLP_MAX_FRAME];
    size_t length = 0;
    bool overflow = false;
    uint32_t oversized = 0;
    uint32_t malformed_utf8 = 0;
    hlp_line_cb_t callback = nullptr;
    void* user = nullptr;
};

void hlp_rx_init(HlpRx* rx, hlp_line_cb_t callback, void* user);
void hlp_rx_feed(HlpRx* rx, const uint8_t* data, size_t length);
bool hlp_utf8_valid(const uint8_t* data, size_t length);

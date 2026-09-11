#include "hlp_core.h"
#include <cstring>

static void reset(HlpRx* rx) {
    rx->length = 0;
    rx->overflow = false;
}

bool hlp_utf8_valid(const uint8_t* data, size_t length) {
    size_t i = 0;
    while (i < length) {
        uint8_t c = data[i++];
        if (c < 0x80) continue;
        size_t need = c >= 0xF0 ? 3 : c >= 0xE0 ? 2 : c >= 0xC2 ? 1 : 0;
        if (!need || i + need > length) return false;
        uint32_t code = c & ((1u << (6 - need)) - 1u);
        for (size_t j = 0; j < need; ++j) {
            uint8_t part = data[i++];
            if ((part & 0xC0) != 0x80) return false;
            code = (code << 6) | (part & 0x3F);
        }
        if (code > 0x10FFFF || (code >= 0xD800 && code <= 0xDFFF)
                || (need == 1 && code < 0x80)
                || (need == 2 && code < 0x800)
                || (need == 3 && code < 0x10000)) return false;
    }
    return true;
}

void hlp_rx_init(HlpRx* rx, hlp_line_cb_t callback, void* user) {
    if (!rx) return;
    *rx = HlpRx();
    rx->callback = callback;
    rx->user = user;
}

void hlp_rx_feed(HlpRx* rx, const uint8_t* data, size_t length) {
    if (!rx || !data) return;
    for (size_t i = 0; i < length; ++i) {
        uint8_t c = data[i];
        if (c == '\n') {
            if (rx->overflow) {
                rx->oversized++;
            } else {
                size_t n = rx->length;
                if (n && rx->buffer[n - 1] == '\r') --n; // Strip \r
                if (!hlp_utf8_valid(rx->buffer, n)) {
                    rx->malformed_utf8++;
                } else if (rx->callback) {
                    rx->buffer[n] = 0;
                    rx->callback((const char*)rx->buffer, n, rx->user);
                }
            }
            reset(rx);
        } else if (!rx->overflow) {
            if (rx->length >= (HLP_MAX_FRAME - 1)) {
                rx->overflow = true;
            } else {
                rx->buffer[rx->length++] = c;
            }
        }
    }
}

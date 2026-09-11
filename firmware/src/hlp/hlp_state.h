#pragma once

#include <cstdint>
#include <cstddef>
#include "config.h"

struct AlertEntry {
    int k = 0; // Alert code
    int d = -1; // Distance (metres)
    int v = 0; // Value (km/h)
};

struct HudState {
    bool  nav = false;
    int   spd = 0;
    int   lim = 0;
    bool  over = false;
    int   trn = 0;
    int   trn2 = 0;
    int   dst = -1;
    int   exit_num = 0;

    char  st[41] = {0};   // Current street (UTF-8)
    char  st2[41] = {0};  // Next street (UTF-8)

    char  eta[6] = {0};   // "HH:MM"
    int   rmin = 0;
    int   rm = 0;
    float rkm = 0.0f;

    bool  avg = false;    // Vietnam: No-passing zone!
    int   avgL = 0;       // Remaining distance (metres)
    int   avgR = 0;       // Recommended speed (km/h)
    int   avgP = 0;       // Progress (0..100)

    int   alr = 0;        // Nearest alert code
    int   alrD = -1;      // Nearest alert distance
    int   alrV = 0;       // Nearest alert value

    AlertEntry alrs[4];
    int   alrs_count = 0;

    uint32_t ts = 0;
    uint32_t sess = 0;
    uint32_t last_rx_ms = 0;
};

class HlpState {
public:
    static void reset(HudState& state);
    static bool decodeState(const char* json_str, HudState& state);
};

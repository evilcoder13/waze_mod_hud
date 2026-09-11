#include "hlp_state.h"
#include <ArduinoJson.h>
#include <cstring>

void HlpState::reset(HudState& state) {
    uint32_t preserved_sess = state.sess;
    state = HudState();
    state.sess = preserved_sess;
}

bool HlpState::decodeState(const char* json_str, HudState& state) {
    if (!json_str) return false;

    // Buffer budget for parsing 512-byte JSON
    StaticJsonDocument<768> doc;
    DeserializationError error = deserializeJson(doc, json_str);
    if (error) {
        return false;
    }

    // Navigation & Basic Speed
    state.nav  = (doc["nav"] | 0) != 0;
    state.spd  = doc["spd"] | 0;
    state.lim  = doc["lim"] | 0;
    state.over = (doc["over"] | 0) != 0;

    // Turn & Route
    state.trn      = doc["trn"] | 0;
    state.trn2     = doc["trn2"] | 0;
    state.dst      = doc["dst"] | -1;
    state.exit_num = doc["exit"] | 0;

    // Street Names (UTF-8, <= 40 chars)
    if (doc.containsKey("st")) {
        const char* st = doc["st"] | "";
        strlcpy(state.st, st, sizeof(state.st));
    }
    if (doc.containsKey("st2")) {
        const char* st2 = doc["st2"] | "";
        strlcpy(state.st2, st2, sizeof(state.st2));
    }

    // ETA & Distance
    if (doc.containsKey("eta")) {
        const char* eta = doc["eta"] | "";
        strlcpy(state.eta, eta, sizeof(state.eta));
    }
    state.rmin = doc["rmin"] | 0;
    state.rm   = doc["rm"] | 0;
    state.rkm  = doc["rkm"] | 0.0f;

    // Average Zone / Vietnam No-passing
    state.avg  = (doc["avg"] | 0) != 0;
    state.avgL = doc["avgL"] | 0;
    state.avgR = doc["avgR"] | 0;
    state.avgP = doc["avgP"] | 0;

    // Alerts
    state.alr  = doc["alr"] | 0;
    state.alrD = doc["alrD"] | -1;
    state.alrV = doc["alrV"] | 0;

    // Alert Array (opt-in alrs)
    if (doc.containsKey("alrs")) {
        JsonArray arr = doc["alrs"].as<JsonArray>();
        state.alrs_count = 0;
        for (JsonObject entry : arr) {
            if (state.alrs_count >= 4) break;
            state.alrs[state.alrs_count].k = entry["k"] | 0;
            state.alrs[state.alrs_count].d = entry["d"] | -1;
            state.alrs[state.alrs_count].v = entry["v"] | 0;
            state.alrs_count++;
        }
    }

    // Monotonic timestamp
    state.ts = doc["ts"] | 0;
    return true;
}

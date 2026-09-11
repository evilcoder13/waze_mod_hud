#include "hlp_messages.h"
#include "hlp_device_config.h"
#include "config.h"
#include "utils/logger.h"
#include <ArduinoJson.h>
#include <cstdio>
#include <cstring>

static HudState s_current_state;

void hlp_send_dev(hlp_send_line_fn send, void* user, const char* transport, unsigned rate) {
    if (!send) return;
    char line[480];
    snprintf(line, sizeof(line),
             "{\"v\":1,\"t\":\"dev\",\"name\":\"%s\",\"fw\":\"%s\","
             "\"proto\":[1],\"disp\":{\"w\":%d,\"h\":%d,\"color\":1},"
             "\"can\":[\"speed\",\"limit\",\"turn\",\"lanes\",\"street\",\"eta\",\"avgzone\",\"alerts\",\"device_config\"],"
             "\"want\":{\"rate\":%u,\"fields\":[\"nav\",\"spd\",\"lim\",\"over\",\"trn\",\"trn2\",\"dst\",\"exit\",\"lan\","
             "\"st\",\"st2\",\"eta\",\"rmin\",\"rm\",\"rkm\",\"avg\",\"avgL\",\"avgR\",\"avgP\",\"alr\",\"alrD\",\"alrV\",\"alrs\",\"ts\"]},"
             "\"transport\":\"%s\"}",
             HLP_DEVICE_MODEL, FIRMWARE_VERSION, SCREEN_WIDTH, SCREEN_HEIGHT,
             rate, transport ? transport : "spp");
    send(line, user);
    LOG_I("Sent dev declaration (rate=%u, transport=%s)", rate, transport);
}

void hlp_handle_line(const char* line, size_t length, hlp_send_line_fn send,
                     hlp_state_update_fn on_state, void* user) {
    if (!line || length == 0) return;

    StaticJsonDocument<768> doc;
    DeserializationError err = deserializeJson(doc, line);
    if (err) {
        LOG_W("JSON parse error: %s", err.c_str());
        return;
    }

    JsonObjectConst root = doc.as<JsonObjectConst>();
    int v = root["v"] | 0;
    if (v != 1) {
        return; // Drop unsupported protocol version
    }

    const char* t = root["t"] | "";
    if (!t || !*t) return;

    // 1. Check Ping immediately! Must answer pong before anything else
    if (strcmp(t, "ping") == 0) {
        if (send) {
            send("{\"v\":1,\"t\":\"pong\"}", user);
        }
        return;
    }

    // 2. Check Device Config messages
    if (hlp_device_config_handle(root, send, user)) {
        return;
    }

    // 3. State update snapshot
    if (strcmp(t, "s") == 0) {
        if (HlpState::decodeState(line, s_current_state)) {
            s_current_state.last_rx_ms = millis();
            if (on_state) {
                on_state(s_current_state, user);
            }
        }
        return;
    }

    // 4. Handshake response (hi)
    if (strcmp(t, "hi") == 0) {
        uint32_t new_sess = root["sess"] | 0;
        if (new_sess != s_current_state.sess) {
            LOG_I("New HLP session id=%u detected, resetting state cache", new_sess);
            HlpState::reset(s_current_state);
            s_current_state.sess = new_sess;
        }
        s_current_state.last_rx_ms = millis();
        LOG_I("Handshake hi acknowledged. Rate=%d", root["rate"] | 0);
        return;
    }
}

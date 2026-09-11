#include "hlp_device_config.h"
#include "utils/nvs_config.h"
#include "utils/logger.h"
#include <cstring>
#include <cstdio>

constexpr int CFG_COUNT = 6;

static NvsSettings s_draft;
static uint32_t s_transaction = 0;
static uint32_t s_received_mask = 0;
static int s_received_count = 0;
static int s_expected_count = 0;

static void send_ack(hlp_send_line_t send, void* user, bool ok, const char* field, const char* error) {
    if (!send) return;
    char line[192];
    if (ok) {
        snprintf(line, sizeof(line),
                 "{\"v\":1,\"t\":\"cfg_ack\",\"tx\":%u,\"ok\":true,\"rev\":%u}",
                 s_transaction, NvsConfig::get().config_revision);
    } else {
        snprintf(line, sizeof(line),
                 "{\"v\":1,\"t\":\"cfg_ack\",\"tx\":%u,\"ok\":false,\"field\":\"%s\",\"error\":\"%s\"}",
                 s_transaction, field ? field : "", error ? error : "unknown");
    }
    send(line, user);
}

void hlp_device_config_init() {
    NvsConfig::init();
}

void hlp_device_config_publish(hlp_send_line_t send, void* user) {
    if (!send) return;
    const NvsSettings& active = NvsConfig::get();
    char line[320];

    // cfg_begin
    snprintf(line, sizeof(line),
             "{\"v\":1,\"t\":\"cfg_begin\",\"rev\":%u,\"count\":%d,\"title\":\"Cấu hình HUD\"}",
             active.config_revision, CFG_COUNT);
    send(line, user);

    // 1. brightness
    snprintf(line, sizeof(line),
             "{\"v\":1,\"t\":\"cfg_item\",\"rev\":%u,\"id\":\"brightness\",\"kind\":\"slider\",\"label\":\"Độ sáng\",\"value\":%d,\"min\":10,\"max\":100,\"step\":5}",
             active.config_revision, active.brightness);
    send(line, user);

    // 2. show_eta
    snprintf(line, sizeof(line),
             "{\"v\":1,\"t\":\"cfg_item\",\"rev\":%u,\"id\":\"show_eta\",\"kind\":\"toggle\",\"label\":\"Hiện giờ đến\",\"value\":%s}",
             active.config_revision, active.show_eta ? "true" : "false");
    send(line, user);

    // 3. show_alerts
    snprintf(line, sizeof(line),
             "{\"v\":1,\"t\":\"cfg_item\",\"rev\":%u,\"id\":\"show_alerts\",\"kind\":\"toggle\",\"label\":\"Hiện cảnh báo\",\"value\":%s}",
             active.config_revision, active.show_alerts ? "true" : "false");
    send(line, user);

    // 4. theme
    snprintf(line, sizeof(line),
             "{\"v\":1,\"t\":\"cfg_item\",\"rev\":%u,\"id\":\"theme\",\"kind\":\"selection\",\"label\":\"Giao diện\",\"value\":\"%s\",\"options\":[{\"value\":\"auto\",\"label\":\"Tự động\"},{\"value\":\"day\",\"label\":\"Ban ngày\"},{\"value\":\"night\",\"label\":\"Ban đêm\"}]}",
             active.config_revision, active.theme);
    send(line, user);

    // 5. wifi_enabled
    snprintf(line, sizeof(line),
             "{\"v\":1,\"t\":\"cfg_item\",\"rev\":%u,\"id\":\"wifi_enabled\",\"kind\":\"toggle\",\"label\":\"WiFi HUD\",\"value\":%s}",
             active.config_revision, active.wifi_enabled ? "true" : "false");
    send(line, user);

    // 6. label
    snprintf(line, sizeof(line),
             "{\"v\":1,\"t\":\"cfg_item\",\"rev\":%u,\"id\":\"label\",\"kind\":\"text\",\"label\":\"Tên hiển thị\",\"value\":\"%s\",\"maxLength\":20}",
             active.config_revision, active.label);
    send(line, user);

    // cfg_end
    snprintf(line, sizeof(line),
             "{\"v\":1,\"t\":\"cfg_end\",\"rev\":%u}",
             active.config_revision);
    send(line, user);
}

bool hlp_device_config_handle(const JsonObjectConst& root, hlp_send_line_t send, void* user) {
    const char* type = root["t"] | "";
    if (strcmp(type, "hi") == 0) {
        hlp_device_config_publish(send, user);
        return true;
    }

    if (strcmp(type, "cfg_set_begin") == 0) {
        s_transaction = root["tx"] | 0;
        uint32_t rev = root["rev"] | 0;
        s_expected_count = root["count"] | 0;
        s_received_mask = 0;
        s_received_count = 0;
        s_draft = NvsConfig::get();

        if (rev != s_draft.config_revision || s_expected_count != CFG_COUNT) {
            send_ack(send, user, false, nullptr, "schema revision or count mismatch");
            s_transaction = 0;
        }
        return true;
    }

    if (strcmp(type, "cfg_set") == 0) {
        uint32_t tx = root["tx"] | 0;
        if (!s_transaction || tx != s_transaction) return true;

        const char* id = root["id"] | "";
        bool valid = true;
        uint32_t mask_before = s_received_mask;

        if (strcmp(id, "brightness") == 0) {
            int val = root["value"] | -1;
            if (val >= 10 && val <= 100 && ((val - 10) % 5) == 0) {
                s_draft.brightness = val;
                s_received_mask |= (1u << 0);
            } else {
                valid = false;
            }
        } else if (strcmp(id, "show_eta") == 0) {
            if (root["value"].is<bool>()) {
                s_draft.show_eta = root["value"].as<bool>();
                s_received_mask |= (1u << 1);
            } else {
                valid = false;
            }
        } else if (strcmp(id, "show_alerts") == 0) {
            if (root["value"].is<bool>()) {
                s_draft.show_alerts = root["value"].as<bool>();
                s_received_mask |= (1u << 2);
            } else {
                valid = false;
            }
        } else if (strcmp(id, "theme") == 0) {
            const char* val = root["value"] | "";
            if (!strcmp(val, "auto") || !strcmp(val, "day") || !strcmp(val, "night")) {
                strlcpy(s_draft.theme, val, sizeof(s_draft.theme));
                s_received_mask |= (1u << 3);
            } else {
                valid = false;
            }
        } else if (strcmp(id, "wifi_enabled") == 0) {
            if (root["value"].is<bool>()) {
                s_draft.wifi_enabled = root["value"].as<bool>();
                s_received_mask |= (1u << 4);
            } else {
                valid = false;
            }
        } else if (strcmp(id, "label") == 0) {
            const char* val = root["value"] | "";
            if (strlen(val) <= 20) {
                strlcpy(s_draft.label, val, sizeof(s_draft.label));
                s_received_mask |= (1u << 5);
            } else {
                valid = false;
            }
        } else {
            valid = false;
        }

        if (valid && s_received_mask == mask_before) {
            valid = false; // duplicate id in same tx
        }
        if (valid) {
            s_received_count++;
        } else {
            send_ack(send, user, false, id, "invalid value or duplicate id");
            s_transaction = 0;
        }
        return true;
    }

    if (strcmp(type, "cfg_set_commit") == 0) {
        uint32_t tx = root["tx"] | 0;
        if (!s_transaction || tx != s_transaction) return true;

        if (s_received_count != s_expected_count ||
            s_received_mask != ((1u << CFG_COUNT) - 1u)) {
            send_ack(send, user, false, nullptr, "incomplete transaction");
        } else {
            s_draft.config_revision++;
            if (NvsConfig::save(s_draft)) {
                send_ack(send, user, true, nullptr, nullptr);
            } else {
                send_ack(send, user, false, nullptr, "NVS write failed");
            }
        }
        s_transaction = 0;
        return true;
    }

    return false;
}

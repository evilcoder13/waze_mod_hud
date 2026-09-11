#pragma once

#include <cstdint>
#include <cstddef>

struct NvsSettings {
    int brightness = 70;         // 10..100
    bool show_eta = true;
    bool show_alerts = true;
    char theme[8] = "auto";       // auto, day, night
    bool wifi_enabled = false;
    char wifi_ssid[33] = "";
    char wifi_pass[65] = "";
    char label[21] = "WazeHUD";
    uint32_t config_revision = 1;
};

class NvsConfig {
public:
    static void init();
    static void load(NvsSettings& settings);
    static bool save(const NvsSettings& settings);
    static const NvsSettings& get();
    static void set(const NvsSettings& settings);

private:
    static NvsSettings s_active_settings;
};

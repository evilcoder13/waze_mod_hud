#include "nvs_config.h"
#include <Preferences.h>
#include <cstring>
#include "logger.h"

static Preferences s_prefs;
constexpr const char* NVS_NAMESPACE = "wazehud_cfg";

NvsSettings NvsConfig::s_active_settings;

void NvsConfig::init() {
    load(s_active_settings);
}

void NvsConfig::load(NvsSettings& settings) {
    if (!s_prefs.begin(NVS_NAMESPACE, true)) { // Read-only
        LOG_W("NVS namespace %s open failed, using defaults", NVS_NAMESPACE);
        return;
    }

    settings.brightness = s_prefs.getInt("bright", 70);
    settings.show_eta = s_prefs.getBool("eta", true);
    settings.show_alerts = s_prefs.getBool("alerts", true);
    settings.wifi_enabled = s_prefs.getBool("wifi_en", false);
    settings.config_revision = s_prefs.getUInt("rev", 1);

    String themeStr = s_prefs.getString("theme", "auto");
    strlcpy(settings.theme, themeStr.c_str(), sizeof(settings.theme));

    String ssidStr = s_prefs.getString("w_ssid", "");
    strlcpy(settings.wifi_ssid, ssidStr.c_str(), sizeof(settings.wifi_ssid));

    String passStr = s_prefs.getString("w_pass", "");
    strlcpy(settings.wifi_pass, passStr.c_str(), sizeof(settings.wifi_pass));

    String labelStr = s_prefs.getString("label", "WazeHUD");
    strlcpy(settings.label, labelStr.c_str(), sizeof(settings.label));

    s_prefs.end();
    LOG_I("Loaded NVS config: brightness=%d, rev=%u, wifi=%d",
          settings.brightness, settings.config_revision, settings.wifi_enabled);
}

bool NvsConfig::save(const NvsSettings& settings) {
    if (!s_prefs.begin(NVS_NAMESPACE, false)) { // Read-write
        LOG_E("NVS namespace %s open for write failed", NVS_NAMESPACE);
        return false;
    }

    s_prefs.putInt("bright", settings.brightness);
    s_prefs.putBool("eta", settings.show_eta);
    s_prefs.putBool("alerts", settings.show_alerts);
    s_prefs.putBool("wifi_en", settings.wifi_enabled);
    s_prefs.putUInt("rev", settings.config_revision);
    s_prefs.putString("theme", settings.theme);
    s_prefs.putString("w_ssid", settings.wifi_ssid);
    s_prefs.putString("w_pass", settings.wifi_pass);
    s_prefs.putString("label", settings.label);

    s_prefs.end();
    s_active_settings = settings;
    LOG_I("Saved NVS config successfully, revision=%u", settings.config_revision);
    return true;
}

const NvsSettings& NvsConfig::get() {
    return s_active_settings;
}

void NvsConfig::set(const NvsSettings& settings) {
    s_active_settings = settings;
}

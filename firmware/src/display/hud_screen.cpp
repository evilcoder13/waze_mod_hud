#include "hud_screen.h"
#include "turn_icons.h"
#include "fonts/fonts.h"
#include "display_driver.h"
#include "config.h"
#include "utils/logger.h"
#include <cstdio>
#include <cstring>

static lv_obj_t* s_scr_boot = nullptr;
static lv_obj_t* s_scr_connecting = nullptr;
static lv_obj_t* s_scr_hud = nullptr;
static lv_obj_t* s_scr_no_signal = nullptr;

// ==========================================
// 1. TOP BAR (Alert Bar & HUD Toggle Button)
// ==========================================
static lv_obj_t* s_bar_top = nullptr;
static lv_obj_t* s_lbl_top_alert = nullptr;
static lv_obj_t* s_btn_hud_toggle = nullptr;
static lv_obj_t* s_lbl_hud_btn = nullptr;

// ==========================================
// 2. MIDDLE - LEFT: Turn Indicator
// ==========================================
static lv_obj_t* s_lbl_turn_icon = nullptr;
static lv_obj_t* s_lbl_turn_dst = nullptr;
static lv_obj_t* s_lbl_turn_st2 = nullptr;

// ==========================================
// 3. MIDDLE - CENTER: Speed & Speed Limit
// ==========================================
static lv_obj_t* s_lbl_speed = nullptr;
static lv_obj_t* s_lbl_speed_unit = nullptr;
static lv_obj_t* s_obj_limit_circle = nullptr;
static lv_obj_t* s_lbl_limit = nullptr;

// ==========================================
// 4. MIDDLE - RIGHT: Primary & Secondary Alerts
// ==========================================
static lv_obj_t* s_obj_alert_main = nullptr;
static lv_obj_t* s_lbl_alert_icon = nullptr;
static lv_obj_t* s_lbl_alert_name = nullptr;
static lv_obj_t* s_lbl_alert_dst = nullptr;

static lv_obj_t* s_obj_sub_alr1 = nullptr;
static lv_obj_t* s_lbl_sub_alr1_val = nullptr;
static lv_obj_t* s_lbl_sub_alr1_dst = nullptr;

static lv_obj_t* s_obj_sub_alr2 = nullptr;
static lv_obj_t* s_lbl_sub_alr2_val = nullptr;
static lv_obj_t* s_lbl_sub_alr2_dst = nullptr;

// ==========================================
// 5. BOTTOM: Street Name & Trip ETA Info
// ==========================================
static lv_obj_t* s_bar_bottom = nullptr;
static lv_obj_t* s_lbl_street = nullptr;
static lv_obj_t* s_lbl_eta_info = nullptr;

// Button click event handler to toggle Mirror HUD mode
static void hud_toggle_event_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        LOG_I("HUD toggle button clicked!");
        DisplayDriver::toggleMirror();
        if (s_lbl_hud_btn) {
            if (DisplayDriver::isMirrored()) {
                lv_label_set_text(s_lbl_hud_btn, "HUD 🪞");
            } else {
                lv_label_set_text(s_lbl_hud_btn, "HUD 🔄");
            }
        }
    }
}

void HudScreen::createBootScreen() {
    s_scr_boot = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(s_scr_boot, lv_color_hex(0x000000), 0);

    lv_obj_t* lbl = lv_label_create(s_scr_boot);
    lv_label_set_text(lbl, "WazeHUD CYD");
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x4FC3F7), 0);
    lv_obj_set_style_text_font(lbl, &font_vietnam_24, 0);
    lv_obj_align(lbl, LV_ALIGN_CENTER, 0, -25);

    lv_obj_t* sub = lv_label_create(s_scr_boot);
    lv_label_set_text(sub, "Đang khởi động...");
    lv_obj_set_style_text_color(sub, lv_color_hex(0x9E9E9E), 0);
    lv_obj_set_style_text_font(sub, &font_vietnam_16, 0);
    lv_obj_align(sub, LV_ALIGN_CENTER, 0, 20);
}

void HudScreen::createConnectingScreen() {
    s_scr_connecting = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(s_scr_connecting, lv_color_hex(0x000000), 0);

    lv_obj_t* icon = lv_label_create(s_scr_connecting);
    lv_label_set_text(icon, LV_SYMBOL_BLUETOOTH);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -45);

    lv_obj_t* lbl = lv_label_create(s_scr_connecting);
    lv_label_set_text(lbl, "Đang tìm Waze Mod...");
    lv_obj_set_style_text_color(lbl, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(lbl, &font_vietnam_24, 0);
    lv_obj_align(lbl, LV_ALIGN_CENTER, 0, 15);

    lv_obj_t* dev = lv_label_create(s_scr_connecting);
    lv_label_set_text(dev, "Bluetooth: WazeHUD");
    lv_obj_set_style_text_color(dev, lv_color_hex(0x9E9E9E), 0);
    lv_obj_set_style_text_font(dev, &font_vietnam_16, 0);
    lv_obj_align(dev, LV_ALIGN_CENTER, 0, 48);
}

void HudScreen::createNoSignalScreen() {
    s_scr_no_signal = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(s_scr_no_signal, lv_color_hex(0x000000), 0);

    lv_obj_t* lbl = lv_label_create(s_scr_no_signal);
    lv_label_set_text(lbl, "MẤT TÍN HIỆU");
    lv_obj_set_style_text_color(lbl, lv_color_hex(0xFF5252), 0);
    lv_obj_set_style_text_font(lbl, &font_vietnam_24, 0);
    lv_obj_align(lbl, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t* sub = lv_label_create(s_scr_no_signal);
    lv_label_set_text(sub, "Mở Waze và bật dẫn đường trên điện thoại");
    lv_obj_set_style_text_color(sub, lv_color_hex(0xB0BEC5), 0);
    lv_obj_set_style_text_font(sub, &font_vietnam_16, 0);
    lv_obj_align(sub, LV_ALIGN_CENTER, 0, 20);
}

void HudScreen::createHudScreen() {
    s_scr_hud = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(s_scr_hud, lv_color_hex(0x000000), 0);
    lv_obj_clear_flag(s_scr_hud, LV_OBJ_FLAG_SCROLLABLE);

    // =======================================================
    // 1. TOP BAR (Alert Bar 320x32 px + HUD Mode Toggle Button)
    // =======================================================
    s_bar_top = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_bar_top, 320, 32);
    lv_obj_set_pos(s_bar_top, 0, 0);
    lv_obj_set_style_radius(s_bar_top, 0, 0);
    lv_obj_set_style_border_width(s_bar_top, 0, 0);
    lv_obj_set_style_bg_color(s_bar_top, lv_color_hex(0x111118), 0); // Dark standby
    lv_obj_clear_flag(s_bar_top, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_top_alert = lv_label_create(s_bar_top);
    lv_label_set_text(s_lbl_top_alert, "WazeHUD • Đã kết nối");
    lv_obj_set_width(s_lbl_top_alert, 230);
    lv_label_set_long_mode(s_lbl_top_alert, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_color(s_lbl_top_alert, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_text_font(s_lbl_top_alert, &font_vietnam_16, 0);
    lv_obj_align(s_lbl_top_alert, LV_ALIGN_LEFT_MID, 6, 0);

    // Toggle Button on Top-Right Corner (w=72, h=26)
    s_btn_hud_toggle = lv_btn_create(s_bar_top);
    lv_obj_set_size(s_btn_hud_toggle, 72, 26);
    lv_obj_align(s_btn_hud_toggle, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_radius(s_btn_hud_toggle, 4, 0);
    lv_obj_set_style_bg_color(s_btn_hud_toggle, lv_color_hex(0x2A2A38), 0);
    lv_obj_set_style_border_width(s_btn_hud_toggle, 1, 0);
    lv_obj_set_style_border_color(s_btn_hud_toggle, lv_color_hex(0x555566), 0);
    lv_obj_add_event_cb(s_btn_hud_toggle, hud_toggle_event_cb, LV_EVENT_CLICKED, nullptr);

    s_lbl_hud_btn = lv_label_create(s_btn_hud_toggle);
    lv_label_set_text(s_lbl_hud_btn, "HUD 🔄");
    lv_obj_set_style_text_font(s_lbl_hud_btn, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_hud_btn, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(s_lbl_hud_btn, LV_ALIGN_CENTER, 0, 0);

    // =======================================================
    // 2. MIDDLE - LEFT: Turn Direction, Distance & Next Street
    // =======================================================
    s_lbl_turn_icon = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_turn_icon, "");
    lv_obj_set_style_text_font(s_lbl_turn_icon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(s_lbl_turn_icon, lv_color_hex(0x00E5FF), 0); // Cyan
    lv_obj_set_pos(s_lbl_turn_icon, 24, 40);

    s_lbl_turn_dst = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_turn_dst, "");
    lv_obj_set_style_text_font(s_lbl_turn_dst, &font_vietnam_24, 0);
    lv_obj_set_style_text_color(s_lbl_turn_dst, lv_color_hex(0xFFEB3B), 0); // Gold/Yellow
    lv_obj_set_pos(s_lbl_turn_dst, 10, 96);

    s_lbl_turn_st2 = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_turn_st2, "");
    lv_obj_set_width(s_lbl_turn_st2, 90);
    lv_label_set_long_mode(s_lbl_turn_st2, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(s_lbl_turn_st2, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_turn_st2, lv_color_hex(0xB0BEC5), 0);
    lv_obj_set_pos(s_lbl_turn_st2, 8, 130);

    // =======================================================
    // 3. MIDDLE - CENTER: Speed & Speed Limit Sign
    // =======================================================
    s_lbl_speed = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_speed, "0");
    lv_obj_set_style_text_font(s_lbl_speed, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(s_lbl_speed, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_pos(s_lbl_speed, 108, 42);

    s_lbl_speed_unit = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_speed_unit, "km/h");
    lv_obj_set_style_text_font(s_lbl_speed_unit, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_speed_unit, lv_color_hex(0x9E9E9E), 0);
    lv_obj_set_pos(s_lbl_speed_unit, 114, 102);

    // Round Speed Limit Sign (Standard Vietnam: White bg, Red border, Black number)
    s_obj_limit_circle = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_obj_limit_circle, 54, 54);
    lv_obj_set_pos(s_obj_limit_circle, 168, 42);
    lv_obj_set_style_radius(s_obj_limit_circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s_obj_limit_circle, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_color(s_obj_limit_circle, lv_color_hex(0xE53935), 0);
    lv_obj_set_style_border_width(s_obj_limit_circle, 5, 0);
    lv_obj_clear_flag(s_obj_limit_circle, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_limit = lv_label_create(s_obj_limit_circle);
    lv_label_set_text(s_lbl_limit, "60");
    lv_obj_set_style_text_font(s_lbl_limit, &font_vietnam_24, 0);
    lv_obj_set_style_text_color(s_lbl_limit, lv_color_hex(0x000000), 0);
    lv_obj_align(s_lbl_limit, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(s_obj_limit_circle, LV_OBJ_FLAG_HIDDEN);

    // =======================================================
    // 4. MIDDLE - RIGHT: Primary Alert & Secondary (alrs)
    // =======================================================
    // Primary Alert Card
    s_obj_alert_main = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_obj_alert_main, 82, 58);
    lv_obj_set_pos(s_obj_alert_main, 234, 38);
    lv_obj_set_style_radius(s_obj_alert_main, 6, 0);
    lv_obj_set_style_bg_color(s_obj_alert_main, lv_color_hex(0x181824), 0);
    lv_obj_set_style_border_width(s_obj_alert_main, 1, 0);
    lv_obj_set_style_border_color(s_obj_alert_main, lv_color_hex(0x00E5FF), 0);
    lv_obj_clear_flag(s_obj_alert_main, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_alert_icon = lv_label_create(s_obj_alert_main);
    lv_label_set_text(s_lbl_alert_icon, LV_SYMBOL_WARNING);
    lv_obj_set_style_text_font(s_lbl_alert_icon, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(s_lbl_alert_icon, lv_color_hex(0x00E5FF), 0);
    lv_obj_align(s_lbl_alert_icon, LV_ALIGN_TOP_LEFT, -2, -6);

    s_lbl_alert_name = lv_label_create(s_obj_alert_main);
    lv_label_set_text(s_lbl_alert_name, "CAMERA");
    lv_obj_set_style_text_font(s_lbl_alert_name, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_alert_name, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(s_lbl_alert_name, LV_ALIGN_TOP_RIGHT, 4, -4);

    s_lbl_alert_dst = lv_label_create(s_obj_alert_main);
    lv_label_set_text(s_lbl_alert_dst, "100 M");
    lv_obj_set_style_text_font(s_lbl_alert_dst, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_alert_dst, lv_color_hex(0xFFEB3B), 0); // Yellow
    lv_obj_align(s_lbl_alert_dst, LV_ALIGN_BOTTOM_MID, 0, 4);

    lv_obj_add_flag(s_obj_alert_main, LV_OBJ_FLAG_HIDDEN);

    // Secondary Upcoming Alerts (2 small round signs)
    // Sign 1
    s_obj_sub_alr1 = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_obj_sub_alr1, 34, 34);
    lv_obj_set_pos(s_obj_sub_alr1, 234, 102);
    lv_obj_set_style_radius(s_obj_sub_alr1, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s_obj_sub_alr1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_color(s_obj_sub_alr1, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_border_width(s_obj_sub_alr1, 2, 0);
    lv_obj_clear_flag(s_obj_sub_alr1, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_sub_alr1_val = lv_label_create(s_obj_sub_alr1);
    lv_label_set_text(s_lbl_sub_alr1_val, "80");
    lv_obj_set_style_text_font(s_lbl_sub_alr1_val, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_sub_alr1_val, lv_color_hex(0x000000), 0);
    lv_obj_align(s_lbl_sub_alr1_val, LV_ALIGN_CENTER, 0, 0);

    s_lbl_sub_alr1_dst = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_sub_alr1_dst, "");
    lv_obj_set_style_text_font(s_lbl_sub_alr1_dst, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_sub_alr1_dst, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_pos(s_lbl_sub_alr1_dst, 226, 138);

    lv_obj_add_flag(s_obj_sub_alr1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_lbl_sub_alr1_dst, LV_OBJ_FLAG_HIDDEN);

    // Sign 2
    s_obj_sub_alr2 = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_obj_sub_alr2, 34, 34);
    lv_obj_set_pos(s_obj_sub_alr2, 278, 102);
    lv_obj_set_style_radius(s_obj_sub_alr2, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s_obj_sub_alr2, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_color(s_obj_sub_alr2, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_border_width(s_obj_sub_alr2, 2, 0);
    lv_obj_clear_flag(s_obj_sub_alr2, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_sub_alr2_val = lv_label_create(s_obj_sub_alr2);
    lv_label_set_text(s_lbl_sub_alr2_val, "60");
    lv_obj_set_style_text_font(s_lbl_sub_alr2_val, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_sub_alr2_val, lv_color_hex(0x000000), 0);
    lv_obj_align(s_lbl_sub_alr2_val, LV_ALIGN_CENTER, 0, 0);

    s_lbl_sub_alr2_dst = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_sub_alr2_dst, "");
    lv_obj_set_style_text_font(s_lbl_sub_alr2_dst, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_sub_alr2_dst, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_pos(s_lbl_sub_alr2_dst, 272, 138);

    lv_obj_add_flag(s_obj_sub_alr2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_lbl_sub_alr2_dst, LV_OBJ_FLAG_HIDDEN);

    // =======================================================
    // 5. BOTTOM: Current Street & Remaining Distance / ETA
    // =======================================================
    s_bar_bottom = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_bar_bottom, 320, 56);
    lv_obj_set_pos(s_bar_bottom, 0, 184); // Elevated to avoid being hidden by phone mount
    lv_obj_set_style_radius(s_bar_bottom, 0, 0);
    lv_obj_set_style_border_width(s_bar_bottom, 0, 0);
    lv_obj_set_style_bg_color(s_bar_bottom, lv_color_hex(0x000000), 0); // Pure Black
    lv_obj_clear_flag(s_bar_bottom, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_street = lv_label_create(s_bar_bottom);
    lv_label_set_text(s_lbl_street, "");
    lv_obj_set_width(s_lbl_street, 308);
    lv_label_set_long_mode(s_lbl_street, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(s_lbl_street, &font_vietnam_24, 0);
    lv_obj_set_style_text_color(s_lbl_street, lv_color_hex(0xFFFFFF), 0); // White
    lv_obj_set_pos(s_lbl_street, 6, 2);

    s_lbl_eta_info = lv_label_create(s_bar_bottom);
    lv_label_set_text(s_lbl_eta_info, "");
    lv_obj_set_width(s_lbl_eta_info, 308);
    lv_obj_set_style_text_font(s_lbl_eta_info, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_eta_info, lv_color_hex(0xCFD8DC), 0);
    lv_obj_set_pos(s_lbl_eta_info, 6, 30);
}

void HudScreen::init() {
    createBootScreen();
    createConnectingScreen();
    createHudScreen();
    createNoSignalScreen();
    setScreenState(ScreenState::BOOT);
}

void HudScreen::setScreenState(ScreenState state) {
    switch (state) {
        case ScreenState::BOOT:
            lv_scr_load(s_scr_boot);
            break;
        case ScreenState::CONNECTING:
            lv_scr_load(s_scr_connecting);
            break;
        case ScreenState::HUD_ACTIVE:
            lv_scr_load(s_scr_hud);
            break;
        case ScreenState::NO_SIGNAL:
            lv_scr_load(s_scr_no_signal);
            break;
    }
}

void HudScreen::updateData(const HudState& state) {
    char buf[64];

    // ==========================================
    // 1. TOP ALERT BAR (Red background on alert)
    // ==========================================
    if (state.alr > 0) {
        if (state.alrD >= 0) {
            snprintf(buf, sizeof(buf), "⚠ %s (cách %d m)", get_alert_name_vi(state.alr), state.alrD);
        } else {
            snprintf(buf, sizeof(buf), "⚠ %s", get_alert_name_vi(state.alr));
        }
        lv_label_set_text(s_lbl_top_alert, buf);
        lv_obj_set_style_bg_color(s_bar_top, lv_color_hex(0xD32F2F), 0); // Red Alert Bar
        lv_obj_set_style_text_color(s_lbl_top_alert, lv_color_hex(0xFFFFFF), 0);
    } else {
        lv_label_set_text(s_lbl_top_alert, "WazeHUD • Đã kết nối");
        lv_obj_set_style_bg_color(s_bar_top, lv_color_hex(0x111118), 0); // Dark default
        lv_obj_set_style_text_color(s_lbl_top_alert, lv_color_hex(0x00E5FF), 0);
    }

    // ==========================================
    // 2. TURN DIRECTION (Left Column)
    // ==========================================
    if (state.nav && state.trn > 0) {
        lv_label_set_text(s_lbl_turn_icon, get_turn_symbol(state.trn));
        if (state.dst >= 0) {
            if (state.dst >= 1000) {
                snprintf(buf, sizeof(buf), "%.1f KM", state.dst / 1000.0f);
            } else {
                snprintf(buf, sizeof(buf), "%d M", state.dst);
            }
            lv_label_set_text(s_lbl_turn_dst, buf);
        } else {
            lv_label_set_text(s_lbl_turn_dst, "");
        }
        if (strlen(state.st2) > 0) {
            snprintf(buf, sizeof(buf), "Rẽ: %s", state.st2);
            lv_label_set_text(s_lbl_turn_st2, buf);
        } else {
            lv_label_set_text(s_lbl_turn_st2, "");
        }
    } else {
        lv_label_set_text(s_lbl_turn_icon, "");
        lv_label_set_text(s_lbl_turn_dst, "");
        lv_label_set_text(s_lbl_turn_st2, "");
    }

    // ==========================================
    // 3. SPEED & SPEED LIMIT (Center Column)
    // ==========================================
    snprintf(buf, sizeof(buf), "%d", state.spd);
    lv_label_set_text(s_lbl_speed, buf);
    if (state.over || (state.lim > 0 && state.spd > state.lim)) {
        lv_obj_set_style_text_color(s_lbl_speed, lv_color_hex(0xFF1744), 0); // Vivid Red
    } else {
        lv_obj_set_style_text_color(s_lbl_speed, lv_color_hex(0xFFFFFF), 0); // White
    }

    if (state.lim > 0) {
        snprintf(buf, sizeof(buf), "%d", state.lim);
        lv_label_set_text(s_lbl_limit, buf);
        lv_obj_clear_flag(s_obj_limit_circle, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_obj_limit_circle, LV_OBJ_FLAG_HIDDEN);
    }

    // ==========================================
    // 4. ALERTS (Right Column)
    // ==========================================
    if (state.alr > 0) {
        lv_label_set_text(s_lbl_alert_icon, get_alert_icon_symbol(state.alr));
        lv_label_set_text(s_lbl_alert_name, get_alert_name_vi(state.alr));
        if (state.alrD >= 0) {
            if (state.alrD >= 1000) {
                snprintf(buf, sizeof(buf), "%.1f KM", state.alrD / 1000.0f);
            } else {
                snprintf(buf, sizeof(buf), "%d M", state.alrD);
            }
            lv_label_set_text(s_lbl_alert_dst, buf);
        } else {
            lv_label_set_text(s_lbl_alert_dst, "");
        }
        lv_obj_clear_flag(s_obj_alert_main, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_obj_alert_main, LV_OBJ_FLAG_HIDDEN);
    }

    // Secondary Alert Signs (alrs)
    if (state.alrs_count > 0 && state.alrs[0].v > 0) {
        snprintf(buf, sizeof(buf), "%d", state.alrs[0].v);
        lv_label_set_text(s_lbl_sub_alr1_val, buf);
        if (state.alrs[0].d >= 1000) {
            snprintf(buf, sizeof(buf), "%.1fK", state.alrs[0].d / 1000.0f);
        } else {
            snprintf(buf, sizeof(buf), "%dM", state.alrs[0].d);
        }
        lv_label_set_text(s_lbl_sub_alr1_dst, buf);
        lv_obj_clear_flag(s_obj_sub_alr1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(s_lbl_sub_alr1_dst, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_obj_sub_alr1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_lbl_sub_alr1_dst, LV_OBJ_FLAG_HIDDEN);
    }

    if (state.alrs_count > 1 && state.alrs[1].v > 0) {
        snprintf(buf, sizeof(buf), "%d", state.alrs[1].v);
        lv_label_set_text(s_lbl_sub_alr2_val, buf);
        if (state.alrs[1].d >= 1000) {
            snprintf(buf, sizeof(buf), "%.1fK", state.alrs[1].d / 1000.0f);
        } else {
            snprintf(buf, sizeof(buf), "%dM", state.alrs[1].d);
        }
        lv_label_set_text(s_lbl_sub_alr2_dst, buf);
        lv_obj_clear_flag(s_obj_sub_alr2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(s_lbl_sub_alr2_dst, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_obj_sub_alr2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_lbl_sub_alr2_dst, LV_OBJ_FLAG_HIDDEN);
    }

    // ==========================================
    // 5. BOTTOM: Current Street & ETA Info
    // ==========================================
    if (strlen(state.st) > 0) {
        snprintf(buf, sizeof(buf), "📍 %s", state.st);
        lv_label_set_text(s_lbl_street, buf);
    } else {
        lv_label_set_text(s_lbl_street, "");
    }

    if (state.avg) {
        if (state.avgL > 0) {
            snprintf(buf, sizeof(buf), "⛔ ĐOẠN CẤM VƯỢT (còn %d m)", state.avgL);
        } else {
            snprintf(buf, sizeof(buf), "⛔ ĐOẠN CẤM VƯỢT");
        }
        lv_label_set_text(s_lbl_eta_info, buf);
        lv_obj_set_style_text_color(s_lbl_eta_info, lv_color_hex(0xFF5722), 0);
    } else if (state.nav && (strlen(state.eta) > 0 || state.rm > 0 || state.rkm > 0)) {
        char eta_buf[32] = "";
        char dist_buf[32] = "";
        if (strlen(state.eta) > 0) {
            snprintf(eta_buf, sizeof(eta_buf), "ETA %s", state.eta);
        }
        if (state.rm > 0) {
            snprintf(dist_buf, sizeof(dist_buf), "Còn %.1f km", state.rm / 1000.0f);
        } else if (state.rkm > 0) {
            snprintf(dist_buf, sizeof(dist_buf), "Còn %.1f km", state.rkm);
        }

        if (strlen(eta_buf) > 0 && strlen(dist_buf) > 0) {
            snprintf(buf, sizeof(buf), "%s  •  %s", eta_buf, dist_buf);
        } else if (strlen(eta_buf) > 0) {
            snprintf(buf, sizeof(buf), "%s", eta_buf);
        } else {
            snprintf(buf, sizeof(buf), "%s", dist_buf);
        }
        lv_label_set_text(s_lbl_eta_info, buf);
        lv_obj_set_style_text_color(s_lbl_eta_info, lv_color_hex(0xCFD8DC), 0);
    } else {
        lv_label_set_text(s_lbl_eta_info, "");
    }
}

#include "hud_screen.h"
#include "turn_icons.h"
#include "fonts/fonts.h"
#include "config.h"
#include <cstdio>
#include <cstring>

static lv_obj_t* s_scr_boot = nullptr;
static lv_obj_t* s_scr_connecting = nullptr;
static lv_obj_t* s_scr_hud = nullptr;
static lv_obj_t* s_scr_no_signal = nullptr;

// HUD screen elements
static lv_obj_t* s_bar_alert = nullptr;
static lv_obj_t* s_lbl_alert = nullptr;

static lv_obj_t* s_lbl_speed = nullptr;
static lv_obj_t* s_lbl_speed_unit = nullptr;
static lv_obj_t* s_obj_limit_circle = nullptr;
static lv_obj_t* s_lbl_limit = nullptr;

static lv_obj_t* s_lbl_turn_icon = nullptr;
static lv_obj_t* s_lbl_turn_dst = nullptr;
static lv_obj_t* s_lbl_turn_st2 = nullptr;

static lv_obj_t* s_lbl_street = nullptr;
static lv_obj_t* s_lbl_eta_row = nullptr;
static lv_obj_t* s_lbl_dist_rem = nullptr;

static lv_obj_t* s_bar_avg = nullptr;
static lv_obj_t* s_lbl_avg = nullptr;

void HudScreen::createBootScreen() {
    s_scr_boot = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(s_scr_boot, lv_color_hex(0x101018), 0);

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
    lv_obj_set_style_bg_color(s_scr_connecting, lv_color_hex(0x101018), 0);

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
    lv_obj_set_style_bg_color(s_scr_no_signal, lv_color_hex(0x1A1A2E), 0);

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
    lv_obj_set_style_bg_color(s_scr_hud, lv_color_hex(0x1A1A2E), 0);
    lv_obj_clear_flag(s_scr_hud, LV_OBJ_FLAG_SCROLLABLE);

    // 1. Alert Bar (Top: 320x30)
    s_bar_alert = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_bar_alert, 320, 30);
    lv_obj_set_pos(s_bar_alert, 0, 0);
    lv_obj_set_style_radius(s_bar_alert, 0, 0);
    lv_obj_set_style_border_width(s_bar_alert, 0, 0);
    lv_obj_set_style_bg_color(s_bar_alert, lv_color_hex(0xD32F2F), 0);
    lv_obj_clear_flag(s_bar_alert, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_alert = lv_label_create(s_bar_alert);
    lv_label_set_text(s_lbl_alert, "");
    lv_obj_set_style_text_color(s_lbl_alert, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(s_lbl_alert, &font_vietnam_16, 0);
    lv_obj_align(s_lbl_alert, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(s_bar_alert, LV_OBJ_FLAG_HIDDEN);

    // 2. Left Section: Speed & Speed Limit
    s_lbl_speed = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_speed, "0");
    lv_obj_set_style_text_font(s_lbl_speed, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(s_lbl_speed, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_pos(s_lbl_speed, 18, 36);

    s_lbl_speed_unit = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_speed_unit, "km/h");
    lv_obj_set_style_text_font(s_lbl_speed_unit, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_speed_unit, lv_color_hex(0x9E9E9E), 0);
    lv_obj_set_pos(s_lbl_speed_unit, 26, 92);

    // Speed limit round sign
    s_obj_limit_circle = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_obj_limit_circle, 44, 44);
    lv_obj_set_pos(s_obj_limit_circle, 26, 120);
    lv_obj_set_style_radius(s_obj_limit_circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s_obj_limit_circle, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_color(s_obj_limit_circle, lv_color_hex(0xF44336), 0);
    lv_obj_set_style_border_width(s_obj_limit_circle, 4, 0);
    lv_obj_clear_flag(s_obj_limit_circle, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_limit = lv_label_create(s_obj_limit_circle);
    lv_label_set_text(s_lbl_limit, "50");
    lv_obj_set_style_text_font(s_lbl_limit, &font_vietnam_24, 0);
    lv_obj_set_style_text_color(s_lbl_limit, lv_color_hex(0x000000), 0);
    lv_obj_align(s_lbl_limit, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(s_obj_limit_circle, LV_OBJ_FLAG_HIDDEN);

    // 3. Middle Section: Turn Arrow, Turn Distance, Next Street
    s_lbl_turn_icon = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_turn_icon, "");
    lv_obj_set_style_text_font(s_lbl_turn_icon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(s_lbl_turn_icon, lv_color_hex(0x4FC3F7), 0);
    lv_obj_set_pos(s_lbl_turn_icon, 142, 34);

    s_lbl_turn_dst = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_turn_dst, "");
    lv_obj_set_width(s_lbl_turn_dst, 115);
    lv_obj_set_style_text_align(s_lbl_turn_dst, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(s_lbl_turn_dst, &font_vietnam_24, 0);
    lv_obj_set_style_text_color(s_lbl_turn_dst, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_pos(s_lbl_turn_dst, 108, 90);

    s_lbl_turn_st2 = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_turn_st2, "");
    lv_obj_set_width(s_lbl_turn_st2, 115);
    lv_obj_set_style_text_align(s_lbl_turn_st2, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(s_lbl_turn_st2, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(s_lbl_turn_st2, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_turn_st2, lv_color_hex(0xB0BEC5), 0);
    lv_obj_set_pos(s_lbl_turn_st2, 108, 125);

    // 4. Right Section: ETA, Distance Remaining & Average Zone
    s_lbl_eta_row = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_eta_row, "");
    lv_obj_set_width(s_lbl_eta_row, 96);
    lv_obj_set_style_text_font(s_lbl_eta_row, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_eta_row, lv_color_hex(0x90CAF9), 0);
    lv_obj_set_pos(s_lbl_eta_row, 224, 40);

    s_lbl_dist_rem = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_dist_rem, "");
    lv_obj_set_width(s_lbl_dist_rem, 96);
    lv_obj_set_style_text_font(s_lbl_dist_rem, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_dist_rem, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_pos(s_lbl_dist_rem, 224, 70);

    // Vietnam No-Passing Zone bar (Avg Zone)
    s_bar_avg = lv_bar_create(s_scr_hud);
    lv_obj_set_size(s_bar_avg, 94, 28);
    lv_obj_set_pos(s_bar_avg, 222, 128);
    lv_obj_set_style_bg_color(s_bar_avg, lv_color_hex(0x37474F), 0);
    lv_obj_set_style_bg_color(s_bar_avg, lv_color_hex(0xFF5722), LV_PART_INDICATOR);
    lv_bar_set_range(s_bar_avg, 0, 100);

    s_lbl_avg = lv_label_create(s_bar_avg);
    lv_label_set_text(s_lbl_avg, "CẤM VƯỢT");
    lv_obj_set_style_text_color(s_lbl_avg, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(s_lbl_avg, &font_vietnam_16, 0);
    lv_obj_align(s_lbl_avg, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(s_bar_avg, LV_OBJ_FLAG_HIDDEN);

    // 5. Bottom Street Bar (y: 202..240, 320x38)
    lv_obj_t* bar_street = lv_obj_create(s_scr_hud);
    lv_obj_set_size(bar_street, 320, 38);
    lv_obj_set_pos(bar_street, 0, 202);
    lv_obj_set_style_radius(bar_street, 0, 0);
    lv_obj_set_style_border_width(bar_street, 0, 0);
    lv_obj_set_style_bg_color(bar_street, lv_color_hex(0x10101F), 0);
    lv_obj_clear_flag(bar_street, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_street = lv_label_create(bar_street);
    lv_label_set_text(s_lbl_street, "");
    lv_obj_set_width(s_lbl_street, 305);
    lv_label_set_long_mode(s_lbl_street, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(s_lbl_street, &font_vietnam_24, 0);
    lv_obj_set_style_text_color(s_lbl_street, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(s_lbl_street, LV_ALIGN_CENTER, 0, 0);
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

    // Speed
    snprintf(buf, sizeof(buf), "%d", state.spd);
    lv_label_set_text(s_lbl_speed, buf);
    if (state.over || (state.lim > 0 && state.spd > state.lim)) {
        lv_obj_set_style_text_color(s_lbl_speed, lv_color_hex(0xFF5252), 0); // Red
    } else {
        lv_obj_set_style_text_color(s_lbl_speed, lv_color_hex(0xFFFFFF), 0); // White
    }

    // Speed limit
    if (state.lim > 0) {
        snprintf(buf, sizeof(buf), "%d", state.lim);
        lv_label_set_text(s_lbl_limit, buf);
        lv_obj_clear_flag(s_obj_limit_circle, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_obj_limit_circle, LV_OBJ_FLAG_HIDDEN);
    }

    // Turn arrow & distance
    if (state.nav && state.trn > 0) {
        lv_label_set_text(s_lbl_turn_icon, get_turn_symbol(state.trn));
        if (state.dst >= 0) {
            if (state.dst >= 1000) {
                snprintf(buf, sizeof(buf), "%.1f km", state.dst / 1000.0f);
            } else {
                snprintf(buf, sizeof(buf), "%d m", state.dst);
            }
            lv_label_set_text(s_lbl_turn_dst, buf);
        } else {
            lv_label_set_text(s_lbl_turn_dst, "");
        }
        lv_label_set_text(s_lbl_turn_st2, state.st2);
    } else {
        lv_label_set_text(s_lbl_turn_icon, "");
        lv_label_set_text(s_lbl_turn_dst, "");
        lv_label_set_text(s_lbl_turn_st2, "");
    }

    // Street name (Vietnamese full Unicode)
    lv_label_set_text(s_lbl_street, state.st);

    // ETA row & Distance remaining
    if (state.nav && (strlen(state.eta) > 0 || state.rm > 0 || state.rkm > 0)) {
        if (strlen(state.eta) > 0) {
            snprintf(buf, sizeof(buf), "ETA %s", state.eta);
            lv_label_set_text(s_lbl_eta_row, buf);
        } else {
            lv_label_set_text(s_lbl_eta_row, "");
        }

        if (state.rm > 0) {
            snprintf(buf, sizeof(buf), "Còn %.1f km", state.rm / 1000.0f);
            lv_label_set_text(s_lbl_dist_rem, buf);
        } else if (state.rkm > 0) {
            snprintf(buf, sizeof(buf), "Còn %.1f km", state.rkm);
            lv_label_set_text(s_lbl_dist_rem, buf);
        } else {
            lv_label_set_text(s_lbl_dist_rem, "");
        }
    } else {
        lv_label_set_text(s_lbl_eta_row, "");
        lv_label_set_text(s_lbl_dist_rem, "");
    }

    // Alert Bar
    if (state.alr > 0) {
        if (state.alrD >= 0) {
            snprintf(buf, sizeof(buf), "⚠  %s  (còn %d m)", get_alert_name_vi(state.alr), state.alrD);
        } else {
            snprintf(buf, sizeof(buf), "⚠  %s", get_alert_name_vi(state.alr));
        }
        lv_label_set_text(s_lbl_alert, buf);
        lv_obj_clear_flag(s_bar_alert, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_bar_alert, LV_OBJ_FLAG_HIDDEN);
    }

    // Average Zone Bar (Vietnam: Cấm Vượt)
    if (state.avg) {
        lv_bar_set_value(s_bar_avg, state.avgP, LV_ANIM_OFF);
        if (state.avgL > 0) {
            snprintf(buf, sizeof(buf), "%d m", state.avgL);
        } else {
            snprintf(buf, sizeof(buf), "CẤM VƯỢT");
        }
        lv_label_set_text(s_lbl_avg, buf);
        lv_obj_clear_flag(s_bar_avg, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_bar_avg, LV_OBJ_FLAG_HIDDEN);
    }
}

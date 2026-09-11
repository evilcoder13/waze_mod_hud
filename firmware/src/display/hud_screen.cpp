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

// --- HUD Main Elements (Dashboard Style 320x240) ---
// Left: Turn direction & Turn distance
static lv_obj_t* s_lbl_turn_icon = nullptr;
static lv_obj_t* s_lbl_turn_dst = nullptr;

// Center: Current Speed & Speed Limit Sign
static lv_obj_t* s_lbl_speed = nullptr;
static lv_obj_t* s_lbl_speed_unit = nullptr;
static lv_obj_t* s_obj_limit_circle = nullptr;
static lv_obj_t* s_lbl_limit = nullptr;

// Right Top: Primary Alert (Icon + Distance in cyan/orange)
static lv_obj_t* s_obj_alert_main = nullptr;
static lv_obj_t* s_lbl_alert_icon = nullptr;
static lv_obj_t* s_lbl_alert_dst = nullptr;

// Right Bottom: Upcoming Speed Signs (alrs)
static lv_obj_t* s_obj_alr1 = nullptr;
static lv_obj_t* s_lbl_alr1_val = nullptr;
static lv_obj_t* s_lbl_alr1_dst = nullptr;

static lv_obj_t* s_obj_alr2 = nullptr;
static lv_obj_t* s_lbl_alr2_val = nullptr;
static lv_obj_t* s_lbl_alr2_dst = nullptr;

// Bottom Section: Street & ETA
static lv_obj_t* s_lbl_street = nullptr;
static lv_obj_t* s_lbl_eta_info = nullptr;

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

    // ==========================================
    // 1. LEFT: Turn Indicator (Arrow + Distance)
    // ==========================================
    s_lbl_turn_icon = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_turn_icon, "");
    lv_obj_set_style_text_font(s_lbl_turn_icon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(s_lbl_turn_icon, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_pos(s_lbl_turn_icon, 20, 20);

    s_lbl_turn_dst = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_turn_dst, "");
    lv_obj_set_style_text_font(s_lbl_turn_dst, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_turn_dst, lv_color_hex(0xFFEB3B), 0); // Gold/Yellow
    lv_obj_set_pos(s_lbl_turn_dst, 10, 85);

    // ==========================================
    // 2. CENTER: Speed & Speed Limit Sign
    // ==========================================
    s_lbl_speed = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_speed, "0");
    lv_obj_set_style_text_font(s_lbl_speed, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(s_lbl_speed, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_pos(s_lbl_speed, 115, 20);

    s_lbl_speed_unit = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_speed_unit, "km/h");
    lv_obj_set_style_text_font(s_lbl_speed_unit, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_speed_unit, lv_color_hex(0x9E9E9E), 0);
    lv_obj_set_pos(s_lbl_speed_unit, 118, 85);

    // Speed limit circle (Round sign, white background, red border, black text)
    s_obj_limit_circle = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_obj_limit_circle, 54, 54);
    lv_obj_set_pos(s_obj_limit_circle, 175, 20);
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

    // ==========================================
    // 3. RIGHT TOP: Primary Alert (Icon + Distance)
    // ==========================================
    s_obj_alert_main = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_obj_alert_main, 75, 60);
    lv_obj_set_pos(s_obj_alert_main, 240, 15);
    lv_obj_set_style_bg_opa(s_obj_alert_main, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(s_obj_alert_main, 0, 0);
    lv_obj_clear_flag(s_obj_alert_main, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_alert_icon = lv_label_create(s_obj_alert_main);
    lv_label_set_text(s_lbl_alert_icon, "");
    lv_obj_set_style_text_font(s_lbl_alert_icon, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(s_lbl_alert_icon, lv_color_hex(0x00E5FF), 0); // Cyan
    lv_obj_align(s_lbl_alert_icon, LV_ALIGN_TOP_MID, 0, 0);

    s_lbl_alert_dst = lv_label_create(s_obj_alert_main);
    lv_label_set_text(s_lbl_alert_dst, "");
    lv_obj_set_style_text_font(s_lbl_alert_dst, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_alert_dst, lv_color_hex(0xFFEB3B), 0); // Yellow
    lv_obj_align(s_lbl_alert_dst, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(s_obj_alert_main, LV_OBJ_FLAG_HIDDEN);

    // ==========================================
    // 4. RIGHT BOTTOM: Upcoming Speed Signs (alrs)
    // ==========================================
    // Sign 1
    s_obj_alr1 = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_obj_alr1, 32, 32);
    lv_obj_set_pos(s_obj_alr1, 240, 85);
    lv_obj_set_style_radius(s_obj_alr1, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s_obj_alr1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_color(s_obj_alr1, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_border_width(s_obj_alr1, 2, 0);
    lv_obj_clear_flag(s_obj_alr1, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_alr1_val = lv_label_create(s_obj_alr1);
    lv_label_set_text(s_lbl_alr1_val, "80");
    lv_obj_set_style_text_font(s_lbl_alr1_val, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_alr1_val, lv_color_hex(0x000000), 0);
    lv_obj_align(s_lbl_alr1_val, LV_ALIGN_CENTER, 0, 0);

    s_lbl_alr1_dst = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_alr1_dst, "");
    lv_obj_set_style_text_font(s_lbl_alr1_dst, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_alr1_dst, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_pos(s_lbl_alr1_dst, 230, 120);

    lv_obj_add_flag(s_obj_alr1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_lbl_alr1_dst, LV_OBJ_FLAG_HIDDEN);

    // Sign 2
    s_obj_alr2 = lv_obj_create(s_scr_hud);
    lv_obj_set_size(s_obj_alr2, 32, 32);
    lv_obj_set_pos(s_obj_alr2, 280, 85);
    lv_obj_set_style_radius(s_obj_alr2, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s_obj_alr2, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_color(s_obj_alr2, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_border_width(s_obj_alr2, 2, 0);
    lv_obj_clear_flag(s_obj_alr2, LV_OBJ_FLAG_SCROLLABLE);

    s_lbl_alr2_val = lv_label_create(s_obj_alr2);
    lv_label_set_text(s_lbl_alr2_val, "60");
    lv_obj_set_style_text_font(s_lbl_alr2_val, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_alr2_val, lv_color_hex(0x000000), 0);
    lv_obj_align(s_lbl_alr2_val, LV_ALIGN_CENTER, 0, 0);

    s_lbl_alr2_dst = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_alr2_dst, "");
    lv_obj_set_style_text_font(s_lbl_alr2_dst, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_alr2_dst, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_pos(s_lbl_alr2_dst, 275, 120);

    lv_obj_add_flag(s_obj_alr2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_lbl_alr2_dst, LV_OBJ_FLAG_HIDDEN);

    // ==========================================
    // 5. BOTTOM: Street Name & Navigation Footer
    // ==========================================
    s_lbl_street = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_street, "");
    lv_obj_set_width(s_lbl_street, 310);
    lv_label_set_long_mode(s_lbl_street, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(s_lbl_street, &font_vietnam_24, 0);
    lv_obj_set_style_text_color(s_lbl_street, lv_color_hex(0xFFD54F), 0); // Amber/Gold text
    lv_obj_set_pos(s_lbl_street, 8, 160);

    s_lbl_eta_info = lv_label_create(s_scr_hud);
    lv_label_set_text(s_lbl_eta_info, "");
    lv_obj_set_width(s_lbl_eta_info, 310);
    lv_obj_set_style_text_font(s_lbl_eta_info, &font_vietnam_16, 0);
    lv_obj_set_style_text_color(s_lbl_eta_info, lv_color_hex(0xB0BEC5), 0);
    lv_obj_set_pos(s_lbl_eta_info, 8, 200);
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

    // 1. Current Speed
    snprintf(buf, sizeof(buf), "%d", state.spd);
    lv_label_set_text(s_lbl_speed, buf);
    if (state.over || (state.lim > 0 && state.spd > state.lim)) {
        lv_obj_set_style_text_color(s_lbl_speed, lv_color_hex(0xFF1744), 0); // Vivid Red
    } else {
        lv_obj_set_style_text_color(s_lbl_speed, lv_color_hex(0xFFFFFF), 0); // Pure White
    }

    // 2. Speed Limit Sign
    if (state.lim > 0) {
        snprintf(buf, sizeof(buf), "%d", state.lim);
        lv_label_set_text(s_lbl_limit, buf);
        lv_obj_clear_flag(s_obj_limit_circle, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_obj_limit_circle, LV_OBJ_FLAG_HIDDEN);
    }

    // 3. Turn Arrow & Distance (Left)
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
    } else {
        lv_label_set_text(s_lbl_turn_icon, "");
        lv_label_set_text(s_lbl_turn_dst, "");
    }

    // 4. Primary Alert (Top Right)
    if (state.alr > 0) {
        lv_label_set_text(s_lbl_alert_icon, get_alert_icon_symbol(state.alr));
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

    // 5. Upcoming Alert Signs from alrs array (Bottom Right)
    if (state.alrs_count > 0 && state.alrs[0].v > 0) {
        snprintf(buf, sizeof(buf), "%d", state.alrs[0].v);
        lv_label_set_text(s_lbl_alr1_val, buf);
        if (state.alrs[0].d >= 1000) {
            snprintf(buf, sizeof(buf), "%.1fK", state.alrs[0].d / 1000.0f);
        } else {
            snprintf(buf, sizeof(buf), "%dM", state.alrs[0].d);
        }
        lv_label_set_text(s_lbl_alr1_dst, buf);
        lv_obj_clear_flag(s_obj_alr1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(s_lbl_alr1_dst, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_obj_alr1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_lbl_alr1_dst, LV_OBJ_FLAG_HIDDEN);
    }

    if (state.alrs_count > 1 && state.alrs[1].v > 0) {
        snprintf(buf, sizeof(buf), "%d", state.alrs[1].v);
        lv_label_set_text(s_lbl_alr2_val, buf);
        if (state.alrs[1].d >= 1000) {
            snprintf(buf, sizeof(buf), "%.1fK", state.alrs[1].d / 1000.0f);
        } else {
            snprintf(buf, sizeof(buf), "%dM", state.alrs[1].d);
        }
        lv_label_set_text(s_lbl_alr2_dst, buf);
        lv_obj_clear_flag(s_obj_alr2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(s_lbl_alr2_dst, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_obj_alr2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_lbl_alr2_dst, LV_OBJ_FLAG_HIDDEN);
    }

    // 6. Bottom Street Name & Route Info
    if (strlen(state.st2) > 0) {
        snprintf(buf, sizeof(buf), "Rẽ: %s", state.st2);
        lv_label_set_text(s_lbl_street, buf);
    } else if (strlen(state.st) > 0) {
        lv_label_set_text(s_lbl_street, state.st);
    } else {
        lv_label_set_text(s_lbl_street, "");
    }

    // 7. Footer: ETA & Remaining Distance / Cấm Vượt
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
        lv_obj_set_style_text_color(s_lbl_eta_info, lv_color_hex(0xB0BEC5), 0);
    } else {
        lv_label_set_text(s_lbl_eta_info, "");
    }
}

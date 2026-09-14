#pragma once

#include <cstdint>
#include <lvgl.h>

const char* get_alert_name_vi(int alr_code);
void draw_turn_arrow_canvas(lv_obj_t* canvas, int trn_code, lv_color_t color);
void draw_alert_icon_canvas(lv_obj_t* canvas, int alr_code);

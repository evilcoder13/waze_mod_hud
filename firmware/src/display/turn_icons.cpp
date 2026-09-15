#include "turn_icons.h"

const char* get_alert_name_vi(int alr_code) {
    switch (alr_code) {
        case 1:  return "CẢNH SÁT";
        case 2:  return "CAM TỐC ĐỘ";
        case 3:  return "CAM ĐÈN ĐỎ";
        case 4:  return "NGUY HIỂM";
        case 5:  return "TAI NẠN";
        case 6:  return "KẸT XE";
        case 7:  return "ĐƯỜNG ĐÓNG";
        case 8:  return "GIẢM TỐC";
        case 9:  return "CẤM VƯỢT";
        case 10: return "HẾT CẤM VƯỢT";
        case 11: return "ĐƯỜNG SẮT";
        case 12: return "TRẠM THU PHÍ";
        case 13: return "XE DỪNG";
        case 14: return "CÔNG TRƯỜNG";
        case 15: return "Ổ GÀ";
        case 16: return "THỜI TIẾT";
        case 18: return "NGUY HIỂM";
        case 21: return "HẾT GIỚI HẠN";
        case 56: return "GỜ GIẢM TỐC";
        case 57: return "TRƯỜNG HỌC";
        default: return "CẢNH BÁO";
    }
}

void draw_turn_arrow_canvas(lv_obj_t* canvas, int trn_code, lv_color_t color) {
    if (!canvas) return;
    lv_canvas_fill_bg(canvas, lv_color_hex(0x000000), LV_OPA_TRANSP);

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = color;
    line_dsc.width = 6;
    line_dsc.round_start = 1;
    line_dsc.round_end = 1;

    lv_draw_rect_dsc_t tri_dsc;
    lv_draw_rect_dsc_init(&tri_dsc);
    tri_dsc.bg_color = color;
    tri_dsc.bg_opa = LV_OPA_COVER;
    tri_dsc.border_width = 0;

    switch (trn_code) {
        case 1:
        case 19: {
            lv_point_t p[2] = { {24, 42}, {24, 18} };
            lv_canvas_draw_line(canvas, p, 2, &line_dsc);
            lv_point_t head[3] = { {13, 21}, {24, 6}, {35, 21} };
            lv_canvas_draw_polygon(canvas, head, 3, &tri_dsc);
            break;
        }

        case 2:
        case 6: {
            lv_point_t p[3] = { {34, 42}, {34, 24}, {17, 24} };
            lv_canvas_draw_line(canvas, p, 3, &line_dsc);
            lv_point_t head[3] = { {20, 14}, {6, 24}, {20, 34} };
            lv_canvas_draw_polygon(canvas, head, 3, &tri_dsc);
            break;
        }

        case 3:
        case 7: {
            lv_point_t p[3] = { {14, 42}, {14, 24}, {31, 24} };
            lv_canvas_draw_line(canvas, p, 3, &line_dsc);
            lv_point_t head[3] = { {28, 14}, {42, 24}, {28, 34} };
            lv_canvas_draw_polygon(canvas, head, 3, &tri_dsc);
            break;
        }

        case 4:
        case 13:
        case 15: {
            lv_point_t p[3] = { {30, 42}, {30, 28}, {16, 16} };
            lv_canvas_draw_line(canvas, p, 3, &line_dsc);
            lv_point_t head[3] = { {24, 8}, {8, 10}, {14, 26} };
            lv_canvas_draw_polygon(canvas, head, 3, &tri_dsc);
            break;
        }

        case 5:
        case 14:
        case 16: {
            lv_point_t p[3] = { {18, 42}, {18, 28}, {32, 16} };
            lv_canvas_draw_line(canvas, p, 3, &line_dsc);
            lv_point_t head[3] = { {24, 8}, {40, 10}, {34, 26} };
            lv_canvas_draw_polygon(canvas, head, 3, &tri_dsc);
            break;
        }

        case 8:
        case 9:
        case 20: {
            lv_point_t p[5] = { {34, 42}, {34, 18}, {30, 9}, {18, 9}, {14, 26} };
            lv_canvas_draw_line(canvas, p, 5, &line_dsc);
            lv_point_t head[3] = { {6, 23}, {14, 39}, {22, 23} };
            lv_canvas_draw_polygon(canvas, head, 3, &tri_dsc);
            break;
        }

        case 10:
        case 11:
        case 12: {
            lv_draw_arc_dsc_t arc_dsc;
            lv_draw_arc_dsc_init(&arc_dsc);
            arc_dsc.color = color;
            arc_dsc.width = 5;
            lv_canvas_draw_arc(canvas, 24, 26, 14, 45, 330, &arc_dsc);

            lv_point_t head[3] = { {33, 12}, {41, 23}, {29, 21} };
            lv_canvas_draw_polygon(canvas, head, 3, &tri_dsc);
            break;
        }

        case 17: {
            lv_point_t pole[2] = { {16, 42}, {16, 9} };
            lv_canvas_draw_line(canvas, pole, 2, &line_dsc);
            lv_point_t flag[3] = { {16, 9}, {38, 19}, {16, 29} };
            lv_canvas_draw_polygon(canvas, flag, 3, &tri_dsc);
            break;
        }

        default:
            break;
    }
}

void draw_alert_icon_canvas(lv_obj_t* canvas, int alr_code) {
    if (!canvas) return;
    lv_canvas_fill_bg(canvas, lv_color_hex(0x000000), LV_OPA_TRANSP);

    lv_draw_rect_dsc_t r_dsc;
    lv_draw_rect_dsc_init(&r_dsc);

    lv_draw_line_dsc_t l_dsc;
    lv_draw_line_dsc_init(&l_dsc);

    if (alr_code == 1) {
        r_dsc.bg_color = lv_color_hex(0x1976D2);
        r_dsc.bg_opa = LV_OPA_COVER;
        r_dsc.radius = 6;
        lv_canvas_draw_rect(canvas, 4, 4, 32, 32, &r_dsc);

        r_dsc.bg_color = lv_color_hex(0xFFFFFF);
        r_dsc.radius = 2;
        lv_canvas_draw_rect(canvas, 10, 16, 20, 8, &r_dsc);
    }
    else if (alr_code == 2 || alr_code == 3) {
        r_dsc.bg_color = lv_color_hex(0x00E5FF);
        r_dsc.bg_opa = LV_OPA_COVER;
        r_dsc.radius = 4;
        lv_canvas_draw_rect(canvas, 3, 10, 21, 20, &r_dsc);

        r_dsc.bg_color = lv_color_hex(0x111118);
        r_dsc.radius = LV_RADIUS_CIRCLE;
        lv_canvas_draw_rect(canvas, 8, 15, 11, 11, &r_dsc);

        r_dsc.bg_color = lv_color_hex(0x00B0FF);
        r_dsc.radius = 2;
        lv_canvas_draw_rect(canvas, 7, 6, 8, 4, &r_dsc);

        r_dsc.bg_color = lv_color_hex(0x212121);
        r_dsc.radius = 3;
        lv_canvas_draw_rect(canvas, 27, 6, 10, 28, &r_dsc);

        r_dsc.bg_color = lv_color_hex(0xFF1744);
        r_dsc.radius = LV_RADIUS_CIRCLE;
        lv_canvas_draw_rect(canvas, 29, 8, 6, 6, &r_dsc);

        r_dsc.bg_color = (alr_code == 3) ? lv_color_hex(0x424242) : lv_color_hex(0xFFEA00);
        lv_canvas_draw_rect(canvas, 29, 17, 6, 6, &r_dsc);

        r_dsc.bg_color = lv_color_hex(0x00E676);
        lv_canvas_draw_rect(canvas, 29, 26, 6, 6, &r_dsc);
    }
    else if (alr_code == 4 || alr_code == 5 || alr_code == 18) {
        r_dsc.bg_color = lv_color_hex(0xFF9100);
        r_dsc.bg_opa = LV_OPA_COVER;
        lv_point_t tri[3] = { {20, 4}, {4, 34}, {36, 34} };
        lv_canvas_draw_polygon(canvas, tri, 3, &r_dsc);

        l_dsc.color = lv_color_hex(0x000000);
        l_dsc.width = 3;
        l_dsc.round_start = 1;
        l_dsc.round_end = 1;
        lv_point_t ex1[2] = { {20, 14}, {20, 24} };
        lv_canvas_draw_line(canvas, ex1, 2, &l_dsc);

        r_dsc.bg_color = lv_color_hex(0x000000);
        r_dsc.radius = LV_RADIUS_CIRCLE;
        lv_canvas_draw_rect(canvas, 19, 28, 3, 3, &r_dsc);
    }
    else if (alr_code == 6) {
        r_dsc.bg_color = lv_color_hex(0xD50000);
        r_dsc.radius = 4;
        lv_canvas_draw_rect(canvas, 4, 12, 32, 20, &r_dsc);

        r_dsc.bg_color = lv_color_hex(0xFFEB3B);
        r_dsc.radius = 1;
        lv_canvas_draw_rect(canvas, 8, 22, 6, 4, &r_dsc);
        lv_canvas_draw_rect(canvas, 26, 22, 6, 4, &r_dsc);
    }
    else if (alr_code == 9) {
        r_dsc.bg_color = lv_color_hex(0xFFFFFF);
        r_dsc.border_color = lv_color_hex(0xD50000);
        r_dsc.border_width = 4;
        r_dsc.radius = LV_RADIUS_CIRCLE;
        lv_canvas_draw_rect(canvas, 4, 4, 32, 32, &r_dsc);

        r_dsc.border_width = 0;
        r_dsc.radius = 2;
        r_dsc.bg_color = lv_color_hex(0xD50000);
        lv_canvas_draw_rect(canvas, 10, 14, 8, 12, &r_dsc);
        r_dsc.bg_color = lv_color_hex(0x000000);
        lv_canvas_draw_rect(canvas, 22, 14, 8, 12, &r_dsc);
    }
    else {
        r_dsc.bg_color = lv_color_hex(0xFF9100);
        r_dsc.radius = LV_RADIUS_CIRCLE;
        lv_canvas_draw_rect(canvas, 6, 6, 28, 28, &r_dsc);

        l_dsc.color = lv_color_hex(0xFFFFFF);
        l_dsc.width = 3;
        lv_point_t ex[2] = { {20, 12}, {20, 22} };
        lv_canvas_draw_line(canvas, ex, 2, &l_dsc);

        r_dsc.bg_color = lv_color_hex(0xFFFFFF);
        lv_canvas_draw_rect(canvas, 19, 25, 3, 3, &r_dsc);
    }
}


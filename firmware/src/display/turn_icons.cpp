#include "turn_icons.h"

const char* get_turn_symbol(int trn_code) {
    switch (trn_code) {
        case 1:  return LV_SYMBOL_UP;          // CONTINUE
        case 2:  return LV_SYMBOL_LEFT;        // LEFT
        case 3:  return LV_SYMBOL_RIGHT;       // RIGHT
        case 4:  return LV_SYMBOL_UP;          // SLIGHT_LEFT
        case 5:  return LV_SYMBOL_UP;          // SLIGHT_RIGHT
        case 6:  return LV_SYMBOL_LEFT;        // SHARP_LEFT
        case 7:  return LV_SYMBOL_RIGHT;       // SHARP_RIGHT
        case 8:  return LV_SYMBOL_REFRESH;     // U_TURN
        case 10: return LV_SYMBOL_LOOP;        // ROUNDABOUT
        case 11: return LV_SYMBOL_LOOP;        // ROUNDABOUT_LEFT
        case 12: return LV_SYMBOL_LOOP;        // ROUNDABOUT_RIGHT
        case 13: return LV_SYMBOL_LEFT;        // KEEP_LEFT
        case 14: return LV_SYMBOL_RIGHT;       // KEEP_RIGHT
        case 15: return LV_SYMBOL_LEFT;        // EXIT_LEFT
        case 16: return LV_SYMBOL_RIGHT;       // EXIT_RIGHT
        case 17: return LV_SYMBOL_OK;          // ARRIVE
        case 19: return LV_SYMBOL_UP;          // ROUNDABOUT_STRAIGHT
        case 20: return LV_SYMBOL_REFRESH;     // ROUNDABOUT_U_TURN
        default: return "";
    }
}

const char* get_alert_icon_symbol(int alr_code) {
    switch (alr_code) {
        case 1:  return LV_SYMBOL_WARNING; // Cảnh sát
        case 2:  return LV_SYMBOL_EYE_OPEN; // Camera tốc độ
        case 3:  return LV_SYMBOL_STOP;     // Camera phạt nguội / đèn đỏ
        case 4:  return LV_SYMBOL_WARNING; // Nguy hiểm
        case 5:  return LV_SYMBOL_CLOSE;   // Tai nạn
        case 6:  return LV_SYMBOL_SHUFFLE; // Kẹt xe
        case 8:  return LV_SYMBOL_DOWN;    // Giảm tốc độ
        case 9:  return LV_SYMBOL_MINUS;   // Cấm vượt
        case 12: return LV_SYMBOL_CHARGE;  // Trạm thu phí
        default: return LV_SYMBOL_WARNING;
    }
}

const char* get_alert_name_vi(int alr_code) {
    switch (alr_code) {
        case 1:  return "CẢNH SÁT";
        case 2:  return "CAMERA TỐC ĐỘ";
        case 3:  return "CAMERA ĐÈN ĐỎ";
        case 4:  return "NGUY HIỂM";
        case 5:  return "TAI NẠN";
        case 6:  return "KẸT XE";
        case 7:  return "ĐƯỜNG ĐÓNG";
        case 8:  return "GIẢM TỐC ĐỘ";
        case 9:  return "CẤM VƯỢT";
        case 10: return "HẾT CẤM VƯỢT";
        case 11: return "ĐƯỜNG SẮT";
        case 12: return "TRẠM THU PHÍ";
        case 13: return "XE DỪNG";
        case 14: return "CÔNG TRƯỜNG";
        case 15: return "Ổ GÀ";
        case 16: return "THỜI TIẾT";
        case 18: return "ĐOẠN NGUY HIỂM";
        case 56: return "GỜ GIẢM TỐC";
        case 57: return "TRƯỜNG HỌC";
        default: return "CẢNH BÁO";
    }
}

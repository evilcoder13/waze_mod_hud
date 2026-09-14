#include "turn_icons.h"

const char* get_turn_symbol(int trn_code) {
    switch (trn_code) {
        case 1:  return LV_SYMBOL_UP;          // Đi thẳng
        case 2:  return LV_SYMBOL_LEFT;        // Rẽ trái
        case 3:  return LV_SYMBOL_RIGHT;       // Rẽ phải
        case 4:  return LV_SYMBOL_LEFT;        // Chếch trái
        case 5:  return LV_SYMBOL_RIGHT;       // Chếch phải
        case 6:  return LV_SYMBOL_LEFT;        // Rẽ gắt trái
        case 7:  return LV_SYMBOL_RIGHT;       // Rẽ gắt phải
        case 8:  return "U";                   // Quay đầu (U-Turn chuẩn dạng chữ U lộn)
        case 9:  return "U";                   // Quay đầu phải
        case 10: return LV_SYMBOL_LOOP;        // Bùng binh
        case 11: return LV_SYMBOL_LEFT;        // Bùng binh rẽ trái
        case 12: return LV_SYMBOL_RIGHT;       // Bùng binh rẽ phải
        case 13: return LV_SYMBOL_LEFT;        // Giữ trái
        case 14: return LV_SYMBOL_RIGHT;       // Giữ phải
        case 15: return LV_SYMBOL_LEFT;        // Ra nhánh trái
        case 16: return LV_SYMBOL_RIGHT;       // Ra nhánh phải
        case 17: return LV_SYMBOL_OK;          // Đến nơi
        case 19: return LV_SYMBOL_UP;          // Vòng xuyến đi thẳng
        case 20: return "U";                   // Vòng xuyến quay đầu
        default: return "";
    }
}

const char* get_alert_icon_symbol(int alr_code) {
    switch (alr_code) {
        case 1:  return "CS";
        case 2:  return "CAM";
        case 3:  return "ĐÈN";
        case 4:  return "!";
        case 5:  return "TN";
        case 6:  return "KẸT";
        case 8:  return "TỐC";
        case 9:  return "CẤM";
        case 12: return "PHÍ";
        default: return "!";
    }
}

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
        case 56: return "GỜ GIẢM TỐC";
        case 57: return "TRƯỜNG HỌC";
        default: return "CẢNH BÁO";
    }
}

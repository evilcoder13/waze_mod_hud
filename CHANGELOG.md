# Changelog

Tất cả những thay đổi nổi bật của dự án **WazeHUD CYD** sẽ được lưu lại tại tài liệu này theo quy chuẩn [Semantic Versioning](https://semver.org/).

---

## [v1.1.0] - 2026-09-14

### ✨ Tính năng mới (Added)
- **Chạm lật gương (Mirror Flip / Windshield HUD)**:
  - Cho phép người dùng chạm nhẹ vào bất kỳ vị trí nào trên màn hình để chuyển đổi qua lại giữa chế độ **Nhìn thẳng (Direct View)** và **Lật gương phản chiếu kính lái (Windshield Mirror)**.
  - Tối ưu bắt tín hiệu ngắt cứng phần cứng qua chân `PIN_TOUCH_IRQ (GPIO 36)` kết hợp đọc thô `getTouchRaw()`, siêu nhạy với cảm ứng điện trở XPT2046.
- **Quản lý phiên bản và lưu trữ file nhị phân (Binary Releases Archive)**:
  - Lưu trữ cố định các bản factory image tại thư mục `bin/` theo format: `wazehud_cyd_vX.Y.Z_factory_0x0.bin`.
- **Tự động hóa CI/CD**:
  - Thiết lập GitHub Actions tự động biên dịch, sinh file factory all-in-one và tự dọn dẹp chỉ giữ lại 3 releases mới nhất.

### 🎨 Cải tiến giao diện (Changed)
- **Thiết kế lại toàn bộ giao diện màn hình ngang 320x240 theo phong cách Dashboard xe hơi hiện đại**:
  - Loại bỏ khối Alert Bar màu đỏ choán chỗ ban đầu, chuyển sang nền đen tuyền (`#000000`) tối giản.
  - Tốc độ xe to rõ, tự đổi màu đỏ rực khi quá tốc độ.
  - Biển hạn chế tốc độ tròn viền đỏ nền trắng chuẩn giao thông Việt Nam.
  - Cụm cảnh báo chính (Camera / CSGT) màu Cyan kèm cự ly mét.
  - Cụm cảnh báo phụ tiếp theo (`alrs`) hiển thị 2 biển báo kèm khoảng cách dạng widget tròn thu nhỏ.
  - Tên đường hiển thị màu vàng ánh kim (`#FFD54F`), hỗ trợ font tiếng Việt không nén, tự động cuộn chữ mượt mà.

---

## [v1.0.0] - 2026-09-11

### ✨ Khởi tạo ban đầu (Initial Release)
- Thiết lập kiến trúc FreeRTOS 2-Core chạy song song tác vụ Render (Core 1) và Giao thức/Mạng (Core 0).
- Hỗ trợ giao thức **HLP/1 (HUD Link Protocol)** nhận dữ liệu từ **Waze Mod** ([wazemod.io.vn](https://wazemod.io.vn/)).
- Hỗ trợ Bluetooth Classic SPP (`WazeHUD`) và WiFi WebSocket (`ws://<ip>:8765/hlp`).
- Cấu hình driver ST7789 cho board CYD bản 2 cổng USB (Type-C + Micro-USB).
- Xuất bản file nạp tích hợp `wazehud_cyd_factory_all_in_one_0x0.bin` (nạp ở offset `0x0`).

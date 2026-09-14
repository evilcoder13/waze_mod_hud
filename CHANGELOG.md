# Changelog

Tất cả những thay đổi nổi bật của dự án **WazeHUD CYD** sẽ được lưu lại tại tài liệu này theo quy chuẩn [Semantic Versioning](https://semver.org/).

## [v1.3.1] - 2026-09-15

### 🚀 Bê nguyên giao diện & cơ chế HUD từ bản tham khảo
- **Bộ mũi tên chỉ hướng Vector Waze (Thân dày, bo cong chuẩn)**:
  - Loại bỏ hoàn toàn các ký tự text thô `<, >, U`.
  - Triển khai bộ vẽ vector động bằng LVGL Canvas: Mũi tên đi thẳng nhọn dày, rẽ trái/phải uốn vuông góc 90°, rẽ chếch 45°, **quay đầu U-Turn chuẩn chữ U ngược có mũi tên chỉ xuống**, vòng xuyến bùng binh bo cong chuẩn.
- **Biểu tượng đồ họa Cảnh báo & Camera (Alert Canvas Graphic)**:
  - Vẽ trực tiếp hình ảnh Camera giao thông (Cyan Box + Lens tròn) kết hợp trụ đèn tín hiệu giao thông (Đỏ - Vàng - Xanh) cho Camera phạt nguội/đèn đỏ.
  - Biển tam giác vàng cảnh báo nguy hiểm/tai nạn, biển đỏ xe ô tô kẹt xe, biển tròn cấm vượt 2 xe.
- **Khắc phục triệt để cơ chế Lật màn hình HUD (Hardware + Touch)**:
  - **Nút bấm vật lý (Hardware BOOT button - GPIO 0)**: Bấm trực tiếp nút BOOT trên bo mạch CYD để chuyển đổi qua lại giữa màn hình thường và màn hình hắt kính (Windshield Mirror HUD) tức thì mà không cần chạm màn hình (học hỏi từ firmware mẫu).
  - **Cảm ứng thông minh**: Hỗ trợ chạm vào bất kỳ vị trí nào trên thanh tiêu đề trên cùng để lật màn hình, không bị trượt khi thao tác lái xe.

---

## [v1.3.0] - 2026-09-14

### 🐛 Sửa lỗi & Hoàn thiện (Fixes & Improvements)
- **Khắc phục triệt để lỗi ô vuông rỗng `[]`**:
  - Thay thế toàn bộ ký tự emoji Unicode chưa có trong font bằng nhãn text thuần Việt (`LẬT HUD`, `CAM ĐÈN ĐỎ`, `WazeHUD Connected`).
- **Sửa icon chỉ hướng quay đầu (U-Turn)**:
  - Loại bỏ ký hiệu 2 mũi tên xoay tròn `LV_SYMBOL_REFRESH`, thay bằng biểu tượng chữ `U` uốn ngược chuẩn Waze.
- **Sửa nút cảm ứng LẬT HUD góc trên bên phải**:
  - Hiệu chỉnh lại ma trận cảm ứng XPT2046 `setTouch({260, 3600, 350, 3450, 1})` cho board CYD 2.8" chiều ngang, bổ sung nhận diện chạm góc trên bên phải (`x >= 230, y <= 45`) đảm bảo bấm là ăn 100%.
- **Sửa tràn chữ & đè chữ ở hàng cảnh báo và card cảnh báo**:
  - Hàng trên: Rút gọn text cảnh báo vừa khít không đè nút HUD: `CAM ĐÈN ĐỎ (270 m)`.
  - Khung cảnh báo bên phải: Bố cục lại thành 2 dòng rõ ràng (Loại cảnh báo màu Cyan ở trên, Khoảng cách mét màu Vàng ở dưới), không còn bị đè chữ.
- **Tận dụng khoảng trắng giữa màn hình**:
  - Bổ sung **Đồng hồ thời gian thực** (Giờ hiện tại tính toán tự động từ Waze Telemetry) ngay dưới tốc độ và biển báo 60.
- **Thiết kế lại Footer chân màn hình (Split Footer)**:
  - Chia đôi 2 bên: Bên trái là tên đường hiện tại (`Phạm Hùng`), bên phải là thông tin chuyến đi (`21:04 • Còn 13 km`).
  - Co font nhỏ gọn 16px và nâng cao vị trí, không còn bị ngàm kẹp của đế giữ che mất.

---

## [v1.2.0] - 2026-09-14

### ✨ Tính năng mới (Added)
- **Nút chuyển đổi HUD góc phải trên Top Bar (`[ 🔄 HUD ]`)**:
  - Tách riêng chức năng lật gương thành nút bấm cảm ứng chuyên dụng trên thanh Alert Bar.
  - Chống chạm nhầm khi người dùng cầm nắm, lau màn hình hoặc căn chỉnh chân đế.
  - Tích hợp driver cảm ứng map tọa độ chạm chính xác vào hệ thống Event của LVGL.

### 🎨 Cải tiến giao diện (Changed)
- **Hàng 1 (Top Bar - 32px)**:
  - Tự động đổi sang nền đỏ tươi (`#D32F2F`) chữ trắng khi có cảnh báo khẩn cấp: `⚠ CAMERA ĐÈN ĐỎ (cách 100 m)`.
  - Giữ nền tối mờ tinh tế khi không có cảnh báo (`WazeHUD • Đã kết nối`).
- **Hàng 2 (Middle Dashboard - 164px) - Bố cục 3 cột cân đối**:
  - Cột trái: Mũi tên uốn lượn chỉ hướng (rẽ trái, phải, đi thẳng, quay đầu) + cự ly `dst` to rõ màu vàng + tên đường rẽ nhánh `st2`.
  - Cột giữa: Tốc độ xe màu trắng cực đại (tự chuyển đỏ khi quá tốc độ) + biển tròn báo giới hạn tốc độ viền đỏ nền trắng số đen.
  - Cột phải: Card cảnh báo chính (Camera / CSGT) nổi bật + 2 vòng tròn hiển thị cảnh báo phụ sắp tới (`alrs`).
- **Hàng 3 (Footer hành trình - 44px)**:
  - Tên đường hiện tại chữ trắng nền đen tuyền: `📍 Nguyễn Chí Thanh`.
  - Giờ đến nơi (ETA) và số km còn lại: `18:04 • Còn 7.0 km`.
  - Căn lề đẩy lên cao để không bị ngàm kẹp của đế đỡ trên xe che khuất.

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

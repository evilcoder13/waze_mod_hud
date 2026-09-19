# 🛠️ Hướng Dẫn Cài Đặt, Ghép Đôi & Vận Hành WazeHUD CYD

Tài liệu này hướng dẫn chi tiết từng bước từ khâu nạp firmware, ghép đôi điện thoại đến lắp đặt thực tế trên xe hơi.

---

## 1. Nạp Firmware (Flash Firmware)

### 1.1. Chuẩn bị
- Bo mạch **ESP32-2432S028 (CYD - Cheap Yellow Display)** 2.8 inch.
- Cáp nạp USB (Cắm vào cổng USB dữ liệu trên board kết nối với chip CH340).
- Trình duyệt **Google Chrome** hoặc **Microsoft Edge** trên máy tính.
- File firmware tải từ mục [Releases](https://github.com/evilcoder13/waze_mod_hud/releases):
  - File: `wazehud_cyd_factory_all_in_one_0x0.bin` (Bản tích hợp đầy đủ Bootloader, Partition, App0 và Firmware).

### 1.2. Các bước nạp qua Web Flasher
1. Kết nối mạch CYD với cổng USB máy tính.
2. Mở trình duyệt truy cập công cụ Web Flasher: [Espressif Web ESPTool](https://espressif.github.io/esptool-js/) hoặc [Adafruit ESPTool](https://adafruit.github.io/Adafruit_WebSerial_ESPTool/).
3. Chọn baudrate: **115200** (hoặc 460800).
4. **MẸO QUAN TRỌNG TRÁNH LỖI KẾT NỐI (Failed to initialize)**:
   - Tụ auto-reset trên mạch CYD có thể không kéo chân GPIO0 xuống kịp nhịp WebSerial.
   - **Nhấn và GIỮ chặt nút `BOOT`** trên bo mạch CYD.
   - Bấm **Connect** (hoặc chọn cổng COM) trên web.
   - Khi web bắt đầu nhận dạng chip ESP32 thành công, bạn **thả tay khỏi nút `BOOT`**.
5. Chọn file nạp:
   - **File**: `wazehud_cyd_factory_all_in_one_0x0.bin`
   - **Offset (Vị trí nạp)**: `0x0` (hoặc `0x0000`)
6. Bấm **Program / Flash** và đợi tiến trình chạy đủ 100%.
7. Bấm nút **RST** trên mạch để khởi động lại. Màn hình sẽ hiển thị logo WazeHUD và trạng thái chờ kết nối Bluetooth.

---

## 2. Ghép Đôi Bluetooth với Điện Thoại Android

1. Mở cài đặt **Bluetooth** trên điện thoại Android, bật Bluetooth và chọn **Tìm kiếm thiết bị / Ghép đôi thiết bị mới**.
2. Tìm thiết bị có tên: **`WazeHUD`**.
3. Bấm ghép đôi (Nếu hỏi mã PIN, nhập `1234` hoặc `0000`).
4. Khi kết nối thành công, thiết bị sẽ lưu trong danh sách thiết bị đã ghép đôi của điện thoại.

---

## 3. Cấu Hình Ứng Dụng Waze Mod

1. Cài đặt và mở ứng dụng **Waze Mod** từ [wazemod.io.vn](https://wazemod.io.vn/).
2. Vào biểu tượng **Cài đặt (Bánh răng)** -> cuộn xuống chọn mục **Cài đặt Mod (Mod Settings)**.
3. Tìm đến phần **HUD Link**:
   - **Kích hoạt HUD Link**: BẬT (ON).
   - **Loại kết nối (Connection Type)**: Chọn **Bluetooth Classic (SPP)**.
   - **Thiết bị Bluetooth (Select Device)**: Chọn **`WazeHUD`**.
   - **Tần số gửi (Stream Rate)**: 8 Hz (Mặc định).
4. Sau 1–2 giây, màn hình WazeHUD trên CYD sẽ tự động chuyển sang giao diện Dashboard đầy đủ với tốc độ, hướng rẽ và cảnh báo camera!

---

## 4. Lắp Đặt & Sử Dụng Thực Tế trên Taplo Xe Hơi

### 4.1. Lựa chọn chế độ hiển thị
WazeHUD hỗ trợ 2 chế độ hiển thị linh hoạt tùy vị trí đặt trên xe:

| Chế độ | Vị trí đặt | Cách hiển thị | Cách kích hoạt |
|---|---|---|---|
| **Nhìn Trực Tiếp (Dashboard Mode)** | Kẹp trên cửa gió điều hòa hoặc gắn giá đỡ điện thoại | Màn hình hiển thị thuận mắt, đọc trực tiếp trên LCD | Chạm nút `LẬT` để hiển thị chữ `LẬT` (chế độ bình thường) |
| **Hắt Kính Lái (Windshield HUD Mirror)** | Đặt nằm ngửa trên taplo xe, ngay sát chân kính lái phía trước vô lăng | Chữ và hình ảnh trên màn hình tự lật gương ngang. Khi phản chiếu lên kính lái, người lái thấy hình ảnh ảo nổi lên thuận mắt | Chạm nút `LẬT` góc trên hoặc bấm nút `BOOT` (GPIO 0) |

### 4.2. Mẹo tối ưu khi hắt kính lái
- **Miếng đệm chống trượt (Anti-slip Mat)**: Dán một miếng cao su chống trượt trên taplo để cố định board mạch, tránh bị trượt khi xe phanh hoặc cua gấp.
- **Tấm phim dán chống bóng mờ (HUD Reflective Film)**: Do kính lái xe hơi gồm 2 lớp kính ép (laminated safety glass), hình ảnh hắt lên có thể tạo bóng đôi nhẹ. Dán một miếng phim phản quang HUD nhỏ trong suốt (kèm theo các bộ HUD thương mại) tại điểm hắt kính sẽ giúp hình ảnh nét căng, sắc sảo như màn hình xe Mercedes/BMW.
- **Nguồn điện**: Cắm cáp Type-C vào tẩu sạc xe hơi (nguồn ra khuyến nghị 5V - 1A đến 2A) để đảm bảo board chạy ổn định cả ngày lẫn đêm.

---

👉 Xem thêm: **[Mô hình Hoạt động WazeHUD](./Model-Waze-Bluetooth-HUD-Mirror.md)**

# 📖 WazeHUD CYD Wiki

Chào mừng bạn đến với trang tài liệu chuyên sâu (**Wiki**) của dự án **WazeHUD CYD** — Thiết bị hiển thị thông tin dẫn đường rời dành riêng cho bản đồ **Waze Mod** trên phần cứng **ESP32-2432S028 (CYD)**.

---

## 📚 Mục lục tài liệu (Table of Contents)

1. [🏠 Trang chủ (Home)](./Home.md)
2. [🔄 Mô hình Hoạt động Toàn diện: Waze Mod ➔ Bluetooth SPP ➔ HUD ➔ Kính lái](./Model-Waze-Bluetooth-HUD-Mirror.md)
   * Tổng quan luồng dữ liệu & kiến trúc FreeRTOS 2 nhân.
   * Giao thức HLP/1 (HUD Link Protocol) thời gian thực (8 Hz).
   * Cơ chế quang học hắt kính (Windshield Optical Reflection) & Lật gương ngang (Horizontal Flip).
   * Bộ giải mã cảnh báo giao thông đặc thù Việt Nam (Camera phạt nguội, cấm vượt vg).
3. [🛠️ Hướng dẫn Cài đặt, Ghép đôi & Vận hành](./Setup-and-Usage-Guide.md)
   * Hướng dẫn nạp firmware qua Web Flasher (khắc phục lỗi bootloader).
   * Ghép đôi Bluetooth với điện thoại Android.
   * Kích hoạt HUD Link trong Waze Mod (wazemod.io.vn).
   * Lắp đặt thực tế trên taplo xe hơi: Nhìn trực tiếp (Dashboard) vs Hắt kính lái (HUD Mirror).

---

## 🎯 Mục tiêu Dự án

Dự án ra đời nhằm giải quyết bài toán: **Lái xe tập trung, không phân tâm nhìn điện thoại, nhưng vẫn nắm trọn mọi cảnh báo giao thông trọng yếu**:
- ✅ **Không cần bật sáng màn hình điện thoại liên tục**: Giảm nhiệt độ pin và tăng tuổi thọ điện thoại khi chạy xe đường dài.
- ✅ **Giao diện tối giản, trực quan**: Tập trung vào Tốc độ xe, Biển giới hạn tốc độ, Mũi tên rẽ nhánh và Camera phạt nguội.
- ✅ **Chi phí cực thấp**: Chỉ cần một bo mạch màn hình ESP32 CYD (giá ~150.000 - 200.000 VNĐ).
- ✅ **Lật kính lái thông minh**: Đặt ngửa màn hình lên taplo để hắt thông số trực tiếp lên kính lái như các dòng xe sang hiện đại.

---

👉 Xem tiếp: **[Mô hình Hoạt động: Waze Mod ➔ Bluetooth ➔ HUD ➔ Kính lái](./Model-Waze-Bluetooth-HUD-Mirror.md)**

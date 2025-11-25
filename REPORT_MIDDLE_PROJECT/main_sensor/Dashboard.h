#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <TFT_eSPI.h>

// Tham chiếu đến biến tft đã khai báo ở file chính
extern TFT_eSPI tft;

// Định nghĩa màu sắc tùy chỉnh cho đẹp hơn
#define C_BG        0x0000 // Nền đen
#define C_CARD      0x10A2 // Màu xám đậm cho khung
#define C_TEMP      0xF800 // Đỏ
#define C_HUM       0x07FF // Xanh dương
#define C_GAS_SAFE  0x07E0 // Xanh lá
#define C_GAS_WARN  0xF800 // Đỏ cam

// Ngưỡng cảnh báo Gas (Bạn chỉnh lại cho khớp với cảm biến nhà bạn)
#define GAS_LIMIT   1500

// --- HÀM 1: VẼ GIAO DIỆN TĨNH (Chỉ chạy 1 lần trong setup) ---
void drawDashboardInterface() {
  tft.fillScreen(C_BG);
  
  // 1. Vẽ Header
  tft.fillRect(0, 0, 320, 30, 0x2124); // Thanh tiêu đề màu xám
  tft.setTextColor(TFT_WHITE, 0x2124);
  tft.setTextDatum(MC_DATUM); // Căn giữa
  tft.drawString("HE THONG GIAM SAT", 160, 15, 2);

  // 2. Khung Nhiệt độ (Bên trái)
  tft.drawRoundRect(10, 40, 145, 100, 5, C_TEMP); // Viền đỏ
  tft.setTextColor(C_TEMP, C_BG);
  tft.drawString("NHIET DO", 82, 60, 2);
  tft.drawString("oC", 130, 100, 2);

  // 3. Khung Độ ẩm (Bên phải)
  tft.drawRoundRect(165, 40, 145, 100, 5, C_HUM); // Viền xanh
  tft.setTextColor(C_HUM, C_BG);
  tft.drawString("DO AM", 237, 60, 2);
  tft.drawString("%", 285, 100, 2);

  // 4. Khung Khí Gas (Bên dưới)
  tft.drawRoundRect(10, 150, 300, 80, 5, TFT_WHITE);
  tft.setTextColor(TFT_WHITE, C_BG);
  tft.drawString("CHAT LUONG KHONG KHI", 160, 165, 2);
}

// --- HÀM 2: CẬP NHẬT SỐ LIỆU (Chạy trong loop) ---
void updateDashboard(float temp, float hum, int gasValue) {
  
  // --- CẬP NHẬT NHIỆT ĐỘ ---
  // Xóa số cũ bằng cách vẽ hình chữ nhật nền đen đè lên vị trí số
  // (Cách này nhanh hơn fillScreen và đỡ nháy hơn)
  tft.setTextColor(TFT_WHITE, C_BG); 
  tft.setTextPadding(100); // Tự động xóa nền cũ rộng 100px
  tft.setTextDatum(MC_DATUM);
  
  // Vẽ số nhiệt độ (Font số 6 hoặc 7 là font led 7 đoạn rất đẹp)
  tft.drawFloat(temp, 1, 75, 100, 6); 

  // --- CẬP NHẬT ĐỘ ẨM ---
  tft.setTextColor(TFT_WHITE, C_BG);
  tft.drawFloat(hum, 1, 230, 100, 6);

  // --- CẬP NHẬT GAS ---
  // Thanh bar hiển thị mức độ gas
  int barWidth = map(gasValue, 0, 4095, 0, 280); // Map giá trị sang độ rộng pixel
  if (barWidth > 280) barWidth = 280;

  // Logic màu sắc cho Gas
  uint16_t gasColor = C_GAS_SAFE;
  String gasStatus = "AN TOAN";
  
  if (gasValue > GAS_LIMIT) {
    gasColor = C_GAS_WARN;
    gasStatus = "CANH BAO!";
  }

  // Vẽ thanh Bar
  tft.fillRect(20, 190, 280, 10, 0x2124); // Vẽ nền thanh bar (xám tối)
  tft.fillRect(20, 190, barWidth, 10, gasColor); // Vẽ mức gas thực tế

  // Vẽ chữ trạng thái
  tft.setTextPadding(200);
  tft.setTextColor(gasColor, C_BG);
  tft.drawString(gasStatus + " (" + String(gasValue) + ")", 160, 215, 4);
  
  // Reset padding
  tft.setTextPadding(0);
}

#endif
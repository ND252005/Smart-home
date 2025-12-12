#include <WiFi.h>
#include <Preferences.h> // Thư viện lưu dữ liệu vào bộ nhớ Flash
#include "mbedtls/md.h"

Preferences preferences;
String myDeviceID = "";

// Hàm tạo chuỗi ngẫu nhiên (UUID giả lập)
String generateRandomID() {
  String randomStr = "";
  // Dùng nhiễu từ chân Analog chưa kết nối để tạo seed ngẫu nhiên
  randomSeed(analogRead(34) + micros()); 
  
  const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  for (int i = 0; i < 16; i++) {
    randomStr += chars[random(0, 36)];
  }
  return randomStr;
}

// Hàm băm SHA-256 (để làm đẹp ID nếu muốn)
String sha256(String payload) {
  byte shaResult[32];
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char *) payload.c_str(), payload.length());
  mbedtls_md_finish(&ctx, shaResult);
  mbedtls_md_free(&ctx);
  String hashStr = "";
  for(int i= 0; i< sizeof(shaResult); i++) {
    char str[3]; sprintf(str, "%02x", shaResult[i]); hashStr += str;
  }
  return hashStr;
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Chờ ổn định

  // 1. Mở vùng nhớ tên là "device-config" (chế độ false = Read/Write)
  preferences.begin("device-config", false);

  // 2. Kiểm tra xem đã có ID lưu trong máy chưa?
  // Nếu chưa có (trả về rỗng), thì tạo mới.
  String savedID = preferences.getString("myID", "");

  if (savedID == "") {
    Serial.println(">>> New Device Detected! Generating new ID...");
    
    // Tạo ID ngẫu nhiên
    String rawID = generateRandomID(); 
    // Băm nó để nhìn chuyên nghiệp hơn (hoặc dùng luôn rawID cũng được)
    String hashedID = sha256(rawID).substring(0, 16); 
    
    // Lưu vào Flash
    preferences.putString("myID", hashedID);
    myDeviceID = hashedID;
    
    Serial.println(">>> ID Generated & Saved to Flash.");
  } else {
    Serial.println(">>> Existing Device. Loading ID from Flash...");
    myDeviceID = savedID;
  }

  // Đóng preferences
  preferences.end();

  Serial.println("----------------------------------------");
  Serial.println("FINAL DEVICE ID: " + myDeviceID);
  Serial.println("----------------------------------------");
  
  // Code kết nối WiFi và gửi lên Server dùng myDeviceID ở đây...
}

void loop() {}
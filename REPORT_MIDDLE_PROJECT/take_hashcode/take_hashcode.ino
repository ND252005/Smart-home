#include <WiFi.h>
#include "mbedtls/md.h"

// Hàm băm chuỗi thành SHA-256
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
    char str[3];
    sprintf(str, "%02x", shaResult[i]);
    hashStr += str;
  }
  return hashStr;
}

void setup() {
  Serial.begin(115200);
  
  // 1. Lấy MAC Address
  String mac = WiFi.macAddress();
  
  // 2. Chuẩn hóa: Xóa dấu hai chấm
  mac.replace(":", ""); 
  
  // 3. Hashing
  String hashedMac = sha256(mac);
  
  Serial.println("Original MAC: " + mac);
  Serial.println("Hashed MAC (Full): " + hashedMac);
  
  // 4. Lấy 12 ký tự đầu để làm HashID gửi lên server
  String shortHash = hashedMac.substring(0, 12);
  Serial.println("Short Hash ID: " + shortHash);
}

void loop() {}
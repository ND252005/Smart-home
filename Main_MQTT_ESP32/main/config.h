    #ifndef CONFIG_H
#define CONFIG_H

#define verion "v1.0.0"

// ----------- WiFi SETTINGS -----------
#define WIFI_SSID       "B5-P5.14"
#define WIFI_PASSWORD   "66668888"

#define WIFI_SSID_AP       "ESP_AP"
#define WIFI_PASSWORD_AP   "12345678"
#define TRY_CONNECT_TIMEOUT 300

// ----------- MQTT SETTINGS -----------
#define MQTT_HOST       "anhpn.ddns.net"   // domain or IP
#define MQTT_PORT       1884
#define MQTT_USER       "ductran"
#define MQTT_PASSWORD   "Duc@2025"
#define MQTT_CLIENT_ID  "ESP32_cua_Duc_day"

// ----------- Version code -----------
#define verion "v1.0.0"

//----------FastAPI Endpoint----------
String SERVER_URL = "http://192.168.79.24:8000";
// ✅ Change this to match your API

// Hashcode (can be static or changed later)
#define DEVICE_HASHCODE "my_hash_123"

#endif

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

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

// ----------- Device code -----------
#define DEVICE_LIMITS 4
extern String SERVER_URL;
extern String DEVICE_HASHCODE;

extern int gates[DEVICE_LIMITS];
extern int state[DEVICE_LIMITS];
//----------- API SETTNGS -----------
#define PING_TIMEOUT 300000
#endif

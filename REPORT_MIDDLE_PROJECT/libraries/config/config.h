#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define verion "v1.0.0"

// ----------- WiFi SETTINGS -----------
#define WIFI_SSID       "B5-P5.14"
#define WIFI_PASSWORD   "66668888"

#define WIFI_SSID_AP       "ESP32_sensor"
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
extern String SERVER_URL;
extern String HASHCODE;
extern int device_pin[2];
extern int read_pin[2];
extern bool state_device[2];
#define DEVICE_CONTROL_LIMITS 2
#define SAMPLE_TIMEOUT 1000

//----------- API SETTNGS -----------
#define PING_TIMEOUT            300000
#define SENSOR_DATA_TIMEOUT     5000
#endif

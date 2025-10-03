#include <Firebase_ESP_Client.h>

#include <Wire.h>
#include <WiFi.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <MPU6050_tockn.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7*3600, 60000); // GMT+7, refresh 60s

MPU6050 mpu6050(Wire);

#define WIFI_SSID "B5-P5.14"
#define WIFI_PASSWORD "66668888"
#define API_KEY "AIzaSyB5ID-GFh2az-j0zltGLiUpR0vuGKV_CSk"
#define DATABASE_URL "https://control-esp32-8bc83-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define Led_pin 22
#define SDA 23
#define SCL 19

FirebaseData fbdo_write, fbdo_read;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long last_send_data_millis = 0;
bool signupOK = false;
float imu_data_angle_X = 0;
float imu_data_angle_Y = 0;
float imu_data_angle_Z = 0;

int pwm_val = 0;
// int ledChannel = 0; // kênh PWM

void setup() {
    Serial.begin(115200);
    Wire.begin(SDA, SCL);
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true);

    pinMode(Led_pin, OUTPUT);


    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("Connecting to wifi...");
    while(WiFi.status() != WL_CONNECTED) {
        Serial.print("."); delay(300);
    }
    Serial.println("OK");
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    //---NTP: đồng bộ thời gian trước---
    timeClient.begin();
    Serial.println("Syncing time with NTP...");
    while (!timeClient.update()) {
        timeClient.forceUpdate();   // bắt buộc update
        delay(500);
    }
    Serial.print("Time synced: ");
    Serial.println(timeClient.getFormattedTime());

    //---Firebase---
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    if(Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("Sign Up OK");
        signupOK = true;
    } else {
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }
    config.token_status_callback = tokenStatusCallback;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    if(!Firebase.RTDB.beginStream(&fbdo_read, "LED/analog"))
        Serial.printf("stream read begin error, %s\n\n", fbdo_read.errorReason().c_str());
}


void debug() {
        if(Firebase.RTDB.setFloat(&fbdo_write, "IMU/data_angle_X", imu_data_angle_X)) {
            Serial.println(); Serial.print(imu_data_angle_X);
            Serial.print(" Successfully saved to: " + fbdo_write.dataPath());
            Serial.println("("+ fbdo_write.dataType() +")");
        } else {
            Serial.println("Failed: " + fbdo_write.errorReason());
        }
        if(Firebase.RTDB.setFloat(&fbdo_write, "IMU/data_angle_Y", imu_data_angle_Y)) {
            Serial.print(imu_data_angle_Y);
            Serial.print(" Successfully saved to: " + fbdo_write.dataPath());
            Serial.println("("+ fbdo_write.dataType() +")");
        } else {
            Serial.println("Failed: " + fbdo_write.errorReason());
        }
        if(Firebase.RTDB.setFloat(&fbdo_write, "IMU/data_angle_Z", imu_data_angle_Z)) {
            Serial.print(imu_data_angle_Z);
            Serial.print(" Successfully saved to: " + fbdo_write.dataPath());
            Serial.println("("+ fbdo_write.dataType() +")");
        } else {
            Serial.println("Failed: " + fbdo_write.errorReason());
        }
}

void saveFloat(String path, float value) {
    if (!Firebase.RTDB.setFloat(&fbdo_write, path.c_str(), value)) {
        Serial.println("Failed to save " + path + ": " + fbdo_write.errorReason());
    }
}

void loop() {
    mpu6050.update();
    if(Firebase.ready() && signupOK && (millis() - last_send_data_millis > 5000 || last_send_data_millis == 0)) {
        timeClient.update();
        last_send_data_millis = millis();
        // -----------------------------Save data to a RTDB
        imu_data_angle_X = mpu6050.getAngleX();
        imu_data_angle_Y = mpu6050.getAngleY();
        imu_data_angle_Z = mpu6050.getAngleZ();
        
        //In góc vào monitor
        //debug();
        
        //Lưu thời gian realtime
        saveFloat("IMU/Realtime/angle_X", imu_data_angle_X);
        saveFloat("IMU/Realtime/angle_Y", imu_data_angle_Y);
        saveFloat("IMU/Realtime/angle_Z", imu_data_angle_Z);

        // Lấy thời gian thực
        unsigned long epochTime = timeClient.getEpochTime();  // giây từ 1970
        struct tm *ptm = gmtime((time_t *)&epochTime);

        char datetime[25];
        sprintf(datetime, "%04d-%02d-%02d %02d:%02d:%02d",
                (ptm->tm_year + 1900),
                (ptm->tm_mon + 1),
                ptm->tm_mday,
                ptm->tm_hour,
                ptm->tm_min,
                ptm->tm_sec);

        // ----------------------------- Lưu log (thêm vào IMU/logs)
        FirebaseJson json;
        json.set("angle_X", imu_data_angle_X);
        json.set("angle_Y", imu_data_angle_Y);
        json.set("angle_Z", imu_data_angle_Z);
        json.set("datetime", datetime);  // hoặc thay bằng giờ thực từ NTP

        if (Firebase.RTDB.pushJSON(&fbdo_write, "IMU/logs", &json)) {
            Serial.println("Log pushed: " + fbdo_write.dataPath());
        } else {
            Serial.println("Log push failed: " + fbdo_write.errorReason());
        }

    }
  //--------------------------------------Read data change from firebase to control devices connect with ESP32
    if(Firebase.ready() && signupOK) {
        if(!Firebase.RTDB.readStream(&fbdo_read))
            Serial.printf("Stream read read error, $s\n\n", fbdo_read.errorReason().c_str());
        if(fbdo_read.streamAvailable()) {
            if(fbdo_read.dataType() == "int") {
                pwm_val = fbdo_read.intData();
                Serial.println("Successful READ from" + fbdo_read.dataPath() + ": " + pwm_val + " (" + fbdo_read.dataType() + ") ");
                analogWrite(Led_pin, 255 - pwm_val);
            }
        }
    }

}

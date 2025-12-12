#include <WiFi.h>
#include <WiFiManager.h>
//#include <WiFiUdp.h>
//#include <EEPROM.h>
#include <HTTPProtocol.h>
#include <ArduinoJson.h>
#include <config.h>
#include <MQTTProtocol.h>
#include "DHT.h"

MQTT* mqtt;
HTTPProtocol* http;



// --- BIẾN TOÀN CỤC ---
//Biến thời gian 
unsigned long last_time_control_status 	= 0;
unsigned long last_time_healthcheck  	= 0;

// Biến đếm trong ngắt (volatile)
volatile unsigned long changeCount1 = 0; 
volatile unsigned long changeCount2 = 0; 

// Biến lưu tần số hiển thị
bool last_state_1 = 0; 
bool last_state_2 = 0;

bool current_state_1 = 0; 
bool current_state_2 = 0;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; 

// --- HÀM NGẮT (ISR) ---
// Ngắt cho Kênh 1
void IRAM_ATTR onChange1() {
  changeCount1++;
}

// Ngắt cho Kênh 2
void IRAM_ATTR onChange2() {
  changeCount2++;
}

void setup() {
	Serial.begin(115200);
	//define pin out 
	for(int i = 0; i < DEVICE_CONTROL_LIMITS; i++) {
		pinMode(device_pin[i], OUTPUT);
		pinMode(read_pin[i], INPUT);
	}
// --- SETUP KÊNH 1 ---
  attachInterrupt(digitalPinToInterrupt(read_pin[0]), onChange1, RISING);
//   digitalWrite(pinRelay1, LOW); 

  // --- SETUP KÊNH 2 ---
  attachInterrupt(digitalPinToInterrupt(read_pin[1]), onChange2, RISING);
//   digitalWrite(pinRelay2, LOW);

	//Define wifi used Wifimanager 
	//WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFiManager wm;
  if (!wm.autoConnect("ESP", "12345678")) {
    Serial.println("Connected is not successful, esp will restart...");
    delay(3000);
    ESP.restart();
  }

	Serial.print("Connecting to WiFi ");
  Serial.println(WiFi.localIP());
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	 	delay(1000);
		mqtt = new MQTT(MQTT_HOST, MQTT_PORT);
		mqtt->setCredentials(MQTT_USER, MQTT_PASSWORD);
		mqtt->begin();

}

void loop() {
  // put your main code here, to run repeatedly:
	if(mqtt) mqtt->loop();
	if(millis() - last_time_control_status > SAMPLE_TIMEOUT) {
		control_device_update();
		last_time_control_status = millis();
	}
	if(millis() - last_time_healthcheck > PING_TIMEOUT) {
		mqtt->PublishStateController("device_1", current_state_1, "device_2", current_state_2);
		last_time_healthcheck = millis();
	}

}

void control_device_update() {
	portENTER_CRITICAL(&mux);
    unsigned long count1 = changeCount1; changeCount1 = 0;
    unsigned long count2 = changeCount2; changeCount2 = 0;
    portEXIT_CRITICAL(&mux);
	
	current_state_1 = (count1>100) ? true : false;
	current_state_2 = (count2>100) ? true : false;
	
	if(last_state_1 != current_state_1 or last_state_2 != current_state_2) {
		mqtt->PublishStateController("device_1", current_state_1, "device_2", current_state_2);
		last_state_1 = current_state_1;
		last_state_2 = current_state_2;
	}
}

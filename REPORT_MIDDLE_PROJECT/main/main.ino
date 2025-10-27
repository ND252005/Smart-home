#include <WiFi.h>
//#include <WiFiManager.h>
//#include <WiFiUdp.h>
//#include <EEPROM.h>
#include <HTTPProtocol.h>
#include <ArduinoJson.h>
#include <config.h>
#include <MQTTProtocol.h>

MQTT* mqtt;
HTTPProtocol* http;



void setup() {
	Serial.begin(115200);
	for(int i = 0; i < DEVICE_LIMITS; i++) {
		pinMode(gates[i], OUTPUT);
	}
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.print("Connecting to WiFi ");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	http = new HTTPProtocol(SERVER_URL.c_str());

	String esp_name = WiFi.macAddress();
	if (http->postRegisterDevice(esp_name)) {
		Serial.println("Device registered successfully!");
		Serial.print("Hashcode: ");
		Serial.println(http->getHashcode());
		DEVICE_HASHCODE = http->getHashcode();

	} else {
		Serial.println("Failed to register device");
	}
	if(DEVICE_HASHCODE != "" ) {
	 delay(1000);
		mqtt = new MQTT(MQTT_HOST, MQTT_PORT, DEVICE_HASHCODE);
		mqtt->setCredentials(MQTT_USER, MQTT_PASSWORD);
		mqtt->begin();
	}

}

void loop() {
  // put your main code here, to run repeatedly:
	if(mqtt) mqtt->loop();

}

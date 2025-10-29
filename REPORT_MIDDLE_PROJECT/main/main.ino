#include <WiFi.h>
#include <WiFiManager.h>
//#include <WiFiUdp.h>
//#include <EEPROM.h>
#include <HTTPProtocol.h>
#include <ArduinoJson.h>
#include <config.h>
#include <MQTTProtocol.h>

MQTT* mqtt;
HTTPProtocol* http;

unsigned long prev_time = 0;

void setup() {
	Serial.begin(115200);
	//define pin out 
	for(int i = 0; i < DEVICE_LIMITS; i++) {
		pinMode(gates[i], OUTPUT);
	}

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

	http = new HTTPProtocol(SERVER_URL.c_str());

	String esp_name = WiFi.macAddress();
	if (http->postRegisterESP(esp_name)) {
		Serial.println("Device registered successfully!");
		Serial.print("Hashcode: ");
		Serial.println(http->getHashcode());
		DEVICE_HASHCODE = http->getHashcode();
		prev_time = millis();
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
	if(prev_time && millis() - prev_time > PING_TIMEOUT) {
		http->healthCheckESP();
		prev_time = millis();
	}

}

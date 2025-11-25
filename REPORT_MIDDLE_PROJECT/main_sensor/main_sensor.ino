#include <WiFi.h>
#include <WiFiManager.h>
//#include <WiFiUdp.h>
//#include <EEPROM.h>
#include <HTTPProtocol.h>
#include <ArduinoJson.h>
#include <config.h>
#include <MQTTProtocol.h>
#include "DHT.h"
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
#include "Dashboard.h"

MQTT* mqtt;
HTTPProtocol* http;

#define DHTTYPE DHT11

#define DHT_PIN 4     // Digital pin connected to the DHT sensor
#define MQ2_PIN 35    //detect gas sensor

DHT dht(DHT_PIN, DHTTYPE); //khai báo thư viện dht


// const int GAS_THRESHOLD_ALARM = 1200; //mức gas cảnh báo
// const int GAS_THRESHOLD_DANGER = 2500; //mức gas báo động



unsigned long prev_time_sensor_status  	= 0;
unsigned long prev_time_healthcheck  	= 0;

void setup() {

	Serial.begin(115200);

	// Khởi động màn hình
	tft.init();
	tft.setRotation(1); // Xoay ngang (Landscape)
	
	// Vẽ khung giao diện (Chỉ gọi 1 lần)
	drawDashboardInterface();

	pinMode(MQ2_PIN, INPUT);
	dht.begin();

	//Define wifi used Wifimanager 
	//WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	WiFiManager wm;
	if (!wm.autoConnect(WIFI_SSID_AP, WIFI_PASSWORD_AP)) {
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
		prev_time_sensor_status = millis();
		prev_time_healthcheck = millis();
	} else {
		Serial.println("Failed to register device");
	}
	if(DEVICE_HASHCODE != "") {
	 delay(1000);
		mqtt = new MQTT(MQTT_HOST, MQTT_PORT, DEVICE_HASHCODE);
		mqtt->setCredentials(MQTT_USER, MQTT_PASSWORD);
		mqtt->begin();
	}

}

void loop() {

	if(mqtt) mqtt->loop();

	if(prev_time_sensor_status && millis() - prev_time_sensor_status > SENSOR_DATA_TIMEOUT) {
		readSensor();
		prev_time_sensor_status = millis();
	}
	if(prev_time_healthcheck && millis() - prev_time_healthcheck > PING_TIMEOUT) {
		http->healthCheckESP();
		prev_time_healthcheck = millis();
	}
}

void readSensor() {
	int gas_dectect = analogRead(MQ2_PIN);
	int humidity = dht.readHumidity();
	int temperature = dht.readTemperature();
	updateDashboard(temperature, humidity, gas_dectect);
	mqtt->PublishStateSensor(temperature, humidity, gas_dectect);
}


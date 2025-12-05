#include <WiFi.h>
#include <WiFiManager.h>
//#include <WiFiUdp.h>
//#include <EEPROM.h>
// #include <HTTPProtocol.h>
#include <ArduinoJson.h>
#include <config.h>
#include <MQTTProtocol.h>
#include "DHT.h"
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
#include "Dashboard.h"

MQTT* mqtt;
// HTTPProtocol* http;

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


	prev_time_sensor_status = millis();
	prev_time_healthcheck = millis();

	 	delay(1000);
		mqtt = new MQTT(MQTT_HOST, MQTT_PORT);
		mqtt->setCredentials(MQTT_USER, MQTT_PASSWORD);
		mqtt->begin();
}

void loop() {

	if(mqtt) mqtt->loop();

	if(prev_time_sensor_status && millis() - prev_time_sensor_status > SENSOR_DATA_TIMEOUT) {
		readSensor();
		prev_time_sensor_status = millis();
	}
	if(prev_time_healthcheck && millis() - prev_time_healthcheck > PING_TIMEOUT) {
		mqtt->Ping2Server();
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


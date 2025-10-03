#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <HTTPClient.h>
#include "config.h"
#include "mbedtls/base64.h"

#define EEPROM_SIZE 1
#define ADDR_RESET 0

WiFiClient espClient;
PubSubClient mqttClient(espClient);

int gates_in[4] = {0, 0, 0, 0};
int gates_out[4] = {0, 0, 0, 0};
bool status[4] = {0, 0, 0, 0};

unsigned long time_check_health = 5000;

void gates_status_setup() {
	for(int i = 0; i < 4; i++) {
		pinMode(gates_out[i], OUTPUT);
		pinMode(gates_in[i], INPUT);
	}
}

void connect_to_WiFi() {
	WiFiManager wm;
	wm.setConnectTimeout(TRY_CONNECT_TIMEOUT);
	bool reset_connect = EEPROM.read(ADDR_RESET);
	if(reset_connect != 0 && reset_connect != 1) reset_connect = false;
	if(reset_connect) {
		wm.resetSettings();
		reset_connect = false;
		EEPROM.write(ADDR_RESET, reset_connect);
		EEPROM.commit();
	}
	if (!wm.autoConnect(WIFI_SSID_AP, WIFI_PASSWORD_AP)) {
		Serial.println("Failed to connect and no timeout hit");
		ESP.restart();
	}
	Serial.println("\n WiFi connected!");
	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());
}

String get_device_ID() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  unsigned char output[32];
  size_t out_len;

  mbedtls_base64_encode(output, sizeof(output), &out_len, mac, 6);
  return String((char*)output);
}

String create_topic() {
	return get_device_ID();
}
String topic_str = create_topic();
const char* TOPIC = topic_str.c_str();

// void connectToMQTT() {
//   while (!mqttClient.connected()) {
//     Serial.print("Connecting to MQTT... ");
//     if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
//       	Serial.println(" Connected to MQTT");
// 		mqttClient.subscribe(TOPIC);
// 	} else {
// 		Serial.print("Failed, rc=");
// 		Serial.print(mqttClient.state());
// 		Serial.println(" -> retrying in 2 seconds");
// 		delay(2000);
//     }
//   }
// }

// void mqtt_callback(char* topic, byte* payload, unsigned int length) {
// 	Serial.print("Message arrived on topic: ");
// 	Serial.println(topic);
// 	Serial.print("Payload: ");
// 	for (int i = 0; i < length; i++) {
// 		Serial.print((char)payload[i]);
// 	}
// 	Serial.println();
// }

// void mqtt_setup() {
// 	mqttClient.setServer(MQTT_HOST, MQTT_PORT);
// 	mqttClient.setCallback(mqtt_callback);	
// }

// void update() {
// 	if (!mqttClient.connected()) {
// 		connectToMQTT();
// 	}
// 	mqttClient.loop();
// 	for(int i = 0; i < 4; i++) {
// 		status[i] = analogRead(gates_in[i]);
// 	}
// }

// void process() {
// 	static String str = "";
// 	if(Serial.available()) {
// 		char ch = Serial.read();
// 		if(ch != '\n') str += ch;
// 		else {
// 			mqttClient.publish(TOPIC, str.c_str());
// 			Serial.println("Published: " + str);
// 			str = "";
// 		}
// 	}
// }
void health_check() {
  static unsigned long last_time = 0;
  if (millis() - last_time > time_check_health) {
    last_time = millis();   // cập nhật thời gian để tránh spam liên tục

    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      http.begin(SERVER_URL);       
      http.addHeader("Content-Type", "application/json");

      char payload[128];
      snprintf(payload, sizeof(payload),
               "{\"ID\":\"%s\",\"device_1\":%d}",
               get_device_ID().c_str(), 100);

      int http_code = http.POST(payload);
      if (http_code > 0) {
        Serial.printf("HTTP Response code: %d\n", http_code);
        Serial.println(http.getString()); // server trả lời
      } else {
        Serial.printf("HTTP POST failed, code: %d\n", http_code);
      }
      http.end();
    }
  }
}



void setup() {
	Serial.begin(115200);
	EEPROM.begin(EEPROM_SIZE);
	gates_status_setup();
	connect_to_WiFi();
//	mqtt_setup();
}


void loop() {
	//update();
	//process();
	health_check();
	//check_connection();
}
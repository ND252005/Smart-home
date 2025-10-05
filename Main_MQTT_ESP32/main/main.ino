#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

#define EEPROM_SIZE 1
#define ADDR_RESET 0

WiFiClient espClient;
PubSubClient mqttClient(espClient);

//String SERVER_URL = "http://192.168.93.24:8000";  

const int DEVICE_LIMITS = 4;

int gates[DEVICE_LIMITS] = {22, 19, 23, 18};
int state[DEVICE_LIMITS] = {0, 0, 0, 0};

String HASHCODE = "";
const char* TOPIC;
WiFiManager wm;


unsigned long TIME_HEALTH_CHECK = 30000;

void gates_setup() {
	for(int i = 0; i < 4; i++) {
		pinMode(gates[i], OUTPUT);
	}
}

void connect_to_WiFi() {
	wm.setConnectTimeout(TRY_CONNECT_TIMEOUT);
	bool is_reset_connect = EEPROM.read(ADDR_RESET);
	if(is_reset_connect != 0 && is_reset_connect != 1) is_reset_connect = true;
	if(is_reset_connect) {
		wm.resetSettings();
		is_reset_connect = false;
		EEPROM.write(ADDR_RESET, is_reset_connect);
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

void create_topic() {
	if (WiFi.status() == WL_CONNECTED) {
    	String mac = WiFi.macAddress();
		HTTPClient http;
		String sever_get_hashcode = SERVER_URL + "/get-hash-code";
		http.begin(sever_get_hashcode.c_str());
		http.addHeader("Content-Type", "application/json");
  // Create JSON object
    StaticJsonDocument<256> doc;

    // Add MAC address
    doc["hashcode"] = WiFi.macAddress();  // e.g. "3C:61:05:2D:E0:78"

    // Create array of states
    JsonArray states = doc.createNestedArray("states");
    for (int i = 0; i < DEVICE_LIMITS; i++) {
      JsonObject state = states.createNestedObject();
      state["id"] = i;
      state["value"] = 0;   // replace with your sensor/relay value
    }

    // Serialize to string
    String json_data;
    serializeJson(doc, json_data);
		Serial.println("JSON Data: " + json_data); // In ra để debug

		int httpResponseCode = http.POST(json_data);
		
		if (httpResponseCode > 0) {
			String response = http.getString();
			Serial.println("Response code: " + String(httpResponseCode));
			Serial.println("Response: " + response);
			StaticJsonDocument<200> doc;
			DeserializationError error = deserializeJson(doc, response);
			if (!error) {
				HASHCODE = doc["hashcode"].as<String>();
				TOPIC = HASHCODE.c_str();

				Serial.println("Hash code received: " + HASHCODE);
			} else {
				Serial.println("JSON parse error");
			}
		} else {
			Serial.print("Error on sending POST: ");
			Serial.println(httpResponseCode);
		}
		http.end();
  	}
}

void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT... ");
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      	Serial.println(" Connected to MQTT");
		//mqttClient.subscribe(TOPIC);
		String device_topic;
		for(int i = 0; i < DEVICE_LIMITS; i++) {
			device_topic = String(TOPIC) + "/device_" + String(i) + "/" + "set";
			mqttClient.subscribe(device_topic.c_str());
		}
	} else {
		Serial.print("Failed, rc=");
		Serial.print(mqttClient.state());
		Serial.println(" -> retrying in 2 seconds");
		delay(2000);
    }
  }
}

void update_state(int index, int status) {
	status = constrain(status, 0, 255);
	analogWrite(gates[index], status);
	state[index] = status;
}

//nghe 
void on_mqtt_message(char* topic, byte* payload, unsigned int length) {
	String message;
	for (unsigned int i = 0; i < length; i++) message += (char)payload[i];
	//if(message == "") return;

	for(int i = 0; i < DEVICE_LIMITS; i++) {
		String str_set = String(TOPIC) + "/device_" + String(i) + "/" + "set";
		if(String(topic) == str_set) {
			int stt = constrain(message.toInt(), 0, 255);
			update_state(i, message.toInt());
			String str_state = String(TOPIC) + "/" + String(i) + "/" + "state";
			mqttClient.publish(str_state.c_str(), message.c_str());
			Serial.println(str_state + ": " + message);
		}
	}
}

void mqtt_setup() {
	mqttClient.setServer(MQTT_HOST, MQTT_PORT);
	mqttClient.setCallback(on_mqtt_message);	
}

void update() {
	if (!mqttClient.connected()) {
		connectToMQTT();
	}
	mqttClient.loop();
	// for(int i = 0; i < 4; i++) {
	// 	state[i] = analogRead(gates[i]);
	// }
}

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

//Ping to server for check alive
void health_check() {
  static unsigned long last_time = 0;
  if (millis() - last_time > TIME_HEALTH_CHECK) {
    last_time = millis();   // update new time

    if (WiFi.status() == WL_CONNECTED) {
		WiFiClient client;
		HTTPClient http;
		String server_health_check = SERVER_URL + "/health-check";
		http.begin(server_health_check);       
		http.addHeader("Content-Type", "application/json");

		StaticJsonDocument<200> doc;
		doc["ESP_ID"] = HASHCODE;
		for(int i = 0; i < DEVICE_LIMITS; i++) {

			doc["state_device_" + String(i)] = String(state[i]);
		}
		String Json_data;
		serializeJson(doc, Json_data);
		Serial.println("JSON Data: " + Json_data); // In ra để debug

		int http_response_code = http.POST(Json_data);
		if (http_response_code > 0) {
			String response = http.getString();
			Serial.println("Response code: " + String(http_response_code));
			Serial.println("Response: " + response);
		} else {
			Serial.print("Error code: ");
			Serial.println(http_response_code);
		}
		http.end();
    }
  }
}



void setup() {
	Serial.begin(115200);
	EEPROM.begin(EEPROM_SIZE);
	connect_to_WiFi();
	create_topic();
	gates_setup();
	mqtt_setup();
}


void loop() {
	update();
	//process();
	health_check();
	//check_connection();
}
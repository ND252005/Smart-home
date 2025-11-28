#include "MQTTProtocol.h"

MQTT* MQTT::instance = nullptr;
MQTT :: MQTT(){}
MQTT::MQTT(String server, int port) 
    : mqtt_server(server), mqtt_port(port), client(espClient){
        topic_sensor = HASHCODE + "/sensor";
        topic_ping = HASHCODE + "/ping";
        instance = this;
    }

void MQTT::begin() {
    client.setServer(mqtt_server.c_str(), mqtt_port);
    client.setCallback(MQTT::MQTTCallBack);
    Serial.print("Connecting to MQTT...");
    while (!client.connected()) {
        if (client.connect("ESP32Client", username.c_str(), password.c_str())) {
            Serial.println("connected");
            // client.subscribe(topic_set.c_str());
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void MQTT::Ping2Server() {
    String buffer = "alive";
    client.publish(topic_ping.c_str(), buffer.c_str());
}


void MQTT::PublishStateSensor(int temperature, int humandity, int gas_detect) {
    StaticJsonDocument<100> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humandity;
    doc["gas_detect"] = gas_detect;

    char buffer[100];
    serializeJson(doc, buffer);
    client.publish(topic_sensor.c_str(), buffer);
}

void MQTT::loop() {
    if (WiFi.status() != WL_CONNECTED) return;
    if (!client.connected()) reconnect();
    client.loop();
}

void MQTT::reconnect() {
    Serial.print("Connecting to MQTT...");

    static unsigned long lastAttempt = 0;
    if (millis() - lastAttempt < 5000) return;  // tránh spam connect
    lastAttempt = millis();

    if (!client.connect(MQTT_CLIENT_ID, username.c_str(), password.c_str())) {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
    }
}

void MQTT::setCredentials(const String& user, const String& pass) {
    username = user;
    password = pass;
}

void MQTT::MQTTCallBack(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
    msg.trim();
    Serial.println(msg);

    StaticJsonDocument<100> doc;
    DeserializationError err = deserializeJson(doc, msg);
    if (err) {
        Serial.println("JSON parse error");
        return;
    }
}

// void MQTT::setHashcode(String hash) {
//     mqtt_hashcode = hash;
//     topic_sensor = mqtt_hashcode + "/sensor";
// }
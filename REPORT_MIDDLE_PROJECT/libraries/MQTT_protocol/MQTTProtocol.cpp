#include "MQTTProtocol.h"

MQTT* MQTT::instance = nullptr;
MQTT :: MQTT(){}
MQTT::MQTT(String server, int port, String hashcode) 
    : mqtt_server(server), mqtt_port(port), mqtt_hashcode(hashcode), client(espClient){
        topic_set = mqtt_hashcode + "/set";
        topic_state = mqtt_hashcode + "/state";
        topic_sensor = mqtt_hashcode + "/sensor";
        instance = this;
    }
void MQTT::PublishStateControl(int gate_, int state_) {
    StaticJsonDocument<100> doc;
    doc["device_id"] = gate_;
    doc["state"] = state_;

    char buffer[100];
    serializeJson(doc, buffer);
    client.publish(topic_state.c_str(), buffer);
}

void MQTT::PublishStateSensor(int temperature, int humandity, int gas_detect) {
    StaticJsonDocument<100> doc;
    doc["temperature"] = temperature;
    doc["humandity"] = humandity;
    doc["gas_detect"] = gas_detect;

    char buffer[100];
    serializeJson(doc, buffer);
    client.publish(topic_state.c_str(), buffer);
}

void MQTT::begin() {
    client.setServer(mqtt_server.c_str(), mqtt_port);
    client.setCallback(MQTT::MQTTCallBack);
    Serial.print("Connecting to MQTT...");
    while (!client.connected()) {
        if (client.connect("ESP32Client", username.c_str(), password.c_str())) {
            Serial.println("connected");
            client.subscribe(topic_set.c_str());
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
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

    if (client.connect(MQTT_CLIENT_ID, username.c_str(), password.c_str())) {
        client.subscribe(topic_set.c_str());
    } else {
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
    int recv_device_id = doc["device_id"];
    state[recv_device_id] = doc["state"];
    analogWrite(gates[recv_device_id], state[recv_device_id]);
    if(instance) instance->PublishStateControl(recv_device_id, state[recv_device_id]);
}

void MQTT::setHashcode(String hash) {
    mqtt_hashcode = hash;
    topic_set = mqtt_hashcode + "/set";
    topic_state = mqtt_hashcode + "/state";
    topic_sensor = mqtt_hashcode + "/sensor";
}
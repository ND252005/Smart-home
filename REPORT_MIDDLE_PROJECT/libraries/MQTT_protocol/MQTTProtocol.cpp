#include "MQTTProtocol.h"

MQTT* MQTT::instance = nullptr;
MQTT :: MQTT(){}
MQTT::MQTT(String server, int port) 
    : mqtt_server(server), mqtt_port(port), client(espClient){
        topic_state = HASHCODE + "/state";
        topic_set = HASHCODE + "/set";
        instance = this;
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

void MQTT::PublishStateController(String name_device_1, int state_1, String name_device_2, int state_2) {
    StaticJsonDocument<100> doc;
    doc[name_device_1] = state_1;
    doc[name_device_2] = state_2;
    char buffer[100];
    serializeJson(doc, buffer);
    client.publish(topic_state.c_str(), buffer);
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
//esp nghe topic state
void MQTT::MQTTCallBack(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
    msg.trim();
    Serial.println(msg);

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, msg);
    if (err) {
        Serial.println("JSON parse error");
        return;
    }

    JsonObject root = doc.as<JsonObject>();

    for (JsonPair kv : root) {
        String deviceName = String(kv.key().c_str());
        if (deviceName.startsWith("device_")) {
            int id = deviceName.substring(7).toInt();
        String msg = String(kv.value().c_str());
            if (msg = "toggle") {
                switch (id) {
                    case 1:{

                        int toggle_status_1 = (read_pin[0] > 100) ? LOW : HIGH; 
                        digitalWrite(device_pin[0], toggle_status_1);
                        Serial.println("Da lat trang thai Device 1");
                        break;
                    }


                    case 2:{
                        int toggle_status_2 = (read_pin[1] > 100) ? LOW : HIGH;     
                        digitalWrite(device_pin[1], toggle_status_2);
                        Serial.println("Da lat trang thai Device 2");
                        break;
                    }

                    
                    case 3:
                        // digitalWrite(device_pin[2], !digitalRead(device_pin[2]));
                        break;

                    default:
                        Serial.println("ID thiet bi khong ton tai");
                        break;
                }
            }
        }
    }
}

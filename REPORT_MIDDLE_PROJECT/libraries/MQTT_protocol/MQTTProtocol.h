#ifndef MQTTPROTOCOL_H
#define MQTTPROTOCOL_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <config.h>


class MQTT {
    private:
        String mqtt_server;
        int mqtt_port;
        String mqtt_hashcode = "";
        String topic_set;
        String topic_state;
        String username;
        String password;

        WiFiClient espClient;
        PubSubClient client{espClient};

        //được dùng để kiểm tra mqtt khai báo đúng cách chưa
        static MQTT* instance;
        void PublishState(int gate_, int state_);
    public:
        MQTT();
        MQTT(String server, int port, String hashcode);
        void begin();
        void loop();
        void reconnect();
        void setHashcode(String hash);
        void setCredentials(const String& user, const String& pass);
        static void MQTTCallBack(char* topic, byte* payload, unsigned int length);
};

#endif
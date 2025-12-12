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
        String topic_set;
        String topic_state;
        String username;
        String password;
    

        WiFiClient espClient;
        PubSubClient client{espClient};

        //được dùng để kiểm tra mqtt khai báo đúng cách chưa
        static MQTT* instance;

    public:
        MQTT();
        MQTT(String server, int port);
        void begin();
        void PublishStateController(String name_device_1, int state_1, String name_device_2, int state_2);
        void loop();
        void reconnect();
        // void setHashcode(String hash);
        // void Ping2Server();
        void setCredentials(const String& user, const String& pass);
        static void MQTTCallBack(char* topic, byte* payload, unsigned int length);
};

#endif
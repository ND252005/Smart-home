#ifndef HTTPPROTOCOL_H
#define HTTPPROTOCOL_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class HTTPProtocol {
public:
    HTTPProtocol();
    HTTPProtocol(const char* serverUrl);
    bool postRegisterDevice(const String& device_name);
    String getHashcode() const;

private:
    String server_url;
    String hashcode;
};

#endif

#ifndef HTTPPROTOCOL_H
#define HTTPPROTOCOL_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <config.h>

class HTTPProtocol {
public:
    HTTPProtocol();
    HTTPProtocol(const char* serverUrl);
    // bool postRegisterESP(const String& device_name);
    bool healthCheckESP();
    // String getHashcode() const;

private:
    String server_url;
    // String hashcode;
};

#endif

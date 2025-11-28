#include "HTTPProtocol.h"

HTTPProtocol::HTTPProtocol(const char* serverUrl)
    : server_url(serverUrl) {}
HTTPProtocol :: HTTPProtocol(){}

// bool HTTPProtocol::postRegisterESP(const String& device_name) {
//     if (WiFi.status() != WL_CONNECTED) {
//         Serial.println("WiFi not connected");
//         return false;
//     }

//     HTTPClient http;
//     http.begin(server_url + "/api/sensor/register");
//     http.addHeader("Content-Type", "application/json");

//     StaticJsonDocument<200> doc;
//     doc["mac_id"] = device_name;
//     String body;
//     serializeJson(doc, body);

//     Serial.println("[HTTP] Sending POST request...");
//     int http_response_code = http.POST(body);

//     if (http_response_code > 0) {
//         String response = http.getString();
//         Serial.print("[HTTP] Response: ");
//         Serial.println(response);

//         StaticJsonDocument<256> resp_doc;
//         DeserializationError err = deserializeJson(resp_doc, response);
//         if (err) {
//             Serial.println("JSON parse error");
//             http.end();
//             return false;
//         }

//         if (resp_doc.containsKey("hashcode")) {
//             hashcode = resp_doc["hashcode"].as<String>();
//             Serial.print("[HTTP] Received hashcode: ");
//             Serial.println(hashcode);
//             http.end();
//             return true;
//         } else {
//             Serial.println("No hashcode field in response");
//         }
//     } else {
//         Serial.print("HTTP request failed, code: ");
//         Serial.println(http_response_code);
//     }

//     http.end();
//     return false;
// }

bool HTTPProtocol::healthCheckESP() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        return false;
    }

    HTTPClient http;
    http.begin(server_url + "/esps/sensor/health-check");

    StaticJsonDocument<200> doc;
    doc["health_check"] = HASHCODE;
    String body;
    serializeJson(doc, body);

    Serial.println("[HTTP] Sending POST request...");
    int http_response_code = http.POST(body);

    if (http_response_code > 0) {
        String response = http.getString();
        Serial.print("[HTTP] Response: ");
        Serial.println(response);

        StaticJsonDocument<256> resp_doc;
        DeserializationError err = deserializeJson(resp_doc, response);
        if (err) {
            Serial.println("JSON parse error");
            http.end();
            return false;
        }

        if (resp_doc.containsKey("server")) {
            String server_state = resp_doc["server"].as<String>();
            Serial.print("[HTTP] Received sever state: ");  
            Serial.println(server_state);
            http.end();
            return true;
        } else {
            Serial.println("No sever state in responde");
        }
    } else {
        Serial.print("HTTP request failed, code: ");
        Serial.println(http_response_code);
    }

    http.end();
    return false;
}


// String HTTPProtocol::getHashcode() const {
//     return this->hashcode;
// }


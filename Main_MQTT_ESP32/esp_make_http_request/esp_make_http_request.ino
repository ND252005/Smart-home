#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Duc Nguyen";
const char* password = "22222222";
const char* SERVER_URL = "http://192.168.180.24:8000/add-device";
void setup() {
	Serial.begin(115200);
	WiFi.begin(ssid, password);
	Serial.print("connect to wifi ..");
	while(WiFi.status() != WL_CONNECTED){
		delay(300);
		Serial.println(".");
	}
	Serial.println("connected");
	Serial.println(WiFi.localIP());
}
//Danh sách thiết bị
  String deviceName = "den hoc";
  bool deviceStatus = false;
  String deviceID = "4020392193ab42"; 
  unsigned long last_time = millis();
void loop() {
  // Biến giá trị (có thể thay bằng giá trị cảm biến)

  if(millis() - last_time > 10000) {
    last_time = millis();
    if (WiFi.status() == WL_CONNECTED) {
		HTTPClient http;
		http.begin(SERVER_URL);
		http.addHeader("Content-Type", "application/json");

		// Tạo JSON body từ biến
		String jsonData = "{";
		jsonData += "\"name\":\"" + deviceName + "\",";
		jsonData += "\"status\":" + String(deviceStatus ? "true" : "false") + ",";
		jsonData += "\"device_id\":\"" + deviceID + "\"";
		jsonData += "}";

		Serial.println("JSON Data: " + jsonData); // In ra để debug

		int httpResponseCode = http.POST(jsonData);

		if (httpResponseCode > 0) {
			String response = http.getString();
			Serial.println("Response code: " + String(httpResponseCode));
			Serial.println("Response: " + response);
		} else {
			Serial.print("Error code: ");
			Serial.println(httpResponseCode);
		}

		http.end();
    }

    }

}

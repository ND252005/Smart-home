#include <WiFi.h>
#include <WiFiManager.h>  // install tzapu WiFiManager library
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntp;
NTPClient timeClient(ntp, "pool.ntp.org", 7*3600, 60000);


void 

void setup() {
	Serial.begin(115200);

	WiFiManager wm;

	// Reset saved WiFi credentials if needed (only for debugging)
	wm.resetSettings();

	// Try to connect, if fail → start AP "ESP32_AP" with password "12345678"
	if (!wm.autoConnect("ESP32_AP", "12345678")) {
		Serial.println("❌ Failed to connect and no timeout hit");
		ESP.restart();
	}

	Serial.println("✅ Connected to WiFi!");
	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());
	timeClient.begin();

}
unsigned long prev_time = millis();
void loop() {
    if(millis() - prev_time > 1000) {
        timeClient.update();

        // In ra giờ theo định dạng HH:MM:SS
        Serial.print("Giờ hiện tại: ");
        Serial.println(timeClient.getFormattedTime());
        prev_time = millis();

    }
}
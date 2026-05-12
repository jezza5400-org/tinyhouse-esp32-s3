#include <HTTPClient.h>
#include <WiFi.h>

const char *ssid = "ssid";
const char *password = "passwd";

void connectWifi() {
	while (WiFi.status() != WL_CONNECTED) {
		Serial.println(String("Attempting to connect to network ") + ssid);
		WiFi.begin(ssid, password);

		unsigned long waitStart = millis();
		while (WiFi.status() != WL_CONNECTED && (millis() - waitStart) < 10000) {
			delay(100);
			Serial.print(".");
		}

		if (WiFi.status() == WL_CONNECTED) break;

		WiFi.disconnect(true, false);
		delay(100);
		WiFi.mode(WIFI_STA);
	}
}

void setup() {
	Serial.begin(115200);

	delay(5000);

	Serial.println("\nConnecting to WiFi...");
	connectWifi();

	Serial.println("\nWiFi connected!");
	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());
}

void loop() {
	if (WiFi.status() == WL_CONNECTED) {
		HTTPClient http;

		http.begin("http://www.google.com");
		int httpCode = http.GET();

		if (httpCode > 0) {
			Serial.printf("Google reachable ✔ (HTTP %d)\n", httpCode);
		} else {
			Serial.printf("Google unreachable ❌ (%s)\n", http.errorToString(httpCode).c_str());
		}

		http.end();
	} else {
		Serial.println("WiFi disconnected!");
	}

	delay(2000); // ping every 2 seconds
}

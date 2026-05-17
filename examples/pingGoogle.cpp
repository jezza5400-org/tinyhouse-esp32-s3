#include <HTTPClient.h>
#include <WiFi.h>

#include "secrets.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

void connectWifi() {
	while (WiFi.status() != WL_CONNECTED) {
		Serial.printf("Attempting to connect to network %s\r\n", ssid);
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
	while (!Serial) yield();
	delay(1000);

	Serial.println("\nConnecting to WiFi...");
	connectWifi();

	IPAddress localIp = WiFi.localIP();
	Serial.printf("\nWiFi connected!\nIP Address: %d.%d.%d.%d\r\n", localIp[0], localIp[1], localIp[2], localIp[3]);
}

void loop() {
	if (WiFi.status() == WL_CONNECTED) {
		HTTPClient http;

		http.begin("http://www.google.com");
		int httpCode = http.GET();

		if (httpCode > 0) {
			Serial.printf("Google reachable (HTTP %d)\n", httpCode);
		} else {
			Serial.printf("Google unreachable (%s)\n", http.errorToString(httpCode).c_str());
		}

		http.end();
	} else {
		Serial.println("WiFi disconnected!");
	}

	delay(2000);
}

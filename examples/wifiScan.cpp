#include "esp_wifi.h"
#include <WiFi.h>

void printNetworks() {
	Serial.println("\n--- WiFi Scan ---");
	int n = WiFi.scanNetworks();
	if (n == 0) {
		Serial.println("No networks found.");
		return;
	}

	for (int i = 0; i < n; i++) {
		Serial.printf("%2d: %-32s  RSSI: %4d  CH: %2d  ENC: %s\n", i, WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i), (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "SECURED");
	}
	Serial.println("-----------------\n");
}

void connectToNetwork(String ssid, String password) {
	Serial.printf("Connecting to '%s'...\n", ssid.c_str());
	WiFi.begin(ssid.c_str(), password.c_str());

	unsigned long start = millis();
	while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
		Serial.print(".");
		delay(300);
	}

	if (WiFi.status() == WL_CONNECTED) {
		Serial.printf("\nConnected! IP: %s\n", WiFi.localIP().toString().c_str());
	} else {
		Serial.println("\nFailed to connect.");
	}
}

void setup() {
	Serial.begin(115200);
	while (!Serial) yield();
	delay(1000);

	Serial.println("\nESP32 WiFi Scanner + Connector");
	esp_wifi_set_country_code("AU", true);
	WiFi.mode(WIFI_STA);
	delay(500);

	printNetworks();
	Serial.println("Type commands:");
	Serial.println("  scan");
	Serial.println("  connect <ssid> <password>");
	Serial.println();
}

String input = "";

void loop() {
	while (Serial.available()) {
		char c = Serial.read();
		if (c == '\n' || c == '\r') {
			if (input.length() > 0) {
				Serial.printf(">> %s\n", input.c_str());

				if (input == "scan") {
					printNetworks();
				} else if (input.startsWith("connect ")) {
					int firstSpace = input.indexOf(' ');
					int secondSpace = input.indexOf(' ', firstSpace + 1);

					if (secondSpace == -1) {
						Serial.println("Usage: connect <ssid> <password>");
					} else {
						String ssid = input.substring(firstSpace + 1, secondSpace);
						String password = input.substring(secondSpace + 1);
						connectToNetwork(ssid, password);
					}
				} else {
					Serial.println("Unknown command.");
				}
			}
			input = "";
		} else {
			input += c;
		}
	}
}

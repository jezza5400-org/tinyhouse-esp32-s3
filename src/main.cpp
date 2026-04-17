#include <Arduino.h>
#include <ArduinoJson.h>

#include <DhtCommon.h>
#include <OneWireCommon.h>
#include <VeDirectParser.h>
#include <WiFiSender.h>

#if defined(ARDUINO_ARCH_RP2040)
#include <WiFiNINA.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

int status = WL_IDLE_STATUS;

constexpr const char *WIFI_SSID = "WiFi-SSID";
constexpr const char *WIFI_PASSWORD = "WiFi-Password";
constexpr const char *DWEET_HOST = "10.245.141.2";
constexpr uint16_t DWEET_PORT = 8080;
constexpr const char *DWEET_THING = "thing-name";
constexpr uint8_t ONEWIRE_PIN = 4;
constexpr uint8_t DHT_PIN = 19;

WiFiSender jsonSender(DWEET_HOST, DWEET_PORT, DWEET_THING);
VeDirectParser veParser;
OneWireCommon oneWireTelemetry(ONEWIRE_PIN);
DhtCommon dhtTelemetry(DHT_PIN, DHT22, 2500);

void connectWifi() {
	while (WiFi.status() != WL_CONNECTED) {
		Serial.println(String("Attempting to connect to network ") + WIFI_SSID);
		status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

		unsigned long waitStart = millis();
		while (WiFi.status() != WL_CONNECTED && (millis() - waitStart) < 10000) delay(100);

		if (WiFi.status() == WL_CONNECTED) break;

#if defined(ARDUINO_ARCH_RP2040)
		WiFi.disconnect();
		WiFi.end();
#elif defined(ARDUINO_ARCH_ESP32)
		WiFi.disconnect(true, false);
		delay(100);
		WiFi.mode(WIFI_STA);
#endif
	}
}

void printWifiStatus() {
	Serial.println(String("SSID: ") + WiFi.SSID());
	Serial.println(String("IP Address: ") + WiFi.localIP().toString());
	Serial.println("Signal strength (RSSI): " + String(WiFi.RSSI()) + " dBm");
}

bool publishCombinedPayload() {
	JsonDocument combinedDoc;
	JsonObject payload = combinedDoc.to<JsonObject>();

	JsonObject victron = payload["victron"].to<JsonObject>();
	veParser.copyFieldsTo(victron);
	oneWireTelemetry.appendPayload(payload);
	dhtTelemetry.appendPayload(payload);

	if (combinedDoc.overflowed()) {
		Serial.println("Combined payload dropped (JSON overflow)");
		return false;
	}

	if (Serial) {
		Serial.println("Combined JSON payload:");
		serializeJson(combinedDoc, Serial);
		Serial.println();
	}

	return jsonSender.send(combinedDoc);
}

void setup() {
	Serial.begin(9600);
	Serial1.begin(19200);
	while (!Serial);
	oneWireTelemetry.begin();

#if defined(ARDUINO_ARCH_RP2040)
	dhtTelemetry.begin();
#endif

	veParser.begin();
	connectWifi();
	printWifiStatus();
}

void loop() {
	oneWireTelemetry.poll();
	dhtTelemetry.poll();

	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("Wi-Fi disconnected, reconnecting");
		connectWifi();
		printWifiStatus();
	}

	veParser.process(Serial1);

	if (veParser.hasFreshFrame()) {
		if (!publishCombinedPayload()) Serial.println("Failed to send combined telemetry payload");
		veParser.markFrameConsumed();
	}

	yield();
}

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include <DhtCommon.h>
#include <OneWireCommon.h>
#include <VeDirectParser.h>
#include <WiFiSender.h>

#include "secrets.h"

constexpr char DWEET_HOST[] = "10.42.110.2";
constexpr uint16_t DWEET_PORT = 8080;
constexpr uint8_t RELAY_PIN = 7;
constexpr uint8_t DHT_PIN = 15;
constexpr uint8_t ONEWIRE_PIN = 16;
constexpr uint8_t T_ON = 20;
constexpr uint8_t T_OFF = 24;
constexpr uint16_t BATT_CUTOFF = 11800;
constexpr uint16_t SENSOR_FAILSAFE_OFF_MS = 5000;

WiFiSender jsonSender(DWEET_HOST, DWEET_PORT, DWEET_THING);
VeDirectParser veParser;
OneWireCommon oneWireCommon(ONEWIRE_PIN);
DhtCommon dhtCommon(DHT_PIN, DHT22, 2500);

void controlHeater() {
	static unsigned long sensorLostAtMs = 0;

	const bool sensorConnected = oneWireCommon.isConnected();
	const float waterTempC = oneWireCommon.getTempC();
	const bool powerOk = veParser.getBattVoltage() > BATT_CUTOFF || veParser.getPanelVoltage() > BATT_CUTOFF;
	const bool heaterOn = digitalRead(RELAY_PIN) == HIGH;

	if (!sensorConnected) {
		if (sensorLostAtMs == 0) sensorLostAtMs = millis();
		if (millis() - sensorLostAtMs >= SENSOR_FAILSAFE_OFF_MS) digitalWrite(RELAY_PIN, LOW);
		return;
	}

	sensorLostAtMs = 0;

	if (!powerOk) {
		digitalWrite(RELAY_PIN, LOW);
		return;
	}

	if (!heaterOn && waterTempC <= T_ON) {
		digitalWrite(RELAY_PIN, HIGH);
	} else if (heaterOn && waterTempC >= T_OFF) {
		digitalWrite(RELAY_PIN, LOW);
	}
}

void printWifiStatus() {
	Serial.println(String("SSID: ") + WiFi.SSID());
	Serial.println(String("IP Address: ") + WiFi.localIP().toString());
	Serial.println("Signal strength (RSSI): " + String(WiFi.RSSI()) + " dBm");
}

void connectWifi() {
	while (WiFi.status() != WL_CONNECTED) {
		Serial.println(String("Attempting to connect to network ") + WIFI_SSID);
		WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

		unsigned long waitStart = millis();
		while (WiFi.status() != WL_CONNECTED && (millis() - waitStart) < 10000) delay(100);

		if (WiFi.status() == WL_CONNECTED) break;

		WiFi.disconnect(true, false);
		delay(100);
		WiFi.mode(WIFI_STA);

		oneWireCommon.pollBlocking();
		veParser.process(Serial1);
		controlHeater();
	}
}

bool publishCombinedPayload() {
	JsonDocument combinedDoc;
	JsonObject payload = combinedDoc.to<JsonObject>();

	JsonObject victron = payload["victron"].to<JsonObject>();
	veParser.copyFieldsTo(victron);
	oneWireCommon.appendPayload(payload);
	dhtCommon.appendPayload(payload);

	if (combinedDoc.overflowed()) {
		Serial.println("Combined payload dropped (JSON overflow)");
		return false;
	}

	Serial.println("Combined JSON payload:");
	serializeJson(combinedDoc, Serial);
	Serial.println();

	return jsonSender.send(combinedDoc);
}

void setup() {
	Serial.begin(9600);
	Serial1.begin(19200);
	// while (!Serial) yield();
	oneWireCommon.begin();

	veParser.begin();
	pinMode(RELAY_PIN, OUTPUT);
	digitalWrite(RELAY_PIN, LOW);
	connectWifi();
	printWifiStatus();
}

void loop() {
	static unsigned long lastPublish = 0;
	oneWireCommon.pollBlocking();
	dhtCommon.poll();
	veParser.process(Serial1);
	controlHeater();

	if (WiFi.status() == WL_CONNECTED && veParser.hasFreshFrame()) {
		if (!oneWireCommon.isConnected() || !dhtCommon.isConnected()) {
			Serial.println("Skipping publish: one or more sensors disconnected");
		} else if (millis() - lastPublish >= 10000) {
			if (!publishCombinedPayload()) Serial.println("Failed to send combined payload");
			lastPublish = millis();
		}

		veParser.markFrameConsumed();
	} else if (WiFi.status() != WL_CONNECTED) {
		connectWifi();
	}
}

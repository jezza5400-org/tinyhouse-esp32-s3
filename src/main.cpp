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

#include "secrets.h"

int status = WL_IDLE_STATUS;

constexpr char DWEET_HOST[] = "10.42.110.2";
constexpr uint16_t DWEET_PORT = 8080;
constexpr uint8_t ONEWIRE_PIN = 4;
constexpr uint8_t RELAY_PIN = 2;
constexpr uint8_t DHT_PIN = 19;
constexpr uint8_t T_ON = 20;
constexpr uint8_t T_OFF = 24;
constexpr uint16_t BATT_CUTOFF = 11800;
constexpr unsigned long SENSOR_FAILSAFE_OFF_MS = 5000;

WiFiSender jsonSender(DWEET_HOST, DWEET_PORT, DWEET_THING);
VeDirectParser veParser;
OneWireCommon oneWireCommon(ONEWIRE_PIN);
DhtCommon dhtCommon(DHT_PIN, DHT22, 2500);

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

void setup() {
	Serial.begin(9600);
	Serial1.begin(19200);
	//while (!Serial) yield();
	oneWireCommon.begin();

#if defined(ARDUINO_ARCH_RP2040)
	dhtCommon.begin();
#endif

	veParser.begin();
	pinMode(RELAY_PIN, OUTPUT);
	digitalWrite(RELAY_PIN, LOW);
	connectWifi();
	printWifiStatus();
}

void loop() {
	static unsigned long lastPublish = 0;

	oneWireCommon.poll(true);
	dhtCommon.poll();
	veParser.process(Serial1);
	controlHeater();

	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("Wi-Fi disconnected, reconnecting");
		connectWifi();
		printWifiStatus();
	}

	if (veParser.hasFreshFrame()) {
		if (!oneWireCommon.isConnected() || !dhtCommon.isConnected()) {
			Serial.println("Skipping publish: one or more sensors disconnected");
		} else if (millis() - lastPublish >= 10000) {
			if (!publishCombinedPayload()) Serial.println("Failed to send combined payload");
			lastPublish = millis();
		}

		veParser.markFrameConsumed();
	}
}

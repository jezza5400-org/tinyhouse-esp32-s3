#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#include <DhtCommon.h>
#include <OneWireCommon.h>
#include <VeDirectParser.h>
#include <WiFiCommon.h>

#include "secrets.h"

constexpr uint8_t RELAY_PIN = 7;
constexpr uint8_t DHT_PIN = 15;
constexpr uint8_t ONEWIRE_PIN = 16;
constexpr uint8_t TEMP_ON = 20;
constexpr uint8_t TEMP_OFF = 24;
constexpr int32_t NET_BATT_MARGIN_ON_MA = 200;
constexpr int32_t NET_BATT_MARGIN_OFF_MA = 50;
constexpr uint16_t BATT_CUTOFF_V = 11800;
constexpr uint16_t SENSOR_FAILSAFE_OFF_MS = 30000;
constexpr float HEATER_EFFECTIVE_POWER_W = 16.8f;
constexpr float REQUIRED_PANEL_POWER_ON_W = HEATER_EFFECTIVE_POWER_W + 6.0f;
constexpr float REQUIRED_PANEL_POWER_OFF_W = HEATER_EFFECTIVE_POWER_W + 3.0f;
constexpr uint16_t MIN_VALID_BATT_VOLTAGE_MV = 1000;

WiFiCommon wifi(HOST, THING);
VeDirectParser veParser;
OneWireCommon oneWireCommon(ONEWIRE_PIN);
DhtCommon dhtCommon(DHT_PIN, DHT22, 2500);

void controlHeater() {
	static unsigned long sensorLostAtMs = 0;

	const bool sensorConnected = oneWireCommon.isConnected();
	const float waterTempC = oneWireCommon.getTempC();
	const bool heaterOn = digitalRead(RELAY_PIN) == HIGH;
	const bool powerOkByBatteryRule = veParser.getBattVoltage() > BATT_CUTOFF_V || veParser.getPanelVoltage() > BATT_CUTOFF_V;

	const uint16_t battVoltageMv = veParser.getBattVoltage();
	const int32_t battCurrentMa = veParser.getBattCurrentMa();
	const uint16_t panelPowerW = veParser.getPanelPowerW();
	const float requiredPanelPowerW = heaterOn ? REQUIRED_PANEL_POWER_OFF_W : REQUIRED_PANEL_POWER_ON_W;
	const bool panelPowerSupportsHeater = static_cast<float>(panelPowerW) >= requiredPanelPowerW;

	int32_t estimatedHeaterCurrentMa = 0;
	if (battVoltageMv >= MIN_VALID_BATT_VOLTAGE_MV) {
		estimatedHeaterCurrentMa = static_cast<int32_t>((HEATER_EFFECTIVE_POWER_W * 1000000.0f) / static_cast<float>(battVoltageMv));
	}

	const bool batteryTrendSupportsHeater = heaterOn ? battCurrentMa >= NET_BATT_MARGIN_OFF_MA : (battCurrentMa - estimatedHeaterCurrentMa) >= NET_BATT_MARGIN_ON_MA;
	const bool solarSurplusAllowsHeater = panelPowerSupportsHeater && batteryTrendSupportsHeater;
	const bool powerOk = powerOkByBatteryRule || solarSurplusAllowsHeater;

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

	if (!heaterOn && waterTempC <= TEMP_ON) {
		digitalWrite(RELAY_PIN, HIGH);
	} else if (heaterOn && waterTempC >= TEMP_OFF) {
		digitalWrite(RELAY_PIN, LOW);
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

	return wifi.send(combinedDoc);
}

void setup() {
	Serial.begin(115200);
	Serial1.begin(19200, SERIAL_8N1, 18, 17);
	oneWireCommon.begin();
	veParser.begin();
	pinMode(RELAY_PIN, OUTPUT);
	digitalWrite(RELAY_PIN, LOW);
	// while (!Serial) yield();
	// delay(1000);
	WiFi.mode(WIFI_STA);
	configureWiFiCountry();
	connectWiFi(WIFI_SSID, WIFI_PASSWORD);
	printWiFiStatus();
}

void loop() {
	oneWireCommon.pollBlocking();
	dhtCommon.poll();
	veParser.process(Serial1);
	controlHeater();

	Serial.printf("Onewire: %f, DHT Temp: %f, DHT Humid: %f\r\n", oneWireCommon.getTempC(), dhtCommon.getTempC(), dhtCommon.getHumidPct());

	if (WiFi.status() == WL_CONNECTED && veParser.hasFreshFrame()) {
		if (!oneWireCommon.isConnected() || !dhtCommon.isConnected()) {
			Serial.println("Skipping publish: one or more sensors disconnected");
		} else {
			if (publishCombinedPayload()) {
				vTaskDelay(pdMS_TO_TICKS(30000));
			} else {
				Serial.println("Failed to send combined payload");
			}
		}

		veParser.markFrameConsumed();
	} else if (WiFi.status() != WL_CONNECTED) {
		connectWiFi(WIFI_SSID, WIFI_PASSWORD);
	}
}

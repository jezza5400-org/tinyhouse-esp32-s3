#include <ArduinoJson.h>

#include "WiFiCommon.h"
#include "secrets.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *url = HOST;
const char *thing = THING;
WiFiCommon wifi(url, thing);

void setup() {
	Serial.begin(115200);
	while (!Serial) yield();
	delay(1000);

	connectWiFi(ssid, password);

	JsonDocument payload;
	JsonObject victron = payload["victron"].to<JsonObject>();
	victron["PID"] = "0xA067";
	victron["FW"] = "174";
	victron["SER#"] = "HQ20351YEAA";
	victron["V"] = "12510";
	victron["I"] = "-40";
	victron["VPV"] = "10";
	victron["PPV"] = "0";
	victron["CS"] = "0";
	victron["MPPT"] = "0";
	victron["OR"] = "0x00000001";
	victron["ERR"] = "0";
	victron["LOAD"] = "ON";
	victron["IL"] = "0";
	victron["H19"] = "1590";
	victron["H20"] = "0";
	victron["H21"] = "30";
	victron["H22"] = "0";
	victron["H23"] = "0";
	victron["HSDS"] = "219";

	JsonObject oneWireTemp = payload["OneWireTemp"].to<JsonObject>();
	oneWireTemp["sensor_connected"] = true;
	oneWireTemp["temperature_c"] = 17.375;

	JsonObject dht22 = payload["DHT22"].to<JsonObject>();
	dht22["sensor_connected"] = true;
	dht22["temperature_c"] = 22.3;
	dht22["humidity_pct"] = 56;

	wifi.send(payload);
}

void loop() {
	vTaskDelay(portMAX_DELAY);
}

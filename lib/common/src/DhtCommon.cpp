#include "DhtCommon.h"

DhtCommon::DhtCommon(uint8_t pin, uint8_t sensorModel, unsigned long readIntervalMs)
	: _pin(pin), _sensorModel(sensorModel), _readIntervalMs(readIntervalMs) {}

void DhtCommon::poll() {
	unsigned long now = millis();
	if ((now - _lastReadMillis) < _readIntervalMs) return;
	_lastReadMillis = now;

	float temperatureC = 0.0f;
	float humidity = 0.0f;
	uint8_t errorCode = read_dht(temperatureC, humidity, _pin, _sensorModel);
	if (errorCode == 0) {
		_connected = true;
		_temperatureC = temperatureC;
		_humidityPct = humidity;
	} else if (errorCode != 1) {
		_connected = false;
	}
}

void DhtCommon::appendPayload(JsonObject payload) const {
	JsonObject dht22 = payload["DHT22"].to<JsonObject>();
	dht22["sensor_connected"] = _connected;
	dht22["temperature_c"] = _temperatureC;
	dht22["humidity_pct"] = _humidityPct;
}

float DhtCommon::getTempC() const {
	return _temperatureC;
}

float DhtCommon::getHumidPct() const {
	return _humidityPct;
}

bool DhtCommon::isConnected() const {
	return _connected;
}

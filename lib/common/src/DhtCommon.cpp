#include "DhtCommon.h"

DhtCommon::DhtCommon(uint8_t pin, uint8_t sensorModel, unsigned long readIntervalMs)
	: _pin(pin), _sensorModel(sensorModel), _readIntervalMs(readIntervalMs)
#if defined(ARDUINO_ARCH_RP2040)
	  ,
	  _dht(pin, sensorModel)
#endif
{
}

#if defined(ARDUINO_ARCH_RP2040)
void DhtCommon::begin() {
	_dht.begin();
}
#endif

void DhtCommon::poll() {
	unsigned long now = millis();
	if ((now - _lastReadMillis) < _readIntervalMs) return;
	_lastReadMillis = now;

#if defined(ARDUINO_ARCH_ESP32)
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
#elif defined(ARDUINO_ARCH_RP2040)
	float temperatureC = _dht.readTemperature();
	float humidity = _dht.readHumidity();
	if (isnan(humidity) || isnan(temperatureC)) {
		_connected = false;
	} else {
		_connected = true;
		_temperatureC = temperatureC;
		_humidityPct = humidity;
	}
#endif
}

void DhtCommon::appendPayload(JsonObject payload) const {
	JsonObject dht22 = payload["DHT22"].to<JsonObject>();
	dht22["sensor_connected"] = _connected;
	dht22["temperature_c"] = _temperatureC;
	dht22["humidity_pct"] = _humidityPct;
}

bool DhtCommon::isConnected() const {
	return _connected;
}

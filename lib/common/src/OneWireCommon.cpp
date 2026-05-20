#include "OneWireCommon.h"

OneWireCommon::OneWireCommon(uint8_t pin)
	: _bus(pin), _sensors(&_bus) {}

void OneWireCommon::begin() {
	_sensors.begin();
	_sensors.setWaitForConversion(false);
	_addressKnown = _sensors.getAddress(_address, 0);
	if (!_addressKnown) Serial.println("OneWire sensor address not found at startup");
	_sensors.requestTemperatures();
}

void OneWireCommon::pollBlocking() {
	_sensors.requestTemperatures();
	unsigned long start = millis();
	float temp = DEVICE_DISCONNECTED_C;
	while ((millis() - start) < 3000) {
		if (_sensors.isConversionComplete()) {
			temp = _sensors.getTempCByIndex(0);
			if (temp != DEVICE_DISCONNECTED_C) break;
			_sensors.requestTemperatures();
		}
		yield();
	}
	if (temp == DEVICE_DISCONNECTED_C) Serial.println("OneWire device disconnected.");
	_temperatureC = temp;
	_conversionInProgress = false;
}

void OneWireCommon::pollAsync() {
	if (_conversionInProgress && _sensors.isConversionComplete()) {
		_conversionInProgress = false;
		if (!_addressKnown) _addressKnown = _sensors.getAddress(_address, 0);
		_temperatureC = _addressKnown ? _sensors.getTempC(_address) : DEVICE_DISCONNECTED_C;
		if (_temperatureC == DEVICE_DISCONNECTED_C) {
			if (_addressKnown && !_sensors.isConnected(_address)) _addressKnown = false;
			Serial.println("OneWire device disconnected.");
		}
	} else if (!_conversionInProgress) {
		_sensors.requestTemperatures();
		_conversionInProgress = true;
	}
}

void OneWireCommon::appendPayload(JsonObject payload) const {
	JsonObject oneWireTemp = payload["OneWireTemp"].to<JsonObject>();
	oneWireTemp["sensor_connected"] = isConnected();
	oneWireTemp["temperature_c"] = _temperatureC;
}

float OneWireCommon::getTempC() const {
	return _temperatureC;
}

bool OneWireCommon::isConnected() const {
	return _temperatureC != DEVICE_DISCONNECTED_C;
}

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

void OneWireCommon::poll(bool blocking) {
	_sensors.requestTemperatures();
	if (blocking) {
		unsigned long start = millis();
		float temp = DEVICE_DISCONNECTED_C;
		while ((millis() - start) < 3000) {
			if (_sensors.isConversionComplete()) {
				temp = _sensors.getTempC(_address);
				if (temp != DEVICE_DISCONNECTED_C) break;
				_sensors.requestTemperatures();
			}
			yield();
		}
		_temperatureC = temp;
	} else if (_sensors.isConversionComplete()) {
		if (!_addressKnown) _addressKnown = _sensors.getAddress(_address, 0);
		_temperatureC = _addressKnown ? _sensors.getTempC(_address) : DEVICE_DISCONNECTED_C;
		if (_temperatureC == DEVICE_DISCONNECTED_C) {
			if (_addressKnown && !_sensors.isConnected(_address)) _addressKnown = false;
			Serial.println("OneWire device disconnected.");
		}
	}
}

void OneWireCommon::appendPayload(JsonObject payload) const {
	JsonObject oneWireTemp = payload["OneWireTemp"].to<JsonObject>();
	oneWireTemp["sensor_connected"] = _addressKnown && _temperatureC != DEVICE_DISCONNECTED_C;
	oneWireTemp["temperature_c"] = _temperatureC;
}

float OneWireCommon::getTempC() const {
	return _temperatureC;
}

bool OneWireCommon::isConnected() const {
	return _addressKnown && _temperatureC != DEVICE_DISCONNECTED_C;
}

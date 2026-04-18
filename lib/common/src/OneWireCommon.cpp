#include "OneWireCommon.h"

OneWireCommon::OneWireCommon(uint8_t pin)
	: _bus(pin), _sensors(&_bus) {}

void OneWireCommon::begin() {
	_sensors.begin();
	_addressKnown = _sensors.getAddress(_address, 0);
	if (!_addressKnown && Serial) Serial.println("OneWire sensor address not found at startup");
	_sensors.setWaitForConversion(false);
	_sensors.requestTemperatures();
}

void OneWireCommon::poll(bool blocking) {
	if (blocking) {
		_sensors.requestTemperatures();
		unsigned long start = millis();
		while (!_sensors.isConversionComplete() && (millis() - start) < 10000) yield();
	} else if (!_sensors.isConversionComplete()) {
		return;
	}

	if (!_addressKnown) _addressKnown = _sensors.getAddress(_address, 0);
	_temperatureC = _addressKnown ? _sensors.getTempC(_address) : DEVICE_DISCONNECTED_C;
	if (_temperatureC == DEVICE_DISCONNECTED_C) {
		if (_addressKnown && !_sensors.isConnected(_address)) _addressKnown = false;
		Serial.println("OneWire device disconnected.");
	}

	_sensors.requestTemperatures();
}

void OneWireCommon::appendPayload(JsonObject payload) const {
	JsonObject oneWireTemp = payload["OneWireTemp"].to<JsonObject>();
	oneWireTemp["sensor_connected"] = _addressKnown;
	oneWireTemp["temperature_c"] = _temperatureC;
}

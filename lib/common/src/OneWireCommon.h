#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <DallasTemperature.h>
#include <OneWire.h>

/*!
 * @brief Read DS18B20-style temperature data from a OneWire bus.
 */
class OneWireCommon {
public:
	/*!
	 * @brief Create a OneWire temperature helper for a specific GPIO pin.
	 * @param pin
	 *         GPIO pin connected to the OneWire data bus.
	 */
	explicit OneWireCommon(uint8_t pin);
	/*!
	 * @brief Initialize the OneWire bus and start the first conversion.
	 */
	void begin();
	/*!
	 * @brief Update cached temperature data from the OneWire sensor. Waits for conversion to completion.
	 */
	void pollBlocking();
	/*!
	 * @brief Update cached temperature data from the OneWire sensor. Returns early if conversion is still in progress.
	 */
	void pollAsync();
	/*!
	 * @brief Append cached OneWire values into a JSON payload.
	 * @param payload
	 *         JSON object that receives the OneWireTemp sub-object fields.
	 */
	void appendPayload(JsonObject payload) const;
	/*!
	 * @brief Return the last known temperature in Celsius.
	 */
	float getTempC() const;
	/*!
	 * @brief Report whether a OneWire temperature sensor is connected.
	 */
	bool isConnected() const;

private:
	OneWire _bus;
	DallasTemperature _sensors;
	DeviceAddress _address;
	bool _conversionInProgress = false;
	bool _addressKnown = false;
	float _temperatureC = 0.0f;
};

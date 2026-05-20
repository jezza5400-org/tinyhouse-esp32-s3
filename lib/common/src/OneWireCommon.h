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
	 * @brief Initialize the OneWire bus, probe the first sensor address, and prime the first conversion.
	 */
	void begin();

	/*!
	 * @brief Request a temperature conversion and block until it completes (~750ms at 12-bit resolution).
	 *        Updates the cached temperature from the first sensor on the bus.
	 */
	void pollBlocking();

	/*!
	 * @brief Advance the legacy async polling state machine.
	 */
	void pollAsync();

	/*!
	 * @brief Append cached OneWire values into a JSON payload.
	 * @param payload
	 *         JSON object that receives the OneWireTemp sub-object fields:
	 *         sensor_connected (bool) and temperature_c (float).
	 */
	void appendPayload(JsonObject payload) const;

	/*!
	 * @brief Return the last known temperature in Celsius.
	 *        Returns DEVICE_DISCONNECTED_C (-127.0) if the sensor is absent.
	 */
	float getTempC() const;

	/*!
	 * @brief Return true if the last poll returned a valid temperature.
	 */
	bool isConnected() const;

private:
	OneWire _bus;
	DallasTemperature _sensors;
	DeviceAddress _address;
	bool _conversionInProgress = false;
	bool _addressKnown = false;
	float _temperatureC = DEVICE_DISCONNECTED_C;
};

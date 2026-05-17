#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <dhtESP32-rmt.h>

/*!
 * @brief Cross-platform DHT temperature and humidity helper.
 */
class DhtCommon {
public:
	/*!
	 * @brief Create a DHT sensor helper.
	 * @param pin
	 *         GPIO pin connected to the DHT data line.
	 * @param sensorModel
	 *         DHT sensor model constant (for example DHT22).
	 * @param readIntervalMs
	 *         Minimum interval between sensor reads in milliseconds.
	 */
	DhtCommon(uint8_t pin, uint8_t sensorModel = DHT22, unsigned long readIntervalMs = 2500);
	/*!
	 * @brief Read sensor data when the configured interval has elapsed.
	 */
	void poll();
	/*!
	 * @brief Append cached DHT values into a JSON payload.
	 * @param payload
	 *         JSON object that receives the DHT22 sub-object fields.
	 */
	void appendPayload(JsonObject payload) const;
	/*!
	 * @brief Report whether the sensor is currently connected.
	 */
	bool isConnected() const;

private:
	uint8_t _pin;
	uint8_t _sensorModel;
	unsigned long _readIntervalMs;
	unsigned long _lastReadMillis = 0;
	float _temperatureC = 0.0f;
	float _humidityPct = 0.0f;
	bool _connected = false;
};

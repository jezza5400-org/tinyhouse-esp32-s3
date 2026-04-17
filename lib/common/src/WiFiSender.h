#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#if defined(ARDUINO_ARCH_RP2040)
#include <WiFiNINA.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

/*!
 * @brief  HTTPS client helper for posting JSON payloads over WiFi.
 */
class WiFiSender {
public:
	/*!
	 * @brief  Create an HTTPS sender for posting sensor data.
	 * @param  host
	 *         Destination host name or IP.
	 * @param  port
	 *         Destination TCP port.
	 * @param  thing
	 *         Dweet thing name used in the request path.
	 */
	WiFiSender(const char *host, uint16_t port, const char *thing);
	/*!
	 * @brief  Send a JSON document to the configured HTTPS endpoint.
	 * @param  payloadDoc
	 *         JSON document serialized as the POST body.
	 * @return true when a TCP connection and request write complete successfully.
	 */
	bool send(const JsonDocument &payloadDoc);

private:
	const char *_host;
	uint16_t _port;
	const char *_thing;
	WiFiClient _client;
};

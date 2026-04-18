#include "WiFiSender.h"

WiFiSender::WiFiSender(const char *host, uint16_t port, const char *thing)
	: _host(host), _port(port), _thing(thing) {}

bool WiFiSender::send(const JsonDocument &payloadDoc) {
	Serial.println(String("Connecting to ") + _host + ":" + String(_port));
	_client.stop();

	if (!_client.connect(_host, _port)) {
		Serial.println("Connection failed");
		return false;
	}

	size_t payloadSize = measureJson(payloadDoc);

	_client.println(String("POST /dweet/for/") + _thing + " HTTP/1.1");
	_client.println(String("Host: ") + _host + ":" + String(_port));
	_client.println("Content-Type: application/json");
	_client.println(String("Content-Length: ") + payloadSize);
	_client.println("Connection: close");
	_client.println();
	serializeJson(payloadDoc, _client);

	unsigned long start = millis();
	while ((millis() - start) < 1000 && (_client.connected() != 0 || _client.available() != 0)) {
		while (_client.available() != 0) {
			Serial.write(static_cast<uint8_t>(_client.read()));
			start = millis();
		}
		yield();
	}

	_client.stop();
	return true;
}

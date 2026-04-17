#include "WiFiSender.h"

WiFiSender::WiFiSender(const char *host, uint16_t port, const char *thing)
	: _host(host), _port(port), _thing(thing) {}

bool WiFiSender::send(const JsonDocument &payloadDoc) {
	if (Serial) Serial.println(String("Connecting to ") + _host + ":" + String(_port));
	_client.stop();

	if (!_client.connect(_host, _port)) {
		if (Serial) Serial.println("Connection failed");
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
	while ((millis() - start) < 1000) {
		while (_client.available()) {
			int incoming = _client.read();
			if (Serial && incoming >= 0) Serial.write(static_cast<uint8_t>(incoming));
			start = millis();
		}
		if (!_client.connected() && !_client.available()) break;
		delay(1);
	}

	_client.stop();
	return true;
}

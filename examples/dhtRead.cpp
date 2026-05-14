#include <Arduino.h>
#include <dhtESP32-rmt.h>

#define DHT_SENSOR_MODEL DHT22

constexpr uint8_t DHT_PIN = 8;
constexpr unsigned long DHT_READ_INTERVAL_MS = 2000;

const char *dhtErrorToString(uint8_t errorCode) {
	switch (errorCode) {
		case 0:
			return "OK";
		case 1:
			return "TOO_SOON";
		case 2:
			return "DRIVER";
		case 3:
			return "TIMEOUT";
		case 4:
			return "NACK";
		case 5:
			return "BAD_DATA";
		case 6:
			return "CHECKSUM";
		case 7:
			return "UNDERFLOW";
		case 8:
			return "OVERFLOW";
		default:
			return "UNKNOWN";
	}
}

void setup() {
	Serial.begin(115200);
	while (!Serial) yield();
	Serial.println("DHT pin: " + String(DHT_PIN) + "\nDHT Test");
}

void loop() {
	static unsigned long lastReadMs = 0;
	if ((millis() - lastReadMs) < DHT_READ_INTERVAL_MS) return;
	lastReadMs = millis();

	float humidity = NAN;
	float temperatureC = NAN;

	uint8_t errorCode = read_dht(temperatureC, humidity, DHT_PIN, DHT_SENSOR_MODEL);
	if (errorCode != 0) {
		Serial.println("DHT read error: " + String(errorCode) + " (" + dhtErrorToString(errorCode) + ")");
		return;
	}

	Serial.println("Humidity: " + String(humidity) + " %, Temp: " + String(temperatureC) + " Celsius");
}

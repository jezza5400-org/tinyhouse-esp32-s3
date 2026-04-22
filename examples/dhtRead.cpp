/*
   Cross-target DHT example:
   - ESP32-S3: htmltiger dhtESP32-rmt (RMT/non-blocking)
   - RP2040: standard DHT.h
*/

#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
#include <dhtESP32-rmt.h>
#elif defined(ARDUINO_ARCH_RP2040)
#include <DHT.h>
#else
#error "This example supports ARDUINO_ARCH_ESP32 and ARDUINO_ARCH_RP2040 only."
#endif

constexpr uint8_t DHT_PIN = 19;
constexpr unsigned long DHT_READ_INTERVAL_MS = 2000;

// Set to DHT11 if your sensor is DHT11.
#define DHT_SENSOR_MODEL DHT22

#if defined(ARDUINO_ARCH_RP2040)
DHT dht(DHT_PIN, DHT_SENSOR_MODEL);
#endif

#if defined(ARDUINO_ARCH_ESP32)
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
#endif

void setup() {
	Serial.begin(9600);
	while (!Serial) yield();
	Serial.print("DHT pin: ");
	Serial.println(DHT_PIN);
#if DHT_SENSOR_MODEL == DHT22
	Serial.println("DHT model: DHT22/AM2302");
#elif DHT_SENSOR_MODEL == DHT11
	Serial.println("DHT model: DHT11");
#else
	Serial.println("DHT model: custom");
#endif

#if defined(ARDUINO_ARCH_RP2040)
	dht.begin();
#endif
}

void loop() {
	static unsigned long lastReadMs = 0;
	if ((millis() - lastReadMs) < DHT_READ_INTERVAL_MS) return;
	lastReadMs = millis();

	float humidity = NAN;
	float temperatureC = NAN;

#if defined(ARDUINO_ARCH_ESP32)
	uint8_t errorCode = read_dht(temperatureC, humidity, DHT_PIN, DHT_SENSOR_MODEL);
	if (errorCode != 0) {
		Serial.print("DHT read error ");
		Serial.print(errorCode);
		Serial.print(" (");
		Serial.print(dhtErrorToString(errorCode));
		Serial.println(")");
		return;
	}
#elif defined(ARDUINO_ARCH_RP2040)
	humidity = dht.readHumidity();
	temperatureC = dht.readTemperature();
	if (isnan(humidity) || isnan(temperatureC)) {
		Serial.println("DHT read failed");
		return;
	}
#endif

	Serial.print("Humidity: ");
	Serial.print(humidity);
	Serial.print(" %, Temp: ");
	Serial.print(temperatureC);
	Serial.println(" Celsius");
}

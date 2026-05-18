#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>

OneWire oneWire(17);
DallasTemperature sensors(&oneWire);
unsigned long start;

void setup(void) {
	Serial.begin(115200);
	while (!Serial) yield();
	delay(1000);
	Serial.println("Dallas Temperature IC Control Library Demo");
	sensors.begin();
	sensors.setWaitForConversion(true);
	Serial.printf("Current Resolution: %d bits\r\n", sensors.getResolution());
}

void loop(void) {
	start = millis();
	Serial.print("Requesting temperatures...");
	sensors.requestTemperatures();
	Serial.printf("DONE\nTemperature is: %.4f, Found in: %lu\r\n", sensors.getTempCByIndex(0), millis() - start);
}

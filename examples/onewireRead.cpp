#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(4);
DallasTemperature sensors(&oneWire);
unsigned long start;

void setup(void) {
	Serial.begin(9600);
	while (!Serial) yield();
	Serial.println("Dallas Temperature IC Control Library Demo");
	sensors.begin();
	sensors.setWaitForConversion(true);
	Serial.println("Current Resolution: " + String(sensors.getResolution()) + " bits");
}

void loop(void) {
	start = millis();
	Serial.print("Requesting temperatures...");
	sensors.requestTemperatures();
	Serial.println("DONE\nTemperature is: " + String(sensors.getTempCByIndex(0)) + ", Found in: " + String((millis() - start)));
}

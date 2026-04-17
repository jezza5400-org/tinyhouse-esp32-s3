#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(4);
DallasTemperature sensors(&oneWire);

void setup(void) {
	Serial.begin(9600);
	while (!Serial);
	Serial.println("Dallas Temperature IC Control Library Demo");
	sensors.begin();
}

void loop(void) {
	// call sensors.requestTemperatures() to issue a global temperature
	// request to all devices on the bus
	Serial.print(" Requesting temperatures...");
	sensors.requestTemperatures(); // Send the command to get temperatures
	Serial.println("DONE");

	Serial.print("Temperature is: ");
	Serial.print(sensors.getTempCByIndex(0)); // Why "byIndex"?
	// You can have more than one IC on the same bus.
	// 0 refers to the first IC on the wire
	delay(1000);
}

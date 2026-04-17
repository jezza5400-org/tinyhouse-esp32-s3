#include <Arduino.h>
#include <WiFiNINA.h>

void setup()
{
	pinMode(LEDR, OUTPUT);
	pinMode(LEDG, OUTPUT);
	pinMode(LEDB, OUTPUT);
	pinMode(2, OUTPUT); // Digital pin 2
}

void loop()
{
	for (int i = 0; i < 8; i++)
	{
		digitalWrite(LEDR, (i & 0b100) ? HIGH : LOW);
		digitalWrite(LEDG, (i & 0b010) ? HIGH : LOW);
		digitalWrite(LEDB, (i & 0b001) ? HIGH : LOW);
		digitalWrite(2, !digitalRead(2));
		delay(1000);
	}
}

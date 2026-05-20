#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 48
#define NUM_LEDS 1

CRGB leds[NUM_LEDS];

void setup() {
	FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(128);
	Serial.begin(115200);
	leds[0] = CHSV(0, 255, 255);
	FastLED.show();
}

void loop() {
	static uint8_t hue = 0;
	leds[0] = CHSV(hue, 255, 255);
	FastLED.show();
	Serial.printf("Hue: %hhu\r\n", hue);
	hue++;
	delay(100);
}

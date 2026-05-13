#include <FastLED.h>

#define LED_PIN 48
#define NUM_LEDS 1

CRGB leds[NUM_LEDS];

void setup() {
	FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(128);
}

void loop() {
	static uint8_t hue = 0;

	leds[0] = CHSV(hue, 255, 255);
	FastLED.show();

	hue++;
	delay(100);
}

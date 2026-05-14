#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 48
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

void setup() {
	FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(128);
	Serial.begin(115200);
	Serial0.begin(115200);
	leds[0] = CHSV(0, 255, 255);
	FastLED.show();
	while (!Serial) yield();
}

void loop() {
	static uint8_t hue = 0;
	leds[0] = CHSV(hue, 255, 255);
	FastLED.show();
	Serial.println("Hue from Serial  " + String(hue));
	Serial0.println("Hue from Serial0 " + String(hue));
	hue++;
	delay(100);
}

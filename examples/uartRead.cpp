#include <Arduino.h>
#include <ArduinoJson.h>

#include <VeDirectParser.h>

namespace {
constexpr uint32_t VE_DIRECT_BAUD = 19200;
constexpr uint8_t VE_DIRECT_RX_PIN = 18;
constexpr uint8_t VE_DIRECT_TX_PIN = 17;

VeDirectParser veParser;
}

void setup() {
	Serial.begin(115200);

	while (!Serial) yield();
	delay(1000);

	Serial1.begin(VE_DIRECT_BAUD, SERIAL_8N1, VE_DIRECT_RX_PIN, VE_DIRECT_TX_PIN);
	veParser.begin();

	Serial.println("VE.Direct UART reader ready");
	Serial.printf("Serial1 RX GPIO %u, TX GPIO %u at %lu baud\n", VE_DIRECT_RX_PIN, VE_DIRECT_TX_PIN, static_cast<unsigned long>(VE_DIRECT_BAUD));
}

void loop() {
	veParser.process(Serial1);

	if (!veParser.hasFreshFrame()) return;

	JsonDocument frameDoc;
	JsonObject victron = frameDoc["victron"].to<JsonObject>();
	veParser.copyFieldsTo(victron);

	Serial.println("VE.Direct frame:");
	serializeJsonPretty(frameDoc, Serial);
	Serial.println();

	veParser.markFrameConsumed();
}

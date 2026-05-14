#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_psram.h"
#include "esp_system.h"
#include <Arduino.h>

void setup() {
	Serial.begin(115200);

	delay(5000);

	while (!Serial) yield();

	Serial.println("\n=== ESP32‑S3 Hardware Validation ===");

	// CHIP INFO
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);

	Serial.printf("Chip Model: %s\n", ESP.getChipModel());
	Serial.printf("Cores: %d\n", chip_info.cores);
	Serial.printf("Revision: %d\n", chip_info.revision);

	// FLASH SIZE
	uint32_t flash_size = 0;
	esp_flash_t *chip = esp_flash_default_chip; // S3 requires explicit chip pointer
	esp_flash_get_size(chip, &flash_size);

	Serial.printf("Detected Flash Size: %u MB\n", flash_size / (1024 * 1024));

	// PSRAM
	size_t psram_size = esp_psram_get_size();
	Serial.printf("Detected PSRAM Size: %u MB\n", psram_size / (1024 * 1024));

	if (psram_size == 0) {
		Serial.println("PSRAM ERROR: Not detected!");
	} else {
		Serial.println("PSRAM OK: Detected.");
	}

	// FLASH SPEED & MODE
	Serial.printf("Flash Speed: %u Hz\n", ESP.getFlashChipSpeed());
	Serial.printf("Flash Mode: %u\n", ESP.getFlashChipMode());

	// VALIDATION
	Serial.println("\n=== Validation Results ===");

	if (flash_size == 16 * 1024 * 1024) {
		Serial.println("Flash size matches N16R8 (16MB)");
	} else {
		Serial.println("Flash size mismatch! Check board JSON.");
	}

	if (psram_size == 8 * 1024 * 1024) {
		Serial.println("PSRAM size matches N16R8 (8MB)");
	} else {
		Serial.println("PSRAM size mismatch! Check board JSON.");
	}

	Serial.println("\nValidation complete.");
}

void loop() {
	vTaskDelay(portMAX_DELAY);
}

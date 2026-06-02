#pragma once

#include <Arduino.h>
#include <stdint.h>

class VeDirectParser {
public:
	void begin();
	void poll();
	bool hasFreshFrame() const;

	/*!
	 * @brief Copy parsed fields from the latest frame into a JSON object.
	 * @param target
	 *         Destination JSON object to receive key/value fields.
	 */
	void copyFieldsTo(JsonObject target) const;

	/*!
	 * @brief Mark the latest frame as consumed.
	 */
	void markFrameConsumed();

	/*!
	 * @brief Clear cached telemetry and invalidate the latest parsed frame.
	 */
	void clearTelemetry();

	/*!
	 * @brief Return true when the latest validated frame is newer than maxAgeMs.
	 */
	bool isTelemetryFresh(uint32_t maxAgeMs) const;

	/*!
	 * @brief Returns the last known battery voltage in mV.
	 */
	uint16_t getBattVoltage() const;

	/*!
	 * @brief Returns the last known panel voltage in mV.
	 */
	uint16_t getPanelVoltage() const;

	/*!
	 * @brief Returns the last known battery current in mA.
	 */
	int32_t getBattCurrentMa() const;

	/*!
	 * @brief Returns the last known panel power in W.
	 */
	uint16_t getPanelPowerW() const;

private:
	enum class ParseState : uint8_t {
		Idle,
		SawCR,
		InFrame,
	};

	/*!
	 * @brief Returns the last known battery current in mA.
	 */
	int16_t getBattCurrentMa() const;

	/*!
	 * @brief Returns the last known panel power in W.
	 */
	uint16_t getPanelPowerW() const;

private:
	enum class ParseState : uint8_t {
		Idle,
		ReadingKey,
		ReadingValue,
	};

	void handleLine(const String &line);

	ParseState _state = ParseState::Idle;
	String _key;
	String _value;
	bool _hasFreshFrame = false;
	uint32_t _lastValidFrameAtMs = 0;
	uint16_t _battVoltage = 0;
	uint16_t _panelVoltage = 0;
	int32_t _battCurrentMa = 0;
	uint16_t _panelPowerW = 0;
};

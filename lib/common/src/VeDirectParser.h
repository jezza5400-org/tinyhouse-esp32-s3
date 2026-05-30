#pragma once

#include <Arduino.h>
#include <stdint.h>

class VeDirectParser {
public:
	void begin();
	void poll();
	bool hasFreshFrame() const;

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
	uint16_t _battVoltage = 0;
	uint16_t _panelVoltage = 0;
	int16_t _battCurrentMa = 0;
	uint16_t _panelPowerW = 0;
};
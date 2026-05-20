#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

/*!
 * @brief Incremental parser for VE.Direct text frames.
 */
class VeDirectParser {
public:
	/*!
	 * @brief Reset parser state and clear buffered frame data.
	 */
	void begin();

	/*!
	 * @brief Consume bytes from a stream and parse complete frames.
	 * @param serialStream
	 *         Stream that provides VE.Direct serial data.
	 */
	void process(Stream &serialStream);

	/*!
	 * @brief Report whether a new validated frame is available.
	 * @return true when a fresh frame is ready to be consumed.
	 */
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
	 * @brief Returns the last known battery voltage in mV.
	 */
	uint16_t getBattVoltage() const;

	/*!
	 * @brief Returns the last known panel voltage in mV.
	 */
	uint16_t getPanelVoltage() const;

private:
	enum class ParseState : uint8_t {
		Idle,
		SawCR,
		InFrame,
	};

	/*!
	 * @brief Clear or initialize the in-progress frame and update parser state.
	 */
	void resetWorkingFrame();

	/*!
	 * @brief Clear or initialize the in-progress frame and update parser state.
	 */
	void startFrame();

	/*!
	 * @brief Validate checksum, publish the completed frame, and reset state.
	 */
	void finalizeWorkingFrame();

	/*!
	 * @brief Parse one tab-delimited line into the working frame.
	 * @param line
	 *         Raw line text without carriage return/newline characters.
	 */
	void handleLine(const String &line);

	JsonDocument _workingFrameDoc;
	JsonDocument _latestFrameDoc;
	String _lineBuffer;
	ParseState _state = ParseState::Idle;
	uint8_t _checksum = 0;
	bool _hasFreshFrame = false;
	uint16_t _battVoltage = 0;
	uint16_t _panelVoltage = 0;
};

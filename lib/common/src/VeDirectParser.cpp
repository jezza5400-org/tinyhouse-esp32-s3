#include "VeDirectParser.h"

void VeDirectParser::begin() {
	resetWorkingFrame();
	_latestFrameDoc.clear();
	_hasFreshFrame = false;
}

void VeDirectParser::resetWorkingFrame() {
	_workingFrameDoc.clear();
	_lineBuffer = "";
	_checksum = 0;
	_state = ParseState::Idle;
}

void VeDirectParser::startFrame() {
	resetWorkingFrame();
	_workingFrameDoc["fields"].to<JsonObject>();
	_checksum = static_cast<uint8_t>('\r' + '\n');
	_state = ParseState::InFrame;
}

void VeDirectParser::finalizeWorkingFrame() {
	if (_checksum != 0) {
		Serial.println("VE.Direct frame dropped (checksum mismatch)");
		resetWorkingFrame();
		return;
	}

	if (_workingFrameDoc.overflowed()) {
		Serial.println("VE.Direct frame dropped (JSON overflow)");
		resetWorkingFrame();
		return;
	}

	_latestFrameDoc.clear();
	JsonObject latestFields = _latestFrameDoc["fields"].to<JsonObject>();
	for (JsonPairConst kv : _workingFrameDoc["fields"].as<JsonObjectConst>()) {
		latestFields[kv.key()] = kv.value();
	}

	_hasFreshFrame = true;
	resetWorkingFrame();
}

void VeDirectParser::handleLine(const String &line) {
	if (line.length() == 0) return;

	int tabIndex = line.indexOf('\t');
	if (tabIndex <= 0) return;

	String key = line.substring(0, tabIndex);
	if (key == "Checksum") return; // Checksum byte is validated as soon as it arrives in process().

	String value = line.substring(tabIndex + 1);
	_workingFrameDoc["fields"][key] = value;
}

void VeDirectParser::process(Stream &serialStream) {
	while (serialStream.available()) {
		int rawIncoming = serialStream.read();
		if (rawIncoming < 0) break;
		uint8_t incoming = static_cast<uint8_t>(rawIncoming);
		char c = static_cast<char>(incoming);

		if (_state != ParseState::InFrame) {
			if (c == '\r') {
				_state = ParseState::SawCR;
			} else if (c == '\n' && _state == ParseState::SawCR) {
				startFrame();
			} else {
				_state = ParseState::Idle;
			}
			continue;
		}

		// `_checksum + incoming` is evaluated then cast which keeps the low 8 bits, so the result is the modulo 256.
		_checksum = static_cast<uint8_t>(_checksum + incoming);

		if (_lineBuffer == "Checksum\t") {
			finalizeWorkingFrame();
			_lineBuffer = "";
		} else if (c == '\n') {
			handleLine(_lineBuffer);
			_lineBuffer = "";
		} else if (c != '\r') {
			_lineBuffer += c;
		}
	}
}

bool VeDirectParser::hasFreshFrame() const {
	return _hasFreshFrame;
}

void VeDirectParser::copyFieldsTo(JsonObject target) const {
	for (JsonPairConst kv : _latestFrameDoc["fields"].as<JsonObjectConst>()) {
		target[kv.key()] = kv.value();
	}
}

void VeDirectParser::markFrameConsumed() {
	_hasFreshFrame = false;
}

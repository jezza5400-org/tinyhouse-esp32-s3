#include "VeDirectParser.h"

void VeDirectParser::begin() {
	_workingFrameDoc.clear();
	_latestFrameDoc.clear();
	_lineBuffer = "";
	_frameActive = false;
	_checksum = 0;
	_sawIdleCarriageReturn = false;
	_hasFreshFrame = false;
}

void VeDirectParser::resetWorkingFrame() {
	_workingFrameDoc.clear();
	_lineBuffer = "";
	_frameActive = false;
	_checksum = 0;
	_sawIdleCarriageReturn = false;
}

void VeDirectParser::startWorkingFrame() {
	_workingFrameDoc.clear();
	_workingFrameDoc["fields"].to<JsonObject>();
	_lineBuffer = "";
	_frameActive = true;
	_checksum = static_cast<uint8_t>(static_cast<uint8_t>('\r') + static_cast<uint8_t>('\n'));
}

void VeDirectParser::finalizeWorkingFrame() {
	// VE.Direct frames are valid when the 8-bit sum of all bytes is 0.
	// `_checksum` stores that running 8-bit sum. Because the variable is `uint8_t`,
	// each assignment keeps only the low 8 bits, which is equivalent to modulo 256.
	// So `_checksum != 0` means the frame sum is non-zero in mod-256 arithmetic.
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

		if (!_frameActive) {
			if (c == '\r') {
				_sawIdleCarriageReturn = true;
			} else if (c == '\n' && _sawIdleCarriageReturn) {
				startWorkingFrame();
				_sawIdleCarriageReturn = false;
			} else {
				_sawIdleCarriageReturn = false;
			}
			continue;
		}

		// `_checksum + incoming` is evaluated using integer promotion (as `int`),
		// then cast back to `uint8_t`. That cast keeps the low 8 bits, so the result
		// is exactly the running sum modulo 256.
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

#include "WiFiCommon.h"

namespace {
String urlEncode(const String &input) {
	String encoded;
	encoded.reserve(input.length() * 3);

	for (size_t i = 0; i < input.length(); ++i) {
		const uint8_t c = static_cast<uint8_t>(input[i]);
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
			encoded += static_cast<char>(c);
		} else {
			static constexpr char hex[] = "0123456789ABCDEF";
			encoded += '%';
			encoded += hex[c >> 4];
			encoded += hex[c & 0x0F];
		}
	}

	return encoded;
}

String variantToString(JsonVariantConst value) {
	if (value.is<bool>()) {
		return value.as<bool>() ? "true" : "false";
	}

	if (value.is<const char *>()) {
		const char *text = value.as<const char *>();
		return text == nullptr ? "" : text;
	}

	return value.as<String>();
}

void appendQueryParameters(String &queryString, JsonVariantConst value, const String &prefix) {
	if (value.is<JsonObjectConst>()) {
		for (JsonPairConst pair : value.as<JsonObjectConst>()) {
			String childKey = prefix.length() == 0 ? String(pair.key().c_str()) : prefix + "[" + pair.key().c_str() + "]";
			appendQueryParameters(queryString, pair.value(), childKey);
		}
		return;
	}

	if (value.is<JsonArrayConst>()) {
		size_t index = 0;
		for (JsonVariantConst item : value.as<JsonArrayConst>()) {
			String childKey = prefix.length() == 0 ? String(index) : prefix + "[" + String(index) + "]";
			appendQueryParameters(queryString, item, childKey);
			++index;
		}
		return;
	}

	if (prefix.length() == 0) {
		return;
	}

	if (queryString.length() > 0) {
		queryString += '&';
	}

	queryString += urlEncode(prefix);
	queryString += '=';
	queryString += urlEncode(variantToString(value));
}

String buildQueryString(JsonVariantConst payload) {
	String queryString;
	queryString.reserve(256);
	appendQueryParameters(queryString, payload, "");
	return queryString;
}
}

WiFiCommon::WiFiCommon(const char *host, const char *thing)
	: _host(host), _thing(thing) {
	_client.setCACert(isrgRootX1);
}

bool WiFiCommon::send(const JsonDocument &payload) {
	return sendQueryString(buildQueryString(payload.as<JsonVariantConst>()));
}

bool WiFiCommon::send(const char *payload) {
	if (payload == nullptr) {
		Serial.println("Refusing to send empty dweet query string");
		return false;
	}

	return sendQueryString(String(payload));
}

bool WiFiCommon::sendQueryString(const String &queryString) {
	if (queryString.isEmpty()) {
		Serial.println("Refusing to send empty dweet query string");
		return false;
	}

	Serial.printf("Connecting to %s\r\n", _host);

	HTTPClient https;
	https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
	String requestUrl = String(_host) + _thing + "?" + queryString;
	https.begin(_client, requestUrl);
	Serial.printf("Sending HTTPS GET...\n%s\r\n", requestUrl.c_str());
	int httpCode = https.GET();
	bool success = false;

	if (httpCode > 0) {
		Serial.printf("HTTP %d\n", httpCode);
		Serial.printf("%s\r\n", https.getString().c_str());
		success = httpCode >= 200 && httpCode < 300;
	} else {
		Serial.printf("Request failed: %s\n", https.errorToString(httpCode).c_str());
	}

	https.end();
	_client.stop();
	return success;
}

bool connectWiFi(const char *ssid, const char *password) {
	Serial.printf("Connecting to '%s'...\n", ssid);
	WiFi.begin(ssid, password);

	unsigned long start = millis();
	while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
		Serial.print(".");
		delay(300);
	}

	if (WiFi.status() == WL_CONNECTED) {
		IPAddress localIp = WiFi.localIP();
		Serial.printf("\nConnected! IP: %d.%d.%d.%d\n", localIp[0], localIp[1], localIp[2], localIp[3]);
		return true;
	} else {
		Serial.println("\nFailed to connect.");
		return false;
	}
}

void printWiFiStatus() {
	IPAddress localIp = WiFi.localIP();
	Serial.printf("SSID: %s\r\nIP Address: %d.%d.%d.%d\r\nSignal strength (RSSI): %d dBm\r\n",
		WiFi.SSID().c_str(),
		localIp[0],
		localIp[1],
		localIp[2],
		localIp[3],
		WiFi.RSSI());
}

void resetWiFi() {
	WiFi.disconnect(true, false);
	delay(100);
	WiFi.mode(WIFI_STA);
}

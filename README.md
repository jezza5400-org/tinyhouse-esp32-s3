# Tinyhouse Thermal Mass Controller

Firmware for an ESP32-S3 that keeps the tinyhouse thermal mass at the temperature set in the script. It reads Victron VE.Direct power data plus temperature/humidity sensors, drives a relay-controlled heater, and posts a combined JSON payload over Wi-Fi.

## Pins and Connections

- GPIO7: Relay control output (heater).
- GPIO15: DHT22 data (ambient temperature and humidity).
- GPIO16: OneWire data (DS18B20 thermal mass temperature sensor).
- Serial1 (TX GPIO 17, RX GPIO 18, UART, 19200 baud): Victron VE.Direct RX/TX (uses the board default UART1 pins).
- Serial (USB): Debug logging.

## Logic Flow

1. Setup:

- Start Serial at 9600 for debug and Serial1 at 19200 for VE.Direct.
- Initialize OneWire and DHT sensor helpers.
- Start the VE.Direct parser.
- Configure the relay pin as output and default it to OFF.
- Connect to Wi-Fi and print network status.

2. Loop:

- Poll OneWire and DHT sensors.
- Process VE.Direct bytes from Serial1.
- Control the heater with hysteresis:
  - If the OneWire sensor is missing for 5s, force the relay OFF.
  - If battery voltage or panel voltage is below 11800 mV, force the relay OFF.
  - Otherwise, turn ON at 20C and turn OFF at 24C.
- Reconnect Wi-Fi if disconnected.
- When a fresh VE.Direct frame arrives and both sensors are connected, publish a combined JSON payload every 10s.

The temperature setpoints are defined in src/main.cpp as `T_ON` and `T_OFF`.

## Example Payload

```json
{
  "victron": {
    "PID": "0xA067",
    "FW": "174",
    "SER#": "HQ20351YEAA",
    "V": "12510",
    "I": "-40",
    "VPV": "10",
    "PPV": "0",
    "CS": "0",
    "MPPT": "0",
    "OR": "0x00000001",
    "ERR": "0",
    "LOAD": "ON",
    "IL": "0",
    "H19": "1590",
    "H20": "0",
    "H21": "30",
    "H22": "0",
    "H23": "0",
    "HSDS": "219"
  },
  "OneWireTemp": {
    "sensor_connected": true,
    "temperature_c": 17.375
  },
  "DHT22": {
    "sensor_connected": true,
    "temperature_c": 22.3,
    "humidity_pct": 56
  }
}
```

## Configuration

Wi-Fi credentials and the Dweet thing name live in include/secrets.h.

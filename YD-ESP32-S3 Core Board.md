# YD-ESP32-S3 Core Board

## Introduction

YD-ESP32-S3 core board is designed by VCC-GND Studio. If needed, you can visit [https://www.vcc-gnd.com](https://www.vcc-gnd.com) to purchase one. This board uses the ESP32-S3 chip and is suitable for both IoT application prototypes and production applications. It features two USB connectors: a dedicated USB-to-serial port (CH343P by WCH) and the native ESP32-S3 USB port.

![Top and bottom views](https://github.com/vcc-gnd/YD-ESP32-S3/blob/main/IMG/img1.PNG?raw=true)

This guide will help you get started with the YD-ESP32-S3 quickly, and provides detailed information about this development board.

YD-ESP32-S3 is an entry-level development board based on the Wi-Fi + Bluetooth® LE module ESP32-S3-WROOM-1.

Most of the module pins are broken out to headers on both sides of the board, so developers can easily connect multiple peripherals with jumper wires as needed, and the board can also be plugged into a breadboard for use.

![img](https://github.com/vcc-gnd/YD-ESP32-S3/blob/main/IMG/YD-ESP32-S3.PNG?raw=true)

1. This is a minimal ESP32-S3 core board, using Espressif's ESP32-S3 module.
2. Dedicated LDO circuit for wireless function, eliminating concerns about insufficient power supply.
3. Equipped with one WS2812 RGB LED (note that it is not lit directly through a GPIO).
4. The RST button is used for the external reset function. The Boot button, together with the RST button, can be used to enter bootloader mode after reset and can then be used as a user button, that is GPIO0.
5. Two Type-C ports are featured on the board: one for direct USB connection (GPIO19, GPIO20) and the other for USB-to-serial communication. A hardware USB-to-serial bridge chip (CH343) is also onboard.

## Hardware Overview

![Module Overview](https://github.com/vcc-gnd/YD-ESP32-S3/blob/main/IMG/img2.PNG?raw=true)

| Main Component | Description |
| :--- | :--- |
| ESP32-S3-WROOM-1 | ESP32-S3-WROOM-1 is a general-purpose Wi-Fi + low-power Bluetooth MCU module with rich peripheral interfaces, strong neural-network computing ability and signal-processing capability, designed for the AI and AIoT markets. ESP32-S3-WROOM-1 uses a PCB onboard antenna. |
| 5 V to 3.3 V LDO (5 V to 3.3 V LDO) | Power converter, input 5 V, output 3.3 V, current 1 A. |
| Pin Headers (headers) | All available GPIO pins, except the flash SPI bus, are broken out to the board headers. |
| USB-to-UART Port (USB to UART port) | USB Type-C interface, can be used as the board power input, can flash firmware to the chip, and can also be used as a communication interface through the onboard USB-to-UART bridge. |
| Boot Button (Boot button) | Download button. Hold **Boot** while pressing **Reset** to enter "firmware download" mode and download firmware via serial. After startup, it can be used as a normal input button, using GPIO0. |
| Reset Button (Reset button) | Reset button. |
| USB Port (USB port) | ESP32-S3 USB OTG interface, supports the full-speed USB 1.1 standard. The ESP32-S3 USB interface can be used as the board power input, can flash firmware to the chip, can communicate with the chip via the USB protocol, and can also be used for JTAG debugging. |
| USB-to-UART Bridge (USB to UART bridge) | The chip is CH343P, made by WCH (QinHeng). Website: <http://www.wch-ic.com/> Driver: <http://www.wch-ic.com/products/CH343.html>? |
| RGB LED | Addressable RGB LED, driven by GPIO48. Model: WS2812. |
| PWR LED | Power indicator LED. It lights up when the board is powered and cannot be controlled by a program. |
| TX LED | LED on the ESP32-S3 serial TXD line. When serial data is transmitted, the LED flashes. If the serial function is not used, it can be used as a GPIO, GPIO43. |
| RX LED | LED on the ESP32-S3 serial RXD line. When serial data is received, the LED flashes. If the serial function is not used, it can be used as a GPIO, GPIO44. |

## Note

On boards with onboard ESP32-S3-WROOM-1 module series (using 8-line SPI flash/PSRAM), pins GPIO35, GPIO36 and GPIO37 are used for communication between the internal ESP32-S3 chip and the SPI flash/PSRAM and cannot be used externally.

## Getting Started

Before powering on, make sure the board is undamaged.

## Functional Block Diagram

The main components and connections of the YD-ESP32-S3 are shown below:

![Functional Block Diagram](https://github.com/vcc-gnd/YD-ESP32-S3/blob/main/IMG/img4.png?raw=true)

## Power Options

You can choose one of the following three power supply methods:

1. **USB-to-UART interface power** or **ESP32-S3 USB interface power** (recommended) — You may use one or both simultaneously
2. **Header power via 5V and GND**
3. **Header power via 3.3V and GND**

## Headers

The table below lists the names and functions of the headers on both sides of the board (P1 and P2). The header names are shown in the YD-ESP32-S3 front-view diagram, and the header numbering matches the board schematic (PDF).

### P1

| No. | Name | Type | Function |
| ---- | ---- | ----- | ------------------------------------------------------------ |
| 1 | 3V3 | P | 3.3 V power |
| 2 | 3V3 | P | 3.3 V power |
| 3 | RST | I | EN |
| 4 | 4 | I/O/T | RTC_GPIO4, GPIO4, TOUCH4, ADC1_CH3 |
| 5 | 5 | I/O/T | RTC_GPIO5, GPIO5, TOUCH5, ADC1_CH4 |
| 6 | 6 | I/O/T | RTC_GPIO6, GPIO6, TOUCH6, ADC1_CH5 |
| 7 | 7 | I/O/T | RTC_GPIO7, GPIO7, TOUCH7, ADC1_CH6 |
| 8 | 15 | I/O/T | RTC_GPIO15, GPIO15, U0RTS, ADC2_CH4, XTAL_32K_P |
| 9 | 16 | I/O/T | RTC_GPIO16, GPIO16, U0CTS, ADC2_CH5, XTAL_32K_N |
| 10 | 17 | I/O/T | RTC_GPIO17, GPIO17, U1TXD, ADC2_CH6 |
| 11 | 18 | I/O/T | RTC_GPIO18, GPIO18, U1RXD, ADC2_CH7, CLK_OUT3 |
| 12 | 8 | I/O/T | RTC_GPIO8, GPIO8, TOUCH8, ADC1_CH7, SUBSPICS1 |
| 13 | 3 | I/O/T | RTC_GPIO3, GPIO3, TOUCH3, ADC1_CH2 |
| 14 | 46 | I/O/T | GPIO46 |
| 15 | 9 | I/O/T | RTC_GPIO9, GPIO9, TOUCH9, ADC1_CH8, FSPIHD, SUBSPIHD |
| 16 | 10 | I/O/T | RTC_GPIO10, GPIO10, TOUCH10, ADC1_CH9, FSPICS0, FSPIIO4, SUBSPICS0 |
| 17 | 11 | I/O/T | RTC_GPIO11, GPIO11, TOUCH11, ADC2_CH0, FSPID, FSPIIO5, SUBSPID |
| 18 | 12 | I/O/T | RTC_GPIO12, GPIO12, TOUCH12, ADC2_CH1, FSPICLK, FSPIIO6, SUBSPICLK |
| 19 | 13 | I/O/T | RTC_GPIO13, GPIO13, TOUCH13, ADC2_CH2, FSPIQ, FSPIIO7, SUBSPIQ |
| 20 | 14 | I/O/T | RTC_GPIO14, GPIO14, TOUCH14, ADC2_CH3, FSPIWP, FSPIDQS, SUBSPIWP |
| 21 | 5V | P | 5 V power |
| 22 | G | G | Ground |

### P2

| No. | Name | Type | Function |
| ---- | ---- | ----- | ----------------------------------------------------- |
| 1 | G | G | Ground |
| 2 | TX | I/O/T | U0TXD, GPIO43, CLK_OUT1 |
| 3 | RX | I/O/T | U0RXD, GPIO44, CLK_OUT2 |
| 4 | 1 | I/O/T | RTC_GPIO1, GPIO1, TOUCH1, ADC1_CH0 |
| 5 | 2 | I/O/T | RTC_GPIO2, GPIO2, TOUCH2, ADC1_CH1 |
| 6 | 42 | I/O/T | MTMS, GPIO42 |
| 7 | 41 | I/O/T | MTDI, GPIO41, CLK_OUT1 |
| 8 | 40 | I/O/T | MTDO, GPIO40, CLK_OUT2 |
| 9 | 39 | I/O/T | MTCK, GPIO39, CLK_OUT3, SUBSPICS1 |
| 10 | 38 | I/O/T | GPIO38, FSPIWP, SUBSPIWP |
| 11 | 37 | I/O/T | SPIDQS, GPIO37, FSPIQ, SUBSPIQ |
| 12 | 36 | I/O/T | SPIIO7, GPIO36, FSPICLK, SUBSPICLK |
| 13 | 35 | I/O/T | SPIIO6, GPIO35, FSPID, SUBSPID |
| 14 | 0 | I/O/T | RTC_GPIO0, GPIO0 |
| 15 | 45 | I/O/T | GPIO45 |
| 16 | 48 | I/O/T | GPIO48, SPICLK_N, SUBSPICLK_N_DIFF, RGB LED |
| 17 | 47 | I/O/T | GPIO47, SPICLK_P, SUBSPICLK_P_DIFF |
| 18 | 21 | I/O/T | RTC_GPIO21, GPIO21 |
| 19 | 20 | I/O/T | RTC_GPIO20, GPIO20, U1CTS, ADC2_CH9, CLK_OUT1, USB_D+ |
| 20 | 19 | I/O/T | RTC_GPIO19, GPIO19, U1RTS, ADC2_CH8, CLK_OUT2, USB_D- |
| 21 | G | G | Ground |
| 22 | G | G | Ground |

P: Power; I: Input; O: Output; T: can be set to high impedance.

## Pinout

![Pinout](https://github.com/vcc-gnd/YD-ESP32-S3/blob/main/IMG/img11.jpg?raw=true)

## CH343 Driver and Firmware Tools

- [CH343 Driver (English)](http://www.wch-ic.com/products/CH343.html)
- [CH343 Driver (Chinese)](https://www.wch.cn/products/CH343.html?from=list)

### MicroPython Firmware Download

The ESP32-S3 download and erase tool (flash_download_tool_3.9.2_0) is the official Windows download tool. Note: no installation is required; simply unzip and use it directly. Double-click the gear icon, select ESP32-S3, then follow the on-screen instructions. Set the starting address to 0x00 and check the box in front of it. If the download fails, the USB-to-serial driver may not be installed correctly. Install the driver first, then try downloading again.

![Flash Download Tool](https://github.com/vcc-gnd/YD-ESP32-S3/blob/main/IMG/img3.png?raw=true)

Note:

You cannot use Thonny's built-in ESP32 downloader for the ESP32-S3. The built-in downloader is designed for the standard ESP32, not the ESP32-S3, and uses the incorrect starting address (0x1000 instead of 0x00). Additionally, do not use the official MicroPython SPIRAM firmware—it will not work correctly on this board.

Instead, use Espressif's official flash tool:

1. Select ESP32-S3 serial download mode (USART)
2. Connect the USB port on the board to your computer
3. Select the corresponding YD-modified firmware
4. Set the starting address to 0x00
5. Check the box before the firmware file
6. Erase the flash first, then download

The firmware is available via links starting with "1-", and the firmware download software is available via links starting with "2-". It is recommended to update the CH343 USB-to-serial hardware driver before use (see reference links starting with "0-"). In Device Manager, confirm that a COM port for the CH343 appears.

**For TASMOTA firmware**, visit the official TASMOTA site: [https://tasmota.github.io/docs/](https://tasmota.github.io/docs/)

**For custom firmware**, use Espressif's download tool: [https://www.espressif.com.cn/en/home](https://www.espressif.com.cn/en/home)

**Reference Materials** for ESP32-S3:

- CH343 hardware serial driver
- YD-modified MicroPython firmware
- Firmware download software
- MicroPython IDE
- Schematics and dimension drawings
- Available at: [http://124.222.62.86/yd-data/YD-ESP32-S3/](http://124.222.62.86/yd-data/YD-ESP32-S3/)

**Programming Documentation:**

- **ESP-IDF (C/C++)**: [https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html)
- **Arduino**: [https://docs.espressif.com/projects/arduino-esp32/en/latest/getting_started.html#about-arduino-esp32](https://docs.espressif.com/projects/arduino-esp32/en/latest/getting_started.html#about-arduino-esp32)
- **MicroPython**: [https://docs.micropython.org/en/latest/esp32/quickref.html](https://docs.micropython.org/en/latest/esp32/quickref.html)

## Counterfeit / Fake / Low-Quality Copies

There are many counterfeit, fake, and poor-quality copies of YD development boards, particularly prevalent in the Shenzhen Huaqiangbei market. Counterfeiters commonly sand off markings and photograph boards for copying, resulting in products full of hidden risks. The following details the dangers of purchasing counterfeit boards, using the YD-ESP32 (ESP32/S2/S3/C3) series as an example:

1. To maximize profit, counterfeit manufacturers often use refurbished or non-official components, arbitrarily replacing them with cheap alternatives in the same package, prioritizing cost over quality.
2. The WS2812 LED signal is not properly soldered on counterfeit boards, rendering the WS2812 unusable. This demonstrates that the counterfeiter did not properly validate the board, cutting corners to reduce costs.
3. Counterfeit boards skip factory testing (cost-cutting measure). Once off the production line, they are immediately packaged and shipped, eliminating an entire quality assurance step.
4. Because counterfeit boards are copied from photographs, many silkscreen markings are incorrect, easily misleading customers. The counterfeiter often lacks true understanding of the board.
5. Counterfeit boards are based on version 1.2 (early 2022), while genuine boards are version 1.4. Counterfeit boards cannot use newer features, and counterfeiters have not bothered to update their copies.
6. Counterfeit boards use third-party "counterfeit" modules (low cost). These modules lack proper impedance matching, leading to increased power consumption, poor signal quality, and frequent crashes when using Wi-Fi and Bluetooth.
7. Since genuine dedicated LDOs are unavailable on the market, counterfeiters choose unsuitable alternatives like the 1117 (low cost), which has excessive dropout voltage, causing crashes and poor signal quality.
8. Counterfeit boards use high-dropout diodes (>0.7 V drop, low cost), causing insufficient margin for the downstream LDO and resulting in high power consumption, crashes, and poor signal.
9. Counterfeit boards provide no technical support. Their documentation is directly copied from ours (still the early version)—counterfeiters have not bothered to find the latest materials. After payment, they simply stop responding.
10. Counterfeit boards sometimes have startup problems and boot directly into bootloader mode, rendering them unusable. Counterfeiters often don't understand this issue.
11. Because counterfeiters copy from photographs without access to schematics, users and even the counterfeiters themselves struggle to understand how to use the products.

Some counterfeiters directly print "YD-ESP32" and similar markings on counterfeit boards to deceive consumers. Worse, some even print our official website ([www.vcc-gnd.com](http://www.vcc-gnd.com)) on counterfeit products. This behavior has triggered legal action, and we will pursue it vigorously. We urge consumers not to chase a marginal saving of a few cents to a few dollars, only to bear all the risks above and waste time and effort. Support genuine YD products; when purchasing, look for the YD and VCC-GND trademarks.

## GPIO Pin Flexibility

In the documentation for various ESP32 series devices, it is stated that they support peripheral communication functions such as I2C, I2S, UART, and SPI. However, the functional pin diagrams do not indicate which pins these functions use. The answer can be found in the peripheral pin assignment documentation: peripheral interfaces such as I2C, I2S, UART, and SPI can be configured on any GPIO pin. Therefore, there is no need to label specific pins in the functional diagram—any GPIO can be assigned the pin function required by these peripheral interfaces.

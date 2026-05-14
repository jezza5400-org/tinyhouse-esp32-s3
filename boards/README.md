# Board Manifest Verification: ESP32-S3 DevKitC-1 N16R8

This folder contains a project-local board manifest used via `boards_dir = boards` in `platformio.ini`.

## Scope And Outcome

- The manifest was copied from the installed espressif32 platform board definition for `esp32-s3-devkitc1-n16r8`.
- Every key/value in this file was reviewed for semantic meaning, where PlatformIO/PIOArduino uses it, and whether it matches ESP32-S3 DevKitC-1 hardware with 16 MB flash + 8 MB PSRAM.
- One intentional local metadata edit was made: `name` changed from `...N16R8V...` to `...N16R8...`.
- The `name` field change is display metadata only and does not affect build/upload behavior.

## Why The `V` Suffix Was Removed In `name`

`V` in Espressif naming is tied to SoC/module variants such as `ESP32-S3R8V` / `ESP32-S3R16V` seen on WROOM-2 lines (1.8 V VDD_SPI domain behavior is explicitly documented by Espressif).

Evidence used:

- Espressif devkit docs: ESP32-S3-DevKitC-1 can be populated with WROOM-1, WROOM-1U, or WROOM-2 modules.
- Espressif module pages show WROOM-1 families based on `ESP32-S3`, `ESP32-S3RH2`, `ESP32-S3R8` and WROOM-2 families explicitly referencing `ESP32-S3R8V` / `ESP32-S3R16V`.
- Espressif FAQ mentions `ESP32-S3R8V` and its 1.8 V VDD_SPI implications on specific GPIO behavior.

For this board profile, the electrical memory interface settings are the authoritative behavior:

- Quad flash (`flash_mode = qio`)
- Octal PSRAM (`psram_type = opi`)
- Combined memory profile (`memory_type = qio_opi`)

Those settings map to N16R8 behavior, so the display string was normalized to `N16R8` to avoid incorrectly implying a WROOM-2 `V` variant.

## Key-By-Key Verification

### Build Section

| JSON key | Value | Meaning | Where used | Why this value is correct |
| --- | --- | --- | --- | --- |
| `build.arduino.partitions` | `default_16MB.csv` | Arduino partition CSV for this board profile | `framework-arduinoespressif32/tools/pioarduino-build.py` (`get_partition_table_csv`, `PARTITIONS_TABLE_CSV`) | Matches 16 MB flash profile and standard OTA-capable layout for this board family |
| `build.arduino.memory_type` | `qio_opi` | Combined flash/PSRAM interface profile | `platform-espressif32/builder/main.py` (`_get_board_memory_type`) | Correct for quad flash + octal PSRAM (N16R8 style hardware) |
| `build.core` | `esp32` | Arduino core directory selector | `pioarduino-build.py` (`cores/<build.core>`) | Correct for Arduino-ESP32 core |
| `build.extra_flags[0]` | `-DARDUINO_ESP32S3_DEV` | Board family compile define | Arduino build system and board-specific conditional code | Correct for ESP32-S3 devkit target |
| `build.extra_flags[1]` | `-DARDUINO_RUNNING_CORE=1` | Arduino loop/task core affinity define | Arduino runtime config/macros | Standard ESP32-S3 Arduino dual-core placement |
| `build.extra_flags[2]` | `-DARDUINO_EVENT_RUNNING_CORE=1` | Arduino event task core define | Arduino runtime config/macros | Consistent with default S3 Arduino profile |
| `build.extra_flags[3]` | `-DBOARD_HAS_PSRAM` | Enables PSRAM-aware paths in Arduino stack | Arduino PSRAM feature gates | Required for 8 MB PSRAM variant |
| `build.f_cpu` | `240000000L` | CPU clock target | Board option `board_build.f_cpu`; used by toolchain/build defines | 240 MHz is normal ESP32-S3 max operating setting |
| `build.f_flash` | `80000000L` | Flash frequency target | `main.py` (`_get_board_f_flash`) and image generation args | 80 MHz is standard high-speed flash setting for this profile |
| `build.flash_mode` | `qio` | Flash interface mode | `main.py` (`_get_board_flash_mode`) -> passed to `elf2image` | Correct for quad flash devices in this profile |
| `build.psram_type` | `opi` | PSRAM bus type | `main.py` (`_get_board_memory_type` fallback) and framework memory config | Correct for octal PSRAM |
| `build.hwids` | `[["0x303A","0x1001"]]` | USB VID/PID hints for board USB identity/autodetection | Upload autodetect flow + board metadata | Espressif VID/PID pair used by these S3 devkits |
| `build.mcu` | `esp32s3` | MCU target for toolchain, uploader, and debug config | Multiple places (`main.py`, `platform.py`) | Correct SoC family |
| `build.variant` | `esp32s3` | Arduino variant folder selection | `pioarduino-build.py` includes `variants/esp32s3` | Correct pin/variant package for S3 devkit |

### Root Metadata / Compatibility Section

| JSON key | Value | Meaning | Where used | Why this value is correct |
| --- | --- | --- | --- | --- |
| `connectivity` | `["wifi","bluetooth"]` | Capability metadata | Boards explorer / metadata consumers | ESP32-S3 supports Wi-Fi + BLE |
| `debug.default_tool` | `esp-builtin` | Preferred debug probe | `platform.py` dynamic debug tool construction | Valid for ESP32-S3 with USB JTAG capabilities |
| `debug.onboard_tools` | `["esp-builtin"]` | Marks onboard debug interfaces | `platform.py` marks tool `onboard=true` | DevKitC-1 S3 supports built-in debug path |
| `debug.openocd_target` | `esp32s3.cfg` | OpenOCD target config | `platform.py` builds OpenOCD server args (`target/<file>`) | Correct OpenOCD target script for ESP32-S3 |
| `frameworks` | `["arduino","espidf"]` | Supported frameworks | PlatformIO environment/framework validation | Correct for espressif32 platform support |
| `platforms` | `["espressif32"]` | Compatible platform IDs | Platform compatibility filtering | Correct PlatformIO platform family |
| `name` | `Espressif ESP32-S3-DevKitC-1-N16R8 (16 MB Flash Quad, 8 MB PSRAM Octal)` | Human-readable board label | Display/catalog metadata | Local normalization removes misleading `V`; no build behavior change |
| `url` | ESP-IDF DevKitC-1 user guide URL | Reference documentation link | Display metadata | Correct vendor documentation target |
| `vendor` | `Espressif` | Vendor metadata | Display/catalog metadata | Correct |

### Upload Section

| JSON key | Value | Meaning | Where used | Why this value is correct |
| --- | --- | --- | --- | --- |
| `upload.flash_size` | `16MB` | Declared flash size for image/header/upload flow | `main.py` and `pioarduino-build.py` pass `--flash-size` | Matches N16 flash hardware |
| `upload.maximum_ram_size` | `327680` | RAM budget for size reporting/checking | PlatformIO size reporting logic | Standard published RAM budget for many ESP32-S3 board defs |
| `upload.maximum_size` | `16777216` | Default maximum firmware image size bound | Board manifest size bound; may be refined by partition logic | Correct physical flash upper bound (16 MB) |
| `upload.require_upload_port` | `true` | Require resolved upload port | Upload flow guardrails | Appropriate for USB serial/JTAG upload workflows |
| `upload.speed` | `921600` | Default serial upload baud | uploader speed default (overridable via `upload_speed`) | Typical high-speed stable setting for ESP32-S3 USB upload |

## Important Behavior Note: `maximum_size` Versus OTA Partition Slot Size

`upload.maximum_size = 16777216` expresses full flash capacity metadata.

Actual application slot size can be smaller when using OTA partition tables. In this profile, `default_16MB.csv` defines OTA app partitions, and PlatformIO builder logic can reduce effective app max to the selected app partition size during build-time checks.

This is expected behavior and does not indicate a flash size mismatch.

## Source References Used For This Verification

PlatformIO docs:

- Custom board JSON structure and semantics: [PlatformIO creating board docs](https://docs.platformio.org/en/latest/platforms/creating_board.html)
- Project-local board override directory and search order: [PlatformIO boards_dir docs](https://docs.platformio.org/en/latest/projectconf/sections/platformio/options/directory/boards_dir.html)
- Espressif32 flash mode, partition table behavior, and related options: [PlatformIO espressif32 docs](https://docs.platformio.org/en/latest/platforms/espressif32.html)
- Board option override pattern (`board_{OBJECT.PATH}`): [PlatformIO platform options docs](https://docs.platformio.org/en/latest/projectconf/sections/env/options/platform/index.html)

Local installed platform/framework code reviewed:

- `~/.platformio/platforms/espressif32/builder/main.py`
- `~/.platformio/packages/framework-arduinoespressif32/tools/pioarduino-build.py`
- `~/.platformio/platforms/espressif32/platform.py`
- `~/.platformio/platforms/espressif32/boards/esp32-s3-devkitc1-n16r8.json`
- `~/.platformio/platforms/espressif32/boards/esp32-s3-devkitc-1-n32r8v.json`

Espressif docs/pages for naming and module distinctions:

- ESP32-S3-DevKitC-1 board overview (module variants on this devkit family): [Espressif devkits docs](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/index.html)
- ESP32-S3-WROOM-1 module page
- ESP32-S3-WROOM-2 module page (shows S3R8V/S3R16V family references)
- ESP-FAQ development board entry referencing S3R8V and VDD_SPI 1.8 V behavior: [Espressif FAQ](https://docs.espressif.com/projects/esp-faq/en/latest/hardware-related/development-board.html)
- Espressif part-numbering overview: [Espressif developer blog](https://developer.espressif.com/blog/2025/03/espressif-part-numbers-explained/)

## Random notes

serial only works properly when:

```text
build_flags =
 -D ARDUINO_USB_CDC_ON_BOOT=1
 -D ARDUINO_USB_MODE=1
```

Port on left:  USB/OTG:   Serial  while(!Serial) blocks
Port on right: USB->UART: Serial0 while(!Serial0) no blocking

Dump whole memory:

```bash
esptool --chip esp32s3 --port /dev/ttyACM0 --baud 115200 read-flash 0x00000 0x1000000 flash_dump.bin
```

Dump partition table:

```bash
esptool --chip esp32s3 --port /dev/ttyACM0 --baud 115200 read-flash 0x8000 0x1000 partition_table.bin
```

Dump bootloader:

```bash
esptool --chip esp32s3 --port /dev/ttyACM0 --baud 115200 read-flash 0x1000 0x7000 bootloader.bin
```

Dump application firmware (get address from partition table):

```bash
esptool --chip esp32s3 --port /dev/ttyACM0 --baud 115200 read-flash 0x10000 0x200000 app.bin
```

| Region          | Address | Size    | Description                |
|-----------------|---------|---------|----------------------------|
| Bootloader      | 0x0000  | ~0x7000 | First-stage bootloader     |
| Partition table | 0x8000  | 0x1000  | Defines all flash regions  |
| App firmware    | 0x10000 | varies  | Main program               |
| NVS             | varies  | varies  | WiFi settings, keys        |
| Filesystem      | varies  | varies  | SPIFFS / LittleFS / FAT    |
| OTA slots       | varies  | varies  | Additional firmware images |

Find locations + sizes: `esptool image-info flash_dump.bin`

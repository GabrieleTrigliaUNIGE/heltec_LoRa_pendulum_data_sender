# Heltec V3 MPU6050 LoRa Node

This project is part of the development of a sensor node based on the **Heltec WiFi LoRa 32 V3** (featuring the ESP32-S3 chip) and the **MPU6050** accelerometer. The ultimate goal of the entire system is the dynamic monitoring of a pendulum by sampling its accelerometric data across three axes (X, Y, Z), and subsequently transmitting it to a gateway via the LoRaWAN/LoRa P2P protocol. The collected data will then be made available via MQTT for real-time analysis on a subscriber node (e.g., a MacBook or a Node-RED/Grafana dashboard).

This specific section of the project implements **local sampling**, **modular hardware management**, and **visual feedback** (on the OLED display and native USB-CDC serial port) to prepare and validate the data before the upcoming RF transmission phase.

---

## Software Architecture

The code has been engineered following a highly modular approach in C++, designed to facilitate debugging and prepare for the integration of the LoRa stack and the MQTT client. 

The project uses a single entry point for configurations, delegating the implementations to separate source files (organized via tabs or a `src` folder in the Arduino IDE):

- `config.h`: The core of the definitions. It contains the macro-flags for module activation, the hardware pinout (for the Heltec V3 and custom I2C sensors), the `MPUData` data structure (which encapsulates X, Y, Z), and function prototypes.
- `display.cpp`: Implementation of the OLED display (U8g2 utilizing software I2C bit-banging to prevent bus conflicts).
- `imu.cpp`: Initialization and safe reading of the MPU6050 (resolving I2C routing issues typical of the ESP32-S3).
- The main `.ino` file: The high-level controller that orchestrates sensor readings and data output.

---

## Hardware Connections (Wiring)

The SX1262 radio module and the OLED display share the same internal power lines. To ensure stable communication with the MPU6050, the I2C bus has been mapped to safe pins via the ESP32-S3's *pin muxing*, effectively avoiding interference.

| MPU6050 Pin | Heltec V3 Pin (ESP32-S3) | Notes |
| :---: | :---: | :--- |
| **VCC** | `3.3V` | Logic power supply (If unstable, add a decoupling capacitor) |
| **GND** | `GND` | Common ground |
| **AD0** | `GND` | Common ground for the correct I2C address (0x68) |
| **SDA** | `GPIO 41` | Data Line (Reassigned via software) |
| **SCL** | `GPIO 42` | Clock Line (Reassigned via software) |

> **Attention for Heltec V3:** The OLED display and the LoRa antenna are powered through the `VEXT` pin (`GPIO 36`). It is essential to set it to a low logic state (`LOW`) within the `setup()` function to power these peripherals.
>
> ⚠️ **VERY IMPORTANT:** Never power on the board (neither via USB nor battery) without having first attached the LoRa antenna. Transmitting without a load (antenna) will irreparably damage the RF amplifier of the SX1262 module.

---

## Development Environment

The project is developed and compiled using the **Arduino IDE**. To properly compile the code for the Heltec V3 and manage the required libraries, follow the configuration steps below.

### 1. Board Manager Installation (Heltec ESP32 Series)
To add support for the Heltec V3 board in the Arduino IDE, follow the official Heltec guide:
1. Go to **File** -> **Preferences** (or **Arduino IDE** -> **Settings** on macOS).
2. In the **Additional Boards Manager URLs** field, paste the following link (if you have multiple URLs, separate them with a comma):
   `https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/1.0.0/package_heltec_esp32_index.json`
3. Click **OK**.
4. Go to **Tools** -> **Board** -> **Boards Manager...**
5. Search for `Heltec ESP32` and install the package provided by Heltec.

### 2. Library Management
The main libraries required for operation must be installed via the Arduino IDE *Library Manager* (`Sketch` -> `Include Library` -> `Manage Libraries...`):
- **`Adafruit MPU6050`** (ensure you also install all associated dependencies required by Adafruit, such as `Adafruit Unified Sensor` and `Adafruit BusIO`).
- **`U8g2`** by *oliver* (for optimized OLED display management).

### 3. Board and Serial Configuration (Tools Menu)
To correctly enable the serial console output via the ESP32-S3's native USB interface (bypassing the legacy UART), it is crucial to set the following parameters in the Arduino IDE `Tools` menu prior to compiling:

- **Board**: `Heltec WiFi LoRa 32(V3) / Wireless shell(V3) / ...` 
- **USB CDC On Boot**: `Enabled` *(Crucial for reading data on the native Serial Monitor)*
- **USB Mode**: `Hardware CDC and JTAG`

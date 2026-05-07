# FireAlarmSystem - Automatic Fire Alarm and Extinguishing System
# Smart Fire Detection and Alarm System

## 1. Project Overview
This project presents an **IoT-integrated autonomous fire suppression and monitoring system**. It utilizes a dual-microcontroller architecture to provide high-precision environmental scanning, real-time cloud telemetry via the Blynk IoT platform, and localized hydraulic suppression. By leveraging multi-sensor fusion and a master-slave communication protocol, the system minimizes false positives and ensures reliable operation in modern urban safety environments.

## 2. System Components

### 2.1 Processing & Connectivity
* **NodeMCU ESP8266 (Master):** Manages Wi-Fi connectivity, Blynk IoT cloud synchronization, and I2C bus orchestration.
* **Arduino Uno (Slave):** Handles real-time sensor data acquisition and provides PWM control for actuators.

### 2.2 Sensing Matrix
* **KY-026 Infrared Flame Sensor:** Detects IR radiation from flames ($760\text{nm}$ – $1100\text{nm}$) with an LM393 comparator.
* **MQ-2 Smoke Sensor:** Monitors combustible gas and smoke particle concentrations.
* **DHT22 (AM2302):** High-precision digital sensor for ambient temperature and relative humidity.

### 2.3 Actuators & Power
* **2-Channel Relay Module:** Provides galvanic isolation and 12V power switching for high-load actuators.
* **Water Pump & Buzzer:** Deployed for localized hydraulic suppression and high-decibel acoustic signaling.
* **Servo Motor:** Continuously sweeps the sensor array across a specified angular displacement to maximize detection coverage.

## 3. Hardware Mapping

| Component | Arduino Pin (Slave) | NodeMCU Pin (Master) | Protocol/Logic |
| :--- | :--- | :--- | :--- |
| **SDA (Data)** | **A4** | **D2 (GPIO 4)** | I2C |
| **SCL (Clock)** | **A5** | **D1 (GPIO 5)** | I2C |
| **MQ-2 Smoke** | Pin A0 | - | Analog Input ($0-1023$) |
| **KY-026 Flame 1** | Pin A1 | - | Digital Input (Active LOW) |
| **KY-026 Flame 2** | Pin A2 | - | Digital Input (Active LOW) |
| **DHT22 Temp** | Pin A3 | - | Single-bus Digital |
| **Servo Motor** | Pin 9 | - | PWM Control |
| **Relay (Pump/Buzzer)**| Pin 12, 13 | - | Digital Output (12V Control) |
| **Common GND** | **GND** | **GND** | **Required for I2C Stability** |

## 4. System Operational Procedure

The operational sequence commences with the initialization of the $I^2C$ communication bus. The Arduino Uno executes a hybrid digital-analog polling routine while the servo-actuated sweep mechanism provides spatial modulation for the detection field. 

### 4.1 Multi-Sensor Fusion
To mitigate false positives, the firmware executes a fusion algorithm requiring a minimum of two criteria to be satisfied simultaneously:
1.  **KY-026:** Digital LOW (IR radiation detected).
2.  **MQ-2:** Analog value $> 550$ (Smoke concentration threshold).
3.  **DHT22:** Temperature reading exceeds the safety setpoint.

### 4.2 Response and Telemetry
Upon validation, the Arduino triggers the 2-channel relay to activate the $12\text{V}$ pump and buzzer. Concurrently, the NodeMCU utilizes a software `Ticker` interrupt to poll the data packet and synchronize it with the Blynk IoT cloud, delivering push notifications to remote clients.

### 4.3 Confirmation Hysteresis
Once sensors return to nominal levels, the system maintains the suppression state for a **5-second temporal hysteresis window**. This ensures the thermal anomaly is fully neutralized before the system de-energizes the actuators and reverts to its baseline monitoring routine.

## 5. Installation and Setup

1.  **Firmware Upload:** Flash the Slave code to the Arduino Uno and the Master code to the NodeMCU ESP8266.
2.  **Libraries:** Ensure the `DHT`, `Wire`, `BlynkSimpleEsp8266`, and `Ticker` libraries are installed in the Arduino IDE.
3.  **Blynk Configuration:** * Create a new Template for ESP8266.
    * Configure Virtual Pins: **V1** (Alarm Status), **V2** (Smoke Gauge), **V3** (Temperature Gauge).
    * Update the `Auth Token`, `SSID`, and `Pass` in the Master code.
4.  **Wiring:** Complete wiring connections according to the provided images.

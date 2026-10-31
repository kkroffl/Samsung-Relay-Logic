Samsung Relay Logic – ESP32 IoT Project

An ESP32-based temperature and humidity monitoring system that uses a DHT11 sensor to monitor environmental conditions, automatically controls a relay based on temperature thresholds, and uploads sensor data to the ThingSpeak IoT cloud platform.

🚀 Project Overview

This project uses an ESP32 microcontroller to monitor temperature and humidity using a DHT11 sensor.

Based on the measured temperature, the system automatically switches a relay ON or OFF. Temperature and humidity readings are also uploaded to ThingSpeak for remote monitoring and data visualization.

✨ Features
Temperature Monitoring: Reads temperature using the DHT11 sensor.
Humidity Monitoring: Measures relative humidity.
Automatic Relay Control: Switches the relay based on predefined temperature thresholds.
Wi-Fi Connectivity: Connects ESP32 to a Wi-Fi network.
Cloud Integration: Uploads sensor readings to ThingSpeak.
Serial Monitoring: Displays sensor readings and relay status through the Serial Monitor.
Automatic Wi-Fi Reconnection: Attempts to reconnect if Wi-Fi connectivity is lost.
🛠️ Hardware Requirements
Component	Purpose
ESP32 Development Board	Main controller
DHT11 Sensor	Temperature and humidity measurement
Relay Module	Switching control
Jumper Wires	Circuit connections
Power Supply	Powering the system
🔌 Pin Configuration
Component	ESP32 GPIO
DHT11 Data Pin	GPIO 21
Relay Input	GPIO 19

Relay Configuration:

Relay ON: LOW
Relay OFF: HIGH
🌡️ Temperature-Based Relay Logic

The system uses two temperature thresholds to control the relay.

Temperature Condition	Relay Action
Temperature ≥ 30°C	Relay ON
Temperature ≤ 25°C	Relay OFF
Temperature between 25°C and 30°C	Maintain previous state

This threshold-based approach provides a 5°C hysteresis range to avoid frequent relay switching.

☁️ ThingSpeak Integration

The ESP32 uploads sensor readings to the ThingSpeak cloud platform.

ThingSpeak Field	Data
Field 1	Temperature (°C)
Field 2	Humidity (%)

The project uses the ThingSpeak Write API to upload sensor readings.

📚 Software Requirements
Arduino IDE
ESP32 Board Package
WiFi Library (included with ESP32 core)
ThingSpeak Library
DHT Sensor Library
⚙️ Installation and Setup
Install the Arduino IDE.
Install the ESP32 board package.
Install the required libraries using Arduino IDE Library Manager.
Connect the DHT11 sensor to GPIO 21.
Connect the relay module to GPIO 19.
Create a ThingSpeak channel with two fields:
Field 1: Temperature
Field 2: Humidity
Configure your Wi-Fi credentials and ThingSpeak API key securely.
Open KarthickRelay.ino in Arduino IDE.
Select your ESP32 board and the correct COM port.
Upload the program to the ESP32.
Open Serial Monitor at 115200 baud to monitor system operation.
📂 Repository Structure
text
Samsung-Relay-Logic/
│
├── KarthickRelay.ino
└── README.md

🔄 Working Principle
ESP32 initializes the DHT11 sensor and relay.
ESP32 connects to the configured Wi-Fi network.
DHT11 measures temperature and humidity.
ESP32 checks the temperature against the configured thresholds.
The relay switches ON or OFF according to the temperature.
Temperature and humidity values are uploaded to ThingSpeak.
The process repeats continuously.
🔮 Future Improvements
Add a real-time temperature and humidity dashboard.
Implement mobile application monitoring.
Add manual relay control through the cloud.
Introduce notifications when temperature exceeds a threshold.
Improve sensor error handling and cloud connectivity.
👨‍💻 Author

Karthick

GitHub: @kkroffl

📄 License

This project is available for educational and learning purposes. Add an appropriate open-source license if you intend to permit reuse and redistribution.

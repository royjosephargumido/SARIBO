# SARIBO
SARIBO or Systematic and Automated Regulation of Irrigation systems for
Backyard farming Operations is an internet-of-things (iot) Irrigation System designed for Backyard Farming created using Arduino Nano and ESP8266 NodeMCU 12E. SARIBO is a waray-waray (Waray) term for "to water plants".

SARIBO contains two modules:

1. SARIBO Leaf Module (SARIBO Leaf). The SARIBO Leaf Module which controls the connected sensors and is placed on the field.
2. SARIBO Root Module (SARIBO Root). The SARIBO Root Module is the main controller module which controls the watering process, activation of relays for the pump and valves, water level among others.

The following are the hardware requirements for SARIBO:

| Hardware  | Description |
| ------------- | ------------- |
| Arduino Nano ATmega328P CH340G  | The micro controller unit (MCU) responsible in the processing, management, control of the overall functionality of the system.  |
| NodeMCU V3 ESP8266 ESP-12E  | The micro controller unit (MCU) responsible in the establishment, control, management, of the Wi-Fi communication functionality of the system.  |
| 12V DC Plastic Solenoid Water Valve (Normally Closed) 1/2"| Used in opening individual Leaf Distribution Lines. |
| Plastic Water Flow Sensor 1/2" | Used to determine the water flow rate. |
| MicroSD Card Reader Module | Data logging functions. |
| 2, 4-Channel 5V Low Level Trigger 10A 250 VAC Relay with Optocoupler | Relays used in controlling the pump and other higher voltage devices. |
| DS3231 RTC Real Time Clock and EEPROM AT24C32 Module | Provides the date and time functionalities. |
| HC-SR04 Ultrasonic Ranging Sensor | Used in determining the water level in the tank/reservoir.  |
| Soil Moisture Sensor Module | Used in determining the soil moisture level. |



The following are the software requirements for SARIBO:

| Software | Specification | Description |
| ------------- | ------------- | ------------- |
| Arduino IDE | Version 1.8.10 | Serves as the Integrated Development Environment (IDE) of the Arduino wherein codes during the development are written using the software. |
| Arduino Nano ATmega328P CH340G Device Driver | | Used as the device driver of the Arduino Nano that uses the cheap CH340G IC. |
| ArduinoJSON Library for Arduino | <ArduinoJson.h> \n Version 6.14.1 by Benoit Blanchon (github.com/bblanchon/ArduinoJson) | An Arduino library used as the parser/decoder (serializer/deserializer) of data of the system that will be sent via the Wi-Fi communication of the modules of the system. |


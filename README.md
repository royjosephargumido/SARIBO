# SARIBO
  
SARIBO or Systematic and Automated Regulation of Irrigation systems for
Backyard farming Operations is an internet-of-things (IOT) Irrigation System designed for Backyard Farming created using ESP8266 NodeMCU 12E v3 (LoLin). SARIBO is a waray-waray (Waray) term for "to water" especially for plants.

SARIBO contains two modules:

1. **SARIBO Leaf Module (SARIBO Leaf).** The SARIBO Leaf Module which controls the connected sensors and is placed on the field.
2. **SARIBO Root Module (SARIBO Root).** The SARIBO Root Module is the main controller module which controls the watering process, activation of relays for the pump and valves, water level among others.


# Stable Version

See the official stable version @ **[Official Stable Release SARIBO version 1.2.7](https://github.com/rjargumido/SARIBO/releases/tag/v.1.2.7)**

# Root Diagram

![Root Fritzing Diagram](https://github.com/rjargumido/SARIBO/blob/master/Fritzing/Root%20Diagram.png)

# Leaf Diagram

![Leaf Fritzing Diagram](https://github.com/rjargumido/SARIBO/blob/master/Fritzing/Leaf%20Diagram.png)


# Hardware Requirements

The following are the **hardware requirements for SARIBO**:

| Hardware  | Description |
| ------------- | ------------- |
| NodeMCU V3 ESP8266 ESP-12E  | The micro controller unit (MCU) responsible in the establishment, control, management, of the Wi-Fi communication functionality of the system.  |
| 12V DC Plastic Solenoid Water Valve (Normally Closed) 1/2"| Used in opening individual Leaf Distribution Lines. |
| Plastic Water Flow Sensor 1/2" | Used to determine the water flow rate. |
| MicroSD Card Reader Module | Data logging functions. |
| 2, 4-Channel 5V Low Level Trigger 10A 250 VAC Relay with Optocoupler | Relays used in controlling the pump and other higher voltage devices. |
| DS3231 RTC Real Time Clock and EEPROM AT24C32 Module | Provides the date and time functionalities. |
| HC-SR04 Ultrasonic Ranging Sensor | Used in determining the water level in the tank/reservoir.  |
| Soil Moisture Sensor Module | Used in determining the soil moisture level. |


# Software Requirements

The following are the **software requirements for SARIBO**:

| Software | Specification | Description |
| ------------- | ------------- | ------------- |
| Arduino IDE | Version 1.8.10 | Serves as the Integrated Development Environment (IDE) of the Arduino wherein codes during the development are written using the software. |
| Arduino Nano ATmega328P CH340G Device Driver | | Used as the device driver of the Arduino Nano that uses the cheap CH340G IC. |
| ArduinoJSON Library for Arduino | Built using [<ArduinoJson.h> ArduinoJson Library version 6.14.1 by Benoit Blanchon](https://github.com/bblanchon/ArduinoJson). | An Arduino library used as the parser/decoder (serializer/deserializer) of data of the system that will be sent via the Wi-Fi communication of the modules of the system. |
| SD Library for Arduino | Built using [<SD.h> SD Library version 1.2.4 by Arduino.cc and SparkFun](https://www.arduino.cc/en/Reference/SD). | An Arduino library used for the data logging purposes. |
| Software Serial Library for Arduino | <SoftwareSerial.h> Included in the Arduino Core libraries using the [Software Serial Library](http://arduiniana.org/libraries/newsoftserial/) | Provides the functionality for the software serial communication between the Arduino Nano and the ESP8288 NodeMCU Wi-Fi Module. |
| Real Time Clock (RTC) Library for Arduino | [<RTClib.h> Version 1.3.3 by AdaFruit](https://github.com/adafruit/RTClib) | Provides the functionality for the setting and accessing of date and time. |
| HCSR04 Ultrasonic Ranging Sensor Library for Arduino | Built using [<HCSR04.h> version 2.0.2 by gamegine](https://github.com/gamegine/HCSR04-ultrasonic-sensor-lib). | Provides the functionality for the control and use of the HCSR04 Ultrasonic Ranging Sensor Module used in determining the water level in the tank. |
| ESP8266 Board for Arduino | Version 2.6.3 by the ESP8266 Community [ESP8266 Libraries](https://github.com/esp8266/Arduino) and [ESP8266 Board](http://arduino.esp8266.com/stable/package_esp8266com_index.json). | Provides the Wi-Fi communication and Web Server functionality. |
| ESP8266 Wi-Fi Library for ESP8266 NodeMCU | <ESP8266WiFi.h> Based on WiFi.h from Arduino WiFi shield library. Copyright (c) 2011-2014 Arduino. Modified by Ivan Grokhotkov, December 2014. Provided in the esp8266 Arduino board. | This provides the functionality in configuring the network settings such as setting the network SSID, the SSID password, ports to be used, IP address, the subnet and other communication related settings. |
| ESP8266 Web Server Library for ESP8266 NodeMCU | <ESP8266WebServer.h> Copyright (c) 2014 Ivan Grokhotkov. Provided in the esp8266 Arduino board. | This provides the network router or the web server that serves as the address or the routes wherein date could be sent or retrieved. |

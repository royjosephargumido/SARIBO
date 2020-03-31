# SARIBO
  
**SARIBO** or ***Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations** is an internet-of-things (IOT) Irrigation System designed for Backyard Farming created using ESP8266 NodeMCU 12E v3 (LoLin). SARIBO is a waray-waray (Waray) term for "to water" especially for plants.

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
| ArduinoJSON Library for Arduino | Built using [<ArduinoJson.h> ArduinoJson Library version 6.14.1 by Benoit Blanchon](https://github.com/bblanchon/ArduinoJson). | An Arduino library used as the parser/decoder (serializer/deserializer) of data of the system that will be sent via the Wi-Fi communication of the modules of the system. |
| SD Library for Arduino | Built using [<SD.h> SD Library version 1.2.4 by Arduino.cc and SparkFun](https://www.arduino.cc/en/Reference/SD). | An Arduino library used for the data logging purposes. |
| Software Serial Library for Arduino | <SoftwareSerial.h> Included in the Arduino Core libraries using the [Software Serial Library](http://arduiniana.org/libraries/newsoftserial/) | Provides the functionality for the software serial communication between the Arduino Nano and the ESP8288 NodeMCU Wi-Fi Module. |
| Real Time Clock (RTC) Library for Arduino | [<RTClib.h> Version 1.3.3 by AdaFruit](https://github.com/adafruit/RTClib) | Provides the functionality for the setting and accessing of date and time. |
| HCSR04 Ultrasonic Ranging Sensor Library for Arduino | Built using [<HCSR04.h> version 2.0.2 by gamegine](https://github.com/gamegine/HCSR04-ultrasonic-sensor-lib). | Provides the functionality for the control and use of the HCSR04 Ultrasonic Ranging Sensor Module used in determining the water level in the tank. |
| ESP8266 Board for Arduino | Version 2.6.3 by the ESP8266 Community [ESP8266 Libraries](https://github.com/esp8266/Arduino) and [ESP8266 Board](http://arduino.esp8266.com/stable/package_esp8266com_index.json). | Provides the Wi-Fi communication and Web Server functionality. |
| ESP8266 Wi-Fi Library for ESP8266 NodeMCU | <ESP8266WiFi.h> Based on WiFi.h from Arduino WiFi shield library. Copyright (c) 2011-2014 Arduino. Modified by Ivan Grokhotkov, December 2014. Provided in the esp8266 Arduino board. | This provides the functionality in configuring the network settings such as setting the network SSID, the SSID password, ports to be used, IP address, the subnet and other communication related settings. |
| ESP8266 Web Server Library for ESP8266 NodeMCU | <ESP8266WebServer.h> Copyright (c) 2014 Ivan Grokhotkov. Provided in the esp8266 Arduino board. | This provides the network router or the web server that serves as the address or the routes wherein date could be sent or retrieved. |


# Data Exchange Standard (DES)

The Data Exchange Standard (DES) is used as the core data exchange, transfer and processing rules used to ensure that the data is being processed the same way throughout the system. It is further divided into two:

1. **Data Exchange Table** Is a table that is being used to consolidate relevant real-time data to be used in the exchange between the Leaf and the Root modules or vice versa.
2. **Requests** Are 2-digit integer code used for determining what type of request is being sent or data to be send.

# Requests

The following are the request codes under Request Code Table v2.1 revision March 31, 2020:

| Request Code  | Description |
| ------------- | ------------- |
| 10 | Soil moisture reading only |
| 11 | Leaf Distribution Line, Open |
| 12 | Leaf Distribution Line, Close |
| 20 | Get Leaf Water Flow Rate Reading |
| 21 | Read current Leaf Water Flow Rate |
| 30 | Power Reading Only |
| 40 | Date and Time reading |
| 41 | Synchronize with Root Date and Time settings |
| 50 | Network Reading |
| 51 | Ping Request |
| 61 | Pull Root Settings |
| 62 | Pull Leaf Settings |

# Data Exchange Table
The following is the table structure used under Data Exchange Table v2.2 revision March 31, 2020:

| Data  | Description |
| ------------- | ------------- |
| origin | The Hardware ID of the requesting module |
| destination | The Hardware ID of the destination module of the request |
| datesent | The current date of the requesting module |
| timesent | The current time of the requesting module |
| request | The request code |
| value | The value being exchange as a validation/required data |


Note:
* Complies with the Hardware Id nomenclature Algorithm found at the Root Hardware ID Management Service (HIMS).
* The Date object is in the format: MMMM dd, YYYY e.g. April 1, 2020 without the zero padding in the day object.
* The Time object is in the format: hh:mm:ss SS e.g. 7:02:09 AM with the zero padding on both minute and second objects only.
* Request codes complies with the recent Request Code Table under the Data Exchange Standard.


Example:
| Data | Content |
| ------------- | ------------- |
| origin | HC7E9701 |
| destination | RBF0928J |
| datesent | April 1, 2020 |
| timesent | 7:02:09 AM |
| request | 11 |
| value | 892 |

The above Data Exchange Table returns a string value of:
*{"origin":"HC7E9701","destination":"RBF0928J","datesent":"April+1,+2020","timesent":7:02:09+AM","request":11,"value":892"}*

The following followwing String value complies with the ArduinoJson object [*see: <ArduinoJson.h> ArduinoJson Library version 6.14.1 by Benoit Blanchon*](https://arduinojson.org/v6/assistant/) and the HTTP URL/URI standards.


# Hardware ID Management Service (HIMS)
The Hardware ID Management Service (HIMS) is the core function that process the naming of modules *(giving of hardware Id)*, and the decoding of HIDs present in the Data Exchange Table for the processing of requests.

**HIDs are 8 pseudo-random generated alphanumeric codes used to name modules for easier network data exchange.*** HIMS serves as the central registration authority of hardware Ids within a specific SARIBO network and ensures that generated HIDs only belongs to the network, and are uniquely generated.

# The Generate Hardware ID Algorithm
The following is the algorithm used in generating HIDs:

1. To create a psuedo-random number, the randomSeed() in the setup() function is placed
2. Randomize between 0 & 1. If value is 1 store a psuedo-random uppercase alphabet character, otherwise a psuedo-random numeric character.
3. Repeat step #2 until 8 alphanumeric characters are generated.
4. Return the character array as a String

The source code for the generation of hardware id is here at [Generate Hardware Id](https://github.com/rjargumido/SARIBO/blob/master/Individual%20Systems/generateHardwareID/generateHardwareID.ino).

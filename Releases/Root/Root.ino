/*
  The S.A.R.I.B.O. Root Module - NodeMCU 12E esp8266 Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  Stable Root (NodeMCU v3 (LoLin)) Module Code
  Version 1.3.2 Revision March 28, 2020
  
  BSD 3-Clause License
  Copyright (c) 2020, Roy Joseph Argumido (rjargumido@outlook.com)
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  
  1. Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.
  
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  
  3. Neither the name of the copyright holder nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <RTClib.h>
#include <HCSR04.h>

HCSR04 waterLevelSensor(4, 15); //Trigger pin (GPIO 4/D2), Echo pin (GPIO 15/ D8)
RTC_DS3231 rtc;

//================= NETWORK PARAMETERS ===============
const char* ssid = "SARIBO Server - Argumido";
const char* password = "1234567890";
const char* host = "192.168.4.1";
String urlPath = "/requests/";

ESP8266WebServer server(80);
//====================================================

//========= LIST OF REGISTERED HARDWARE IDS ==========
const char* rootHID = "HF7890";
const char* leafHID[9] = {"FYF2U734", "3NIAKEK8", "B847KV34",
                              "7F3S82G3", "CF8MT6UJ", "C4I3XDO7",
                              "1J6P1520", "J56X01B5"};
//====================================================

//=============== ARDUINOJSON COMPONENTS =============
DeserializationError err;

const size_t capacity = JSON_OBJECT_SIZE(5) + 90; //For WiFi Communication
DynamicJsonDocument requestData(capacity);
DynamicJsonDocument responseData(capacity);

String requestPayload = "";
String responsePayload = "";
//====================================================

//=========== PIN CONFIGURATION TO NODEMCU ===========
int SER_Pin = 13;   //pin 14 on the 75HC595 & NodeMCU GPIO 13/D7 
int RCLK_Pin = 12;  //pin 12 on the 75HC595 & NodeMCU GPIO 12/D6
int SRCLK_Pin = 14; //pin 11 on the 75HC595 & NodeMCU GPIO 14/D5
//====================================================

//===== 74HC595 SHIFT OUT REGISTER CONFIGURATION =====
const int numberofRegisters = 2;
const int totalPins = numberofRegisters * 8;
boolean registers[totalPins];
//====================================================

//===== PIN CONFIG OF 74HC595 SHIFT OUT REGISTER =====
const int pumpPin = 0;  //Pump
const int msvPin = 1;   //Main Source Valve
const int tivPin = 2;   //Tank In Valve
const int tovPin = 3;   //Tank Out Valve
const int tdvPin = 4;    //Tank Drain Valve
//====================================================

//================ TANK CONFIGURATION ================
const float tankClearance = 3.0;  //Clearance between the sensor tip and the max full tank
float fullTankCap = 0;
float halfTankCap = 0;
//====================================================

//============== FUNCTION PROTOTYPING  ===============
void performRTCCheck();
void startServer();
void initRegisters();
String drain(const bool openDrain);
void initTank();
void writeRegisters();
void setPin(int index, int value);
void pump();
String leaf(const int leafNumber, const bool openLine);
void refillTank();
float checkWaterLevel();
String getDateTime(const int DT);
void handleRoot();
//====================================================

void performRTCCheck() {
  /* 
   *  The RTC Checking sub-program.
   *  
   *  This checks if the RTC module is connected to the Arduino and continues to loop
   *  until the RTC module is connected to the NodeMCU.
   *  
   *  Algorithm:
   *  
   *  [1] Starts the rtc module, if does not responds, loop until the rtc module is connected to NodeMCU
   *      @ if (!rtc.begin())
   *  [2] Checks if the rtc module's power is being cut or drained, then it syncs its time to Root
   *      @ if (rtc.lostPower())
   */
  if (!rtc.begin()) {
    Serial.println("Couldn't find the RTC module! Please make sure the RTC module is connected to the main board.");
    while (1);
  }

  /* 
   *  Checks if the RTC module  lost its power then synchronizes 
   *  the date and time from any connected Leaf modules
   */
  if (rtc.lostPower()) {
    Serial.println("RTC module lost power! Synchronizing time with server.");
    /*
     * The code for the date and time sncyhronization here
     * following line sets the RTC to the date & time this sketch was compiled
     */
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    /* 
     *  This line sets the RTC with an explicit date & time,
     *  for example to set March 9, 2020 7:58:32 AM you would call:
     *  
     *  rtc.adjust(DateTime(year, month, day, hour, minute, second));
     *  
     *  example:
     *  rtc.adjust(DateTime(2020, 3, 8, 3, 0, 0));
     */
  }
}

void startServer() {
  Serial.println();

  Serial.println("Starting Root Module Server...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();

  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(myIP);

  server.on(urlPath, handleRoot);
  server.begin();
  Serial.print("SARIBO Root Server @ ");
  Serial.print(myIP);
  Serial.println(urlPath);
  Serial.println("SARIBO Root Server running.");
}

void initRegisters() {
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);
  
  //This clears the registers
  for(int i = totalPins - 1; i >=  0; i--){
     registers[i] = HIGH;
  }
  writeRegisters();
}

String drain(const bool openDrain) {
  String message = "";
  
  switch(openDrain) {
    case true: //OPEN DRAIN VALVE
      setPin(tdvPin, LOW);
      message = "Drain valve open.";
      break;

    case false: //CLOSE DRAIN VALVE
      setPin(tdvPin, HIGH); //Closes the Drain Valve
      message = "Drain valve closed.";
      break;

    default:
      message = "Unable to perform open/close drain valve.Invalid open drain valve command.";
      break;
  }
  return message;
}

void initTank() {
  double currTCap = 0.0;
  double totalTReads = 0.0;
  int i;

  drain(true);  //Opens the drain valve to ensure tank is empty during initialization
  
  for(i = 1; i < 11; i++) {
    currTCap = waterLevelSensor.dist();
    totalTReads += currTCap;
    Serial.print("Reading #");
    Serial.print(i);
    Serial.print(": ");
    Serial.print("Current: ");
    Serial.println(currTCap);
    delay(1000);
  }

  if(i == 11) {
    fullTankCap = (totalTReads / 10) - tankClearance;
    halfTankCap = fullTankCap / 2;
    Serial.print("Full Tank: ");
    Serial.println(fullTankCap);
  }
  Serial.println("Tank refill on process.");
}

void writeRegisters() {
  digitalWrite(RCLK_Pin, LOW);

  for(int i = totalPins - 1; i >=  0; i--){
    digitalWrite(SRCLK_Pin, LOW);

    int val = registers[i];

    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);
  }
  digitalWrite(RCLK_Pin, HIGH);
}

void setPin(int index, int value) {
  /*
   * Set an individual pin HIGH or LOW
   */
  registers[index] = value;
  writeRegisters();
}

void pump() {
  int countOpenLeafLines = 0;
  for(int i = 8; i < totalPins; i++){
     if(registers[i] == 0)  // Leaf Distribution Line is OPEN
      countOpenLeafLines++;
  }
  
  if(countOpenLeafLines == 0) //There is no more open distribution lines
    setPin(pumpPin, HIGH);  //PUMP OFF
  else
    setPin(pumpPin, LOW);  //PUMP ON
}

String leaf(const String hID, const bool openLine) {
  String message = "";
  int leafPin = 0;

  for(int i = 0; i < 8; i++) {
    if((String)leafHID[i] == hID) {
      leafPin = i + 8;
    }
  }
  /*
   * Checks if the leaf to be open has a registered pin number in the system
   * based on the total number of pins available.
   */
  if(leafPin < totalPins) {
    switch(openLine) {
      case true:  //opens the specific leaf distribution line
        setPin(leafPin, LOW);
        message =  "Leaf " + (String)(leafPin-7) + " distribution line opened.";
        break;

      case false:  //opens the specific leaf distribution line
        setPin(leafPin, HIGH);
        message = "Leaf " + (String)(leafPin-7) + " distribution line closed.";
        break;
    }
  } else {
    if(openLine == true)
      message = "Unable to open Leaf " + (String)(leafPin-7) + " distribution line.";
    else
      message = "Unable to close Leaf " + (String)(leafPin-7) + " distribution line.";
  }
  pump();
  return message;
}

void refillTank() {
  const int waterLevel = checkWaterLevel();
  
  Serial.print("Current Tank Capacity: ");
  Serial.print(waterLevel);
  Serial.print("%");
  Serial.print("\tFull Capacity: ");
  Serial.println(fullTankCap);
  
  if(waterLevel > 101) {
    /*
     * AUTOMATIC WATER TANK SPILL TRIGGER
     * 
     * The current tank water level exceeds the maximum tank capacity set by the system,
     * this will drain the tank for 5 seconds then closes after the time. This is to avoid
     * water to distroy the ultrasonic ranging sensor.
     */
    Serial.println("Current water level exceed maximum tank capacity! Spilling excess water.");
    setPin(pumpPin, HIGH);  //Pump Off
    setPin(msvPin, HIGH);  //Main Source Valve Off
    setPin(tivPin, HIGH);  //Tank In Valve Off
    setPin(tovPin, HIGH);  //Tank Out Valve Off
    drain(true);
    delay(5000);
    drain(false);
  } else if(waterLevel < 50) {
    /*
     * WATER TANK REFILL PROCESS
     * 
     * The current water level in the tank is lesser than the half capacity of the tank.
     * This will pauses all watering process to prioritizes the refill of the tank
     * to ensure that the tank has a sufficient amount of stored water for the system and
     * resumes all watering process when the tank is full.
     */
    
    Serial.println("Water tank refill on process.");
    drain(false); //Closes the drain valve
    setPin(pumpPin, LOW);  //Pump On
    setPin(msvPin, LOW);  //Main Source Valve On
    setPin(tivPin, LOW);  //Tank In Valve On
    setPin(tovPin, HIGH);  //Tank Out Valve Off
    
    while(checkWaterLevel() < 101) {
      Serial.println("Refilling...");
      delay(1000);
    }
    
    setPin(pumpPin, HIGH);  //Pump Off
    setPin(msvPin, HIGH);  //Main Source Valve Off
    setPin(tivPin, HIGH);  //Tank In Valve Off
    
  } else if(waterLevel > 50 && waterLevel < 101){
    /* 
     *  TANK IN SUFFICIENT WATER LEVEL
     */
    Serial.println("Sufficient tank water level.");
    drain(false); //Closes the drain valve
    setPin(msvPin, HIGH);  //Main Source Valve Off
    setPin(tivPin, HIGH);  //Tank In Valve Off
  }
}

float checkWaterLevel() {
  float currTankCap = ((ceil(fullTankCap - (waterLevelSensor.dist() - tankClearance))) / fullTankCap) * 100;
  return currTankCap;
}

String getDateTime(const int DT) {
  /* 
   *  The get Date and Time sub-program.
   *  
   *  This returns the current date and time set in the RTC module.
   *  This requires a constant integer parameter to return the
   *  neccessary DateTime string based on the asked value, based
   *  on the following parameter values:
   *  
   *  0 = returns the current date as a String
   *  1 = returns the current time as a String
   *  2 = return the current date and time with a newline delimiter as a String
   *  
   *  Algorithm:
   *  
   *  [1] Converts default the 24-hour format to 12-hour format with AM/PM designation
   *      @ if(now.hour() > 12) 13:24:35 -> 1:24:35 PM
   *  [2] Adds the '0' padding to the minute and second if the value is less than 10
   *      e.g. 8:7:9 -> 8:07:09
   *      @ if(now.minute() < 10) & if(now.second() < 10)
   *  [3] Creates the String Date and Time.
   *      @ String Date = ...; and String Time = ...;
   *  [4] Returns the Date or Time request or the error message as String
   */
  String HF = ""; // AM/PM time notation
  int h = 0;  //holder for the current hour value
  String m = "";
  String s = "";
  const char monthNames[12][12] = {"January", "February", "March",
                                   "April", "May", "June", "July",
                                   "August", "September", "October",
                                   "November", "December"};

  DateTime now = rtc.now();

  // Converting 24H to 12H with AM/PM designation
  if(now.hour() > 12) {
    h = now.hour() % 12;
    HF = " PM";
  }else
  {
    h = now.hour();
    HF = " AM";
  }

  // Adding the '0' Padding to minute if minute is lesser than 10
  if(now.minute() < 10) { m = "0" + (String)now.minute(); }
  else { m = (String)now.minute(); }

  // Adding the '0' Padding to second if second is lesser than 10
  if(now.second() < 10) { s = "0" + (String)now.second(); }
  else { s = (String)now.second(); }

  String Date =  (String)monthNames[now.month()] + ' ' + (String)now.day() + ", " + (String)now.year();
  String Time = (String)h + ':' + (String)m + ':' + (String)s + (String)HF;

  switch(DT)
  {
    case 0:
      return Date;
      break;

    case 1:
      return Time;
      break;

    case 2:
      return (String)Date + '\n' + (String)Time;
      break;

    default:
      return "Invalid getDateTime return code! \n0 = returns the current date\n1 = returns the current time\n2 = returns the current date and time.";
      break;
  }
}

void handleRoot() {
  if (server.hasArg("data")) {
    requestPayload = server.arg(0);
    Serial.println("Leaf request data recieved.");
  }

  err = deserializeJson(requestData, requestPayload);
  if (err) {
      Serial.print(F("Unable to decode message! Error: "));
      Serial.println(err.c_str());
  }

  const char* origin = requestData["origin"];
  const char* datesent = requestData["datesent"];
  const char* timesent = requestData["timesent"];
  int request = requestData["request"];
  int value = requestData["value"];

  String resp = "";
  switch(request) {
    case 11:  //open distribution line request
      resp = leaf(origin, true);
      break;

    case 12:  //open distribution line request
      resp = leaf(origin, false);
      break;
  }

  Serial.println("============ REQUEST INFORMATION ============");
  Serial.print("Origin: ");
  Serial.println(origin);

  Serial.print("Date Sent: ");
  Serial.println(datesent);

  Serial.print("Time Sent: ");
  Serial.println(timesent);

  Serial.print("Request: ");
  Serial.println(request);

  Serial.print("Value: ");
  Serial.println(value);
  Serial.println("=============================================");

  responseData["origin"] = "HF7890";
  responseData["datesent"] = getDateTime(0);
  responseData["timesent"] = getDateTime(1);
  responseData["request"] = 10;
  responseData["value"] = resp;

  responsePayload = "";
  serializeJson(responseData, responsePayload);

  server.send(200, "text/plain", responsePayload);
}

void setup() {
  Serial.begin(115200);
  initRegisters();
  //initTank();
  //performRTCCheck();
  //startServer();
}

void loop() {
	//server.handleClient();
	Serial.println(leaf("FYF2U734", true));
	delay(5000); 

	Serial.println();
	delay(5000); 
}

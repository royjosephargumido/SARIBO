/*
  The S.A.R.I.B.O. Prototype Functions
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SARIBO Leaf Settings Prototype Function
  Version 1.1 Revision April 4, 2020
  
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
 
#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <RTClib.h>

//================ FUNCTION PROTOTYPING ==============
void initRTC();
void initSD();
void decodeJsonData(const DeserializationError error);
void writeDefaults();
void loadSettings();
void dispData();
//====================================================

//================= PIN CONFIGURATION ================
const int sd_cs = 15;;  //SD CS Pin to NodeMCU D8/GPIO 15
//====================================================

//================== DEVICE OBJECTS =================
File file;
DateTime now;
RTC_DS3231 rtc;
//====================================================

//=================== REQUEST CODES ==================
const int rDGen = 10;
const int rOpen = 11;
const int rClose = 12;
const int rPower = 30;
const int rDTRead = 40;
const int rDTSync = 41;
const int rPing = 51;
const int rRoot = 61;
const int rSoil = 71;
const int rHID = 72;
//====================================================

//============== DEFAULT CONFIGURATIONS ==============
const char* __ssid = "SARIBO-SERVER";
const char* __key = "1234567890";
const char* __hostIP = "192.168.4.0";
const char* __reqPath = "/requests/?data=";
const int __port = 8080;
const char* __wut = "6:00:00";
const int __maxDryness = 1001;
const int __minDryness = 600;
const int __idealMoist = 450;
const int __clearance = 3;
//====================================================

//============= FILE MANAGEMENT SERVICE ==============
const int maxBuffer = 1000;
const char* __dirCore = "System";
const char* __fpSettings = "System/SysConfig.txt";

String globalHID = "";
String localHID = "";
String ssid = "";
String key = "";
String host = "";
String urlpath = "";
int port;
String wut = "";
int maxdryness;
int mindryness;
int idealmoist;
//====================================================

//============== ARDUINOJSON COMPONENTS ==============
const size_t __coreCap = JSON_OBJECT_SIZE(12) + maxBuffer;
DynamicJsonDocument settingsCORE(__coreCap);
//====================================================

void initRTC() {
  while(!rtc.begin()) {
    Serial.println("Couldn't find the RTC module!");
    delay(1000);
  }
  
  if(rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  now = rtc.now();
  randomSeed(now.unixtime());

  Serial.println("RTC module running.");
}

void initSD() {
  Serial.println("Initializing SD card...");
  pinMode(sd_cs, OUTPUT);

  while(!SD.begin(sd_cs)) {
    Serial.println("Unable to detect the SD Card module!");
    delay(1000);
  }
  
  Serial.println("SD Card ready.");
}

void decodeJsonData(const DeserializationError error) {
  if(error) {
    Serial.print(F("Unable to decode message! Error: "));
    Serial.println(error.c_str());
  }
}

void writeDefaults() {
  String rawData = "";

  SD.mkdir(__dirCore);

  
  do {
    file = SD.open(__fpSettings, FILE_WRITE);
    if(file) {
      Serial.println("Unable to create the settings file!");
      delay(500);
    }
  }while(!file);

  //settingsCORE["localHID"] = sendrequest(rHID);
  settingsCORE["localHID"] = "2J41F7FQ";
  settingsCORE["globalHID"] = globalHID;
  settingsCORE["ssid"] = __ssid;
  settingsCORE["key"] = __key;
  settingsCORE["host"] = __hostIP;
  settingsCORE["urlpath"] = __reqPath;
  settingsCORE["port"] = __port;
  settingsCORE["wakeuptime"] = __wut;
  settingsCORE["maxdryness"] = __maxDryness;
  settingsCORE["mindryness"] = __minDryness;
  settingsCORE["idealmoist"] = __idealMoist;

  serializeJson(settingsCORE, rawData);

  file.print(rawData);
  file.close();
  
  Serial.println("Done writing default settings.");
}

void loadSettings() {
  char data[maxBuffer];
  int i = 0;

  do {
    file = SD.open(__fpSettings);
    if(!file) {
      Serial.println("Unable to load settings!");
      delay(500);
    }
  }while(!file);

  while(file.available()) {
    data[i] = file.read();
    i++;
  }
  data[i] = '\0';

  decodeJsonData(deserializeJson(settingsCORE, data));

  globalHID = (const char*)settingsCORE["globalHID"];
  localHID = (const char*)settingsCORE["localHID"];
  ssid = (const char*)settingsCORE["ssid"];
  key = (const char*)settingsCORE["key"];
  host = (const char*)settingsCORE["host"];
  urlpath = (const char*)settingsCORE["urlpath"];
  port = settingsCORE["port"];
  wut = (const char*)settingsCORE["wakeuptime"];
  maxdryness = settingsCORE["maxdryness"];
  mindryness = settingsCORE["mindryness"];
  idealmoist = settingsCORE["idealmoist"];

  file.close();

  Serial.println("Settings loaded.");
}

void dispData() {
  Serial.print("Local HID: ");
  Serial.println(localHID);
  Serial.print("Root HID: ");
  Serial.println(globalHID);
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(key);
  Serial.print("Host: ");
  Serial.println(host);
  Serial.print("URL Path: ");
  Serial.println(urlpath);
  Serial.print("Port: ");
  Serial.println(port);
  Serial.print("Wake up Time: ");
  Serial.println(wut);
  Serial.print("Maximum Soil Dryness: ");
  Serial.println(maxdryness);
  Serial.print("Minimum Soil Dryness: ");
  Serial.println(mindryness);
  Serial.print("Ideal Soil Moisture: ");
  Serial.println(idealmoist);
}

void setup() {
  delay(3000);
  Serial.begin(115200);

  initRTC();
  initSD();
  
  if(SD.exists(__fpSettings)) {
    loadSettings();
    dispData();
  }else {
    writeDefaults();
  }
}

void loop() {
}

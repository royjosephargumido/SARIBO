/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU 12E esp8266 Module Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  OFFICIAL STABLE RELEASE - Leaf Module, Arduino Nano Code
  Version 1.2.8 Revision April 4, 2020
  
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
#include <ESP8266WiFi.h>

//================ FUNCTION PROTOTYPING ==============
void connectToServer();
void sendrequest(const int rCode, const int value);
void initRTC();
void initSD();
void decodeJsonData(const DeserializationError error);
void writeDefaults();
void loadSettings();
void dispData();
void perfSMRead();
String getDateTime(const int DTReq);
//====================================================

//================= PIN CONFIGURATION ================
const int sd_cs = 15;;  //SD CS Pin to NodeMCU D8/GPIO 15
const int sm = 0;  //Analog 0 Pin of NodeMCU
//====================================================

//================== DEVICE OBJECTS =================
File file;
DateTime now;
RTC_DS3231 rtc;
WiFiClient client;
DeserializationError err;
//====================================================

//================== GLOBAL VARIABLES ================
const int count = 10;
const int maxBuffer = 1000;
bool stopSMRead = false;
long int smReadings = 0;
int times = 1;
bool rsOpen = false;
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
const char* __hostIP = "192.168.4.1";
const char* __reqPath = "/requests/?data=";
const int __port = 80;
const char* __wut = "6:00:00";
const int __maxDryness = 1001;
const int __minDryness = 600;
const int __idealMoist = 450;
//====================================================

//============= FILE MANAGEMENT SERVICE ==============
const char* __dirCore = "System";
const char* __fpSettings = "System/SysConfig.txt";

String globalHID = "RTX72890";
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
const size_t __rrCap = JSON_OBJECT_SIZE(8) + maxBuffer;
DynamicJsonDocument settingsCORE(__coreCap);
DynamicJsonDocument requestData(__rrCap);
DynamicJsonDocument responseData(__rrCap);
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
  //rtc.adjust(DateTime(__DATE__, __TIME__));
  //DateTime t = DateTime(rtc.now().unixtime()+30);
  //rtc.adjust(t);
  
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

  //settingsCORE["localHID"] = sendrequest(rHID, 0);
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
  Serial.println("============== LEAF INFORMATION ==============");
  Serial.print("Date: ");
  Serial.println(getDateTime(0));
  Serial.print("Time: ");
  Serial.println(getDateTime(1));
  
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
  Serial.println("==============================================");
}

void perfSMRead() {
  if(stopSMRead == false) {
    if(times <= count) {
      int sm = analogRead(sm);

      Serial.print("Reading #");
      Serial.print(times);
      Serial.print(": ");
      Serial.println(sm);

      smReadings += sm;
      times++;
      delay(1000);
    }

    if(times == count + 1) {
      stopSMRead = true;
      int finalSMV = smReadings / count;

      Serial.println("=======================================");
      Serial.print("Final Soil Moisture Reading: ");
      Serial.println(finalSMV);

      if(finalSMV >= maxdryness) {
        Serial.println("Soil moisture misreading!");
        sendrequest(rSoil, finalSMV);
        stopSMRead = false;
        times = 1;
        perfSMRead();        
      }else if(finalSMV < maxdryness && finalSMV > mindryness) {
        Serial.println("Soil is dry! Open distribution line request sent.");
        sendrequest(rOpen, finalSMV);
        rsOpen = true;
      }else if(finalSMV <= mindryness && finalSMV > idealmoist) {
        Serial.println("Soil is in ideal soil moisture value.");
        if(rsOpen == true) {
          Serial.println("Close distribution line request sent.");
          sendrequest(rClose, finalSMV);
        }else {
          Serial.println("Soil moisture reading sent.");
          sendrequest(rDGen, finalSMV);
        }
      }else if(finalSMV < idealmoist) {
        Serial.println("Soil is wet!");
        if(rsOpen == true) {
          Serial.println("Close distribution line request sent.");
          sendrequest(rClose, finalSMV);
        }else {
          Serial.println("Soil moisture reading sent.");
          sendrequest(rDGen, finalSMV);
        }
      }
      stopSMRead = true;
    }
  }
}

void connectToServer() {
  int retries = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, key);

  Serial.println();
  
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    retries++;
    delay(1000);
  }
  
  Serial.println();
  Serial.print("Connected to Server after ");
  Serial.print(retries);
  Serial.println(" retries.");

  Serial.print("Leaf IP Address: ");
  Serial.println(WiFi.localIP());
}

String getDateTime(const int DTReq) {
  now = rtc.now();
  const char monthNames[12][12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
  String HF = "";
  int h = 0;
  String M = "";
  String d = "";
  String m = "";
  String s = "";

  // COnverting 24H to 12H with AM/PM designation
  if(now.hour() > 12) {
    h = now.hour() % 12;
    HF = "+PM";
  }else
  {
    h = now.hour();
    HF = "+AM";
  }

  // Adding the '0' Padding to minute if minute is lesser than 10
  if(now.minute() < 10) { m = "0" + (String)now.minute(); }
  else { m = (String)now.minute(); }

  // Adding the '0' Padding to second if second is lesser than 10
  if(now.second() < 10) { s = "0" + (String)now.second(); }
  else { s = (String)now.second(); }

  // Adding the '0' Padding to month if month is lesser than 10
  if(now.month() < 10) { M = "0" + (String)now.month(); }
  else { M = (String)now.month(); }

  // Adding the '0' Padding to month if month is lesser than 10
  if(now.day() < 10) { d = "0" + (String)now.day(); }
  else { d = (String)now.day(); }
  
  String Date =  (String)monthNames[now.month() - 1] + '+' + (String)now.day() + ",+" + (String)now.year();
  String Time = (String)h + ':' + (String)m + ':' + (String)s + (String)HF;
  String TId = localHID + now.year() + M + d + now.hour() + m + s;
  
  switch(DTReq)
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

    case 3:
      return TId;
      break;

    default:
      return "Invalid getDateTime return code! \n0 = returns the current date\n1 = returns the current time\n2 = returns the current date and time.";
      break;
  }
}

void sendrequest(const int rCode, const int value) {
  String payload = "";
  
  while(!client.connect(host, port)) {
    Serial.print("Unable to reach host(");
    Serial.print(host);
    Serial.print(", at port ");
    Serial.print(port);
    Serial.println("). Reconnecting...");
    delay(1000);
  }

  const String transid = getDateTime(3);
  const String dsent = getDateTime(0);
  const String tsent = getDateTime(1);

  requestData["id"] = transid;
  requestData["origin"] = localHID;
  requestData["destination"] = globalHID;
  requestData["datesent"] = dsent;
  requestData["timesent"] = tsent;
  requestData["request"] = rCode;
  requestData["value"] = value;
  
  serializeJson(requestData, payload);
  client.print(String("GET ") + urlpath + payload + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  
  unsigned long timeout = millis();
  while(client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("Server unreachable. Reconnecting...");
      sendrequest(rCode, value);
      //client.stop();
      return;
    }
  }

  String responsePayload = "";
  while(client.available()) {
    responsePayload = client.readStringUntil('\r');
  }

  decodeJsonData(deserializeJson(responseData, responsePayload));
  
  const char* origin = responseData["origin"];
  const char* datesent = responseData["datesent"];
  const char* timesent = responseData["timesent"];
  int request = responseData["request"];
  const char* _value = responseData["value"];

  Serial.println("============ RESPONSE INFORMATION ============");
  Serial.print("Origin: ");
  Serial.println(origin);
  
  Serial.print("Date Sent: ");
  Serial.println(datesent);
  
  Serial.print("Time Sent: ");
  Serial.println(timesent);
  
  Serial.print("Request: ");
  Serial.println(request);
  
  Serial.print("Value: ");
  Serial.println(_value);
  Serial.println("=============================================");

  Serial.println("Hibernation started.");
  client.stop();
  ESP.deepSleep(10e6);
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

  connectToServer();

  Serial.println("======== SOIL MOISTURE READING ========");
  for(int i = 1; i <= count; i++)
    perfSMRead();
}

void loop() {
}

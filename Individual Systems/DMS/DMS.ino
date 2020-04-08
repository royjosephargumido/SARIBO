/*
  The S.A.R.I.B.O. Prototype Functions
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SARIBO Data Management Service Function
  Version 1.2.3 Revision April 78 2020
  
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

//================= PIN CONFIGURATION ================
const int sd_cs = 15;;  //SD CS Pin to NodeMCU D8/GPIO 15
//====================================================

//================== DEVICE OBJECTS =================
File file;
//====================================================

//================== GLOBAL VARIABLES ================
const int maxBuffer = 2000;
//====================================================

//============== DEFAULT CONFIGURATIONS ==============
const char* _DMSHEADER = "DMS-LEAF-SETTINGS";
const char* _DMSVER = "1.2.3";
const char* _FDCREATED = "April 7, 2020";
const char* _FTCREATED = "16:05:32";
const char* _FOWNER = "ADMIN04072020160532";
const char* _FDREV = "April 7, 2020";
const char* _FTREV = "16:05:32";
const char* _FMUID = "ADMIN04072020162012";

const char* _netSSID = "SARIBO-SERVER";
const char* _netKEY = "1234567890";
const char* _netHOSTIP = "192.168.4.1";
const char* _netURLPATH = "/requests/?data=";
const int _netPORT = 8080;

const char* _GLOBAL = "HF7890QN";
const char* _LOCAL = "8A5011X7";

const char* _dirLog = "Logs";
const char* _dirCore = "System";
const char* _fpDocu = "Documentation/SARIBO_Manual_v3.pdf";
const char* _fpSettings = "System/SysConfig.txt";

const int _rDGen = 10;
const int _rOpen = 11;
const int _rClose = 12;
const int _rPower = 20;
const int _rDTRead = 30;
const int _rDTSync = 31;
const int _rSoil = 41;

const char* _WUT = "6:00:00";
const int _MAX = 1001;
const int _MIN = 600;
const int _IDEAL = 450;
//====================================================

struct newCache {
  String ssid;
  String key;
  String ip;
  String path;
  int port;

  String global;
  String local;
  String coreDir;
  String fpConfig;

  int rDGen;
  int rOpen;
  int rClose;
  int rDTRead;
  int rDTSync;
  int rSoil;
  
  String wut;
  int maxdryness;
  int mindryness;
  int idealmoisture;
}cache;

//============== ARDUINOJSON COMPONENTS ==============
const size_t capacity = JSON_OBJECT_SIZE(32) + maxBuffer;
DynamicJsonDocument settingsCORE(capacity);
//====================================================

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

  SD.mkdir(_dirCore);

  do {
    file = SD.open(_fpSettings, FILE_WRITE);
    if(file) {
      Serial.println("Unable to create the settings file!");
      delay(500);
    }
  }while(!file);

  settingsCORE["header"] = _DMSHEADER;
  settingsCORE["version"] = _LDMSVER;
  settingsCORE["dcreated"] = _FDCREATED;
  settingsCORE["tcreated"] = _FTCREATED;
  settingsCORE["createdby"] = _FOWNER;
  settingsCORE["dmodified"] = _FDREV;
  settingsCORE["tmodified"] = _FTREV;
  settingsCORE["modifiedby"] = _FMUID;
  settingsCORE["ssid"] = _netSSID;
  settingsCORE["key"] = _netKEY;
  settingsCORE["ip"] = _netHOSTIP;
  settingsCORE["urlpath"] = _netURLPATH;
  settingsCORE["port"] = _netPORT;
  settingsCORE["global"] = _GLOBAL;
  settingsCORE["local"] = _LOCAL;
  settingsCORE["dlog"] = _dirLog;
  settingsCORE["dcore"] = _dirCore;
  settingsCORE["ddocu"] = _fpDocu;
  settingsCORE["fpdocu"] = _fpSettings;
  settingsCORE["fpconfig"] = _fpSettings;
  settingsCORE["rdgen"] = _rDGen;
  settingsCORE["rdopen"] = _rOpen;
  settingsCORE["rclose"] = _rClose;
  settingsCORE["rpower"] = _rPower;
  settingsCORE["rdtread"] = _rDTRead;
  settingsCORE["rdtsync"] = _rDTSync;
  settingsCORE["rsoil"] = _rSoil;
  settingsCORE["wut"] = _WUT;
  settingsCORE["max"] = _MAX;
  settingsCORE["min"] = _MIN;
  settingsCORE["ideal"] = _IDEAL;

  serializeJson(settingsCORE, rawData);

  file.print(rawData);
  file.close();
  
  Serial.println("Done writing default settings.");
}

void loadSettings() {
  char data[maxBuffer];
  int i = 0;

  do {
    file = SD.open(_fpSettings);
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

  cache.ssid = settingsCORE["ssid"].as<String>();
  cache.key = settingsCORE["key"].as<String>();
  cache.ip = settingsCORE["ip"].as<String>();
  cache.path = settingsCORE["urlpath"].as<String>();
  cache.port = settingsCORE["port"].as<int>();
  cache.global = settingsCORE["global"].as<String>();
  cache.local = settingsCORE["local"].as<String>();
  cache.coreDir = settingsCORE["dcore"].as<String>();
  cache.fpConfig = settingsCORE["fpconfig"].as<String>();
  cache.rDGen = settingsCORE["rdgen"].as<int>();
  cache.rOpen = settingsCORE["ropen"].as<int>();
  cache.rClose = settingsCORE["rclose"].as<int>();
  cache.rDTRead = settingsCORE["rdtread"].as<int>();
  cache.rDTSync = settingsCORE["rdtsync"].as<int>();
  cache.rSoil = settingsCORE["rsoil"].as<int>();
  cache.wut = settingsCORE["wut"].as<String>();
  cache.maxdryness = settingsCORE["max"].as<int>();
  cache.mindryness = settingsCORE["min"].as<int>();
  cache.idealmoisture = settingsCORE["ideal"].as<int>();

  Serial.print("SSID: ");
  Serial.println(cache.ssid);

  Serial.print("Key: ");
  Serial.println(cache.key);

  Serial.print("IP: ");
  Serial.println(cache.ip);

  Serial.print("URL Path: ");
  Serial.println(cache.path);

  Serial.print("Port: ");
  Serial.println(cache.port);

  Serial.print("Root HID: ");
  Serial.println(cache.global);

  Serial.print("Local HID: ");
  Serial.println(cache.local);

  Serial.print("Wake up time: ");
  Serial.println(cache.wut);

  Serial.print("Max Soil Dryness: ");
  Serial.println(cache.maxdryness);

  Serial.print("Min Soil Dryness: ");
  Serial.println(cache.mindryness);

  Serial.print("Ideal Soil Moisture: ");
  Serial.println(cache.idealmoisture);

  file.close();
  Serial.println("Settings loaded.");
}

void setup() {
  delay(3000);
  Serial.begin(115200);

  initSD();
  writeDefaults();
  loadSettings();
}

void loop() {
}

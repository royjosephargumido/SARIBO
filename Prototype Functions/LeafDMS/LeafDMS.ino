/*
  The S.A.R.I.B.O. Prototype Functions
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SARIBO Data Management Service - Leaf Module
  Version 1.2.4 Revision April 13, 2020
  
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

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <ArduinoJson.h>

const int sdCS = 15;
const int maxBuffer = 2000;

const size_t fileBuffer = JSON_OBJECT_SIZE(30) + maxBuffer;
DynamicJsonDocument settingsData(fileBuffer);

/*
 * Meta-Data: 10
 * DMS-FSS:   4
 * Request:   4
 * HID:       2
 * Network:   5
 * Natives:   5
 * ---------------
 * Total:    30 Definitions
 */

// ================= DMS-META-DATA =================
const char* _device = "LEAF";
const char* _header = "LEAF-SETTINGS";
const char* _dmsversion = "1.2.4";
const char* _dcreated = "April 1, 2020";
const char* _tcreated = "16:05:32";
const char* _owner = "ADMIN04072020160532";
const char* _dmodified = "April 1, 2020";
const char* _tmodified = "16:22:47";
const char* _modifiedby = "ADMIN04072020162012 ";
const char* _umode = "SD-SIGNED";
// =================================================

// ==================== DMS-FSS ====================
const char* _repdir = "Replication";
const char* _coredir = "System";
const char* _logsdir = "Logs";
const char* _corefn = "CoreConfig.txt";
// =================================================

// ==================== REQUEST ====================
const int _rSoil = 10;
const int _rGen = 20;
const int _rOpen = 21;
const int _rClose = 22;
// =================================================

// ====================== HID ======================
const char* _local = "2J41F7FQ";
const char* _root = "23JB74K5";
// =================================================

// ==================== NETWORK ====================
const char* _ssid = "SARIBO-CHARLIE";
const char* _key = "1234567890";
const char* _host = "192.168.4.1";
const char* _reqpath = "/requests/?data=";
const int _port = 80;
// =================================================

// ==================== NATIVES ====================
const char* _wut = "6:00:00";
const int _smrp = 10;
const int _maxd = 1001;
const int _mind = 600;
const int _ideal = 450;
// =================================================


//======================= CACHE ======================
File file;

String device;
String dmsheader;
String dmsversion;
String dcreated;
String tcreated;
String owner;
String dmodified;
String tmodified;
String modifiedby;
String umode;

String repDir;
String coreDir;
String logsDir;
String coreFN;

String root;
String local;

String ssid;
String key;
String host;
String reqpath;
int port;

int rSoil;
int rGen;
int rOpen;
int rClose;

String wut;
int smrp;
int maxd;
int mind;
int ideal;
//====================================================

int sdfail;

void initSD() {
  pinMode(sdCS, OUTPUT);

  while(!SD.begin(sdCS)) {
    Serial.println("Unable to detect the SD Card module!");
    sdfail++;
    delay(1000);
  }
}

String decodeJsonData(const DeserializationError error) {
  if(error)
    return "Error " + (String)error.c_str();
  else
    return "No decoding error.";
}

void writeLeafSettings() {
  String rawData;

  SD.mkdir(_coredir);

  do {
    file = SD.open((String)_coredir + (String)"/" + (String)_corefn, FILE_WRITE);
    if(file) {
      Serial.println("Unable to create the settings file!");
      delay(1000);
    }
  }while(!file);

  settingsData["device"] = _device;
  settingsData["header"] = _header;
  settingsData["dmsver"] = _dmsversion;
  settingsData["dcreated"] = _dcreated;
  settingsData["tcreated"] = _tcreated;
  settingsData["owner"] = _owner;
  settingsData["dmodified"] = _dmodified;
  settingsData["tmodified"] = _tmodified;
  settingsData["modifiedby"] = _modifiedby;
  settingsData["umode"] = _umode;

  settingsData["repdir"] = _repdir;
  settingsData["coredir"] = _coredir;
  settingsData["logsdir"] = _logsdir;
  settingsData["corefn"] = _corefn;

  settingsData["rsoil"] = _rSoil;
  settingsData["rgen"] = _rGen;
  settingsData["ropen"] = _rOpen;
  settingsData["rclose"] = _rClose;
  
  settingsData["root"] = _root;
  settingsData["local"] = _local;
  
  settingsData["ssid"] = _ssid;
  settingsData["key"] = _key;
  settingsData["host"] = _host;
  settingsData["reqpath"] = _reqpath;
  settingsData["port"] = _port;
  
  settingsData["wut"] = _wut;
  settingsData["smrp"] = _smrp;
  settingsData["maxd"] = _maxd;
  settingsData["mind"] = _mind;
  settingsData["ideal"] = _ideal;

  serializeJson(settingsData, rawData);

  file.print(rawData);
  file.close();
  
  Serial.println("Done writing default settings.");
}

void loadLeafSettings() {
  char data[maxBuffer];
  int i = 0;

  do {
    file = SD.open((String)_coredir + (String)"/" +  (String)_corefn);
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

  decodeJsonData(deserializeJson(settingsData, data));
    
  device = settingsData["device"].as<String>();
  dmsheader = settingsData["header"].as<String>();
  dmsversion = settingsData["dmsver"].as<String>();
  dcreated = settingsData["dcreated"].as<String>();
  tcreated = settingsData["tcreated"].as<String>();
  owner = settingsData["owner"].as<String>();
  dmodified = settingsData["dmodified"].as<String>();
  tmodified = settingsData["tmodified"].as<String>();
  modifiedby = settingsData["modifiedby"].as<String>();
  umode = settingsData["umode"].as<String>();
  
  repDir = settingsData["repdir"].as<String>();
  coreDir = settingsData["coredir"].as<String>();
  logsDir = settingsData["logsdir"].as<String>();
  coreFN = settingsData["corefn"].as<String>();

  rSoil = settingsData["rsoil"].as<int>();
  rGen = settingsData["rgen"].as<int>();
  rOpen = settingsData["ropen"].as<int>();
  rClose = settingsData["rclose"].as<int>();

  root = settingsData["root"].as<String>();
  local = settingsData["local"].as<String>();
  
  ssid = settingsData["ssid"].as<String>();
  key = settingsData["key"].as<String>();
  host = settingsData["host"].as<String>();
  reqpath = settingsData["reqpath"].as<String>();
  port = settingsData["port"].as<int>();
  
  wut = settingsData["wut"].as<String>();
  smrp = settingsData["smrp"].as<int>();
  maxd = settingsData["maxd"].as<int>();
  mind = settingsData["mind"].as<int>();
  ideal = settingsData["ideal"].as<int>();

  Serial.println("================= META-DATA =================");
  Serial.print("SARIBO Module:\t\t");
  Serial.println(device);
  Serial.print("Content type:\t\t");
  Serial.println(dmsheader);
  Serial.print("Version:\t\t\t");
  Serial.println(dmsversion);
  Serial.print("Date created:\t\t");
  Serial.println(dcreated);
  Serial.print("Time created:\t\t");
  Serial.println(tcreated);
  Serial.print("File owner:\t\t\t");
  Serial.println(owner);
  Serial.print("Date last modified:\t");
  Serial.println(dmodified);
  Serial.print("Time last modified:\t");
  Serial.println(tmodified);
  Serial.print("Last modified by:\t\t");
  Serial.println(modifiedby);
  Serial.print("Updated via:\t\t");
  Serial.println(umode);
  Serial.println("=============================================");
  
  Serial.println();

  Serial.println("========= FILE STRUCTURING STANDARD =========");
  Serial.print("Replication Directory:\t");
  Serial.println(repDir);
  Serial.print("System Directory:\t\t");
  Serial.println(coreDir);
  Serial.print("Logs Directory:\t\t");
  Serial.println(logsDir);
  Serial.print("Settings Filename:\t");
  Serial.println(coreFN);
  Serial.println("=============================================");
  
  Serial.println();

  Serial.println("================ HARDWARE ID ================");
  Serial.print("Root HID:\t\t\t");
  Serial.println(root);
  Serial.print("Local HID:\t\t\t");
  Serial.println(local);
  Serial.println("=============================================");

  Serial.println();

  Serial.println("================== NETWORK ==================");
  Serial.print("SSID:\t\t\t\t");
  Serial.println(ssid);
  Serial.print("Password:\t\t\t");
  Serial.println(key);
  Serial.print("Host IP:\t\t\t");
  Serial.println(host);
  Serial.print("Requests Path:\t\t");
  Serial.println(reqpath);
  Serial.print("Port:\t\t\t\t");
  Serial.println(port);
  Serial.println("=============================================");

  Serial.println();

  Serial.println("=============== REQUEST CODE ================");
  Serial.print("Soil Reading:\t\t");
  Serial.println(rSoil);
  Serial.print("General Reading:\t\t");
  Serial.println(rGen);
  Serial.print("Open Line:\t\t\t");
  Serial.println(rOpen);
  Serial.print("Close Line:\t\t\t");
  Serial.println(rClose);
  Serial.println("=============================================");

  Serial.println();

  Serial.println("================== NATIVES ==================");
  Serial.print("Wake-up Time:\t\t");
  Serial.println(wut);
  Serial.print("SM reading precision:\t");
  Serial.println(smrp);
  Serial.print("Maximum Soil Dryness:\t");
  Serial.println(maxd);
  Serial.print("Minimum Soil Dryness:\t");
  Serial.println(mind);
  Serial.print("Ideal Soil Moisture:\t");
  Serial.println(ideal);
  Serial.println("=============================================");

  file.close();
}

void setup() {
  Serial.begin(115200);
  delay(3000);

  initSD();
  writeLeafSettings();
  loadLeafSettings();
}

void loop() {
}

/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU-12E v3 esp8266 Module Code
  Version 1.3.0 Revision April 14, 2020
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
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
#include <ESP8266WiFi.h>

//================= PIN CONFIGURATION ================
const int sdCS = 15;
const int smVCC = 5;
const int smAP = 0;
//====================================================

//======================= CACHE ======================
File file;
IPAddress myIP;
int sdfail = 0;
int serverfail = 0;
int hostfail = 0;
int sconnfail = 0;
bool startWater = false;

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
String logsDir;

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

//============== ARDUINOJSON COMPONENTS ==============
const int maxBuffer = 2000;

const size_t desBuffer = JSON_OBJECT_SIZE(4) + maxBuffer;
const size_t fileBuffer = JSON_OBJECT_SIZE(28) + maxBuffer;
DynamicJsonDocument settingsData(fileBuffer);
DynamicJsonDocument exchangeData(desBuffer);
//====================================================

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

void loadLeafSettings() {
  char data[maxBuffer];
  int i = 0;

  do {
    file = SD.open("System/CoreConfig.txt");
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
  logsDir = settingsData["logsdir"].as<String>();

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

  Serial.println("Settings successfully loaded into the cache.");
  file.close();
}

void connectToServer() {  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, key);

  Serial.println();
  
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    serverfail++;
    delay(1000);
  }
  
  Serial.println();
  Serial.print("Connected to Server after ");
  Serial.print(serverfail);
  Serial.println(" seconds.");

  myIP = WiFi.localIP();
  Serial.print("Leaf IP Address: ");
  Serial.println(myIP);
}

void sendrequest(const int rCode, const int value) {
  WiFiClient client;
  
  while(!client.connect(host, port)) {
    Serial.print("Unable to reach host(");
    Serial.print(host);
    Serial.print(":");
    Serial.print(port);
    Serial.println("). Reconnecting...");
    hostfail++;
    delay(1000);
  }

  Serial.print("Connected to host(");
  Serial.print(host);
  Serial.print(":");
  Serial.print(port);
  Serial.print(") after ");
  Serial.print(hostfail);
  Serial.println(" seconds.");

  exchangeData["id"] = "**sample_id**";
  exchangeData["origin"] = local;
  exchangeData["request"] = rCode;
  exchangeData["value"] = value;

  Serial.println();
  Serial.println("****************************************");
  Serial.println("Request sent!");

  String payload = "";
  serializeJson(exchangeData, payload);
  client.print(String("GET ") + reqpath + payload + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  Serial.println("Waiting for server response...");
  Serial.println("****************************************");
  
  unsigned long timeout = millis();
  while(client.available() == 0) {
    if(millis() - timeout > 5000) {
      Serial.println("Server unreachable. Reconnecting...");
      sconnfail++;
      sendrequest(rCode, value);
      return;
    }
  }

  Serial.print("Connected to server after ");
  Serial.print(sconnfail);
  Serial.println(" seconds.");

  String responsePayload = "";
  while(client.available()) {
    responsePayload = client.readStringUntil('\r');
  }

  decodeJsonData(deserializeJson(exchangeData, responsePayload));

  String id = exchangeData["id"].as<String>();
  const char* origin = exchangeData["origin"];
  int request = exchangeData["request"];
  const char* _value = exchangeData["value"];

  Serial.println();
  
  Serial.println("=============== REQUEST ===============");
  Serial.print("ID: ");
  Serial.println("**sample_id**");
  Serial.print("Origin: ");
  Serial.println(local);
  Serial.print("Request: ");
  Serial.println(rCode);
  Serial.print("Value: ");
  Serial.println(value);
  Serial.println("======================================");

  Serial.println("============== RESPONSE ==============");
  Serial.print("ID: ");
  Serial.println(id);
  Serial.print("Origin: ");
  Serial.println(origin);
  Serial.print("Request: ");
  Serial.println(request);
  Serial.print("Value: ");
  Serial.println(_value);
  Serial.println("======================================");
  
  client.stop();
  ESP.deepSleep(10e6);
}

void perfSMRead() {
  int count = 0;
  long int readings = 0;

  digitalWrite(smVCC, HIGH);
  delay(3000);

  while(count < smrp) {
    int smv = analogRead(smAP);
    readings += smv;
    count++;
    delay(1000);

    Serial.print("Reading #");
    Serial.print(count);
    Serial.print(": ");
    Serial.println(smv);
  }
  
  const int finalSMV = readings / count;

  Serial.println();
  Serial.print("Final Soil Moisture value: ");
  Serial.println(finalSMV);

  if(finalSMV >= maxd) {
    Serial.println("Irregular reading! Repeating soil moisture reading.");
    sendrequest(rSoil, finalSMV);
    perfSMRead();
  }
  else if (finalSMV < maxd && finalSMV > mind)  {
    Serial.println("Soil is dry! Starting watering process.");
    sendrequest(rOpen, finalSMV);
    startWater = true;
  }
  else if (finalSMV <= mind && finalSMV > ideal) {
    Serial.println("Soil is in ideal soil moisture value.");    
    if(startWater == true) {  //Watering Process Started
      Serial.println("Ending watering process.");
      sendrequest(rClose, finalSMV);
    }
    else {
      Serial.println("Logging soil moisture value.");
      sendrequest(rGen, finalSMV);
    }
  }
  else {
    Serial.println("Soil is wet!");
    sendrequest(rGen, finalSMV);
  }
  Serial.println("***************************************");
}

void setup() {
  Serial.begin(115200);
  delay(3000);

  Serial.println();

  pinMode(smVCC, OUTPUT);
  digitalWrite(smVCC, LOW);

  initSD();
  loadLeafSettings();
  connectToServer();
  
  Serial.println();
  
  perfSMRead();
}

void loop() {
}

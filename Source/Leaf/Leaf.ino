/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU-12E v3 esp8266 Module Code
  Version 1.3.1 Revision April 16, 2020
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
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

//================= PIN CONFIGURATION ================
const int sdCS = 15;
const int smVCC = 5;
const int smAP = 0;
//====================================================

//======================= CACHE ======================
const int _sleep = 5;
const int maxBuffer = 2000;
const long int sleepTime = 1000000 * _sleep;
const char* coreFN = "System/CoreConfig.txt";

File file;
IPAddress myIP;
int ngid = 0;
int sdfail = 0;
int hostfail = 0;
int loadfail = 0;
int sconnfail = 0;
int serverfail = 0;
int irrReadfail = 1;
bool startWater = false;
int reads = 0;
unsigned long bootInit;

String repDir;
String logsDir;
String root;
String local;
String ssid;
String key;
String host;
String reqpath;
int port;
int rFail = 1;
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
const size_t desBuffer = JSON_OBJECT_SIZE(4) + maxBuffer;
const size_t fileBuffer = JSON_OBJECT_SIZE(28) + maxBuffer;
DynamicJsonDocument settingsData(fileBuffer);
DynamicJsonDocument exchangeData(desBuffer);
//====================================================

void initSD() {
  pinMode(sdCS, OUTPUT);
  while(!SD.begin(sdCS)) {
    if(sdfail == 60) {
      ESP.restart();
    } else {
      Serial.println("Unable to detect the SD Card module!");
      sdfail++;
      delay(1000);
    }
  }
}

String generateTID() {
  return local + '-' + String(++ngid);
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
  
  file = SD.open(coreFN);
  while(!file) {
    loadfail++;
    Serial.println("Unable to load settings!");
    delay(1000);
    file = SD.open(coreFN);
  }

  while(file.available()) {
    data[i] = file.read();
    i++;
  }
  data[i] = '\0';

  String decodeSF = decodeJsonData(deserializeJson(settingsData, data));
  
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
  
  Serial.println("Connecting to " + ssid + "...");
  while(WiFi.status() != WL_CONNECTED) {
    
    serverfail++;
    delay(1000);
  }

  myIP = WiFi.localIP();
  Serial.println("Connected to Server after " + (String)serverfail + " seconds.");
  Serial.println("Leaf IP Address: " + myIP.toString());
}

void sendrequest(const int rCode, const int value) {
  WiFiClient client;
  
  while(!client.connect(host, port)) {
    //Serial.println("Unable to reach host(" + host + ":" + String(port) + "). Reconnecting...");
    hostfail++;
    delay(1000);
  }

  String reqID = generateTID();
  exchangeData["id"] = reqID;
  exchangeData["origin"] = local;
  exchangeData["request"] = rCode;
  exchangeData["value"] = value;
  
  String payload = "";
  serializeJson(exchangeData, payload);
  client.print("GET " + reqpath + payload + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while(client.available() == 0) {
    if(millis() - timeout > 60000) {
      Serial.println("Server unreachable. Reconnecting...");
      sconnfail++;
      sendrequest(rCode, value);
      return;
    }
  }

  Serial.println("\n****************************************");
  Serial.println("Connected to host(" + host + ":" + port + ") after " + String(hostfail) + " seconds.");
  Serial.println("Connected to server after " + String(sconnfail) + " seconds.");
  Serial.println("Request sent!");
  Serial.println("****************************************");

  String responsePayload = "";
  while(client.available()) {
    responsePayload = client.readStringUntil('\r');
  }
  Serial.println();
  Serial.println(decodeJsonData(deserializeJson(exchangeData, responsePayload)));

  String resID = exchangeData["id"].as<String>();
  const char* origin = exchangeData["origin"];
  int request = exchangeData["request"];
  const char* _value = exchangeData["value"];

  if(!(reqID == resID)) {
    Serial.println("Empty response! Resending request.");
    sendrequest(rCode, value);
  } else {
    Serial.println("\n=============== REQUEST ===============");
    Serial.println("ID: " + String(reqID));
    Serial.println("Origin: " + local);
    Serial.println("Request: " + String(rCode));
    Serial.println("Value: " + String(value));
    Serial.println("======================================");
  
    Serial.println("============== RESPONSE ==============");
    Serial.println("ID: " + String(resID));
    Serial.println("Origin: " + String(origin));
    Serial.println("Request: " + String(request));
    Serial.println("Value: " + String(_value));
    Serial.println("======================================");
    
    client.stop();
  }
}

void perfSMRead() {
  int count = 0;
  long int readings = 0;

  digitalWrite(smVCC, HIGH);
  delay(3000);

  reads++;
  Serial.println("\nSoil moisture reading #" + String(reads));
  Serial.println("***************************************");
  while(count < smrp) {
    int smv = analogRead(smAP);
    readings += smv;
    count++;
    Serial.println("Reading #" + String(count) + ": " + String(smv));
    delay(1000);
  }
  
  const int finalSMV = readings / count;
  Serial.println("***************************************");
  Serial.println("Final soil moisture value: " + String(finalSMV));

  if(finalSMV >= maxd) {
    Serial.println("Irregular reading! Fail #" + String(irrReadfail));
    if((irrReadfail % 5) == 0) {
      Serial.println("Soil moisture reading sent to server.");
      Serial.println("***************************************");
      sendrequest(rSoil, finalSMV);
    }
    
    Serial.println("\n***************************************");
    Serial.println("Repeating soil moisture reading.");
    Serial.println("***************************************");
    irrReadfail++;
    perfSMRead();
  }
  else if (finalSMV < maxd && finalSMV > mind)  {
    irrReadfail = 0;
    Serial.println("Soil is dry!\nStarting watering process.");
    Serial.println("***************************************");
    sendrequest(rOpen, finalSMV);
    startWater = true;
    Serial.println("\nWaiting for the soil absorption interval (5 minutes)...");
    delay(5000);
    sendrequest(rClose, finalSMV);
    perfSMRead();
  }
  else if (finalSMV <= mind && finalSMV > ideal) {
    irrReadfail = 0;
    Serial.println("Soil is in ideal soil moisture value.");
    if(startWater == true) {  //Watering Process Started
      Serial.println("Ending watering process.");
      Serial.println("***************************************");
      sendrequest(rClose, finalSMV);
    }
    else {
      Serial.println("Logging soil moisture value.");
      Serial.println("***************************************");
      sendrequest(rGen, finalSMV);
    }
  }
  else {
    irrReadfail = 0;
    Serial.println("Soil is wet!");
    Serial.println("***************************************");
    sendrequest(rGen, finalSMV);
  }
  digitalWrite(smVCC, LOW);
}

void setup() {
  bootInit = millis();
  pinMode(smVCC, OUTPUT);
  digitalWrite(smVCC, LOW);
  
  Serial.begin(115200);
  delay(3000);

  Serial.println("\n");
  initSD();
  loadLeafSettings();
  connectToServer();
  perfSMRead();

  int runSecs = (millis() - bootInit) / 1000;
  int runMins = runSecs / 60;
  Serial.println("\nRuntime: " + String(runMins) + " minutes and " + String(runSecs % 60) + " seconds");
  
  Serial.flush();
  ESP.deepSleep(sleepTime);
}

void loop() {
}

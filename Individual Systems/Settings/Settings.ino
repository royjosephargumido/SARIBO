/*
  The S.A.R.I.B.O. Prototype Functions
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SARIBO Settings Prototype Function
  Version 1.2 Revision April 3, 2020
  
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

File myFile;
const int sd_cs = 15;

//================= DMS COMPONENTS ===============
const int maxBuffer = 1000;
const char* settingsFile = "System/SysConfig.txt";
const char* hidFile = "System/HID.txt";

String content = "";
String rootHID = "";
String localhid = "";
String ssid = "";
String key = "";
String host = "";
String urlpath = "";
int port;
String wakeuptime = "";
int maxdryness;
int mindryness;
int idealmoist;
int tankclearance;
int fulltank;

String leaf01hid = "";
String leaf02hid = "";
String leaf03hid = "";
String leaf04hid = "";
//====================================================

//=============== ARDUINOJSON COMPONENTS =============
const size_t coreCap = JSON_OBJECT_SIZE(17) + maxBuffer;
const size_t hidCap = JSON_OBJECT_SIZE(5);
DynamicJsonDocument settingsCORE(coreCap);
DynamicJsonDocument settingsHID(hidCap);
//====================================================

void initSD() {
  Serial.println("Initializing SD card...");
  pinMode(sd_cs, OUTPUT);

  while(!SD.begin(sd_cs)) {
    Serial.println("Unable to detect the SD Card module!");
    delay(500);
  }
  Serial.println("SD Card detected.");
}

String generateHID() {
  char HID[8]; //Container for the generated character
  for(int i = 0; i < 8; i++){
    if(random(2) == 1)  //Randomizes between 0 & 1
      HID[i] = random(65, 90);  //1 means alphabet character
    else  //0 means a numeric character
      HID[i] = random(48, 57);
  }
  //Adds the terminating character in the last position of the char array
  HID[8] = '\0';
  
  return HID;
}

void dispJSONError(const DeserializationError error) {
  if(error) {
    Serial.print(F("Unable to decode message! Error: "));
    Serial.println(error.c_str());
  }
}

void loadSFile(const char* file) {
  char data[maxBuffer];
  int i = 0;
  
  myFile = SD.open(file);
  
  if(myFile) {
    while (myFile.available()) {
      data[i] = myFile.read();
      i++;
    }
    data[i] = '\0';

    if(!strcmp(file, settingsFile)) {
      /*
       * CORE SETTINGS
       */
      dispJSONError(deserializeJson(settingsCORE, data));
      
      content = (const char*)settingsCORE["content"];
      rootHID = (const char*)settingsCORE["roothid"];
      localhid = (const char*)settingsCORE["localhid"];
      ssid = (const char*)settingsCORE["ssid"];
      key = (const char*)settingsCORE["key"];
      host = (const char*)settingsCORE["host"];
      urlpath = (const char*)settingsCORE["urlpath"];
      port = settingsCORE["port"];
      wakeuptime = (const char*)settingsCORE["wakeuptime"];
      maxdryness = settingsCORE["maxdryness"];
      mindryness = settingsCORE["mindryness"];
      idealmoist = settingsCORE["idealmoist"];
      tankclearance = settingsCORE["clearance"];
      fulltank = settingsCORE["fulltank"];
    }else if(!strcmp(file, hidFile)) {
      /*
       * HARDWARE ID SETTINGS
       */
      dispJSONError(deserializeJson(settingsHID, data));

      content = (const char*)settingsHID["content"];
      leaf01hid = (const char*)settingsHID["leaf01hid"];
      leaf02hid = (const char*)settingsHID["leaf02hid"];
      leaf03hid = (const char*)settingsHID["leaf03hid"];
      leaf04hid = (const char*)settingsHID["leaf04hid"];
    }
    myFile.close();
  }else {
    Serial.println("Error loading settings!");
  }
}

void writeSFile(const char* file) {
  String rawData = "";
  SD.mkdir("System");

  if (SD.exists(file)){
    loadSFile(settingsFile);
    SD.remove(file);
  }

  myFile = SD.open(file, FILE_WRITE);

  if(myFile) {
    if(!strcmp(file, settingsFile)) {
      settingsCORE["content"] = "System-Configuration";
      settingsCORE["localhid"] = "2J41F7FQ";
      
      if(rootHID == "") {
        settingsCORE["roothid"] = generateHID();
      }else{
        settingsCORE["roothid"] = rootHID;
      }
      
      settingsCORE["ssid"] = "SARIBO - Argumido";
      settingsCORE["key"] = "123456789";
      settingsCORE["host"] = "192.168.4.1";
      settingsCORE["urlpath"] = "/requests/?data=";
      settingsCORE["port"] = 80;
      settingsCORE["wakeuptime"] = "6:00:00";
      settingsCORE["maxdryness"] = 1001;
      settingsCORE["mindryness"] = 600;
      settingsCORE["idealmoist"] = 450;
      settingsCORE["clearance"] = 3;
      settingsCORE["fulltank"] = 450;

      serializeJson(settingsCORE, rawData);
    }else if(!strcmp(file, hidFile)) {
      settingsHID["content"] = "System-HID";
      settingsHID["leaf01hid"] = "2J41F7FQ";
      settingsHID["leaf02hid"] = "TE90JXXP";
      settingsHID["leaf03hid"] = "29A8L56Z";
      settingsHID["leaf04hid"] = "2K35HJL4";

      serializeJson(settingsHID, rawData);
    }
    myFile.print(rawData);
    myFile.close();
  }
}

void dispCSData() {
  Serial.print("Local HID: ");
  Serial.println(localhid);
  Serial.print("Root HID: ");
  Serial.println(rootHID);
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
  Serial.println(wakeuptime);
  Serial.print("Maximum Soil Dryness: ");
  Serial.println(maxdryness);
  Serial.print("Minimum Soil Dryness: ");
  Serial.println(mindryness);
  Serial.print("Ideal Soil Moisture: ");
  Serial.println(idealmoist);
  Serial.print("Tank Clearance (in cm): ");
  Serial.println(tankclearance);
  Serial.print("Full tank capacity: ");
  Serial.println(fulltank);

  Serial.print("Leaf 01 HID: ");
  Serial.println(leaf01hid);
  Serial.print("Leaf 02 HID: ");
  Serial.println(leaf02hid);
  Serial.print("Leaf 03 HID: ");
  Serial.println(leaf03hid);
  Serial.print("Leaf 04 HID: ");
  Serial.println(leaf04hid);
}

void setup() {
  Serial.begin(9600);
  /*
  * If the analog input pin 0 is unconnected, random analog
  * noise will cause the call to randomSeed() to generate
  * different seed numbers each time the sketch runs.
  * randomSeed() will then shuffle the random function.
  */
  randomSeed(analogRead(0));

  initSD();

  writeSFile(settingsFile);
  loadSFile(settingsFile);

  writeSFile(hidFile);
  loadSFile(hidFile);
  
  dispCSData();
}

void loop() {
}

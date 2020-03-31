/*
  The S.A.R.I.B.O. Prototype Functions
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SARIBO Settings Prototype Function
  Version 1.1 Revision April 1, 2020
  
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

/*
 * CS -> D8 of NodeMCU v3
 * SCK -> D5 of NodeMCU v3
 * MOSI -> D7 of NodeMCU v3
 * MISO -> D6 of NodeMCU v3
 */

File myFile;
const int sdcs = 15;
const char* settingsFile = "System/SysConfig.txt";

DeserializationError err;
const size_t capacity = JSON_OBJECT_SIZE(8);
DynamicJsonDocument settings(capacity);

void loadSettings() {
  char data[200];
  int i = 0;
  
  myFile = SD.open(settingsFile);
  
  if (myFile) {
    while (myFile.available()) {
      data[i] = myFile.read();
      i++;
    }
    data[i] = '\0';

    err = deserializeJson(settings, data);
    if(err) {
      Serial.print(F("Unable to decode message! Error: "));
      Serial.println(err.c_str());
    }
    
    const char* localhid = settings["localhid"];
    const char* roothid = settings["roothid"];
    const char* ssid = settings["ssid"];
    const char* key = settings["key"];
    const char* host = settings["host"];
    const char* urlpath = settings["urlpath"];
    const int port = settings["port"];
    const char* wakeuptime = settings["wakeuptime"];
  
    Serial.print("Local HID: ");
    Serial.println(localhid);
    Serial.print("Root HID: ");
    Serial.println(roothid);
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

    myFile.close();
  }else {
    Serial.println("Error loading settings!");
  }
}

void writeSettings() {
  SD.mkdir("System");

  if (SD.exists(settingsFile)){
    SD.remove(settingsFile);
  }

  myFile = SD.open(settingsFile, FILE_WRITE);

  if (myFile) {
    settings["localhid"] = "2J41F7FQ";
    settings["roothid"] = "HSDOSSUR";
    settings["ssid"] = "SARIBO - Argumido";
    settings["key"] = "123456789";
    settings["host"] = "192.168.4.1";
    settings["urlpath"] = "/requests/?data=";
    settings["port"] = 80;
    settings["wakeuptime"] = "6:00:00";

    String settingsData = "";
    serializeJson(settings, settingsData);
    
    myFile.print(settingsData);
    myFile.close();
  }
}

void setup() {
  Serial.begin(9600);

  Serial.print("Initializing SD card...");
  pinMode(sdcs, OUTPUT);

  while(!SD.begin(15)) {
    Serial.println("Unable to detect the SD Card module!");
    return;
  }
  Serial.println("SD Card detected.");

  writeSettings();
  loadSettings();
}

void loop() {
}

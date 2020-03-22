/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU 12E esp8266 Module Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  OFFICIAL STABLE RELEASE - Root Module, Arduino Nano Code
  Version 1.2.7 Revision March 22, 2020
  
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

#include <ArduinoJson.h>

const int baudRate = 9600;
//================= PIN CONFIGURATION ===============
/* 
 *  NOTE:
 *  When revising any part hereof, please revise the physical
 *  pin configuration. It is highly advised that the pump
 *  should not be placed in a Pulse-Width Modulation (PWM) Pin.
 */
const int pump = 2;
const int MSValve = 3;
const int TIValve = 4;
const int TOValve = 5;
const int TDValve = 13;

const int leaf1 = 6;
const int leaf2 = 7;
const int leaf3 = 8;
const int leaf4 = 9;
//===================================================

//============== WATER PROCESS COUNTERS =============
bool refillOn = false;
bool dischargeOn = false;
//===================================================

//============== ARDUINO JSON COMPONENTS ============
const size_t capacity = JSON_OBJECT_SIZE(2) + 50;
DynamicJsonDocument recvData(capacity);
//===================================================

void setupRelays() {
  const int pins[] = {pump, MSValve, TIValve, TOValve, TDValve, leaf1, leaf2, leaf3, leaf4};
  
  for(int i = 0; i < (sizeof(pins) / sizeof(int)); i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], HIGH);
  }
}

void relaySwitch(String hID, const int request) {
  char* leafHID[] = { "92EC9416", // Leaf1 HID
                      "3B414B13", // Leaf2 HID
                      "B1185347", // Leaf3 HID
                      "AC08E3C3", // Leaf4 HID
                      "28F052C1", // Leaf5 HID
                      "859C5259", // Leaf6 HID
                      "5135D578", // Leaf7 HID
                      "120503C9" }; // Leaf8 HID

  int activateLeaf = 0;
  for(int i = 0; i < 8; i++) {
    if((String)leafHID[i] == hID) {
      activateLeaf = i + 1;
    }
  }

  switch(activateLeaf) {
    case 1:
      if(request == 11){  // OPEN
        dischargeOn = true;
        digitalWrite(leaf1, LOW);
      }else { // CLOSE
        dischargeOn = false;
        digitalWrite(leaf1, HIGH);
      }
      break;

    case 2:
      if(request == 11){  // OPEN
        dischargeOn = true;
        digitalWrite(leaf2, LOW);
      }else { // CLOSE
        dischargeOn = false;
        digitalWrite(leaf2, HIGH);
      }
      break;

    case 3:
      if(request == 11){  // OPEN
        dischargeOn = true;
        digitalWrite(leaf3, LOW);
      }else { // CLOSE
        dischargeOn = false;
        digitalWrite(leaf3, HIGH);
      }
      break;

    case 4:
      if(request == 11){  // OPEN
        dischargeOn = true;
        digitalWrite(leaf4, LOW);
      }else { // CLOSE
        dischargeOn = false;
        digitalWrite(leaf4, HIGH);
      }
      break;
  }
  if(dischargeOn == false) {
    digitalWrite(pump, HIGH);
    Serial.println("Pump off.");
  }
  else {
    digitalWrite(pump, LOW);
    Serial.println("Pump on.");
  }
}

void recieveData() {
  DeserializationError error = deserializeJson(recvData, Serial);
  if (error) {
    Serial.print(F("Error decoding recieved data!\nError: "));
    Serial.println(error.c_str());
    return;
  }else {
    const char* origin = recvData["origin"];
    const int request = recvData["request"];
  
    Serial.print("Origin: ");
    Serial.println(origin);
  
    Serial.print("Request: ");
    Serial.println(request);

    relaySwitch(origin, int(request));
  }
}

void setup() {
  Serial.begin(baudRate);

  setupRelays();
  Serial.println("Waiting for data...");
}

void loop() {
  recieveData();
}

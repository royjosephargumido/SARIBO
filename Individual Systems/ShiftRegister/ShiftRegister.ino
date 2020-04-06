/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU 12E esp8266 Module Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SHIFT REGISTER PROTOTYPE SYSTEM ONLY
  
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

#include <ESP8266WiFi.h>

//================ FUNCTION PROTOTYPING ==============
void initRegisters();
void writeRegisters();
void setPin(int index, int value);
int leaf(const int leafNumber, const bool openLine);
//====================================================

//================= PIN CONFIGURATION ================
const int SERPin = 13;   //74HC595 Pin 14 | GPIO 13 | D7
const int RCLKPin = 12;  //74HC595 Pin 12 | GPIO 12 | D6
const int SRCLKPin = 14; //74HC595 Pin 10 | GPIO 14 | D5
//====================================================

//=========== SHIFT REGISTER CONFIGURATION ===========
//-----Change the value if daisy-chaining 74HC595s----
const int numberofRegisters = 1; 
//----------------------------------------------------
const int totalPins = numberofRegisters * 8;
boolean registers[totalPins];
//====================================================

void initRegisters() {
  pinMode(SERPin, OUTPUT);
  pinMode(RCLKPin, OUTPUT);
  pinMode(SRCLKPin, OUTPUT);
  
  //This clears the registers
  for(int i = totalPins - 1; i >=  0; i--)
     registers[i] = HIGH;
}

void setPin(int index, int value) {
  registers[index] = value;
}

void writeRegisters() {
  Serial.print(registers[0]);
  Serial.print(registers[1]);
  Serial.print(registers[2]);
  Serial.print(registers[3]);
  Serial.print(registers[4]);
  Serial.print(registers[5]);
  Serial.print(registers[6]);
  Serial.println(registers[7]);
  
  digitalWrite(RCLKPin, LOW);
  
  for(int i = totalPins - 1; i >=  0; i--){
    digitalWrite(SRCLKPin, LOW);
    int val = registers[i];
    digitalWrite(SERPin, val);
    digitalWrite(SRCLKPin, HIGH);
  }
  digitalWrite(RCLKPin, HIGH);
}

int leaf(const int leafNumber, const bool openLine) {
  if(leafNumber + 2 < totalPins) {
    if(openLine == true) {
        setPin(leafNumber + 2, LOW);
        return 1;
    }else if(openLine == false) {
        setPin(leafNumber + 2, HIGH);
        return 2;
    }
  } else {
    return 0;
  }
}

void setup(){
  Serial.begin(115200);
  delay(3000);
  initRegisters();
}

void loop(){  
  leaf(1, true);
  writeRegisters(); 
  delay(1000);

  leaf(1, false);
  writeRegisters();
  delay(1000);
}

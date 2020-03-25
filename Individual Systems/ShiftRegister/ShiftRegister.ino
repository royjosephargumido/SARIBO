/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU 12E esp8266 Module Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  SHIFT REGISTER PROTOTYPE SYSTEM ONLY
  Revision March 25, 2020
  Compatible with SARIBO Version 1.2.7
  
  Note:
  Attach the data pin/ interrupt pin from the water flow sensor to
  GPIO 13 or the Digital Pin 7 of the NodeMCU
  Use digitalPinToInterrupt(pin_number) to get the interrupt pin
  of the used digital pin for the water flow sensor.
  
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

int SER_Pin = 13;   //pin 14 on the 75HC595
int RCLK_Pin = 12;  //pin 12 on the 75HC595
int SRCLK_Pin = 14; //pin 11 on the 75HC595

const int numberofRegisters = 2;
const int totalPins = numberofRegisters * 8;
boolean registers[totalPins];

void initRegisters() {
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);
  
  //This clears the registers
  for(int i = totalPins - 1; i >=  0; i--){
     registers[i] = HIGH;
  }
}

void writeRegisters(){
  digitalWrite(RCLK_Pin, LOW);

  for(int i = totalPins - 1; i >=  0; i--){
    digitalWrite(SRCLK_Pin, LOW);

    int val = registers[i];

    digitalWrite(SER_Pin, val);
    digitalWrite(SRCLK_Pin, HIGH);
  }
  digitalWrite(RCLK_Pin, HIGH);
}

void setup(){
  initRegisters();
}

//set an individual pin HIGH or LOW
void setPin(int index, int value){
  registers[index] = value;
}


void loop(){
  setPin(8, LOW);
  setPin(9, HIGH);
  setPin(10, HIGH);
  setPin(11, HIGH);
  writeRegisters();  //MUST BE CALLED TO DISPLAY CHANGES  
  delay(500);

  setPin(8, HIGH);
  setPin(9, LOW);
  setPin(10, HIGH);
  setPin(11, HIGH);
  writeRegisters();  //MUST BE CALLED TO DISPLAY CHANGES  
  delay(500);

  setPin(8, HIGH);
  setPin(9, HIGH);
  setPin(10, LOW);
  setPin(11, HIGH);
  writeRegisters();  //MUST BE CALLED TO DISPLAY CHANGES  
  delay(500);

  setPin(8, HIGH);
  setPin(9, HIGH);
  setPin(10, HIGH);
  setPin(11, LOW);
  writeRegisters();  //MUST BE CALLED TO DISPLAY CHANGES  
  delay(500);
}

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

//
void setPin(int index, int value){
  /*
   * Set an individual pin HIGH or LOW
   */
  registers[index] = value;
}

void pump() {
  int countOpenLeafLines = 0;
  for(int i = 8; i < totalPins; i++){
     if(registers[i] == 0)  // Leaf Distribution Line is OPEN
      countOpenLeafLines++;
  }
  if(countOpenLeafLines == 0) //There is no more open distribution lines
    setPin(0, HIGH);  //PUMP OFF
  else
    setPin(0, LOW);  //PUMP ON
}

String leaf(const int leafNumber, const bool openLine) {
  const int leafPin = leafNumber + 7;
  String message = "";

  /*  
   * Checks if the leaf to be open has a
   * registered pin number in the system
   * based on the total number of pins available.
   */
  if(leafPin < totalPins) {
    switch(openLine) {
      case true:  //opens the specific leaf distribution line
        setPin(leafPin, LOW);
        message = "Leaf " + (String)leafNumber + " distribution line opened.";
        break;

      case false:  //opens the specific leaf distribution line
        setPin(leafPin, HIGH);
        message = "Leaf " + (String)leafNumber + " distribution line closed.";
        break;
    }
  } else {
    if(openLine == true)
      message = "Unable to open Leaf " + (String)leafNumber + " distribution line.\nUnable to reach the Leaf's port number @ " + (String)leafPin + ".";
    else
      message = "Unable to close Leaf " + (String)leafNumber + " distribution line.\nUnable to reach the Leaf's port number @ " + (String)leafPin + ".";
  }

  pump();
  return message;
}

void setup(){
  initRegisters();
}

void loop(){

  leaf(2, true);
  leaf(3, false);
  leaf(4, false);
  writeRegisters(); //Display changes
  delay(2000);

  leaf(2, false);
  leaf(3, true);
  leaf(4, false);
  writeRegisters();
  delay(2000);

  leaf(2, false);
  leaf(3, false);
  leaf(4, true);
  writeRegisters();
  delay(2000);

  leaf(2, false);
  leaf(3, false);
  leaf(4, false);
  writeRegisters();
  delay(2000);
}

  
/*
  The S.A.R.I.B.O. Root Module - NodeMCU 12E esp8266 Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  Stable Root (NodeMCU v3 (LoLin)) Module Code
  Version 1.3.1 Revision March 27, 2020
  
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


#include <HCSR04.h>

HCSR04 waterLevelSensor(4, 15); //Trigger pin (GPIO 4/D2), Echo pin (GPIO 15/ D8)

//=========== PIN CONFIGURATION TO NODEMCU ===========
int SER_Pin = 13;   //pin 14 on the 75HC595 & NodeMCU GPIO 13/D7 
int RCLK_Pin = 12;  //pin 12 on the 75HC595 & NodeMCU GPIO 12/D6
int SRCLK_Pin = 14; //pin 11 on the 75HC595 & NodeMCU GPIO 14/D5
//====================================================

//===== 74HC595 SHIFT OUT REGISTER CONFIGURATION =====
const int numberofRegisters = 2;
const int totalPins = numberofRegisters * 8;
boolean registers[totalPins];
//====================================================

//===== PIN CONFIG OF 74HC595 SHIFT OUT REGISTER =====
const int pumpPin = 0;  //Pump
const int msvPin = 1;   //Main Source Valve
const int tivPin = 2;   //Tank In Valve
const int tovPin = 3;   //Tank Out Valve
const int tdvPin = 4;    //Tank Drain Valve
//====================================================

//================ TANK CONFIGURATION ================
const float tankClearance = 3.0;  //Clearance between the sensor tip and the max full tank
float fullTankCap = 0;
float halfTankCap = 0;
//====================================================

void initRegisters() {
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);
  
  //This clears the registers
  for(int i = totalPins - 1; i >=  0; i--){
     registers[i] = HIGH;
  }
  writeRegisters();
}

String drain(const bool openDrain) {
  String message = "";
  
  switch(openDrain) {
    case true: //OPEN DRAIN VALVE
      setPin(tdvPin, LOW);
      message = "Drain valve open.";
      break;

    case false: //CLOSE DRAIN VALVE
      setPin(tdvPin, HIGH); //Closes the Drain Valve
      message = "Drain valve closed.";
      break;

    default:
      message = "Unable to perform open/close drain valve.Invalid open drain valve command.";
      break;
  }
  return message;
}

void initTank() {
  double currTCap = 0.0;
  double totalTReads = 0.0;
  int i;

  drain(true);  //Opens the drain valve to ensure tank is empty during initialization
  
  for(i = 1; i < 11; i++) {
    currTCap = waterLevelSensor.dist();
    totalTReads += currTCap;
    Serial.print("Reading #");
    Serial.print(i);
    Serial.print(": ");
    Serial.print("Current: ");
    Serial.println(currTCap);
    delay(1000);
  }

  if(i == 11) {
    fullTankCap = (totalTReads / 10) - tankClearance;
    halfTankCap = fullTankCap / 2;
    Serial.print("Full Tank: ");
    Serial.println(fullTankCap);
  }
  Serial.println("Tank refill on process.");
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
  writeRegisters();
}

void pump() {
  int countOpenLeafLines = 0;
  for(int i = 8; i < totalPins; i++){
     if(registers[i] == 0)  // Leaf Distribution Line is OPEN
      countOpenLeafLines++;
  }
  if(countOpenLeafLines == 0) //There is no more open distribution lines
    setPin(pumpPin, HIGH);  //PUMP OFF
  else
    setPin(pumpPin, LOW);  //PUMP ON
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
      message = "Unable to open Leaf " + (String)leafNumber + " distribution line.";
    else
      message = "Unable to close Leaf " + (String)leafNumber + " distribution line.";
  }

  pump();
  return message;
}

void refillTank() {
  const int waterLevel = checkWaterLevel();
  
  Serial.print("Current Tank Capacity: ");
  Serial.print(waterLevel);
  Serial.print("%");
  Serial.print("\tFull Capacity: ");
  Serial.println(fullTankCap);
  
  if(waterLevel > 101) {
    /*
     * AUTOMATIC WATER TANK SPILL TRIGGER
     * 
     * The current tank water level exceeds the maximum tank capacity set by the system,
     * this will drain the tank for 5 seconds then closes after the time. This is to avoid
     * water to distroy the ultrasonic ranging sensor.
     */
    Serial.println("Current water level exceed maximum tank capacity! Spilling excess water.");
    setPin(pumpPin, HIGH);  //Pump Off
    setPin(msvPin, HIGH);  //Main Source Valve Off
    setPin(tivPin, HIGH);  //Tank In Valve Off
    setPin(tovPin, HIGH);  //Tank Out Valve Off
    drain(true);
    delay(5000);
    drain(false);
  } else if(waterLevel < 50) {
    /*
     * WATER TANK REFILL PROCESS
     * 
     * The current water level in the tank is lesser than the half capacity of the tank.
     * This will pauses all watering process to prioritizes the refill of the tank
     * to ensure that the tank has a sufficient amount of stored water for the system and
     * resumes all watering process when the tank is full.
     */
    
    Serial.println("Water tank refill on process.");
    drain(false); //Closes the drain valve
    setPin(pumpPin, LOW);  //Pump On
    setPin(msvPin, LOW);  //Main Source Valve On
    setPin(tivPin, LOW);  //Tank In Valve On
    setPin(tovPin, HIGH);  //Tank Out Valve Off
    
    while(checkWaterLevel() < 101) {
      Serial.println("Refilling...");
      delay(1000);
    }
    
    setPin(pumpPin, HIGH);  //Pump Off
    setPin(msvPin, HIGH);  //Main Source Valve Off
    setPin(tivPin, HIGH);  //Tank In Valve Off
    
  } else if(waterLevel > 50 && waterLevel < 101){
    /* 
     *  TANK IN SUFFICIENT WATER LEVEL
     */
    Serial.println("Sufficient tank water level.");
    drain(false); //Closes the drain valve
    setPin(msvPin, HIGH);  //Main Source Valve Off
    setPin(tivPin, HIGH);  //Tank In Valve Off
  }

  //writeRegisters();
}

float checkWaterLevel() {
  float currTankCap = ((ceil(fullTankCap - (waterLevelSensor.dist() - tankClearance))) / fullTankCap) * 100;
  return currTankCap;
}

void setup(){
  Serial.begin(115200);
  initRegisters();
  initTank();
}

void loop(){
  refillTank();
  delay(1000);
}

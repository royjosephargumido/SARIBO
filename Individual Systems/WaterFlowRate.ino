/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU 12E esp8266 Module Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  Version 1.02.04 Revision March 17, 2020

  WATER FLOW RATE CHECKING PROTOTYPE SYSTEM ONLY
  Revision March 18, 2020
  
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

// Variable init
const int buttonPin = 13; // variable for D2 pin

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per litre per minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

/* 
 *  Interrupt Service Routine (ISR) should be placed before the setup() function and
 *  the ISR should be placed into the IRAM of the NodeMCU and not in the RAM.
 *  In the normal program compilation, objects and source codes are placed in the RAM,
 *  but ISR's should be placed in the IRAM for faster access.
 *  
 *  This is to avoid the:
 *  ISR not in IRAM! user exception (panic/abort/assert) runtime error
 *  
 *  IRAM (Internal RAM, or on-chip RAM (OCRAM)) is the address range of RAM that is internal to the CPU.
 */
void ICACHE_RAM_ATTR pulseCounter()
{
    // Increment the pulse counter
    pulseCount++;
}

void setup()
{
    Serial.begin(115200); // Start the Serial communication to send messages to the computer
    delay(10);

    pulseCount = 0;
    flowRate = 0.0;
    flowMilliLitres = 0;
    totalMilliLitres = 0;
    oldTime = 0;

    digitalWrite(buttonPin, HIGH);
    attachInterrupt(digitalPinToInterrupt(buttonPin), pulseCounter, RISING);
}

void loop() {
  interrupts();   //Enables interrupts on the Arduino
  delay (1000);   //Wait for 1 second 
  noInterrupts(); //Disable the interrupts on the Arduino

  // Because this loop may not complete in exactly 1 second intervals we calculate
  // the number of milliseconds that have passed since the last execution and use
  // that to scale the output. We also apply the calibrationFactor to scale the output
  // based on the number of pulses per second per units of measure (litres/minute in
  // this case) coming from the sensor.
  flowRate = ((1000 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

  // Note the time this processing pass was executed. Note that because we've
  // disabled interrupts the millis() function won't actually be incrementing right
  // at this point, but it will still return the value it was set to just before
  // interrupts went away.
  oldTime = millis();

  // Divide the flow rate in litres/minute by 60 to determine how many litres have
  // passed through the sensor in this 1 second interval, then multiply by 1000 to
  // convert to millilitres.
  flowMilliLitres = (flowRate / 60) * 1000;

  // Add the millilitres passed in this second to the cumulative total
  totalMilliLitres += flowMilliLitres;

  Serial.print("Flow rate: ");
  Serial.print(flowRate);
  Serial.println(" L./Min.");

  // Print the number of litres flowed in this second
  Serial.print("  Current Liquid Flowing: "); // Output separator
  Serial.print(flowMilliLitres);
  Serial.print("mL/Sec");

  // Print the cumulative total of litres flowed since starting
  Serial.print("  Output Liquid Quantity: "); // Output separator
  Serial.print(totalMilliLitres);
  Serial.println("mL");
  
  // Reset the pulse counter so we can start incrementing again
  pulseCount = 0;
}

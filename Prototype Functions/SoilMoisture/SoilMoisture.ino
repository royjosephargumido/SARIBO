/*
  The S.A.R.I.B.O. Prototype Functions
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SARIBO Soil Moisture Check Function
  Version 1.1.3 Revision April 13, 2020
  
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
 
#include <Wire.h>
#include <ESP8266WiFi.h>

const int smAP = 0;  //Analog 0 Pin of NodeMCU

const int maxd = 1000;
const int mind = 600;
const int ideal = 500;
const int smrp = 60;

bool startWater = false;

void perfSMRead() {
  int count = 0;
  long int readings;
  
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
  
  Serial.print("Final Soil Moisture value: ");
  Serial.println(finalSMV);

  if(finalSMV >= maxd) {
    Serial.println("Irregular reading! Repeating soil moisture reading.");
    //sendrequest(rSoil, finalSMV);
    perfSMRead();
  }
  else if (finalSMV < maxd && finalSMV > mind)  {
    Serial.println("Soil is dry! Starting watering process.");
    //sendrequest(rOpen, finalSMV);
    startWater = true;
  }
  else if (finalSMV <= mind && finalSMV > ideal) {
    Serial.println("Soil is in ideal soil moisture value.");    
    if(startWater == true) {  //Watering Process Started
      Serial.println("Ending watering process.");
      //sendrequest(rClose, finalSMV);
    }
    else {
      Serial.println("Logging soil moisture value.");
      //sendrequest(rGen, finalSMV);
    }
  }
  else {
    Serial.println("Soil is wet!");
    
  }
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println();

  perfSMRead();
}

void loop() {
}

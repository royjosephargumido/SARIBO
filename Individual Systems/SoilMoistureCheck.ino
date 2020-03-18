/*
  The S.A.R.I.B.O. Leaf Module - Arduino Nano Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  Version 1.02.03 Revision March 17, 2020

  SOIL MOISTURE CHECKING PROTOTYPE SYSTEM ONLY
  Revision March 18, 2020
  
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


#include <ArduinoJson.h>    // Used in sending data. Uses the ArduinoJson version 6.14.1
//================================ OBJECTS AND GLOBAL VARIABLES ================================
const String hardwareID = "L1";
const int openDistributionLine = 11;
const int closeDistributionLine = 12;
const int soilMoistureReading = 20;

const int soilmoisturePin = A0;

bool stopSMCheck = false;

int times = 1;
long int soilmoistureReadings = 0;
unsigned long oldTime;
//==============================================================================================

int soilMoistureManagement()
{
  int finalSMV = 0;
  
  if(stopSMCheck == false)
  {
    if(times < 11)
    {
      int sm = analogRead(soilmoisturePin);
      Serial.print("Reading #");
      Serial.print(times);
      Serial.print(": ");
      Serial.println(sm);
          
      soilmoistureReadings += sm;
      times = times + 1;
      delay(1000);
    }

    if(times == 11)
    {
      stopSMCheck = true;
      finalSMV = soilmoistureReadings / 10;
      
      Serial.print("Final Reading: ");
      Serial.println(finalSMV);

      //load the configurations from /System/SYSDEF.TXT
      int maxsoildryness = 1001;    //Maximum soil dryness value
      int minsoildryness = 600;     //Minimum soil dryness value
      int idealsoilmoisture = 450;
  
      if(finalSMV >= maxsoildryness)
      {
        /* 
         *  Soil moisture MISREADINGS.
         *  If the final soil moisture value falls under this range,
         *  the soil moisture is too dry for the error level of the sensor or
         *  the sensor wires are broken giving misreadings.
         *  
         * 20 = code for SOIL MOISTURE READING REQUEST as specified in
         * SARIBO Data Request Standards v. 2.0 rev Mar. 16, 2020
         */
         createRequest(soilMoistureReading, finalSMV);
         Serial.println("\nSoil moisture misreading!");
         Serial.println("Kindly check Leaf01 Soil Moisture Sensor for misreadings or check for broken sensor wires.");
         stopSMCheck = true;
         return 0;
      }else
      {
        if(finalSMV < maxsoildryness && finalSMV > minsoildryness)
        {
          /* 
           * Soil is DRY based from the range of the Soil Dryness set by the user
           * then creates an open distribution valve request to the Root module
           * sending the final soil moisture value as a validating value for double checking.
           * 
           * 11 = code for OPEN DISTRIBUTION VALVE REQUEST as specified in
           * SARIBO Data Request Standards v. 2.0 rev Mar. 16, 2020
           */
          createRequest(openDistributionLine, finalSMV);
          Serial.println("\nSoil is dry!");
          Serial.println("Open Leaf01 distribution valve request sent.");
        }
        else if(finalSMV <= minsoildryness && finalSMV > idealsoilmoisture)
        {
          /* 
           * Soil is in the IDEAL SOIL MOISTURE LEVEL and creates a
           * soil moisture reading request to the Root module sending
           * the final soil moisture value
           * 
           * 20 = code for SOIL MOISTURE READING REQUEST as specified in
           * SARIBO Data Request Standards v. 2.0 rev Mar. 16, 2020
           */
          Serial.println("\nSoil is in ideal soil moisture level.");
          createRequest(soilMoistureReading, finalSMV);
        }
        else if(finalSMV <= idealsoilmoisture)
        {
          /* 
           * Soil is WET when the final soil moisture value is lesser than the
           * minimum soil dryness threshold value then creates a close distribution 
           * valve request to the Root module and attaches the final soil moisture
           * value as a validating value for double checking.
           * 
           * 12 = code for CLOSE DISTRIBUTION VALVE REQUEST as specified in
           * SARIBO Data Request Standards v. 2.0 rev Mar. 16, 2020
           */
          createRequest(closeDistributionLine, finalSMV);
          Serial.println("\nSoil is wet!");
          Serial.println("Close Leaf01 distribution valve request sent.");
        }//end else if(finalSMV < minsoildryness)
        
        stopSMCheck = true;
        return 1;
      }
    }//end if(times == 11)
  }//end if(stopSMCheck == false)
}

void createRequest(int requestCode, int validatingValue) 
{  
  //Insert DateTime Stamping procedures here
  
  /* 
   *  Creates a JSON document and initializes its size based on the assistant
   *  found in the ArduinoJSON documentation website.
   *  
   *  See: arduinojson.org/v6/assistant/
   */
  const size_t capacity = JSON_OBJECT_SIZE(7) + 600;
  DynamicJsonDocument data(capacity);

  data["interrupt"] = 1111;
  data["datesent"] = "March 18, 2020";
  data["timesent"] = "4:17:48 PM";
  data["origin"] = hardwareID;         // The hardware UUID assigned to the specific Leaf Module
  data["request"] = requestCode;       // The request code
  data["value"] = validatingValue;    // The validating value

  /* 
   *  This serializes the JSON object "data" then send through the serial
   *  or sends the  JSON object "data" from Arduino to the NodeMCU via serial communication
   */
  serializeJson(data, Serial);
}


void setup()
{
  Serial.begin(19200);
}

void loop()
{
  if(soilMoistureManagement() == 0)
  {
    stopSMCheck = false;
    soilmoistureReadings = 0;
    times = 1;
    soilMoistureManagement();
  }
}

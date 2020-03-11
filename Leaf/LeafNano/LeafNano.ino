/*
  The S.A.R.I.B.O. Leaf Module
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations

  Version 1.01.01 Revision March 11, 2020
  BSD 3-Clause License
  Copyright (c) 2020, Roy Joseph Argumido
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
#include <RTClib.h>         // Provides the Date Time functionality
#include <SoftwareSerial.h> // Provides the serial communication

DateTime now;     // Creates a DateTime object
RTC_DS3231 rtc;   // Creates the RTC object

int count = 1;              // Used in calculating the number of soil moisture check
bool stopCheck = false;     // Used as a counter whether to continue or stop the soil moisture reading
unsigned long oldTime;      // Used in calculating the  seconds passed since the initial time reading 
long int soilmoisture = 0;  // The soil moisture value is saved as a long data type to cater higher precission

void performRTCCheck() {
  /* 
   *  Checks if the RTC module is connected to the Arduino
   *  Continues to loop until the RTC module is connected to the Arduino.
   */
  if (!rtc.begin()) {
    Serial.println("Couldn't find the RTC module! Please make sure the RTC module is connected to the main board.");
    while (1);
  }

  /* 
   *  Checks if the RTC module  lost its power then synchronizes 
   *  the date and time from any connected Leaf modules
   */
  if (rtc.lostPower()) {
    Serial.println("RTC module lost power! Synchronizing time with server.");
    /*
     * The code for the date and time sncyhronization here
     * following line sets the RTC to the date & time this sketch was compiled
     */
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    /* 
     *  This line sets the RTC with an explicit date & time,
     *  for example to set March 9, 2020 7:58:32 AM you would call:
     *  
     *  rtc.adjust(DateTime(year, month, day, hour, minute, second));
     *  
     *  example:
     *  rtc.adjust(DateTime(2020, 3, 8, 3, 0, 0));
     */
  }
}

void checkSoilMoisture()
{
  int finalSoilMoisture = 0;
  
  if(stopCheck == false)
  {
    /* 
     * reads and compute the average soil moisture sensor
     * once every second for 1 minute for better accuracy
     */
    if(((millis() - oldTime) > 1000) && count < 11)
    {
      soilmoisture += analogRead(A0);   // Reads the soil moisture and adds the old readings

      /*
       * Displays the data for verification
       */
      Serial.print("Reading #");
      Serial.println(count);
      Serial.print("Soil moisture: ");
      Serial.println(soilmoisture);
    
      oldTime = millis();   // Saves the the number of milli seconds past since the reading
      count = count + 1;    // Used in the reading for 1 minute
    }

    // Stops soil moisture reading until 1 minute (60 seconds)
    if(count == 10)
    {
      oldTime = 0;       // Resets the millisecond timer
      count = 0;        // Resets the number of seconds counter
      stopCheck = true; // Stops the reading of the soil moisture
      finalSoilMoisture = soilmoisture / 10;  // Calculates the average soil moisture value
      
      Serial.print("Final Soil moisture: ");
      Serial.println(finalSoilMoisture);

      //load the configurations from /System/SYSDEF.TXT
      int maxsoildryness = 1001;    //Maximum soil dryness value
      int minsoildryness = 500;     //Minimum soil dryness value
      int maxsoilwetness = 190;     //Maximum soil wetness that triggers to close the distribution

      if(finalSoilMoisture < maxsoildryness && finalSoilMoisture > minsoildryness)
      {
        /* 
         * Soil is DRY based from the range of the Soil Dryness set by the user
         * then creates an open distribution valve request to the Root module
         * sending the final soil moisture value as a validating value for double checking.
         * 
         * 101 = code for OPEN distribution valve request as specified in the data request table
         */
        createRequestTable(101, finalSoilMoisture);
        Serial.print("Open Leaf01 distribution valve request sent.");
      }
      else if(SoilMoistureValue < minsoildryness)
      {
        /* 
         * Soil is WET when the final soil moisture value is lesser than the
         * minimum soil dryness threshold value then creates a close distribution 
         * valve request to the Root module and attaches the final soil moisture
         * value as a validating value for double checking.
         * 
         * 102 = code for CLOSE distribution valve request as specified in the data request table
         */
        createRequestTable(102, finalSoilMoisture);
        Serial.print("\n\nClose Leaf01 distribution valve request sent.");
      }
    }
  }
}

void createRequestTable(int requestValue, int validatingValue)
{
  //DateTime Stamping procedures
  DateTime now = rtc.now();
  String DateSent = ((String)now.day()) + "/" + ((String)now.month()) + "/" + ((String)now.year());
  String TimeSent = ((String)now.hour()) + ":" + ((String)now.minute()) + ":" +  ((String)now.second());
  //End DateTime stamping procedures
  
  Serial.println(DateSent);
  Serial.println(TimeSent);
  Serial.println();

  /* 
   *  Creates a JSON document and initializes its size based on the assistant
   *  found in the ArduinoJSON documentation website.
   *  
   *  See: arduinojson.org/v6/assistant/
   */
  const size_t capacity = JSON_OBJECT_SIZE(7) + 600;
  DynamicJsonDocument data(capacity);

  data["sendrequest"] = 1111;                    // 1111 as a validator code for a valid serial communication
  data["id"] = "leaf01-101-11x57x34-2x21x2020";  // Transaction ID
  data["datesent"] = DateSent;                   // The date when the message is created
  data["timesent"] = TimeSent;                   // The time when the message is created
  data["origin"] = "689DB004-E03A-4A37-9944-02CB7B2844BE"; // The hardware UUID assigned to the specific Leaf Module
  data["request"] = requestValue;                // The request code
  data["value"] = validatingValue;               // The validating value

  /* 
   *  This serializes the JSON object "data" then send through the serial
   *  or sends the  JSON object "data" from Arduino to the NodeMCU via serial communication
   */
  serializeJson(data, Serial);
}

void setup() {
  /* 
   *  Sets the baud rate or the channel
   *  also used in as the baud rate for the serial communication
   */
  Serial.begin(19200);

  performRTCCheck();  // Checks the presence of the RTC module during program start
}

void loop() {
  DateTime now = rtc.now(); // Sets the DateTime object "now" as the current DateTime of the system
  checkSoilMoisture();  // Checks the soil moisture
  delay(1000);  // Creates a delay of 1 second 
}

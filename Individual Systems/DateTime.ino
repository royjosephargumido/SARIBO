/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU 12E esp8266 Module Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  Version 1.02.04 Revision March 18, 2020

  SARIBO Date and Time Function
  Compatible to SARIBO Version 1.02.04 and higher
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
 
#include <RTClib.h>

//====================================================
DateTime now;     // Creates a DateTime object
RTC_DS3231 rtc;   // Creates the RTC object

const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char monthNames[127][12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
//====================================================

void setup() {
  Serial.begin(9600);
  delay(3000); // wait for console opening
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    
    // Comment out below lines once you set the date & time.
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now();
  
  String HF = "";
  int h = 0;
  String m = "";
  String s = "";

  // COnverting 24H to 12H with AM/PM designation
  if(now.hour() > 12) {
    h = now.hour() % 12;
    HF = " PM";
  }else
  {
    h = now.hour();
    HF = " AM";
  }

  // Adding the '0' Padding to minute if minute is lesser than 10
  if(now.minute() < 10) { m = "0" + (String)now.minute(); }
  else { m = (String)now.minute(); }

  // Adding the '0' Padding to second if second is lesser than 10
  if(now.second() < 10) { s = "0" + (String)now.second(); }
  else { s = (String)now.second(); }
  
  String Date =  (String)monthNames[now.month()] + ' ' + (String)now.day() + ", " + now.year();
  String Time = (String)h + ':' + (String)m + ':' + (String)s + HF;
  
  Serial.print(Date);
  Serial.print(' ');
  Serial.print(Time);
  Serial.println();
  delay(1000);
}

/*
  SARIBO Date and Time Function
  Revision April 13, 2020
  
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
#include <RTClib.h>

//================== DEVICE OBJECTS ==================
DateTime now;
RTC_DS3231 rtc;
//====================================================

void initRTC() { 
  while(!rtc.begin()) {
    Serial.println("Couldn't find the RTC module!");
    rtcfail++;
    delay(1000);
  }
  if(rtc.lostPower()) {
    Serial.println("RTC lost power! Synchronizing date and time with server.");
  }
}

String getDateTime(const int DTReq) {
  now = rtc.now();
  const char monthNames[12][12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
  String M;
  String d;
  String m;
  String s;

  // Adding the '0' Padding to minute if minute is lesser than 10
  if(now.minute() < 10) { m = "0" + (String)now.minute(); }
  else { m = (String)now.minute(); }

  // Adding the '0' Padding to second if second is lesser than 10
  if(now.second() < 10) { s = "0" + (String)now.second(); }
  else { s = (String)now.second(); }

  // Adding the '0' Padding to month if month is lesser than 10
  if(now.month() < 10) { M = "0" + (String)now.month(); }
  else { M = (String)now.month(); }

  // Adding the '0' Padding to day if day is lesser than 10
  if(now.day() < 10) { d = "0" + (String)now.day(); }
  else { d = (String)now.day(); }
  
  switch(DTReq) {
    case 0: //Time in 24H Format
      return (String)((String)now.hour() + ":" + m + ":" + s);
      break;

    case 1: //Date in Normal Format (January 01, 1901)
      return (String)((String)monthNames[now.month() - 1] + " " + d + ", " + now.year());
      break;

    case 2: //Date in URL Compliant Format (January+01,+1901)
      return (String)((String)monthNames[now.month() - 1] + "+" + d + ",+" + now.year());
      break;

    case 3: //Time in Transaction ID Format (120000) HHmmss
      return (String)(now.hour() + m + s);
      break;
      
    case 4: //Date in Transaction ID Format (01011901) MMDDYYYY
      return (String)(M + d + now.year());
      break;
  }
}

void setup() {
  Serial.begin(9600);
  delay(3000); // wait for console opening
  
  initRTC();
}

void loop() {
  Serial.print("Time in 24H Format: ");
  Serial.println(getDateTime(0));
  
  Serial.print("Date in Normal Format: ");
  Serial.println(getDateTime(1));
  
  Serial.print("Date in URL Compliant Format: ");
  Serial.println(getDateTime(2));
  
  Serial.print("Time in Transaction ID Format: ");
  Serial.println(getDateTime(3));
  
  Serial.print("Date in Transaction ID Format: ");
  Serial.println(getDateTime(4));
  
  delay(2000);
}

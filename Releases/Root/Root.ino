/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU 12E esp8266 Module Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SARIBO WIFI COMMUNICATION STABLE PROTOTYPE - ROOT MODULE
  Compatible to SARIBO Leaf Version 1.2.7 and higher
  Version 1.1.3 Revision March 21, 2020
  
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

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <RTClib.h>         // Provides the Date Time functionality

RTC_DS3231 rtc;   // Creates the RTC object

//================= NETWORK PARAMETERS ===============
const char* ssid = "SARIBO Server - Argumido";
const char* password = "1234567890";
const char* host = "192.168.4.1";
String urlPath = "/requests/";

ESP8266WebServer server(80);

DeserializationError err;
const size_t capacity = JSON_OBJECT_SIZE(5) + 90;
DynamicJsonDocument requestData(capacity);
DynamicJsonDocument responseData(capacity);
String requestPayload = "";
String responsePayload = "";
//====================================================

void performRTCCheck()
{
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

String getDateTime(int DT)
{
  /* 
   *  0 = returns the current date
   *  1 = returns the current time
   *  2 = return the current date and time with a newline delimiter
   */
  DateTime now = rtc.now();
  const char monthNames[12][12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
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
  
  String Date =  (String)monthNames[now.month()] + ' ' + (String)now.day() + ", " + (String)now.year();
  String Time = (String)h + ':' + (String)m + ':' + (String)s + (String)HF;

  switch(DT)
  {
    case 0:
      return Date;
      break;

    case 1:
      return Time;
      break;

    case 2:
      return (String)Date + '\n' + (String)Time;
      break;

    default:
      return "Invalid getDateTime return code! \n0 = returns the current date\n1 = returns the current time\n2 = returns the current date and time.";
      break;
  }
}

void handleRoot() {
  if (server.hasArg("data")) {
    requestPayload = server.arg(0);
    Serial.println("Leaf request data recieved.");
  }

  err = deserializeJson(requestData, requestPayload);
  if (err) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(err.c_str());
  }

  const char* origin = requestData["origin"];
  const char* datesent = requestData["datesent"];
  const char* timesent = requestData["timesent"];
  int request = requestData["request"];
  int value = requestData["value"];

  Serial.println("============ REQUEST INFORMATION ============");
  Serial.print("Origin: ");
  Serial.println(origin);
  
  Serial.print("Date Sent: ");
  Serial.println(datesent);
  
  Serial.print("Time Sent: ");
  Serial.println(timesent);
  
  Serial.print("Request: ");
  Serial.println(request);
  
  Serial.print("Value: ");
  Serial.println(value);
  Serial.println("=============================================");
  
  responseData["origin"] = "HF7890";
  responseData["datesent"] = getDateTime(0);
  responseData["timesent"] = getDateTime(1);
  responseData["request"] = 10;
  responseData["value"] = "Leaf01 Open Distribution Line Request Approved.";

  const char* xo = responseData["origin"];
  const char* xds = responseData["datesent"];
  const char* xts = responseData["timesent"];
  int xr = responseData["request"];
  const char* xv = responseData["value"];

  responsePayload = "";
  serializeJson(responseData, responsePayload);
  
  server.send(200, "text/plain", responsePayload);
  
  Serial.println("============ RESPONSE INFORMATION ===========");
  Serial.print("Origin: ");
  Serial.println(xo);
  
  Serial.print("Date Sent: ");
  Serial.println(xds);
  
  Serial.print("Time Sent: ");
  Serial.println(xts);
  
  Serial.print("Request: ");
  Serial.println(xr);
  
  Serial.print("Value: ");
  Serial.println(xv);
  Serial.println("=============================================");
}

void startServer() {
  Serial.println();
  
  Serial.println("Starting Root Module Server...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();

  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(myIP);
  
  server.on(urlPath, handleRoot);
  server.begin();
  Serial.print("SARIBO Root Server @ ");
  Serial.print(myIP);
  Serial.println(urlPath);
  Serial.println("SARIBO Root Server running.");
}

void setup() {
  Serial.begin(115200);

  performRTCCheck();  // Checks the presence of the RTC module during program start

  startServer();
}

void loop() {
  server.handleClient();
}

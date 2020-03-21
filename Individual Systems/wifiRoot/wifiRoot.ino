/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU 12E esp8266 Module Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SARIBO WIFI COMMUNICATION STABLE PROTOTYPE - ROOT MODULE
  Compatible to SARIBO Version 1.2.4 and higher
  Version 1.3 Revision March 21, 2020
  
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
  responseData["datesent"] = "April 1, 2020";
  responseData["timesent"] = "10:42:11 PM";
  responseData["request"] = 10;
  responseData["value"] = "Leaf01 Open Distribution Line Request Approved.";

  responsePayload = "";
  serializeJson(responseData, responsePayload);
  
  server.send(200, "text/plain", responsePayload);
  Serial.println("============ RESPONSE INFORMATION ===========");
  Serial.println(responsePayload);
  Serial.println("=============================================");
}

void setup() {
  Serial.begin(115200);
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

void loop() {
  server.handleClient();
}

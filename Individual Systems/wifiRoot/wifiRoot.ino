/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU 12E esp8266 Module Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SARIBO PROTOTYPE FOR WIFI COMMUNICATION - ROOT MODULE
  Compatible to SARIBO Version 1.2.4 and higher
  Version 1.2 Revision March 20, 2020
  
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
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const char *ssid      = "SARIBO Server - Argumido";
const char *password  = "1234567890";

ESP8266WebServer server(80);

const size_t capacity = JSON_OBJECT_SIZE(5) + 90;
DynamicJsonDocument data(capacity);

void handleSentVar() {
  String sensor_values;
  if (server.hasArg("value")) {
    sensor_values = server.arg(0);
    Serial.println(sensor_values);
  }

  DeserializationError err = deserializeJson(data, sensor_values);
  if (err) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(err.c_str());
  }
  
  const char* datesent = data["datesent"];
  const char* timesent = data["timesent"];
  const char* origin = data["origin"];
  int request = data["request"];
  int value = data["value"];
  
  Serial.println(F("Response:"));
  Serial.print("Date Sent: ");
  Serial.println(datesent);
  Serial.print("Time Sent: ");
  Serial.println(timesent);
  Serial.print("Origin: ");
  Serial.println(origin);
  Serial.print("Request: ");
  Serial.println(request);
  Serial.print("Value: ");
  Serial.println(value);

  server.send(200, "text/plain", "Data received");
}

void setup() {
  Serial.begin(9600);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  
  server.on("/data/", HTTP_GET, handleSentVar);
  server.begin();

  Serial.println("Root running @ COM 9");
  Serial.print("IP: ");
  Serial.print(myIP);
}

void loop() {
  server.handleClient();
}

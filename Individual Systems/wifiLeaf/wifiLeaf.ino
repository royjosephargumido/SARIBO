/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU 12E esp8266 Module Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SARIBO WIFI COMMUNICATION STABLE PROTOTYPE - LEAF MODULE
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

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

//================= NETWORK PARAMETERS ===============
const char* ssid = "SARIBO Server - Argumido"; 
const char* password = "1234567890"; 
const char* host = "192.168.4.1";
String urlPath = "/requests/?data=";
const int httpPort = 80;

WiFiClient client;

DeserializationError err;
const size_t capacity = JSON_OBJECT_SIZE(5) + 600;
DynamicJsonDocument requestData(capacity);
DynamicJsonDocument responseData(capacity);
String requestPayload = "";
String responsePayload = "";
//====================================================

int buff = 1;
void setup() {
  Serial.begin(115200);
  Serial.println();
  
  Serial.print("\nConnecting to ");
  Serial.print(ssid);
  Serial.println("...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) { delay(1000); }
  
  Serial.println();
  Serial.print("Leaf connected to Root Server (");
  Serial.print(ssid);
  Serial.print(") @ ");
  Serial.print(host);
  Serial.print(" on port ");
  Serial.println(httpPort);

  Serial.print("Leaf IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  requestPayload = "";
  while(!client.connect(host, httpPort)) {
    Serial.print("Failed to connect to ");
    Serial.print(host);
    Serial.print(" on port ");
    Serial.println(httpPort);
    delay(1000);
  }
  
  requestData["origin"] = "9E6R46";
  requestData["datesent"] = "April+1,+2020";
  requestData["timesent"] = "10:32:24+PM";
  requestData["request"] = 11;
  requestData["value"] = 762;
  serializeJson(requestData, requestPayload);

  String urlData = urlPath + requestPayload;
  client.print(String("GET ") + urlData + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while(client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout!");
      client.stop();
      return;
    }
  }

  while(client.available()) {
    responsePayload = client.readStringUntil('\r');
  }

  err = deserializeJson(responseData, responsePayload);
  if(err) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(err.c_str());
  }
  
  const char* origin = responseData["origin"];
  const char* datesent = responseData["datesent"];
  const char* timesent = responseData["timesent"];
  int request = responseData["request"];
  const char* value = responseData["value"];

  Serial.println("============ RESPONSE INFORMATION ============");
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
}

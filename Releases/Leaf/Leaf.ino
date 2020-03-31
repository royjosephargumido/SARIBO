/*
  The S.A.R.I.B.O. Leaf Module - NodeMCU 12E esp8266 Module Code
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  OFFICIAL STABLE RELEASE - Leaf Module, Arduino Nano Code
  Version 1.2.7 Revision March 22, 2020
  
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
#include <RTClib.h>         // Provides the Date Time functionality
#include <ESP8266WiFi.h>

//=========== PIN CONFIGURATION TO NODEMCU ===========
const String hardwareID = "FYF2U734";
const int soilmoisturePin = A0;
const int waterflowsensorPin = 13; // variable for D7/GPIO Pin 13
//====================================================

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
//====================================================

//============== DATA REQUEST STANDARDS ==============
/* 
 * DO NOT MODIFY THIS CODES WITHOUT MODIFYING THE CODES
 * PRESENT IN THE SERVER SIDE
 * 
 * THIS COMPLIES WITH THE
 * SARIBO Data Request Standards v. 2.0 rev Mar. 16, 2020
 */
const int openDistributionLine = 11;
const int closeDistributionLine = 12;
const int soilMoistureReading = 20;
const int waterFlowRateReading = 30;
const int powerReading = 40;
const int timeReading = 50;
const int syncTime = 51;
const int networkReading = 60;
const int sendPingRequest = 61;
const int getServerSettings = 70;
const int setClientSetting = 71;
//====================================================

//================================ OBJECTS AND GLOBAL VARIABLES ================================
RTC_DS3231 rtc;   // Creates the RTC object

bool stopSMCheck = false;     // Used as a counter whether to continue or stop the soil moisture reading

int times = 1;
unsigned long oldTime;      // Used in calculating the  seconds passed since the initial time reading

long int soilmoistureReadings = 0;  // The soil moisture value is saved as a long data type to cater higher precission

float calibrationFactor = 4.5;  //The hall-effect flow sensor outputs approximately 4.5 pulses per second per litre per minute of flow.
volatile byte pulseCount = 0;
float flowRate = 0.00;
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 0;
//==============================================================================================

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

float calculateWaterFlow()
{
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
  /*
  // Print the number of litres flowed in this second
  Serial.print("Current Liquid Flowing: "); // Output separator
  Serial.print(flowMilliLitres);
  Serial.print("mL/Sec");
  // Print the cumulative total of litres flowed since starting
  Serial.print("Output Liquid Quantity: "); // Output separator
  Serial.print(totalMilliLitres);
  Serial.println("mL");
  */
  // Reset the pulse counter so we can start incrementing again
  pulseCount = 0;

  return flowRate;
}

void connectToServer() {
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

void connectToHost() {
  while(!client.connect(host, httpPort)) {
    Serial.print("Failed to connect to ");
    Serial.print(host);
    Serial.print(" on port ");
    Serial.println(httpPort);
    delay(1000);
  }
}

void getServerResponse() {
  unsigned long timeout = millis();
  while(client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout!");
      client.stop();
      return;
    }
  }

  String responsePayload = "";
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
         Serial.println("\nSoil moisture misreading!");
         Serial.println("Kindly check Leaf01 Soil Moisture Sensor for misreadings or check for broken sensor wires.");
         pushRequest(soilMoistureReading, finalSMV);
         
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
          Serial.println("\nSoil is dry!");
          Serial.println("Open Leaf01 distribution valve request sent.");
          pushRequest(openDistributionLine, finalSMV);
          
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
          pushRequest(soilMoistureReading, finalSMV);
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
          Serial.println("\nSoil is wet!");
          Serial.println("Close Leaf01 distribution valve request sent.");
          pushRequest(closeDistributionLine, finalSMV);
        }//end else if(finalSMV < minsoildryness)
        
        stopSMCheck = true;
        return 1;
      }
    }//end if(times == 11)
  }//end if(stopSMCheck == false)
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
    HF = "+PM";
  }else
  {
    h = now.hour();
    HF = "+AM";
  }

  // Adding the '0' Padding to minute if minute is lesser than 10
  if(now.minute() < 10) { m = "0" + (String)now.minute(); }
  else { m = (String)now.minute(); }

  // Adding the '0' Padding to second if second is lesser than 10
  if(now.second() < 10) { s = "0" + (String)now.second(); }
  else { s = (String)now.second(); }
  
  String Date =  (String)monthNames[now.month()] + '+' + (String)now.day() + ",+" + (String)now.year();
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

void pushRequest(int requestCode, int validatingValue) 
{
  connectToHost();
  
  String requestPayload = "";
  
  requestData["datesent"] = getDateTime(0);
  requestData["timesent"] = getDateTime(1);
  requestData["origin"] = hardwareID;         // The hardware UUID assigned to the specific Leaf Module
  requestData["request"] = requestCode;       // The request code
  requestData["value"] = validatingValue;    // The validating value

  const char* origin = requestData["origin"];
  const char* datesent = requestData["datesent"];
  const char* timesent = requestData["timesent"];
  int request = requestData["request"];
  int value = requestData["value"];

  /* 
   *  This serializes the JSON object "data" then send through the serial
   *  or sends the  JSON object "data" from Arduino to the NodeMCU via serial communication
   */
  serializeJson(requestData, requestPayload);

  String urlData = urlPath + requestPayload;
  client.print(String("GET ") + urlData + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

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
  
  getServerResponse();
  Serial.println("Response recieved.");
}

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

void setup() {
  /* 
   *  Sets the baud rate or the channel
   *  also used in as the baud rate for the serial communication
   */
  Serial.begin(115200);
  
  digitalWrite(waterflowsensorPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(waterflowsensorPin), pulseCounter, RISING);

  performRTCCheck();  // Checks the presence of the RTC module during program start

  connectToServer();
}

void loop() {
  int res = soilMoistureManagement();
  if(res == 0)
  {
    /* 
     * soilMoistureManagement() will return 0 if it reads inaccurate soil moisture readings
     * then repeats until it get accurate readings
     */
    stopSMCheck = false;
    soilmoistureReadings = 0;
    times = 1;
    soilMoistureManagement();
  }
}

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>

const char* wifiName = "SARIBO Server - Argumido";
const char* wifiPass = "1234567890";
const char* host = "http://192.168.8.108";

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  
  Serial.print("Connecting to ");
  Serial.println(wifiName);

  WiFi.begin(wifiName, wifiPass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP
}

void loop() {
  HTTPClient http;    //Declare object of class HTTPClient

  // Send request
  http.useHTTP10(true);
  http.begin(host);
  http.GET();

  DynamicJsonDocument data(2048);
  deserializeJson(data, http.getStream());

  // Decode JSON/Extract values
  String Date = data["datesent"];
  String Time = data["timesent"];
  String Origin = data["origin"];
  String Request = data["request"];
  String Value = data["value"];
  
  Serial.println(F("Response:"));
  Serial.println(Date);
  Serial.println(Time);
  Serial.println(Origin);
  Serial.println(Request);
  Serial.println(Value);

  http.end();  //Close connection
  delay(5000);  //GET Data at every 5 seconds
}

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>

const char* wifiName = "SARIBO Server - Argumido";
const char* wifiPass = "1234567890";
IPAddress ip(192,168,8,108);
IPAddress gateway(192,168,11,4);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(wifiName, wifiPass);
  
  Serial.begin(115200); //NodeMCU serial baud rate channel
  
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println("192.168.8.108");
  
  // Configure the server's routes
  server.on("/", handleRequests); // use this route to update the sensor value
  server.begin();
  Serial.println("Root running @ COM 9");
}

void loop() { server.handleClient(); }

void handleRequests()
{
  String payload;
  
  const size_t capacity = JSON_OBJECT_SIZE(5) + 500;
  DynamicJsonDocument data(capacity);

  data["datesent"] = "April 19, 2020";
  data["timesent"] = "1:16:24 PM";
  data["origin"] = "92EC9416";         // The hardware UUID assigned to the specific Leaf Module
  data["request"] = 11;       // The request code
  data["value"] = 678;    // The validating value

  serializeJson(data, payload);
  server.send(200, "text/html", payload);  
}

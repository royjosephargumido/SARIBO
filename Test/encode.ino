#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

const char* wifiName = "HUAWEI-C7D9";
const char* wifiPass = "amamfbt0";
  
ESP8266WebServer server(80);  //Define server object

//Handles http request 
void handleRoot() {
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
   
// the setup function runs once when you press reset or power the board
void setup() {
  
  Serial.begin(115200);
  delay(10);
  Serial.println();
  
  Serial.print("Connecting");
  
  WiFi.begin(wifiName, wifiPass);   //Connect to WiFi

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP

  server.on("/", handleRoot);      //Associate handler function to web requests
    
  server.begin(); //Start web server
  Serial.println("HTTP server started");
}

void loop() {
  //Handle Clinet requests
  server.handleClient();
}

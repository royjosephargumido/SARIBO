#include <ESP8266WiFi.h>

// Initialize network parameters
const char * HydraRootSSID = "Hydra Server";
const char * HydraRootPassword = "1234567890";
const char* host = "192.168.11.4"; // as specified in the server configuration

// Initialize sensor parameters
int moisture = 0;

// Set up the client objet
WiFiClient client;

void setup() {
  // Connect to the server
  WiFi.begin(HydraRootSSID, HydraRootPassword);
  Serial.begin(9600);
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address (AP): "); Serial.println(WiFi.localIP());
}

void loop() {
  // Get a measurement from the sensor
  moisture = analogRead(A0);
  Serial.print("Soil moisture: "); Serial.println(moisture);
  
  // Connect to the server and send the data as a URL parameter
  if(client.connect(host,80)) {
    String url = "/update?value=";
    url += String(moisture);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host +  "\r\n" + 
                 "Connection: keep-alive\r\n\r\n"); // minimum set of required URL headers
    delay(10);
    // Read all the lines of the response and print them to Serial
    
    Serial.println("Response: ");
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
  delay(1000);
}

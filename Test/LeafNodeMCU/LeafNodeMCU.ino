#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

// Initialize network parameters
const char * HydraRootSSID = "Hydra Server";
const char * HydraRootPassword = "1234567890";
const char* host = "192.168.11.4"; // as specified in the server configuration

String data = "";

// Set up the client objet
WiFiClient client;

void setup() {
  Serial.begin(19200);
  
  // Connect to the server
  WiFi.begin(HydraRootSSID, HydraRootPassword);
  
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address (AP): "); Serial.println(WiFi.localIP());
}

void loop() {
  const size_t capacity = JSON_OBJECT_SIZE(10) + 600;
  DynamicJsonDocument rawData(capacity);

  // Parse JSON object
  deserializeJson(rawData, Serial);

  int sendrequest = rawData["sendrequest"];
  if(sendrequest == 1111)
  {
    /*
    const char* id = doc["id"];             // "leaf01-101-11x57x34-2x21x2020"
    const char* datesent = doc["datesent"]; // "2/21/2020"
    const char* timesent = doc["timesent"]; // "11:57:34"
    const char* origin = doc["origin"];     // "689DB004-E03A-4A37-9944-02CB7B2844BE"
    int request = doc["request"];           // 101
    int value = doc["value"];               // 765
    */
    
    DynamicJsonDocument processedData(capacity);
    
    processedData["id"] = rawData["id"];
    processedData["datesent"] = rawData["datesent"];
    processedData["timesent"] = rawData["timesent"];
    processedData["origin"] = rawData["origin"];
    processedData["request"] = rawData["request"];
    processedData["value"] = rawData["value"];

    String payload = "";
    serializeJson(processedData, payload);  // Sends the data to the NodeMCU via the serial communication
    Serial.println(payload);

    // Connect to the server and send the data as a URL parameter
    if(client.connect(host, 80)) {
      String url = "/requests?packet=";
      url += String(payload);
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

    /*
    Serial.print("ID: ");
    Serial.println(id);
    
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
  
    Serial.print("\n\n");
    */
  }else
  {
    Serial.println("Waiting for data...\n\n");
  }
  
  delay(500);
}

#include <ArduinoJson.h>

String message = "";
bool messageReady = false;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Monitor serial communication
  while(Serial.available()) {
    message = Serial.readString();
    messageReady = true;
  }
  // Only process message if there's one
  if(messageReady) {
    // The only messages we'll parse will be formatted in JSON
    DynamicJsonDocument doc(1024); // ArduinoJson version 6+
    // Attempt to deserialize the message
    DeserializationError error = deserializeJson(doc,message);
    if(error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      messageReady = false;
      return;
    }
    if(doc["type"] == "request") {
      doc["type"] = "response";
      // Get data from analog sensors
      doc["distance"] = analogRead(A0);
      doc["gas"] = analogRead(A1);
      serializeJson(doc,Serial);
    }
    messageReady = false;
  }
}

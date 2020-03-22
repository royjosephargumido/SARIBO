#include <ArduinoJson.h>

const size_t capacity = JSON_OBJECT_SIZE(2) + 50;
DynamicJsonDocument recvData(capacity);

void recieveData() {
  DeserializationError error = deserializeJson(recvData, Serial);
  if (error) {
    //Serial.print(F("deserializeJson() failed: "));
    //Serial.println(error.c_str());
    return;
  }else {
    const char* origin = recvData["origin"];
    const int request = recvData["request"];
  
    Serial.print("Origin: ");
    Serial.println(origin);
  
    Serial.print("Request: ");
    Serial.println(request);
  }
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  recieveData();
}

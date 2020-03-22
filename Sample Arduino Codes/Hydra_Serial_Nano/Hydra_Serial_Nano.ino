#include <ArduinoJson.h>

const size_t capacity = JSON_OBJECT_SIZE(3) + 100;
DynamicJsonDocument recvData(capacity);

void setup() {
  Serial.begin(9600);
}

void loop() {
  DeserializationError error = deserializeJson(recvData, Serial);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }else {
    const char* origin = recvData["origin"];
    const int request = recvData["request"];
    const int value = recvData["value"];
  
    Serial.print("Origin: ");
    Serial.println(origin);
  
    Serial.print("Request: ");
    Serial.println(request);
  
    Serial.print("Value: ");
    Serial.println(value);
  }
  delay(1000);
}

#include <ArduinoJson.h>

const size_t capacity = JSON_OBJECT_SIZE(2) + 50;
DynamicJsonDocument recvData(capacity);

void recieveData() {
  String rawData = "";
  DeserializationError error = deserializeJson(recvData, Serial);
  serializeJson(recvData, rawData);
  if (error) {
    //Serial.print(F("deserializeJson() failed: "));
    //Serial.println(error.c_str());
    return;
  }else {
    Serial.println(rawData);
  }
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  recieveData();
}

#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial s(3,1);

const size_t capacity = JSON_OBJECT_SIZE(3) + 100;
DynamicJsonDocument transData(capacity);

void setup() {
  s.begin(9600);
}


void loop() {
  transData["origin"] = "92EC9416";
  transData["request"] = 11;
  transData["value"] = 765;

  serializeJson(transData, s);
  delay(1000);
}

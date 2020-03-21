#include <SoftwareSerial.h> //Included SoftwareSerial Library
#include <ArduinoJson.h>

//Started SoftwareSerial at RX and TX pin of ESP8266/NodeMCU
SoftwareSerial s(3,1);

void setup() {
  Serial.begin(9600);
}

void loop() {
  const size_t capacity = JSON_OBJECT_SIZE(6);
  DynamicJsonDocument doc(capacity);
  
  doc["id"] = "leaf01-101-11x57x34-2x21x2020";
  doc["datesent"] = "2/21/2020";
  doc["timesent"] = "11:57:34";
  doc["origin"] = "689DB004-E03A-4A37-9944-02CB7B2844BE";
  doc["request"] = 101;
  doc["value"] = 765;
  
  serializeJson(doc, Serial);
  delay(500);
}

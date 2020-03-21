#include <ArduinoJson.h>
#include <SoftwareSerial.h>

int data; //Initialized variable to store recieved data

void setup() {
  //Serial Begin at 9600 Baud 
  Serial.begin(9600);
}

void loop() {
  const size_t capacity = JSON_OBJECT_SIZE(3) + 40;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, Serial);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Extract values
  const char* id = doc["id"];             // "leaf01-101-11x57x34-2x21x2020"
  const char* datesent = doc["datesent"]; // "2/21/2020"
  const char* timesent = doc["timesent"]; // "11:57:34"
  const char* origin = doc["origin"];     // "689DB004-E03A-4A37-9944-02CB7B2844BE"
  int request = doc["request"];           // 101
  int value = doc["value"];               // 765

  Serial.println(id);
  Serial.println(datesent);
  Serial.println(timesent);
  Serial.println(origin);
  Serial.println(request);
  Serial.println(value);
  
  delay(500);
}

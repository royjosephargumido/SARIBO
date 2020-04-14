#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//================ FUNCTION PROTOTYPING ==============
void initServer();
String generateHID();
void decodeJsonData(const DeserializationError error);
void handleRoot();
//====================================================

//============== DEFAULT CONFIGURATIONS ==============
const char* __ssid = "SARIBO-CHARLIE";
const char* __key = "1234567890";
const char* __hostIP = "192.168.4.1";
const int __port = 80;
const char* __reqPath = "/requests/";

const char* __rootHID = "HF7890QN";

ESP8266WebServer server(__port);
//====================================================

//=================== REQUEST CODES ==================
const int rDGen = 10;
const int rOpen = 11;
const int rClose = 12;
const int rPower = 20;
const int rDTRead = 30;
const int rDTSync = 31;
const int rSoil = 41;
const int rHID = 42;
//====================================================

//============== ARDUINOJSON COMPONENTS ==============
DeserializationError err;

const size_t capacity = JSON_OBJECT_SIZE(4) + 2000;
DynamicJsonDocument requestData(capacity);
DynamicJsonDocument responseData(capacity);
//====================================================

void initServer() {  
  Serial.println("Starting Root Server...");
  
  WiFi.softAP(__ssid, __key);
  IPAddress myIP = WiFi.softAPIP();
  server.on(__reqPath, handleRoot);
  server.begin();
  
  Serial.println("SARIBO Root Server running.");
  Serial.print("SSID: ");
  Serial.println(__ssid);
  Serial.print("IP Address: ");
  Serial.println(myIP);

  digitalWrite(0, HIGH);
}

void decodeJsonData(const DeserializationError error) {
  if(error) {
    Serial.print(F("Unable to decode message! Error: "));
    Serial.println(error.c_str());
  }
}

void handleRoot() {
  String requestPayload = "";
  String responsePayload = "";
  String _hid = "";
  
  if(server.hasArg("data")) { requestPayload = server.arg(0); }

  decodeJsonData(deserializeJson(requestData, requestPayload));
  
  const char* id = requestData["id"];
  const char* origin = requestData["origin"];
  const int request = requestData["request"];
  const int value = requestData["value"];

  Serial.println("============ REQUEST INFORMATION ============");
  Serial.print("Id: ");
  Serial.println(id);
  Serial.print("Origin: ");
  Serial.println(origin);
  Serial.print("Request: ");
  Serial.println(request);
  Serial.print("Value: ");
  Serial.println(value);
  Serial.println("=============================================");
  
  responseData["id"] = id;
  responseData["origin"] = __rootHID;
  responseData["request"] = 10;
  responseData["value"] = "Northwest Samar State University";
  
  responsePayload = "";
  serializeJson(responseData, responsePayload);
  server.send(200, "text/plain", responsePayload);

  Serial.println("============ RESPONSE INFORMATION ============");
  Serial.print("Id: ");
  Serial.println(id);
  Serial.print("From: ");
  Serial.println(__rootHID);
  Serial.print("Request: ");
  Serial.println(request);
  Serial.print("Value: ");
  Serial.println(_hid);
  Serial.println("=============================================");

  digitalWrite(2, HIGH);
  delay(5000);
  digitalWrite(2, LOW);
}

void setup() {
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  Serial.println();

  randomSeed(millis());
  initServer();
}

void loop() {
  server.handleClient();
}

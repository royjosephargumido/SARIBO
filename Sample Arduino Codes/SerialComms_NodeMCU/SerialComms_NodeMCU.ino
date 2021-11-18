/*
 * PIN DIAGRAM
 * 
 * SD CARD
 * --------------------
 * SD Card  NodeMCU
 * GND      GND
 * VCC      3V3/VCC
 * MISO     D6
 * MOSI     D7
 * SCK      D5
 * CS       D8
 */

#define BLYNK_TEMPLATE_ID "";
#define BLYNK_DEVICE_NAME "";
#define BLYNK_AUTH_TOKEN "";

#include <SD.h>             //Used by the SD Card Module
#include <ArduinoJson.h>    //Used to save device data
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//============ BLYNK COMPONENENTS ============
BlynkTimer timer;
const char auth[] = BLYNK_AUTH_TOKEN;
const char ssid[] = "";
const char pass[] = "";
//============================================

//==================== OTHERS ====================
int ndex = 0;  //Index
const int maxBuffer = 2000;
char data[maxBuffer];
const byte sdCS = D8;  //SD Card Chip Select Pin
unsigned long runtime;
int buttonState;
//================================================

//==================== ARDUINOJSON ====================
const size_t fileBuffer = JSON_OBJECT_SIZE(30) + maxBuffer;
DynamicJsonDocument settingsData(fileBuffer);
DynamicJsonDocument arduinoData(fileBuffer);
//=====================================================

//==================== FILE ====================
const String settingsDir = "System";
const String fpath = settingsDir + "/" + "Settings.txt";
File file;
//==============================================

//==================== CACHE ====================
double _rdp1ph = 0.00;  //Realtime Data - P1 Ph - V0
int _rdp1sm = 0;        //Realtime Data - P1 Soil Moisture - V1
int _tp1sm1 = 0;        //Threshold - P1 Soil Moisture 1 - V13
double _tp1phmin = 0;   //Threshold - P1 Ph - V2
double _tp1phmax = 0;   //Threshold - P1 Ph - V3

double _rdp2ph = 0.00;  //Realtime Data - P2 Ph - V6
int _rdp2sm = 0;        //Realtime Data - P2 Soil Moisture - V7
double _tp2phmin = 0;   //Threshold - P2 Ph - V8
double _tp2phmax = 0;   //Threshold - P2 Ph - V9
int _tp2sm2 = 0;        //Threshold - P2 Soil Moisture 2 - V15
//==============================================

void initSD() {
  //Detects the SD Card Module
  while(!SD.begin(sdCS)) {
    Serial.println("Unable to detect the SD Card module!");
    delay(1000);
  }

  if(SD.exists(fpath)) {    //Check if the SD Card contains the Settings file
    //Opens the file for reading
    do {
      file = SD.open(fpath);
      if(!file) {
        Serial.println("Unable to load settings from the SD Card!");
        delay(500);
      }
    }while(!file);

    //Reads the content of the file
    ndex = 0;
    while(file.available()) {
      data[ndex] = file.read();
      ndex++;
    }
    data[ndex] = '\0';
  } else {      //If the SD Card does not contain the Settings file
    Serial.println("Does not exist.");
    SD.mkdir(settingsDir);

    //Creates the file and then closes it after
    do {
      file = SD.open(fpath, FILE_WRITE);
      if(file) {
        Serial.println("Unable to create the settings file!");
        delay(1000);
      }
    }while(!file);
    
    Serial.println("File created!");
  }  
  file.close();
}

String decodeJsonData(const DeserializationError error) {
  if(error)
    return "Error " + (String)error.c_str();
  else
    return "No decoding error.";
}

void displayCache() { //for checking only
  Serial.println();

  Serial.println("PLANT 1 DATA --------------------------------");
  Serial.println("PH Level Threshold: " + String(_tp1phmin) + "%");
  Serial.println("PH Level Threshold: " + String(_tp1phmax) + "%");
  Serial.println("Water Moisture Threshold (max): " + String(_tp1sm1));
  Serial.println("---------------------------------------------");
  
  Serial.println();
  
  Serial.println("PLANT 2 DATA --------------------------------");
  Serial.println("PH Level Threshold: " + String(_tp2phmin) + "%");
  Serial.println("PH Level Threshold: " + String(_tp2phmax) + "%");
  Serial.println("Water Moisture Threshold (max): " + String(_tp2sm2));
  Serial.println("---------------------------------------------");
}

void loadSettingstoCache() { 
  ndex = 0; //ndex

  //Opens the file for reading
  do {
    file = SD.open(fpath);
    if(!file) {
      Serial.println("Unable to load data from the SD Card to the cache!");
      delay(500);
    }
  }while(!file);

  //reads the content of the file
  while(file.available()) {
    data[ndex] = file.read();
    ndex++;
  }
  data[ndex] = '\0'; //\0 =end of string

  decodeJsonData(deserializeJson(settingsData, data));
  
  _tp1phmin = settingsData["p1phmin"].as<double>();
  _tp1phmax = settingsData["p1phmax"].as<double>();
  _tp1sm1 = settingsData["p1sm1"].as<int>();

  _tp2phmin = settingsData["p2phmin"].as<double>();
  _tp2phmax = settingsData["p2phmax"].as<double>();
  _tp2sm2 = settingsData["p2sm2"].as<int>();

  file.close();
  displayCache();
}

void updateSettings() {
  String rawData;
  SD.remove(fpath); //Deletes the settings file to avoid redundany

  //Opens the file and then closes it after
  do {
    file = SD.open(fpath, FILE_WRITE);
    if(file) {
      Serial.println("Unable to open the settings file!");
      delay(1000);
    }
  }while(!file);

  //Encode cache data to JSON
  settingsData["p1sm1"] = _tp1sm1;
  settingsData["p1phmin"] = _tp1phmin;
  settingsData["p1phmax"] = _tp1phmax;
  settingsData["p2sm2"] = _tp2sm2;
  settingsData["p2phmin"] = _tp2phmin;
  settingsData["p2phmax"] = _tp2phmax;
  
  serializeJson(settingsData, rawData); //Writes the JSON to the string rawData
  file.print(rawData);  //the string rawData is stored in the file
  file.close();
  
  Serial.println("Settings updated!");
  loadSettingstoCache();
}

BLYNK_CONNECTED() {
  Blynk.syncAll();  //Syncs all data from the Blynk server
}

void sendDataToArduino(int key, String value) {
  String rawData;
  
  switch(key) { /*Encodes the data parameter to JSON*/
    case 0: //Pump
      arduinoData["pump"] = value;
      break;

     case 1:  //Threshold - P1 Soil Moisture - V13
      arduinoData["tp1sm"] = value;
      break;

     case 2:  //Threshold - P1 Ph MIN - V2
      arduinoData["tp1phmin"] = value;
      break;

     case 3:  //Threshold - P1 Ph MAX - V3
      arduinoData["tp1phmax"] = value;
      break;

     case 4:  //Threshold - P2 Soil Moisture - V15
      arduinoData["tp2sm"] = value;
      break;

     case 5:  //Threshold - P2 Ph MIN - V8
      arduinoData["tp2phmin"] = value;
      break;

     case 6: //Threshold - P2 Ph MAX - V9
      arduinoData["tp2phmax"] = value;
      break;
  }

  //Writes the JSON to the string rawData
  serializeJson(arduinoData, rawData);
  
  //Sends the raw data to Arduino via Serial Communication
  Serial.print(rawData);
  Serial.println("\nData sent to Arduino.");
}

//========================= PLANT 1 COMMANDS =========================
BLYNK_WRITE(V5) { //Plant 1 Water Pump
  buttonState = param.asInt();
  
  if(buttonState == 1)
    sendDataToArduino(0, "1");  //Water Pump ON
  else
    sendDataToArduino(0, "2");  //Water Pump OFF
}

BLYNK_WRITE(V4) { //Plant 1 Fertilizer Pump
  buttonState = param.asInt();
  
  if(buttonState == 1)
    sendDataToArduino(0, "3");  //Fertilizer Pump ON
  else
    sendDataToArduino(0, "4");  //Fertilizer Pump OFF
}

BLYNK_WRITE(V13) { //Set Plant 1 Soil Moisture Threshold
  _tp1sm1 = param.asDouble(); //Write value to cache
  updateSettings();
  sendDataToArduino(1, (String)_tp1sm1);
}

BLYNK_WRITE(V2) { //Set Plant 1 Ph Threshold MIN
  _tp1phmin = param.asDouble(); //Write value to cache
  updateSettings();
  sendDataToArduino(2, (String)_tp1phmin);
}

BLYNK_WRITE(V3) { //Set Plant 1 Ph Threshold MAX
  _tp1phmax = param.asDouble(); //Write value to cache
  updateSettings();
  sendDataToArduino(3, (String)_tp1phmax);
}
//========================= END PLANT 1 COMMANDS ========================

//========================= PLANT 2 COMMANDS =========================
BLYNK_WRITE(V11) { //Plant 2 Water Pump
  buttonState = param.asInt();
  
  if(buttonState == 1)
    sendDataToArduino(0, "5"); //Water Pump ON
  else
    sendDataToArduino(0, "6"); //Water Pump OFF
}

BLYNK_WRITE(V10) { //Plant 2 Fertilizer Pump
  buttonState = param.asInt();
  
  if(buttonState == 1)
    sendDataToArduino(0, (String)7); //Fertilizer Pump ON
  else
    sendDataToArduino(0, (String)8); //Fertilizer Pump OFF
}

BLYNK_WRITE(V15) { //Set Plant 2 Soil Moisture Threshold
  _tp2sm2 = param.asDouble(); //Write value to cache
  updateSettings();
  sendDataToArduino(4, (String)_tp2sm2);
}

BLYNK_WRITE(V8) { //Set Plant 2 Ph Threshold MIN
  _tp2phmin = param.asDouble(); //Write value to cache
  updateSettings();
  sendDataToArduino(5, (String)_tp2phmin);
}

BLYNK_WRITE(V9) { //Set Plant 2 Ph Threshold MAX
  _tp2phmax = param.asDouble(); //Write value to cache
  updateSettings();
  sendDataToArduino(6, (String)_tp2phmax);
}
//========================= END PLANT 2 COMMANDS ========================

void sendRealtimeData() {
  /*
   * V0 - Realtime Data - Plant 1  - Ph
   * V1 - Realtime Data - Plant 1  - Soil Moisture
   * V2 - Threshold - Plant 1  - Ph MIN
   * V3 - Threshold - Plant 1  - Ph MAX
   * V13 - Threshold - Plant 1  - Soil Moisture
   * 
   * V6 - Realtime Data - Plant 2  - Ph
   * V7 - Realtime Data - Plant 2  - Soil Moisture
   * V8 - Threshold - Plant 2  - Ph MIN
   * V9 - Threshold - Plant 2  - Ph MAX
   * V15 - Threshold - Plant 2  - Soil Moisture
   */

  //Sends realtime data to app
  Blynk.virtualWrite(V0, _rdp1ph); //Plant 1 Ph Level Realtime Data
  Blynk.virtualWrite(V1, _rdp1sm); //Plant 1 Soil Moisture Realtime Data
  
  Blynk.virtualWrite(V6, _rdp2ph); //Plant 2 Ph Level Realtime Data
  Blynk.virtualWrite(V7, _rdp2sm); //Plant 2 Soil Moisture Realtime Data
}

void setup() {
  Serial.println("Initializing system...");
  
  runtime = millis();  
  Serial.begin(9600);
  pinMode(sdCS, OUTPUT);
  initSD();    //initialize sd card=buksan sd card
  
  Serial.println("\n------------------- CACHE -------------------"); //load to variable in nodemcu
  loadSettingstoCache();
  Serial.println("---------------------------------------------");
  
  Serial.println();

  Serial.println("Connecting to " + String(ssid) + "...");
  Blynk.begin(auth, ssid, pass);  //Initialize Blynk
  Serial.println("Connected to Blynk server.");

  Serial.print("Connected to " + (String)ssid + " after  ");
  int runSecs = (millis() - runtime) / 1000;
  int runMins = runSecs / 60;
  Serial.println(String(runMins) + " minutes and " + String(runSecs % 60) + " seconds.");
  
  //Sends Ph and Soil Moisture Value to the app every 1 second
  timer.setInterval(1000L, sendRealtimeData);

  Serial.println("\nSystem running.\n");
}

void loop() {
  Blynk.run();
  timer.run();
}

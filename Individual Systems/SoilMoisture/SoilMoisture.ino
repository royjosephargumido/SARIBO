const int dub = 970;
const int dlb = 600;
const int wub = 500;
const int wlb = 300;
const int readTimes = 60;
int finalSMV = 0;

const int smVCC = D1;
const int smAP = 0;

int smState = LOW;

int smpdFail = 0;
int smlcFail = 0;
int shmfFail = 0;

bool wateringOn = false;
int SMRprocess = 0;
bool stopCheck = false;


void readSM() {
  long int sumSMV = 0;
  int count = 0;

  if(smState == LOW) {
    smState = HIGH;
    digitalWrite(smVCC, smState);
  }
  delay(3000);

  Serial.println(F("***************************************"));
  
  if(smpdFail == 60 || smlcFail == 60 || shmfFail == 60) {
    Serial.println(F("\n***************************************"));
    
    String err = "";
    if(smpdFail == 60)
      err = "Soil Moisture Probe Disconnection Failure";
    else if(smlcFail == 60)
      err = "Soil Moisture Lost Confidence Failure";
    else
      err = "Soil Hygrometer Module Failure";
      
    Serial.println("Reboot cause: ");
    Serial.println(err);
    Serial.println(F("Restarting Leaf Module."));
    Serial.println(F("***************************************"));
    ESP.restart();
    
  }else {
    
    ++SMRprocess;
    Serial.printf("Process #%d", SMRprocess);
    Serial.println(F("\n***************************************"));
        
    while(count < readTimes) {
      int smv = analogRead(smAP);

      if(smv > dub || smv < wlb) {
        /*
         * Inaccurate soil moisture readings
         */
        Serial.println(F("\nInaccurate reading!"));
        Serial.println(F("==============================="));
        
        if(smv == 1024) {
          Serial.println(F("Soil Moisture Probe Disconnected!"));
          ++smpdFail;
        }else if(smv < 1024 && smv > dub) {
          Serial.println(F("Soil Moisture Lost Confidence Failure!"));
          ++smlcFail;
        }else {
          Serial.println(F("Soil Hygrometer Module Failure!"));
          ++shmfFail;
        }
        Serial.printf("Value: %d\nRepeating soil moisture check.\n", smv);
        Serial.println(F("==============================="));
        Serial.println(F("***************************************\n"));
        
        smState = LOW;
        readSM();
        break;
        
      }else {
        sumSMV += smv;
        ++count;
        Serial.printf("Reading #%d: %d\n", count, smv);
        delay(1000);
      }
    }
  }

  if(count == readTimes) {
    finalSMV = sumSMV / readTimes;
    Serial.println(F("***************************************"));
    Serial.printf("Soil moisture: %d\n", finalSMV);
    smState = LOW;
    digitalWrite(smVCC, smState);
  }
}

void processSMV() {
  if(finalSMV <= dub && finalSMV > dlb) {
    /*
     * SOIL IS DRY
     */
     Serial.println(F("The soil is dry."));
     Serial.println(F("Starting watering process."));
     wateringOn = true;
     delay(5000);
     processSMV();
  }else if(finalSMV <= dlb && finalSMV > wub) {
    /*
     * SOIL IS IN IDEAL MOISTURE
     */
     Serial.println(F("The soil is in the ideal soil moisture content."));
     stopCheck = false;
  }else if(finalSMV <= wub && finalSMV > wlb) {
    /*
     * SOIL IS WET
     */
    Serial.println(F("The soil is wet."));
    stopCheck = false;
  }
  Serial.println(F("***************************************"));
}

void setup() {
  pinMode(smVCC, OUTPUT);
  digitalWrite(smVCC, smState);
  
  Serial.begin(115200);
  delay(3000);

  Serial.println(F("\n"));
  readSM();
  processSMV();
}

void loop() {
}

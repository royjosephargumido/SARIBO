/*
    Hydra: Water Level Checking Process
    
    The Water Level Checking Process is the main process responsible in
    checking the water level in the tank. It requires the following values:
    minWL as Minimum Water Level in percentage value

    Copyright © 2020 by Argumido, Roy Joseph
    Version 1.0.1 rev. February 12, 2020 17:28
 */
#include <HCSR04.h>

HCSR04 hc(5,6);//initialization of class HCSR04(trig pin , echo pin)

//Loads the required values from sysdef.txt
const int bufferv = 3;  //gap between the sensor tip and the max water level
const float factor = 0.12;  //distance between the tank bottom to the sensor tip divided by 100
long dist = 0;

void setup()
{    
  //BAUD RATE SERIAL COMMUNICATION
  Serial.begin(9600);
}

void loop()
{
  dist = (hc.dist() - bufferv) / factor;
  Serial.print("Water Level: ");
  Serial.print(100 - dist);
  Serial.println("%");
  delay(2000);
}

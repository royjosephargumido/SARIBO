Soil Moisture, and Reading Standards v. 1.0 rev Mar. 16, 2020
-----------------------------------------------------------

Pin configuration
-----------------------------------------------------------
The Soil Moisture sensor (MHI sensor series) uses the following connection to Arduino

| Soil Moisture  | Arduino Pin |
| ------------- | ------------- |
| VCC+ | 5V VCC+ |
| GND | Gnd |
| A0 | A0 |


Soil Moisture Reading Algorithm
-----------------------------------------------------------
**1.** Read the soil moisture value (AnalogRead(pin)) once every second (60 counts) for one minute for accuracy and save it in a *long int* data type.

**2.** After 60 counts, compute the final soil moisture value and divide it by 60.

**3.** Load user soil moisture level configuration such as the *maxsoildryness*, and the *minsoildryness* from SD card (/core/SYSCONFIG.txt)

**4.** When the final soil moisture is between the *maxsoildryness* and the *minsoildryness* range (means the soil is dry), trigger the *OPEN distribution request*, ***go to line 5***, else, trigger the *CLOSE distribution request*.

**5.** After 4 minutes from the *OPEN distribution request*, ***repeat steps 1-4***

***Note:*** Always create a high verbosity log.



Source Code
-----------------------------------------------------------

/*
 * Source code for Soil Moisture Checking
 * SARIBO v. 1.2.2 Copyright (c) 2020, Roy Joseph Argumido (rjargumido@outlook.com)
 */
int count = 1;              // Used in calculating the number of soil moisture check
bool stopCheck = false;     // Used as a counter whether to continue or stop the soil moisture reading
unsigned long oldTime;      // Used in calculating the  seconds passed since the initial time reading 
long int soilmoisture = 0;  // The soil moisture value is saved as a long data type to cater higher precission

void checkSoilMoisture()
{
  int finalSoilMoisture = 0;
  
  if(stopCheck == false)
  {
    /* 
     * reads and compute the average soil moisture sensor
     * once every second for 1 minute for better accuracy
     */
    if(((millis() - oldTime) > 1000) && count < 11)
    {
      soilmoisture += analogRead(A0);   // Reads the soil moisture and adds the old readings

      /*
       * Displays the data for verification
       */
      Serial.print("Reading #");
      Serial.println(count);
      Serial.print("Soil moisture: ");
      Serial.println(soilmoisture);
    
      oldTime = millis();   // Saves the the number of milli seconds past since the reading
      count = count + 1;    // Used in the reading for 1 minute
    }

    // Stops soil moisture reading until 1 minute (60 seconds)
    if(count == 10)
    {
      oldTime = 0;       // Resets the millisecond timer
      count = 0;        // Resets the number of seconds counter
      stopCheck = true; // Stops the reading of the soil moisture
      finalSoilMoisture = soilmoisture / 10;  // Calculates the average soil moisture value
      
      Serial.print("Final Soil moisture: ");
      Serial.println(finalSoilMoisture);

      //load the configurations from /System/SYSDEF.TXT
      int maxsoildryness = 1001;    //Maximum soil dryness value
      int minsoildryness = 500;     //Minimum soil dryness value

      if(finalSoilMoisture < maxsoildryness && finalSoilMoisture > minsoildryness)
      {
        /* 
         * Soil is DRY based from the range of the Soil Dryness set by the user
         * then creates an open distribution valve request to the Root module
         * sending the final soil moisture value as a validating value for double checking.
         * 
         * 101 = code for OPEN distribution valve request as specified in the data request table
         */
        createRequestTable(11, finalSoilMoisture);
        Serial.print("Open Leaf01 distribution valve request sent.");
      }
      else if(finalSoilMoisture < minsoildryness)
      {
        /* 
         * Soil is WET when the final soil moisture value is lesser than the
         * minimum soil dryness threshold value then creates a close distribution 
         * valve request to the Root module and attaches the final soil moisture
         * value as a validating value for double checking.
         * 
         * 102 = code for CLOSE distribution valve request as specified in the data request table
         */
        createRequestTable(12, finalSoilMoisture);
        Serial.print("\n\nClose Leaf01 distribution valve request sent.");
      }
    }
  }
}



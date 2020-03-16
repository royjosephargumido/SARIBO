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

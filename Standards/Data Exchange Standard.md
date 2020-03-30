The Data Exchange Standard (DES) is used as the core data exchange, transfer and processing rules used to ensure that the data is being processed the same way throughout the system. It is further divided into two:

1. **Data Exchange Table** Is a table that is being used to consolidate relevant real-time data to be used in the exchange between the Leaf and the Root modules or vice versa.
2. **Requests** Are 2-digit integer code used for determining what type of request is being sent or data to be send.

***Requests***
The following are the request codes under Request Code Table v2.1 revision March 31, 2020:

| Request Code  | Description |
| ------------- | ------------- |
| 10 | Soil moisture reading only |
| 11 | Leaf Distribution Line, Open |
| 12 | Leaf Distribution Line, Close |
| 20 | Get Leaf Water Flow Rate Reading |
| 21 | Read current Leaf Water Flow Rate |
| 30 | Power Reading Only |
| 40 | Date and Time reading |
| 41 | Synchronize with Root Date and Time settings |
| 50 | Network Reading |
| 51 | Ping Request |
| 61 | Pull Root Settings |
| 62 | Pull Leaf Settings |

***Data Exchange Table***
The following is the table structure used under Data Exchange Table v2.2 revision March 31, 2020:

| Data  | Description |
| ------------- | ------------- |
| origin* | The Hardware ID of the requesting module |
| destination* | The Hardware ID of the destination module of the request |
| datesent** | The current date of the requesting module |
| timesent*** | The current time of the requesting module |
| request****  | The request code |
| value  | The value being exchange as a validation/required data |


Note:
* Complies with the Hardware Id nomenclature Algorithm found at the Root Hardware ID Management Service (HIMS).
** The Date object is in the format: MMMM dd, YYYY e.g. April 1, 2020 without the zero padding in the day object.
*** The Time object is in the format: hh:mm:ss SS e.g. 7:02:09 AM with the zero padding on both minute and second objects only.
**** Request codes complies with the recent Request Code Table under the Data Exchange Standard.


Example:

origin : HC7E9701
destination : RBF0928J
datesent : April 1, 2020
timesent : 7:02:09 AM
request : 11
value : 892

The above Data Exchange Table returns a string value of:
*{"origin":"HC7E9701","destination":"RBF0928J","datesent":"April+1,+2020","timesent":7:02:09+AM","request":11,"value":892"}*

The following followwing String value complies with the ArduinoJson object [*see: <ArduinoJson.h> ArduinoJson Library version 6.14.1 by Benoit Blanchon*](https://github.com/bblanchon/ArduinoJson) and the HTTP URL/URI standards.

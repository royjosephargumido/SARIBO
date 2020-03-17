SARIBO REPORTING SERVICE
Version 2.2, Revision March 17, 2020
**************************************************
Power On Self Test Log
/logs/POST/POST_Mar-1-2020.txt

SARIBO L1

March 1, 2020
6:58:56
**************************************************
Starting Power On Self Test...

6:58:58

Checking network configurations...
Server SSID: Hydra Server - NwSSU
Server IP Address: 192.168.11.4
Port: 80
Pinging 192.168.11.4...
Leaf01: Ping request sent to server on 1 March, 2020 7:00:1
Server: Server responded at 204 ms.
Ping ok. Server available.

7:00:10

Synchronizing date and time with Server...
Leaf01: DateTime request sent to server on 1 March, 2020 7:01:4
Server Date: March 1, 2020
Server Time: 7:01:28
Local Date: March 1, 2020
Local Time: 7:01:28
Date and time synchronized.

7:01:30

Checking power status...
Voltage: 3.37
Capacity: 58%
Leaf01: Power status report sent to server on 1 March, 2020 7:01:52
Server: Leaf01 power status recorded.
Power status ok.

7:02:14

Performing soil moisture check...
Activating soil moisture sensor...
Soil moisture sensor activated.
Performing soil moisture reading...
***
Soil Moisture Reading #1: 478
Soil Moisture Reading #2: 477
Soil Moisture Reading #3: 478
...
Soil Moisture Reading #58: 468
Soil Moisture Reading #59: 470
Soil Moisture Reading #60: 474
***
Average soil moisture: 474
Soil is in ideal soil moisture level.
Leaf01: Soil moisture report sent to server on 1 March, 2020 7:03:17
Server: Leaf01 soil moisture status recorded.
Soil moisture reading ok.

7:03:17

Performing watering...
Running watering confirmation check...
Leaf01: Watering request sent to server on 1 March, 2020 7:03:22
Server: Leaf01 open distribution request approved. Opening valve in 5 seconds.
Performing water flow rate reading...
***
Water flow Reading #1: 1.77 ml./sec.
Water flow Reading #2: 1.77 ml./sec.
Water flow Reading #3: 1.77 ml./sec.
Water flow Reading #4: 1.77 ml./sec.
Water flow Reading #5: 1.75 ml./sec.
***
Average water flow rate: 1.76 ml./sec.
Water verified at the last sprinkler.
Leaf01: Watering report sent to server on 1 March, 2020 7:04:12
Server: Leaf01 watering report recorded. Leaf01 distribution valve closed.
Leaf01 water valve ok.
Water flow reading ok.
Water sensor ok.

7:05:28
Network configuration: Good
Server: Available
DateTime: Synchronized
Power: 58%
Average soil moisture: 474
Soil Moisture reading: Ok
Distribution valve: Ok
Average water flow rate: 1.76 ml./sec.
Water flow reading: OK

Network: Active
Server Reporting Service: Active

End of Power On Self Test reporting.
**************************************************
Leaf01: POST log sent to server. 1 March, 2020 7:06:7
Server: Leaf01 POST log successfully recorded. Signed SARIBO ROOT - NwSSU @ 192.168.11.4 on March 1, 2020 7:06:36

{803C8A5D-CA6B-41EA-B572-76C9C86AA322}

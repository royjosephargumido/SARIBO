Data Request Standards v. 2.0 rev Mar. 16, 2020
-----------------------------------------------------------
The following are the SARIBO standards used in parsing and deserializing a request table.


Request Standards Table (2 character format)
-----------------------------------------------------------

10	Distribution Line
11	Open Distribution Line
12	Close Distribution Line

20	Soil Moisture Reading

30	Water Flow Reading

40	Power Reading

50	Time Reading
51	Perform Date and Time Sync between Leaf and Root

60	Network Reading
61	Ping Request

70	Settings
71	Synchronize Settings in the network

80	SARIBO General Channel

90	Log, High Verbosity

Data Table
-----------------------------------------------------------
o		//Origin or where the data come from
r		//Request see Request Table
v		//Validation or return value e.g. soil moisture value


Hardware ID naming and registration:
-----------------------------------------------------------
Format: MN

Where:

M = Module type:

R = Root module
L = Leaf module


N = module number


E.g.:

L1 means Leaf Module 1


Data Process Table
-----------------------------------------------------------
Process: Leaf01 to Root Open Distribution Line
Returns:

o:	L1
r:	11
v:	887

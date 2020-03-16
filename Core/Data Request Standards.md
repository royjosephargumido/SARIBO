Data Request Standards v. 2.0 rev Mar. 16, 2020
-----------------------------------------------------------
The following are the SARIBO standards used in parsing and deserializing a request table.


Request Standards Table (2 character format)
-----------------------------------------------------------
| Code  | Description |
| ------------- | ------------- |
| 10 | Distribution Line - General Request |
| 11 | Open Distribution Line |
| 20 | Soil Moisture Reading |
| 30 | Water Flow Reading |
| 40 | Power Reading |
| 50 | Time Reading |
| 51 | Perform Date and Time Sync between Leaf and Root |
| 60 | Network Reading |
| 61 | Ping Request |
| 70 | Settings |
| 71 | Synchronize Settings in the network |
| 80 | SARIBO General Channel |
| 90 | Log, High Verbosity |

Data Table
-----------------------------------------------------------
| Code  | Description |
| ------------- | ------------- |
| o | Origin or where the data come from |
| r | Request see Request Standards Table |
| v | Validation or return value e.g. soil moisture value |


Hardware ID naming and registration:
-----------------------------------------------------------
**Format: <module_type><module_number>**

*Where module type:*

| Code  | Description |
| ------------- | ------------- |
| R | Root Module, default is R0 |
| L | Leaf Module, starts in L1...L*n* |

Example:

L1 means Leaf Module 1


Sample Data Request
-----------------------------------------------------------

Process: Leaf01 to Root Open Distribution Line
Returns:

o: L1
r: 11
v: 887

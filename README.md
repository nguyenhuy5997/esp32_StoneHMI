| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP32StoneHMI
This project demonstrate how to use Stone HMI communicate with ESP32 to control Relay and update the history of action into Stone HMI

## How to use

### Hardware Required
1. Board ESP32
2. Stone HMI 
3. Relay 3.3V or 5V

### Setup the Hardware

Connect the ESP32, Stone HMI and Relay as follows.

```
  -----------------------------------------------------------------------------------------
  | Relay 2               | Relay 1            | ESP Pin              | Stone HMI         |
  | ----------------------|--------------------|----------------------|--------------------
  |                       |                    | GPIO4                | RxD positive (RXp)|
  |                       |                    | GPIO5                | TxD positive (TXp)|
  | Data in               |                    | GPIO21               |                   |
  |                       |Data in             | GPIO22               |                   |
  | VCC (D+)              |VCC (D+)            |                      |                   |
  | GND (D-)              |GND (D-)            | GND                  |                   |
  -----------------------------------------------------------------------------------------
```
Note: Stone HMI support TTL and RS232 communication
By default, Stone HMI uses RS232, to switch to TTL, user change the resistor 0(Omh) from RS232 to TTL
```

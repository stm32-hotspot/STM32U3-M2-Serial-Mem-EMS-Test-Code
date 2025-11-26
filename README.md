# STM32U3 Naquada M.2 Memory Connector Application

Naquada application demonstrating the deployment of M.2 memory connector application.

---

## Features Demonstrated in This Example

- M.2 memory connector with naquada board on STM32U3

---

## Build application

### Build application for development

- open Project.eww in Naquada/EWARM with IAR EWARM v9.60.3
- click on build icon

---

## Execute application

### When application is built for development

- plug a memory board in the M.2 connector
- connect USBC cable to the naquada carrier board
- connect STLINK V3 mini to the carrier board 
- connect USBC cable to the STLINK mini
- use and configure tera term (115200, 8 bit, no parity 1 stop bit, no flow control)
- use the load button to flash the STM32U3 with the application.
- reset or use the go button to launch the application

You should see the following output on the console

```

M.2 MEMORY BOARD TEST INFO  : start of the test, do not unplug

M.2 MEMORY BOARD TEST INFO  : EEPROM IDs detected

M.2 MEMORY BOARD TEST INFO  : EEPROM CPN : MB1928-18LA-C01

M.2 MEMORY BOARD TEST INFO  : Flash memory test is PASSED

M.2 MEMORY BOARD TEST INFO  : Safe to unplug the M.2 memory board
```

---

## Feedback and contributions

Please refer to the [CONTRIBUTING.md](CONTRIBUTING.md) guide.

/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

// This describes Green Naquada board elements when the naquada sensor board has been cut off

#ifndef _MB2095B_MCU_H_
#define _MB2095B_MCU_H_

#include "MB2095B_NET.h"

typedef enum { // Brisk LEDs example (static)
  NO_LED = 0,
  LED_1, // missing
  LED_2, // missing
  LED_3, // missing
  LED_4, // missing
  LED_5, // missing
  LED_6, // Naquada MCU
  LED_C1, // Carrier LED1
  LED_C2, // Carrier LED2
  LED_C3, // Carrier LED3
  
} LED_Names_t;

extern uint8_t LED_COUNT;

extern I2C_MasterIO_t gI2C_Bus_Sensor; // Same I2C bus for both DIP28 and STMod+ PA11=SCL2, PA12=SDA2
extern const IO_Pad_t MIO_SDA_Sensor;
extern const IO_Pad_t MIO_SCL_Sensor;

extern I2C_MasterIO_t gI2C_Bus_DigitalHeader; // Same I2C bus for both DIP28 and STMod+ PA11=SCL2, PA12=SDA2
extern const IO_Pad_t MIO_SDA_DigitalHeader;
extern const IO_Pad_t MIO_SCL_DigitalHEader;


extern I2C_MasterIO_t gI2C_Bus_M2SerialMem; 
extern const IO_Pad_t MIO_SDA_M2SerialMem;
extern const IO_Pad_t MIO_SCL_M2SerialMem;


extern const IO_Pad_t UserKeyPin_ANALOG;
extern const IO_Pad_t UserKeyPin_EXTI;

extern IO_Pad_t BT_TX_Pad;
extern IO_Pad_t BT_RX_Pad;

extern IO_Pad_t ST_TX_Pad;
extern IO_Pad_t ST_RX_Pad;

extern IO_Pad_t MM_UART_TX1_Pad;
extern IO_Pad_t MM_UART_RX1_Pad;

#define MM_UART_TX2_Pad BT_TX_Pad // use same MCU pin
#define MM_UART_RX2_Pad BT_RX_Pad // use same MCU pin

extern I2C_SlaveDevice_t gM2SM_M24256D;
extern I2C_SlaveDevice_t gM2SM_M24256D_ID;
extern EEP_t M2SM_M24256D;


#endif
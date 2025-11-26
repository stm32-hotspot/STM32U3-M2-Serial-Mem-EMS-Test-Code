/**
  ******************************************************************************
  * @file           : BoardSet.h
  * @brief          : Header for BoardSet.c file.
  *                   This define which board(s) are managed in this SW, due to board modularity 
  *                   (cuttable, connectors)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef _BOARDSET_H_
#define _BOARDSET_H_

#include "main.h"

// Red Naquada (itteration 1)
//#define MB2095_MCU // Sensor part cut off
//#define MB2095_SENSORS // Uncut

// Green Naquada (itteration 2)
#define MB2095B_MCU // Sensor part cut off with carrier board assumed to be plugged in (IO configured for it)
//#define MB2095B_SENSORS // Uncut

//#define MB2222 // Carrier board support

//=================================================
#ifdef MB2095_MCU
#include "MB2095_MCU.h"
#endif
#ifdef MB2095_SENSORS
#include "MB2095_SENSORS.h"
#endif

#ifdef MB2095B_MCU
#include "MB2095B_MCU.h"
#endif
#ifdef MB2095B_SENSORS
#include "MB2095B_SENSORS.h"
#endif

int32_t Board_IO_Init(void); // defined in the corresponding board MBxxx file 
uint8_t Board_Signal_Init(void); // defined in the corresponding board MBxxx file

int32_t Board_Init(void); // top level API for main.c
uint8_t Board_IsPlugged(void);
uint8_t Board_Run(void);
extern uint8_t gSlaveSweep; // To radar sweep I2C slave addresses in debug mode and find out which slave ACK on the bus


void BSP_I2C_MasterIO_Init(void); 
void BSP_I2C_MasterIO_Test(void);
extern I2C_MasterIO_t gI2C_Bus_Sensor;
extern I2C_MasterIO_t gI2C_Bus_DigitalHeader;
//extern I2C_MasterIO_t gI2C_Arduino;

#endif
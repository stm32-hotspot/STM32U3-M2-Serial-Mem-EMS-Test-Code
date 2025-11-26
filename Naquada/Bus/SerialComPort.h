/**
  ******************************************************************************
  * @file    sif.h (Serial InterFace)
  * @author  S.Ma
  * @brief   ...
  * 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018-2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef _SERIALCOMPORT_H_
#define _SERIALCOMPORT_H_

#include "stm32u3xx_ll_usart.h"





// import from SPIP_SIF references
typedef enum {
  BT_SERIAL, // bluetooth HC-05 or 4 pin serial connector
  ST_SERIAL, // STLink serial interface
  MM_SERIAL, // Micromod serial interface using LPUART
  MAX_SERIAL, // 2
} XXX_Serial_t;

extern u8fifo_t u8fifo_to_TX[MAX_SERIAL], u8fifo_from_RX[MAX_SERIAL]; // These FIFO are re-routable to any peripheral. 4 TX FIFO channels, 4 RX FIFO channels

uint32_t u8fifo_to_USARTn_NoLongerEmpty(uint32_t ct);
uint32_t u8fifo_to_USARTn_Emptied(uint32_t ct);
void Serials_Init(void);

#endif

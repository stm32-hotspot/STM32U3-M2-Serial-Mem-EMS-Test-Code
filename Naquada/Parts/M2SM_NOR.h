/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

#ifndef __M2SM_NOR_H
#define __M2SM_NOR_H

#include "stm32u3xx_hal.h"
#include "STM32U3LQFP100_SMPS.h"
#include "io_drive.h"
#include "SerialMem.h"
#include "commons.h"

#include "M24256D.h"

#include "M2SM_W25Q16JV.h"
#include "M2SM_MX25LM51245G.h"
#include "M2SM_MX25UM51245G.h"
#include "M2SM_M95P32.h"
#include "M2SM_IS25LP032.h"


/* Memory index --> Memory Type */
/*
0 --> W25Q16JV --> MB1928-33LB-C01
1 --> MX25LM51245G --> MB1927-33BA-C01
2 --> MX25UM51245G--> MB1927-18BA-C01
3 --> ST 1.8V M95P32 --> MB1928-18LA-C01
4 --> IS 3.3V IS25LP032 --> MB1928-33LA-C01
*/

//#define NUMBER_OF_MEMORY_SUPPORTED sizeof(tSerialMemGlobalInfo/tSerialMemGlobalInfo[0]);

typedef enum {
  W25Q16JV, /* Winbond 3.3V */
  MX25LM51245G, /* macronix 3.3V */
  MX25UM51245G, /* macronix 1.8V */
  M95P32, /* ST 1.8V */
  IS25LP032  /* IS 3.3V */
} MemoryName_t;

extern SerialMemGlobalInfo_t WB_SerialMemGlobalInfo;
extern SerialMemGlobalInfo_t MX_33_SerialMemGlobalInfo;
extern SerialMemGlobalInfo_t MX_18_SerialMemGlobalInfo;
extern SerialMemGlobalInfo_t ST_SerialMemGlobalInfo;
extern SerialMemGlobalInfo_t IS_SerialMemGlobalInfo;

SerialMemGlobalInfo_t *tSerialMemGlobalInfo[]={&WB_SerialMemGlobalInfo, &MX_33_SerialMemGlobalInfo, &MX_18_SerialMemGlobalInfo, &ST_SerialMemGlobalInfo, &IS_SerialMemGlobalInfo};
#define NUMBER_OF_MEMORY_SUPPORTED sizeof(tSerialMemGlobalInfo)/sizeof(*tSerialMemGlobalInfo)

uint8_t WB_NOR_Test(SerialMemGlobalInfo_t *Smgi);
uint8_t MX_33_NOR_Test(SerialMemGlobalInfo_t *Smgi);
uint8_t MX_18_NOR_Test(SerialMemGlobalInfo_t *Smgi);
uint8_t ST_NOR_Test(SerialMemGlobalInfo_t *Smgi);
uint8_t IS_NOR_Test(SerialMemGlobalInfo_t *Smgi);

uint8_t (*M2SM_NOR_Test[])(SerialMemGlobalInfo_t *Smgi)={WB_NOR_Test, MX_33_NOR_Test, MX_18_NOR_Test, ST_NOR_Test, IS_NOR_Test};


void (*MSM_NOR_ReadInfo[])(M2SerMem_t * pM2SM)={WB_NOR_ReadInfo, MX_33_NOR_ReadInfo, MX_18_NOR_ReadInfo, ST_NOR_ReadInfo, IS_NOR_ReadInfo};


#endif /* __M2SM_NOR_H */
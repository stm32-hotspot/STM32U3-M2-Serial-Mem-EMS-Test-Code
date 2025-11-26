/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#ifndef _M2SM_EEP_PNP_H_
#define _M2SM_EEP_PNP_H_

#include "main.h"
#include "SerialMem.h"
#include "M24256D.h"


// In order to facilitate user experience, all M2 memory add-on board comes with preprogrammed memory information.
// This information is placed at the end of the EEPROM memory segment so the user can still use the first memory zone for anything, storing user data/configuration etc...

// To make things flexible enough, we suppose the memory is M24128 = 128 kbit = 32 kbytes.
// 1 kbyte to describe the board itself
// up to 4x1 kbyte for each memory description
// 1 kbyte for the SPI interface
// 1 kbyte for the I2C interface
// 1 kbyte for the rest (for future use). 32-8=24 kbyte are free for user
// 0x6000-0x7FFF = 8 kbyte


//typedef enum {
//
//M2SM_INFO_BASE = 0x6000, // 0x6000 Board information
//M2SM_MEMA_BASE = 0x6400, // 0x6400 Memory A information (data 0..3+)
//M2SM_MEMB_BASE = 0x6800, // 0x6800 Memory B infromation (data 4..7+)
//M2SM_MEMC_BASE = 0x6C00, // 0x6A00 Memory C information (data 8..11+)
//M2SM_MEMD_BASE = 0x7000, // 0x7000 Memory D information (data 12..15)
//M2SM_SPI_BASE = 0x7400,  // 0x7400 SPI devices information
//M2SM_I2C_BASE = 0x7800,  // 0x7800 I2C devices information
//M2SM_RESERVED_BASE = 0x7C00, // 0x7C00 Reserved
//
//} EEP_Info_Bases_t;

typedef enum {
  
M2SM_TESTINFO_BASE = 0x0000, // 0x0000 Test Information (number and status) base
M2SM_INFO_BASE = 0x2000, // 0x2000 Board information
M2SM_MEMA_BASE = 0x2400, // 0x2400 Memory A information (data 0..3+)
M2SM_MEMB_BASE = 0x2800, // 0x2800 Memory B infromation (data 4..7+)
M2SM_MEMC_BASE = 0x2C00, // 0x2A00 Memory C information (data 8..11+)
M2SM_MEMD_BASE = 0x3000, // 0x3000 Memory D information (data 12..15)
M2SM_SPI_BASE = 0x3400,  // 0x3400 SPI devices information
M2SM_I2C_BASE = 0x3800,  // 0x3800 I2C devices information
M2SM_RESERVED_BASE = 0x3C00, // 0x7C00 Reserved

} EEP_Info_Bases_t;

typedef enum {
  TEST_FAILED,
  TEST_PASSED
} TEST_STATUS_t;

// Description of the structure info of the board itself

typedef struct {
  
  uint8_t VerRev[2];
  char  CPN[16]; // board order code
  char  FG[16]; // board version code
  uint16_t Vmin_Vin_x100; // incoming power supply acceptable voltage
  uint16_t Vmax_Vin_x100;
  uint16_t Vout_x100; // Vout value if 0 = no regulator, vout = vin

  uint8_t PopulatedMemoryCount; // 0..4 max from D0+ this tells how many valid xspi memory slots
  uint8_t MaxMemoryCount; // 0..4 max how many xspi memories can be populated
  uint8_t JEDEC_MemA[4];
  uint8_t JEDEC_MemB[4];

  uint8_t JEDEC_MemC[4];
  uint8_t JEDEC_MemD[4];
  uint8_t Clock_MHz_Min;
  uint8_t Clock_MHz_Max; // 56 bytes used
  
  uint32_t Reserved1;
  uint32_t Reserved2;

  // we will fill later the rest
} M2SM_ID_INFO_t;

typedef struct {
  uint8_t Detected;
  uint8_t Detected_Id;
  uint8_t Empty;
  uint8_t M2SM_Index;
  uint8_t M2SM_Max;
} EEP_INFO_t;

uint8_t M2SM_IsBoardDetected(I2C_SlaveDevice_t* pD);
extern uint8_t EEP_Data1kbyte[1024];
int8_t M2SM_EEP_Read1kbyte(EEP_t* pEEP, EEP_Info_Bases_t Adr);
int8_t M2SM_EEP_Write1kbyte(EEP_t* pEEP, EEP_Info_Bases_t Adr);
int8_t M2SM_EEP_Erase1kbyte(EEP_t* pEEP, EEP_Info_Bases_t Adr);

void M2SM_EEP_PNP_init(EEP_t* pEEP);
EEP_INFO_t M2SM_EEP_PNP_polling(EEP_t* pEEP);
uint8_t M2SM_EEP_PNP_filling(EEP_t* pEEP, uint8_t MemoryType);
void M2SM_EEP_PNP_saving(EEP_t* pEEP, uint8_t TestResult);


void M2SM_LED_UpdateStatus(uint8_t LED_RedOn, uint8_t LED_GreenOn);
#endif
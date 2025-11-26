/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

#ifndef __M2SM_EEP_INFO
#define __M2SM_EEP_INFO


#include "M2SM_EEP_PNP.h"

/*
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
*/

/* 15 character_long string for the board order code*/
//  0123456701234567 [16 char max]
// "B-M2MEM-PACK1";
// "MB1927-33BA-C01"; MX25LM51245G // board order code MX25 3V3 BGA 1 populated
// "MB1927-18BA-C01"; // board order code MX25 1V8 BGA 1 populated
// "MB1928-33LA-C01";
// "MB1928-33LB-C01"; // board order code WB 3V3 SO 1 populated
// "MB1928-18LA-C01";  M95P32 1V8 // board order code M95P32 SO8 1 populated
#define CPN_WB_3V3_STRING "MB1928-33LB-C01"
#define CPN_MX_3V3_STRING "MB1927-33BA-C01"
#define CPN_MX_1V8_STRING "MB1927-18BA-C01"
#define CPN_ST_1V8_STRING "MB1928-18LA-C01"
#define CPN_IS_3V3_STRING "MB1928-33LA-C01"

M2SM_ID_INFO_t M2SM_INFO_W25Q16JV =  {
  
  .VerRev = { 0x01, 0x00 },
  
  .CPN = CPN_WB_3V3_STRING, // board order code WB 3V3 SO 1 populated
  .FG =  "V0.1", // board version code
  .Vmin_Vin_x100 = 270, // incoming power supply acceptable voltage
  .Vmax_Vin_x100 = 360,
  .PopulatedMemoryCount = 1, // 0..4 max from D0+ this tells how many valid xspi memory slots
  .MaxMemoryCount = 2, // 0..4 max how many xspi memories can be populated
  .JEDEC_MemA =   {0xEF, 0x70, 0x15, 0x00 }, // this correspond to D0..D3 The last byte meaning is TBD
  .JEDEC_MemB = { 0, 0, 0, 0 }, // this correspond to D4..D7
  .JEDEC_MemC = { 0, 0, 0, 0 }, // this correspond to D8..D11
  .JEDEC_MemD = { 0, 0, 0, 0 }, // this correspond to D12..D15
  .Clock_MHz_Min = 25,
  .Clock_MHz_Max = 133,
  .Reserved1 = 0,
  .Reserved2 = 0,
  // we will fill later the rest
} ;

M2SM_ID_INFO_t M2SM_INFO_MX25LM51245G =  {
  
  .VerRev = { 0x01, 0x00 },
  
  .CPN = CPN_MX_3V3_STRING, // board order code MX25 1V8 BGA 1 populated
  .FG =  "V0.1", // board version code
  .Vmin_Vin_x100 = 270, // incoming power supply acceptable voltage
  .Vmax_Vin_x100 = 360,
  .PopulatedMemoryCount = 1, // 0..4 max from D0+ this tells how many valid xspi memory slots
  .MaxMemoryCount = 2, // 0..4 max how many xspi memories can be populated
  .JEDEC_MemA =  {0xC2, 0x85, 0x3A, 0x00 }, // this correspond to D0..D3 The last byte meaning is TBD
  .JEDEC_MemB = { 0, 0, 0, 0 }, // this correspond to D4..D7
  .JEDEC_MemC = { 0, 0, 0, 0 }, // this correspond to D8..D11
  .JEDEC_MemD = { 0, 0, 0, 0 }, // this correspond to D12..D15
  .Clock_MHz_Min = 25,
  .Clock_MHz_Max = 133,
  .Reserved1 = 0,
  .Reserved2 = 0,
  // we will fill later the rest
} ;

M2SM_ID_INFO_t M2SM_INFO_MX25UM51245G =  {
  
  .VerRev = { 0x01, 0x00 },
  
  .CPN = CPN_MX_1V8_STRING, // board order code MX25 3V3 BGA 1 populated
  .FG =  "V0.1", // board version code
  .Vmin_Vin_x100 = 270, // incoming power supply acceptable voltage
  .Vmax_Vin_x100 = 360,
  .PopulatedMemoryCount = 1, // 0..4 max from D0+ this tells how many valid xspi memory slots
  .MaxMemoryCount = 2, // 0..4 max how many xspi memories can be populated
  .JEDEC_MemA =  {0xC2, 0x85, 0x3A, 0x00 }, // this correspond to D0..D3 The last byte meaning is TBD
  .JEDEC_MemB = { 0, 0, 0, 0 }, // this correspond to D4..D7
  .JEDEC_MemC = { 0, 0, 0, 0 }, // this correspond to D8..D11
  .JEDEC_MemD = { 0, 0, 0, 0 }, // this correspond to D12..D15
  .Clock_MHz_Min = 25,
  .Clock_MHz_Max = 133,
  .Reserved1 = 0,
  .Reserved2 = 0,
  // we will fill later the rest
} ;

M2SM_ID_INFO_t M2SM_INFO_M95P32 =  {
  
  .VerRev = { 0x01, 0x00 },
  
  .CPN = CPN_ST_1V8_STRING, // board order code ST 1V8 1 populated
  .FG =  "V0.1", // board version code
  .Vmin_Vin_x100 = 270, // incoming power supply acceptable voltage
  .Vmax_Vin_x100 = 360,
  .PopulatedMemoryCount = 1, // 0..4 max from D0+ this tells how many valid xspi memory slots
  .MaxMemoryCount = 2, // 0..4 max how many xspi memories can be populated
  .JEDEC_MemA =  {0x00, 0x00, 0x00, 0x00 }, // this correspond to D0..D3 The last byte meaning is TBD
  .JEDEC_MemB = { 0, 0, 0, 0 }, // this correspond to D4..D7
  .JEDEC_MemC = { 0, 0, 0, 0 }, // this correspond to D8..D11
  .JEDEC_MemD = { 0, 0, 0, 0 }, // this correspond to D12..D15
  .Clock_MHz_Min = 25,
  .Clock_MHz_Max = 133,
  .Reserved1 = 0,
  .Reserved2 = 0,
  // we will fill later the rest
} ;

M2SM_ID_INFO_t M2SM_INFO_IS25LP032D =  {
  
  .VerRev = { 0x01, 0x00 },
  
  .CPN = CPN_IS_3V3_STRING, // board order IS 3V3 BGA 1 populated
  .FG =  "V0.1", // board version code
  .Vmin_Vin_x100 = 270, // incoming power supply acceptable voltage
  .Vmax_Vin_x100 = 360,
  .PopulatedMemoryCount = 1, // 0..4 max from D0+ this tells how many valid xspi memory slots
  .MaxMemoryCount = 2, // 0..4 max how many xspi memories can be populated
  .JEDEC_MemA =  {0x00, 0x00, 0x00, 0x00 }, // this correspond to D0..D3 The last byte meaning is TBD
  .JEDEC_MemB = { 0, 0, 0, 0 }, // this correspond to D4..D7
  .JEDEC_MemC = { 0, 0, 0, 0 }, // this correspond to D8..D11
  .JEDEC_MemD = { 0, 0, 0, 0 }, // this correspond to D12..D15
  .Clock_MHz_Min = 25,
  .Clock_MHz_Max = 133,
  .Reserved1 = 0,
  .Reserved2 = 0,
  // we will fill later the rest
} ;

int8_t *tCPN[] =             {CPN_WB_3V3_STRING,   CPN_MX_3V3_STRING,       CPN_MX_1V8_STRING,       CPN_ST_1V8_STRING, CPN_IS_3V3_STRING};
M2SM_ID_INFO_t *tM2SM_INFO[]={&M2SM_INFO_W25Q16JV, &M2SM_INFO_MX25LM51245G, &M2SM_INFO_MX25UM51245G, &M2SM_INFO_M95P32, &M2SM_INFO_IS25LP032D};
#define NUMBER_OF_MEMORY_SUPPORTED sizeof(tCPN)/sizeof(*tCPN)


#endif /* __M2SM_EEP_INFO */
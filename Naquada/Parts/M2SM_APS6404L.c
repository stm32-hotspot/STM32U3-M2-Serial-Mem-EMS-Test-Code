/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#include "main.h"
#include "SerialMem.h"
#include "M2SM_APS6404L.h"

// W25Q128JV Winbond NOR 128 mbit flash memory
// SerialMemory Description Structure


// 1 bit STR, 8 bit STR is working. 8 bit DTR returns de-orderely data. debug time exceeded.


// These contain a memory description from datasheet

SerialMemGlobalInfo_t SerialMemGlobalInfo =
{
  // Names, part number, manufacturer. We support reset, fast SDR, fast DDR, impedence tune
  .CPN = "APS6404L",
  .ManufacturerName = "AP MEMORY",
  .MemoryType = RAM_Type, // 
  .JEDEC_ID = {0x00, 0x00, 0x00 }, // or 0x70 0x18
  .Support_Features = {         
                      .SDR_1bit_Supported = 1,
                      .SDR_4bit_Supported = 1,
                      },
  .WaitStateVsMHzVsXDR = { {6, 66, 66}, {8, 84, 84}, {10, 104, 104}, {12, 104, 104}, {14, 133, 133}, {16, 133, 133}, {18, 133, 133}, {20, 133, 133} },
  
  .MaxFreq_MHz = 144,       // 
  .Vmin_V_x100 = 162,
  .Vmax_V_x100 = 198,
  
  // 64Mbit
  .SizekByte = 8192, // -1 not available
  .BlockSizeByte = 1024,   // -1 not available
  .SectorSizeByte = 1024,  // -1 not available
  .PageSizeByte = 1024,    // -1 not available
  .ChosenErasableAreaByteSize = 1024, // sector size chosen here (default wrap size)
  .RecommendedMinDataWriteChunkCRCSize = 16,
  .ErasedByteValue = 0xFF,

  .PresetValues32b = {  0x00000000, // 0
                        0x00000001, // 1
                        0x00000200, // 2
                        0x00000300, // 3
                        0x00000400, // 4
                        0x00000500, // 5
                        0x00000800, // 6
                        0x00000C00, // 7
                        0x00000D00, // 8
                        0x00000E00, // 9
                        0x00000F00, // 10
                        0x40000000, // 11
                        0x80000000, // 12
                        0, // 13
                        0, // 14
                        0, // 15
  },
  
  // put here wait states table
  .SR_WEL_MaskVal = 0x02, // set 1 to activate latch
  .SR_WIP_MaskVal = 0x01, // set 1 when memory is busy
  .SDR1BIT_MaskVal = 0,
  .SRDWideBIT_MaskVal = 0,
  .DDR_MaskVal = 0,
  .IMPEDENCE_MaskVal = 0,
  
} ;

DummyCycleVsMHz_t DummyCycleVsMHz[] = {
  
  {  66,  66,  6, 7 },
  {  84,  84,  8, 6 },
  { 104, 104, 10, 5 },
  { 104, 104, 12, 4 },
  { 133, 133, 14, 3 },
  { 133, 133, 16, 2 },
  { 133, 133, 18, 1 },
  { 133, 133, 20, 0 },
};

// Let's do macronix first to understand what we need to get functionality out.
// These will eventually become part of the serialMEM.c (and depending on M2/SerialMem memory board topology, up to 4 memories can be supported)
XSPI_RegularCmdTypeDef     gCommand;


uint8_t JedecInfo[4];
uint8_t ManufDeviceID[2];
uint8_t UniqueID[8];
uint8_t SFDP[256];

uint8_t Mem_SecurityReg[2];
uint8_t Mem_SR[3]; // 3 status registers

extern SerialTransaction_t ST;

M2SerMem_t M2SM = {
  .local_ospi = &hxspi1,
  .pCommand = &gCommand,
  .Config = {},
  .pDataSegment = DataSegment,
  .pST = &ST,
  .MemoryConfigurationMode = 0,
  .MemoryBusMode = {_S1, _S1, _NONE, _S1},
  .DQS_Enabled = 0,
  .RegisterDummyCycles = 20,
  
// here we add information about the memory, which we optionally downloaded from serial I2C EEPROM on board for plug and play purpose  
};

M2SerMem_t * pM2SM = &M2SM;

void MX_NOR_SetMode(M2SerMem_t * pM2SM, uint8_t newmode) {
  
  switch(newmode) {
    
  case 0: // 1 bit SDR
    pM2SM->MemoryBusMode[0] = pM2SM->MemoryBusMode[1] = pM2SM->MemoryBusMode [3] = _S1;
    pM2SM->MemoryBusMode[2] = _NONE;
    pM2SM->MemoryConfigurationMode = 0;
    pM2SM->DQS_Enabled = 0;
    break;
  case 1: // Perf mode (no mode 2). 4 bit address and data when possible (QPI enabled mode)
    pM2SM->MemoryBusMode[0] = _S4;
    pM2SM->MemoryBusMode[1] = _S4;
    pM2SM->MemoryBusMode[2] = _NONE;
    pM2SM->MemoryBusMode[3] = _S4;
    
    pM2SM->MemoryConfigurationMode = 1;
    pM2SM->DQS_Enabled = 0;
    break;

  default:
    while(1);
  }
  
}


// Needed function by memory
//==============================================================================
// Here we implement the generic functions that we need to operate properly the serial memory, not all the features...

extern XSPI_HandleTypeDef hxspi1;



void MX_NOR_EnterQPIMode(M2SerMem_t * pM2SM) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x35, 0x00000000, 0, 0, 0 }, // 1bit STR
    { 0x35, 0x00000000, 0, 0, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];

  if(mode==1) {
    while(1); // already enabled
  }
  else
  {
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionCMD(pM2SM);    
  }
  
}


void MX_NOR_LeaveQPIMode(M2SerMem_t * pM2SM) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x00, 0x00000000, 0, 0, 0 }, // 1bit STR
    { 0xF5, 0x00000000, 0, 0, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];
 
  if(mode==1) {
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionCMD(pM2SM);    
  }
  else
  {
    while(1); // already enabled
  }
  
}


void MX_NOR_ReadData(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x0B, 0x00000000, 3, 0, 8 }, // 1bit STR
    { 0xEB, 0x00000000, 3, 0, 6 }, // 4bit STR 6 or 4 dummy ??
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = Addr;
  ST.DataSizeByte = ByteSize;

  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, pBytes);
}

void MX_NOR_ReadAllRegisters(M2SerMem_t * pM2SM) {
  
}


void MX_NOR_ReadID(M2SerMem_t * pM2SM, uint8_t* Bytes) {
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x9F, 0x00000000, 3, 6, 0 }, // 1bit STR
    { 0x9F, 0x00000000, 3, 6, 0 }, // 4bit STR
  };
  
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, Bytes); 
  
}


void MX_NOR_ReadInfo(M2SerMem_t * pM2SM) {

  MX_NOR_ReadAllRegisters(pM2SM);

  MX_NOR_ReadID(pM2SM, UniqueID);
  
  MX_NOR_ReadData(pM2SM, 0x00000000, DataSegment, 1024);
  
}



void MX_NOR_ProgramData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte) {
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x02, 0x00000000, 3, 0, 0 }, // 1bit STR
    { 0x38, 0x00000000, 3, 0, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = Addr;
  ST.DataSizeByte = SizeByte;

  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionWrite(pM2SM, DataSegment);
}
//========================================================



//========================================================
// Test the driver

uint8_t Impedance = 6;
volatile uint8_t test_branch = 1;
uint8_t Mem_page[32];
void 
MX25LM5145G_Test(void)
{
  // prepare autopoll
  SM_ConfigInit(pM2SM);
  MX_NOR_SetMode(pM2SM,0);
  SerialMemoryTuneDelay();
  //Run the first test in 1 bit mode
  //HAL_XSPI_SetClockPrescaler(&hxspi1, 4);
  //MX_NOR_ReadInfo(pM2SM);
  //MX_NOR_WriteCR(Impedance);
//  HAL_XSPI_SetClockPrescaler(&hxspi1, 1);
  MX_NOR_ReadInfo(pM2SM);

  switch(test_branch) {
  case 1:
     
    //while(1); // stop here for now
    
    DataBufferFill(0,512,0x55,1);
    MX_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,1024,0,1);
    MX_NOR_ProgramData(pM2SM, 0x00000000, 256);
    DataBufferFill(0,1024,0,0);
    MX_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,1024,0x80,0xFF);
    MX_NOR_ProgramData(pM2SM, 0x00000100, 16);
    MX_NOR_ReadInfo(pM2SM);
    break;
  case 2: // test going to 8 bit SDR mode then back to 1 bit SDR mode
    // going to 8 bit SDR mode
    MX_NOR_ReadInfo(pM2SM);
    MX_NOR_EnterQPIMode(pM2SM); // QE bit set
    MX_NOR_SetMode(pM2SM, 1);
    
    MX_NOR_ReadInfo(pM2SM);
    
    MX_NOR_LeaveQPIMode(pM2SM); // QE bit reset
    MX_NOR_SetMode(pM2SM, 0);
    
    MX_NOR_ReadInfo(pM2SM);    
    // program page in 4 bit mode
    break;
  case 4:
    NOPs(1);
    break;
  default: while(1);
  }
    
  while(1)
    NOPs(1);
  
}

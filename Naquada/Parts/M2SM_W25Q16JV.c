/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#include "main.h"
#include "SerialMem.h"
#include "M2SM_W25Q16JV.h"

// W25Q128JV Winbond NOR 128 mbit flash memory
// SerialMemory Description Structure

extern XSPI_HandleTypeDef hxspi1;
// 1 bit STR, 8 bit STR is working. 8 bit DTR returns de-orderely data. debug time exceeded.

uint8_t JEDECID_W25Q16JV_1[]={0xEF, 0x40, 0x15, 0x00 };
uint8_t JEDECID_W25Q16JV_2[]={0xEF, 0x70, 0x15, 0x00 };
uint8_t JEDECID_W25Q16JV_3[]={0xFF, 0xFF, 0xFF, 0xFF };


uint8_t *JEDECID_W25Q16JV[NUMBEROFID]={JEDECID_W25Q16JV_1, JEDECID_W25Q16JV_2, JEDECID_W25Q16JV_3};

// These contain a memory description from datasheet
SerialMemGlobalInfo_t WB_SerialMemGlobalInfo =
{
  // Names, part number, manufacturer. We support reset, fast SDR, fast DDR, impedence tune
  .CPN = "W25Q16JV",
  .ManufacturerName = "WINBOND",
  .MemoryType = NOR_Type, // SFDP, Flash, Sector erase, Page write model
  .DataPinIndexStart = 0,// to fill
  .DataPinIndexEnd = 3,//  to fill
  .JEDEC_ID = JEDECID_W25Q16JV, // or 0x70 0x18
  .Support_Features = {         
                      .SFDP_Supported = 1,
                      .JEDEC_ID_Supported = 1, 
                      .SDR_1bit_Supported = 1,
                      .SDR_4bit_Supported = 1,
                      .WAITSTATES_Tuning_Supported = 1,
                      .IMPEDANCE_Tuning_Supported = 1,
                      },
  .WaitStateVsMHzVsXDR = { {6, 66, 66}, {8, 84, 84}, {10, 104, 104}, {12, 104, 104}, {14, 133, 133}, {16, 133, 133}, {18, 133, 133}, {20, 133, 133} },
  
  .MaxFreq_MHz = 133,       // 
  .Vmin_V_x100 = 270,
  .Vmax_V_x100 = 360,
  
  .Delay_after_power_up_us = 20,// tVSL to fill
  
  // 128Mbit
  .SizekByte = 2048, // -1 not available
  .BlockSizeByte = 1024*64L,   // -1 not available
  .BlockEraseTimeMaxMs = 2000,// to fill
  .SectorSizeByte = 4096,  // -1 not available
  .SectorEraseTimeMaxMs = 400,// to fill
  .PageSizeByte = 256,    // -1 not available
  .PageWriteTimeMaxMs = 3,// to fill
  .ChosenErasableAreaByteSize = 4096, // sector size chosen here
  .WriteRegisterTimeMaxMs = 15,// to fill
  .ResetTimeMaxMs = 5,//tPUD? tRESET=1us to fill
  .AutopollingIntervalTime = 0,// to fill // what unit ?
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
  },
  
  // put here wait states table
  .SR_WEL_MaskVal = 0x02, // set 1 to activate latch
  .SR_WIP_MaskVal = 0x01, // set 1 when memory is busy
  .SR_RegIndex = 0,// to fill // should be 0
  .SDR1BIT_MaskVal = 0,
  .SDR1BIT_RegIndex = 0,// to fill
  .SRDWideBIT_MaskVal = 0,
  .DDR_MaskVal = 0,
  .DDR_MaskVal_RegIndex = 0,// to fill
  .IMPEDANCE_MaskVal = 0,
  .IMPEDANCE_RegIndex = 0,// to fill
  .DummyCycleVsMHz = {0},// to fill
  .DummyCycle_MaskVal = 0,// to fill
  .DummyCycle_RegIndex = 0,// to fill
  
} ;


static DummyCycleVsMHz_t DummyCycleVsMHz[] = {
  
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
extern XSPI_RegularCmdTypeDef     gCommand;
//XSPI_AutoPollingTypeDef    gConfig;
//uint8_t MemoryConfigurationMode = 0;
//SerialConfiguration_t MemoryBusMode[4] = {_S1, _S1, _NONE, _S1}; // CMD > ADDR > ALT > DATA
//uint32_t DQS_Enabled = HAL_XSPI_DQS_DISABLE;
//uint32_t IOSelect = HAL_XSPI_SELECT_IO_7_0;
//uint8_t RegisterDummyCycles = 20; // reset value
//uint32_t TT; //   SEND_CMD | DO_WRITE | DO_READ | DO_AUTOPOLL

extern uint8_t JedecInfo[4];
static uint8_t ManufDeviceID[2];
static uint8_t UniqueID[8];
static uint8_t SFDP[256];

static uint8_t Mem_SecurityReg[2];
static uint8_t Mem_SR[3]; // 3 status registers

extern SerialTransaction_t ST;

extern M2SerMem_t * pM2SM;

void WB_NOR_SetMode(M2SerMem_t * pM2SM, uint8_t newmode) {
  
  switch(newmode) {
    
  case 0: // 1 bit SDR
    pM2SM->MemoryBusMode[0] = pM2SM->MemoryBusMode[1] = pM2SM->MemoryBusMode [3] = _S1;
    pM2SM->MemoryBusMode[2] = _NONE;
    pM2SM->MemoryConfigurationMode = 0;
    pM2SM->DQS_Enabled = 0;
    break;
  case 1: // Perf mode (no mode 2). 4 bit address and data when possible
    pM2SM->MemoryBusMode[0] = _S1;
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

uint8_t WB_NOR_ReadStatusRegister(M2SerMem_t * pM2SM) {

  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x05, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x05, 0x00000000, 0, 1, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];

  if(mode==1) {
    WB_NOR_SetMode(pM2SM, 0);
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionRead(pM2SM, Bytes);    
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionRead(pM2SM, Bytes);    
  }

  pM2SM->Registers[W25Q_STATUS_REG1_INDEX] = Bytes[0];
  
  return Bytes[0];
}

uint8_t WB_NOR_ReadStatusRegister2(M2SerMem_t * pM2SM) {

  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x35, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x35, 0x00000000, 0, 1, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];

  if(mode==1) {
    WB_NOR_SetMode(pM2SM, 0);
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionRead(pM2SM, Bytes);    
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionRead(pM2SM, Bytes);    
  }

  pM2SM->Registers[W25Q_STATUS_REG2_INDEX] = Bytes[0];
  
  return Bytes[0];
}

uint8_t WB_NOR_ReadStatusRegister3(M2SerMem_t * pM2SM) {

  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x15, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x15, 0x00000000, 0, 1, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];

  if(mode==1) {
    WB_NOR_SetMode(pM2SM, 0);
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionRead(pM2SM, Bytes);    
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionRead(pM2SM, Bytes);    
  }

  pM2SM->Registers[W25Q_STATUS_REG3_INDEX] = Bytes[0];
  
  return Bytes[0];
}

void WB_NOR_WriteEnable(M2SerMem_t * pM2SM) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x06, 0x00000000, 0, 0, 0 }, // 1bit STR
    { 0x06, 0x00000000, 0, 0, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];

  if(mode==1) {
    WB_NOR_SetMode(pM2SM, 0);
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionCMD(pM2SM);    
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionCMD(pM2SM);    
  }
  
  WB_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}


void WB_NOR_WriteEnableForVolatileSRs(M2SerMem_t * pM2SM) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x50, 0x00000000, 0, 0, 0 }, // 1bit STR
    { 0x50, 0x00000000, 0, 0, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];

  if(mode==1) {
    WB_NOR_SetMode(pM2SM, 0);
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionCMD(pM2SM);    
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionCMD(pM2SM);    
  }
  
  WB_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}

void WB_NOR_WriteDisable(M2SerMem_t * pM2SM) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x04, 0x00000000, 0, 0, 0 }, // 1bit STR
    { 0x04, 0x00000000, 0, 0, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];

  if(mode==1) {
    WB_NOR_SetMode(pM2SM, 0);
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionCMD(pM2SM);    
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionCMD(pM2SM);    
  }
  
  WB_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}


void WB_NOR_WaitBusyEnds(M2SerMem_t * pM2SM) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x05, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x05, 0x00000000, 0, 1, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  if(mode==1) {
    WB_NOR_SetMode(pM2SM, 0);
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionPoll(pM2SM);    
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    SM_PresetCommandParameters(pM2SM, &ST); // READ SR
    SM_DoTransactionPoll(pM2SM);    
  }  
  
  WB_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}



void WB_NOR_ReadData(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x0B, 0x00000000, 3, 0, 8 }, // 1bit STR
    { 0xEB, 0x00000000, 4, 0, 4 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = Addr;

  if(mode==1)
    ST.Addr = Addr << 8;
  
  ST.DataSizeByte = ByteSize;

  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, pBytes);
}

void WB_NOR_ReadAllRegisters(M2SerMem_t * pM2SM) {
  
  WB_NOR_ReadStatusRegister(pM2SM);
  WB_NOR_ReadStatusRegister2(pM2SM);
  WB_NOR_ReadStatusRegister3(pM2SM);
}

void WB_NOR_ReadJedec(M2SerMem_t * pM2SM, uint8_t* Bytes) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x9F, 0x00000000, 0, 3, 0 }, // 1bit STR
    { 0x9F, 0x00000000, 0, 3, 0 }, // 4bit STR // TO CHECK IF THIS WORKS
  };
    
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  
  SerialTransaction_t ST = STs[mode];

  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, Bytes); 
}


void WB_NOR_ReadSFDP(M2SerMem_t * pM2SM, uint8_t* Bytes) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x5A, 0x00000000, 4, 256, 0 }, // 1bit STR
    { 0x5A, 0x00000000, 4, 256, 0 }, // 4bit STR
  };
  
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, Bytes); 
}


void WB_NOR_ReadManufID(M2SerMem_t * pM2SM, uint8_t* Bytes) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x90, 0x00000000, 3, 2, 0 }, // 1bit STR
    { 0x90, 0x00000000, 3, 2, 0 }, // 4bit STR
  };
  
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, Bytes); 
}

void WB_NOR_ReadID(M2SerMem_t * pM2SM, uint8_t* Bytes) {
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x4B, 0x00000000, 4, 8, 0 }, // 1bit STR
    { 0x4B, 0x00000000, 4, 8, 0 }, // 4bit STR
  };
  
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, Bytes); 
  
}

void WB_NOR_ReadSecurityRegister(M2SerMem_t * pM2SM) { // this actually can read a 256 page
  
  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x4B, 0x00000000, 3, 1, 8 }, // 1bit STR
    { 0x4B, 0x00000000, 3, 1, 8 }, // 4bit STR
  };
  
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, Bytes); 
  
  pM2SM->Registers[W25Q_SECURITY_REG_INDEX] = Bytes[0];
}

void WB_NOR_ReadInfo(M2SerMem_t * pM2SM) {

  WB_NOR_ReadAllRegisters(pM2SM);
  //WB_NOR_ReadStatusRegister(pM2SM);
  //WB_NOR_ReadStatusRegister2(pM2SM);
  //WB_NOR_ReadStatusRegister3(pM2SM);

  WB_NOR_ReadManufID(pM2SM, ManufDeviceID);

  WB_NOR_ReadID(pM2SM, UniqueID);
  
  WB_NOR_ReadJedec(pM2SM, JedecInfo);
  WB_NOR_ReadSFDP(pM2SM, SFDP);
  
  WB_NOR_ReadData(pM2SM, 0x00000000, DataSegment, 4096);

  WB_NOR_ReadSecurityRegister(pM2SM);      
  
}

//////////////////////
void WB_NOR_EraseSegment(M2SerMem_t * pM2SM, uint32_t Addr) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x20, 0x00000000, 3, 0, 0 }, // 1bit STR
    { 0x20, 0x00000000, 3, 0, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = Addr;

  if(mode==1) {
    WB_NOR_SetMode(pM2SM, 0);
    WB_NOR_WriteEnable(pM2SM);
    SM_PresetCommandParameters(pM2SM, &ST);
    SM_DoTransactionCMD(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
    
    WB_NOR_WaitBusyEnds(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    WB_NOR_WriteEnable(pM2SM);
    SM_PresetCommandParameters(pM2SM, &ST);
    SM_DoTransactionCMD(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
    
    WB_NOR_WaitBusyEnds(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
  }
  
  NOPs(1);  
  
}


void WB_NOR_ProgramData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte) {
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x02, 0x00000000, 3, 0, 0 }, // 1bit STR
    { 0x02, 0x00000000, 3, 0, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = Addr;
  ST.DataSizeByte = SizeByte;

  if(mode==1) {
    WB_NOR_SetMode(pM2SM, 0);
    WB_NOR_WriteEnable(pM2SM);
    SM_PresetCommandParameters(pM2SM, &ST);
    SM_DoTransactionWrite(pM2SM, DataSegment);
    WB_NOR_WaitBusyEnds(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    WB_NOR_WriteEnable(pM2SM);
    SM_PresetCommandParameters(pM2SM, &ST);
    SM_DoTransactionWrite(pM2SM, DataSegment);
    WB_NOR_WaitBusyEnds(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
  }
  
  // use security register to avoid reading back all data to confirm proper writing. speeds things up.}
}
//========================================================


void WB_NOR_WriteSR1(M2SerMem_t * pM2SM,  uint8_t byteval) {
  
  uint8_t Bytes[2];
  Bytes[0] = Bytes[1] = byteval;

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x01, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x01, 0x00000000, 0, 1, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  if(mode==1) { // this could be simplified, yet remains here for hack
    WB_NOR_SetMode(pM2SM, 0);
    WB_NOR_WriteEnableForVolatileSRs(pM2SM);
    SM_PresetCommandParameters(pM2SM, &ST);
    SM_DoTransactionWrite(pM2SM, Bytes);  
    WB_NOR_WaitBusyEnds(pM2SM);
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    WB_NOR_WriteEnableForVolatileSRs(pM2SM);
    SM_PresetCommandParameters(pM2SM, &ST);
    SM_DoTransactionWrite(pM2SM, Bytes);  
    WB_NOR_WaitBusyEnds(pM2SM);
  }
}


void WB_NOR_WriteSR2(M2SerMem_t * pM2SM, uint8_t byteval) {

  uint8_t Bytes[2];
  Bytes[0] = Bytes[1] = byteval;

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x31, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x31, 0x00000000, 0, 1, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  if(mode==1) { // this could be simplified, yet remains here for hack
    WB_NOR_SetMode(pM2SM, 0);
    WB_NOR_WriteEnableForVolatileSRs(pM2SM);
    SM_PresetCommandParameters(pM2SM, &ST);
    SM_DoTransactionWrite(pM2SM, Bytes);  
    WB_NOR_WaitBusyEnds(pM2SM); // ???
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    WB_NOR_WriteEnableForVolatileSRs(pM2SM);
    SM_PresetCommandParameters(pM2SM, &ST);
    SM_DoTransactionWrite(pM2SM, Bytes);  
    WB_NOR_WaitBusyEnds(pM2SM); // ???
  }
  
}



void WB_NOR_WriteSR3(M2SerMem_t * pM2SM, uint8_t byteval) {

  uint8_t Bytes[2];
  Bytes[0] = Bytes[1] = byteval;

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x11, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x11, 0x00000000, 0, 1, 0 }, // 4bit STR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  if(mode==1) { // this could be simplified, yet remains here for hack
    WB_NOR_SetMode(pM2SM, 0);
    WB_NOR_WriteEnableForVolatileSRs(pM2SM);
    SM_PresetCommandParameters(pM2SM, &ST);
    SM_DoTransactionWrite(pM2SM, Bytes);  
    WB_NOR_WaitBusyEnds(pM2SM); // ???
    WB_NOR_SetMode(pM2SM, 1);
  }
  else
  {
    WB_NOR_WriteEnableForVolatileSRs(pM2SM);
    SM_PresetCommandParameters(pM2SM, &ST);
    SM_DoTransactionWrite(pM2SM, Bytes);  
    WB_NOR_WaitBusyEnds(pM2SM); // ???
  }  
}



//========================================================
// Test the driver

static uint8_t Impedance = 6;
static volatile uint8_t test_branch = 1;
static uint8_t Mem_page[32];
void W25Q16JV_Test(void)
{
  uint8_t aBytes[10]={0};
  uint8_t *pBytes=aBytes;
    
  // prepare autopoll
  SM_ConfigInit(pM2SM);
  
  SerialMemoryTuneDelay();
  //Run the first test in 1 bit mode
  //HAL_XSPI_SetClockPrescaler(&hxspi1, 4);
  WB_NOR_ReadInfo(pM2SM);
  //WB_NOR_WriteCR(Impedance);
//  HAL_XSPI_SetClockPrescaler(&hxspi1, 1);
  WB_NOR_ReadInfo(pM2SM);

  switch(test_branch) {
  case 1:
    WB_NOR_ReadStatusRegister(pM2SM);
    WB_NOR_WriteEnable(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
    WB_NOR_WriteDisable(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
    
    //while(1); // stop here for now
    
    WB_NOR_EraseSegment(pM2SM, 0);
    
    DataBufferFill(0,512,0x55,1);
    WB_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,1024,0,1);
    WB_NOR_ProgramData(pM2SM, 0x00000000, 256);
    DataBufferFill(0,1024,0,0);
    WB_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,1024,0x80,0xFF);
    WB_NOR_ProgramData(pM2SM, 0x00000100, 16);
    WB_NOR_ReadInfo(pM2SM);
    break;
  case 2:
    WB_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,1024,0,1);
    WB_NOR_ReadData(pM2SM, 0x00000000, pBytes, sizeof(aBytes));
    for (int i = 0; i < sizeof(aBytes); i++) if(aBytes[i] != pM2SM->pDataSegment[i]) break;
    break;  
  case 3: // test going to 8 bit SDR mode then back to 1 bit SDR mode
    // going to 8 bit SDR mode
    WB_NOR_ReadInfo(pM2SM);
    WB_NOR_WriteSR2(pM2SM, 0x02); // QE bit set
    WB_NOR_SetMode(pM2SM, 1);
    
    WB_NOR_ReadInfo(pM2SM);
    
    WB_NOR_WriteSR2(pM2SM, 0x00); // QE bit reset
    WB_NOR_SetMode(pM2SM, 0);
    
    WB_NOR_ReadInfo(pM2SM);    
    // program page in 4 bit mode
    break;
  case 4:
  case 5:
    NOPs(1);
    break;
  default: while(1);
  }
    
  while(1)
    NOPs(1);
  
}

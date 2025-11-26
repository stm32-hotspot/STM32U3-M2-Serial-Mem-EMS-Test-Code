/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#include "main.h"
#include "SerialMem.h"
#include "M2SM_IS25LP032.h"
// W25Q128JV Winbond NOR 128 mbit flash memory
// SerialMemory Description Structure

// 1 bit STR, 8 bit STR is working. 8 bit DTR returns de-orderely data. debug time exceeded.

// These contain a memory description from datasheet

uint8_t JEDECID_IS25LP032_1[]={0x9D, 0x60, 0x16, 0x00 };
uint8_t JEDECID_IS25LP032_2[]={0xFF, 0xFF, 0xFF, 0xFF };
uint8_t JEDECID_IS25LP032_3[]={0xFF, 0xFF, 0xFF, 0xFF };

uint8_t *JEDECID_IS25LP032[NUMBEROFID]={JEDECID_IS25LP032_1, JEDECID_IS25LP032_2, JEDECID_IS25LP032_3};

SerialMemGlobalInfo_t IS_SerialMemGlobalInfo =
{
  // Names, part number, manufacturer. We support reset, fast SDR, fast DDR, impedance tune
  .CPN = "IS25LP032D",
  .ManufacturerName = "ISSI",
  .MemoryType = NOR_Type, // SFDP, Flash, Sector erase, Page write model
  .DataPinIndexStart = 0,// to fill
  .DataPinIndexEnd = 3,// to fill
  .JEDEC_ID = JEDECID_IS25LP032, // or 0x70 0x18
  .Support_Features = {         
                      .SFDP_Supported = 1,
                      .JEDEC_ID_Supported = 1, 
                      .SDR_1bit_Supported = 1,
                      .SDR_4bit_Supported = 1,
                      .DDR_4bit_Supported = 1,
                      .WAITSTATES_Tuning_Supported = 1,
                      .IMPEDANCE_Tuning_Supported = 1,
                      },
  .WaitStateVsMHzVsXDR = { {6, 66, 66}, {8, 84, 84}, {10, 104, 104}, {12, 104, 104}, {14, 133, 133}, {16, 133, 133}, {18, 133, 133}, {20, 133, 133} },
  
  .MaxFreq_MHz = 133,       // 
  .Vmin_V_x100 = 230,
  .Vmax_V_x100 = 360,
  
  .Delay_after_power_up_us = 100,// tVSL to fill
  
  // 128Mbit
  .SizekByte = 4096, // -1 not available
  .BlockSizeByte = 1024*64L,   // -1 not available
  .BlockEraseTimeMaxMs = 1500,// to fill
  .SectorSizeByte = 4096,  // -1 not available
  .SectorEraseTimeMaxMs = 300,// to fill
  .PageSizeByte = 256,    // -1 not available
  .PageWriteTimeMaxMs = 1,// to fill
  .ChosenErasableAreaByteSize = 4096, // sector size chosen here
  .WriteRegisterTimeMaxMs = 15,// to fill
  .ResetTimeMaxMs = 1,// 100µs, to fill
  .AutopollingIntervalTime = 0,// to fill // what unit ?
  .RecommendedMinDataWriteChunkCRCSize = 16,
  .ErasedByteValue = 0xFF,

  .PresetValues32b = {  0x00000000, // 0
                        0x00000001, // 1
                        0x00000000, // 2
                        0x00000000, // 3
                        0x00000000, // 4
                        0x00000000, // 5
                        0x00000000, // 6
                        0x00000000, // 7
                        0x00000000, // 8
                        0x00000000, // 9
                        0x00000000, // 10
                        0x00000000, // 11
                        0x00000000, // 12
                        0, // 13
                        0, // 14
                        0, // 15
  },
  
  // put here wait states table
  .SR_WEL_MaskVal = 0x02, // set 1 to activate latch
  .SR_WIP_MaskVal = 0x01, // set 1 when memory is busy
  .SR_RegIndex = 0,// to fill // should be 0
  .SDR1BIT_MaskVal = 0x40,
  .SDR1BIT_RegIndex = 0,// to fill
  .SRDWideBIT_MaskVal = 0x40,
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

extern uint8_t JedecInfo[4];
static uint8_t ManufDeviceID[2];
static uint8_t UniqueID[8];
static uint8_t SFDP[256];

extern SerialTransaction_t ST;

extern M2SerMem_t * pM2SM;

void IS_NOR_SetMode(M2SerMem_t * pM2SM, uint8_t newmode) {
  
  SerialConfiguration_t * MemoryBusMode = pM2SM->MemoryBusMode;
  
  switch(newmode) {
    
  case 0: // 1 bit SDR
    MemoryBusMode[0] = MemoryBusMode[1] = MemoryBusMode [3] = _S1;
    MemoryBusMode[2] = _NONE;
    pM2SM->MemoryConfigurationMode = 0;
    pM2SM->DQS_Enabled = 0;
    break;
  case 1: // 4 bit SDR QPI
    MemoryBusMode[0] = MemoryBusMode[1] = MemoryBusMode [3] = _S4;
    MemoryBusMode[2] = _NONE;
    pM2SM->MemoryConfigurationMode = 1;
    pM2SM->DQS_Enabled = 0;
    break;
  case 2: // 4 bit DDR QPI (performance mode)
    MemoryBusMode[0] = _S4; // command remains in STR mode
    MemoryBusMode[1] = MemoryBusMode [3] = _D4;
    MemoryBusMode[2] = _NONE;
    pM2SM->MemoryConfigurationMode = 2;
    pM2SM->DQS_Enabled = 0;
    break;
  default:
    while(1);
  }
  
}


// Needed function by memory
//==============================================================================
// Here we implement the generic functions that we need to operate properly the serial memory, not all the features...

void ConfigInit(XSPI_AutoPollingTypeDef* pConfig, uint8_t preset);

extern XSPI_HandleTypeDef hxspi1;

uint8_t IS_NOR_ReadStatusRegister(M2SerMem_t * pM2SM) {

  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x05, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x05, 0x00000000, 0, 1, 0 }, // 4bit STR
    { 0x05, 0x00000000, 0, 1, 0 }  // 4bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST); // READ SR
  SM_DoTransactionRead(pM2SM, Bytes);
  pM2SM->Registers[IS25LP_STATUS_REG_INDEX] = Bytes[0];
  
  return Bytes[0];
}


uint8_t IS_NOR_ReadFunctionRegister(M2SerMem_t * pM2SM) {
  
  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x48, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x48, 0x00000000, 0, 1, 0 }, // 4bit STR
    { 0x48, 0x00000000, 0, 1, 0 }  // 4bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST); // READ SR
  SM_DoTransactionRead(pM2SM, Bytes);
  pM2SM->Registers[IS25LP_FUNCTION_REG_INDEX] = Bytes[0];
  
  return Bytes[0];
}


uint8_t IS_NOR_ReadParametersRegister(M2SerMem_t * pM2SM) {

  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x61, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x61, 0x00000000, 0, 1, 0 }, // 4bit STR
    { 0x61, 0x00000000, 0, 1, 0 }  // 4bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST); // READ SR
  SM_DoTransactionRead(pM2SM, Bytes);
  pM2SM->Registers[IS25LP_READ_REG_INDEX] = Bytes[0];
  
  return Bytes[0];  
}


uint8_t IS_NOR_ReadExtendeddParametersRegister(M2SerMem_t * pM2SM) {

  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x81, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x81, 0x00000000, 0, 1, 0 }, // 4bit STR
    { 0x81, 0x00000000, 0, 1, 0 }  // 4bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST); // READ SR
  SM_DoTransactionRead(pM2SM, Bytes);
  pM2SM->Registers[IS25LP_EXTENDED_READ_INDEX] = Bytes[0];
  
  return Bytes[0];  
}


void IS_NOR_EnterQPI_Mode(M2SerMem_t * pM2SM) {
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x35, 0x00000000, 0, 0, 0 }, // 1bit STR -- The only valid one
    { 0, 0x00000000, 0, 0, 0 }, // 4bit STR
    { 0, 0x00000000, 0, 0, 0 }  // 4bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=1 /* <!!!> */)   while(1); // this command works only if we are not yet in 4 bit mode.
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST); // READ SR
  SM_DoTransactionCMD(pM2SM);
  
  IS_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}


void IS_NOR_DisableQPI_Mode(M2SerMem_t * pM2SM) {
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0, 0x00000000, 0, 0, 0 }, // 1bit STR -- The only valid one
    { 0xF5, 0x00000000, 0, 0, 0 }, // 4bit STR
    { 0xF5, 0x00000000, 0, 0, 0 }  // 4bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  if(mode==0 /* <!!!> */)   while(1); // this command works only if we are in 4 bit mode.
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST); // READ SR
  SM_DoTransactionCMD(pM2SM);
  
  IS_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}


void IS_NOR_WriteEnable(M2SerMem_t * pM2SM) {
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x06, 0x00000000, 0, 0, 0 }, // 1bit STR -- The only valid one
    { 0x06, 0x00000000, 0, 0, 0 }, // 4bit STR
    { 0x06, 0x00000000, 0, 0, 0 }  // 4bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST); // READ SR
  SM_DoTransactionCMD(pM2SM);
    
  IS_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}


void IS_NOR_WriteDisable(M2SerMem_t * pM2SM) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x04, 0x00000000, 0, 0, 0 }, // 1bit STR -- The only valid one
    { 0x04, 0x00000000, 0, 0, 0 }, // 4bit STR
    { 0x04, 0x00000000, 0, 0, 0 }  // 4bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST); // READ SR
  SM_DoTransactionCMD(pM2SM);
    
  IS_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}


void IS_NOR_WaitBusyEnds(M2SerMem_t * pM2SM) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x05, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x05, 0x00000000, 0, 1, 0 }, // 8bit STR
    { 0x05, 0x00000000, 0, 1, 0 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  SM_PresetCommandParameters(pM2SM, &ST);// READ SR
  SM_DoTransactionPoll(pM2SM);
  
  IS_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}


void IS_NOR_ReadAllRegisters(M2SerMem_t * pM2SM) {
  
  IS_NOR_ReadStatusRegister(pM2SM);
  IS_NOR_ReadFunctionRegister(pM2SM);
  IS_NOR_ReadParametersRegister(pM2SM);
  IS_NOR_ReadExtendeddParametersRegister(pM2SM);
}


void IS_NOR_ReadID(M2SerMem_t * pM2SM, uint8_t* Bytes, uint8_t* pSerialNb) {

//      ST.CMD = 0xAB; // Read ID
//      ST.Addr = 0;
//      ST.AddrSizeByte = 3;
//      ST.DataSizeByte = 8;
//      SM_PresetCommandParameters(&gCommand, &ST);
//      SM_DoTransaction(&hospi1, &ST, UniqueID);
  
  const SerialTransaction_t STs[3] = { // RDMDID MF7..0, ID7..0
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x90, 0x00000000, 3, 2, 0 }, // 1bit STR
    { 0x90, 0x00000000, 3, 2, 0 }, // 8bit STR
    { 0x90, 0x00000000, 3, 2, 0 }  // 8bit DTR Data in STR mode (exception)
  };
  
  SerialConfiguration_t t = pM2SM->MemoryBusMode[3];
  
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, Bytes); 
  
  // serial number stored in chunk
    const SerialTransaction_t STsBis[3] = { // RDMDID MF7..0, ID7..0
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x4B, 0x00000000, 3, 8, 8 }, // 1bit STR
    { 0x4B, 0x00000000, 3, 8, 8 }, // 8bit STR
    { 0x4B, 0x00000000, 3, 8, 8 }  // 8bit DTR Data in STR mode (exception)
  };
  
  ST = STsBis[mode];
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, pSerialNb); 

}


void IS_NOR_ReadJedec(M2SerMem_t * pM2SM, uint8_t* Bytes) {

//      ST.CMD = 0x9F; // Read JEDEC
//      ST.Addr = 0;
//      ST.AddrSizeByte = 0;
//      ST.DataSizeByte = 3;
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x9F, 0x00000000, 0, 3, 0 }, // 1bit STR
    { 0x9F, 0x00000000, 0, 3, 0 }, // 8bit STR
    { 0x9F, 0x00000000, 0, 3, 0 }  // 8bit DTR Data in STR mode (exception) // AF alternate command in QPI mode
  };
  
  SerialConfiguration_t t = pM2SM->MemoryBusMode[3];
  
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  
  SerialTransaction_t ST = STs[mode];

  if(mode == 2) 
  {
    pM2SM->MemoryBusMode[3] = _S8; // STR for data read in JEDEC info retrieval
    SM_PresetCommandParameters(pM2SM, &ST);
    pM2SM->MemoryBusMode[3] = t;    // restore
  }
  else
  {
    SM_PresetCommandParameters(pM2SM, &ST);
  }
  
  SM_DoTransactionRead(pM2SM, Bytes); 
}


void IS_NOR_ReadSFDP(M2SerMem_t * pM2SM, uint8_t* Bytes) {

//      ST.CMD = 0x5A;
//      ST.Addr = 0;
//      ST.AddrSizeByte = 4;
//      ST.DataSizeByte = 256;
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x5A, 0x00000000, 4, 256, 0 }, // 1bit STR
    { 0x5A, 0x00000000, 4, 256, 0 }, // 8bit STR
    { 0x5A, 0x00000000, 4, 256, 0 }  // 8bit DTR // 4 or 6 ?
  };
  
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, Bytes); 
}


void IS_NOR_ReadData(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x0B, 0x00000000, 3, 0, -1 /*8*/ }, // 1bit STR
    { 0xEB, 0x00000000, 4, 0, -1 /*4*/ }, // 8bit STR
    { 0x0D, 0x00000000, 3, 0, -1 /*6*/ }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  
  SerialTransaction_t ST = STs[mode];
  ST.Addr = Addr;
  if(mode == 1) ST.Addr = Addr<<8; // artificial way to avoid using ALT for mode bits
  ST.DataSizeByte = ByteSize;

  if(mode == 1) {
    if(pM2SM->RegisterDummyCycles<2) while(1);
    pM2SM->RegisterDummyCycles -= 2;
    SM_PresetCommandParameters(pM2SM, &ST);
    pM2SM->RegisterDummyCycles += 2;
  }
  else
  {
    SM_PresetCommandParameters(pM2SM, &ST);
  }
  SM_DoTransactionRead(pM2SM, pBytes);
}


void IS_NOR_ReadManufID(M2SerMem_t * pM2SM, uint8_t* pBytes) {
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x90, 0x00000000, 3, 2, 0 }, // 1bit STR
    { 0x90, 0x00000000, 3, 2, 0 }, // 8bit STR
    { 0x90, 0x00000000, 3, 2, 0 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, pBytes);
  
}


void IS_NOR_ReadInfo(M2SerMem_t * pM2SM) {
  
  IS_NOR_ReadAllRegisters(pM2SM);
  IS_NOR_ReadID(pM2SM, UniqueID, DataSegment);
  IS_NOR_ReadJedec(pM2SM, JedecInfo);
  IS_NOR_ReadSFDP(pM2SM, SFDP);
  IS_NOR_ReadManufID(pM2SM, ManufDeviceID);
  IS_NOR_ReadData(pM2SM, 0x00000000, DataSegment, 4096);

//   IS_NOR_ReadData(pM2SM, 0x00000000, DataSegment, 4096);
}


//////////////////////
void IS_NOR_EraseSegment(M2SerMem_t * pM2SM, uint32_t Addr) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0xD7, 0x00000000, 3, 0, 0 }, // 1bit STR
    { 0x20, 0x00000000, 3, 0, 0 }, // 8bit STR
    { 0x20, 0x00000000, 3, 0, 0 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = Addr;

  IS_NOR_WriteEnable(pM2SM);
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionCMD(pM2SM);
  
  IS_NOR_ReadStatusRegister(pM2SM);
  IS_NOR_ReadStatusRegister(pM2SM);
  
  IS_NOR_WaitBusyEnds(pM2SM);
  IS_NOR_ReadStatusRegister(pM2SM);
  NOPs(1);  
}


void IS_NOR_ProgramData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte) {
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x02, 0x00000000, 3, 0, 0 }, // 1bit STR
    { 0x02, 0x00000000, 3, 0, 0 }, // 8bit STR
    { 0x02, 0x00000000, 3, 0, 0 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = Addr;
  ST.DataSizeByte = SizeByte;

  IS_NOR_WriteEnable(pM2SM);

  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionWrite(pM2SM, DataSegment); // we base from beginning of buffer ?
  IS_NOR_WaitBusyEnds(pM2SM);
  
  // use security register to avoid reading back all data to confirm proper writing. speeds things up.
}

//========================================================

void IS_NOR_WriteStatusRegister(M2SerMem_t * pM2SM, uint8_t Byte) {

  uint8_t Bytes[2];
  Bytes[0] = Bytes[1] = Byte;

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x01, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x01, 0x00000000, 0, 1, 0 }, // 8bit STR
    { 0x01, 0x00000000, 0, 1, 0 }  // 8bit DTR 2
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  IS_NOR_WriteEnable(pM2SM);
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionWrite(pM2SM, Bytes); 
  
//  IS_NOR_WaitBusyEnds(pM2SM);
  IS_NOR_ReadStatusRegister(pM2SM);
}


void IS_NOR_WriteFunctionRegister(M2SerMem_t * pM2SM, uint8_t Byte) { 
 
  uint8_t Bytes[2];
  Bytes[0] = Bytes[1] = Byte;

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x42, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x42, 0x00000000, 0, 1, 0 }, // 8bit STR
    { 0x42, 0x00000000, 0, 1, 0 }  // 8bit DTR 2
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  IS_NOR_WriteEnable(pM2SM);
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionWrite(pM2SM, Bytes); 
  
//  IS_NOR_WaitBusyEnds(pM2SM);
  IS_NOR_ReadStatusRegister(pM2SM);  
  
  IS_NOR_ReadFunctionRegister(pM2SM);
}


void IS_NOR_WriteParametersRegister(M2SerMem_t * pM2SM, uint8_t Byte) {
  
  uint8_t Bytes[2];
  Bytes[0] = Bytes[1] = Byte;

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0xC0, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x63, 0x00000000, 0, 1, 0 }, // 8bit STR
    { 0x63, 0x00000000, 0, 1, 0 }  // 8bit DTR 2
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  IS_NOR_WriteEnable(pM2SM);
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionWrite(pM2SM, Bytes); 
  
//  IS_NOR_WaitBusyEnds(pM2SM);
  IS_NOR_ReadStatusRegister(pM2SM);  
  
  IS_NOR_ReadParametersRegister(pM2SM);  
}


void IS_NOR_WriteExtendeddParametersRegister(M2SerMem_t * pM2SM, uint8_t Byte) {
// Write Extended Read parameter register
 
  uint8_t Bytes[2];
  Bytes[0] = Bytes[1] = Byte;

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    { 0x83, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x83, 0x00000000, 0, 1, 0 }, // 8bit STR
    { 0x83, 0x00000000, 0, 1, 0 }  // 8bit DTR 2
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  IS_NOR_WriteEnable(pM2SM);
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionWrite(pM2SM, Bytes);   
//  IS_NOR_WaitBusyEnds(pM2SM);
  
  IS_NOR_ReadExtendeddParametersRegister(pM2SM);
}




int32_t IS_NOR_GetDummyCycles(M2SerMem_t * pM2SM) { // untested

  uint8_t RPR = IS_NOR_ReadParametersRegister(pM2SM);
  RPR = RPR>>3 & 0x0F;
  return RPR;
}

void IS_NOR_SetDummyCycles(M2SerMem_t * pM2SM, int32_t dummycycles) { // untested

  uint8_t RPR = IS_NOR_ReadParametersRegister(pM2SM);
  dummycycles &= 0x0F;
  RPR &= ~0x78; // clear dummy bit field
  RPR |= dummycycles<<3;
  IS_NOR_WriteParametersRegister(pM2SM, RPR);
  IS_NOR_ReadParametersRegister(pM2SM); // verify
  pM2SM->RegisterDummyCycles = dummycycles;
}


//========================================================
// Test the driver

static uint8_t Impedance = 6;
static volatile uint8_t test_branch = 1;
static uint8_t Mem_page[32];
void IS25LP032D_Test(void)
{
  // prepare autopoll
  SM_ConfigInit(pM2SM);
  
  //SerialMemoryTuneDelay();
//  SM_PresetCommandParameters(&gCommand, TT, 0x05, 0, 1, 0); // READ SR 
  IS_NOR_SetDummyCycles(pM2SM, 8);
  //Run the first test in 1 bit mode
//  HAL_XSPI_SetClockPrescaler(&hxspi1, 4);
  IS_NOR_ReadInfo(pM2SM);
  //IS_NOR_WriteCR(pM2SM, Impedance);
//  HAL_XSPI_SetClockPrescaler(&hxspi1, 1);
  IS_NOR_ReadInfo(pM2SM);

  switch(test_branch) {
  case 1:
    IS_NOR_ReadStatusRegister(pM2SM);
    IS_NOR_WriteEnable(pM2SM);
    IS_NOR_ReadStatusRegister(pM2SM);
    IS_NOR_WriteDisable(pM2SM);
    IS_NOR_ReadStatusRegister(pM2SM);
    
    //while(1); // stop here for now
    
    IS_NOR_EraseSegment(pM2SM, 0);
    
    DataBufferFill(0,512,0x55,1);
    IS_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,1024,0,1);
    IS_NOR_ProgramData(pM2SM, 0x00000000, 256);
    DataBufferFill(0,1024,0,0);
    IS_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,1024,0x80,0xFF);
    IS_NOR_ProgramData(pM2SM, 0x00000100, 16);
    IS_NOR_ReadInfo(pM2SM);
    break;
  case 2: // test going to 8 bit SDR mode then back to 1 bit SDR mode
    // going to 8 bit SDR mode
    IS_NOR_EnterQPI_Mode(pM2SM);
    IS_NOR_SetMode(pM2SM, 1);
    DataBufferFill(0,1024,0,0);
    IS_NOR_ReadData(pM2SM, 0x00000000, DataSegment, 4096);
    
    IS_NOR_SetMode(pM2SM, 2);
    DataBufferFill(0,1024,0,0);
    IS_NOR_ReadData(pM2SM, 0x00000000, DataSegment, 4096);
    
    
    IS_NOR_DisableQPI_Mode(pM2SM);
    IS_NOR_SetMode(pM2SM, 0);
    // do something
    IS_NOR_WriteParametersRegister(pM2SM, 0x03); // change the burst length we don't use
    IS_NOR_WriteExtendeddParametersRegister(pM2SM, 0xB0); // change impedance
    IS_NOR_ReadFunctionRegister(pM2SM);
    IS_NOR_ReadParametersRegister(pM2SM);
    IS_NOR_ReadExtendeddParametersRegister(pM2SM);
    
    //IS_NOR_WriteCR(0x07);
    IS_NOR_ReadInfo(pM2SM);
    
    // going back to 1 bit SDR mode
    //IS_NOR_WriteCR2(0,0x00); // switch to 8 bit SDR mode
    
    IS_NOR_ReadInfo(pM2SM);    
    break;
  case 3: // test going to 8 bit SDR then DDR mode

    NOPs(1);
    
    break;
  case 4:
    IS_NOR_ReadInfo(pM2SM);
    NOPs(1);
    break;
  }
    
  while(1)
    NOPs(1);
}

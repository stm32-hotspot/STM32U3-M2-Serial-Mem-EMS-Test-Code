/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#include "main.h"
#include "SerialMem.h"
// SerialMemory Description Structure
// MX25LM51245G 1,8V 1/8 bit NOR Macronix
#include "M2SM_MX25LM51245G.h"

// 1 bit STR, 8 bit STR is working. 8 bit DTR returns de-orderely data. debug time exceeded.
extern XSPI_HandleTypeDef hxspi1;
// These contain a memory description from datasheet

uint8_t JEDECID_MX25LM51245G_1[]={0xC2, 0x85, 0x3A, 0x00 };
uint8_t JEDECID_MX25LM51245G_2[]={0xC2, 0x80, 0x3A, 0x01 };// reserved for future use when thevolatge can be measured
uint8_t JEDECID_MX25LM51245G_3[]={0xFF, 0xFF, 0xFF, 0xFF };
   
uint8_t *JEDECID_MX25LM51245G[NUMBEROFID]={JEDECID_MX25LM51245G_1, JEDECID_MX25LM51245G_2, JEDECID_MX25LM51245G_3};

SerialMemGlobalInfo_t MX_33_SerialMemGlobalInfo =
{
  // Names, part number, manufacturer. We support reset, fast SDR, fast DDR, impedence tune
  .CPN = "MX25LM51245G",
  .ManufacturerName = "MACRONIX",
  .MemoryType = NOR_Type, // SFDP, Flash, Sector erase, Page write model
  .DataPinIndexStart = 0,// to fill
  .DataPinIndexEnd = 7,// to fill
  .JEDEC_ID = JEDECID_MX25LM51245G,
  .Support_Features = {         
                      .SFDP_Supported = 1,
                      .JEDEC_ID_Supported = 1, 
                      .SDR_1bit_Supported = 1,
                      .SDR_8bit_Supported = 1,
                      .DDR_1bit_Supported = 1,
                      .DDR_8bit_Supported = 1,
                      .DQS_Supported = 1,
                      .WAITSTATES_Tuning_Supported = 1,
                      .IMPEDANCE_Tuning_Supported = 1,
                      },
  .WaitStateVsMHzVsXDR = { {6, 66, 66}, {8, 84, 84}, {10, 104, 104}, {12, 104, 104}, {14, 133, 133}, {16, 133, 133}, {18, 133, 133}, {20, 133, 133} },
  
  .MaxFreq_MHz = 133,       // 
  .Vmin_V_x100 = 270,
  .Vmax_V_x100 = 360,
  
  .Delay_after_power_up_us = 3000,// tVSL to fill
  
  .SizekByte = 65536, // -1 not available
  .BlockSizeByte = 1024*64L,   // -1 not available
  .BlockEraseTimeMaxMs = 2000,// to fill
  .SectorSizeByte = 4096,  // -1 not available
  .SectorEraseTimeMaxMs = 400,// to fill
  .PageSizeByte = 256,    // -1 not available
  .PageWriteTimeMaxMs = 1,// to fill
  .ChosenErasableAreaByteSize = 4096, // sector size chosen here
  .WriteRegisterTimeMaxMs = 40,// to fill
  .ResetTimeMaxMs = 100,//tREADY2? to fill
  .AutopollingIntervalTime = 0,// to fill // what unit ?
  .RecommendedMinDataWriteChunkCRCSize = 16,
  .ErasedByteValue = 0xFF,

  .PresetValues32b = {  0x00000000, // 0 // SR same index as register's, put 0 if unused
                        0x00000001, // 1 // CR
                        0x00000000, // 2 // Security
                        0x00000000, // 3 CR2_0
                        0x00000200, // 4
                        0x00000300, // 5
                        0x00000400, // 6
                        0x00000500, // 7
                        0x00000800, // 8
                        0x00000C00, // 9
                        0x00000D00, // 10
                        0x00000E00, // 11
                        0x00000F00, // 12
                        0x40000000, // 13
                        0x80000000, // 14
                        0, // 15
  },
  
  // put here wait states table
  .SR_WEL_MaskVal = 0x02, // set 1 to activate latch
  .SR_WIP_MaskVal = 0x01, // set 1 when memory is busy
  .SR_RegIndex = 0,// to fill // should be 0
  .SDR1BIT_MaskVal = 0x03,
  .SDR1BIT_RegIndex = 0,// to fill
  .SRDWideBIT_MaskVal = 0x03,
  .DDR_MaskVal = 0x03,
  .DDR_MaskVal_RegIndex = 0,// to fill
  .IMPEDANCE_MaskVal = 0x07,
  .IMPEDANCE_RegIndex = 1, // CR bit 2..0 default 111 24 ohm... 000 146 ohms
  .DummyCycle_MaskVal = 0x07,
  .DummyCycle_RegIndex = 5, // CR2_300
} ;

const uint32_t CR2_Adresses[] = { 
  0x00000000, // 0
  0x00000200, // 1
  0x00000300, // 2
  0x00000400, // 3
  0x00000500, // 4 
  0x00000800, // 5
  0x00000C00, // 6 
  0x00000D00, // 7 
  0x00000E00, // 8
  0x00000F00, // 9
  0x40000000, // 10
  0x80000000  // 11
};

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
static uint8_t SFDP[256];
//uint8_t DataBuffer[4096];

extern SerialTransaction_t ST;

extern M2SerMem_t * pM2SM;

void MX_33_NOR_SetMode(M2SerMem_t * pM2SM, uint8_t newmode) {
  
  SerialConfiguration_t * MemoryBusMode = pM2SM->MemoryBusMode;
  
  switch(newmode) {
    
  case 0: // 1 bit SDR
    MemoryBusMode[0] = MemoryBusMode[1] = MemoryBusMode [3] = _S1;
    MemoryBusMode[2] = _NONE;
    pM2SM->MemoryConfigurationMode = 0;
    pM2SM->DQS_Enabled = 0;
    break;
  case 1: // 8 bit SDR
    MemoryBusMode[0] = MemoryBusMode[1] = MemoryBusMode [3] = _S8;
    MemoryBusMode[2] = _NONE;
    pM2SM->MemoryConfigurationMode = 1;
    pM2SM->DQS_Enabled = 0;
    break;
  case 2: // 8 bit DDR (memory's DQS will turn on)
    MemoryBusMode[0] = MemoryBusMode[1] = MemoryBusMode [3] = _D8;
    MemoryBusMode[2] = _NONE;
    pM2SM->MemoryConfigurationMode = 2;
    pM2SM->DQS_Enabled = 1;
    break;
  default:
    while(1);
  }
  
}


// Needed function by memory
//==============================================================================
// Here we implement the generic functions that we need to operate properly the serial memory, not all the features...

//void SM_PresetCommandParameters(XSPI_RegularCmdTypeDef* pCommand, uint32_t TT, uint16_t CMD, void* pMem, uint32_t Addr, uint32_t DataSizeByte, int8_t DummyCycles);
//void ConfigInit(XSPI_AutoPollingTypeDef* pConfig, uint8_t preset);

//=== First, we try to read information from the memory

uint8_t MX_33_NOR_ReadStatusRegister(M2SerMem_t * pM2SM) {

  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x05, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x05FA, 0x00000000, 4, 1, 4 }, // 8bit STR
    { 0x05FA, 0x00000000, 4, 2, 6 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs))   while(1);
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST); // READ SR
  SM_DoTransactionRead(pM2SM, Bytes);
  pM2SM->Registers[MX25LM_STATUS_REG_INDEX] = Bytes[0];
  
  return Bytes[0];
}

void MX_33_NOR_WriteEnable(M2SerMem_t * pM2SM) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x06, 0x00000000, 0, 0, 0 }, // 1bit STR
    { 0x06F9, 0x00000000, 0, 0, 0 }, // 8bit STR
    { 0x06F9, 0x00000000, 0, 0, 0 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  SM_PresetCommandParameters(pM2SM, &ST); // WREN
  SM_DoTransactionCMD(pM2SM);
  
  MX_33_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}

void MX_33_NOR_WriteDisable(M2SerMem_t * pM2SM) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x04, 0x00000000, 0, 0, 0 }, // 1bit STR
    { 0x04FB, 0x00000000, 0, 0, 0 }, // 8bit STR
    { 0x04FB, 0x00000000, 0, 0, 0 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  SM_PresetCommandParameters(pM2SM, &ST);// 
  SM_DoTransactionCMD(pM2SM);
  
  MX_33_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}

void MX_33_NOR_WaitBusyEnds(M2SerMem_t * pM2SM) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x05, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x05FA, 0x00000000, 4, 1, 4 }, // 8bit STR
    { 0x05FA, 0x00000000, 4, 1, 6 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  SM_PresetCommandParameters(pM2SM, &ST);// READ SR
  SM_DoTransactionPoll(pM2SM);
  
  MX_33_NOR_ReadStatusRegister(pM2SM);// bonus for debug
}


//========================================================

void MX_33_NOR_ReadCR(M2SerMem_t * pM2SM) {
  
  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x15, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x15EA, 0x00000001, 4, 1, 4 }, // 8bit STR -1
    { 0x15EA, 0x00000001, 4, 1, 5 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST); // READ CR
  SM_DoTransactionRead(pM2SM, Bytes);
  
  pM2SM->Registers[MX25LM_CONTROL_REG_INDEX] = Bytes[0];
}

void MX_33_NOR_WriteCR(M2SerMem_t * pM2SM, uint8_t Byte) {

  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x01, 0x00000000, 0, 2, 0 }, // 1bit STR
    { 0x01FE, 0x00000001, 4, 1, 0 }, // 8bit STR
    { 0x01FE, 0x00000001, 4, 2, 0 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  MX_33_NOR_ReadStatusRegister(pM2SM);
  MX_33_NOR_WriteEnable(pM2SM);  
  
  switch(pM2SM->MemoryConfigurationMode) {
    
  case 0: // 1bit STR
      Bytes[0] = pM2SM->Registers[MX25LM_STATUS_REG_INDEX]; // existing SR value
      Bytes[1] = Byte; // new CR value
    break;
    
  case 1: // 8 bit STR and DDR
    Bytes[0] = Bytes[1] = Byte;
    break;

  case 2: // 8 bit DDR
    Bytes[0] = Bytes[1] = Byte;
    break;
    
  default:
    while(1); // bug
  }
  
  MX_33_NOR_WriteEnable(pM2SM);
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionWrite(pM2SM, Bytes); // we base from beginning of buffer ?
  MX_33_NOR_WaitBusyEnds(pM2SM);
  MX_33_NOR_ReadStatusRegister(pM2SM);  
}

void MX_33_NOR_ReadAllCR2(M2SerMem_t * pM2SM) {
  
  for(int i=0;i<countof(CR2_Adresses);i++) {
    MX_33_NOR_ReadCR2(pM2SM, i);
  }
}

void MX_33_NOR_ReadCR2(M2SerMem_t * pM2SM, uint8_t index) {

  uint8_t Bytes[2];
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x71, 0x00000000, 4, 1, 0 }, // 1bit STR
    { 0x718E, 0x00000000, 4, 1, 4 }, // 8bit STR
    { 0x718E, 0x00000000, 4, 2, 5 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = CR2_Adresses[index];

  SM_PresetCommandParameters(pM2SM, &ST);//, 0x718E, CR2_Adresses[index], 2, -1);
  SM_DoTransactionRead(pM2SM, Bytes); //
  
  pM2SM->Registers[MX25LM_CR2_0 + index] = Bytes[0];
}


void MX_33_NOR_WriteCR2(M2SerMem_t * pM2SM, uint8_t index, uint8_t byteval) {
  
  uint8_t Bytes[2];
  Bytes[0] = Bytes[1] = byteval;

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x72, 0x00000000, 4, 1, 0 }, // 1bit STR
    { 0x728D, 0x00000000, 4, 1, 0 }, // 8bit STR
    { 0x728D, 0x00000000, 4, 1, 0 }  // 8bit DTR 2
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = CR2_Adresses[index];

  MX_33_NOR_WriteEnable(pM2SM);
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionWrite(pM2SM, Bytes); 
}


void MX_33_NOR_ReadSecurityRegister(M2SerMem_t * pM2SM) {
  
  uint8_t Bytes[2];
  
  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x2B, 0x00000000, 0, 1, 0 }, // 1bit STR
    { 0x2BD4, 0x00000000, 4, 1, 4 }, // 8bit STR
    { 0x2BD4, 0x00000000, 4, 1, 6 }  // 8bit DTR // 4 or 6 ? 1
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];

  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, Bytes); 
  
  pM2SM->Registers[MX25LM_SECURITY_REG_INDEX] = Bytes[0];
}

void MX_33_NOR_ReadAllRegisters(M2SerMem_t * pM2SM) {
  
  MX_33_NOR_ReadStatusRegister(pM2SM);
  MX_33_NOR_ReadCR(pM2SM);
  MX_33_NOR_ReadAllCR2(pM2SM);
  MX_33_NOR_ReadSecurityRegister(pM2SM);
}


void MX_33_NOR_ReadJedec(M2SerMem_t * pM2SM, uint8_t* Bytes) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x9F, 0x00000000, 0, 3, 0 }, // 1bit STR
    { 0x9F60, 0x00000000, 4, 3, 4 }, // 8bit STR
    { 0x9F60, 0x00000000, 4, 4, 6 }  // 8bit DTR Data in STR mode (exception)
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

void MX_33_NOR_ReadSFDP(M2SerMem_t * pM2SM, uint8_t* Bytes) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x5A, 0x00000000, 4, 256, 0 }, // 1bit STR
    { 0x5AA5, 0x00000000, 4, 256, 20 }, // 8bit STR
    { 0x5AA5, 0x00000000, 4, 256, 6 }  // 8bit DTR // 4 or 6 ?
  };
  
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  
  SerialTransaction_t ST = STs[mode];
  
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, Bytes); 
}

void MX_33_NOR_ReadData(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x0C, 0x00000000, 4, 0, 8 }, // 1bit STR
    { 0xEC13, 0x00000000, 4, 0, -1 }, // 8bit STR
    { 0xEE11, 0x00000000, 4, 0, 6 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = Addr;
  ST.DataSizeByte = ByteSize;

  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionRead(pM2SM, pBytes);
}


//////////////////////
void MX_33_NOR_EraseSegment(M2SerMem_t * pM2SM, uint32_t Addr) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x21, 0x00000000, 4, 0, 0 }, // 1bit STR
    { 0x21DE, 0x00000000, 4, 0, 0 }, // 8bit STR
    { 0x21DE, 0x00000000, 4, 0, 0 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = Addr;

  MX_33_NOR_WriteEnable(pM2SM);
  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionCMD(pM2SM);
  
  MX_33_NOR_ReadStatusRegister(pM2SM);
  MX_33_NOR_ReadStatusRegister(pM2SM);
  
  MX_33_NOR_WaitBusyEnds(pM2SM);
  MX_33_NOR_ReadStatusRegister(pM2SM);
  NOPs(1);  
}


void MX_33_NOR_ProgramData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte) {

  const SerialTransaction_t STs[3] = {
    // CMD; Addr; AddrSizeByte; DataSizeByte; DummyCycles;
    {   0x12, 0x00000000, 4, 0, 0 }, // 1bit STR
    { 0x12ED, 0x00000000, 4, 0, 0 }, // 8bit STR
    { 0x12ED, 0x00000000, 4, 0, 0 }  // 8bit DTR
  };
  int8_t mode = pM2SM->MemoryConfigurationMode;
  if(mode>=countof(STs)) while(1);
  SerialTransaction_t ST = STs[mode];
  ST.Addr = Addr;
  ST.DataSizeByte = SizeByte;

  MX_33_NOR_WriteEnable(pM2SM);

  SM_PresetCommandParameters(pM2SM, &ST);
  SM_DoTransactionWrite(pM2SM, DataSegment); // we base from beginning of buffer ?
  MX_33_NOR_WaitBusyEnds(pM2SM);
  
  // use security register to avoid reading back all data to confirm proper writing. speeds things up.
}

//================================= 8>< ------------------------------------------------------------------------

void MX_33_NOR_ReadInfo(M2SerMem_t * pM2SM) {

//  SerialConfiguration_t t;
  MX_33_NOR_ReadStatusRegister(pM2SM);  
  
  switch(pM2SM->MemoryConfigurationMode) {
    
  case 0: // 1bit STR
      MX_33_NOR_ReadCR(pM2SM);
      MX_33_NOR_ReadJedec(pM2SM, JedecInfo);
      MX_33_NOR_ReadSFDP(pM2SM, SFDP);
      MX_33_NOR_ReadData(pM2SM, 0x00000000, DataSegment, 4096);
      MX_33_NOR_ReadAllCR2(pM2SM);
      MX_33_NOR_ReadSecurityRegister(pM2SM);
    break;
    
  case 1: // 8 bit STR
      MX_33_NOR_ReadCR(pM2SM);
      MX_33_NOR_ReadJedec(pM2SM, JedecInfo);
      MX_33_NOR_ReadSFDP(pM2SM, SFDP);
      MX_33_NOR_ReadData(pM2SM, 0x00000000,DataSegment,4096);
      MX_33_NOR_ReadAllCR2(pM2SM);
      MX_33_NOR_ReadSecurityRegister(pM2SM);
    break;
    
  case 2: // 8 bit DDR

      MX_33_NOR_ReadJedec(pM2SM, JedecInfo);
      MX_33_NOR_ReadSFDP(pM2SM, SFDP);
      MX_33_NOR_ReadData(pM2SM, 0x00000000,DataSegment,4096);
      MX_33_NOR_ReadCR(pM2SM);
      MX_33_NOR_ReadData(pM2SM, 0x00000000,DataSegment,4096);
      MX_33_NOR_ReadAllCR2(pM2SM);
      MX_33_NOR_ReadJedec(pM2SM, JedecInfo);
      MX_33_NOR_ReadSFDP(pM2SM, SFDP);

      // Read security register
      MX_33_NOR_ReadSecurityRegister(pM2SM);
    break;
    
  default:
    while(1); // bug
  }
  
}

int8_t MX_33_NOR_GetImpedance(M2SerMem_t * pM2SM) { // to be tested

  MX_33_NOR_ReadCR(pM2SM);
  return pM2SM->Registers[MX25LM_CONTROL_REG_INDEX] & 0x07; // hard coded for now
}

void MX_33_NOR_SetImpedance(M2SerMem_t * pM2SM, int8_t imp) { // to be tested
  
  uint8_t Bytes[2];
  MX_33_NOR_ReadCR(pM2SM);
  Bytes[0] = pM2SM->Registers[MX25LM_CONTROL_REG_INDEX] & ~0x07;
  Bytes[0] |= imp & 0x07;
  MX_33_NOR_WriteCR(pM2SM, Bytes[0]);
}

int32_t MX_33_NOR_GetDummyCycles(M2SerMem_t * pM2SM) { // untested
  
//  uint8_t Bytes[2];
  MX_33_NOR_ReadCR2(pM2SM, 5);
  return pM2SM->Registers[MX25LM_CR2_300] & ~0x07;
}

void MX_33_NOR_SetDummyCycles(M2SerMem_t * pM2SM, int32_t dummycycles) { // untested
  
  uint8_t Bytes[2];
  MX_33_NOR_ReadCR2(pM2SM, 5);
  Bytes[0] = pM2SM->Registers[MX25LM_CONTROL_REG_INDEX] & ~0x07;
  Bytes[0] |= dummycycles & 0x07;
  MX_33_NOR_WriteCR2(pM2SM, 5, Bytes[0]);
}


//========================================================
// Test the driver

static uint8_t Impedance = 6;
static volatile uint8_t test_branch = 1;
static uint8_t Mem_page[32];
void 
MX25LM5145G_Test(void)
{
  // prepare autopoll
  SM_ConfigInit(pM2SM);
   
  SerialMemoryTuneDelay();
  
  //Run the first test in 1 bit mode
  //HAL_XSPI_SetClockPrescaler(&hxspi1, 4);
  MX_33_NOR_ReadInfo(pM2SM);
  MX_33_NOR_WriteCR(pM2SM, Impedance);
//  HAL_XSPI_SetClockPrescaler(&hxspi1, 1);
  MX_33_NOR_ReadInfo(pM2SM);

  switch(test_branch) {
  case 1:
    MX_33_NOR_ReadStatusRegister(pM2SM);
    MX_33_NOR_WriteEnable(pM2SM);
    MX_33_NOR_ReadStatusRegister(pM2SM);
    MX_33_NOR_WriteDisable(pM2SM);
    MX_33_NOR_ReadStatusRegister(pM2SM);
    
    //while(1); // stop here for now
    
    MX_33_NOR_EraseSegment(pM2SM, 0);
    
    DataBufferFill(0,512,0x55,1);
    MX_33_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,1024,0,1);
    MX_33_NOR_ProgramData(pM2SM, 0x00000000, 256);
    DataBufferFill(0,1024,0,0);
    MX_33_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,1024,0x80,0xFF);
    MX_33_NOR_ProgramData(pM2SM, 0x00000100, 16);
    MX_33_NOR_ReadInfo(pM2SM);
    break;
  case 2: // test going to 8 bit SDR mode then back to 1 bit SDR mode
    // going to 8 bit SDR mode
    MX_33_NOR_WriteCR2(pM2SM, 0,0x01); // switch to 8 bit SDR mode
    MX_33_NOR_SetMode(pM2SM, 1);

    // do something
    MX_33_NOR_ReadInfo(pM2SM);  
    MX_33_NOR_EraseSegment(pM2SM, 0x00010000); // second 64 kbyte block
    DataBufferFill(0,1024,0x00,0x11);
    MX_33_NOR_ProgramData(pM2SM, 0x00010000, 16);
    MX_33_NOR_ReadData(pM2SM, 0x00010000, Mem_page, 16);
    
    MX_33_NOR_WriteCR(pM2SM, 0x07);
    MX_33_NOR_ReadInfo(pM2SM);
    
    // going back to 1 bit SDR mode
    MX_33_NOR_WriteCR2(pM2SM, 0,0x00); // switch to 8 bit SDR mode
    MX_33_NOR_SetMode(pM2SM, 0);
    MX_33_NOR_ReadInfo(pM2SM);    
    break;
  case 3: // test going to 8 bit SDR then DDR mode
    // DQS is always enabled in DTR mode
    MX_33_NOR_WriteCR2(pM2SM, 0,0x02); // activate 8 bit DTR mode and reconfigure the HW cell for it
    MX_33_NOR_SetMode(pM2SM, 2); // 8 bit DTR mode with DQS enabled
    // now we are DTR mode now!
    MX_33_NOR_ReadData(pM2SM, 0x00000000, DataSegment, 4096);
    MX_33_NOR_ReadInfo(pM2SM);
    NOPs(1);
    
    break;
  case 4:
    MX_33_NOR_ReadAllCR2(pM2SM);
    MX_33_NOR_WriteCR2(pM2SM, 1,0x02);
    MX_33_NOR_ReadInfo(pM2SM);
    MX_33_NOR_ReadCR2(pM2SM, 1);
    NOPs(1);
    break;
  }
    
  while(1)
    NOPs(1);
}

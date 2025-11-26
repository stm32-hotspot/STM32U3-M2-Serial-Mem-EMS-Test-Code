/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#include "main.h"
#include "SerialMem.h"
#include "M2SM_W25Q128JV.h"
// W25Q128JV Winbond NOR 128 mbit flash memory
// SerialMemory Description Structure
// 590 code line before rewrite

// 1 bit STR, 8 bit STR is working. 8 bit DTR returns de-orderely data. debug time exceeded.

void DataBufferFill(uint32_t adr_start, uint32_t sizebyte, uint8_t value, uint8_t incstep);
uint8_t MX_NOR_ReadStatusRegister(void);
void MX_NOR_WriteEnable(void);
void MX_NOR_WriteDisable(void);
void MX_NOR_WaitBusyEnds(void);

void MX_NOR_ReadInfo(void);
void MX_NOR_EraseSegment(uint32_t Addr);
void MX_NOR_ProgramData(uint32_t Addr, uint32_t SizeByte);

// These contain a memory description from datasheet

SerialMemGlobalInfo_t SerialMemGlobalInfo =
{
  // Names, part number, manufacturer. We support reset, fast SDR, fast DDR, impedence tune
  .CPN = "W25Q16JV",
  .ManufacturerName = "WINBOND",
  .MemoryType = NOR_Type, // SFDP, Flash, Sector erase, Page write model
  .JEDEC_ID = {0x17, 0x40, 0x18 }, // or 0x70 0x18
  .Support_Features = {         
                      .SFDP_Supported = 1,
                      .JEDEC_ID_Supported = 1, 
                      .SDR_1bit_Supported = 1,
                      .SDR_4bit_Supported = 1,
                      .WAITSTATES_Tuning_Supported = 1,
                      .IMPEDANCE_Tuning_Supported = 1,
                      },
  .WaitStateVsMHzVsXDR = { {6, 66, 66}, {8, 84, 84}, {10, 104, 104}, {12, 104, 104}, {14, 133, 133}, {16, 133, 133}, {18, 133, 133}, {20, 133, 133} },
  
  .MaxFreq_MHz = 133,  // 
  .Vmin_V_x100 = 270,
  .Vmax_V_x100 = 360,
  
  // 128Mbit
  .SizekByte = 2048, // -1 not available
  .BlockSizeByte = 1024*64L,   // -1 not available
  .SectorSizeByte = 4096,  // -1 not available
  .PageSizeByte = 256,    // -1 not available
  .ChosenErasableAreaByteSize = 4096, // sector size chosen here
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
  .WAITSTATE_MaskVal = 0,
  
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
XSPI_AutoPollingTypeDef    gConfig;
uint8_t MemoryConfigurationMode = 0;
SerialConfiguration_t MemoryBusMode[4] = {_S1, _S1, _NONE, _S1}; // CMD > ADDR > ALT > DATA
uint32_t DQS_Enabled = HAL_XSPI_DQS_DISABLE;
uint32_t IOSelect = HAL_XSPI_SELECT_IO_7_0;
uint8_t RegisterDummyCycles = 20; // reset value
uint32_t TT; //   SEND_CMD | DO_WRITE | DO_READ | DO_AUTOPOLL

uint8_t JedecInfo[4];
uint8_t ManufDeviceID[2];
uint8_t UniqueID[8];
uint8_t SFDP[256];

uint8_t Mem_SecurityReg[2];
uint8_t Mem_SR[3]; // 3 status registers


void MX_NOR_SetMode(uint8_t newmode);
void MX_NOR_SetMode(uint8_t newmode) {
  
  switch(newmode) {
    
  case 0: // 1 bit SDR
    MemoryBusMode[0] = MemoryBusMode[1] = MemoryBusMode [3] = _S1;
    MemoryBusMode[2] = _NONE;
    MemoryConfigurationMode = 0;
    break;
  case 1: // 4 bit Quad IO only for DATA
    MemoryBusMode[0] = MemoryBusMode[1] = _S1;
    MemoryBusMode [3] = _S4;
    MemoryBusMode[2] = _NONE;
    MemoryConfigurationMode = 1;
    break;
  case 2: // 4 bit Quad IO for ADDRESS AND DATA
    MemoryBusMode[0] = _S1;
    MemoryBusMode[1] = MemoryBusMode [3] = _S4;
    MemoryBusMode[2] = _NONE;
    MemoryConfigurationMode = 2;
    break;
  default:
    while(1);
  }
  
}


// Needed function by memory
//==============================================================================
// Here we implement the generic functions that we need to operate properly the serial memory, not all the features...

//void ConfigInit(XSPI_AutoPollingTypeDef* pConfig, uint8_t preset);

extern XSPI_HandleTypeDef hxspi1;

uint8_t MX_NOR_ReadStatusRegister(void) {

  SerialTransaction_t ST;
  ST.TT = DO_READ;  
  ST.Addr = 0;
  ST.AddrSizeByte = 0;
  ST.DataSizeByte = 1;
  ST.CMD = 0x05;
  
  switch(MemoryConfigurationMode) {
    
  case 0:
  SM_PresetCommandParameters(&gCommand, &ST);//TT, 0x05, 0, 1, 0); // READ SR
  SM_DoTransaction(&hxspi1, &ST, Mem_SR);
  break;
  
  default: while(1);
  }
  
  return Mem_SR[0];
}

void MX_NOR_WriteEnable(void) {
  
  SerialTransaction_t ST;
  ST.TT = 0;
  ST.CMD = 0x06;
  ST.Addr = 0x00000000;
  ST.AddrSizeByte = 0;
  ST.DataSizeByte = 0;
  ST.DummyCycles = 0;
  
  switch(MemoryConfigurationMode) {
  case 0:
    SM_PresetCommandParameters(&gCommand, &ST);//, 0x06, 0, 0, 0); // WREN
    SM_DoTransaction(&hxspi1, &ST, DataSegment);
    break;
  default: while(1);
  }
  
  MX_NOR_ReadStatusRegister();// bonus for debug
}

void MX_NOR_WriteDisable(void) {

  SerialTransaction_t ST;
  ST.TT = 0;
  ST.CMD = 0x04;
  ST.Addr = 0x00000000;
  ST.AddrSizeByte = 0;
  ST.DataSizeByte = 0;
  ST.DummyCycles = 0;
  
  switch(MemoryConfigurationMode) {
  case 0:
    SM_PresetCommandParameters(&gCommand, &ST);//, 0x04, 0, 0, 0); //
    SM_DoTransaction(&hxspi1, &ST, DataSegment);
  break;
  default: while(1);
  }
  
  MX_NOR_ReadStatusRegister();// bonus for debug
}

void MX_NOR_WaitBusyEnds(void) {

  SerialTransaction_t ST;
  ST.TT = DO_AUTOPOLL;
  ST.CMD = 0x05;
  ST.Addr = 0;
  ST.AddrSizeByte = 0;
  ST.DataSizeByte = 1;
  ST.DummyCycles = 0;
  
  switch(MemoryConfigurationMode) {
    
  case 0:
    SM_PresetCommandParameters(&gCommand, &ST);//, 0x05, 0, 1, 0); // READ SR
    SM_DoTransaction(&hxspi1, &ST, Mem_SR);
    break;
  default: while(1);
  }
  
  MX_NOR_ReadStatusRegister();// bonus for debug
}

void MX_NOR_ReadData(uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);
void MX_NOR_ReadData(uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize) {

  SerialTransaction_t ST;
  ST.TT = DO_READ;
  ST.CMD = 0x0B;
  ST.Addr = Addr;
  ST.AddrSizeByte = 3;
  ST.DataSizeByte = ByteSize;
  ST.DummyCycles = 8;
  
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hxspi1, &ST, pBytes);
    break;
    
  case 1: // 1 bit command and address, followed by 4 bit data mode
      ST.CMD = 0x6B;
      SM_PresetCommandParameters(&gCommand, &ST); 
      SM_DoTransaction(&hxspi1, &ST, pBytes);
    break;
    
  case 2: // 1 bit command and 4 bit address AND data
      ST.CMD = 0xEB;
      ST.DummyCycles = 4;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hxspi1, &ST, pBytes);
    break;
    
  default:
    while(1); // bug
  }
  
}


void MX_NOR_ReadInfo(void) {

  SerialTransaction_t ST;
  ST.TT = DO_READ;
  ST.CMD = 0x00;
  ST.Addr = 0;
  ST.AddrSizeByte = 4;
  ST.DataSizeByte = 1;
  ST.DummyCycles = 0;
  
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
      MX_NOR_ReadStatusRegister();
      
      ST.CMD = 0x35;
      ST.Addr = 0;
      ST.AddrSizeByte = 0;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hxspi1, &ST, &Mem_SR[1]);

      ST.CMD = 0x15;
      ST.Addr = 0;
      ST.AddrSizeByte = 0;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hxspi1, &ST, &Mem_SR[2]);

      ST.CMD = 0x90;
      ST.Addr = 0;
      ST.AddrSizeByte = 3;
      ST.DataSizeByte = 2;
      ST.DummyCycles = 0;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hxspi1, &ST, ManufDeviceID);

      ST.CMD = 0x4B;
      ST.Addr = 0;
      ST.AddrSizeByte = 4;
      ST.DataSizeByte = 8;
      //ST.DummyCycles = 16;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hxspi1, &ST, UniqueID);
      
      
      ST.CMD = 0x9F;
      ST.Addr = 0;
      ST.AddrSizeByte = 0;
      ST.DataSizeByte = 3;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hxspi1, &ST, JedecInfo);
      
      ST.CMD = 0x5A;
      ST.Addr = 0;
      ST.AddrSizeByte = 4;
      ST.DataSizeByte = 256;
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x5A, 0, 256, 0);
      SM_DoTransaction(&hxspi1, &ST, SFDP);
      
      MX_NOR_ReadData(0x00000000, DataSegment, 4096);

      // Read security register
      ST.CMD = 0x48;
      ST.Addr = 0;
      ST.AddrSizeByte = 3;      
      ST.DataSizeByte = 1;
      ST.DummyCycles = 8;
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x2B, 0, 1, 0); // READ Security register
      SM_DoTransaction(&hxspi1, &ST, Mem_SecurityReg);      
    break;
    
  case 1: // 8 bit STR
      MX_NOR_ReadData(0x00000000, DataSegment, 4096);
    break;
    
  case 2: // 8 bit DDR
      MX_NOR_ReadData(0x00000000, DataSegment, 4096);
    break;
    
  default:
    while(1); // bug
  }
  
}

//////////////////////
void MX_NOR_EraseSegment(uint32_t Addr) {

  SerialTransaction_t ST;
  ST.TT = 0;
  ST.CMD = 0x20;
  ST.Addr = Addr;
  ST.AddrSizeByte = 3;
  ST.DataSizeByte = 0;
  ST.DummyCycles = 0;
  
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
      MX_NOR_WriteEnable();
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x21, Addr, 0, 0); // SECTOR ERASE
      SM_DoTransaction(&hxspi1, &ST, 0);
      
      MX_NOR_ReadStatusRegister();
      MX_NOR_ReadStatusRegister();
      
      MX_NOR_WaitBusyEnds();
      NOPs(1);
    break;
    
  default:
    while(1); // bug
  }
  
}


void MX_NOR_ProgramData(uint32_t Addr, uint32_t SizeByte) {

  SerialTransaction_t ST;
  ST.TT = DO_WRITE;
  ST.CMD = 0x02;
  ST.Addr = Addr;
  ST.AddrSizeByte = 3;
  ST.DataSizeByte = SizeByte;
  ST.DummyCycles = 0;

  // we should check the program is valid... for now we beleive it is 
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
      MX_NOR_WriteEnable();
      // 256 max page
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x12, Addr, SizeByte, 0); // PAGE PROGRAM
      SM_DoTransaction(&hxspi1, &ST, DataSegment); // we base from beginning of buffer ?
      MX_NOR_WaitBusyEnds();
    break;

  case 1: // 1bit STR // DO NOT USE THIS BECAUSE YOU WILL NOT SAVE MUCH PERF AND THE READ STATUS REGISTER SHOULD BE HARDCODED HERE IN S1-S1-S1 mode
      while(1); // requires more coding in the wait busy end (change addressing mode on the fly and restore)
      MX_NOR_WriteEnable();
      ST.CMD = 0x32;
      // 256 max page
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x12, Addr, SizeByte, 0); // PAGE PROGRAM
      SM_DoTransaction(&hxspi1, &ST, DataSegment); // we base from beginning of buffer ?
      MX_NOR_WaitBusyEnds();
    break;
    
  default:
    while(1); // bug
  }
  
  // use security register to avoid reading back all data to confirm proper writing. speeds things up.
}

//========================================================
uint8_t SRCR[2];
void MX_NOR_WriteSR1(uint8_t Byte);
void MX_NOR_WriteSR1(uint8_t Byte) {
/*  
  SerialTransaction_t ST;
  ST.TT = DO_WRITE;
  ST.CMD = 0x00;
  ST.Addr = 0;
  ST.AddrSizeByte = 0;
  ST.DataSizeByte = 1;
  ST.DummyCycles = 0;
  uint8_t Bytes[2];
  
  
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
    // READ SR
    MX_NOR_ReadStatusRegister();
      SRCR[0] = Mem_SR[0]; // SR
      SRCR[1] = Byte; // new CR value
    // WREN
      MX_NOR_WriteEnable();
    // WRITE SR+CR  
      ST.CMD = 0x01;
      ST.DataSizeByte = 2;
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x01, 0, 2, 0); // WRITE SR+CR
      SM_DoTransaction(&hxspi1, &ST, SRCR); // we base from beginning of buffer ?
    // AUTOPOLL
      MX_NOR_WaitBusyEnds();
    break;
    
  default:
    while(1); // bug
  }
*/  
}

uint8_t Mem_CR2Dual[2];
void MX_NOR_WriteSR2(uint8_t index);
void MX_NOR_WriteSR2(uint8_t index) {
/*  
  SerialTransaction_t ST;
  ST.TT = DO_READ;
  ST.CMD = 0x00;
  ST.Addr = CR2_Adresses[index];
  ST.AddrSizeByte = 4;
  ST.DataSizeByte = 1;
  ST.DummyCycles = 0;
  
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
    ST.CMD = 0x71;
    SM_PresetCommandParameters(&gCommand, &ST);//, 0x71, CR2_Adresses[index], 1, 0);
    SM_DoTransaction(&hxspi1, &ST, Mem_CR2Dual); //
    Mem_CR2[index] = Mem_CR2Dual[0];
    break;
    
  default:
    while(1); // bug
  }
  */
}



void MX_NOR_WriteSR3(uint8_t byteval);
void MX_NOR_WriteSR3(uint8_t byteval) {
/*
  SerialTransaction_t ST;
  ST.TT = DO_WRITE;
  ST.CMD = 0x00;
  ST.Addr = CR2_Adresses[index];
  ST.AddrSizeByte = 4;
  ST.DataSizeByte = 1;
  ST.DummyCycles = 0;
  
  Mem_CR2Dual[0] = Mem_CR2Dual[1] = byteval;
  
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
    MX_NOR_WriteEnable();
    ST.CMD = 0x72;
    SM_PresetCommandParameters(&gCommand, &ST);//, 0x72, CR2_Adresses[index], 1, 0); // WRITE CR2
    SM_DoTransaction(&hxspi1, &ST, Mem_CR2Dual); 
    break;
    
  default:
    while(1); // bug
  }
*/  
}

//========================================================
// Test the driver

uint8_t Impedance = 6;
volatile uint8_t test_branch = 1;
uint8_t Mem_page[32];
void 
MX25LM5145G_Test(void)
{
  // prepare autopoll
  SM_ConfigInit(&gConfig, 0);
//  SM_PresetCommandParameters(&gCommand, TT, 0x05, 0, 1, 0); // READ SR 
  
  //Run the first test in 1 bit mode
  HAL_XSPI_SetClockPrescaler(&hxspi1, 4);
  MX_NOR_ReadInfo();
  //MX_NOR_WriteCR(Impedance);
//  HAL_XSPI_SetClockPrescaler(&hxspi1, 1);
  MX_NOR_ReadInfo();

  switch(test_branch) {
  case 1:
    MX_NOR_ReadStatusRegister();
    MX_NOR_WriteEnable();
    MX_NOR_ReadStatusRegister();
    MX_NOR_WriteDisable();
    MX_NOR_ReadStatusRegister();
    
    //while(1); // stop here for now
    
    MX_NOR_EraseSegment(0);
    
    DataBufferFill(0,512,0x55,1);
    MX_NOR_ReadInfo();
    DataBufferFill(0,1024,0,1);
    MX_NOR_ProgramData(0x00000000, 256);
    DataBufferFill(0,1024,0,0);
    MX_NOR_ReadInfo();
    DataBufferFill(0,1024,0x80,0xFF);
    MX_NOR_ProgramData(0x00000100, 16);
    MX_NOR_ReadInfo();
    break;
  case 2: // test going to 8 bit SDR mode then back to 1 bit SDR mode
    // going to 8 bit SDR mode
    //MX_NOR_WriteCR2(0,0x01); // switch to 8 bit SDR mode
    MX_NOR_SetMode(1);
    MX_NOR_ReadInfo();
//    DataBufferFill(0x200,16,0x55,0x10);
//    MX_NOR_ProgramData(0x00000200, 16);
//    MX_NOR_ReadInfo();
    MX_NOR_SetMode(2);
    MX_NOR_ReadInfo();
    
    MX_NOR_SetMode(0);
    MX_NOR_ReadInfo();    
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

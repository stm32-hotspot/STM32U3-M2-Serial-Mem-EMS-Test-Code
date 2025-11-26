/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#include "main.h"
#include "SerialMem.h"
// APS6404L-SQN 1.8V QSPI PSRAM 64 Mbit, 8Mbyte, page 1024 bytes
// SerialMemory Description Structure


// 1 bit STR, 8 bit STR is working. 8 bit DTR returns de-orderely data. debug time exceeded.

void DataBufferFill(uint32_t adr_start, uint32_t sizebyte, uint8_t value, uint8_t incstep);
uint8_t MX_NOR_ReadStatusRegister(void);
void MX_NOR_WriteEnable(void);
void MX_NOR_WriteDisable(void);
void MX_NOR_WaitBusyEnds(void);

void MX_NOR_ReadInfo(void);
void MX_NOR_EraseSegment(uint32_t Addr);
void MX_NOR_ProgramData(uint32_t Addr, uint32_t SizeByte);
//void MX_NOR_WriteCR(uint8_t Byte);
//void MX_NOR_ReadCR2(uint8_t index);
//void MX_NOR_WriteCR2(uint8_t index, uint8_t Value);

// These contain a memory description from datasheet

SerialMemGlobalInfo_t SerialMemGlobalInfo =
{
  // Names, part number, manufacturer. We support reset, fast SDR, fast DDR, impedence tune
  .CPN = "APS6404L-SQN",
  .ManufacturerName = "AP MEMORY",
  .MemoryType = RAM_Type, // SFDP, Flash, Sector erase, Page write model
  .JEDEC_ID = {0x17, 0x40, 0x18 }, // or 0x70 0x18
  .Wirering = WIRED_TO_DATA0_7, // 8 bit data bus wired
  .Support_Features = {         
                      .SFDP_Supported = 1,
                      .JEDEC_ID_Supported = 1, 
                      .SDR_1bit_Supported = 1,
                      .SDR_4bit_Supported = 1,
                      .WAITSTATES_Tuning_Supported = 1,
                      .IMPEDANCE_Tuning_Supported = 1,
                      },
  .WaitStateVsMHzVsXDR = { {6, 66, 66}, {8, 84, 84}, {10, 104, 104}, {12, 104, 104}, {14, 133, 133}, {16, 133, 133}, {18, 133, 133}, {20, 133, 133} },
  
  .MaxFreq_MHz = { 144, 0, 0, 0 },       // 
  .Vmin_V_x100 = 270,
  .Vmid_V_x100 = 0,   // support 2 voltage ranges with different speed support
  .Vmax_V_x100 = 360,
  
  // 128Mbit
  .SizekByte = 4096, // -1 not available
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
  },
  
  // put here wait states table
  .SR_WEL_MaskVal = 0x02, // set 1 to activate latch
  .SR_WIP_MaskVal = 0x01, // set 1 when memory is busy
  .SDR1BIT_MaskVal = 0,
  .SRDWideBIT_MaskVal = 0,
  .DDR_MaskVal = 0,
  .IMPEDENCE_MaskVal = 0,
  .WAITSTATE_MaskVal = 0,
  .AUTOPOLL_Interval = 0,
  .CR2_DUMMYCYCLES_LOHIMHz = 0,
  
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
uint8_t Mem_SR[1]; // status register
uint8_t Mem_FR[1]; // function register
uint8_t Mem_RR[1]; // read register
uint8_t Mem_ERR[1]; // extended read register
uint8_t Mem_ABR[1]; // auto boot register

void MX_NOR_SetMode(uint8_t newmode);
void MX_NOR_SetMode(uint8_t newmode) {
  
  switch(newmode) {
    
  case 0: // 1 bit SDR
    MemoryBusMode[0] = MemoryBusMode[1] = MemoryBusMode [3] = _S1;
    MemoryBusMode[2] = _NONE;
    MemoryConfigurationMode = 0;
    break;
  case 1: // 4 bit SDR QPI
    MemoryBusMode[0] = MemoryBusMode[1] = MemoryBusMode [3] = _S4;
    MemoryBusMode[2] = _NONE;
    MemoryConfigurationMode = 1;
    break;
  default:
    while(1);
  }
  
}


// Needed function by memory
//==============================================================================
// Here we implement the generic functions that we need to operate properly the serial memory, not all the features...

//void SM_PresetCommandParameters(XSPI_RegularCmdTypeDef* pCommand, uint32_t TT, uint16_t CMD, void* pMem, uint32_t Addr, uint32_t DataSizeByte, int8_t DummyCycles);
void ConfigInit(XSPI_AutoPollingTypeDef* pConfig, uint8_t preset);


//=== First, we try to read information from the memory
void MX_NOR_DefaultBootMode(void);

extern XSPI_HandleTypeDef hxspi1;
#define hospi1 hxspi1


uint8_t MX_NOR_ReadStatusRegister(void) {
  while(1);
}

uint8_t MX_NOR_ReadFunctionRegister(void);
uint8_t MX_NOR_ReadFunctionRegister(void) {
  while(1);
}

uint8_t MX_NOR_ReadParametersRegister(void);
uint8_t MX_NOR_ReadParametersRegister(void) {
  while(1);
}


uint8_t MX_NOR_ReaExtendeddParametersRegister(void);
uint8_t MX_NOR_ReaExtendeddParametersRegister(void) {
  while(1);
}


void MX_NOR_EnterQPI_Mode(void) {
  
  SerialTransaction_t ST;
  ST.TT = 0;
  ST.CMD = 0x35;
  ST.Addr = 0x00000000;
  ST.AddrSizeByte = 0;
  ST.DataSizeByte = 0;
  ST.DummyCycles = 0;
  
  switch(MemoryConfigurationMode) {
  case 0:
    SM_PresetCommandParameters(&gCommand, &ST);
    SM_DoTransaction(&hospi1, &ST, DataSegment);
    break;
  default: while(1);
  }
  
  MX_NOR_ReadStatusRegister();// bonus for debug
}

void MX_NOR_DisableQPI_Mode(void) {
  
  SerialTransaction_t ST;
  ST.TT = 0;
  ST.CMD = 0xF5;
  ST.Addr = 0x00000000;
  ST.AddrSizeByte = 0;
  ST.DataSizeByte = 0;
  ST.DummyCycles = 0;
  
  switch(MemoryConfigurationMode) {
  case 1:
    SM_PresetCommandParameters(&gCommand, &ST);
    SM_DoTransaction(&hospi1, &ST, DataSegment);
    break;
  default: while(1);
  }
  
  MX_NOR_ReadStatusRegister();// bonus for debug
}



void MX_NOR_WriteEnable(void) {

  while(1);
}

void MX_NOR_WriteDisable(void) {

  while(1);
}

void MX_NOR_WaitBusyEnds(void) {

  while(1);
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
      SM_DoTransaction(&hospi1, &ST, pBytes);
    break;
    
  case 1: // QPI 4 bit
      ST.CMD = 0xEB;
      ST.DummyCycles = 6;
      SM_PresetCommandParameters(&gCommand, &ST); 
      SM_DoTransaction(&hospi1, &ST, pBytes);
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
      uint8_t MX_NOR_ReadFunctionRegister();
/*      
      ST.CMD = 0x35;
      ST.Addr = 0;
      ST.AddrSizeByte = 0;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hospi1, &ST, &Mem_SR[1]);

      ST.CMD = 0x15;
      ST.Addr = 0;
      ST.AddrSizeByte = 0;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hospi1, &ST, &Mem_SR[2]);

      ST.CMD = 0x90;
      ST.Addr = 0;
      ST.AddrSizeByte = 0;
      ST.DataSizeByte = 3;
      //ST.DummyCycles = 16;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hospi1, &ST, ManufDeviceID);
*/
      
      ST.CMD = 0xAB; // Read ID
      ST.Addr = 0;
      ST.AddrSizeByte = 3;
      ST.DataSizeByte = 8;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hospi1, &ST, UniqueID);

      ST.CMD = 0x9F; // Read JEDEC
      ST.Addr = 0;
      ST.AddrSizeByte = 0;
      ST.DataSizeByte = 3;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hospi1, &ST, JedecInfo);
      
      ST.CMD = 0x5A;
      ST.Addr = 0;
      ST.AddrSizeByte = 4;
      ST.DataSizeByte = 256;
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x5A, 0, 256, 0);
      SM_DoTransaction(&hospi1, &ST, SFDP);
      
      MX_NOR_ReadData(0x00000000, DataSegment, 4096);
/*
      // Read security register
      ST.CMD = 0x48;
      ST.Addr = 0;
      ST.AddrSizeByte = 3;      
      ST.DataSizeByte = 1;
      ST.DummyCycles = 8;
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x2B, 0, 1, 0); // READ Security register
      SM_DoTransaction(&hospi1, &ST, Mem_SecurityReg);      
*/
    break;
    
  case 1: // 8 bit STR
      MX_NOR_ReadStatusRegister();
      
      ST.CMD = 0x35;
      ST.Addr = 0;
      ST.AddrSizeByte = 0;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hospi1, &ST, &Mem_SR[1]);

      ST.CMD = 0x15;
      ST.Addr = 0;
      ST.AddrSizeByte = 0;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hospi1, &ST, &Mem_SR[2]);

      ST.CMD = 0x90;
      ST.Addr = 0;
      ST.AddrSizeByte = 0;
      ST.DataSizeByte = 3;
      //ST.DummyCycles = 16;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hospi1, &ST, ManufDeviceID);

      
      ST.CMD = 0x9F;
      ST.Addr = 0;
      ST.AddrSizeByte = 0;
      ST.DataSizeByte = 3;
      SM_PresetCommandParameters(&gCommand, &ST);
      SM_DoTransaction(&hospi1, &ST, JedecInfo);
      
      ST.CMD = 0x5A;
      ST.Addr = 0;
      ST.AddrSizeByte = 4;
      ST.DataSizeByte = 256;
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x5A, 0, 256, 0);
      SM_DoTransaction(&hospi1, &ST, SFDP);
      
      MX_NOR_ReadData(0x00000000, DataSegment, 4096);

      // Read security register
      ST.CMD = 0x48;
      ST.Addr = 0;
      ST.AddrSizeByte = 3;      
      ST.DataSizeByte = 1;
      ST.DummyCycles = 8;
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x2B, 0, 1, 0); // READ Security register
      SM_DoTransaction(&hospi1, &ST, Mem_SecurityReg);      
    break;
    
  default:
    while(1); // bug
  }
  
}

//////////////////////
void MX_NOR_EraseSegment(uint32_t Addr) {

  while(1);
}


void MX_NOR_ProgramData(uint32_t Addr, uint32_t SizeByte) {

  SerialTransaction_t ST;
  ST.TT = DO_WRITE;
  ST.CMD = 0x02;
  ST.Addr = Addr;
  ST.AddrSizeByte = 3;
  ST.DataSizeByte = SizeByte;
  ST.DummyCycles = 0;
  
  // wrap limit?

  // we should check the program is valid... for now we beleive it is 
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
      // 256 max page
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x12, Addr, SizeByte, 0); // PAGE PROGRAM
      SM_DoTransaction(&hospi1, &ST, DataSegment); // we base from beginning of buffer ?
    break;

  case 1: //
      ST.CMD = 0x38;
      SM_PresetCommandParameters(&gCommand, &ST);//, 0x12, Addr, SizeByte, 0); // PAGE PROGRAM
      SM_DoTransaction(&hospi1, &ST, DataSegment); // we base from beginning of buffer ?
    break;
    
  default:
    while(1); // bug
  }
  
  // use security register to avoid reading back all data to confirm proper writing. speeds things up.
}

//========================================================

void MX_NOR_WriteSR1(uint8_t Byte);
void MX_NOR_WriteSR1(uint8_t Byte) {

  while(1);
}

void MX_NOR_WriteSR2(uint8_t Byte);
void MX_NOR_WriteSR2(uint8_t Byte) { 
  // function register
  while(1);
}



void MX_NOR_WriteSR3(uint8_t Byte);
void MX_NOR_WriteSR3(uint8_t Byte) {
// Read parameters register
  while(1);
}

void MX_NOR_WriteSR4(uint8_t Byte);
void MX_NOR_WriteSR4(uint8_t Byte) {
// Write Extended Read parameter register
  while(1);
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
  HAL_XSPI_SetClockPrescaler(&hospi1, 4);
  MX_NOR_ReadInfo();
  //MX_NOR_WriteCR(Impedance);
  HAL_XSPI_SetClockPrescaler(&hospi1, 1);
  MX_NOR_ReadInfo();
  MX_NOR_ReadFunctionRegister();
  MX_NOR_ReadParametersRegister();
  MX_NOR_ReaExtendeddParametersRegister();

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
    MX_NOR_EnterQPI_Mode();
    MX_NOR_SetMode(1);
    DataBufferFill(0,1024,0,0);
    MX_NOR_ReadData(0x00000000, DataSegment, 4096);
    
    MX_NOR_SetMode(2);
    DataBufferFill(0,1024,0,0);
    MX_NOR_ReadData(0x00000000, DataSegment, 4096);
    
    
    MX_NOR_DisableQPI_Mode();
    MX_NOR_SetMode(0);
    // do something
    MX_NOR_WriteSR3(0x03); // change the burst length we don't use
    MX_NOR_WriteSR4(0xB0); // change impedence
    MX_NOR_ReadFunctionRegister();
    MX_NOR_ReadParametersRegister();
    MX_NOR_ReaExtendeddParametersRegister();
    
    //MX_NOR_WriteCR(0x07);
    MX_NOR_ReadInfo();
    
    // going back to 1 bit SDR mode
    //MX_NOR_WriteCR2(0,0x00); // switch to 8 bit SDR mode
    
    MX_NOR_ReadInfo();    
    break;
  case 3: // test going to 8 bit SDR then DDR mode
//    MX_NOR_WriteCR2(1,0x02); // activate DQS without using it (we can be 1 bit or 8 bit STR mode at this point)
    // DQS is always enabled in DTR mode
    //MX_NOR_WriteCR2(0,0x02); // activate 8 bit DTR mode and reconfigure the HW cell for it
    MX_NOR_SetMode(2); // 8 bit DTR mode without DQS for now
    OCTOSPI1->TCR &= ~(1<<30); // disable 1/2 clock shift sampling (does not work if prescale = 0)
    OCTOSPI1->TCR |= (1<<28); // enable 1/4 clock shift sampling (does not work if prescale = 0)
    // now we are DTR mode now!
    DQS_Enabled = 0;
    MX_NOR_ReadData(0x00000000, DataSegment, 4096);
    MX_NOR_ReadInfo();
    NOPs(1);
    
    break;
  case 4:
    //MX_NOR_WriteCR2(1,0x02);
    MX_NOR_ReadInfo();
    //MX_NOR_ReadCR2(1);
    NOPs(1);
    break;
  }
    
  while(1)
    NOPs(1);
  
}

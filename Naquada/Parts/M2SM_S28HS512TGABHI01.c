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

void DataBufferFill(uint32_t adr_start, uint32_t sizebyte, uint8_t value, uint8_t incstep);
uint8_t MX_NOR_ReadStatusRegister(void);
void MX_NOR_WriteEnable(void);
void MX_NOR_WriteDisable(void);
void MX_NOR_WaitBusyEnds(void);

void MX_NOR_ReadInfo(void);
void MX_NOR_EraseSegment(uint32_t Addr);
void MX_NOR_ProgramData(uint32_t Addr, uint32_t SizeByte);
void MX_NOR_WriteCR(uint8_t Byte);
void MX_NOR_ReadCR2(uint8_t index);
void MX_NOR_WriteCR2(uint8_t index, uint8_t Value);

// These contain a memory description from datasheet

SerialMemGlobalInfo_t SerialMemGlobalInfo =
{
  // Names, part number, manufacturer. We support reset, fast SDR, fast DDR, impedence tune
  .CPN = "MX25LM5145G",
  .ManufacturerName = "MACRONIX",
  .MemoryType = NOR_Type, // SFDP, Flash, Sector erase, Page write model
  .JEDEC_ID = {0xC2, 0x85, 0x3A },
  .Wirering = WIRED_TO_DATA0_7, // 8 bit data bus wired
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
  
  .MaxFreq_MHz = { 133, 0, 0, 0 },       // 
  .Vmin_V_x100 = 270,
  .Vmid_V_x100 = 0,   // support 2 voltage ranges with different speed support
  .Vmax_V_x100 = 360,
  
  
  .SizekByte = 65536, // -1 not available
  .BlockSizeByte = 1024*64L,   // -1 not available
  .SectorSizeByte = 2048,  // -1 not available
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
SerialConfiguration_t MemoryBusMode[4] = {_S1, _S1, _S1, _S1}; //,_S8,_D8};
uint32_t DQS_Enabled = HAL_XSPI_DQS_DISABLE;
uint32_t IOSelect = HAL_XSPI_SELECT_IO_7_0;
uint8_t RegisterDummyCycles = 20; // reset value
uint32_t TT; //   SEND_CMD | DO_WRITE | DO_READ | DO_AUTOPOLL

uint8_t JedecInfo[3];
uint8_t SFDP[256];
//uint8_t DataBuffer[4096];
uint8_t Mem_SR[2],Mem_CR[2],Mem_CR2[16]; // 2 for DDR extra wasted byte ? Or odd length is ok ?
uint8_t Mem_SecurityReg[2];


void MX_NOR_SetMode(uint8_t newmode);
void MX_NOR_SetMode(uint8_t newmode) {
  
  switch(newmode) {
    
  case 0: // 1 bit SDR
    MemoryBusMode[0] = MemoryBusMode[1] = MemoryBusMode[2] = MemoryBusMode [3] = _S1;
    MemoryConfigurationMode = 0;
    break;
  case 1: // 8 bit SDR
    MemoryBusMode[0] = MemoryBusMode[1] = MemoryBusMode[2] = MemoryBusMode [3] = _S8;
    MemoryConfigurationMode = 1;
    break;
  case 2: // 8 bit DDR
    MemoryBusMode[0] = MemoryBusMode[1] = MemoryBusMode[2] = MemoryBusMode [3] = _D8;
    MemoryConfigurationMode = 2;
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
  
  switch(MemoryConfigurationMode) {
    
  case 0:
  TT = DO_READ | NO_ALT | NO_ADR;
  SM_PresetCommandParameters(&gCommand, TT, 0x05, 0, 1, 0); // READ SR
  SM_DoTransaction(&hospi1, TT, Mem_SR);
  break;
  
  case 1:
  case 2:
  TT = DO_READ | NO_ALT | NO_ADR;
  SM_PresetCommandParameters(&gCommand, TT, 0x05FA, 0, 1, 4); //+ 4 WS // READ SR
  SM_DoTransaction(&hospi1, TT, Mem_SR);
    break;
  default: while(1);
  }
  
  return Mem_SR[0];
}

void MX_NOR_WriteEnable(void) {
  switch(MemoryConfigurationMode) {
    
  case 0:
    TT = NO_DAT | NO_ADR | NO_ALT;  
    SM_PresetCommandParameters(&gCommand, TT, 0x06, 0, 0, 0); // WREN
    SM_DoTransaction(&hospi1, TT, DataSegment);
    break;
  case 1:
  case 2:
    TT = NO_DAT | NO_ADR | NO_ALT;  
    SM_PresetCommandParameters(&gCommand, TT, 0x06F9, 0, 0, 0); // WREN
    SM_DoTransaction(&hospi1, TT, DataSegment);
    break;
  default: while(1);
  }
  
  MX_NOR_ReadStatusRegister();// bonus for debug
}

void MX_NOR_WriteDisable(void) {
  switch(MemoryConfigurationMode) {
    
  case 0:
    TT = NO_DAT | NO_ADR | NO_ALT;  
    SM_PresetCommandParameters(&gCommand, TT, 0x04, 0, 0, 0); //
    SM_DoTransaction(&hospi1, TT, DataSegment);
  break;
  case 1:
  case 2:
    TT = NO_DAT | NO_ADR | NO_ALT;  
    SM_PresetCommandParameters(&gCommand, TT, 0x04FB, 0, 0, 0); //
    SM_DoTransaction(&hospi1, TT, DataSegment);
    break;
  default: while(1);
  }
  
  MX_NOR_ReadStatusRegister();// bonus for debug
}

void MX_NOR_WaitBusyEnds(void) {
  
  switch(MemoryConfigurationMode) {
    
  case 0:
    TT = NO_ALT | NO_ADR | DO_AUTOPOLL; // you can't manually read if you use AUTOPOLL
    SM_PresetCommandParameters(&gCommand, TT, 0x05, 0, 1, 0); // READ SR
    SM_DoTransaction(&hospi1, TT, Mem_SR);
  break;
  
  case 1:
    TT = NO_ALT | NO_ADR | DO_AUTOPOLL; // you can't manually read if you use AUTOPOLL
    SM_PresetCommandParameters(&gCommand, TT, 0x05FA, 0, 1, 4); //+ 4WS // READ SR // DDR read 1 or 2 bytes ?
    SM_DoTransaction(&hospi1, TT, Mem_SR);
    break;
  case 2:
    TT = NO_ALT | NO_ADR | DO_AUTOPOLL; // you can't manually read if you use AUTOPOLL
    SM_PresetCommandParameters(&gCommand, TT, 0x05FA, 0, 1, 4); //+ 4WS // READ SR // DDR read 1 or 2 bytes ?
    SM_DoTransaction(&hospi1, TT, Mem_SR);
    break;
  default: while(1);
  }
  
  MX_NOR_ReadStatusRegister();// bonus for debug
}

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

void MX_NOR_ReadData(uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);
void MX_NOR_ReadData(uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize) {
 
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
      TT = DO_READ | NO_ALT;    
      SM_PresetCommandParameters(&gCommand, TT, 0x0C, Addr, ByteSize, 8); // FAST_READ4B
      SM_DoTransaction(&hospi1, TT, pBytes);
    break;
    
  case 1: // 8 bit STR
      TT = DO_READ | NO_ALT;    
      SM_PresetCommandParameters(&gCommand, TT, 0xEC13, Addr, ByteSize, -1); // 8READ 
      SM_DoTransaction(&hospi1, TT, pBytes);
    break;
    
  case 2: // 8 bit DDR
      TT = DO_READ | NO_ALT;
      SM_PresetCommandParameters(&gCommand, TT, 0xEE11, Addr, ByteSize, -1); // 8DTRD
      SM_DoTransaction(&hospi1, TT, pBytes);
    break;
    
  default:
    while(1); // bug
  }
  
}


void MX_NOR_ReadInfo(void) {

  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
      MX_NOR_ReadStatusRegister();
      
      TT = DO_READ | NO_ALT | NO_ADR;
      SM_PresetCommandParameters(&gCommand, TT, 0x15, 0, 1, 0); // READ CR
      SM_DoTransaction(&hospi1, TT, Mem_CR);
      
      TT = DO_READ | NO_ALT | NO_ADR;
      SM_PresetCommandParameters(&gCommand, TT, 0x9F, 0, 3, 0);
      SM_DoTransaction(&hospi1, TT, JedecInfo);
      TT = DO_READ | NO_ALT ;
      SM_PresetCommandParameters(&gCommand, TT, 0x5A, 0, 256, 0);
      SM_DoTransaction(&hospi1, TT, SFDP);
      
      MX_NOR_ReadData(0x00000000, DataSegment, 4096);

      TT = DO_READ | NO_ALT ;      
      for(int i=0;i<countof(CR2_Adresses);i++) {
        MX_NOR_ReadCR2(i);
        //SM_PresetCommandParameters(&gCommand, TT, 0x71, CR2_Adresses[i], 1, 0); // READ CR2
        //SM_DoTransaction(&hospi1, TT, &Mem_CR2[i]);
      }

      // Read security register
      TT = DO_READ | NO_ALT | NO_ADR;
      SM_PresetCommandParameters(&gCommand, TT, 0x2B, 0, 1, 0); // READ Security register
      SM_DoTransaction(&hospi1, TT, Mem_SecurityReg);      
    break;
    
  case 1: // 8 bit STR
      MX_NOR_ReadStatusRegister();
      
      TT = DO_READ | NO_ALT | NO_ADR;
      SM_PresetCommandParameters(&gCommand, TT, 0x15EA, 0, 1, -1); // READ CR
      SM_DoTransaction(&hospi1, TT,  Mem_CR);
    
      TT = DO_READ | NO_ALT ;
      SM_PresetCommandParameters(&gCommand, TT, 0x9F60, 0, 3, 4); // 4 WAIT STATES NEEDED EVEN IF SPEC IS SO SO
      SM_DoTransaction(&hospi1, TT,  JedecInfo);
      TT = DO_READ | NO_ALT ;
      SM_PresetCommandParameters(&gCommand, TT, 0x5AA5, 0, 256, 20);
      SM_DoTransaction(&hospi1, TT,  SFDP);
      
      MX_NOR_ReadData(0x00000000,DataSegment,4096);
      
      TT = DO_READ | NO_ALT ;      
      for(int i=0;i<countof(CR2_Adresses);i++) {
        MX_NOR_ReadCR2(i);
        //SM_PresetCommandParameters(&gCommand, TT, 0x718E, CR2_Adresses[i], 1, -1); // READ CR2
        //SM_DoTransaction(&hospi1, TT, &Mem_CR2[i]);
      }
      // Read security register
      TT = DO_READ | NO_ALT | NO_ADR;
      SM_PresetCommandParameters(&gCommand, TT, 0x2BD4, 0, 1, 0); // READ Security register
      SM_DoTransaction(&hospi1, TT, Mem_SecurityReg);      
    break;
    
  case 2: // 8 bit DDR
      MX_NOR_ReadStatusRegister();
      TT = DO_READ | NO_ALT | NO_ADR;
      SM_PresetCommandParameters(&gCommand, TT, 0x15EA, 0, 1, 4); // READ CR
      SM_DoTransaction(&hospi1, TT,  Mem_CR);

      MX_NOR_ReadData(0x00000000,DataSegment,4096);

      TT = DO_READ | NO_ALT ;      
      for(int i=0;i<countof(CR2_Adresses);i++) {
        MX_NOR_ReadCR2(i);
        //SM_PresetCommandParameters(&gCommand, TT, 0x718E, CR2_Adresses[i], 1, -1); // READ CR2
        //SM_DoTransaction(&hospi1, TT, &Mem_CR2[i]);
      }
      
      TT = DO_READ | NO_ALT ; // DATA IN SDR MODE !!!!! TO ADJUST !!!!!
      SM_PresetCommandParameters(&gCommand, TT, 0x9F60, 0, 3, 0);
      SM_DoTransaction(&hospi1, TT,  JedecInfo);
      TT = DO_READ | NO_ALT ;
      SM_PresetCommandParameters(&gCommand, TT, 0x5AA5, 0, 256, 20);
      SM_DoTransaction(&hospi1, TT,  SFDP);

      // Read security register
      TT = DO_READ | NO_ALT | NO_ADR;
      SM_PresetCommandParameters(&gCommand, TT, 0x2BD4, 0, 1, 0); // READ Security register
      SM_DoTransaction(&hospi1, TT, Mem_SecurityReg);      
    break;
    
  default:
    while(1); // bug
  }
  
}

//////////////////////
void MX_NOR_EraseSegment(uint32_t Addr) {

  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
      MX_NOR_WriteEnable();
      TT = NO_ALT | NO_DAT ;
      SM_PresetCommandParameters(&gCommand, TT, 0x21, Addr, 0, 0); // SECTOR ERASE
      SM_DoTransaction(&hospi1, TT, 0);
      
      MX_NOR_ReadStatusRegister();
      MX_NOR_ReadStatusRegister();
      
      MX_NOR_WaitBusyEnds();
      NOPs(1);
      
    break;
    
  case 1: // 8 bit STR
  case 2: // 8 bit DTR
      MX_NOR_WriteEnable();
      TT = NO_ALT | NO_DAT ;
      SM_PresetCommandParameters(&gCommand, TT, 0x21DE, Addr, 0, 0); // SECTOR ERASE
      SM_DoTransaction(&hospi1, TT, 0);
      
      MX_NOR_WaitBusyEnds();
      
//      MX_NOR_ReadStatusRegister();
      MX_NOR_ReadStatusRegister();
      
//      MX_NOR_WaitBusyEnds();
      NOPs(1);
    break;
    
  default:
    while(1); // bug
  }
  
}


void MX_NOR_ProgramData(uint32_t Addr, uint32_t SizeByte) {

  // we should check the program is valid... for now we beleive it is 
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
      MX_NOR_WriteEnable();
      // 256 max page
      
      TT = DO_WRITE | NO_ALT;
      SM_PresetCommandParameters(&gCommand, TT, 0x12, Addr, SizeByte, 0); // PAGE PROGRAM
      SM_DoTransaction(&hospi1, TT, DataSegment); // we base from beginning of buffer ?
      MX_NOR_WaitBusyEnds();
    break;
    
  case 2:
      if(Addr & 1)
        while(1);
  case 1: // 8 bit STR and DDR
      MX_NOR_WriteEnable();
      TT = DO_WRITE | NO_ALT;
      SM_PresetCommandParameters(&gCommand, TT, 0x12ED, Addr, SizeByte, 0); // PAGE PROGRAM
      SM_DoTransaction(&hospi1, TT, DataSegment); // we base from beginning of buffer ?
      MX_NOR_WaitBusyEnds();
    break;
    
  default:
    while(1); // bug
  }
  
  // use security register to avoid reading back all data to confirm proper writing. speeds things up.
}

//========================================================
uint8_t SRCR[2];

void MX_NOR_WriteCR(uint8_t Byte) {
  
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
    // READ SR
    MX_NOR_ReadStatusRegister();
      SRCR[0] = Mem_SR[0]; // SR
      SRCR[1] = Byte; // new CR value
    // WREN
      MX_NOR_WriteEnable();
      //TT = NO_DAT | NO_ADR | NO_ALT;  
      //SM_PresetCommandParameters(&gCommand, TT, 0x06, 0, 0, 0); // WREN
      //SM_DoTransaction(&hospi1, TT, 0);
    // WRITE SR+CR  
      TT = DO_WRITE | NO_ADR | NO_ALT;
      SM_PresetCommandParameters(&gCommand, TT, 0x01, 0, 2, 0); // WRITE SR+CR
      SM_DoTransaction(&hospi1, TT, SRCR); // we base from beginning of buffer ?
    // AUTOPOLL
      MX_NOR_WaitBusyEnds();
    break;
    
  case 2:
  case 1: //TODO!!!!! // 8 bit STR and DDR
      MX_NOR_WriteEnable();
      //SM_PresetCommandParameters(&gCommand, TT, 0x06F9, 0, 0, 0); // WREN
      //SM_DoTransaction(&hospi1, TT, DataSegment);
      TT = DO_WRITE;
      SM_PresetCommandParameters(&gCommand, TT, 0x12ED, 0, 0, 0); // PAGE PROGRAM
      MX_NOR_WaitBusyEnds();
      MX_NOR_ReadStatusRegister();
//      SM_PresetCommandParameters(&gCommand, TT, 0x05FA, 0, 1, 4); // READ SR      
//      SM_DoTransaction(&hospi1, TT, DataSegment);
    break;
    
  default:
    while(1); // bug
  }
  
}

uint8_t Mem_CR2Dual[2];

void MX_NOR_ReadCR2(uint8_t index) {
  
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
    TT = DO_READ | NO_ALT;
    SM_PresetCommandParameters(&gCommand, TT, 0x71, CR2_Adresses[index], 1, 0);
    SM_DoTransaction(&hospi1, TT, Mem_CR2Dual); //
    Mem_CR2[index] = Mem_CR2Dual[0];
    break;
    
  case 2:
  case 1: //TODO!!!!! // 8 bit STR and DDR
    TT = DO_READ | NO_ALT;
    SM_PresetCommandParameters(&gCommand, TT, 0x718E, CR2_Adresses[index], 2, -1);
    SM_DoTransaction(&hospi1, TT, Mem_CR2Dual); //
    Mem_CR2[index] = Mem_CR2Dual[0];
    break;
    
  default:
    while(1); // bug
  }
  
}




void MX_NOR_WriteCR2(uint8_t index, uint8_t byteval) {

  Mem_CR2Dual[0] = Mem_CR2Dual[1] = byteval;
  
  switch(MemoryConfigurationMode) {
    
  case 0: // 1bit STR
    MX_NOR_WriteEnable();
    TT = DO_WRITE | NO_ALT;
    SM_PresetCommandParameters(&gCommand, TT, 0x72, CR2_Adresses[index], 1, 0); // WRITE CR2
    SM_DoTransaction(&hospi1, TT, Mem_CR2Dual); 
    break;
    
  case 2:
  case 1: //TODO!!!!! // 8 bit STR and DDR
    MX_NOR_WriteEnable();
    TT = DO_WRITE | NO_ALT;
    SM_PresetCommandParameters(&gCommand, TT, 0x728D, CR2_Adresses[index], 1, 0); // WRITE CR2
    SM_DoTransaction(&hospi1, TT, Mem_CR2Dual); 
    break;
    
  default:
    while(1); // bug
  }
  
}

//========================================================
// Test the driver

uint8_t Impedance = 6;
volatile uint8_t test_branch = 2;
uint8_t Mem_page[32];
void 
MX25LM5145G_Test(void)
{
  // prepare autopoll
  SM_ConfigInit(&gConfig, 0);
  SM_PresetCommandParameters(&gCommand, TT, 0x05, 0, 1, 0); // READ SR 
  
  //Run the first test in 1 bit mode
  HAL_XSPI_SetClockPrescaler(&hospi1, 4);
  MX_NOR_ReadInfo();
  MX_NOR_WriteCR(Impedance);
  HAL_XSPI_SetClockPrescaler(&hospi1, 0);
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
    MX_NOR_ReadInfo();
    DataBufferFill(0,1024,0x80,0xFF);
    MX_NOR_ProgramData(0x00000100, 16);
    MX_NOR_ReadInfo();
    break;
  case 2: // test going to 8 bit SDR mode then back to 1 bit SDR mode
    // going to 8 bit SDR mode
    MX_NOR_WriteCR2(0,0x01); // switch to 8 bit SDR mode
    MX_NOR_SetMode(1);

    // do something
    MX_NOR_ReadInfo();    
    MX_NOR_EraseSegment(0x00010000); // second 64 kbyte block
    DataBufferFill(0,1024,0x00,0x11);
    MX_NOR_ProgramData(0x00010000, 16);
    MX_NOR_ReadData(0x00010000, Mem_page, 16);
    
    // going back to 1 bit SDR mode
    MX_NOR_WriteCR2(0,0x00); // switch to 8 bit SDR mode
    MX_NOR_SetMode(0);
    MX_NOR_ReadInfo();    
    break;
  case 3: // test going to 8 bit SDR then DDR mode
    NOPs(1);
    break;
  case 4:
    for(int i=0;i<countof(CR2_Adresses);i++)
      MX_NOR_ReadCR2(i);
    MX_NOR_WriteCR2(1,0x02);
    MX_NOR_ReadInfo();
    MX_NOR_ReadCR2(1);
    NOPs(1);
    break;
  }
    
  while(1)
    NOPs(1);
  
}

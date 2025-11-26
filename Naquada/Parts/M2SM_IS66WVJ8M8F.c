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
  .CPN = "IS66WVJ8M8F",
  .ManufacturerName = "ISSI",
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
  .Vmin_V_x100 = 27,
  .Vmid_V_x100 = 0,   // support 2 voltage ranges with different speed support
  .Vmax_V_x100 = 36,
  
  
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
//SerialConfiguration_t MemoryBusMode[4] = {_D8, _D8, _D8, _D8}; //,_S8,_D8};
SerialConfiguration_t MemoryBusMode[4] = {_S8, _D8, _D8, _D8}; //,_S8,_D8};
uint32_t DQS_Enabled = 1;
uint32_t IOSelect = HAL_XSPI_SELECT_IO_7_0;
uint8_t RegisterDummyCycles = 6; // reset value
uint32_t TT; //   SEND_CMD | DO_WRITE | DO_READ | DO_AUTOPOLL

uint8_t JedecInfo[3];
uint8_t SFDP[256];
//uint8_t DataBuffer[4096];
uint8_t Mem_SR[2],Mem_CR[2],Mem_CR2[16]; // 2 for DDR extra wasted byte ? Or odd length is ok ?
uint8_t Mem_SecurityReg[2];


void MX_NOR_SetMode(uint8_t newmode);
void MX_NOR_SetMode(uint8_t newmode) {
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
  
  TT = DO_READ | NO_ALT ;
  SM_PresetCommandParameters(&gCommand, TT, 0x00C0, 0, 2, 2); // READ SR
  SM_DoTransaction(&hospi1, TT, Mem_SR);
  
  return Mem_SR[0];
}


void MX_NOR_ReadData(uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);
void MX_NOR_ReadData(uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize) 
{
 
  TT = DO_READ | NO_ALT;
  SM_PresetCommandParameters(&gCommand, TT, 0xA000, Addr, ByteSize, -1); // 8DTRD
  SM_DoTransaction(&hospi1, TT, pBytes);
  
}


void MX_NOR_ReadInfo(void) {

      MX_NOR_ReadStatusRegister();
      
      TT = DO_READ | NO_ALT ;
      SM_PresetCommandParameters(&gCommand, TT, 0xC000, 0, 2, 2); // READ CR
      SM_DoTransaction(&hospi1, TT, JedecInfo);
      
      TT = DO_READ | NO_ALT ;
      SM_PresetCommandParameters(&gCommand, TT, 0xE000, 0, 2, 2);
      SM_DoTransaction(&hospi1, TT, SFDP);

      while(1);
      
      MX_NOR_ReadData(0x00000000, DataSegment, 4096);

  
}

//////////////////////
void MX_NOR_EraseSegment(uint32_t Addr) {

  
  // use security register to avoid reading back all data to confirm proper writing. speeds things up.
}

//========================================================
uint8_t SRCR[2];

void MX_NOR_WriteCR(uint8_t Byte) {
  
}

uint8_t Mem_CR2Dual[2];

void MX_NOR_ReadCR2(uint8_t index) {
  
  
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

    
  while(1)
    NOPs(1);
  
}

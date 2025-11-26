/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

#ifndef SERIAL_MEM_H
#define SERIAL_MEM_H

// This file tries to define a universal way to describe a serial memory protocol/functionality
// as a data structure which can be I2C loaded, enabling hot swap/plug

#define NUMBEROFID 3

typedef enum {
  _1BIT_MODE = 0,
  _WIDE_MODE = 1,
  _PERF_MODE = 2,
} SerialMemoryModes_t;

typedef enum {
  _NONE,
  _S1,
  _S4,
  _D4,
  _S8,
  _D8,
  _S16,
  _D16,
} SerialConfiguration_t;

// Needed function by memory
// These are values of interest human readable
typedef enum {
  Unpopulated_Type,
  Unknown_Type,
  Other_Type,
  NOR_Type,
  EEP_Type,
  NAND_Type,
  RAM_Type,
  RRAM_Type,
  MRAM_Type,
  PCM_Type,
  DISPLAY_Type,
  Memory_Type_Nb,
} Memory_Type_t;

typedef enum {
  NO_DQS_SUPPORT,
  DQS_SUPPORT,
} DQS_Support_t;

typedef enum {
  NO_CLKN_SUPPORT,
  CLKN_SUPPORT,
} CLKN_Support_t;

typedef struct{
  uint8_t DummyCycleNb;
  uint8_t SDR_MHz;
  uint8_t DDR_MHz;
  uint8_t reserved;
} WaitStateVsMHzVsXDR_t;

typedef struct {
  uint32_t SFDP_Supported : 1; // 0
  uint32_t JEDEC_ID_Supported : 1; //
  
  uint32_t SDR_1bit_Supported : 1;
  uint32_t SDR_4bit_Supported : 1;
  uint32_t SDR_8bit_Supported : 1;
  uint32_t SRD_16bit_Supported : 1;
  
  uint32_t DDR_1bit_Supported : 1;
  uint32_t DDR_4bit_Supported : 1;
  uint32_t DDR_8bit_Supported : 1; // 8
  uint32_t DDR_16bit_Supported : 1;
  
  uint32_t DQS_Supported : 1;
  uint32_t WAITSTATES_Tuning_Supported : 1;
  uint32_t IMPEDANCE_Tuning_Supported : 1;
  uint32_t x_Supported : 1;
  uint32_t xx_Supported : 1;
  uint32_t xxx_Supported : 1;
  uint32_t xxxx_Supported : 1; // 16
} Mem_Support_Features_t;

typedef struct {
   uint8_t MHz_SDR;
   uint8_t MHz_DDR;
   uint8_t DummyCycleNb;
   uint8_t DC2_0;
} DummyCycleVsMHz_t;

// These contain a memory description from datasheet
typedef struct
{
  uint8_t VersionRevision;
  // Names, part number, manufacturer. We support reset, fast SDR, fast DDR, impedence tune
  char CPN[24];
  char ManufacturerName[24];
  Memory_Type_t MemoryType;
  int8_t DataPinIndexStart;
  int8_t DataPinIndexEnd;
  Mem_Support_Features_t Support_Features;
  uint8_t **JEDEC_ID[4]; // expected values
  WaitStateVsMHzVsXDR_t WaitStateVsMHzVsXDR[8];
  
  // Voltage domain information
  uint16_t MaxFreq_MHz;       // Max frequency in MHz for up to 2 operating voltage range
  uint16_t Vmin_V_x100;
  uint16_t Vmax_V_x100;
  
  uint16_t Delay_after_power_up_us;
  
  // TODO: Manage 2 types of sectors in the same memory (infineon)
  
//  uint16_t DelayAfterReset_us;
  // Memory space information
  int32_t SizekByte; // -1 not available
  int32_t BlockSizeByte;   // -1 not available
  int32_t BlockEraseTimeMaxMs;
  int32_t SectorSizeByte;  // -1 not available
  int32_t SectorEraseTimeMaxMs;
  int32_t PageSizeByte;    // -1 not available
  int32_t PageWriteTimeMaxMs;
  int32_t ChosenErasableAreaByteSize;
  int32_t WriteRegisterTimeMaxMs;
  int32_t ResetTimeMaxMs;
  int32_t AutopollingIntervalTime; // what unit ?
  int32_t RecommendedMinDataWriteChunkCRCSize;
  uint8_t ErasedByteValue;

  uint32_t PresetValues32b[16]; // last one comes from code passing parameter
  
  // put here wait states table
  uint8_t SR_WEL_MaskVal; // SR
  uint8_t SR_WIP_MaskVal; // SR
  uint8_t SR_RegIndex; // should be 0
  uint8_t SDR1BIT_MaskVal;
  uint8_t SDR1BIT_RegIndex;
  uint8_t SRDWideBIT_MaskVal;
  uint8_t DDR_MaskVal;
  uint8_t DDR_MaskVal_RegIndex;
  uint8_t IMPEDANCE_MaskVal;
  uint8_t IMPEDANCE_RegIndex;
  DummyCycleVsMHz_t DummyCycleVsMHz[8];
  uint8_t DummyCycle_MaskVal;
  uint8_t DummyCycle_RegIndex;
  uint8_t x_spare;
  uint8_t xx_spare;
  uint8_t xxx_spare;
  uint8_t xxxx_spare;
  uint8_t xxxxx_spare;
  uint8_t xxxxxx_spare;
  uint8_t xxxxxxx_spare;
  uint8_t xxxxxxxx_spare;
  uint8_t xxxxxxxxx_spare;
  uint8_t xxxxxxxxxx_spare;
  uint8_t xxxxxxxxxxx_spare;
  uint8_t xxxxxxxxxxxx_spare;
  
} SerialMemGlobalInfo_t;

//=========================
//extern uint32_t                         DQS_Enabled;
//extern uint8_t                          RegisterDummyCycles;

typedef struct {
  uint16_t CMD;
  uint32_t Addr;
  int8_t AddrSizeByte;
  int32_t DataSizeByte; 
  int8_t DummyCycles; // if negative, use the memory register wait states instead
} SerialTransaction_t;

//extern SerialTransaction_t ST;
// built progressively
typedef struct {
  
  XSPI_HandleTypeDef *local_ospi;
  XSPI_RegularCmdTypeDef* pCommand;
  XSPI_AutoPollingTypeDef Config;
  
  SerialTransaction_t* pST;
  
  uint8_t MemoryConfigurationMode;
  SerialConfiguration_t MemoryBusMode[4];
    
  uint8_t * pDataSegment; // a 4096 bytes RAM to clone, erase, modify, write on NOR flash
  
  uint8_t Chunk[32];
  
  uint32_t      DQS_Enabled;

  uint8_t       RegisterDummyCycles;

  uint8_t  Registers[16]; // RAM area which can be addressed to store register data. [0] is the Status Register
  // each register is doubled for DTR potential side effects
  // register names are #define for each specific vendor. Status register is always offset 0
  // status registers, then
  // control registers, then
  // mapped registers, then
  
  SerialMemGlobalInfo_t* pInfo;
    
} M2SerMem_t;

void SM_PresetCommandParameters(M2SerMem_t * pM2SM, SerialTransaction_t* pST);
void SM_DoTransaction(XSPI_HandleTypeDef *local_ospi, SerialTransaction_t* pST, uint8_t *pData);

extern uint8_t DataSegment[4096];
void DataBufferFill(uint32_t adr_start, uint32_t sizebyte, uint8_t value, uint8_t incstep);

void SerialMemoryTuneDelay(void);

void SM_DoTransactionCMD(M2SerMem_t * pM2SM);
void SM_DoTransactionWrite(M2SerMem_t * pM2SM, uint8_t *pData);
void SM_DoTransactionRead(M2SerMem_t * pM2SM, uint8_t *pData);
void SM_DoTransactionPoll(M2SerMem_t * pM2SM);

void SM_ConfigInit(M2SerMem_t * pM2SM);
//void ConfigInit(XSPI_AutoPollingTypeDef* pConfig, uint8_t preset);

#endif // SERIAL_MEM_H
/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#include "main.h"
//#include "MX25LM51245G.h"



static XSPI_RegularCmdTypeDef     gCommand,gCommandBf;
void CommandInit(XSPI_RegularCmdTypeDef* pCommand, uint8_t preset);

void CommandInit(XSPI_RegularCmdTypeDef* pCommand, uint8_t preset)
{
  pCommand->OperationType		= HAL_XSPI_OPTYPE_COMMON_CFG; // fix for all
  pCommand->IOSelect 			= HAL_XSPI_SELECT_IO_3_0; //  fix for all
  pCommand->Instruction       		= 0; // table[commandcode]
  pCommand->InstructionMode   		= HAL_XSPI_INSTRUCTION_1_LINE; // fix for all
  pCommand->InstructionWidth		= HAL_XSPI_INSTRUCTION_8_BITS; // fix for all
  pCommand->InstructionDTRMode		= HAL_XSPI_INSTRUCTION_DTR_DISABLE; // fix for all
  pCommand->Address			= 0; // 0 or add
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE; // vary: NONE or _1_LINE if ddd
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_32_BITS; // variable, mostly 32 bit
  pCommand->AddressDTRMode		= HAL_XSPI_ADDRESS_DTR_DISABLE; // fix for all
  pCommand->AlternateBytes		= 0; // fix for all
  pCommand->AlternateBytesMode		= HAL_XSPI_ALT_BYTES_NONE; // fix for all
  pCommand->AlternateBytesWidth		= HAL_XSPI_ALT_BYTES_8_BITS; // fix for all
  pCommand->AlternateBytesDTRMode	= HAL_XSPI_ALT_BYTES_DTR_DISABLE; // fix for all
  pCommand->DataMode			= HAL_XSPI_DATA_NONE; // NONE or _1_LINE _2_LINES or _4_LINES
  pCommand->DataLength			= 1;
  pCommand->DataDTRMode			= HAL_XSPI_DATA_DTR_DISABLE; // fix for all
  pCommand->DummyCycles			= 0; // 0 or 8
  pCommand->DQSMode			= HAL_XSPI_DQS_DISABLE; // fix for all
  pCommand->SIOOMode			= HAL_XSPI_SIOO_INST_EVERY_CMD; // fix for all
}

static XSPI_AutoPollingTypeDef    gConfig,gConfigBF;
void ConfigInit(XSPI_AutoPollingTypeDef* pConfig, uint8_t preset);

void ConfigInit(XSPI_AutoPollingTypeDef* pConfig, uint8_t preset) {
  
  pConfig->MatchValue               	= 0x00;
  pConfig->MatchMask                    = 0x03;
  pConfig->MatchMode       		= HAL_XSPI_MATCH_MODE_AND;
  pConfig->IntervalTime                 = 0x10;
  pConfig->AutomaticStop   		= HAL_XSPI_AUTOMATIC_STOP_ENABLE;
  
}

void WREN(XSPI_HandleTypeDef *local_ospi) // treated
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
	
  pCommand->Instruction 		= CMD_SPI_WREN;
  pCommand->Address			= 0;
  pCommand->AddressMode  		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

void WRDI(XSPI_HandleTypeDef *local_ospi) // treated
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);

  pCommand->Instruction       		= CMD_SPI_WRDI;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

uint8_t RDSR(XSPI_HandleTypeDef *local_ospi) // treated
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  uint8_t statusValue;

  pCommand->Instruction       		= CMD_SPI_RDSR;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_XSPI_Receive(local_ospi, &statusValue, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
	return statusValue;
}

void RDSR_Autocomplete(XSPI_HandleTypeDef *local_ospi) // treated
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  /* Configure automatic polling mode to wait for write enabling ---- */
  pCommand->Instruction       		= CMD_SPI_RDSR;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  ConfigInit(&gConfig, 0);
  
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_XSPI_AutoPolling(local_ospi, pConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}



void Read_JEDEC(XSPI_HandleTypeDef *local_ospi,uint8_t nb_data,uint8_t *pData) // treated
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);

  pCommand->Instruction       		= CMD_SPI_RDID;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= nb_data;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_XSPI_Receive(local_ospi, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}
void Read_SFDP(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData) // treated
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_SPI_RDSFDP;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= nb_data;
  pCommand->DummyCycles			= 8;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
	  
  if (HAL_XSPI_Receive(local_ospi, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

void WRSR(XSPI_HandleTypeDef *local_ospi,uint32_t nb_data,uint8_t *pData) // treated
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
	
  pCommand->Instruction       		= CMD_SPI_WRSR;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= nb_data;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
	  
  if (HAL_XSPI_Transmit(local_ospi, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  RDSR_Autocomplete(local_ospi);
  /* Configure automatic polling mode to wait for write enabling ---- */
/*  pCommand->Instruction       		= CMD_SPI_RDSR;
  pCommand->Address		        = 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; // 
  pCommand->DataMode	        	= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength  		= 1;
  pCommand->DummyCycles			= 0;

  ConfigInit(&gConfig, 0);

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_XSPI_AutoPolling(local_ospi, pConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
*/
}
void Single_Read(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData) // treated
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_SPI_READ4B;
  pCommand->Address			= add;
  
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_32_BITS;
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= nb_data;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
	  
  if (HAL_XSPI_Receive(local_ospi, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}
void FAST_Read(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData) // treated
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_SPI_FAST_READ4B;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_32_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= nb_data;
  pCommand->DummyCycles			= 8;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
	  
  if (HAL_XSPI_Receive(local_ospi, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

void FAST_DRead(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  while(1); // unsupported
}

void FAST_QRead(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  while(1); // unsupported
}

void Page_Write(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);

  pCommand->Instruction       		= CMD_SPI_PP4B;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_32_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= nb_data;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
	  
  if (HAL_XSPI_Transmit(local_ospi, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  RDSR_Autocomplete(local_ospi);
  /* Configure automatic polling mode to wait for write enabling ---- */
/*  pCommand->Instruction       		= CMD_SPI_RDSR;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  ConfigInit(&gConfig, 0);
  
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_XSPI_AutoPolling(local_ospi, pConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
*/
}
void Page_Prog(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData) // what's the difference page program and page write ?
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_SPI_PP4B;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_32_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= nb_data;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
	  
  if (HAL_XSPI_Transmit(local_ospi, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure automatic polling mode to wait for write enabling ---- */
  RDSR_Autocomplete(local_ospi);
/*  pCommand->Instruction       		= CMD_SPI_RDSR;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  ConfigInit(&gConfig, 0);
  
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }	
  
  if (HAL_XSPI_AutoPolling(local_ospi, pConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
*/
}
void Page_Erase(XSPI_HandleTypeDef *local_ospi,uint32_t Add)
{
  while(1); // not supported
}
void Sector_Erase(XSPI_HandleTypeDef *local_ospi,uint32_t Add)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_SPI_SE4B;
  pCommand->Address			= Add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_32_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 0;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure automatic polling mode to wait for write enabling ---- */
  RDSR_Autocomplete(local_ospi);
/*  
  pCommand->Instruction       		= CMD_SPI_RDSR;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  ConfigInit(&gConfig, 0);
  
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }	
  
  if (HAL_XSPI_AutoPolling(local_ospi, pConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
*/
}
void Block_Erase(XSPI_HandleTypeDef *local_ospi,uint32_t Add)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_SPI_BE4B;
  pCommand->Address			= Add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_32_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 0;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure automatic polling mode to wait for write enabling ---- */
  RDSR_Autocomplete(local_ospi);
  
/*  pCommand->Instruction       		= CMD_SPI_RDSR;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  ConfigInit(&gConfig, 0);

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }	
  
  if (HAL_XSPI_AutoPolling(local_ospi, pConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
*/
}
void Chip_Erase(XSPI_HandleTypeDef *local_ospi)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_SPI_CE;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 0;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure automatic polling mode to wait for write enabling ---- */
  RDSR_Autocomplete(local_ospi);
/*  pCommand->Instruction       		= CMD_SPI_RDSR;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  ConfigInit(&gConfig, 0);
  
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }	
  
  if (HAL_XSPI_AutoPolling(local_ospi, pConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
*/
}
void Read_ID(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);

  pCommand->Instruction       		= CMD_SPI_RDID;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_32_BITS; // ???
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= nb_data;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
	  
  if (HAL_XSPI_Receive(local_ospi, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}
void FAST_Read_ID(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  while(1); /// only one command
}
void Write_ID(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  while(1); // ?
}
void Deep_Power_Down(XSPI_HandleTypeDef *local_ospi)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_SPI_DP;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}
void Deep_Power_Down_Release(XSPI_HandleTypeDef *local_ospi)
{
  while(1); // ?
}
void WriteVolatileRegister(XSPI_HandleTypeDef *local_ospi,uint8_t regVal)
{
  while(1); //
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);

//  pCommand->Instruction       		= CMD_WRITE_VOLATILE_REG;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
	  
  if (HAL_XSPI_Transmit(local_ospi, &regVal, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure automatic polling mode to wait for write enabling ---- */
  RDSR_Autocomplete(local_ospi);
/*  pCommand->Instruction       		= CMD_SPI_RDSR;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_1_LINE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  ConfigInit(&gConfig, 0);
  
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }	
  
  if (HAL_XSPI_AutoPolling(local_ospi, pConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
*/
}
void ClearSafetyFlag(XSPI_HandleTypeDef *local_ospi)
{
  while(1);//
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  //pCommand->Instruction       		= CMD_CLEAR_SAFETY_REG;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}
void Reset_Enable(XSPI_HandleTypeDef *local_ospi)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_SPI_RSTEN;
  pCommand->Address		        = 0;
  pCommand->AddressMode       	        = HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler(); 
  }
}
void Soft_Reset(XSPI_HandleTypeDef *local_ospi)
{
  while(1);//?
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  //pCommand->Instruction       		= CMD_SOFT_RESET;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; //
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}




#include "main.h"
#include "m95p32.h"

//#define HAL_XSPI_SELECT_IO_3_0 HAL_XSPI_SELECT_IO_3_0

// MAGIC API: HAL_StatusTypeDef HAL_XSPI_SetClockPrescaler(XSPI_HandleTypeDef *hxspi, uint32_t Prescaler)
void RDSR_Autocomplete(XSPI_HandleTypeDef *local_ospi);

XSPI_RegularCmdTypeDef     gCommand,gCommandBf;
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
  pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS; // fix for all
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

XSPI_AutoPollingTypeDef    gConfig,gConfigBF;
void ConfigInit(XSPI_AutoPollingTypeDef* pConfig, uint8_t preset);

void ConfigInit(XSPI_AutoPollingTypeDef* pConfig, uint8_t preset) {
  
  pConfig->MatchValue               	= 0x00;
  pConfig->MatchMask                    = 0x03;
  pConfig->MatchMode       		= HAL_XSPI_MATCH_MODE_AND;
  pConfig->IntervalTime                 = 0x10;
  pConfig->AutomaticStop   		= HAL_XSPI_AUTOMATIC_STOP_ENABLE;
  
}

void WREN(XSPI_HandleTypeDef *local_ospi)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
	
  pCommand->Instruction 		= CMD_WREN;
  pCommand->Address			= 0;
  pCommand->AddressMode  		= HAL_XSPI_ADDRESS_NONE;
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

void WRDI(XSPI_HandleTypeDef *local_ospi)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);

  pCommand->Instruction       		= CMD_WRDI;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}
uint8_t RDSR(XSPI_HandleTypeDef *local_ospi)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  uint8_t statusValue;

  pCommand->Instruction       		= CMD_READ_STATUS_REG;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
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
  pCommand->Instruction       		= CMD_READ_STATUS_REG;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
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


void Read_JEDEC(XSPI_HandleTypeDef *local_ospi,uint8_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);

  pCommand->Instruction       		= CMD_READ_JEDEC;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
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
void Read_SFDP(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_READ_SFDP;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
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
void WRSR(XSPI_HandleTypeDef *local_ospi,uint32_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
	
  pCommand->Instruction       		= CMD_WRITE_SATUTS_CONF_REG;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
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
  pCommand->Instruction       		= CMD_READ_STATUS_REG;
  pCommand->Address		        = 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
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
}
void Single_Read(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_READ_DATA;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
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
void FAST_Read(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_FAST_READ_SINGLE;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
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
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_FAST_READ_DUAL;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->DataMode			= HAL_XSPI_DATA_2_LINES;
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
void FAST_QRead(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_FAST_READ_QUAD;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->DataMode			= HAL_XSPI_DATA_4_LINES;
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
void Page_Write(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);

  pCommand->Instruction       		= CMD_WRITE_PAGE;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
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
}
void Page_Prog(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_PROG_PAGE;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
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
}
void Page_Erase(XSPI_HandleTypeDef *local_ospi,uint32_t Add)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_ERASE_PAGE;
  pCommand->Address			= Add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 0;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure automatic polling mode to wait for write enabling ---- */
  pCommand->Instruction       		= CMD_READ_STATUS_REG;
  pCommand->Address			= 0;
  pCommand->DataMode			=	HAL_XSPI_DATA_1_LINE;
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
void Sector_Erase(XSPI_HandleTypeDef *local_ospi,uint32_t Add)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_ERASE_SECTOR;
  pCommand->Address			= Add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 0;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure automatic polling mode to wait for write enabling ---- */
  RDSR_Autocomplete(local_ospi);
}
void Block_Erase(XSPI_HandleTypeDef *local_ospi,uint32_t Add)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_ERASE_BLOCK;
  pCommand->Address			= Add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 0;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure automatic polling mode to wait for write enabling ---- */
  pCommand->Instruction       		= CMD_READ_STATUS_REG;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->DataMode			=	HAL_XSPI_DATA_1_LINE;
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
void Chip_Erase(XSPI_HandleTypeDef *local_ospi)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_ERASE_CHIP;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 0;
  pCommand->DummyCycles			= 0;
	
  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure automatic polling mode to wait for write enabling ---- */
  RDSR_Autocomplete(local_ospi);
}
void Read_ID(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);

  pCommand->Instruction       		= CMD_READ_ID_PAGE;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
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
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_FAST_READ_ID_PAGE;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
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
void Write_ID(XSPI_HandleTypeDef *local_ospi,uint32_t add,uint32_t nb_data,uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);

  pCommand->Instruction       		= CMD_WRITE_ID_PAGE;
  pCommand->Address			= add;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_1_LINE;
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
}
void Deep_Power_Down(XSPI_HandleTypeDef *local_ospi)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_DEEP_POWER_DOWN;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
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
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_DEEP_POWER_DOWN_RELEASE;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->DataMode			= HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}
void WriteVolatileRegister(XSPI_HandleTypeDef *local_ospi,uint8_t regVal)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);

  pCommand->Instruction       		= CMD_WRITE_VOLATILE_REG;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->DataMode			=	HAL_XSPI_DATA_1_LINE;
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
}
void ClearSafetyFlag(XSPI_HandleTypeDef *local_ospi)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_CLEAR_SAFETY_REG;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->DataMode			=	HAL_XSPI_DATA_NONE;
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
  
  pCommand->Instruction       		= CMD_ENABLE_RESET;
  pCommand->Address		        = 0;
  pCommand->AddressMode       	        = HAL_XSPI_ADDRESS_NONE;
  pCommand->DataMode			=	HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler(); 
  }
}
void Soft_Reset(XSPI_HandleTypeDef *local_ospi)
{
  XSPI_RegularCmdTypeDef* pCommand = &gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &gConfig;
  CommandInit(pCommand, 1);
  
  pCommand->Instruction       		= CMD_SOFT_RESET;
  pCommand->Address			= 0;
  pCommand->AddressMode       		= HAL_XSPI_ADDRESS_NONE;
  pCommand->DataMode			=	HAL_XSPI_DATA_NONE;
  pCommand->DataLength			= 1;
  pCommand->DummyCycles			= 0;

  if (HAL_XSPI_Command(local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
XSPI_HandleTypeDef hxspi1;

/* USER CODE BEGIN PV */

LL_DLYB_CfgTypeDef dlyb_cfg, dlyb_cfg_test;


uint32_t OCTOSPI1_Freq;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_OCTOSPI1_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
static void OSPI_WriteEnable(XSPI_HandleTypeDef *hospi);
static void OSPI_AutoPollingMemReady(XSPI_HandleTypeDef *hospi);
static void OSPI_OctalModeCfg(XSPI_HandleTypeDef *hospi);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define BufferSize 256
uint8_t aTxBuffer[BufferSize] = {


		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
		0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
		0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
		0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
		0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
		0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
		0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
		0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};
uint8_t aRxBuffer[BufferSize] = {0};
uint8_t ID[3] = {0} ;
uint8_t SFDP[4] = {0} ;
uint8_t CR2[6] = {0} ;
uint32_t res =0;

#define WRITE_READ_ADDR     ((uint32_t)0x00000000)

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  XSPI_RegularCmdTypeDef sCommand = {0};

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_OCTOSPI1_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
/*****************************************Delay block configuration*******************************************************/
  /* get OCTOSPI frequency */
  OCTOSPI1_Freq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_OCTOSPI1);

  /* Delay block configuration ------------------------------------------------ */
  if (HAL_XSPI_DLYB_GetClockPeriod(&hxspi1,&dlyb_cfg) != HAL_OK)
  {
    while(1){};
  }

  /*when DTR, PhaseSel is divided by 4 (emperic value)*/
  dlyb_cfg.PhaseSel /=4;

  /* save the present configuration for check*/
  dlyb_cfg_test = dlyb_cfg;

  /*set delay block configuration*/
  if(HAL_XSPI_DLYB_SetConfig(&hxspi1,&dlyb_cfg) != HAL_OK )
  {
    Error_Handler() ;
  }

  /*check the set value*/
  if(HAL_XSPI_DLYB_GetConfig(&hxspi1,&dlyb_cfg) != HAL_OK )
  {
    Error_Handler() ;
  }

  if ((dlyb_cfg.PhaseSel != dlyb_cfg_test.PhaseSel) || (dlyb_cfg.Units != dlyb_cfg_test.Units))
  {
    while(1){};
  }

  /*****************************************Delay block configuration*******************************************************/


  /* Configure the memory in octal mode ------------------------------------- */
    OSPI_OctalModeCfg(&hxspi1);

 {
  /* Read ID : Expected Result: C2853A : ---------------------------------------- */
   sCommand.OperationType            = HAL_XSPI_OPTYPE_COMMON_CFG;
   sCommand.IOSelect                 = HAL_XSPI_SELECT_IO_7_0;

   sCommand.Instruction              = 0x9F60;
   sCommand.InstructionMode          = HAL_XSPI_INSTRUCTION_8_LINES;
   sCommand.InstructionWidth         = HAL_XSPI_INSTRUCTION_16_BITS;
   sCommand.InstructionDTRMode       = HAL_XSPI_INSTRUCTION_DTR_ENABLE;

   sCommand.Address                  = WRITE_READ_ADDR;
   sCommand.AddressMode              = HAL_XSPI_ADDRESS_8_LINES;
   sCommand.AddressWidth             = HAL_XSPI_ADDRESS_32_BITS;
   sCommand.AddressDTRMode           = HAL_XSPI_ADDRESS_DTR_ENABLE;

   sCommand.AlternateBytes           = 0;
   sCommand.AlternateBytesMode       = HAL_XSPI_ALT_BYTES_NONE;
   sCommand.AlternateBytesWidth      = 0;
   sCommand.AlternateBytesDTRMode    = 0;

   sCommand.DataMode                 = HAL_XSPI_DATA_8_LINES;
   sCommand.DataLength               = 3;
   sCommand.DataDTRMode              = HAL_XSPI_DATA_DTR_DISABLE;

   sCommand.DummyCycles              = 6; //
   sCommand.DQSMode                  = HAL_XSPI_DQS_ENABLE;
   sCommand.SIOOMode           		 = HAL_XSPI_SIOO_INST_ONLY_FIRST_CMD;

	if (HAL_XSPI_Command(&hxspi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
   {
	 Error_Handler();
   }

	if (HAL_XSPI_Receive(&hxspi1, ID, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
   {
	 Error_Handler();
   }

	        res = ID[0]<<16|ID[1]<<8|ID[2];
}
	/* Read SFDP : Expected Result: SFDP : ---------------------------------------- */
 {

	       sCommand.OperationType            = HAL_XSPI_OPTYPE_COMMON_CFG;
		   sCommand.IOSelect                 = HAL_XSPI_SELECT_IO_7_0;

		   sCommand.Instruction              = 0x5AA5;
		   sCommand.InstructionMode          = HAL_XSPI_INSTRUCTION_8_LINES;
		   sCommand.InstructionWidth         = HAL_XSPI_INSTRUCTION_16_BITS;
		   sCommand.InstructionDTRMode       = HAL_XSPI_INSTRUCTION_DTR_ENABLE;

		   sCommand.Address                  = WRITE_READ_ADDR;
		   sCommand.AddressMode              = HAL_XSPI_ADDRESS_8_LINES;
		   sCommand.AddressWidth             = HAL_XSPI_ADDRESS_32_BITS;
		   sCommand.AddressDTRMode           = HAL_XSPI_ADDRESS_DTR_ENABLE;

		   sCommand.AlternateBytes           = 0;
		   sCommand.AlternateBytesMode       = HAL_XSPI_ALT_BYTES_NONE;
		   sCommand.AlternateBytesWidth      = 0;
		   sCommand.AlternateBytesDTRMode    = 0;

		   sCommand.DataMode                 = HAL_XSPI_DATA_8_LINES;
		   sCommand.DataLength               = 4;
		   sCommand.DataDTRMode              = HAL_XSPI_DATA_DTR_ENABLE;

		   sCommand.DummyCycles              = 6; //
		   sCommand.DQSMode                  = HAL_XSPI_DQS_ENABLE;
		   sCommand.SIOOMode           		 = HAL_XSPI_SIOO_INST_ONLY_FIRST_CMD;

			if (HAL_XSPI_Command(&hxspi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		   {
			 Error_Handler();
		   }

			if (HAL_XSPI_Receive(&hxspi1, SFDP, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		   {
			 Error_Handler();
		   }


}


/******************************* Step 1: Erase memory ********************************************************************/


    OSPI_WriteEnable(&hxspi1);

    /* Erasing Sequence -------------------------------------------------- */
    sCommand.OperationType            = HAL_XSPI_OPTYPE_COMMON_CFG;
    sCommand.Instruction              = 0x21DE;
    sCommand.InstructionMode          = HAL_XSPI_INSTRUCTION_8_LINES;
    sCommand.InstructionWidth         = HAL_XSPI_INSTRUCTION_16_BITS;
    sCommand.InstructionDTRMode       = HAL_XSPI_INSTRUCTION_DTR_ENABLE;
    sCommand.Address                  = WRITE_READ_ADDR;
    sCommand.AddressMode              = HAL_XSPI_ADDRESS_8_LINES;
    sCommand.AddressWidth             = HAL_XSPI_ADDRESS_32_BITS;
    sCommand.AddressDTRMode           = HAL_XSPI_ADDRESS_DTR_ENABLE;

    sCommand.AlternateBytes           = 0;
    sCommand.AlternateBytesMode       = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.AlternateBytesWidth      = 0;
    sCommand.AlternateBytesDTRMode    = 0;

    sCommand.DataMode                 = HAL_XSPI_DATA_NONE;
    sCommand.DataLength               = 0;
    sCommand.DataDTRMode              = 0;
    sCommand.DummyCycles              = 0;
    sCommand.DQSMode                  = HAL_XSPI_DQS_DISABLE;

    if (HAL_XSPI_Command(&hxspi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }

    /* Configure automatic polling mode to wait for end of erase ---------- */
    OSPI_AutoPollingMemReady(&hxspi1);


/**************************** Write in memory ************************************************************************/


    /* Enable write operations ---------------------------------------- */
    OSPI_WriteEnable(&hxspi1);

    /* Writing Sequence ------------------------------------------------ */
    sCommand.OperationType            = HAL_XSPI_OPTYPE_COMMON_CFG;
    sCommand.Instruction              = 0x12ED;
    sCommand.InstructionMode          = HAL_XSPI_INSTRUCTION_8_LINES;
    sCommand.InstructionWidth         = HAL_XSPI_INSTRUCTION_16_BITS;
    sCommand.InstructionDTRMode       = HAL_XSPI_INSTRUCTION_DTR_ENABLE;
    sCommand.Address                  = WRITE_READ_ADDR;
    sCommand.AddressMode              = HAL_XSPI_ADDRESS_8_LINES;
    sCommand.AddressWidth             = HAL_XSPI_ADDRESS_32_BITS;
    sCommand.AddressDTRMode           = HAL_XSPI_ADDRESS_DTR_ENABLE;
    sCommand.AlternateBytes           = 0;
    sCommand.AlternateBytesMode       = HAL_XSPI_ALT_BYTES_NONE;
    sCommand.AlternateBytesWidth      = 0;
    sCommand.AlternateBytesMode       = 0;
    sCommand.DataMode                 = HAL_XSPI_DATA_8_LINES;
    sCommand.DataLength               = BufferSize;
    sCommand.DataDTRMode              = HAL_XSPI_DATA_DTR_ENABLE;
    sCommand.DummyCycles              = 0;
    sCommand.DQSMode                  = HAL_XSPI_DQS_DISABLE;
    if (HAL_XSPI_Command(&hxspi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_XSPI_Transmit(&hxspi1, aTxBuffer,HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }



    OSPI_AutoPollingMemReady(&hxspi1);


    /* Reading Sequence ------------------------------------------------ */
   	  sCommand.OperationType            = HAL_XSPI_OPTYPE_COMMON_CFG;
   	  sCommand.IOSelect                 = HAL_XSPI_SELECT_IO_7_0;

   	  sCommand.Instruction              = 0xEE11;
   	  sCommand.InstructionMode          = HAL_XSPI_INSTRUCTION_8_LINES;
   	  sCommand.InstructionWidth         = HAL_XSPI_INSTRUCTION_16_BITS;
   	  sCommand.InstructionDTRMode       = HAL_XSPI_INSTRUCTION_DTR_ENABLE;

   	  sCommand.Address                  = WRITE_READ_ADDR;
   	  sCommand.AddressMode              = HAL_XSPI_ADDRESS_8_LINES;
   	  sCommand.AddressWidth             = HAL_XSPI_ADDRESS_32_BITS;
   	  sCommand.AddressDTRMode           = HAL_XSPI_ADDRESS_DTR_ENABLE;

   	  sCommand.AlternateBytes           = 0;
   	  sCommand.AlternateBytesMode       = HAL_XSPI_ALT_BYTES_NONE;
   	  sCommand.AlternateBytesWidth      = 0;
   	  sCommand.AlternateBytesDTRMode    = 0;

   	  sCommand.DataMode                 = HAL_XSPI_DATA_8_LINES;
   	  sCommand.DataLength               = BufferSize;
   	  sCommand.DataDTRMode              = HAL_XSPI_DATA_DTR_ENABLE;

   	  sCommand.DummyCycles              = 6;
   	  sCommand.DQSMode                  = HAL_XSPI_DQS_ENABLE;

   	  if (HAL_XSPI_Command(&hxspi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
   	  {
   		Error_Handler();
   	  }

   	  if (HAL_XSPI_Receive(&hxspi1, aRxBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
   	  {
   		Error_Handler();
   	  }

   	 /* Result comparison ----------------------------------------------- */
   	  for (uint16_t index = 0; index < 256; index++)
   	  {
   	    if (aRxBuffer[index] != aTxBuffer[index])
   	    {
   	      while(1);
   	    }
   	  }



/***********************************************************************************************************/


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	  HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}





/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	 RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	  /** Enable Epod Booster
	  */
	  __HAL_RCC_PWR_CLK_ENABLE();
	  HAL_RCCEx_EpodBoosterClkConfig(RCC_EPODBOOSTER_SOURCE_HSI, RCC_EPODBOOSTER_DIV1);
	  HAL_PWREx_EnableEpodBooster();

	  /** Configure the main internal regulator output voltage
	  */
	  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  /** Set Flash latency before increasing MSIS
	  */
	  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_2);

	  /** Initializes the CPU, AHB and APB buses clocks
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSIS;
	  RCC_OscInitStruct.MSISState = RCC_MSI_ON;
	  RCC_OscInitStruct.MSISSource = RCC_MSI_RC0;
	  RCC_OscInitStruct.MSISDiv = RCC_MSI_DIV1;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  /** Initializes the CPU, AHB and APB buses clocks
	  */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
	                              |RCC_CLOCKTYPE_PCLK3;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSIS;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	  {
	    Error_Handler();
	  }
}



/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache (default 2-ways set associative cache)
  */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief OCTOSPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OCTOSPI1_Init(void)
{

	/* USER CODE BEGIN OCTOSPI1_Init 0 */

	/* USER CODE END OCTOSPI1_Init 0 */

	HAL_XSPI_DLYB_CfgTypeDef HAL_XSPI_DLYB_Cfg_Struct = {0};

	/* USER CODE BEGIN OCTOSPI1_Init 1 */

	/* USER CODE END OCTOSPI1_Init 1 */
	/* OCTOSPI1 parameter configuration*/
	hxspi1.Instance = OCTOSPI1;
	hxspi1.Init.FifoThresholdByte = 4;
	hxspi1.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
	hxspi1.Init.MemoryType = HAL_XSPI_MEMTYPE_MACRONIX;
	hxspi1.Init.MemorySize = HAL_XSPI_SIZE_512MB;
	hxspi1.Init.ChipSelectHighTimeCycle = 3;
	hxspi1.Init.FreeRunningClock = HAL_XSPI_FREERUNCLK_DISABLE;
	hxspi1.Init.ClockMode = HAL_XSPI_CLOCK_MODE_0;
	hxspi1.Init.WrapSize = HAL_XSPI_WRAP_NOT_SUPPORTED;
	hxspi1.Init.ClockPrescaler = 1;
	hxspi1.Init.SampleShifting = HAL_XSPI_SAMPLE_SHIFT_NONE;
	hxspi1.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_ENABLE; //
	hxspi1.Init.ChipSelectBoundary = HAL_XSPI_BONDARYOF_NONE;
	hxspi1.Init.DelayBlockBypass = HAL_XSPI_DELAY_BLOCK_ON;//
	hxspi1.Init.MaxTran = 0;
	hxspi1.Init.Refresh = 0;
	if (HAL_XSPI_Init(&hxspi1) != HAL_OK)
	{
	Error_Handler();
	}
	HAL_XSPI_DLYB_Cfg_Struct.Units = 0;
	HAL_XSPI_DLYB_Cfg_Struct.PhaseSel = 0;
	if (HAL_XSPI_DLYB_SetConfig(&hxspi1, &HAL_XSPI_DLYB_Cfg_Struct) != HAL_OK)
	{
	Error_Handler();
	}
  /* USER CODE BEGIN OCTOSPI1_Init 2 */

  /* USER CODE END OCTOSPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

/***********************Check your GPIO Ports*************************/

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */





static void OSPI_WriteEnable(XSPI_HandleTypeDef *hospi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  XSPI_AutoPollingTypeDef sConfig = {0};

  /* Enable write operations ------------------------------------------ */
  sCommand.OperationType      			= HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.IOSelect           			= HAL_XSPI_SELECT_IO_7_0;

  sCommand.Instruction        			= 0x06F9;

  sCommand.InstructionMode    			= HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth   			= HAL_XSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDTRMode 			= HAL_XSPI_INSTRUCTION_DTR_ENABLE;

  sCommand.Address                		= 0U;
  sCommand.AddressMode        			= HAL_XSPI_ADDRESS_NONE;
  sCommand.AddressWidth             	= HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode           	= HAL_XSPI_ADDRESS_DTR_ENABLE;

   sCommand.AlternateBytes           	= 0U;
   sCommand.AlternateBytesMode       	= HAL_XSPI_ALT_BYTES_NONE;
   sCommand.AlternateBytesWidth      	= 0U;
   sCommand.AlternateBytesDTRMode    	= 0U;

  sCommand.DataMode           			= HAL_XSPI_DATA_NONE;
  sCommand.DataLength              	 	= 0U;
  sCommand.DummyCycles        			= 0U;

  sCommand.DQSMode            			= HAL_XSPI_DQS_DISABLE;
  sCommand.SIOOMode           			= HAL_XSPI_SIOO_INST_ONLY_FIRST_CMD;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure automatic polling mode to wait for write enabling ---- */
  sCommand.Instruction        			= 0x05FA;
  sCommand.InstructionMode    			= HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth   			= HAL_XSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDTRMode 			= HAL_XSPI_INSTRUCTION_DTR_ENABLE;

  sCommand.Address        				= 0x0;
  sCommand.AddressMode    				= HAL_XSPI_ADDRESS_8_LINES;
  sCommand.AddressWidth   				= HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode 				= HAL_XSPI_ADDRESS_DTR_ENABLE;

  sCommand.DataMode       				= HAL_XSPI_DATA_8_LINES;
  sCommand.DataDTRMode    				= HAL_XSPI_DATA_DTR_ENABLE;
  sCommand.DataLength     				= 1;
  sCommand.DummyCycles   				= 4;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.MatchValue      				= WRITE_ENABLE_MATCH_VALUE;
  sConfig.MatchMask       				= WRITE_ENABLE_MASK_VALUE;
  sConfig.MatchMode       				= HAL_XSPI_MATCH_MODE_AND;
  sConfig.IntervalTime    				= AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop   				= HAL_XSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hospi: OSPI handle
  * @retval None
  */
static void OSPI_AutoPollingMemReady(XSPI_HandleTypeDef *hospi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  XSPI_AutoPollingTypeDef sConfig = {0};


  /* Configure automatic polling mode to wait for memory ready ------ */
  sCommand.OperationType      			= HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction        			= 0x05FA;
  sCommand.InstructionMode    			= HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth   			= HAL_XSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDTRMode 			= HAL_XSPI_INSTRUCTION_DTR_ENABLE;        //

  sCommand.Address            			= 0x00000000;
  sCommand.AddressMode        			= HAL_XSPI_ADDRESS_8_LINES;
  sCommand.AddressWidth       			= HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode     			= HAL_XSPI_ADDRESS_DTR_ENABLE;

  sCommand.AlternateBytes           	= 0;
  sCommand.AlternateBytesMode       	= HAL_XSPI_ALT_BYTES_NONE;
  sCommand.AlternateBytesWidth      	= 0;
  sCommand.AlternateBytesDTRMode    	= 0;

  sCommand.DataMode           			= HAL_XSPI_DATA_8_LINES;
  sCommand.DataDTRMode        			= HAL_XSPI_DATA_DTR_ENABLE;

  sCommand.DataLength         			= 1;
  sCommand.DummyCycles        			= 6;
  sCommand.DQSMode            			= HAL_XSPI_DQS_ENABLE;   //
  sCommand.SIOOMode           			= HAL_XSPI_SIOO_INST_EVERY_CMD;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.MatchValue      = 0x00;
  sConfig.MatchMask       = 0x01;
  sConfig.MatchMode       = HAL_XSPI_MATCH_MODE_AND;
  sConfig.IntervalTime    = 0x10;
  sConfig.AutomaticStop   = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }
}

/**
  * @brief  This function configure the memory in Octal mode.
  * @param  hospi: OSPI handle
  * @retval None
  */
static void OSPI_OctalModeCfg(XSPI_HandleTypeDef *hospi)
{

	XSPI_RegularCmdTypeDef  sCommand = {0};
	  XSPI_AutoPollingTypeDef sConfig = {0};
	  uint8_t reg = 0x00;

	  //enable write operation (but in single line mode)
	  {
	    /* Enable write operations ---------------------------------------- */
	    sCommand.OperationType              = HAL_XSPI_OPTYPE_COMMON_CFG;
	    sCommand.IOSelect                   = HAL_XSPI_SELECT_IO_7_0;

	    sCommand.Instruction                = 0x06; //0x06: Single mode
	    sCommand.InstructionMode            = HAL_XSPI_INSTRUCTION_1_LINE;
	    sCommand.InstructionWidth           = HAL_XSPI_INSTRUCTION_8_BITS;
	    sCommand.InstructionDTRMode         = HAL_XSPI_INSTRUCTION_DTR_DISABLE;

	    sCommand.Address                    = 0;
	    sCommand.AddressMode                = HAL_XSPI_ADDRESS_NONE;
	    sCommand.AddressWidth               = 0;
	    sCommand.AddressDTRMode             = 0;

	    sCommand.AlternateBytes             = 0;
	    sCommand.AlternateBytesMode         = HAL_XSPI_ALT_BYTES_NONE;
	    sCommand.AlternateBytesWidth        = 0;
	    sCommand.AlternateBytesDTRMode      = 0;

	    sCommand.DataMode                   = HAL_XSPI_DATA_NONE;
	    sCommand.DataLength                 = 0;
	    sCommand.DataDTRMode                = 0;

	    sCommand.DummyCycles                = 0;
	    sCommand.DQSMode                    = HAL_XSPI_DQS_DISABLE;

	    if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }

	    /* Configure automatic polling mode to wait for write enabling */
	    sCommand.OperationType              = HAL_XSPI_OPTYPE_COMMON_CFG;
	    sCommand.IOSelect                   = HAL_XSPI_SELECT_IO_7_0;

	    sCommand.Instruction                = 0x05; //0x05 Single mode
	    sCommand.InstructionMode            = HAL_XSPI_INSTRUCTION_1_LINE;
	    sCommand.InstructionWidth           = HAL_XSPI_INSTRUCTION_8_BITS;
	    sCommand.InstructionDTRMode         = HAL_XSPI_INSTRUCTION_DTR_DISABLE;

	    sCommand.Address                    = 0;
	    sCommand.AddressMode                = HAL_XSPI_ADDRESS_NONE;
	    sCommand.AddressWidth               = 0;
	    sCommand.AddressDTRMode             = 0;

	    sCommand.AlternateBytes             = 0;
	    sCommand.AlternateBytesMode         = HAL_XSPI_ALT_BYTES_NONE;
	    sCommand.AlternateBytesWidth        = 0;
	    sCommand.AlternateBytesDTRMode      = 0;

	    sCommand.DataMode                   = HAL_XSPI_DATA_1_LINE;
	    sCommand.DataLength                 = 1;
	    sCommand.DataDTRMode                = HAL_XSPI_DATA_DTR_DISABLE;

	    sCommand.DummyCycles                = 0;
	    sCommand.DQSMode                    = HAL_XSPI_DQS_DISABLE;

	    if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }

	    sConfig.MatchValue                  = 0x02; //0x02
	    sConfig.MatchMask                   = 0x02;  //0x02
	    sConfig.MatchMode                   = HAL_XSPI_MATCH_MODE_AND;
	    sConfig.IntervalTime                = 0x10;    //0x10
	    sConfig.AutomaticStop               = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

	    if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }
	  }

	  //Write Configuration register 2 (config Dummy cycle side Mem)

	  {

	    sCommand.OperationType              = HAL_XSPI_OPTYPE_COMMON_CFG;
	    sCommand.IOSelect                   = HAL_XSPI_SELECT_IO_7_0;

	    sCommand.Instruction                = 0x72; //0x72 single mode
	    sCommand.InstructionMode            = HAL_XSPI_INSTRUCTION_1_LINE;
	    sCommand.InstructionWidth           = HAL_XSPI_INSTRUCTION_8_BITS;
	    sCommand.InstructionDTRMode         = HAL_XSPI_INSTRUCTION_DTR_DISABLE;

	    sCommand.Address                    = 0x00000300;
	    sCommand.AddressMode                = HAL_XSPI_ADDRESS_1_LINE;
	    sCommand.AddressWidth               = HAL_XSPI_ADDRESS_32_BITS;
	    sCommand.AddressDTRMode             = HAL_XSPI_ADDRESS_DTR_DISABLE;

	    sCommand.AlternateBytes             = 0;
	    sCommand.AlternateBytesMode         = HAL_XSPI_ALT_BYTES_NONE;
	    sCommand.AlternateBytesWidth        = 0;
	    sCommand.AlternateBytesDTRMode      = 0;

	    sCommand.DataMode                   = HAL_XSPI_DATA_1_LINE;
	    sCommand.DataLength                 = 1;
	    sCommand.DataDTRMode                = HAL_XSPI_DATA_DTR_DISABLE;

	    sCommand.DummyCycles                = 0;
	    sCommand.DQSMode                    = HAL_XSPI_DQS_DISABLE;

	    if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }

	    reg =   0x07; //Configure dummy cycle to 6

	    if (HAL_XSPI_Transmit(hospi, &reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }

	     /* Wait that the memory is ready ---------------------------------- */

	     sCommand.OperationType              = HAL_XSPI_OPTYPE_COMMON_CFG;
	    sCommand.IOSelect                   = HAL_XSPI_SELECT_IO_7_0;

	    sCommand.Instruction                = 0x05; //0x05 Single mode
	    sCommand.InstructionMode            = HAL_XSPI_INSTRUCTION_1_LINE;
	    sCommand.InstructionWidth           = HAL_XSPI_INSTRUCTION_8_BITS;
	    sCommand.InstructionDTRMode         = HAL_XSPI_INSTRUCTION_DTR_DISABLE;

	    sCommand.Address                    = 0;
	    sCommand.AddressMode                = HAL_XSPI_ADDRESS_NONE;
	    sCommand.AddressWidth               = 0;
	    sCommand.AddressDTRMode             = 0;

	    sCommand.AlternateBytes             = 0;
	    sCommand.AlternateBytesMode         = HAL_XSPI_ALT_BYTES_NONE;
	    sCommand.AlternateBytesWidth        = 0;
	    sCommand.AlternateBytesDTRMode      = 0;

	    sCommand.DataMode                   = HAL_XSPI_DATA_1_LINE;
	    sCommand.DataLength                 = 1;
	    sCommand.DataDTRMode                = HAL_XSPI_DATA_DTR_DISABLE;

	    sCommand.DummyCycles                = 0;
	    sCommand.DQSMode                    = HAL_XSPI_DQS_DISABLE;

	    if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }

	    sConfig.MatchValue                  = 0x00;
	    sConfig.MatchMask                   = 0x01;
	    sConfig.MatchMode                   = HAL_XSPI_MATCH_MODE_AND;
	    sConfig.IntervalTime                = 0x10;    //0x10
	    sConfig.AutomaticStop               = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

	    if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }

	  }

	   //enable write operation (but in single line mode)
	  {
	    /* Enable write operations ---------------------------------------- */
	    sCommand.OperationType              = HAL_XSPI_OPTYPE_COMMON_CFG;
	    sCommand.IOSelect                   = HAL_XSPI_SELECT_IO_7_0;

	    sCommand.Instruction                = 0x06; //0x06: Single mode
	    sCommand.InstructionMode            = HAL_XSPI_INSTRUCTION_1_LINE;
	    sCommand.InstructionWidth           = HAL_XSPI_INSTRUCTION_8_BITS;
	    sCommand.InstructionDTRMode         = HAL_XSPI_INSTRUCTION_DTR_DISABLE;

	    sCommand.Address                    = 0;
	    sCommand.AddressMode                = HAL_XSPI_ADDRESS_NONE;
	    sCommand.AddressWidth               = 0;
	    sCommand.AddressDTRMode             = 0;

	    sCommand.AlternateBytes             = 0;
	    sCommand.AlternateBytesMode         = HAL_XSPI_ALT_BYTES_NONE;
	    sCommand.AlternateBytesWidth        = 0;
	    sCommand.AlternateBytesDTRMode      = 0;

	    sCommand.DataMode                   = HAL_XSPI_DATA_NONE;
	    sCommand.DataLength                 = 0;
	    sCommand.DataDTRMode                = 0;

	    sCommand.DummyCycles                = 0;
	    sCommand.DQSMode                    = HAL_XSPI_DQS_DISABLE;

	    if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }

	    /* Configure automatic polling mode to wait for write enabling */
	    sCommand.OperationType              = HAL_XSPI_OPTYPE_COMMON_CFG;
	    sCommand.IOSelect                   = HAL_XSPI_SELECT_IO_7_0;

	    sCommand.Instruction                = 0x05; //0x05 Single mode
	    sCommand.InstructionMode            = HAL_XSPI_INSTRUCTION_1_LINE;
	    sCommand.InstructionWidth           = HAL_XSPI_INSTRUCTION_8_BITS;
	    sCommand.InstructionDTRMode         = HAL_XSPI_INSTRUCTION_DTR_DISABLE;

	    sCommand.Address                    = 0;
	    sCommand.AddressMode                = HAL_XSPI_ADDRESS_NONE;
	    sCommand.AddressWidth               = 0;
	    sCommand.AddressDTRMode             = 0;

	    sCommand.AlternateBytes             = 0;
	    sCommand.AlternateBytesMode         = HAL_XSPI_ALT_BYTES_NONE;
	    sCommand.AlternateBytesWidth        = 0;
	    sCommand.AlternateBytesDTRMode      = 0;

	    sCommand.DataMode                   = HAL_XSPI_DATA_1_LINE;
	    sCommand.DataLength                 = 1;
	    sCommand.DataDTRMode                = HAL_XSPI_DATA_DTR_DISABLE;

	    sCommand.DummyCycles                = 0;
	    sCommand.DQSMode                    = HAL_XSPI_DQS_DISABLE;

	    if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }

	    sConfig.MatchValue                  = 0x02; //0x02
	    sConfig.MatchMask                   = 0x02;  //0x02
	    sConfig.MatchMode                   = HAL_XSPI_MATCH_MODE_AND;
	    sConfig.IntervalTime                = 0x10;    //0x10
	    sConfig.AutomaticStop               = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

	    if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }
	  }

	  //Write Configuration register 2 (config DTR OPI)
	  {
	    /* Write Configuration register 2 ---------------------------------------- */
	    sCommand.OperationType              = HAL_XSPI_OPTYPE_COMMON_CFG;
	    sCommand.IOSelect                   = HAL_XSPI_SELECT_IO_7_0;

	    sCommand.Instruction                = 0x72; //0x72 single mode
	    sCommand.InstructionMode            = HAL_XSPI_INSTRUCTION_1_LINE;
	    sCommand.InstructionWidth           = HAL_XSPI_INSTRUCTION_8_BITS;
	    sCommand.InstructionDTRMode         = HAL_XSPI_INSTRUCTION_DTR_DISABLE;

	    sCommand.Address                    = 0x00000000;
	    sCommand.AddressMode                = HAL_XSPI_ADDRESS_1_LINE;
	    sCommand.AddressWidth               = HAL_XSPI_ADDRESS_32_BITS;
	    sCommand.AddressDTRMode             = HAL_XSPI_ADDRESS_DTR_DISABLE;

	    sCommand.AlternateBytes             = 0;
	    sCommand.AlternateBytesMode         = HAL_XSPI_ALT_BYTES_NONE;
	    sCommand.AlternateBytesWidth        = 0;
	    sCommand.AlternateBytesDTRMode      = 0;

	    sCommand.DataMode                   = HAL_XSPI_DATA_1_LINE;
	    sCommand.DataLength                 = 1;
	    sCommand.DataDTRMode                = HAL_XSPI_DATA_DTR_DISABLE;

	    sCommand.DummyCycles                = 0;
	    sCommand.DQSMode                    = HAL_XSPI_DQS_DISABLE;

	    if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }

	    reg = 0x02; //enter DTR mode

	    if (HAL_XSPI_Transmit(hospi, &reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	    {
	      Error_Handler();
	    }

	    /* Wait that the configuration is effective and check that memory is ready */
	       HAL_Delay(500);

	    /* Wait that the memory is ready ---------------------------------- */
	    OSPI_AutoPollingMemReady(hospi);
	  }
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32u3xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32u3xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32U3xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32u3xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */
void USART1_IRQHandler(void);
void USART1_IRQHandler(void) //TODO we need to test overrun and get statistics
{
  uint8_t byte;

  if (LL_USART_IsEnabledIT_RXNE(USART1)&&(LL_USART_IsActiveFlag_RXNE(USART1))) { // RNXEbyte received by USART
    AddTou8fifo(&u8fifo_from_RX[BT_SERIAL], LL_USART_ReceiveData8(USART1));
  }
  
  if (LL_USART_IsEnabledIT_TXE(USART1)&&(LL_USART_IsActiveFlag_TXE(USART1))) { // TXE byte ready to send by USART
    byte = Clipu8fifo_Down(&u8fifo_to_TX[BT_SERIAL]);
    LL_USART_TransmitData8(USART1, byte);
  }
}
#if 0
void LPUART1_IRQHandler(void) 
{
  uint8_t byte;
  /* USER CODE BEGIN USART1_IRQn 0 */

  if (LL_USART_IsEnabledIT_RXNE(USART1)&&(LL_USART_IsActiveFlag_RXNE(USART1))) { // RNXEbyte received by USART
    AddTou8fifo(&u8fifo_to_SPIP[BT_STL_SERIAL], LL_USART_ReceiveData8(USART1));
  }
  
  if (LL_USART_IsEnabledIT_TXE(USART1)&&(LL_USART_IsActiveFlag_TXE(USART1))) { // TXE byte ready to send by USART
    byte = Clipu8fifo_Down(&u8fifo_from_SPIP[BT_STL_SERIAL]);
    LL_USART_TransmitData8(USART1, byte);
  }
  /* USER CODE END USART1_IRQn 0 */
  /* USER CODE BEGIN USART1_IRQn 1 */
  /* USER CODE END USART1_IRQn 1 */
}
#endif

/**
* @brief This function handles USART1 global interrupt.
*/
void USART3_IRQHandler(void);
void USART3_IRQHandler(void)
{
  uint8_t byte;
  /* USER CODE BEGIN USART2_IRQn 0 */

  if (LL_USART_IsEnabledIT_RXNE(USART3)&&(LL_USART_IsActiveFlag_RXNE(USART3))) { // RNXEbyte received by USART
    AddTou8fifo(&u8fifo_to_TX[ST_SERIAL], LL_USART_ReceiveData8(USART3));
  }
  
  if (LL_USART_IsEnabledIT_TXE(USART3)&&(LL_USART_IsActiveFlag_TXE(USART3))) { // TXE byte ready to send by USART
    byte = Clipu8fifo_Down(&u8fifo_to_TX[ST_SERIAL]);
    LL_USART_TransmitData8(USART3, byte);
  }
}
/* USER CODE END 1 */

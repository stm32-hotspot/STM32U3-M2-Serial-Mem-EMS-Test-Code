/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u3xx_hal.h"
#include "stm32u3xx_ll_usart.h"
#include "stm32u3xx_ll_rcc.h"
#include "stm32u3xx_ll_bus.h"
#include "stm32u3xx_ll_cortex.h"
#include "stm32u3xx_ll_system.h"
#include "stm32u3xx_ll_utils.h"
#include "stm32u3xx_ll_pwr.h"
#include "stm32u3xx_ll_gpio.h"
#include "stm32u3xx_ll_dma.h"

#include "stm32u3xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "printf_color.h"
    
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#ifndef countof
#define countof(a) (sizeof(a) / sizeof(*(a)))
#endif
  
#define SYS_CLK_MHZ 96 // editable to use : uint32_t SystemCoreClock = 48000000UL;
#define SYS_CLK_HZ (SYS_CLK_MHZ * 1000000)
#define BASE_TIMER_CLOCK_MHZ SYS_CLK_MHZ // (PWM LEDs.c) we use MSI 48MHz here to tune the timers for PWM dimming with 1 us period  
  
#include "commons.h"  
#include "u8fifo.h"
#include "STM32U3LQFP100_SMPS.h"
#include "io_drive.h"

#include "LEDs.h"
#include "I2C_MasterIO.h"
#include "SPI_MasterIO.h"  
  
  
#include "M24256D.h"  
  
#include "BoardSet.h"
  


#include "stm32u3xx_ll_bus.h"
#include "stm32u3xx_ll_rcc.h"  
#include "stm32u3xx_ll_pwr.h"    
#include "stm32u3xx_ll_adc.h"
#include "stm32u3xx_ll_usart.h"
#include "stm32u3xx_ll_lpuart.h"
#include "stm32u3xx_ll_gpio.h"  
  
#include "SerialComPort.h"
#include "naquada_board.h"
  
#include "app.h"  
  



/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

  
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
int32_t app_init();
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

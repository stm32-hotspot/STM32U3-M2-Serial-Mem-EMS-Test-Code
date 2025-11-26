/**
  ******************************************************************************
  * @file           : app.c
  * @brief          : application init and start
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

#include "app.h"



void App_Init(void) {

  printf(CLEAR_SCREEN);
  HAL_Delay(50);
  printf("\n\r "UNDERLINE"STM32U3 Naquada board, M.2 board memory test"RESET" \n\r\n\r");
  
  /* Counter init used for I2C w/IO */
  CNT_Init(); // configure DBG_CYCCNT core function (not available on Cortex M0)

  if(Board_Init()) // initialize MCU GPIO and corresponding functional signals going onto the board  
  {
    printf_info("Safe to "BOLD"unplug"RESET" the M.2 memory board\n\r");
    Board_WaitForUnplug();
  }
  
  
}


void App_Run(void) {
  printf_info("Safe to "BLUE"plug"RESET" the M.2 memory board\n\r");
  Board_WaitForPlug();
  printf(CLEAR_SCREEN);
  HAL_Delay(50);
  
  printf_info("start of the test, do not unplug\n\r");
  Board_App_Run();
  printf_info("Safe to "BOLD"unplug"RESET" the M.2 memory board\n\r");
  Board_WaitForUnplug();
}
















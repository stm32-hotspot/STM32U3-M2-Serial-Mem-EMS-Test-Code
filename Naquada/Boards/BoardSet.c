/**
  ******************************************************************************
  * @file           : BoardSet.c
  * @brief          : Board init and setup
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

#include "BoardSet.h"    


int32_t Board_Init(void) { // top level API for main.c
  
  Board_IO_Init();
 
  Board_Signal_Init();

  return 0;
}
  

void Board_WaitForUnplug(void) {
  do{ HAL_Delay(500); }
  while(Board_IsPlugged());
  M2_GREEN_LED_OFF;
  M2_RED_LED_OFF;
}
    
void Board_WaitForPlug(void) {
  do{ HAL_Delay(500); }
  while(!Board_IsPlugged());
}

void Board_App_Run(void) {
  HAL_Delay(500);
  Board_Run();
}


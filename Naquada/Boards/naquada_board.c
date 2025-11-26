/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#include "main.h"
//#include "naquada_board.h"
//#include "carrier_board.h"

// This source file includes all code dealing with HW Board connectivity and on-board (breakable) remote components
// All HW resources related

char SensorIslandCut = 0;


/**
  * @}
  */

void ospi_init(void) {
__HAL_RCC_OCTOSPI1_CLK_ENABLE();


  
  //-MX_OCTOSPI1_Init();

}






/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#ifndef _NAQUADA_BOARD_H_
#define _NAQUADA_BOARD_H_

extern char SensorIslandCut;

void ospi_init(void);
//void ospi_test(uint8_t);

void M95P32_Test(void);
void MX25LM51245_Test(void);
#endif
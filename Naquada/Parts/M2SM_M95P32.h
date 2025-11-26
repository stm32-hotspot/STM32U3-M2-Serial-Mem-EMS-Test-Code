/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

/*
 * m95p32_hal_xspi.h
 *
 *  Created on: May 30, 2023
 *      Author: Memory appli team
 */

#ifndef INC_M95P32_HAL_XSPI_H_
#define INC_M95P32_HAL_XSPI_H_

   
// These are the registers in the memory (max 16 of them)
typedef enum {
  M95P_STATUS_REG_INDEX, // 0
  M95P_CONFIG_REG_INDEX, // 1
  M95P_SAFETY_REG_INDEX, // 2
  M95P_VOLATILE_REG_INDEX, // 3
  M95P_REGISTERS_COUNT, // 
} ST_25UM_Regs_Index_t;

uint8_t ST_NOR_ReadStatusRegister(M2SerMem_t * pM2SM);
void ST_NOR_WriteStatusRegister(M2SerMem_t * pM2SM, uint8_t Byte);
void ST_NOR_WriteConfigRegister(M2SerMem_t * pM2SM, uint8_t Byte);

uint8_t ST_NOR_ReadConfigAndSafetyRegister(M2SerMem_t * pM2SM);

//uint8_t ST_NOR_ReadConfigurationRegister(M2SerMem_t * pM2SM);
void ST_NOR_WriteConfigurationRegister(M2SerMem_t * pM2SM, uint8_t Byte);

uint8_t ST_NOR_ReadSafetyRegister(M2SerMem_t * pM2SM);

void ST_NOR_ClearSafetyRegister(M2SerMem_t * pM2SM);
void ST_NOR_WriteSafetyRegister(M2SerMem_t * pM2SM, uint8_t Byte);

uint8_t ST_NOR_ReadVolatileRegister(M2SerMem_t * pM2SM);
void ST_NOR_WriteVolatileRegister(M2SerMem_t * pM2SM, uint8_t Byte);

void ST_NOR_ReadSFDP(M2SerMem_t * pM2SM, uint8_t* Bytes);
void ST_NOR_ReadJedec(M2SerMem_t * pM2SM, uint8_t* Bytes);
void ST_NOR_ReadID(M2SerMem_t * pM2SM, uint8_t* Bytes);

void ST_NOR_WriteEnable(M2SerMem_t * pM2SM);
void ST_NOR_WriteDisable(M2SerMem_t * pM2SM);
void ST_NOR_WaitBusyEnds(M2SerMem_t * pM2SM);

void ST_NOR_ReadInfo(M2SerMem_t * pM2SM);
void ST_NOR_EraseSegment(M2SerMem_t * pM2SM, uint32_t Addr);
void ST_NOR_ProgramData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte);

void ST_NOR_WriteData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte);
void ST_NOR_ReadAllRegisters(M2SerMem_t * pM2SM);

int8_t ST_NOR_GetImpedence(M2SerMem_t * pM2SM);
void ST_NOR_SetImpedence(M2SerMem_t * pM2SM, int8_t imp);// TODO
int32_t ST_NOR_GetDummyCycles(M2SerMem_t * pM2SM);// TODO
void ST_NOR_SetDummyCycles(M2SerMem_t * pM2SM, int32_t dummycycles);// TODO

void ST_NOR_ReadData(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);
void ST_NOR_ReadDataQuad(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);

#endif /* INC_M95P32_HAL_XSPI_H_ */
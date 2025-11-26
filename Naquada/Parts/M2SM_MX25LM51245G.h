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

#ifndef _M2SM_MX25L51245G_H_
#define _M2SM_MX25L51245G_H_

// These are the registers in the memory (max 16 of them)
typedef enum {
  MX25LM_STATUS_REG_INDEX,
  MX25LM_CONTROL_REG_INDEX,
  MX25LM_SECURITY_REG_INDEX,
  MX25LM_CR2_0,
  MX25LM_CR2_200,
  MX25LM_CR2_300,
  MX25LM_CR2_400,
  MX25LM_CR2_500,
  MX25LM_CR2_800,
  MX25LM_CR2_C00,
  MX25LM_CR2_D00,
  MX25LM_CR2_E00,
  MX25LM_CR2_F00,
  MX25LM_CR2_40000000,
  MX25LM_CR2_80000000,
  MX25LM_REGISTERS_COUNT,
} MX_25LM_Regs_Index_t;

uint8_t MX_33_NOR_ReadStatusRegister(M2SerMem_t * pM2SM);
void MX_33_NOR_WriteStatusRegister(M2SerMem_t * pM2SM, uint8_t Byte);
void MX_33_NOR_WriteConfigRegister(M2SerMem_t * pM2SM, uint8_t Byte);

uint8_t MX_33_NOR_ReadConfigAndSafetyRegister(M2SerMem_t * pM2SM);

//uint8_t MX_33_NOR_ReadConfigurationRegister(M2SerMem_t * pM2SM);
void MX_33_NOR_WriteConfigurationRegister(M2SerMem_t * pM2SM, uint8_t Byte);

uint8_t MX_33_NOR_ReadSafetyRegister(M2SerMem_t * pM2SM);

void MX_33_NOR_ClearSafetyRegister(M2SerMem_t * pM2SM);
void MX_33_NOR_WriteSafetyRegister(M2SerMem_t * pM2SM, uint8_t Byte);

uint8_t MX_33_NOR_ReadVolatileRegister(M2SerMem_t * pM2SM);
void MX_33_NOR_WriteVolatileRegister(M2SerMem_t * pM2SM, uint8_t Byte);

void MX_33_NOR_ReadSFDP(M2SerMem_t * pM2SM, uint8_t* Bytes);
void MX_33_NOR_ReadJedec(M2SerMem_t * pM2SM, uint8_t* Bytes);
void MX_33_NOR_ReadID(M2SerMem_t * pM2SM, uint8_t* Bytes);

void MX_33_NOR_WriteEnable(M2SerMem_t * pM2SM);
void MX_33_NOR_WriteDisable(M2SerMem_t * pM2SM);
void MX_33_NOR_WaitBusyEnds(M2SerMem_t * pM2SM);

void MX_33_NOR_ReadInfo(M2SerMem_t * pM2SM);
void MX_33_NOR_EraseSegment(M2SerMem_t * pM2SM, uint32_t Addr);
void MX_33_NOR_ProgramData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte);

void MX_33_NOR_WriteData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte);
void MX_33_NOR_ReadAllRegisters(M2SerMem_t * pM2SM);

int8_t MX_33_NOR_GetImpedance(M2SerMem_t * pM2SM);
void MX_33_NOR_SetImpedance(M2SerMem_t * pM2SM, int8_t imp);// TODO
int32_t MX_33_NOR_GetDummyCycles(M2SerMem_t * pM2SM);// TODO
void MX_33_NOR_SetDummyCycles(M2SerMem_t * pM2SM, int32_t dummycycles);// TODO

void MX_33_NOR_ReadData(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);
//void MX_33_NOR_ReadDataQuad(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);

void MX_33_NOR_ReadCR2(M2SerMem_t * pM2SM, uint8_t index);
void MX_33_NOR_WriteCR(M2SerMem_t * pM2SM, uint8_t Byte);

#endif /* INC_M95P32_HAL_XSPI_H_ */
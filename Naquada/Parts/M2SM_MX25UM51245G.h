/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#ifndef _M2SM_MX2UM51245G_H_
#define _M2SM_MX2UM51245G_H_

extern XSPI_HandleTypeDef hxspi1;


// These are the registers in the memory (max 16 of them)
typedef enum {
  MX25UM_STATUS_REG_INDEX,
  MX25UM_CONTROL_REG_INDEX,
  MX25UM_SECURITY_REG_INDEX,
  MX25UM_CR2_0,
  MX25UM_CR2_200,
  MX25UM_CR2_300,
  MX25UM_CR2_400,
  MX25UM_CR2_500,
  MX25UM_CR2_800,
  MX25UM_CR2_C00,
  MX25UM_CR2_D00,
  MX25UM_CR2_E00,
  MX25UM_CR2_F00,
  MX25UM_CR2_40000000,
  MX25UM_CR2_80000000,
  MX25UM_REGISTERS_COUNT,
} MX_25UM_Regs_Index_t;


uint8_t MX_18_NOR_ReadStatusRegister(M2SerMem_t * pM2SM);
void MX_18_NOR_WriteEnable(M2SerMem_t * pM2SM);
void MX_18_NOR_WriteDisable(M2SerMem_t * pM2SM);
void MX_18_NOR_WaitBusyEnds(M2SerMem_t * pM2SM);

void MX_18_NOR_ReadInfo(M2SerMem_t * pM2SM);
void MX_18_NOR_EraseSegment(M2SerMem_t * pM2SM, uint32_t Addr);
void MX_18_NOR_ProgramData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte);
void MX_18_NOR_WriteCR(M2SerMem_t * pM2SM, uint8_t Byte);
void MX_18_NOR_ReadCR2(M2SerMem_t * pM2SM, uint8_t index);
void MX_18_NOR_ReadAllCR2(M2SerMem_t * pM2SM);
void MX_18_NOR_WriteCR2(M2SerMem_t * pM2SM, uint8_t index, uint8_t Value);

void MX_18_NOR_ReadAllRegisters(M2SerMem_t * pM2SM);
int8_t MX_18_NOR_GetImpedance(M2SerMem_t * pM2SM);
void MX_18_NOR_SetImpedance(M2SerMem_t * pM2SM, int8_t imp);// TODO
int32_t MX_18_NOR_GetDummyCycles(M2SerMem_t * pM2SM);// TODO
void MX_18_NOR_SetDummyCycles(M2SerMem_t * pM2SM, int32_t dummycycles);// TODO

void MX_18_NOR_SetMode(M2SerMem_t * pM2SM, uint8_t newmode);

void MX_18_NOR_ReadCR(M2SerMem_t * pM2SM);
void MX_18_NOR_ReadSecurityRegister(M2SerMem_t * pM2SM);

void MX_18_NOR_ReadJedec(M2SerMem_t * pM2SM, uint8_t * Bytes);
void MX_18_NOR_ReadSFDP(M2SerMem_t * pM2SM, uint8_t* Bytes);

void MX_18_NOR_ReadData(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);



#endif

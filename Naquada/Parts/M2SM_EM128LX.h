/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#ifndef _M2SM_EM128LX_H_
#define _M2SM_EM128LX_H_

extern XSPI_HandleTypeDef hxspi1;

// These are the registers in the memory (max 16 of them)
typedef enum {
  EM_STATUS_REG_INDEX,
  EM_FLAG_STATUS_REG_INDEX,
  EM_CR_0000,
  EM_CR_0001,
  EM_CR_0002,
  EM_CR_0003,
  EM_CR_0004,
  EM_CR_0005,
  EM_CR_0006,
  EM_CR_0007,
  EM_CR_0008,
  EM_REGISTERS_COUNT,
} MX_25UM_Regs_Index_t;


uint8_t MX_NOR_ReadStatusRegister(M2SerMem_t * pM2SM);
void MX_NOR_WriteEnable(M2SerMem_t * pM2SM);
void MX_NOR_WriteDisable(M2SerMem_t * pM2SM);
void MX_NOR_WaitBusyEnds(M2SerMem_t * pM2SM);

void MX_NOR_ReadInfo(M2SerMem_t * pM2SM);
void MX_NOR_EraseSegment(M2SerMem_t * pM2SM, uint32_t Addr);
void MX_NOR_ProgramData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte);
void MX_NOR_WriteCR(M2SerMem_t * pM2SM, uint8_t Byte);
void MX_NOR_ReadCR2(M2SerMem_t * pM2SM, uint8_t index);
void MX_NOR_ReadAllCR2(M2SerMem_t * pM2SM);
void MX_NOR_WriteCR2(M2SerMem_t * pM2SM, uint8_t index, uint8_t Value);

void MX_NOR_ReadAllRegisters(M2SerMem_t * pM2SM);
int8_t MX_NOR_GetImpedence(M2SerMem_t * pM2SM);
void MX_NOR_SetImpedence(M2SerMem_t * pM2SM, int8_t imp);// TODO
int32_t MX_NOR_GetDummyCycles(M2SerMem_t * pM2SM);// TODO
void MX_NOR_SetDummyCycles(M2SerMem_t * pM2SM, int32_t dummycycles);// TODO

void MX_NOR_SetMode(M2SerMem_t * pM2SM, uint8_t newmode);

void MX_NOR_ReadCR(M2SerMem_t * pM2SM);
void MX_NOR_ReadSecurityRegister(M2SerMem_t * pM2SM);

void MX_NOR_ReadJedec(M2SerMem_t * pM2SM, uint8_t * Bytes);
void MX_NOR_ReadSFDP(M2SerMem_t * pM2SM, uint8_t* Bytes);

void MX_NOR_ReadData(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);

void MX_NOR_DFIM_Once(M2SerMem_t * pM2SM);


#endif

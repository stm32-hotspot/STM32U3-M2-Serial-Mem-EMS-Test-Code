/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#ifndef _M2SM_W25Q16JV_H_
#define _M2SM_W25Q16JV_H_

extern XSPI_HandleTypeDef hxspi1;

// These are the registers in the memory (max 16 of them)
typedef enum {
 
  APS6404L_REGISTERS_COUNT,
} MX_25UM_Regs_Index_t;


void MX_NOR_EnterQPIMode(M2SerMem_t * pM2SM);
void MX_NOR_LeaveQPIMode(M2SerMem_t * pM2SM);

void MX_NOR_ReadID(M2SerMem_t * pM2SM, uint8_t* Bytes);

void MX_NOR_ReadInfo(M2SerMem_t * pM2SM);
void MX_NOR_ProgramData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte);

void MX_NOR_ReadAllRegisters(M2SerMem_t * pM2SM);
int8_t MX_NOR_GetImpedence(M2SerMem_t * pM2SM);
void MX_NOR_SetImpedence(M2SerMem_t * pM2SM, int8_t imp);// TODO
int32_t MX_NOR_GetDummyCycles(M2SerMem_t * pM2SM);// TODO
void MX_NOR_SetDummyCycles(M2SerMem_t * pM2SM, int32_t dummycycles);// TODO

void MX_NOR_SetMode(M2SerMem_t * pM2SM, uint8_t newmode);

void MX_NOR_ReadData(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);

int32_t MX_NOR_GetDummyCycles(M2SerMem_t * pM2SM);
void MX_NOR_SetDummyCycles(M2SerMem_t * pM2SM, int32_t dummycycles);


#endif

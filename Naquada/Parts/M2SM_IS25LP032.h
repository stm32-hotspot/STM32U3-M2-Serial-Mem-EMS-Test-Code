/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#ifndef _M2SM_IS25LP032_H_
#define _M2SM_IS25LP032_H_


extern XSPI_HandleTypeDef hxspi1;

// These are the registers in the memory (max 16 of them)
typedef enum {
  IS25LP_STATUS_REG_INDEX, // 0
  IS25LP_FUNCTION_REG_INDEX, // 1
  IS25LP_READ_REG_INDEX, // 2
  IS25LP_EXTENDED_READ_INDEX, // 3
  IS25LP_AUTOBOOT_INDEX, // 4
  IS25LP_SECURITY_1, // 5
  IS25LP_SECURITY_2, // 6
  IS25LP_REGISTERS_COUNT,
} IS_25UM_Regs_Index_t;


uint8_t IS_NOR_ReadStatusRegister(M2SerMem_t * pM2SM);

uint8_t IS_NOR_ReadFunctionRegister(M2SerMem_t * pM2SM);
uint8_t IS_NOR_ReadParametersRegister(M2SerMem_t * pM2SM);
uint8_t IS_NOR_ReadExtendeddParametersRegister(M2SerMem_t * pM2SM);
void IS_NOR_EnterQPI_Mode(M2SerMem_t * pM2SM);
void IS_NOR_DisableQPI_Mode(M2SerMem_t * pM2SM);
void IS_NOR_WriteStatusRegister(M2SerMem_t * pM2SM, uint8_t Byte);
void IS_NOR_WriteFunctionRegister(M2SerMem_t * pM2SM, uint8_t Byte);
void IS_NOR_WriteParametersRegister(M2SerMem_t * pM2SM, uint8_t Byte);
void IS_NOR_WriteExtendeddParametersRegister(M2SerMem_t * pM2SM, uint8_t Byte);

void IS_NOR_ReadID(M2SerMem_t * pM2SM, uint8_t* Bytes, uint8_t* pSerialNb);
void IS_NOR_ReadJedec(M2SerMem_t * pM2SM, uint8_t* Bytes);
void IS_NOR_ReadSFDP(M2SerMem_t * pM2SM, uint8_t* Bytes);
void IS_NOR_ReadManufID(M2SerMem_t * pM2SM, uint8_t* pBytes);

void IS_NOR_WriteEnable(M2SerMem_t * pM2SM);
void IS_NOR_WriteDisable(M2SerMem_t * pM2SM);
void IS_NOR_WaitBusyEnds(M2SerMem_t * pM2SM);

void IS_NOR_ReadInfo(M2SerMem_t * pM2SM);
void IS_NOR_EraseSegment(M2SerMem_t * pM2SM, uint32_t Addr);
void IS_NOR_ProgramData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte);
void IS_NOR_WriteCR(M2SerMem_t * pM2SM, uint8_t Byte);
void IS_NOR_ReadCR2(M2SerMem_t * pM2SM, uint8_t index);
void IS_NOR_ReadAllCR2(M2SerMem_t * pM2SM);
void IS_NOR_WriteCR2(M2SerMem_t * pM2SM, uint8_t index, uint8_t Value);

void IS_NOR_ReadAllRegisters(M2SerMem_t * pM2SM);
int8_t IS_NOR_GetImpedence(M2SerMem_t * pM2SM);
void IS_NOR_SetImpedence(M2SerMem_t * pM2SM, int8_t imp);// TODO
int32_t IS_NOR_GetDummyCycles(M2SerMem_t * pM2SM);// TODO
void IS_NOR_SetDummyCycles(M2SerMem_t * pM2SM, int32_t dummycycles);// TODO

void IS_NOR_SetMode(M2SerMem_t * pM2SM, uint8_t newmode);

void IS_NOR_ReadCR(M2SerMem_t * pM2SM);
void IS_NOR_ReadSecurityRegister(M2SerMem_t * pM2SM);

void IS_NOR_ReadJedec(M2SerMem_t * pM2SM, uint8_t * Bytes);
void IS_NOR_ReadSFDP(M2SerMem_t * pM2SM, uint8_t* Bytes);

void IS_NOR_ReadData(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);


int32_t IS_NOR_GetDummyCycles(M2SerMem_t * pM2SM);
void IS_NOR_SetDummyCycles(M2SerMem_t * pM2SM, int32_t dummycycles);


#endif

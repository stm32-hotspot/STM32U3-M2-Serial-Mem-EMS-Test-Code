/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#ifndef __M2SMW25Q16JV_H_
#define __M2SMW25Q16JV_H_

// These are the registers in the memory (max 16 of them)
typedef enum {
  W25Q_STATUS_REG1_INDEX, // 0
  W25Q_STATUS_REG2_INDEX, // 1
  W25Q_STATUS_REG3_INDEX, // 2
  W25Q_SECURITY_REG_INDEX,
  W25Q_REGISTERS_COUNT, // 
} WB_25UM_Regs_Index_t;


uint8_t WB_NOR_ReadStatusRegister(M2SerMem_t * pM2SM);
void WB_NOR_WriteStatusRegister(M2SerMem_t * pM2SM, uint8_t Byte);
void WB_NOR_WriteConfigRegister(M2SerMem_t * pM2SM, uint8_t Byte);

uint8_t WB_NOR_ReadConfigAndSafetyRegister(M2SerMem_t * pM2SM);

//uint8_t WB_NOR_ReadConfigurationRegister(M2SerMem_t * pM2SM);
void WB_NOR_WriteConfigurationRegister(M2SerMem_t * pM2SM, uint8_t Byte);

uint8_t WB_NOR_ReadSafetyRegister(M2SerMem_t * pM2SM);

void WB_NOR_ClearSafetyRegister(M2SerMem_t * pM2SM);
void WB_NOR_WriteSafetyRegister(M2SerMem_t * pM2SM, uint8_t Byte);

uint8_t WB_NOR_ReadVolatileRegister(M2SerMem_t * pM2SM);
void WB_NOR_WriteVolatileRegister(M2SerMem_t * pM2SM, uint8_t Byte);

void WB_NOR_ReadSFDP(M2SerMem_t * pM2SM, uint8_t* Bytes);
void WB_NOR_ReadJedec(M2SerMem_t * pM2SM, uint8_t* Bytes);
void WB_NOR_ReadID(M2SerMem_t * pM2SM, uint8_t* Bytes);

void WB_NOR_WriteEnable(M2SerMem_t * pM2SM);
void WB_NOR_WriteDisable(M2SerMem_t * pM2SM);
void WB_NOR_WaitBusyEnds(M2SerMem_t * pM2SM);

void WB_NOR_ReadInfo(M2SerMem_t * pM2SM);
void WB_NOR_EraseSegment(M2SerMem_t * pM2SM, uint32_t Addr);
void WB_NOR_ProgramData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte);

void WB_NOR_WriteData(M2SerMem_t * pM2SM, uint32_t Addr, uint32_t SizeByte);
void WB_NOR_ReadAllRegisters(M2SerMem_t * pM2SM);

int8_t WB_NOR_GetImpedance(M2SerMem_t * pM2SM);
void WB_NOR_SetImpedance(M2SerMem_t * pM2SM, int8_t imp);// TODO
int32_t WB_NOR_GetDummyCycles(M2SerMem_t * pM2SM);// TODO
void WB_NOR_SetDummyCycles(M2SerMem_t * pM2SM, int32_t dummycycles);// TODO

void WB_NOR_ReadData(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);
void WB_NOR_ReadDataQuad(M2SerMem_t * pM2SM, uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);

#endif /* __M2SMW25Q16JV_H_ */
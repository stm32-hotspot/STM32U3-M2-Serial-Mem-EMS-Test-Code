/*
 * m95p32_hal_xspi.h
 *
 *  Created on: May 30, 2023
 *      Author: Memory appli team
 */

#ifndef INC_M95P32_HAL_XSPI_H_
#define INC_M95P32_HAL_XSPI_H_

#define NUMBER_OF_PAGES_ELEC 		8192
#define BYTES_PER_PAGE_ELEC	 		512
#define MEM_SIZE			 		NUMBER_OF_PAGES_ELEC*BYTES_PER_PAGE_ELEC
#define SECTOR_NB			 		1024
#define BLOCK_NB			 		64
#define SECTOR_SIZE			 		4096
#define BLOCK_SIZE			 		65536

#define CMD_ERASE_SECTOR			0x20
#define CMD_ERASE_BLOCK				0xD8
#define CMD_ERASE_CHIP				0XC7
#define CMD_ERASE_PAGE				0xDB
#define CMD_PROG_PAGE				0x0A
#define CMD_WRITE_PAGE				0x02
#define CMD_WREN					0x06
#define CMD_WRDI					0x04
#define	CMD_READ_STATUS_REG			0x05
#define CMD_READ_CONF_SAFETY_REG	0x15
#define CMD_CLEAR_SAFETY_REG		0x50
#define CMD_READ_VOLATILE_REG		0x85
#define CMD_WRITE_VOLATILE_REG		0x81
#define	CMD_WRITE_SATUTS_CONF_REG	0x01
#define CMD_READ_DATA				0x03
#define CMD_FAST_READ_SINGLE		0x0B
#define CMD_FAST_READ_DUAL			0x3B
#define CMD_FAST_READ_QUAD			0x6B
#define CMD_READ_ID_PAGE			0x83
#define CMD_FAST_READ_ID_PAGE		0x8B
#define CMD_WRITE_ID_PAGE			0x82
#define CMD_DEEP_POWER_DOWN			0xB9
#define CMD_DEEP_POWER_DOWN_RELEASE	0xAB
#define CMD_READ_JEDEC				0x9F
#define CMD_READ_SFDP				0x5A
#define CMD_ENABLE_RESET			0x66
#define CMD_SOFT_RESET				0x99

void WREN(XSPI_HandleTypeDef *local_hospi);
void WRDI(XSPI_HandleTypeDef *local_hospi);
uint8_t RDSR(XSPI_HandleTypeDef *local_hospi);
void AutoPolling(XSPI_HandleTypeDef *local_hospi);
void WRSR(XSPI_HandleTypeDef *local_hospi,uint32_t nb_data,uint8_t *pData);
void Single_Read(XSPI_HandleTypeDef *local_hospi,uint32_t add,uint32_t nb_data,uint8_t *pData);
void FAST_Read(XSPI_HandleTypeDef *local_hospi,uint32_t add,uint32_t nb_data,uint8_t *pData);
void FAST_DRead(XSPI_HandleTypeDef *local_hospi,uint32_t add,uint32_t nb_data,uint8_t *pData);
void FAST_QRead(XSPI_HandleTypeDef *local_hospi,uint32_t add,uint32_t nb_data,uint8_t *pData);
void Page_Write(XSPI_HandleTypeDef *local_hospi,uint32_t add,uint32_t nb_data,uint8_t *pData);
void Page_Prog(XSPI_HandleTypeDef *local_hospi,uint32_t add,uint32_t nb_data,uint8_t *pData);
void Page_Erase(XSPI_HandleTypeDef *local_hospi,uint32_t Add);
void Sector_Erase(XSPI_HandleTypeDef *local_hospi,uint32_t Add);
void Block_Erase(XSPI_HandleTypeDef *local_hospi,uint32_t Add);
void Chip_Erase(XSPI_HandleTypeDef *local_hospi);
void Read_ID(XSPI_HandleTypeDef *local_hospi,uint32_t add,uint32_t nb_data,uint8_t *pData);
void FAST_Read_ID(XSPI_HandleTypeDef *local_hospi,uint32_t add,uint32_t nb_data,uint8_t *pData);
void Write_ID(XSPI_HandleTypeDef *local_hospi,uint32_t add,uint32_t nb_data,uint8_t *pData);
void Deep_Power_Down(XSPI_HandleTypeDef *local_hospi);
void Deep_Power_Down_Release(XSPI_HandleTypeDef *local_hospi);
void Read_JEDEC(XSPI_HandleTypeDef *local_hospi,uint8_t nb_data,uint8_t *pData);
void ReadConfigReg(XSPI_HandleTypeDef *local_hospi,uint8_t nb_reg,uint8_t *pData);
uint8_t ReadVolatileReg(XSPI_HandleTypeDef *local_hospi);
void WriteVolatileRegister(XSPI_HandleTypeDef *local_hospi,uint8_t regVal);
void ClearSafetyFlag(XSPI_HandleTypeDef *local_hospi);
void Read_SFDP(XSPI_HandleTypeDef *local_hospi,uint32_t add,uint32_t nb_data,uint8_t *pData);
void Reset_Enable(XSPI_HandleTypeDef *local_hospi);
void Soft_Reset(XSPI_HandleTypeDef *local_hospi);

#endif /* INC_M95P32_HAL_XSPI_H_ */
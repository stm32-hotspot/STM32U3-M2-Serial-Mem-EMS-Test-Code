/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#ifndef MX25LM5145G_H
#define MX25LM5145G_H

#define NUMBER_OF_PAGES_ELEC 		                262144
#define BYTES_PER_PAGE_ELEC	 		        256
#define MEM_SIZE			 		NUMBER_OF_PAGES_ELEC*BYTES_PER_PAGE_ELEC
#define SECTOR_NB			 		16384
#define BLOCK_NB			 		1024
#define SECTOR_SIZE			 		4096
#define BLOCK_SIZE			 		65536


#define CMD_SPI_READ3B                  0x03    // Read 24 bit adr
#define CMD_SPI_FAST_READ3B             0x0B    // Fast read 24 bit adr
#define CMD_SPI_PP3B                    0x02    // page program 24 bit adr
#define CMD_SPI_SE3B                    0x20    // sector erase 24 bit adr
#define CMD_SPI_BE3B                    0xD8    // block erase 24 bit adr
#define CMD_SPI_CE                      0x60    // Chip erase 24 bit adr
#define CMD_SPI_CE_ALT                  0xC7    // Chip erase 24 bit adr

#define CMD_SPI_READ4B                  0x13    // read 32 bit adr
#define CMD_SPI_FAST_READ4B             0x0C    // fast read 32 bit adr
#define CMD_SPI_PP4B                    0x12    // page program 32 bit adr
#define CMD_SPI_SE4B                    0x21    // sector erase 32 bit adr
#define CMD_SPI_BE4B                    0xDC    // block erase 32 bit adr

#define CMD_SPI_WREN                    0x06    //* write enable
#define CMD_SPI_WRDI                    0x04    //* write disable
#define CMD_SPI_PGM_ERS_SUSPEND         0xB0    // suspend program erase cycle
#define CMD_SPI_PRG_ERS_RESUME          0x30    // resume program erase cycle
#define CMD_SPI_DP                      0xB9    // deep power down

#define CMD_SPI_SBL                     0xC0    // Set burst length
#define CMD_SPI_ENSO                    0xB1    // enter secured OTP
#define CMD_SPI_EXSO                    0xC1    // exit secured OTP

#define CMD_SPI_NOP                     0x00    // no operation
#define CMD_SPI_RSTEN                   0x66    // reset enable
#define CMD_SPI_RST                     0x99    // reset memory

#define CMD_SPI_RDID                    0x9F    //* read JEDEC identification
#define CMD_SPI_RDSFDP                  0x5A    //* read SFDP
#define CMD_SPI_RDSR                    0x05    //* read status register
#define CMD_SPI_RDCR                    0x15    // read configuration register
#define CMD_SPI_WRSR                    0x01    // write status/configuration register
#define CMD_SPI_RDCR2                   0x71    // read configuration register #2
#define CMD_SPI_WRCR2                   0x72    // write configuration register #2
#define CMD_SPI_RDFBR                   0x16    // read fast boot register
#define CMD_SPI_WRFBR                   0x17    // write fast boot register
#define CMD_SPI_ESFBR                   0x18    // erase fast boot register
#define CMD_SPI_RDSCUR                  0x2B    // read security register
#define CMD_SPI_WRSCUR                  0x2F    // write security register
#define CMD_SPI_WRLR                    0x2C    // write lock register
#define CMD_SPI_RDLR                    0x2D    // read lock register
#define CMD_SPI_WRSPB                   0xE3    // SPB bit program
#define CMD_SPI_ESSPB                   0xE4    // all SPB bit erase
#define CMD_SPI_RDSPB                   0xE2    // read SPB status
#define CMD_SPI_WRDPB                   0xE1    // write DPB register
#define CMD_SPI_RDDPB                   0xE0    // read DPB register
#define CMD_SPI_WPSEL                   0x68    // Write protect selection
#define CMD_SPI_GBULK                   0x98    // gang block unlock
#define CMD_SPI_RDPASS                  0x27    // read password register
#define CMD_SPI_WRPASS                  0x28    // write password register
#define CMD_SPI_PASSULK                 0x29    // password unlock



void WREN(XSPI_HandleTypeDef *local_hospi);
void WRDI(XSPI_HandleTypeDef *local_hospi);
uint8_t RDSR(XSPI_HandleTypeDef *local_hospi);
void RDSR_Autocomplete(XSPI_HandleTypeDef *local_ospi);
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











#endif
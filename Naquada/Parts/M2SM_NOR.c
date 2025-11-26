/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

#include "M2SM_NOR.h"


extern uint8_t *JEDECID_W25Q16JV[];
extern uint8_t *JEDECID_MX25LM51245G[];
extern uint8_t *JEDECID_MX25UM51245G[];
extern uint8_t *JEDECID_M95P32[];
extern uint8_t *JEDECID_IS25LP032[];

uint8_t JedecInfo[4]={0x00,0x00,0x00,0x00};
uint8_t **tW25Q16JV_JedecId=JEDECID_W25Q16JV;
uint8_t **tMX25LM51245G_JedecId=JEDECID_MX25LM51245G;
uint8_t **tMX25UM51245G_JedecId=JEDECID_MX25UM51245G;
uint8_t **tM95P32_JedecId=JEDECID_M95P32;
uint8_t **tIS25LP032_JedecId=JEDECID_IS25LP032;

static uint8_t SFDP[256];

extern XSPI_HandleTypeDef hxspi1;
XSPI_RegularCmdTypeDef     gCommand;
extern SerialTransaction_t ST;

M2SerMem_t M2SM = {
  .local_ospi = &hxspi1,
  .pCommand = &gCommand,
  .Config = {},
  .pDataSegment = DataSegment,
  .pST = &ST,
  .MemoryConfigurationMode = 0,
  .MemoryBusMode = {_S1, _S1, _NONE, _S1},
  .DQS_Enabled = 0,
  .RegisterDummyCycles = 20,
  
// here we add information about the memory, which we optionally downloaded from serial I2C EEPROM on board for plug and play purpose  
 // .pInfo = &SerialMemGlobalInfo, // 05 26
   .pInfo = NULL
};

M2SerMem_t * pM2SM = &M2SM;

uint8_t JedecID_Cmp(uint8_t *tJEDEC_ID1, uint8_t ***tJEDEC_ID2)
{
  uint8_t i;
  uint8_t Id;
  uint8_t IdTest=0;
  uint8_t *pJedec_Id1=tJEDEC_ID1;
  uint8_t **pJedec_Id2=*tJEDEC_ID2;
  for(Id=0;Id<NUMBEROFID;Id++)
  {
    for(i=0;i<sizeof(JedecInfo);i++)
    {
      if(pJedec_Id1[i] != pJedec_Id2[Id][i]){ IdTest=0; break;}
      else IdTest=1;
    }
    if(IdTest) break;
  }
  return IdTest;
}

//========================================================
// Check all 5 kind of memory to know which one is used
uint8_t M2SM_NOR_CheckMem(EEP_t* pEEP)
{
  MemoryName_t MemoryIndex;
  
  // prepare autopoll
  SM_ConfigInit(pM2SM);
  
  SerialMemoryTuneDelay();
  
  //Run the first read in 1 bit mode
  for(MemoryIndex=(MemoryName_t)0;MemoryIndex<NUMBER_OF_MEMORY_SUPPORTED;MemoryIndex++)
  {  
    MSM_NOR_ReadInfo[MemoryIndex](pM2SM);//JEDEC ID will be put in JedecInfo buffer
    pM2SM->pInfo = tSerialMemGlobalInfo[MemoryIndex];//Info corresponding to the memory index
    if(JedecID_Cmp(JedecInfo, pM2SM->pInfo->JEDEC_ID))// test if JedecInfo buffer is the same as the one which correspond to the memory index
    {
      return MemoryIndex;
    }
  }
       
  pM2SM->pInfo = NULL;
  return NUMBER_OF_MEMORY_SUPPORTED;
}


uint8_t WB_NOR_Test(SerialMemGlobalInfo_t *Smgi)
{
  uint32_t i=0;
  uint8_t aBytes[16];
  uint8_t OspiTest=0;
  uint8_t *pBytes=aBytes;
  
  pM2SM->pInfo = Smgi;
  
  SM_ConfigInit(pM2SM);
  SerialMemoryTuneDelay();
  WB_NOR_ReadInfo(pM2SM);
  
  /* test going to max bit SDR mode then back to 1 bit SDR mode */
  WB_NOR_ReadStatusRegister(pM2SM);
  WB_NOR_WriteEnable(pM2SM);
  WB_NOR_ReadStatusRegister(pM2SM);
  WB_NOR_WriteDisable(pM2SM);
  WB_NOR_ReadStatusRegister(pM2SM);
  WB_NOR_EraseSegment(pM2SM, 0);
    
  // going to 8 bit SDR mode
  WB_NOR_ReadInfo(pM2SM);
  WB_NOR_WriteSR2(pM2SM, 0x02); // QE bit set
  WB_NOR_SetMode(pM2SM, 1);
  WB_NOR_ReadInfo(pM2SM);
  
  DataBufferFill(0,16,0,0);
  WB_NOR_ReadInfo(pM2SM);
  DataBufferFill(0,16,0x55,0);
  WB_NOR_ProgramData(pM2SM, 0x00000000, 256);
  DataBufferFill(0,16,0,0);
  WB_NOR_ReadInfo(pM2SM);
  
  WB_NOR_ReadInfo(pM2SM);
  DataBufferFill(0,16,0x55,0);
  WB_NOR_ReadData(pM2SM, 0x00000000, pBytes, sizeof(aBytes));
  for (i = 0; i < sizeof(aBytes); i++) {
    if(aBytes[i] != pM2SM->pDataSegment[i]) {
      OspiTest=0;
      break;
    }
  }
  if(i==sizeof(aBytes)) OspiTest=1;
       
  if(OspiTest)
  {    
    WB_NOR_ReadStatusRegister(pM2SM);
    WB_NOR_WriteEnable(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
    WB_NOR_WriteDisable(pM2SM);
    WB_NOR_ReadStatusRegister(pM2SM);
    WB_NOR_EraseSegment(pM2SM, 0);
    
    DataBufferFill(0,16,0,0);
    WB_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,16,0xAA,0);
    WB_NOR_ProgramData(pM2SM, 0x00000000, 256);
    DataBufferFill(0,16,0,0);
    WB_NOR_ReadInfo(pM2SM);
    
    WB_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,16,0xAA,0);
    WB_NOR_ReadData(pM2SM, 0x00000000, pBytes, sizeof(aBytes));
    for (i = 0; i < sizeof(aBytes); i++) {
      if(aBytes[i] != pM2SM->pDataSegment[i]) {
        OspiTest=0;
        break;
      }
    }
  }
  if(i==sizeof(aBytes)) OspiTest=1;
    
    
  WB_NOR_ReadInfo(pM2SM);
  WB_NOR_WriteSR2(pM2SM, 0x00); // QE bit reset
  WB_NOR_SetMode(pM2SM, 0);
  WB_NOR_ReadInfo(pM2SM);    

  return OspiTest;
}

uint8_t MX_33_NOR_Test(SerialMemGlobalInfo_t *Smgi)
{
  uint32_t i=0;
  uint8_t Impedance = 6;
  uint8_t Mem_page[16]={0};
  uint8_t OspiTest=0;
  
  pM2SM->pInfo = Smgi;
  pM2SM->RegisterDummyCycles=20;//default
  
  // prepare autopoll
  SM_ConfigInit(pM2SM);
  SerialMemoryTuneDelay();
  //Run the first test in 1 bit mode
  MX_33_NOR_ReadInfo(pM2SM);
  //MX_33_NOR_WriteCR(pM2SM, Impedance);
  MX_33_NOR_ReadInfo(pM2SM);
  
  // test going to 8 bit SDR mode then back to 1 bit SDR mode
  // going to 8 bit SDR mode
  MX_33_NOR_WriteCR2(pM2SM, 0,0x01); // switch to 8 bit SDR mode
  MX_33_NOR_SetMode(pM2SM, 1);
  
  // do something
  MX_33_NOR_ReadInfo(pM2SM);  
  MX_33_NOR_EraseSegment(pM2SM, 0x00000000); // first 64 kbyte block
  DataBufferFill(0,16,0x55,0);
  MX_33_NOR_ProgramData(pM2SM, 0x00000000, 16);
  MX_33_NOR_ReadData(pM2SM, 0x00000000, Mem_page, 16);
  for (i = 0; i < sizeof(Mem_page); i++) {
    if(Mem_page[i] != pM2SM->pDataSegment[i]) {
      OspiTest=0;
      break;
    }
  }
  if(i==sizeof(Mem_page)) OspiTest=1;
  
  if(OspiTest)
  {
    DataBufferFill(0,16,0xAA,0);
    MX_33_NOR_ProgramData(pM2SM, 0x00000100, 16);
    MX_33_NOR_ReadData(pM2SM, 0x00000100, Mem_page, 16);
    for (i = 0; i < sizeof(Mem_page); i++) {
      if(Mem_page[i] != pM2SM->pDataSegment[i]) {
        OspiTest=0;
        break;
      }
    }
    if(i==sizeof(Mem_page)) OspiTest=1;
  
    MX_33_NOR_WriteCR(pM2SM, 0x07);
    MX_33_NOR_ReadInfo(pM2SM);
  }
  
  // going back to 1 bit SDR mode
  MX_33_NOR_WriteCR2(pM2SM, 0,0x00); // switch to 1 bit SDR mode
  MX_33_NOR_SetMode(pM2SM, 0);
  MX_33_NOR_ReadInfo(pM2SM);    
    
  
  return OspiTest;
}


uint8_t MX_18_NOR_Test(SerialMemGlobalInfo_t *Smgi)
{
  uint8_t Impedance = 6;
  uint8_t Mem_page[16];
  uint32_t i=0;
  uint8_t OspiTest=0;
  
  pM2SM->pInfo = Smgi;
  pM2SM->RegisterDummyCycles=20;//default
  
  // prepare autopoll
  SM_ConfigInit(pM2SM);
  SerialMemoryTuneDelay();
  //Run the first test in 1 bit mode
  MX_18_NOR_ReadInfo(pM2SM);
  MX_18_NOR_WriteCR(pM2SM, Impedance);
  MX_18_NOR_ReadInfo(pM2SM);
  
  // test going to 8 bit SDR mode then back to 1 bit SDR mode
  // going to 8 bit SDR mode
  MX_18_NOR_WriteCR2(pM2SM, 0,0x01); // switch to 8 bit SDR mode
  MX_18_NOR_SetMode(pM2SM, 1);
  
  // do something
  MX_18_NOR_ReadInfo(pM2SM);  
  MX_18_NOR_EraseSegment(pM2SM, 0x00000000); // first 64 kbyte block
  DataBufferFill(0,16,0x55,0);
  MX_18_NOR_ProgramData(pM2SM, 0x00000000, 16);
  MX_18_NOR_ReadData(pM2SM, 0x00000000, Mem_page, 16);
  for (i = 0; i < sizeof(Mem_page); i++) {
    if(Mem_page[i] != pM2SM->pDataSegment[i]) {
      OspiTest=0;
      break;
    }
  }
  if(i==sizeof(Mem_page)) OspiTest=1;
  
  if(OspiTest)
  {
    DataBufferFill(0,16,0xAA,0);
    MX_18_NOR_ProgramData(pM2SM, 0x00000100, 16);
    MX_18_NOR_ReadData(pM2SM, 0x00000100, Mem_page, 16);
    for (i = 0; i < sizeof(Mem_page); i++) {
      if(Mem_page[i] != pM2SM->pDataSegment[i]) {
        OspiTest=0;
        break;
      }
    }
    if(i==sizeof(Mem_page)) OspiTest=1;
  
    MX_18_NOR_WriteCR(pM2SM, 0x07);
    MX_18_NOR_ReadInfo(pM2SM);
  }
  
  // going back to 1 bit SDR mode
  MX_18_NOR_WriteCR2(pM2SM, 0,0x00); // switch to 1 bit SDR mode
  MX_18_NOR_SetMode(pM2SM, 0);
  MX_18_NOR_ReadInfo(pM2SM);    
  
  return OspiTest;
}

uint8_t ST_NOR_Test(SerialMemGlobalInfo_t *Smgi)
{
  uint32_t i=0;
  uint8_t Mem_page[16];
  uint8_t OspiTest=0;
  
  pM2SM->pInfo = Smgi;
  
   // prepare autopoll
  SM_ConfigInit(pM2SM);
  ST_NOR_SetMode(pM2SM, 0);// 1 line command mode
  
  SerialMemoryTuneDelay();
  
  ST_NOR_ReadInfo(pM2SM);
  
  ST_NOR_EraseSegment(pM2SM, 0);
    
  DataBufferFill(0,16,0x55,0);
  ST_NOR_ProgramData(pM2SM, 0x00000000, 16);
  ST_NOR_ReadDataQuad(pM2SM, 0x00000000, Mem_page, 16);
  for (i = 0; i < sizeof(Mem_page); i++) {
    if(Mem_page[i] != pM2SM->pDataSegment[i]) {
      OspiTest=0;
      break;
    }
  }
  if(i==sizeof(Mem_page)) OspiTest=1;
  
  if(OspiTest)
  {
    DataBufferFill(0,16,0xAA,0);
    ST_NOR_ProgramData(pM2SM, 0x00000100, 16);
    ST_NOR_ReadDataQuad(pM2SM, 0x00000100, Mem_page, 16);
    for (i = 0; i < sizeof(Mem_page); i++) {
      if(Mem_page[i] != pM2SM->pDataSegment[i]) {
        OspiTest=0;
        break;
      }
    }
    if(i==sizeof(Mem_page)) OspiTest=1;
  }
  
  return OspiTest;
}

uint8_t IS_NOR_Test(SerialMemGlobalInfo_t *Smgi)
{
  uint32_t i=0;
  uint8_t Mem_page[16];
  uint8_t OspiTest=0;
  
  pM2SM->pInfo = Smgi;
  
   // prepare autopoll
  SM_ConfigInit(pM2SM);
  
  IS_NOR_SetDummyCycles(pM2SM, 8);
  IS_NOR_ReadInfo(pM2SM);

  // going to 8 bit SDR mode
  IS_NOR_EnterQPI_Mode(pM2SM);
  IS_NOR_SetMode(pM2SM, 1);
    
  IS_NOR_EraseSegment(pM2SM, 0);
    
  DataBufferFill(0,16,0x55,0);
  IS_NOR_ProgramData(pM2SM, 0x00000000, 16);
  IS_NOR_ReadData(pM2SM, 0x00000000, Mem_page, 16);
  for (i = 0; i < sizeof(Mem_page); i++) {
    if(Mem_page[i] != pM2SM->pDataSegment[i]) {
      OspiTest=0;
      break;
    }
  }
  if(i==sizeof(Mem_page)) OspiTest=1;
  
  if(OspiTest)
  {
    DataBufferFill(0,16,0xAA,0);
    IS_NOR_ProgramData(pM2SM, 0x00000100, 16);
    IS_NOR_ReadData(pM2SM, 0x00000100, Mem_page, 16);
    for (i = 0; i < sizeof(Mem_page); i++) {
      if(Mem_page[i] != pM2SM->pDataSegment[i]) {
        OspiTest=0;
        break;
      }
    }
    if(i==sizeof(Mem_page)) OspiTest=1;
  }
  

  /* back to normal */
  IS_NOR_DisableQPI_Mode(pM2SM);
  IS_NOR_SetMode(pM2SM, 0);
  
  // do something
  IS_NOR_WriteParametersRegister(pM2SM, 0x03); // change the burst length we don't use
  IS_NOR_WriteExtendeddParametersRegister(pM2SM, 0xB0); // change impedance
  IS_NOR_ReadFunctionRegister(pM2SM);
  IS_NOR_ReadParametersRegister(pM2SM);
  IS_NOR_ReadExtendeddParametersRegister(pM2SM);
  IS_NOR_ReadInfo(pM2SM);
  IS_NOR_SetDummyCycles(pM2SM, 20);//default
    
  return OspiTest;
}


#if 0
static uint8_t Impedance = 6;
static volatile uint8_t test_branch = 1;
static uint8_t Mem_page[32];
void IS25LP032D_Test(void)
{
  // prepare autopoll
  SM_ConfigInit(pM2SM);
  
  //SerialMemoryTuneDelay();
//  SM_PresetCommandParameters(&gCommand, TT, 0x05, 0, 1, 0); // READ SR 
  IS_NOR_SetDummyCycles(pM2SM, 8);
  //Run the first test in 1 bit mode
//  HAL_XSPI_SetClockPrescaler(&hxspi1, 4);
  IS_NOR_ReadInfo(pM2SM);
  //IS_NOR_WriteCR(pM2SM, Impedance);
//  HAL_XSPI_SetClockPrescaler(&hxspi1, 1);
  IS_NOR_ReadInfo(pM2SM);

  switch(test_branch) {
  case 1:
    IS_NOR_ReadStatusRegister(pM2SM);
    IS_NOR_WriteEnable(pM2SM);
    IS_NOR_ReadStatusRegister(pM2SM);
    IS_NOR_WriteDisable(pM2SM);
    IS_NOR_ReadStatusRegister(pM2SM);
    
    //while(1); // stop here for now
    
    IS_NOR_EraseSegment(pM2SM, 0);
    
    DataBufferFill(0,512,0x55,1);
    IS_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,1024,0,1);
    IS_NOR_ProgramData(pM2SM, 0x00000000, 256);
    DataBufferFill(0,1024,0,0);
    IS_NOR_ReadInfo(pM2SM);
    DataBufferFill(0,1024,0x80,0xFF);
    IS_NOR_ProgramData(pM2SM, 0x00000100, 16);
    IS_NOR_ReadInfo(pM2SM);
    break;
  case 2: // test going to 8 bit SDR mode then back to 1 bit SDR mode
    // going to 8 bit SDR mode
    IS_NOR_EnterQPI_Mode(pM2SM);
    IS_NOR_SetMode(pM2SM, 1);
    DataBufferFill(0,1024,0,0);
    IS_NOR_ReadData(pM2SM, 0x00000000, DataSegment, 4096);
    
    IS_NOR_SetMode(pM2SM, 2);
    DataBufferFill(0,1024,0,0);
    IS_NOR_ReadData(pM2SM, 0x00000000, DataSegment, 4096);
    
    
    IS_NOR_DisableQPI_Mode(pM2SM);
    IS_NOR_SetMode(pM2SM, 0);
    // do something
    IS_NOR_WriteParametersRegister(pM2SM, 0x03); // change the burst length we don't use
    IS_NOR_WriteExtendeddParametersRegister(pM2SM, 0xB0); // change impedance
    IS_NOR_ReadFunctionRegister(pM2SM);
    IS_NOR_ReadParametersRegister(pM2SM);
    IS_NOR_ReadExtendeddParametersRegister(pM2SM);
    
    //IS_NOR_WriteCR(0x07);
    IS_NOR_ReadInfo(pM2SM);
    
    // going back to 1 bit SDR mode
    //IS_NOR_WriteCR2(0,0x00); // switch to 8 bit SDR mode
    
    IS_NOR_ReadInfo(pM2SM);    
    break;
  case 3: // test going to 8 bit SDR then DDR mode

    NOPs(1);
    
    break;
  case 4:
    IS_NOR_ReadInfo(pM2SM);
    NOPs(1);
    break;
  }
    
  while(1)
    NOPs(1);
}
#endif

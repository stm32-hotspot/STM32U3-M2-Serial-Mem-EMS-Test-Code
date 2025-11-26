/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */


#include "main.h"
#include "SerialMem.h"
// SerialMemory Description Structure

/*
typedef struct {
  uint32_t TT;  
  uint16_t CMD;
  uint32_t Addr;
  int8_t AddrSizeByte;
  int32_t DataSizeByte; //// negative is a read in, positive is a write out, zero means no data  
  int8_t DummyCycles;
} SerialTransaction_t;
*/
extern XSPI_HandleTypeDef hxspi1;
uint8_t DataSegment[4096];
SerialTransaction_t ST;
//SerialTransaction_t SerialTransaction;

//extern M2SerMem_t * pM2SM;

void DataBufferFill(uint32_t adr_start, uint32_t sizebyte, uint8_t value, uint8_t incstep) {
  
  for(uint32_t adr = adr_start; adr<(adr_start+sizebyte); adr++) {
    
    if(adr>countof(DataSegment))
      break;
    
    DataSegment[adr] = value;
    value = value + incstep; // can rollover 8 bit, can decrement with incstep = 0xFF
  }
  
}


void SM_ConfigInit(M2SerMem_t * pM2SM) {
  
  XSPI_AutoPollingTypeDef* pConfig = &pM2SM->Config;
  
  pConfig->MatchValue               	= 0x00;
  pConfig->MatchMask                    = 0x03;
  pConfig->MatchMode       		= HAL_XSPI_MATCH_MODE_AND;
  pConfig->IntervalTime                 = 0x10;
  pConfig->AutomaticStop   		= HAL_XSPI_AUTOMATIC_STOP_ENABLE;
}


void SM_PresetCommandParameters(M2SerMem_t * pM2SM, SerialTransaction_t* pST) { //uint32_t TT, uint16_t CMD, uint32_t Addr, uint32_t DataSizeByte, int8_t DummyCycles) {

  XSPI_RegularCmdTypeDef* pCommand = pM2SM->pCommand;
  SerialConfiguration_t * MemoryBusMode = pM2SM->MemoryBusMode;
  
  pCommand->OperationType		= HAL_XSPI_OPTYPE_COMMON_CFG; // fix for all
//  pCommand->IOSelect 			= HAL_XSPI_SELECT_IO_3_0;//HAL_XSPI_SELECT_IO_7_0; //  fix for all
  pCommand->IOSelect 			= HAL_XSPI_SELECT_IO_7_0; //  fix for all  
  pCommand->Instruction       		= pST->CMD;

//==== Work on the bus width for each phase of the transaction  
  if(MemoryBusMode[0]==_S1) {
    pCommand->InstructionMode   		= HAL_XSPI_INSTRUCTION_1_LINE;
    pCommand->InstructionWidth		        = HAL_XSPI_INSTRUCTION_8_BITS;
    pCommand->InstructionDTRMode		= HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  }
  else
    if(MemoryBusMode[0]==_S4) {
      pCommand->InstructionMode   		= HAL_XSPI_INSTRUCTION_4_LINES;
      pCommand->InstructionWidth		= HAL_XSPI_INSTRUCTION_8_BITS;
      pCommand->InstructionDTRMode		= HAL_XSPI_INSTRUCTION_DTR_DISABLE;
    }
    else
      if(MemoryBusMode[0]==_S8) {
        pCommand->InstructionMode   		= HAL_XSPI_INSTRUCTION_8_LINES;
        pCommand->InstructionWidth		= HAL_XSPI_INSTRUCTION_16_BITS;
        pCommand->InstructionDTRMode		= HAL_XSPI_INSTRUCTION_DTR_DISABLE;
      }
      else
        if(MemoryBusMode[0]==_D4) {
          pCommand->InstructionMode   		= HAL_XSPI_INSTRUCTION_4_LINES;
          pCommand->InstructionWidth		= HAL_XSPI_INSTRUCTION_8_BITS;
          pCommand->InstructionDTRMode		= HAL_XSPI_INSTRUCTION_DTR_ENABLE;
        }
        else
        if(MemoryBusMode[0]==_D8) {
          pCommand->InstructionMode   		= HAL_XSPI_INSTRUCTION_8_LINES;
          pCommand->InstructionWidth		= HAL_XSPI_INSTRUCTION_16_BITS;
          pCommand->InstructionDTRMode		= HAL_XSPI_INSTRUCTION_DTR_ENABLE;
        }
        else
          while(1); // bug
  
  if((MemoryBusMode[1]==_NONE)||( pST->AddrSizeByte==0 )) {
    pCommand->Address			= 0;
    pCommand->AddressMode       	= HAL_XSPI_ADDRESS_NONE; // vary: NONE or _1_LINE if ddd
    pCommand->AddressDTRMode		= HAL_XSPI_ADDRESS_DTR_DISABLE;
  }
  else
    if(MemoryBusMode[1]==_S1) {
      pCommand->Address			= pST->Addr; // 0 or add
      pCommand->AddressMode       	= HAL_XSPI_ADDRESS_1_LINE; // vary: NONE or _1_LINE if ddd
      pCommand->AddressDTRMode		= HAL_XSPI_ADDRESS_DTR_DISABLE;
    }
    else
      if(MemoryBusMode[1]==_S4) {
        pCommand->Address		= pST->Addr; // 0 or add
        pCommand->AddressMode       	= HAL_XSPI_ADDRESS_4_LINES; // vary: NONE or _1_LINE if ddd
        pCommand->AddressDTRMode	= HAL_XSPI_ADDRESS_DTR_DISABLE;
      }
      else
        if(MemoryBusMode[1]==_S8) {
          pCommand->Address		= pST->Addr; // 0 or add
          pCommand->AddressMode       	= HAL_XSPI_ADDRESS_8_LINES; // vary: NONE or _1_LINE if ddd
          pCommand->AddressDTRMode	= HAL_XSPI_ADDRESS_DTR_DISABLE;
        }
        else
          if(MemoryBusMode[1]==_D4) {
            pCommand->Address		= pST->Addr; // 0 or add
            pCommand->AddressMode       	= HAL_XSPI_ADDRESS_4_LINES; // vary: NONE or _1_LINE if ddd
            pCommand->AddressDTRMode	= HAL_XSPI_ADDRESS_DTR_ENABLE;
          }
          else
            if(MemoryBusMode[1]==_D8) {
              //if(pST->Addr & 1) while(1); // ODD address forbidden
              pCommand->Address		= pST->Addr; // 0 or add
              pCommand->AddressMode       	= HAL_XSPI_ADDRESS_8_LINES; // vary: NONE or _1_LINE if ddd
              pCommand->AddressDTRMode	= HAL_XSPI_ADDRESS_DTR_ENABLE;
            }
            else
              while(1);// bug

  switch(pST->AddrSizeByte) {
  case 0:     break;
  case 1:       pCommand->AddressWidth		= HAL_XSPI_ADDRESS_8_BITS;     break;
  case 2:       pCommand->AddressWidth		= HAL_XSPI_ADDRESS_16_BITS;    break;
  case 3:       pCommand->AddressWidth		= HAL_XSPI_ADDRESS_24_BITS;    break;
  case 4:       pCommand->AddressWidth		= HAL_XSPI_ADDRESS_32_BITS;    break;
  default: while(1) NOPs(1);
  }

  //=======================================
  // never used on macronix
  if(MemoryBusMode[2]==_NONE) { // MemoryBusMode[2]
    pCommand->AlternateBytes		        = 0; // fix for all
    pCommand->AlternateBytesMode		= HAL_XSPI_ALT_BYTES_NONE; // fix for all
    pCommand->AlternateBytesWidth		= HAL_XSPI_ALT_BYTES_8_BITS; // fix for all
  }
  else
    while(1) NOPs(1); // more to do
  //=======================================
  
  if((MemoryBusMode[3]==_NONE)||(pST->DataSizeByte==0)) {    
    pCommand->DataMode			= HAL_XSPI_DATA_NONE; // NONE or _1_LINE _2_LINES or _4_LINES
    pCommand->DataLength		= 0;
    pCommand->DataDTRMode		= HAL_XSPI_DATA_DTR_DISABLE; // fix for all
  }
  else
    if(MemoryBusMode[3]==_S1) {      
      pCommand->DataMode		= HAL_XSPI_DATA_1_LINE; // NONE or _1_LINE _2_LINES or _4_LINES
      pCommand->DataLength		= pST->DataSizeByte;
      pCommand->DataDTRMode		= HAL_XSPI_DATA_DTR_DISABLE; // fix for all
    }
    else
      if(MemoryBusMode[3]==_S4) {      
        pCommand->DataMode		= HAL_XSPI_DATA_4_LINES; // NONE or _1_LINE _2_LINES or _4_LINES
        pCommand->DataLength		= pST->DataSizeByte;
        pCommand->DataDTRMode		= HAL_XSPI_DATA_DTR_DISABLE; // fix for all
      }
      else
        if(MemoryBusMode[3]==_S8) {  
          pCommand->DataMode		= HAL_XSPI_DATA_8_LINES; // NONE or _1_LINE _2_LINES or _4_LINES
          pCommand->DataLength		= pST->DataSizeByte;
          pCommand->DataDTRMode		= HAL_XSPI_DATA_DTR_DISABLE; // fix for all
        }
        else
          if(MemoryBusMode[3]==_D4) {      
            pCommand->DataMode		= HAL_XSPI_DATA_4_LINES; // NONE or _1_LINE _2_LINES or _4_LINES
            pCommand->DataLength	= pST->DataSizeByte;
            pCommand->DataDTRMode	= HAL_XSPI_DATA_DTR_ENABLE; // fix for all
          }
          else
            if(MemoryBusMode[3]==_D8) {      
              //if(pST->DataSizeByte & 1) while(1) ; // can't use odd number of bytes in DDR // not true for AP Memory Control Registers
              pCommand->DataMode	= HAL_XSPI_DATA_8_LINES; // NONE or _1_LINE _2_LINES or _4_LINES
              pCommand->DataLength	= pST->DataSizeByte;
              pCommand->DataDTRMode	= HAL_XSPI_DATA_DTR_ENABLE; // fix for all
            }
            else
              while(1); // bug

  if(pST->DummyCycles>=0)
    pCommand->DummyCycles		= pST->DummyCycles; // 0 or 8
  else
    pCommand->DummyCycles		= pM2SM->RegisterDummyCycles; // 0 or 8 or 6 with DQS
  
  if(pM2SM->DQS_Enabled)
    pCommand->DQSMode			= HAL_XSPI_DQS_ENABLE; // fix for all
  else
    pCommand->DQSMode			= HAL_XSPI_DQS_DISABLE; // fix for all    
  
  pCommand->SIOOMode			= HAL_XSPI_SIOO_INST_EVERY_CMD; // fix for all
}



void SM_DoTransactionCMD(M2SerMem_t * pM2SM)
{
  XSPI_RegularCmdTypeDef* pCommand = pM2SM->pCommand;//&gCommand;

  if (HAL_XSPI_Command(pM2SM->local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    Error_Handler();
}

void SM_DoTransactionWrite(M2SerMem_t * pM2SM, uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = pM2SM->pCommand;//&gCommand;

  if (HAL_XSPI_Command(pM2SM->local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    Error_Handler();

  if (HAL_XSPI_Transmit(pM2SM->local_ospi, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    Error_Handler();
  
}

void SM_DoTransactionRead(M2SerMem_t * pM2SM, uint8_t *pData)
{
  XSPI_RegularCmdTypeDef* pCommand = pM2SM->pCommand;//&gCommand;

  if (HAL_XSPI_Command(pM2SM->local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    Error_Handler();

  if (HAL_XSPI_Receive(pM2SM->local_ospi, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    Error_Handler();
}


void SM_DoTransactionPoll(M2SerMem_t * pM2SM)
{
  XSPI_RegularCmdTypeDef* pCommand = pM2SM->pCommand;//&gCommand;
  XSPI_AutoPollingTypeDef* pConfig = &pM2SM->Config;//&gConfig;
  
  if (HAL_XSPI_Command(pM2SM->local_ospi, pCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    Error_Handler();

  if (HAL_XSPI_AutoPolling(pM2SM->local_ospi, pConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    Error_Handler();
  
}















//=====================================================================================
// This is the code to validate the driver with macronix (first baby step)

// Now here is the generic universal memory interface with write and read only

void MX_NOR_ReadData(uint32_t Addr, uint8_t* pBytes, uint32_t ByteSize);

uint8_t SerialMemoryReadData(XSPI_HandleTypeDef *local_ospi, M2SerMem_t* psm, uint32_t Addr, uint8_t* pb, uint32_t SizeByte);

uint8_t SerialMemoryReadData(XSPI_HandleTypeDef *local_ospi, M2SerMem_t* psm, uint32_t Addr, uint8_t* pb, uint32_t SizeByte) {
 
  // first we need to know if the address or size is incompatible with DDR
  // if it is, we need to manually read the beginning and end properly
  // check valid range
  uint32_t adr;
  if((Addr+SizeByte)>((psm->pInfo->SizekByte) * 1024L)) // 05 26
    while(1); // out of range
  if(SizeByte==0)
    return 0;
  
  // now we need to take care of the odd address or odd number of bytes to read
  uint8_t buf[2]; // always read bigger in case size is 1
  if(Addr & 1) { // DDR only
    adr = Addr & 0xFFFFFFFE;
    MX_NOR_ReadData(adr,buf,2);
    pb[0] = buf[1];
    Addr = adr + 1;
    SizeByte = SizeByte - 1; // first byte read, deducted. Start address increased to even one
    if(SizeByte==0)
      return 0; // done
  }
  
  // Start address is now even. how about number of bytes to read ?
  if(SizeByte&1) { // DDR only. odd number of bytes
    adr = Addr + SizeByte - 1;
    MX_NOR_ReadData(adr,buf,2);
    pb[SizeByte-1] = buf[0];
    SizeByte = SizeByte - 1;
  }
  
  // now we do the one block read command which works in DDR
  MX_NOR_ReadData(Addr,pb,SizeByte);

  return 0;
}

uint8_t SerialMemoryWriteData(XSPI_HandleTypeDef *local_ospi, M2SerMem_t* psm, uint32_t Addr, uint8_t* pb, uint32_t SizeByte);

uint8_t SerialMemoryWriteData(XSPI_HandleTypeDef *local_ospi, M2SerMem_t* psm, uint32_t Addr, uint8_t* pb, uint32_t SizeByte) {

  // first we need to know if the address or size is incompatible with DDR
  // if it is, we need to manually read the beginning and end properly
  // check valid range
//  uint32_t adr;
  if((Addr+SizeByte)>((psm->pInfo->SizekByte) * 1024L)) // 05 26
    while(1); // out of range
  if(SizeByte==0)
    return 0;
  
  // writing works with sectors of 4096 bytes
  // we use the sector buffer to slice the writing
  // we should also check previous data content to know if erase is necessary
  // save the erase cycles, prolong memory lifespan
  uint32_t sliceadr;
  uint8_t neederase = 0;
  uint32_t erasesize = psm->pInfo->ChosenErasableAreaByteSize; // 05 26
  uint32_t chunksize = psm->pInfo->RecommendedMinDataWriteChunkCRCSize; // 05 26
//  uint32_t startprogadr;
//  uint32_t endprogadr;
  
  // get the first sector including the address until we cover the specified block end address 
  for(uint32_t segadr = Addr & erasesize; segadr < (Addr+SizeByte); segadr+= erasesize)
  {
    MX_NOR_ReadData(segadr,psm->pDataSegment,erasesize); // read the whole sector 

    sliceadr = max2(Addr,segadr) & chunksize;
    
    for(uint32_t i=sliceadr;i<(segadr+erasesize);i++)
    {
      if((psm->pDataSegment[i] & pb[i-Addr])!=pb[i-Addr])
      { // need to flip some bits from 0 to 1, so need erase phase
        neederase = 1;
        break;
      }
    }
    
    if(neederase)
    { // erase the sector
void MX_NOR_EraseSegment(uint32_t Addr);      
      MX_NOR_EraseSegment(segadr);
    }
    
    // program the new data in the entire sector (can be made more elegant)
    for(uint32_t i=sliceadr;i<(segadr+erasesize);i++)
    {
      if(i<Addr) continue;
      if(i>=(Addr+SizeByte)) continue;
      // replace data
      psm->pDataSegment[i] = pb[i-Addr]; // replace flash cache data by new data before programming
    }

    for(uint32_t i=sliceadr;i<(segadr+erasesize);i=i+psm->pInfo->PageSizeByte) {
void MX_NOR_ProgramData(uint32_t Addr, uint32_t SizeByte);
      MX_NOR_ProgramData(i, psm->pInfo->PageSizeByte); // 4096/256=16 times.
    }
  
  
  }
  
  return 0;
}

//================= Delay block related

LL_DLYB_CfgTypeDef dlyb_cfg, dlyb_cfg_test;
uint32_t OCTOSPI1_Freq;


void SerialMemoryTuneDelay(void) {
  
/*****************************************Delay block configuration*******************************************************/
  /* get OCTOSPI frequency */
  OCTOSPI1_Freq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_OCTOSPI1);

  /* Delay block configuration ------------------------------------------------ */
  if (HAL_XSPI_DLYB_GetClockPeriod(&hxspi1,&dlyb_cfg) != HAL_OK)
  {
    while(1){};
  }

  /*when DTR, PhaseSel is divided by 4 (empirical value)*/
  dlyb_cfg.PhaseSel /=4;

  /* save the present configuration for check*/
  dlyb_cfg_test = dlyb_cfg;

  /*set delay block configuration*/
  if(HAL_XSPI_DLYB_SetConfig(&hxspi1,&dlyb_cfg) != HAL_OK )
  {
    Error_Handler() ;
  }

  /*check the set value*/
  if(HAL_XSPI_DLYB_GetConfig(&hxspi1,&dlyb_cfg) != HAL_OK )
  {
    Error_Handler() ;
  }

  if ((dlyb_cfg.PhaseSel != dlyb_cfg_test.PhaseSel) || (dlyb_cfg.Units != dlyb_cfg_test.Units))
  {
    while(1){};
  }

  /*****************************************Delay block configuration*******************************************************/

  
}

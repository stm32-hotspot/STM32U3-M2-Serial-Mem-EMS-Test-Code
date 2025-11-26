/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */



#include "M2SM_EEP_PNP.h"
#include "M2SM_EEP_Info.h"

extern SerialMemGlobalInfo_t *tSerialMemGlobalInfo[];
int8_t volatile debug_fork = 0;
uint8_t M2SM_Detected = 0;

uint8_t EEP_Data1kbyte[1024]; // the buffer to read 1k piece

uint8_t M2SM_IsBoardDetected(I2C_SlaveDevice_t* pD) {
  
 return I2C_MasterIO_IsSlavePresent(pD);
}



int8_t M2SM_EEP_Read1kbyte(EEP_t* pEEP, EEP_Info_Bases_t Adr) {
  
  
  return EEP_ReadBytes (pEEP, Adr, EEP_Data1kbyte, 1024);
}


int8_t M2SM_EEP_Write1kbyte(EEP_t* pEEP, EEP_Info_Bases_t Adr) {
  
  EEP_ReadID(pEEP); // if answering
  return EEP_WriteBytes (pEEP, Adr, EEP_Data1kbyte, 1024);
}

int8_t M2SM_EEP_Erase1kbyte(EEP_t* pEEP, EEP_Info_Bases_t Adr) {
  
  EEP_ReadID(pEEP); // if answering
  return EEP_EraseBytes (pEEP, Adr, 1024);
}

void M2SM_EEP_PNP_init(EEP_t* pEEP) {
  
  // nothing to do ?
  M2SM_Detected = 0; // memory is not detected
}


uint16_t EEPROM_BASE = M2SM_INFO_BASE;

EEP_INFO_t M2SM_EEP_PNP_polling(EEP_t* pEEP) {
  
  EEP_INFO_t Eep;
  uint8_t BufferCheck_OK = 0;
    
//  HAL_Delay(100);//to avoid to be stuck and have SDA@0?
  
  Eep.Detected = M2SM_IsBoardDetected(&gM2SM_M24256D);
  if(Eep.Detected) NOPs(1);
  
  Eep.Detected_Id = M2SM_IsBoardDetected(&gM2SM_M24256D_ID);
  if(Eep.Detected_Id) NOPs(2);

#ifdef DEBUG  
  if(Eep.Detected) printf_info("M.2 board EEPROM ID "GREEN"detected"RESET" \n\r");
  else printf_error("M.2 board EEPROM ID"RED"not detected"RESET" \n\r");
  if(Eep.Detected_Id) printf_info("M.2 board EEPROM OTP ID  "GREEN"detected"RESET" \n\r");
  else printf_error("M.2 board EEPROM OTP ID"RED"not detected"RESET" \n\r");
#endif /* DEBUG */
  
  if((Eep.Detected)&&(Eep.Detected_Id))
  {
    printf_info("EEPROM IDs "GREEN"detected"RESET" \n\r");
  }
  else
  {
    printf_error("EEPROM IDs "RED"not detected"RESET" \n\r");
  }

#ifdef DEBUG  
  printf_info("Board CPN available : \n\r");
  for(Eep.M2SM_Index=0; Eep.M2SM_Index<NUMBER_OF_MEMORY_SUPPORTED; Eep.M2SM_Index++)
  {
    printf("- %s \n\r", tCPN[Eep.M2SM_Index]);
  }
#endif /* DEBUG */
  
  /* Try to read mem info */
  if(Eep.Detected && Eep.Detected_Id)
  {
    EEP_ReadID(pEEP);
      
    printf_info("EEPROM CPN : %s\n\r",((M2SM_ID_INFO_t *)(pEEP->ID_Page))->CPN);
    
    for(Eep.M2SM_Index=0;Eep.M2SM_Index<NUMBER_OF_MEMORY_SUPPORTED;Eep.M2SM_Index++)
    {
      if (StringsIdenticals(((M2SM_ID_INFO_t *)(pEEP->ID_Page))->CPN, tCPN[Eep.M2SM_Index], sizeof(tCPN))) break;
    }
    /* At this point Eep.M2SM_Index is set by string compare */
    
    /* Check the content of the eeprom */
    if(Eep.M2SM_Index>=NUMBER_OF_MEMORY_SUPPORTED)
    {
      printf_error("Board is "RED"not implemented"RESET"\n\r");
    }
    else
    {
      BufferCheck_OK = BytesIdenticals(pEEP->ID_Page, (uint8_t *)tM2SM_INFO[Eep.M2SM_Index], sizeof(pEEP->ID_Page));
      M2SM_EEP_Read1kbyte(pEEP, M2SM_INFO_BASE);
      BufferCheck_OK += BytesIdenticals(EEP_Data1kbyte, (uint8_t *)tM2SM_INFO[Eep.M2SM_Index], sizeof(M2SM_ID_INFO_t));
      M2SM_EEP_Read1kbyte(pEEP, M2SM_MEMA_BASE);
      BufferCheck_OK += BytesIdenticals(EEP_Data1kbyte, (uint8_t *)tSerialMemGlobalInfo[Eep.M2SM_Index], sizeof(SerialMemGlobalInfo_t));
    }
    
    
    //CopyFromToByteSize (EEP_Data1kbyte, &M2SM_INFO, sizeof(M2SM_ID_INFO_t));
    
    if (BufferCheck_OK == 3)
    {
      
      //printf_info("EEPROM was properly "GREEN"filled"RESET"\n\r");
      //printf("\n\r Board CPN read : %s \n\r", M2SM_INFO.CPN);
      //printf("\n\r Board CPN read : %s \n\r", tM2SM_INFO[m2sm_index]->CPN);
      M2_RED_LED_OFF;
      Eep.Empty=0;
    }
    else
    {
      printf_error("Board EEPROM "RED" empty or corrupted !!! "RESET" \n\r");
      M2_RED_LED_ON;
      Eep.Empty=1;
      printf_error(""BG_RED"Please reset the system in case of hang up"RESET" \n\r");
    }
  }

  //printf_info("Eep: detected (%d), detectd Id (%d), empty (%d), index (%d), max (%d)\n\r",Eep.Detected, Eep.Detected_Id, Eep.Empty, Eep.M2SM_Index, Eep.M2SM_Max);//debug
  
  Eep.M2SM_Max = NUMBER_OF_MEMORY_SUPPORTED;
  return Eep;
}


uint8_t M2SM_EEP_PNP_filling(EEP_t* pEEP, uint8_t MemoryType)
{
  CopyFromToByteSize (tM2SM_INFO[MemoryType], pEEP->ID_Page, sizeof(pEEP->ID_Page));    
  //EEP_EraseBytes (pEEP, M2SM_INFO_BASE, sizeof(pEEP->ID_Page));
  EEP_WriteID(pEEP);
  CopyFromToByteSize (tM2SM_INFO[MemoryType], EEP_Data1kbyte, sizeof(M2SM_ID_INFO_t));
  //M2SM_EEP_Erase1kbyte(pEEP,M2SM_INFO_BASE);
  M2SM_EEP_Write1kbyte(pEEP,M2SM_INFO_BASE);
  CopyFromToByteSize (tSerialMemGlobalInfo[MemoryType], EEP_Data1kbyte, sizeof(SerialMemGlobalInfo_t));
  //M2SM_EEP_Erase1kbyte(pEEP,M2SM_MEMA_BASE);
  M2SM_EEP_Write1kbyte(pEEP,M2SM_MEMA_BASE);
  return 0;
}



void M2SM_LED_UpdateStatus(uint8_t LED_RedOn, uint8_t LED_GreenOn) {
  
  if(LED_RedOn)
    BriskBlinkTimedLED(10, 500, 500, 5 );

  if(LED_GreenOn)
    BriskBlinkTimedLED(11, 500, 500, 5 );
  
//BriskBlinkTimedLED(uint8_t index, uint16_t Blink_Period_ms, uint16_t Blink_OffTime_ms, uint8_t Blink_Countdown );  
}


/* Save OSPI test result within 2 bytes, 1 byte for the test number, 1 byte for the result
=> 256 tests max, the size is 512 bytes */
#define TEST_MEMORY_SIZE 512
#define TEST_MAX_NUMBER  256
void M2SM_EEP_PNP_saving(EEP_t* pEEP, uint8_t TestResult)
{
  uint32_t index=0;
  
  /* clean reception buffer */
  for(index=0;index<sizeof(EEP_Data1kbyte);index++) EEP_Data1kbyte[index]=0;
  
  /* Read all memory test status*/
  M2SM_EEP_Read1kbyte(pEEP, M2SM_TESTINFO_BASE);
  
  /* Search next byte to the last test */
  for(index=0;index<TEST_MAX_NUMBER;index++) if(EEP_Data1kbyte[index*2+1]==0xFF) break;// result should be 0 or 1 if a test was done, 0xff in other case
  
  /* Check buffer is nof full, and store a result if not */
  if(index<TEST_MAX_NUMBER)
  {
    EEP_Data1kbyte[index*2] = index;//store test number
    EEP_Data1kbyte[index*2+1] = TestResult;//store test result
    
    /* update result in eeprom */
    M2SM_EEP_Write1kbyte(pEEP, M2SM_TESTINFO_BASE);
  }
}

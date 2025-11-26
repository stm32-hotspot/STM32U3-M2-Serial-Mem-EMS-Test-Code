/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

// This describes Green Naquada board elements when the naquada sensor board has been cut off

#include "main.h"
#include "M24256D.h"
#include "M2SM_EEP_PNP.h"

#ifdef MB2095B_MCU



//===================


I2C_MasterIO_t gI2C_Bus_Sensor; // Same I2C bus for both DIP28 and STMod+ PA11=SCL2, PA12=SDA2
const IO_Pad_t MIO_SDA_Sensor = { U8_4, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };
const IO_Pad_t MIO_SCL_Sensor = { U8_1, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };

I2C_MasterIO_t gI2C_Bus_DigitalHeader; // Same I2C bus for both DIP28 and STMod+ PA11=SCL2, PA12=SDA2
const IO_Pad_t MIO_SDA_DigitalHeader = { CN3_14 /*PB_9*/, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };
const IO_Pad_t MIO_SCL_DigitalHEader = { CN3_13 /*PB_8*/, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };

  // I2C for M2
I2C_MasterIO_t gI2C_Bus_M2SerialMem; 
//const IO_Pad_t MIO_SDA_M2SerialMem = { PA_6, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };
//const IO_Pad_t MIO_SCL_M2SerialMem = { PB_2, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };  
const IO_Pad_t MIO_SDA_M2SerialMem = { PA_6, {.Mode = IO_OUTPUT, .Pull = IO_NOPULL, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };
const IO_Pad_t MIO_SCL_M2SerialMem = { PB_2, {.Mode = IO_OUTPUT, .Pull = IO_NOPULL, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };  




const IO_Pad_t UserKeyPin_ANALOG =      { PD_11, { .Mode = IO_ANALOG, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 } }; // as digital push-in button, can also be used as analog keyboard...
const IO_Pad_t UserKeyPin_EXTI =        { PD_11, { .Mode = IO_INPUT, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 } }; // as digital push-in button, can also be used as analog keyboard...



// Green Naquada (B)
  const IO_Pad_t QSPI_IO1 = {  /*U11_5*/  PB_0,  { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO1, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO2 = {  /*U11_3*/  PA_7,  { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO2, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO3 = {  /*U11_7*/  PC_6,  { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO3, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_NCS = {  /*U11_1*/  PE_11, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_NCS, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO0 = {  /*U11_5*/  PE_12, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO0, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_CLK = {  /*U11_6*/  PE_10, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_CLK, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   

// going to ST M2 connector
  const IO_Pad_t QSPI_IO4 =  { PD_4, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO4, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO5 =  { PC_2, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO5, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO6 =  { PC_3, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO6, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO7 =  { PD_7, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO7, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_DQS =  { PE_3, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_DQS, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_NCLK = { PB_5, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_NCLK, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  
// Power contorl of memories
const IO_Pad_t QSPI_LDOEN = { PE_13, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   // SPI_SCK shared as LDO2_EN for BLUE BOARDS

const IO_Pad_t QSPI_LDO1EN = { PE_0,  { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   // SPI_SCK shared as LDO2_EN
const IO_Pad_t QSPI_LDO2EN = { PA_15, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   // SPI_SCK shared as LDO2_EN

const IO_Pad_t QSPI_LED1 /* GREEN */ = { PB_8, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   // LED is off when output high
const IO_Pad_t QSPI_LED2 /* RED */   = { PE_8, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   // LED is off when output high



IO_Pad_t BT_TX_Pad = { CN5_4 /*J1_22*/ /*PA_9*/,  { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = USART1_TX } };
IO_Pad_t BT_RX_Pad = { CN5_3 /*J1_20*/ /*PA_10*/, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = USART1_RX } };


IO_Pad_t ST_TX_Pad = { PD_8, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = USART3_TX } };
IO_Pad_t ST_RX_Pad = { PD_9, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = USART3_RX } };

IO_Pad_t MM_UART_TX1_Pad = { J1_17 /*PB_10*/, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = LPUART1_TX } }; // USART1_TX alternate option 
IO_Pad_t MM_UART_RX1_Pad = { J1_19 /*PB_11*/, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = LPUART1_RX } }; // USART1_RX alternate option

  const IO_Pad_t QSPI_NCS_DEBUG = {  /*U11_1*/  PE_11, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   

  
  
  
//==================

// LEDs  
const IO_Pad_t LED6_AsOutput = {  PD_3, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH } }; // LED6

const IO_Pad_t LEDC1_AsOutput = {  PE_7, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH } }; // LED1

const IO_Pad_t LEDC2_AsOutput = {  PE_6, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH } }; // LED2
const IO_Pad_t LEDC2_AsPWM =    {  PE_6, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH, .Signal = TIM3_CH4 } }; 

const IO_Pad_t LEDC3_AsOutput = {  PE_2, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH } }; // LED3


  



TimedLED_t TimedLEDs[] = { // LEDs are off and not blinking by default
  { 0, 0, 0, 0,  0, 0,},
  { (IO_Pad_t*) 0, 0, 0, 0,  0, 0,   }, //1
  { (IO_Pad_t*) 0, 0, 0, 0,  0, 0,   }, //2    
  { (IO_Pad_t*) 0, 0, 0, 0,  0, 0,   }, //3
  { (IO_Pad_t*) 0, 0, 0, 0,  0, 0,   }, //4  // This LED does nothing  
  { (IO_Pad_t*) 0, 0, 0, 0,  0, 0,   }, //5  // This LED  
  { (IO_Pad_t*)&LED6_AsOutput, 0, 0, 0,  0, 0,   }, //6
  { (IO_Pad_t*)&LEDC1_AsOutput, 0, 0, 0,  0, 0,   }, //7
  { (IO_Pad_t*)&LEDC2_AsPWM, 0, 0, 0,  0, 0,   }, //8
  { (IO_Pad_t*)&LEDC3_AsOutput, 0, 0, 0,  0, 0,   }, //9
// M2 optional dual RED and GREEN LEDs
  { (IO_Pad_t*)&LEDC3_AsOutput, 0, 0, 0,  0, 0,   }, //10
  { (IO_Pad_t*)&QSPI_LED1, 0, 0, 0,  0, 0, 1,  }, //11
  { (IO_Pad_t*)&QSPI_LED2, 0, 0, 0,  0, 0, 1,  }, //12
};


uint8_t LED_COUNT = countof(TimedLEDs);  
  
  
  
  
  
int32_t Board_IO_Init(void) {

  // Init OSPI interface
  IO_PadInit((IO_Pad_t*)&QSPI_IO0);
  IO_PadInit((IO_Pad_t*)&QSPI_IO1);
  IO_PadInit((IO_Pad_t*)&QSPI_IO2);
  IO_PadInit((IO_Pad_t*)&QSPI_IO3);
  IO_PadInit((IO_Pad_t*)&QSPI_NCS);
  IO_PadInit((IO_Pad_t*)&QSPI_CLK);


  // CARRIER BOARD GOING SIGNALS TO M2/SerialMem
  // Activate serial memory power
//BUG  IO_PadInit((IO_Pad_t*)&QSPI_LDOEN);
  do {
    IO_PadInit((IO_Pad_t*)&QSPI_LDOEN);
    IO_PadInit((IO_Pad_t*)&QSPI_LDO1EN);
    //IO_PadInit((IO_Pad_t*)&QSPI_LDO2EN);
    IO_PadSetLow(QSPI_LDO1EN.PadName);
    IO_PadSetLow(QSPI_LDO2EN.PadName);
    IO_PadSetLow(QSPI_LDOEN.PadName); // blue boards
    HAL_Delay(250);  // Cycle power off power on to reset any memory (even if they don't have a reset)
    IO_PadSetHigh(QSPI_LDO1EN.PadName);
    IO_PadSetHigh(QSPI_LDO2EN.PadName);
    IO_PadSetHigh(QSPI_LDOEN.PadName); // blue boards
//    HAL_Delay(500);
  }while(0);
  
//  IO_PadInit((IO_Pad_t*)&QSPI_NCS_DEBUG);
  while(0) { // NCS was poor contact and sometime stay low at memory side. This is to debug...
    IO_PadSetLow(QSPI_NCS_DEBUG.PadName);
    HAL_Delay(1);
    IO_PadSetHigh(QSPI_NCS_DEBUG.PadName);
    HAL_Delay(1);
  } ;
  
  
  IO_PadInit((IO_Pad_t*)&QSPI_LED1);
  IO_PadInit((IO_Pad_t*)&QSPI_LED2);

  IO_PadInit((IO_Pad_t*)&QSPI_IO4);
  IO_PadInit((IO_Pad_t*)&QSPI_IO5);
  IO_PadInit((IO_Pad_t*)&QSPI_IO6);
  IO_PadInit((IO_Pad_t*)&QSPI_IO7);
  IO_PadInit((IO_Pad_t*)&QSPI_DQS);
  //IO_PadInit((IO_Pad_t*)&QSPI_NCLK);

  
  return 0;
}

uint8_t ospi_checkmem(EEP_t* pEEP);
uint8_t ospi_test(uint8_t);

// STMod+ Memory and I2C bus
I2C_SlaveDevice_t gM2SM_M24256D =       { &gI2C_Bus_M2SerialMem, 0xA2, 2 }; // plug and play on STMod+
I2C_SlaveDevice_t gM2SM_M24256D_ID =    { &gI2C_Bus_M2SerialMem, 0xB2, 2 }; // plug and play on STMod+
EEP_t M2SM_M24256D = { &gM2SM_M24256D, &gM2SM_M24256D_ID };

uint8_t Board_Signal_Init(void) {

  int32_t Minbps = 30000;
  int32_t Maxbps = 50000;
  
  printf_info("EEPROM bus initialization\n\r"); 
  
  I2C_MasterIO_Init(&gI2C_Bus_M2SerialMem);
  I2C_MasterIO_ConfigHW(&gI2C_Bus_M2SerialMem, (IO_Pad_t*) &MIO_SDA_M2SerialMem, (IO_Pad_t*) &MIO_SCL_M2SerialMem);
  if(!I2C_MasterIO_ConfigTimings(&gI2C_Bus_M2SerialMem, Minbps, Maxbps))
  {
    printf_error("EEPROM bus "RED"timing configuration problem"RESET" : change MEM board or power off/on \n\r");
    while(1);
  }

#ifdef DEBUG
  printf_info("EEPROM bus "GREEN"timing configuration done"RESET", wait parameter = %d cycles, bitrate = %d kHz \n\r", gI2C_Bus_M2SerialMem.WaitParam, gI2C_Bus_M2SerialMem.bitrate_kHz);
#endif /* DEBUG */  
//  ErrorRecovery(&gI2C_Bus_M2SerialMem);
  
  uint16_t StartAdr;
  // we sweep the slaves
  
  do {
  
    StartAdr = 0;
    while(StartAdr = I2C_MasterIO_FindNextSlaveByAddress(&gI2C_Bus_M2SerialMem, StartAdr)) {

        gSlaveSweep = StartAdr; // breakpoint here to stop as soon as one slave is detected
        NOPs(1); // put breakpoint here
    }; // when StartAdr == 0, it means the sweep is over
    
    NOPs(1); // breakpoint hook to view gSlaveSweep
  } while(0);

#ifdef DEBUG  
  printf_info("EEPROM bus initialization done, device 0x%x found\n\r", gSlaveSweep);
#endif /* DEBIG */  
  
  return Board_Run();  
}


extern XSPI_HandleTypeDef hxspi1;
#define hospi1 hxspi1

extern const IO_Pad_t QSPI_LED1;
extern const IO_Pad_t QSPI_LEDL;
uint8_t Rcv[512] ={0};
uint8_t SR;

void ClearRcv(void);
void ClearRcv(void) {

  uint32_t index;
  for(index=0; index<512; index++)
    Rcv[index]= 0;
}

extern uint8_t M2SM_NOR_CheckMem(EEP_t* pEEP);
uint8_t ospi_checkmem(EEP_t* pEEP)
{
  return M2SM_NOR_CheckMem(pEEP);
}

extern uint8_t (*M2SM_NOR_Test[])(SerialMemGlobalInfo_t *Smgi);
extern SerialMemGlobalInfo_t *tSerialMemGlobalInfo[];
uint8_t ospi_test(uint8_t MemoryIndex) {

  SerialMemGlobalInfo_t *Smgi;
  Smgi=tSerialMemGlobalInfo[MemoryIndex];
  return M2SM_NOR_Test[MemoryIndex](Smgi);
  
}



uint8_t Board_IsPlugged(void) {
  if(I2C_MasterIO_IsSlavePresentByAddress(&gI2C_Bus_M2SerialMem, 0xB2)) return 1;
  else return 0;
}
    
extern uint8_t JedecInfo[];
uint8_t Board_Run(void) {

  EEP_INFO_t EepInfo;
  uint8_t MemoryIndex;
  uint8_t OspiTest=0;
  
    M2SM_EEP_PNP_init(&M2SM_M24256D);
  // Here the quad memory is onboard and we can test the M95P32 here...
  do {
    EepInfo=M2SM_EEP_PNP_polling(&M2SM_M24256D);
  } while(0);
  
  if(EepInfo.Empty && EepInfo.Detected)
  {
    /* obtain the m2sm index and fill the eeprom */
    EepInfo.M2SM_Index=ospi_checkmem(&M2SM_M24256D);// OTP ID
    //MemoryIndex=ospi_checkmem(&M2SM_M24256D);// Standard ID

    if(EepInfo.M2SM_Index < EepInfo.M2SM_Max)
    {
      M2SM_EEP_PNP_filling(&M2SM_M24256D, EepInfo.M2SM_Index);
      EepInfo=M2SM_EEP_PNP_polling(&M2SM_M24256D);
      if(EepInfo.Empty) {
        M2_RED_LED_ON;
        printf_error(""RED"Huge memory plug problem, reset the system"RESET"\n\r");
        while(1); //error
      }
    }
    else
    {
      printf_error("Check memory ID "RED"FAILED"RESET" \n\r");
    }
  }
  else if (!EepInfo.Empty && EepInfo.Detected)
  {
    /* Double check the m2sm index */
    MemoryIndex=ospi_checkmem(&M2SM_M24256D);
    if(MemoryIndex == EepInfo.M2SM_Max)
    {
      printf_error("M.2 memory JEDEC Id status: "RED"JEDEC not supported"RESET"\n\r");
      printf_error("M.2 memory JEDEC Id detected: {0x%02x, 0x%02x, 0x%02x}", JedecInfo[0], JedecInfo[1], JedecInfo[2]);
      if((JedecInfo[0]==0x00)&&(JedecInfo[1]==0x00)&&(JedecInfo[2]==0x00)) printf_error("M.2 memory JEDEC ID: "YELLOW"the M.2 memory board may be wrongly inserted"RESET"\n\r");
      M2_RED_LED_ON;
      return 2;
    }
    else if(EepInfo.M2SM_Index!=MemoryIndex)
    {
      M2SM_EEP_PNP_filling(&M2SM_M24256D, MemoryIndex);
      EepInfo.M2SM_Index=MemoryIndex;
    }
  }

  if(!EepInfo.Empty && EepInfo.Detected)
  {
    OspiTest=ospi_test(EepInfo.M2SM_Index); // This does test the memory
    printf_info("Flash memory test is %s"RESET"\n\r",OspiTest?""GREEN"PASSED":""RED"FAILED");
    OspiTest?M2_GREEN_LED_ON:M2_RED_LED_ON;
    
    /* EEPROM result saving */
    M2SM_EEP_PNP_saving(&M2SM_M24256D, OspiTest);
    
    
  }
  
  if((EepInfo.Detected)&&(EepInfo.Detected_Id)) return 1;
  else return 0;
}





#endif //#ifdef MB2095B_MCU
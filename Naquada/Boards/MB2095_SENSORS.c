/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

// This describes uncut Red Naquada board elements with sensors

#include "main.h"

#ifdef MB2095_SENSORS

// There is no name on the joystick side
const IO_Pad_t JOYSTICK = {  PD_11, { .Mode = IO_ANALOG, .Signal = ADC2_IN11, /*.Drive = IO_PUSHPULL,*/ .Pull = IO_NOPULL, /*.Speed = IO_SPEED_03,*/ /*.Odr = IO_ODR_HIGH*/ } };   

const IO_Pad_t LED1_AsOutput = {  LD1_ /*PB_15*/, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH } }; // LED1
const IO_Pad_t LED1_AsPWM =    {  LD1_ /*PB_15*/, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH, .Signal = TIM15_CH2 } }; 

const IO_Pad_t LED2_AsOutput = {  LD2_ /*PB_14*/, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH } }; // LED2
const IO_Pad_t LED2_AsPWM =    {  LD2_ /*PB_14*/, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH, .Signal = TIM15_CH1 } }; 

const IO_Pad_t LED3_AsOutput = {  LD3_ /*PA_8*/, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH } }; // LED3
const IO_Pad_t LED3_AsPWM =    {  LD3_ /*PA_8*/, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_01, .Odr = IO_ODR_HIGH, .Signal = LPTIM2_CH1 } }; 




TimedLED_t TimedLEDs[] = {
  { 0, 0, 0, 0,  0, 0,},
//    { (IO_Pad_t*)&LED1_AsOutput, 0, 0, 0,  0, 0,   },
  { (IO_Pad_t*)&LED1_AsPWM, 0, 0, 0,  0, 0,   },    
//    { (IO_Pad_t*)&LED2_AsOutput, 0, 0, 0,  0, 0,   },
    { (IO_Pad_t*)&LED2_AsPWM, 0, 0, 0,  0, 0,   },    
//    { (IO_Pad_t*)&LED3_AsOutput, 0, 0, 0,  0, 0,   },
    { (IO_Pad_t*)&LED3_AsPWM, 0, 0, 0,  0, 0,   },    
};

uint8_t LED_COUNT = countof(TimedLEDs);

I2C_MasterIO_t gI2C_Bus_Sensor; // Same I2C bus for both DIP28 and STMod+ PA11=SCL2, PA12=SDA2
const IO_Pad_t MIO_SDA_Sensor = { U8_SDA, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };
const IO_Pad_t MIO_SCL_Sensor = { U8_SCL, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };

I2C_MasterIO_t gI2C_Bus_DigitalHeader; // Same I2C bus for both DIP28 and STMod+ PA11=SCL2, PA12=SDA2
const IO_Pad_t MIO_SDA_DigitalHeader = { PB_9, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };
const IO_Pad_t MIO_SCL_DigitalHEader = { PB_8, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };

  // I2C for M2
I2C_MasterIO_t gI2C_Bus_M2SerialMem; 
const IO_Pad_t MIO_SDA_M2SerialMem = { PA_6, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };
const IO_Pad_t MIO_SCL_M2SerialMem = { PB_2, {.Mode = IO_OUTPUT, .Pull = IO_PULLUP, .Drive = IO_OPENDRAIN, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH} };  


const IO_Pad_t UserKeyPin_ANALOG =      { PD_11, { .Mode = IO_ANALOG, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 } }; // as digital push-in button, can also be used as analog keyboard...
const IO_Pad_t UserKeyPin_EXTI =        { PD_11, { .Mode = IO_INPUT, .Pull = IO_NOPULL, .Speed = IO_SPEED_01 } }; // as digital push-in button, can also be used as analog keyboard...


  // Red Naquada
  const IO_Pad_t QSPI_IO1 = {  U11_Q   /*PB0*/,  { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO1, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO2 = {  U11_Wn  /*PA7*/,  { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO2, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO3 = {  U11_Hn  /*PC6*/,  { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO3, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_NCS = {  U11_Sn  /*PE11*/, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_NCS, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO0 = {  U11_D   /*PE12*/, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO0, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_CLK = {  U11_CLK /*PE10*/, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_CLK, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   

  // going to ST M2 connector (because M95P32 is already on the sensor island, the extra 4 data bit for octo can't be used for this.
  // If you keep the sensor island attached to naquada AND plug it into the carrier board, to use the M2/SerialMem, you need to desolder the M95P32 and enable the code lines below
  const IO_Pad_t QSPI_IO4 =  { PD_4, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO4, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO5 =  { PC_2, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO5, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO6 =  { PC_3, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO6, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_IO7 =  { PD_7, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_IO7, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_DQS =  { PE_3, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_DQS, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  const IO_Pad_t QSPI_NCLK = { PB_5, { .Mode = IO_SIGNAL, .Signal = OCTOSPI1_NCLK, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   
  
// Power contorl of memories
  const IO_Pad_t QSPI_LDOEN = { PE_13, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_HIGH } };   // SPI_SCK shared as LDO2_EN

  const IO_Pad_t QSPI_LED1 = { PB_8, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_LOW } };   // LED is on when output low
  const IO_Pad_t QSPI_LED2 = { PE_8, { .Mode = IO_OUTPUT, .Drive = IO_PUSHPULL, .Pull = IO_NOPULL, .Speed = IO_SPEED_03, .Odr = IO_ODR_LOW } };   // LED is on when output low

IO_Pad_t BT_TX_Pad = { CN5_4 /*J1_22*/ /*PA_9*/,  { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = USART1_TX } };
IO_Pad_t BT_RX_Pad = { CN5_3 /*J1_20*/ /*PA_10*/, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = USART1_RX } };


IO_Pad_t ST_TX_Pad = { PD_8, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = USART3_TX } };
IO_Pad_t ST_RX_Pad = { PD_9, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = USART3_RX } };

IO_Pad_t MM_UART_TX1_Pad = { J1_17 /*PB_10*/, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = LPUART1_TX } }; // USART1_TX alternate option 
IO_Pad_t MM_UART_RX1_Pad = { J1_19 /*PB_11*/, { .Mode = IO_SIGNAL, .Drive = IO_PUSHPULL, .Pull = IO_PULLUP, .Speed = IO_SPEED_03, .Signal = LPUART1_RX } }; // USART1_RX alternate option


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
  IO_PadInit((IO_Pad_t*)&QSPI_LDOEN);
  
  IO_PadInit((IO_Pad_t*)&QSPI_LED1);
  IO_PadInit((IO_Pad_t*)&QSPI_LED2);

  IO_PadInit((IO_Pad_t*)&QSPI_IO4);
  IO_PadInit((IO_Pad_t*)&QSPI_IO5);
  IO_PadInit((IO_Pad_t*)&QSPI_IO6);
  IO_PadInit((IO_Pad_t*)&QSPI_IO7);
  IO_PadInit((IO_Pad_t*)&QSPI_DQS);
  //IO_PadInit((IO_Pad_t*)&QSPI_NCLK);

  

  IO_PadInit((IO_Pad_t*)&JOYSTICK);

  
  return 0;
}



int32_t Board_Signal_Init(void) {

  int32_t Minbps = 30000;
  int32_t Maxbps = 1000000;
  I2C_MasterIO_Init(&gI2C_Bus_M2SerialMem);
  I2C_MasterIO_ConfigHW(&gI2C_Bus_M2SerialMem, (IO_Pad_t*) &MIO_SDA_M2SerialMem, (IO_Pad_t*) &MIO_SCL_M2SerialMem);
  I2C_MasterIO_ConfigTimings(&gI2C_Bus_M2SerialMem, Minbps, Maxbps); 
  
//  ErrorRecovery(&gI2C_Bus_M2SerialMem);
  
  uint16_t StartAdr;
  // we sweep the slaves
  StartAdr = 0;
  while(StartAdr = I2C_MasterIO_FindNextSlaveByAddress(&gI2C_Bus_M2SerialMem, StartAdr)) {

      gSlaveSweep = StartAdr; // breakpoint here to stop as soon as one slave is detected
      NOPs(1); // put breakpoint here
  }; // when StartAdr == 0, it means the sweep is over
  
  NOPs(1); // breakpoint hook to view gSlaveSweep
  
  return 0;
}




#endif //#ifdef MB2095_SENSORS
/**
  ******************************************************************************
  * @file    SerialComPort.c (Serial interfaces drivers)
  * @author  S.Ma
  * @brief   Implement MCU side BTEL protocol and Arduino/STMod+ Serial Interfaces (USART) and SW FIFO (BSP)
  * 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018-2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "main.h"

void SerialTest(void);
// protocol each 32 bytes every 5 msec = 64b/10ms = 64kbps

// Here we favor the on-board direct connections to application function
// The other options and indirect connections are using #define redefinition (not directly shown on debug watch window)

// Please note that for Micromod serial there are 2 options, one using USART3 xor shared with STLink debug port, or can use unshared LPUART 

//  BT_SERIAL, // 0

typedef struct {
  
  uint32_t USARTx;
  uint32_t USARTx_IRQn;
  // clock selector info
  
} SerialInfo_t;

SerialInfo_t SerialInfo[] = { // should be in a digital datasheet info
// key, value  
  { (uint32_t) USART1, (uint32_t) USART1_IRQn },
  { (uint32_t) USART3, (uint32_t) USART3_IRQn },
  
};

uint32_t GetIRQn(uint32_t PPP);
uint32_t GetIRQn(uint32_t PPP)
{
  for(int8_t i=0;i<countof(SerialInfo);i++)
  {
    if(SerialInfo[i].USARTx == PPP)
      return SerialInfo[1].USARTx_IRQn;
  }
  
  return 0;
}    

void USARTs_ClockEnable(void);
void USARTs_ClockEnable(void) { //TODO
  __HAL_RCC_USART1_CLK_ENABLE(); 
  __HAL_RCC_USART3_CLK_ENABLE();
  __HAL_RCC_LPUART1_CLK_ENABLE();
  __HAL_RCC_USART1_CONFIG(RCC_USART1CLKSOURCE_HSI); // HSI=HSI16 16MHz
  __HAL_RCC_USART3_CONFIG(RCC_USART3CLKSOURCE_HSI); // 16MHz
  __HAL_RCC_LPUART1_CONFIG(RCC_LPUART1CLKSOURCE_HSI); // 16MHz
}

typedef struct {
  IO_Pad_t* pTX;
  IO_Pad_t* pRX;
  uint32_t CMD;
  u8fifo_t* u8fifo_TX;
  u8fifo_t* u8Fifo_RX;
} SerialPort_t; // This should simplify the serial port implementation as structure


// ==== BSP 

// to build a fifo, we need a buffer for outgoing data to TX pin, and another buffer for incoming data from RX pin
uint8_t Bytes_to_TX[MAX_SERIAL][2048], Bytes_from_RX[MAX_SERIAL][2048]; // big buffer for simpler SW
u8fifo_t u8fifo_to_TX[MAX_SERIAL], u8fifo_from_RX[MAX_SERIAL]; // These are the fifo contexts



#ifdef BT_SERIAL
SerialPort_t HC06_Serial = {
  .pTX = &BT_TX_Pad,
  .pRX = &BT_RX_Pad,
  .CMD = ((115200L)<<12) | 1, // 115200bps, RX TX enabled
  .u8Fifo_TX = &u8fifo_to_TX[BT_SERIAL],
  .u8Fifo_RX = &u8fifo_from_RX[BT_SERIAL],
};
#endif

#ifdef ST_SERIAL
SerialPort_t STLink_Serial = {
  .pTX = &ST_TX_Pad,
  .pRX = &ST_RX_Pad,
  .CMD = ((115200L)<<12) | 1, // 115200bps, RX TX enabled
  .u8Fifo_TX = &u8fifo_to_TX[ST_SERIAL],
  .u8Fifo_RX = &u8fifo_from_RX[ST_SERIAL],
};
#endif

#ifdef MM_SERIAL
SerialPort_t Micromod_Serial = {
  .pTX = &MM_UART_TX1_Pad,
  .pRX = &MM_UART_RX1_Pad,
  .CMD = ((115200L)<<12) | 1, // 115200bps, RX TX enabled
  .Fifo_TX = &u8fifo_to_TX[MM_SERIAL],
  .Fifo_RX = &u8fifo_from_RX[MM_SERIAL],
};
#endif


//=========================================================================================

uint32_t Configure_UART(uint32_t CMD, IO_Pad_t* pTX, IO_Pad_t* pRX);

// Called when the fifo is no longer empty to usually enable peripheral interrupt to digest incoming.
uint32_t u8fifo_to_USARTn_NoLongerEmpty(uint32_t ct) { // the from_SPIP fifo is not longer empty: activate USART RX to digest them
  LL_USART_EnableIT_TXE((USART_TypeDef*) ct); // something to send will fill the BV if enabled
  return 0;
}

// Called to safely stop interrupts of destination peripheral, as the FIFO is now empty
uint32_t u8fifo_to_USARTn_Emptied(uint32_t ct) { // there is no data to feed usart anymore, so disable the interrupt
  LL_USART_DisableIT_TXE((USART_TypeDef*) ct);  // this will be enabled when the FIFO is not empty  
  return 0;
}

//const char sHelloFromUSB[] = " ";//Hello from USB port 0123456789ABCDEF!\n";
//const char sHelloFromARD_STM[] = " ";//Hello from Arduino andSTMod+ add-on board port 0123456789ABCDEF!\n";
const char sHelloFromBT[] = " This is a test";//Hello from HC-06 serial debug port 0123456789ABCDEF!\n";
//const char sHelloToBT[] = " ";//Hello to HC-06 0123456789ABCDEF!\n";
const char HC06_Command[] = "AT+BAUD8\n"; // Can be used to reprogram the default bluetooth dongle baud rate from 9600 to 115200 bps... (manual operation in debug mode)
volatile int32_t DetourHC06_115kProgramming = 0;


void Serials_Init(void) {
// all the USART FIFO are being initialised here  
  //LL_USART_InitTypeDef USART_InitStruct;
//  ErrorStatus Error;

  uint32_t result,i;
  for(i=0;i<MAX_SERIAL;i++) {
    result = Newu8fifo(&u8fifo_to_TX[i], (uint32_t) &Bytes_to_TX[i][0], sizeof(Bytes_to_TX[0]));
    result = Newu8fifo(&u8fifo_from_RX[i], (uint32_t) &Bytes_from_RX[i][0], sizeof(Bytes_from_RX[0]));
  };

  // configure the callbacks
  Hooku8fifo_NoLongerEmpty(&u8fifo_to_TX[ST_SERIAL], u8fifo_to_USARTn_NoLongerEmpty, (uint32_t) USART3 );
  Hooku8fifo_NoLongerEmpty(&u8fifo_to_TX[BT_SERIAL], u8fifo_to_USARTn_NoLongerEmpty, (uint32_t) USART1 );
  
  Hooku8fifo_Emptied(&u8fifo_to_TX[ST_SERIAL], u8fifo_to_USARTn_Emptied, (uint32_t) USART3 );
  Hooku8fifo_Emptied(&u8fifo_to_TX[BT_SERIAL], u8fifo_to_USARTn_Emptied, (uint32_t) USART1 );

  
  // HERE THE USART SHALL BE CONFIGURED OR INITIALIZED WITH DEFAULT SETTINGS
    // all supported USART
  USARTs_ClockEnable();  
  
  uint32_t CMD;
  //CMD[0] = ((9600L)<<12) | 1; // default SPIP going to SWD port (you have to setup the serial interface on PC side for 115200bps like BT dongle
  CMD = ((115200L)<<12) | 1; // STLink Virtual Com Port
  Configure_UART(CMD, &BT_TX_Pad, &BT_RX_Pad);
  CMD = ((115200L)<<12) | 1; // Bluetooth dongle serial port (SPP) with HC-06 or similar
  Configure_UART(CMD, &ST_TX_Pad, &ST_RX_Pad);
  
  
//  for(i=0;i<200;i++) { AddTou8fifo(&u8fifo_to_SPIP[ARD_STM_SERIAL], sHelloFromARD_STM[i]); if(sHelloFromARD_STM[i]==0) break; }
  for(i=0;i<200;i++) { AddTou8fifo(&u8fifo_to_TX[BT_SERIAL], sHelloFromBT[i]); if(sHelloFromBT[i]==0) break; }

//  for(i=0;i<200;i++) { AddTou8fifo(&u8fifo_from_SPIP[ARD_STM_SERIAL], sHelloToARD_STM[i]); if(sHelloToARD_STM[i]==0) break; }
  //for(i=0;i<200;i++) { AddTou8fifo(&u8fifo_from_SPIP[BT_STL_SERIAL], sHelloToBT_STL[i]); if(sHelloToAPP[i]==0) break; }
  //SerialTest();
}

// Decryptor of the 32 bit field
//  CMD[0] = 
//  [7:0] = USARTn:
//  0 => USART3 STL OFF (no GPIO connection)
//  1 => USART3 STL enabled
//  2 => USART3 STL enabled with RX TX swapped
//  3 => USART1 BT OFF
//  4 => USART1 BT enabled
//  5 => USART1 BT enabled with RX TX swapped
//  6 => 
//  7 => 
//  8 => 
//  9 => 
// 10 => 
//  [9:8] = USART parity: 0.None 1.Even 2.Odd (for now, only none)
//  [11:10] = USART1 stop bits: 0.1 1.1,5 2.2 (for now, only one stop bit)
//  [31:12] USART1 Baudrate



uint32_t Configure_UART(uint32_t CMD, IO_Pad_t* pTX, IO_Pad_t* pRX) {

    uint32_t bps = (CMD>>12); // 20 bits
    uint8_t swap_rx_tx = 0;
    uint32_t SIF_ioconfig_now = CMD & 0xF;
      
      // 0..2 : USART3 ST Link OFF ON Swap
      switch(SIF_ioconfig_now) { // for now, there will be glitches when changing GPIOs, and we don't care the FIFO are flushed, or USART is idle
      case 0: // BT USART3 disconnected from GPIOs
        IO_PadDeInit(pTX->PadName);
        IO_PadDeInit(pRX->PadName);
        break;
      case 2: // BT&STL USART1 swapped RX/TX
    	  swap_rx_tx = 1; // continue the rest is same
      case 1: // BT&STL USART1 normal
        IO_PadInit(pTX);
        IO_PadInit(pRX);
        break;
      default: 
        while(1);
      };
    
    // now we select the right usart (this is still crappy, we should use the pad init stucture to find the right PPP!!!! //TODO
    
    DS_PAD_t const*  pDS_PAD = pDS_PAD_MatchByPinAndSignal(pTX->PadName, pTX->Config.Signal);
    USART_TypeDef *USARTx = (USART_TypeDef *)pDS_PAD->PPP_Base;
    LL_USART_InitTypeDef USART_InitStruct;
    ErrorStatus Error;
    
    if(    (USARTx == USART1) 
        || (USARTx == USART3) )
    { // configure USART1/3
      LL_USART_DeInit(USARTx);
      LL_USART_StructInit(&USART_InitStruct);
      USART_InitStruct.BaudRate = bps;
      Error = LL_USART_Init(USARTx, &USART_InitStruct);
      LL_USART_SetTXRXSwap(USARTx, swap_rx_tx ? LL_USART_TXRX_SWAPPED : LL_USART_TXRX_STANDARD); // 
      LL_USART_Enable(USARTx);
      if(USARTx == USART1)
      {
        NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x0F, 0x00));
        NVIC_EnableIRQ(USART1_IRQn);
        LL_USART_EnableIT_RXNE(USARTx);
        if(u8fifo_to_TX[BT_SERIAL].bCount)// Dynamic case: we activate the TX if the u8fifo_from_SPIP is already not empty (or we flush it)
          LL_USART_EnableIT_TXE(USARTx);
      }
      if(USARTx == USART3)
      {
        NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x0F, 0x00));
        NVIC_EnableIRQ(USART3_IRQn);
        LL_USART_EnableIT_RXNE(USARTx);
        if(u8fifo_to_TX[ST_SERIAL].bCount)// Dynamic case: we activate the TX if the u8fifo_from_SPIP is already not empty (or we flush it)
          LL_USART_EnableIT_TXE(USARTx);
      }
    }
    else
      while(1);

  return 1;
}

void SerialTest(void)
{
  // we keep sending 2 chars to the USART
  do
  {
    AddTou8fifo(&u8fifo_to_TX[BT_SERIAL], 0x5A);
    HAL_Delay(5);
  } while(1);
  
}



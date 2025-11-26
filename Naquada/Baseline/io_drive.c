/**
  ******************************************************************************
  * @file    io_driver.c (an alternate way to manage MCU GPIOs)
  * @author  S.Ma
  * @brief   Can coexist with HAL/LL. How this driver deviate from HAL and LL?
  * 
  *  Alternate way to manipulate STM32 GPIO pins (package level) also known as Pads (silicon pad level) 
  *
  *  A pin can be defined as a (port, pinmask) couple of parameters which is best for speed. eg: (GPIOA, GPIO_PIN_7) will describe Port A, pin 7
  *  IO Drive uses a single pin name identifier automatically generated. eg: (PA_7)
  *  Pin name are simply built like this : PA0 = 0, PA1 = 1, PA15 = 15, PB0 = 16, PB1 = 17... etc...
  *  While a bit less optimized and working one pin at a time, it is easier to maintain, more intuitive, more portable across packages and STM32 families.
  *  
  *  When speed is paramount, a way is provided to convert the pin name into a "raw" port+mask in a RAM variable so LL_GPIO functions can be used.
  *  This is typically used for example when performing emulated I2C or SPI through pure GPIOs.
  *
  *  Pin configuration is consise and very compact. 
  *  
  *  The datasheet pinout and alternate function table has been C coded through python scripts
  *  You don't need to remember the alternate function index value, just provide the pin and the signal name (eg. TIM1_CH1) easing portability.
  *  You will less need to read and keep open the STM32 datasheet with this driver.
  * 
  *  There is a netlist BSP file which can enable you to name companion chip pin names and the corresponding MCU pin name will be done directly.
  *
  *  EXTI configuration helper function is provided here. (not the interrupt handler).
  *
  *  Code example and usage is provided at the end of this source file.
  * 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018-2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "main.h"

// These are the include files that you shoud integrate to your project main.h or equivalent.
#include "critical.h" // This is to briefly disable interrupt (single core) when doing a read-modify-write in regular HW register
#include "io_drive.h" // dependency, should be also added to your main.h when integrating this driver to your project.
#include "stm32u3xx_ll_gpio.h" // The driver has Raw functions when speed is required using the LL of the corresponding MCU Target. Adjust this to yours.
#include "STM32U3LQFP100_SMPS.h" // This is the script generated target MCU and package. There is a C and H file. Adjust for your target.

//====================================================================================
// What can we do with this code ? 
// This code is for illustration only
#if 0 
// Define PA_5 as digital input, with pull-up
const IO_Pad_t ARD_D3 = { PA_5, {.Mode = IO_INPUT, .Pull = IO_PULLUP };

// Define PB_4 as digital output, push-pull, no pull-up, set high initially, speed 02
const IO_Pad_t ARD_D4 = { PB_4, {.Mode = IO_OUTPUT, .Pull = IO_NOPULL, .Drive = IO_PUSHPULL, .Speed = IO_SPEED_02, .Odr = IO_ODR_HIGH };

// Define PC_0 as SPI3_MOSI pin (alternate function = 5) (push pull, speed03, no pull)
const IO_Pad_t ARD_D13 = { PC_0, {.Mode = IO_SIGNAL, .Pull = IO_NOPULL, .Drive = IO_PUSHPULL, .Speed = IO_SPEED_03, .Signal = IO_AF_05 };
// if you have the target MCU pinout file integrated, you can also do simply this:
const IO_Pad_t ARD_D13 = { PC_0, {.Mode = IO_SIGNAL, .Pull = IO_NOPULL, .Drive = IO_PUSHPULL, .Speed = IO_SPEED_03, .Signal = SPI3_MOSI };

// Set PA_4 as DAC output
const IO_Pad_t ARD_A0 = { PA_4, {.Mode = IO_ANALOG, .Pull = IO_NOPULL };

void IO_Pad_Test(void);
void IO_Pad_Test(void) {
  
  IO_PadInit(&ARD_D3);
  IO_PadInit(&ARD_D4);
  IO_PadInit(&ARD_D13);
  IO_PadInit(&ARD_A0);

  int32_t pin_level = IO_PadGet(PA_5);
  IO_PadSetHigh(PB_4);
  IO_PadSet(PB_4, 0);

  // If you have the board netlist file, you can use the other chip's end pin
  IO_PadSetHigh(U5_4); // set companion chip named U5 pind 4 (if your MCU package change or the board routing change, your code doesn't, the netlist will)
  while(1);
}
#endif

//ApplicationSignalName ==> Pin(s) ==> Port/bitmask

//------------ Translate pin name into (port and pinmask) when using LL_GPIO function when speed IO toggling is the priority

int32_t 
IO_AddPadToRawPortPads(PadName_t PadName, RawPortPads_t* pRPP ) { // converts a pin name into a (port, pin mask) representation for LL_GPIO functions

  uint8_t gpio_index = PadName>>4; // This get the port name from the pin name (0=GPIOA, 1=GPIOB, 2=GPIOC...)
  
//  if(gpio_index>=countof(IO_GPIOx)) // is it a valid gpio? (optional)
//    return -1;
  
  uint8_t pad_position = PadName & 0x000F; // This get the pin position within a port (0..15)
  pRPP->bfPin = 1<<pad_position;  // This is the bitfield pin mask, eg PA7=0x0080
  if((pGPIO_PadValidMask[gpio_index] & pRPP->bfPin)==0) // we check if this pin exists
    while(1); //return -1;
  
  // valid port, valid pin
  pRPP->pPort = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  return 0;
}

int32_t 
IO_ClearRawPortPads(RawPortPads_t* pRPP) {
  
  pRPP->bfPin = 0;
  pRPP->pPort = NULL;
  return 0;
}

//--------------- IO pads user functions which are similar to HAL, more intuitive yet still quite compact.

//static uint32_t tmp, bfmask, bfval; // for debug watch purpose
int32_t 
IO_PadInit(IO_Pad_t* pIO ) { // this is for a single pin

  uint32_t tmp, bfmask, bfval; // for debug watch purpose
  // transformed HAL
  //uint32_t tmp, bfmask, bfval;
/* TODO use this function and adjust the code    
  RawPortPads_t RPP;
  if(IO_MakeRawPortPadsFromPin(&RPP, pIO->Name)<0)
    return -1; // wrong padname
  */
  //--- original code for checking here --- to use RRP instead
  uint8_t gpio_index = pIO->PadName>>4;
  // first is it a valid gpio?
//  if(gpio_index>=countof(IO_GPIOx))
//    return -1;
  
  uint8_t pad_position = pIO->PadName & 0x000F;
  uint32_t pin_mask = 1<<pad_position;  
  if((pGPIO_PadValidMask[gpio_index] & pin_mask)==0)
    return -1;
  
  // valid port, valid pin
  GPIO_TypeDef *GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];

  CRITICAL_SECT_DEFINE;//uint32_t primask  = __get_PRIMASK(); // save current interrupt context
  
  switch(gpio_index) { // we make sure the corresponding GPIO register is clock enabled so its HW registers can be written properly so you don't have to think about it.
    // each HAL macro is a piece of code, which is difficult to organize as a lookup table indexed by gpio_index unfortunately. Yet smart compiler will optimize the below code.
  case 0:       __HAL_RCC_GPIOA_CLK_ENABLE(); break;
  case 1:       __HAL_RCC_GPIOB_CLK_ENABLE(); break;
#ifdef GPIOC
  case 2:       __HAL_RCC_GPIOC_CLK_ENABLE(); break;
#endif
#ifdef GPIOD  
  case 3:       __HAL_RCC_GPIOD_CLK_ENABLE(); break;
#endif
#ifdef GPIOE
  case 4:       __HAL_RCC_GPIOE_CLK_ENABLE(); break;
#endif
#ifdef GPIOF
  case 5:       __HAL_RCC_GPIOF_CLK_ENABLE(); break;
#endif
#ifdef GPIOG
  case 6:       __HAL_RCC_GPIOG_CLK_ENABLE(); break;  
#endif
#ifdef GPIOH
  case 7:       __HAL_RCC_GPIOH_CLK_ENABLE(); break;
#endif
#ifdef GPIOI
  case 8:        __HAL_RCC_GPIOI_CLK_ENABLE(); break;
#endif
#ifdef GPIOJ
  case 9:        __HAL_RCC_GPIOJ_CLK_ENABLE(); break;  
#endif
#ifdef GPIOK
  case 10:        __HAL_RCC_GPIOK_CLK_ENABLE(); break;  
#endif
#ifdef GPIOL
  case 11:        __HAL_RCC_GPIOL_CLK_ENABLE(); break;  
#endif
#ifdef GPIOM
  case 12:        __HAL_RCC_GPIOM_CLK_ENABLE(); break;  
#endif
#ifdef GPION
  case 13:        __HAL_RCC_GPION_CLK_ENABLE(); break;  
#endif
#ifdef GPIOO
  case 14:        __HAL_RCC_GPIOO_CLK_ENABLE(); break;  
#endif
#ifdef GPIOP
  case 15:        __HAL_RCC_GPIOP_CLK_ENABLE(); break;  
#endif
#ifdef GPIOQ
  case 16:        __HAL_RCC_GPIOQ_CLK_ENABLE(); break;  
#endif
#ifdef GPIOR
  case 17:        __HAL_RCC_GPIOR_CLK_ENABLE(); break;  
#endif
#ifdef GPIOS
  case 18:        __HAL_RCC_GPIOS_CLK_ENABLE(); break;  
#endif
#ifdef GPIOT
  case 19:        __HAL_RCC_GPIOT_CLK_ENABLE(); break;  
#endif
#ifdef GPIOU
  case 20:        __HAL_RCC_GPIOU_CLK_ENABLE(); break;  
#endif
#ifdef GPIOV
  case 21:        __HAL_RCC_GPIOV_CLK_ENABLE(); break;  
#endif
  default: while(1); // something wrong here
  };
  
  // Pin configuration and properties setting.
  
  // If Speed field is non default zero, set the GPIO Speed value accordingly
  if(pIO->Config.Speed) {
    
    bfmask = ~(0x3<<(pad_position * 2)); // speed field is 2 bit wide: 00 01 10 11 possible values
    bfval = (pIO->Config.Speed-1) << (pad_position * 2); 
    CRITICAL_SECT_LOCK();//__set_PRIMASK(1);// atomic start
    tmp = (GPIOx->OSPEEDR & bfmask) | bfval;
    GPIOx->OSPEEDR = tmp;
    CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);// atomic end
  }

  // If Pull field is non default zero, set the GPIO pull up/down value accordingly  
  if(pIO->Config.Pull) {
    
    bfmask = ~(0x3<<(pad_position * 2)); // 2 bit value: 00 01 10 are valid choices
    bfval = (pIO->Config.Pull-1) << (pad_position * 2);
    CRITICAL_SECT_LOCK();//__set_PRIMASK(1);// atomic start
    tmp = (GPIOx->PUPDR & bfmask) | bfval;
    GPIOx->PUPDR = tmp;
    CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);    // atomic end
  }
  
  // Configure the Output Type Open drain, push pull, this is a single bit
  if(pIO->Config.Drive) {

    bfmask = ~(1<<pad_position);
    bfval = (pIO->Config.Drive-1) << (pad_position);
    CRITICAL_SECT_LOCK();//__set_PRIMASK(1);// atomic start
    tmp = (GPIOx->OTYPER & bfmask)|bfval;
    GPIOx->OTYPER = tmp;
    CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);    // atomic end
  }

  // Provide the opportunity to set the output desired level BEFORE activating the pin mode (especially useful when the pin will become an output)
  if(pIO->Config.Odr) {

    bfmask = ~(1<<pad_position);
    bfval = (pIO->Config.Odr-1) << pad_position;
    CRITICAL_SECT_LOCK();//__set_PRIMASK(1);// atomic start
    tmp = (GPIOx->ODR & bfmask)|bfval;
    GPIOx->ODR = tmp;
    CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);    // atomic end
  }
  
  /*--------------------- GPIO Mode Configuration ------------------------*/

  // analog: nothing to do here
  /* IO_SIGNAL = Alternate function mode selection */
  if( pIO->Config.Mode == IO_SIGNAL )
  {
    int16_t Alternate = pIO->Config.Signal; // alternate reaching over 128
    
    if(Alternate > 0x0F) // if it's not a direct value [0..15] (legacy)
    {
      /* Configure Alternate function mapped with the current IO */
      // This will use a database using pin and signal to find out the AF value to program.
#if 1          
      DS_PAD_t* pDS_PAD = pDS_PAD_MatchByPinAndSignal(pIO->PadName, pIO->Config.Signal); // function freeze if no solution
      if( pDS_PAD == NULL ) while(1); // freeze. issue. The desired signal does not exist on the chosen pin
      Alternate = pDS_PAD->AF;
#else
        while(1); // the target package MCU pinout information is missing
        // you need to import and include the right MCU pin source and header file
#endif        
    }
    
    bfmask = ~(0xF << ((pad_position & 0x07U) * 4U));
    bfval = (Alternate & 0x0FUL) << ((pad_position & 0x07U) * 4U);
    CRITICAL_SECT_LOCK();//__set_PRIMASK(1);// atomic start
    tmp = (GPIOx->AFR[pad_position >> 3U] & bfmask)|bfval;
    GPIOx->AFR[pad_position >> 3U] = tmp;
    CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);// atomic end
  }
  
  // This is the pin mode, 00 = input, 01 = output, 10 = signal alternate function, 11 = analog (2 bit field)
  if(pIO->Config.Mode) {

    bfmask = ~(0x3<<(pad_position * 2));
    bfval = (pIO->Config.Mode-1) << (pad_position * 2);
    CRITICAL_SECT_LOCK();//__set_PRIMASK(1); // atomic start
    tmp = (GPIOx->MODER & bfmask )|bfval;
    GPIOx->MODER = tmp;
    CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);//atomic end
    // atomic end
  }
 
  return 0;
}

int32_t 
IO_PadDeInit(PadName_t  PadName) {
  
  IO_Pad_t DeInitPad = { PadName, {.Mode=IO_ANALOG, .Pull=IO_NOPULL, .Drive=IO_PUSHPULL, .Speed=IO_SPEED_00, .Signal = IO_AF_00 } }; // stack based local
  return IO_PadInit( &DeInitPad ); // deinit register sequence could be inverted. To check should be mode dependent?
  
  /*------------------------- EXTI Mode Configuration --------------------*/
  /*------------------------- GPIO Mode Configuration --------------------*/
  /* Configure IO in Analog Mode */
  /* Configure the default Alternate Function in current IO */
  /* Configure the default value for IO Speed */
  /* Configure the default value IO Output Type */
  /* Deactivate the Pull-up and Pull-down resistor for the current IO */
}

int32_t 
IO_PadGet(PadName_t  PadName) {
  
  return (IO_GPIOx[(PadName>>4)&0xF]->IDR)>>(PadName & 0x0F)&1; //This will read the pin level and return 0 or 1
}

// Call this function to set a pin high
int32_t 
IO_PadSetHigh(PadName_t PadName) { // Performance wise : 12 op codes in low optimisations, 30 bytes

  //GPIOx->BSRR = (uint32_t)GPIO_Pin;
  
  uint8_t gpio_index = PadName>>4;
  uint8_t pad_position = PadName & 0x000F;
  uint32_t pin_mask = 1<<pad_position;  
  
  GPIO_TypeDef* GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  GPIOx->BSRR = pin_mask; // atomic
  return 0;
}

// Call this function to set a pin low
int32_t 
IO_PadSetLow(PadName_t PadName) {
  
  //GPIOx->BRR = (uint32_t)GPIO_Pin;
  uint8_t gpio_index = PadName>>4;
  uint8_t pad_position = PadName & 0x000F;
  
  GPIO_TypeDef* GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  uint32_t pin_mask = 1<<pad_position;
  GPIOx->BRR = pin_mask; // atomic
  return 0;
}

// Call this helper function to set the pin from a PinState value
int32_t 
IO_PadSet(PadName_t  PadName, GPIO_PinState PinState) {

  if (PinState != GPIO_PIN_RESET)
  {
    return IO_PadSetLow(PadName);
  }
  else
  {
    return IO_PadSetHigh(PadName);
  }
}

// Call this function to toggle a pin output level 0 ==> 1 or 1 ==> 0
int32_t 
IO_PadToggle(PadName_t  PadName) {
  
  //uint32_t bfmask, bfvalue;
  uint8_t gpio_index = PadName>>4;
  uint8_t pad_position = PadName & 0x000F;
  uint32_t pin_mask = 1<<pad_position;  
  
  GPIO_TypeDef* GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  if( GPIOx->ODR & pin_mask ) pin_mask <<= 16; // output was forced high, force it low
    
  GPIOx->BSRR = pin_mask; // atomic
  return 0;
}

#if 0 // not implemented and validated yet
HAL_StatusTypeDef IO_PadLock(PadName_t  PadName) {
  
  __IO uint32_t tmp = GPIO_LCKR_LCKK;

  /* Check the parameters */
  assert_param(IS_GPIO_LOCK_INSTANCE(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  /* Apply lock key write sequence */
  tmp |= GPIO_Pin;
  /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  GPIOx->LCKR = tmp;
  /* Reset LCKx bit(s): LCKK='0' + LCK[15-0] */
  GPIOx->LCKR = GPIO_Pin;
  /* Set LCKx bit(s): LCKK='1' + LCK[15-0] */
  GPIOx->LCKR = tmp;
  /* Read LCKK bit*/
  tmp = GPIOx->LCKR;

  /* read again in order to confirm lock is active */
  if ((GPIOx->LCKR & GPIO_LCKR_LCKK) != GPIO_LCKR_LCKK)
  {
    return HAL_ERROR;
  }
  return HAL_OK;
}
#endif

int32_t 
IO_CreateRawPortPadsFromPads(RawPortPads_t* pRPP, PadName_t* pPadNames) {
  
  RawPortPads_t RPP = { NULL, 0 };
  uint8_t count=0;
   
  do{
    
    if(IO_AddPadToRawPortPads(*pPadNames, &RPP)<0)
      return -1; // invalid pins
    
    // valid pad here
    if(RPP.pPort == NULL) 
    { // first valid pad to set the variable
      pRPP->pPort = RPP.pPort;
      pRPP->bfPin = RPP.bfPin;
    }
    else
    {
      if(pRPP->pPort != RPP.pPort)
        return -1; // error, not all pins are from the same port, stop, skip?
      pRPP->bfPin |= RPP.bfPin; // add the pin to the port bitmask
    }
    
    pPadNames++; // next element
  }  
  while(count++<16); // max 16 PadNames.

  return 0;
}


void 
IO_RawPadSetHigh(RawPortPads_t* RawPad) {
  
  if(RawPad==NULL)  while(1) {};
  LL_GPIO_SetOutputPin(RawPad->pPort, RawPad->bfPin);
}

void 
IO_RawPadSetLow(RawPortPads_t* RawPad) {
  
  if(RawPad==NULL)  while(1) {};
  LL_GPIO_ResetOutputPin(RawPad->pPort, RawPad->bfPin);
}

uint32_t 
IO_RawPadGet(RawPortPads_t* RawPad) {
  
  if(RawPad==NULL)  while(1) {};
  return LL_GPIO_IsInputPinSet(RawPad->pPort, RawPad->bfPin); // return the pin level
}





//=======================================================================
// Datasheet C encoded database

// const PinAlternateDescription_t PAD[]
// search functions from this const global structure (depends on dice and package)

/* The database element structure details:
typedef struct {
  u8 PinName;
  u32 PPP_Base;
  SignalName_t SignalName;
  u8 AF;// ADC channel
  char* SignalNameString;
} PinAlternateDescription_t;

example:

(..)
{ PB_0, (uint32_t) OCTOSPI1, OCTOSPI1_IO1, 10 }, 
{ PB_0, (uint32_t) OPAMP2, OPAMP2_VOUT, -1 }, 
{ PB_0, (uint32_t) TIM1, TIM1_CH2N, 1 }, 
{ PB_0, (uint32_t) COMP1, COMP1_OUT, 12 }, 
{ PB_0, (uint32_t) 0, AUDIOCLK, 13 }, 
{ PB_0, (uint32_t) ADC2, ADC2_IN9, ADC_CHANNEL_9}, 
{ PB_0, (uint32_t) SPI1, SPI1_NSS, 5 }, 
{ PB_0, (uint32_t) TIM3, TIM3_CH3, 2 }, 
{ PB_0, (uint32_t) 0, EVENTOUT, 15 }, 
{ PB_0, (uint32_t) USART3, USART3_CK, 7 }, 
{ PB_0, (uint32_t) LPTIM3, LPTIM3_CH1, 4 }, 
{ PB_0, (uint32_t) ADC1, ADC1_IN13, ADC_CHANNEL_13}, 
{ PB_1, (uint32_t) LPUART1, LPUART1_DE, 8 }, 
(...)


*/

// find the entry for the corresponding pinname and signal name (because one pin can have multiple signals from same peripheral...)
// we return a pointer to the entry so C code can directly -> its elements
// this is not itterative as there should be only one solution.
// usage: DS_PAD_t* pin_info = pDS_PAD_MatchByPinAndSignal(PA_2, TIM1_CH1);
DS_PAD_t* 
pDS_PAD_MatchByPinAndSignal(PadName_t PinName, STM32_SignalName_t SignalName) {
  
  uint32_t i;
  for(i=0;i<DS_PADsCount;i++) {
    
    if(DS_PADs[i].PinName==PinName)
      if(DS_PADs[i].SignalName==SignalName) {
        // we found the pin!
        return (DS_PAD_t*) &DS_PADs[i]; 
      };
  };
  
  while(1); // nothing found...
  //return NULL; // failed.
}


// Returns the index of entry matching the pin+signal pair from start index (could be multiple answers)
// usage: int32_t new_index = iDS_PAD_NextMatchBySignal(TIM1_CH1, 0); // start from the table beginning
int32_t 
iDS_PAD_NextMatchBySignal(STM32_SignalName_t SignalName, int32_t startindex) {
  
  uint32_t i;
  for(i=0;i<DS_PADsCount;i++) {
    
    if(DS_PADs[i].SignalName==SignalName) {
        // we found the pin!
        return i;
    };
  };
  
  return -1; // nothing found...
}

// IO_Drive related facility function
// uint32_t peripheral_base_address = PadSignal2PPP(pLED1_as_PWM);
// retrieve the peripheral from pin signal to the peripheral name (TIM1_CH1 ==> TIM1, SPI6_MOSI ==> SPI6)
uint32_t 
PadSignal2PPP(IO_Pad_t* pPad) { // explain more

  if(pPad->Config.Mode != IO_SIGNAL)
    //if(pPad->Config.Mode != IO_ANALOG)
      return 0; // not supported
  
  int32_t i = iDS_PAD_NextMatchBySignal(pPad->Config.Signal, 0);
  if(i<0)
    return 0;
  
  return DS_PADs[i].PPP_Base;
}

//=======================================================================

// EXTI : IO Interrupt configuration

// Here we are managing the interrupt from GPIO, which is related to EXTI
// EXTI is independent of the IO configuration, 16 input lines of EXTI can come from IO Port/pin.
// for legacy purpose, we put here the function which added to IO_INIT() will have equivalent functionality to HAL_GPIO_Init()
// usage: uint32_t result = IO_EXTI_Config( PA_7, EXTI_Config)
int32_t 
IO_EXTI_Config( PadName_t PadName, EXTI_Config_t EXTI_Config) {

  uint8_t gpio_index = PadName>>4;
//  if(gpio_index>=countof(IO_GPIOx)) // optional check : is it a valid gpio?
//    return -1;
  
  uint8_t pad_position = PadName & 0x000F;
  uint32_t pin_mask = 1<<pad_position;  
  if((pGPIO_PadValidMask[gpio_index] & pin_mask)==0)
    return -1;
  
  uint8_t port_offset = PadName >> 4;
  // valid port, valid pin
//  GPIO_TypeDef *GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  CRITICAL_SECT_DEFINE;//uint32_t primask  = __get_PRIMASK(); // save current interrupt context
  
  // EXTI_IO_MUX_SETUP
  volatile uint8_t* pu8 = (uint8_t *)&EXTI->EXTICR[0]; // we switch to byte so we have an array of 16 bytes for bit 0..15, simple with byte access!
  pu8[pad_position] = port_offset; // no need to mask interrupt as it's a byte write, no read modify write here
// old scheme  
//  tmp = EXTI->EXTICR[position >> 2U];
//  tmp &= ~((0x0FUL) << (8U * (position & 0x03U)));
//  tmp |= (GPIO_GET_INDEX(GPIOx) << (8U * (position & 0x03U)));
//  EXTI->EXTICR[position >> 2U] = tmp;

  /* Clear EXTI line configuration */
  uint32_t tmp, bfmask, bfval; // for debug watch purpose
  
  bfmask = ~pin_mask;
  
  CRITICAL_SECT_LOCK();//__set_PRIMASK(1);// atomic start
//+ here we clear all pending edge detection  
  EXTI->RTSR1 = bfmask;
  EXTI->FTSR1 = bfmask;
  CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);// atomic end
  // leave here interrupt opportunity to minimize jitter...
  
  CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);// atomic start
  
  bfval = (EXTI_Config.Trigger & EXTI_CHANNEL_TRIGGER_INTERRUPT) ? 1 : 0;
  CRITICAL_SECT_LOCK();//__set_PRIMASK(1);// atomic start
  tmp = EXTI->IMR1;
  tmp = (tmp & bfmask) | bfval;
  EXTI->IMR1 = tmp;
  CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);// atomic start
//  tmp = EXTI->IMR1;
//  tmp &= ~pin_mask;
//  if (EXTI_Config.Trigger & EXTI_CHANNEL_TRIGGER_INTERRUPT)
//    tmp |= pin_mask;
//  EXTI->IMR1 = tmp;
  
  bfval = (EXTI_Config.Trigger & EXTI_CHANNEL_TRIGGER_EVENT) ? 1 : 0;
  CRITICAL_SECT_LOCK();//__set_PRIMASK(1);// atomic start
  tmp = EXTI->EMR1;
  tmp = (tmp & bfmask) | bfval;
  EXTI->EMR1 = tmp;
  CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);// atomic start
//  tmp = EXTI->EMR1;
//  tmp &= ~pin_mask;
//  if (EXTI_Config.Trigger & EXTI_CHANNEL_TRIGGER_EVENT)
//    tmp |= pin_mask;
//  EXTI->EMR1 = tmp;

  /* Clear Rising Falling edge configuration */
  
  bfval = (EXTI_Config.Edge & EXTI_RISING_EDGE) ? 1 : 0;
  CRITICAL_SECT_LOCK();//__set_PRIMASK(1);// atomic start
  tmp = EXTI->RTSR1;
  tmp = (tmp & bfmask) | bfval;
  EXTI->RTSR1 = tmp;
  CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);// atomic start
//  tmp = EXTI->RTSR1;
//  tmp &= ~pin_mask;
//  if (EXTI_Config.Edge & EXTI_RISING_EDGE)
//    tmp |= pin_mask;
//  EXTI->RTSR1 = tmp;

  bfval = (EXTI_Config.Edge & EXTI_FALLING_EDGE) ? 1 : 0;
  CRITICAL_SECT_LOCK();//__set_PRIMASK(1);// atomic start
  tmp = EXTI->FTSR1;
  tmp = (tmp & bfmask) | bfval;
  EXTI->FTSR1 = tmp;
  CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);// atomic start
//  tmp = EXTI->FTSR1;
//  tmp &= ~pin_mask;
//  if (EXTI_Config.Edge & EXTI_FALLING_EDGE)
//    tmp |= pin_mask;
//  EXTI->FTSR1 = tmp;

  return 0;
}


int32_t 
IO_EXTI_DeInit(PadName_t PadName) {

  uint8_t gpio_index = PadName>>4;
  // first is it a valid gpio?
//  if(gpio_index>=countof(IO_GPIOx))
//    return -1;
  
  uint8_t pad_position = PadName & 0x000F;
  uint32_t pin_mask = 1<<pad_position;  
  if((pGPIO_PadValidMask[gpio_index] & pin_mask)==0)
    return -1;
  
//  uint8_t port_offset = PadName >> 4;
  // valid port, valid pin
//  GPIO_TypeDef *GPIOx = (GPIO_TypeDef*) IO_GPIOx[gpio_index];
  CRITICAL_SECT_DEFINE;//uint32_t primask  = __get_PRIMASK(); // save current interrupt context

  CRITICAL_SECT_LOCK();//__set_PRIMASK(1);// atomic start
  /* Clear EXTI line configuration */
  EXTI->IMR1 &= ~pin_mask;
  EXTI->EMR1 &= ~pin_mask;

  /* Clear Rising Falling edge configuration */
  EXTI->RTSR1 &= ~pin_mask;
  EXTI->FTSR1 &= ~pin_mask;

  // clear the latch in case it's already armed.
  EXTI->RTSR1 |= pin_mask;
  EXTI->FTSR1 |= pin_mask;
  CRITICAL_SECT_UNLOCK();//__set_PRIMASK(primask);// atomic start
  
  return 0;
}

// We do not include the interrupt routine here.


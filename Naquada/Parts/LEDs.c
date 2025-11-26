/**
  ******************************************************************************
  * @file    LED.c (LED[s] manager)
  * @author  S.Ma
  * @brief   LED on/off/dimming with PWM using percentage / Blinking pulses mode.
  *          The dimming mode requires TIMER compare HW support, can be disabled at this level.
  * 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018-2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "main.h"
//#include "brisk.h"

// implements digital LED with ON/OFF, blinking (period programmable, nb of pulses or infinite), and if TIMER resources is used, DIMMING in % with duty cycle. (like SLEEP LED for laptops)

void LEDsConfigureTimers(TIM_TypeDef* Timer);
void LEDsConfigureChannel(TIM_TypeDef* Timer, uint32_t TIM_CHANNEL);
void LEDsSetDutyCycle(uint8_t index, uint32_t duty);

void LEDsConfigureLPTimers(LPTIM_TypeDef* Timer);
void LEDsConfigureLPChannel(LPTIM_TypeDef* Timer, uint32_t TIM_CHANNEL);

const uint16_t PercentToDuty[] = { // Eye brightness is in log, while PWM duty is linear, here is the conversion table, assuming 100 percentage steps...
0, // 0
1, // 1
2, // 2
3, // 
4, // 4
5, // 
6, // 6
7, // 
8, // 8
9, // 
10, // 10
11, // 
12, // 
13, // 
14, // 
15, // 
16, // 
17, // 
18, // 
19, // 
20, // 20
21, // 
22, // 
23, // 
24, // 
25, // 
26, // 
27, // 
28, // 
29, // 
30, // 30
31, // 
32, // 
33, // 
34, // 
35, // 
36, // 
37, // 
38, // 
39, // 
40, // 40
41, // 
42, // 
43, // 
44, // 
45, // 
46, // 
47, // 
48, // 
49, // 
50, // 50
51, // 
52, // 
53, // 
54, // 
55, // 
56, // 
57, // 
58, // 
59, // 
60, // 60
61, // 
62, // 
63, // 
64, // 
65, // 
66, // 
67, // 
68, // 
69, // 
70, // 70
71, // 
72, // 
73, // 73
//======
75, // 74
83, // 75
92, // 
102, // 
112, // 
124, // 
137, // 80
152, // 
167, // 
185, // 
205, // 
226, // 
250, // 
276, // 
305, // 
337, // 
373, // 90
412, // 
455, // 
503, // 
556, // 
615, // 
679, // 
751, // 
830, // 
917, // 
1000, // 100
};

//========== LED Definitions as Static Digital Output or Timer PWM based

// Naquada schematics info:
// LD1_ PB_15, TIM15_CH2,
// LD2_ PB_14, TIM15_CH1, 
// LD3_ PA_8,  **LPTIM2_CH1**, (TIM1_CH1)

// we also need to manage the timers.
// we need 100 log levels, so 1000 steps
// if dimming period is 1kHz, we run the timer at 1 MHz
// TIMER 3,4,16 are tied to LEDs

//============================
//  LED_t* pLED;
//  uint16_t Blink_Period_x10ms; // 123 = 1.23 second
//  uint16_t Blink_OffTime_x10ms; // 097 = 0.97 seconds will be off
//  uint16_t Blink_Countdown_x10ms; // this is the countdown from period down to zero. led is on until it reached the offtime.
//  uint8_t Blink_Countdown;
////  uint8_t Blink_CountdownCompleted;
//  uint8_t Dim_perc;
  
// Timed LED
extern TimedLED_t TimedLEDs[];
             
static uint8_t BriskLEDsDisabled = 0;
  
//=================================

void BriskTimedLED_Disable(void) { BriskLEDsDisabled=1;}
void BriskTimedLED_Enable(void) { BriskLEDsDisabled=0;}

void TimedLEDsInit(void) { // Initialize all LEDs as output

  // we assume all GPIO clock enabled have been done already
  // first, all LEDs are put as digital output low (off)
  IO_Pad_t * pLED;
  uint8_t index;

  for(index=1;index<LED_COUNT;index++) {
    pLED = TimedLEDs[index].pLED; // get handle to the pointed LED element
    if(pLED==0) continue; // missing LED //2025
//    IO_PadSetHigh((IO_Pad_t*)pLED->pPad->PadName);
    IO_PadInit(pLED);
    // if it has PWM option, enable the Timer for it

    if(pLED->Config.Mode == IO_SIGNAL) { // if alternate, activate and configure the peripherals.
      TIM_TypeDef* TIM = (TIM_TypeDef*) PadSignal2PPP(pLED);
      if(TIM==0)
          while(1);
      // not nice, easy to read, compact code
      
      // This is really crap to match existing HAL architecture. Far from seamless plug and play...
      // hack for now, missing database corresponding table
      switch(pLED->Config.Signal) 
      {
        case TIM15_CH1: // will be initialized twice...
          __HAL_RCC_TIM15_CLK_ENABLE(); // crap to clean
          LEDsConfigureTimers(TIM15); // Timer period
          LEDsConfigureChannel(TIM15, TIM_CHANNEL_1); // channel duty cycle
          break;
        case TIM15_CH2: 
          __HAL_RCC_TIM15_CLK_ENABLE(); // crap to clean
          LEDsConfigureTimers(TIM15); // Timer period          
          LEDsConfigureChannel(TIM15, TIM_CHANNEL_2); // channel duty cycle
          break;
        case TIM2_CH3:
          __HAL_RCC_TIM2_CLK_ENABLE(); // crap to clean
          LEDsConfigureTimers(TIM2); // Timer period          
          LEDsConfigureChannel(TIM2, TIM_CHANNEL_3); // channel duty cycle
          break;
        case LPTIM2_CH1:
          __HAL_RCC_LPTIM2_CLK_ENABLE(); // crap to clean
          LEDsConfigureLPTimers(LPTIM2); // Timer period          
          LEDsConfigureLPChannel(LPTIM2, LPTIM_CHANNEL_1); // channel duty cycle
          break;
        case LPTIM3_CH1:
          __HAL_RCC_LPTIM3_CLK_ENABLE(); // crap to clean
          LEDsConfigureLPTimers(LPTIM3); // Timer period          
          LEDsConfigureLPChannel(LPTIM3, LPTIM_CHANNEL_1); // channel duty cycle
          break;
        // Carrier board LED_2
        case TIM3_CH4:
          __HAL_RCC_TIM3_CLK_ENABLE(); // crap to clean
          LEDsConfigureTimers(TIM3); // Timer period          
          LEDsConfigureChannel(TIM3, TIM_CHANNEL_4); // channel duty cycle
          break;
          
      default: while(1);
      };
      
    };

  };
             
  TestLEDs();
}

void BriskDimLED(uint8_t index, int8_t percentage) {// a negative value will turn the LED OFF (toggling the sign is used for blinking implementation)

  IO_Pad_t * pLED;
  
  pLED = TimedLEDs[index].pLED; // get handle to the pointed LED element
  
  if(pLED==0) // LED does not exist // 2025
    return;

  if(TimedLEDs[index].InvertedLED) // May 2025
    percentage = 100 - percentage; // invert percentage as driving the LED maybe inverted
  
  if(pLED->Config.Mode == IO_SIGNAL) { // if alternate
    if(percentage>100) percentage = 100; // or TrapError, clip to max 100
    if(percentage<0) LEDsSetDutyCycle(index, PercentToDuty[0]); // 0 to 100 becomes 0 to 1000 as log, later use a lookup table
      else LEDsSetDutyCycle(index, PercentToDuty[percentage]); // 0 to 100 becomes 0 to 1000 as log, later use a lookup table
  }else{
    if(percentage<50){ // Turn LED OFF
      IO_PadSetHigh(pLED->PadName);
    }else{ // Turn LED ON
      IO_PadSetLow(pLED->PadName);
    };
  };
  
  //nov24 TimedLEDs[index].Dim_perc = percentage; // save the used percentage dimming value
}

// Timer HW peripheral handling to generate PWM 

/* Timer handler declaration */
// PWM LED Dimmig demo enabled
TIM_HandleTypeDef    LED_TimHandle;
LPTIM_HandleTypeDef  LED_LPTimHandle;
/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfigLED;
LPTIM_OC_ConfigTypeDef sLPConfigLED;

void LEDsConfigureTimers(TIM_TypeDef* Timer) {

  LED_TimHandle.Instance = Timer;
  LED_TimHandle.Init.Prescaler         = BASE_TIMER_CLOCK_MHZ-1; // SYSCLK = 48 MHz, timer will run at 1MHz
  LED_TimHandle.Init.Period            = 999; // counter period is 1000 steps
  LED_TimHandle.Init.ClockDivision     = 0;
  LED_TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  LED_TimHandle.Init.RepetitionCounter = 0;
  if (HAL_TIM_PWM_Init(&LED_TimHandle) != HAL_OK)
  { /* Initialization Error */
    TrapError();
  }
}


void LEDsConfigureLPTimers(LPTIM_TypeDef* Timer) {

  LED_LPTimHandle.Instance = Timer;
  LED_LPTimHandle.Init.Clock.Prescaler         = LPTIM_PRESCALER_DIV64; // will be higher frequency as we can't divide by 95 BASE_TIMER_CLOCK_MHZ-1; // SYSCLK = 48 MHz, timer will run at 1MHz
  LED_LPTimHandle.Init.Period            = 999; // counter period is 1000 steps
//  LED_LPTimHandle.Init.ClockDivision     = 0;
//  LED_LPTimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  LED_LPTimHandle.Init.RepetitionCounter = 0;
  LED_LPTimHandle.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
  if (HAL_LPTIM_Init(&LED_LPTimHandle) != HAL_OK)
  { /* Initialization Error */
    TrapError();
  }
}


void LEDsConfigureChannel(TIM_TypeDef* Timer, uint32_t TIM_CHANNEL) {
  
  LED_TimHandle.Instance = Timer;

  sConfigLED.OCMode       = TIM_OCMODE_PWM1;
  sConfigLED.OCPolarity   = TIM_OCPOLARITY_LOW;
  sConfigLED.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfigLED.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfigLED.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  sConfigLED.OCIdleState  = TIM_OCIDLESTATE_RESET;
  sConfigLED.Pulse = 1;  
  if (HAL_TIM_PWM_ConfigChannel(&LED_TimHandle, &sConfigLED, TIM_CHANNEL ) != HAL_OK)
    TrapError();
  
  if (HAL_TIM_PWM_Start(&LED_TimHandle, TIM_CHANNEL) != HAL_OK)
    TrapError();
  
}

/*
HAL_StatusTypeDef HAL_LPTIM_OC_ConfigChannel(LPTIM_HandleTypeDef *hlptim, const LPTIM_OC_ConfigTypeDef *sConfig,
                                             uint32_t Channel);
*/


void LEDsConfigureLPChannel(LPTIM_TypeDef* Timer, uint32_t TIM_CHANNEL) {
  
  LED_LPTimHandle.Instance = Timer;

  //sLPConfigLED.OCMode       = TIM_OCMODE_PWM1;
  sLPConfigLED.OCPolarity   = LPTIM_OCPOLARITY_HIGH;//LOW; //TIM_OCPOLARITY_LOW;
  //sLPConfigLED.OCFastMode   = TIM_OCFAST_DISABLE;
  //sLPConfigLED.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  //sLPConfigLED.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  //sLPConfigLED.OCIdleState  = TIM_OCIDLESTATE_RESET;
  sLPConfigLED.Pulse = 1;  
  if (HAL_LPTIM_OC_ConfigChannel(&LED_LPTimHandle, &sLPConfigLED, TIM_CHANNEL ) != HAL_OK)
    TrapError();
  
  if (HAL_LPTIM_PWM_Start(&LED_LPTimHandle, TIM_CHANNEL) != HAL_OK)
    TrapError();
  
}


void LEDsSetDutyCycle(uint8_t index, uint32_t duty) {
  
  IO_Pad_t * pLED = TimedLEDs[index].pLED; // get handle to the pointed LED element
  if(pLED==0) return; // LED does not exist //2025
  TIM_TypeDef* TIM = (TIM_TypeDef*) PadSignal2PPP(pLED);  
  if(TIM==0) TrapError();
  
  uint32_t TIM_CHANNEL = pLED->Config.Signal;

  sConfigLED.Pulse = duty; // Changing Timer PWM duty fails to work in HAL TIM state machine. Was not foreseen application use-case !
  // 4hours rework cost adaptation
  // monster hack which destroys the user LED code entirely: // not scalable, not portable, not efficient
  if(TIM_CHANNEL==TIM15_CH1)
    TIM15->CCR1 = duty; // for a single hardcoded LED....
  if(TIM_CHANNEL==TIM15_CH2)
    TIM15->CCR2 = duty; // for a single hardcoded LED....
  if(TIM_CHANNEL==LPTIM2_CH1)
    LPTIM2->CCR1 = duty;
  if(TIM_CHANNEL==TIM3_CH4)
    TIM3->CCR4 = duty;
 
  
}


void TimedLED_10msecTick(void) { // should be called from main loop. not timing critical.
  
  uint8_t index;
  TimedLED_t* pTimedLED;
  
  // coming here every 10msec 
  for(index=1;index<LED_COUNT;index++) {
    
    pTimedLED = &TimedLEDs[index];
    if(pTimedLED==0) continue; // LED does not exist //2025
    if(pTimedLED->Blink_Countdown_x10ms==0) continue; // if countdown is zero, do nothing
    // countdown is non zero. decrement it.
    pTimedLED->Blink_Countdown_x10ms--;
    if(pTimedLED->Blink_Countdown_x10ms==0) { // cycle completed
      if(pTimedLED->Blink_Countdown==0) continue; // no more blink pulse needed
      // blink pulse needed
      if(pTimedLED->Blink_Countdown!=BRISK_LED_BLINK_FOREVER) pTimedLED->Blink_Countdown--; // if not infinite blinking, countdown, another pulse to run
      if(pTimedLED->Blink_Countdown) pTimedLED->Blink_Countdown_x10ms = pTimedLED->Blink_Period_x10ms;
    };
    
    // update the LED on/off dimming (refresh every 10 msec in case of dynamic/ISR change or ESD glitch)
    if(pTimedLED->Blink_Countdown_x10ms<=pTimedLED->Blink_OffTime_x10ms) { // Set LED OFF
      BriskDimLED(index, 0);
    } else { // Set LED ON
      BriskDimLED(index, pTimedLED->Dim_perc);
    };
    
  }; // for
  
}

// now here is the function to set the parameters including timings
void BriskDimTimedLED(uint8_t index, int8_t percentage) { // static, blinking disabled
  
  if(BriskLEDsDisabled==1) return;
    
  TimedLED_t* pTimedLED = &TimedLEDs[index];
  if(pTimedLED==0) return; // LED does not exist //2025
  if(percentage>100) TrapError();
  pTimedLED->Blink_Countdown_x10ms = 0; // disable all blinking
  pTimedLED->Dim_perc = percentage;
  BriskDimLED(index, percentage);
}

// use this function AFTER setting the dimming. Subject to change.
void BriskBlinkTimedLED(uint8_t index, uint16_t Blink_Period_ms, uint16_t Blink_OffTime_ms, uint8_t Blink_Countdown ) {
  
  TimedLED_t* pTimedLED = &TimedLEDs[index];
  if(pTimedLED==0) return; // LED does not exist //2025
  if(BriskLEDsDisabled==1) return;
  
  if(Blink_OffTime_ms>Blink_Period_ms) TrapError();
  
  pTimedLED->Blink_OffTime_x10ms = (Blink_OffTime_ms)/10;
  pTimedLED->Blink_Countdown = Blink_Countdown;
  //pTimedLED->Dim_perc = pTimedLED->pLED->Dim_perc; // we backup the chosen dimming // nov24
  pTimedLED->Blink_Period_x10ms = (Blink_Period_ms)/10;
  pTimedLED->Blink_Countdown_x10ms = pTimedLED->Blink_Period_x10ms; // this will trigger the ISR handling of the LED
  
}


uint32_t gLED_TestCounter = 0;

void TestLEDs(void) {

    uint8_t i;
    uint32_t duty;
 // skip the test    
    
#if 1    
     for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_OFF);
 
     for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_ON);
      
      for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_OFF);
do {     
      for(duty=0;duty<=100;duty++, HAL_Delay(5))
        for(i=1;i<LED_COUNT;i++)
          BriskDimTimedLED(i,duty);
      

        for(duty=0;duty<=100;duty++, HAL_Delay(5))
          for(i=1;i<LED_COUNT;i++)
            BriskDimTimedLED(i,100-duty);
} while(0);        

    return;
    
    if(gLED_TestCounter==0) {
    
      for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_OFF);
     
    }else 
    if(gLED_TestCounter<3) {

      for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_ON);
      
      for(i=1;i<LED_COUNT;i++, HAL_Delay(15))
        BriskDimTimedLED(i, BRISK_LED_OFF);

    }else 
    if(gLED_TestCounter<5) {

      // Dimmable option
      for(duty=0;duty<=100;duty++, HAL_Delay(5))
        for(i=1;i<LED_COUNT;i++)
          BriskDimTimedLED(i,duty);
      

        for(duty=0;duty<=100;duty++, HAL_Delay(5))
          for(i=1;i<LED_COUNT;i++)
            BriskDimTimedLED(i,100-duty);
          
    }/*else
    
    */
#endif    
}


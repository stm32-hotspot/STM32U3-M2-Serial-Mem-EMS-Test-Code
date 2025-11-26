/**
  ******************************************************************************
  * @file    commons.c (handy functions)
  * @author  A.Es and S.Ma
  * @brief   Iproved, Use of Debug cycle counter to get time elapsed and better than NOPs delay.
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
//#include "brisk.h"
//#include "commons.h"

// NOPs method

void NOP_Init(void) {
}

void NOPs(uint32_t loop) { // global
  while(loop--) asm("nop\n"); 
}

// Cycle counter method
volatile uint32_t *DWT_CYCCNT = (uint32_t *) 0xE0001004; // this is the pointer to the cycle counter


void CNT_Init(void)
{
  volatile uint32_t *DEMCR = (uint32_t *) 0xE000EDFC;  
  volatile uint32_t *LAR  = (uint32_t *) 0xE0001FB0;   // <-- added lock access register  
  volatile uint32_t *DWT_CONTROL = (uint32_t *) 0xE0001000;

  *DEMCR = *DEMCR | 0x01000000;     // enable trace
  *LAR = 0xC5ACCE55;                // <-- added unlock access to DWT (ITM, etc.)registers
  *DWT_CONTROL = *DWT_CONTROL | 1;  // enable DWT cycle counter
}

uint32_t CNT_GetTimeCy(void) 
{
  return *DWT_CYCCNT;
}  

//int32_t CNT_Delay_cy(uint32_t cy) // pure uncalibrated delay. Keep it <2 msec prefferably as it is blocking
//{
//        uint32_t start = *DWT_CYCCNT;  // start get the number of instrucion executed since the start of the code
//	while((*DWT_CYCCNT - start) < cy){} // we wait the number of cycle defined by lenght
//        return(0);
//}
//
//int32_t CNT_Delay_us(uint32_t us) // compiler vendor, option, cache, interrupt nearly independent (vs NOPs method)
//{
//        int32_t start = *DWT_CYCCNT;  // start get the number of instrucion executed since the start of the code
//        int32_t cy = us * SYS_CLK_MHZ; // here we assume single core use of this function
//	while((*DWT_CYCCNT - start) < cy){} // we wait the number of cycle defined by lenght
//        return(0);
//}

int32_t CNT_Delay_cy(uint32_t cy) // pure uncalibrated delay. Keep it <2 msec prefferably as it is blocking
{
        uint32_t start = *DWT_CYCCNT;  // removed sign ! start get the number of instrucion executed since the start of the code
	while(((uint32_t) (*DWT_CYCCNT) - start) < cy){} // we wait the number of cycle defined by lenght
        return(0);
}
 
int32_t CNT_Delay_us(uint32_t us) // compiler vendor, option, cache, interrupt nearly independent (vs NOPs method)
{
        uint32_t start = *DWT_CYCCNT;  // start get the number of instrucion executed since the start of the code
        uint32_t cy = us * SYS_CLK_MHZ; // here we assume single core use of this function
	while(((uint32_t)(*DWT_CYCCNT) - start) < cy){} // we wait the number of cycle defined by lenght
        return(0);
}

int32_t CNT_cy2us_x10(uint32_t cy) { // 0.1 us unit for more precision in small numbers
  
  return ((cy*10)/SYS_CLK_MHZ); // rounded down
}

//---------------- now we choose the implementation

void MinDelay_us(uint32_t us) {
  
  CNT_Delay_us(us);
  //NOPs(us * SYS_CLK_MHZ / 2); // gross
}

void MinDelay_ms(uint32_t ms) { // this can be SW delay with non exact delay.
  
  while(ms--)
    MinDelay_us(1000);
}



int32_t Interpolate_i32 (int32_t x0, int32_t x1, int32_t y0, int32_t y1, int32_t x) { 
  int32_t dwQ;
  dwQ = ((y1-y0))*x+(x1*y0)-(x0*y1);	// overflow not checked yet
  dwQ = dwQ / (x1-x0);// we can also do roundings here
  
  return dwQ;
}

// copy memory block
void CopyFromToByteSize (void* src, const void* dst, uint32_t cnt) {
	uint8_t *d = (uint8_t*)dst;
	uint8_t *s = (uint8_t*)src;

	while(cnt--) {
          *d++ = *s++;
	}
}

void FillByteToByteSize ( uint8_t ByteValue, const void* dst, uint32_t cnt) {
	uint8_t *d = (uint8_t*)dst;

	while(cnt--) {
          *d++ = ByteValue;
	}
}


// copy strings
void StringCopy (void* dst, const void* src, uint32_t cnt) {
	uint8_t *d = (uint8_t*)dst;
	const uint8_t *s = (const uint8_t*)src;

        while(cnt--) {
          *d = *s;
          if(*d == 0) break; // after copy the last null char.
          d++;s++;
	}
}


// Compare memory block
uint32_t BytesIdenticals (const void* dst, const void* src, uint32_t cnt) {	/* ZR:same, NZ:different */

  const uint8_t *d = (const uint8_t *)dst, *s = (const uint8_t *)src;

  if(cnt==0) TrapError();
  
  while(cnt--) {
    if(*d++ != *s++) return 0;
  }

  return 1;
}

// Compare strings
uint32_t StringsIdenticals (const void* dst, const void* src, uint32_t cnt) {	/* ZR:same, NZ:different */
  
	const uint8_t *d = (const uint8_t *)dst, *s = (const uint8_t *)src;

        if(cnt==0) TrapError();

        while(cnt--) {
		if( *d != *s ) return 0;
                if(*d==0) return 1;
                d++;s++;
	};

	return 1;
}






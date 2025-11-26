/**
  ******************************************************************************
  * @file    pfifo.c (Software FIFO -cyclic buffer-)
  * @author  S.Ma
  * @brief   This is a pointers SW FIFO with statistic to tune the buffer size. Has disable interrupt code segments for it.
  *          Triggers on FIFO gets empty/no longer empty, full/no longer full to manage interrupt enable/disable
  *          FIFO which manages 32 bit pointers to make a spooler, dodging complex coding when one peripheral is shared across threads.
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
#include "pfifo.h"

// This creates a tube, byte size, between 2 different processes, being under interrupt or not.
// When calling these functions, disable interrupt before optimizing later on.
// This is a circular buffer which can add/remove byte from either end (head or tail)
// we kept only the functions to implement a FIFO.

uint32_t Hookpfifo_NoLongerEmpty(pfifo_t* ppfifo, uint32_t (*fn)(uint32_t), uint32_t ct) {

  ppfifo->ctNoLongerEmpty = ct;  
  ppfifo->fnNoLongerEmpty = fn;
  return 0;
}

uint32_t Hookpfifo_Emptied(pfifo_t* ppfifo, uint32_t (*fn)(uint32_t), uint32_t ct) {

  ppfifo->ctEmptied = ct;    
  ppfifo->fnEmptied = fn;
  return 0;
}

//==========================================================
// sw fifo creation, setup
uint32_t Newpfifo(pfifo_t* ppfifo, uint32_t begin, int32_t size) {

  if(size==0)
    while(1); // no memory for it?

  ppfifo->pbLowest = begin; // left is decreasing memory address with rollover circular buffer
  ppfifo->pbHighest = begin + (size - 1); // tight is increasing memory address with rollover circular buffer inclusive pointer
  
  // valid SRAM space
  ppfifo->bCountLimit = size; // we validate the strand size (action can occur)
  ppfifo->bCount = 0;
  return begin;
}

//==========================================================
// manage the left side

static uint32_t _Clippfifo_Down(pfifo_t* ppfifo);
static uint32_t _Clippfifo_Down(pfifo_t* ppfifo) { // 0 or 1, returns the bit pointer

//  if(bCountLimit==0) while(1); // error
  
  if(ppfifo->bCount==0) // if strand empty, nothing to read from it, error
    while(1); // error, nothing on this strand, check its size is non zero first!

  ppfifo->bCount--;  // strand not empty. Get the left bit first, reduce strand size
  //ppfifo->Out = *(uint8_t*) ppfifo->pbDown; // we create one more bit space on the left (lower memory)
  ppfifo->Out = *(uint32_t*) ppfifo->pbDown; // we create one more bit space on the left (lower memory)
  
  // point to the new left side
  ppfifo->pbDown++;
  if(ppfifo->pbDown>ppfifo->pbHighest) // circular memory space bottom reached.
    ppfifo->pbDown = ppfifo->pbLowest; // jump to higher end
  
  if(ppfifo->bCount==0) {
    if(ppfifo->fnEmptied) {// if the strand turns empty, tell someone?
      ppfifo->fnEmptied(ppfifo->ctEmptied);
    }else{
      ppfifo->FlagEmptied = 1;
    };
  };

  // one item was added, check if someone is ready to empty it
//TBD  if(ppfifo->fnIn) ((uint32_t(*)(uint32_t))ppfifo->fnIn)(ppfifo->ctIn);
  
  return ppfifo->Out;// return read valid bit
}

//==========================================================
// manage the right side
static uint32_t _Gluepfifo_Up(pfifo_t* ppfifo);
static uint32_t _Gluepfifo_Up(pfifo_t* ppfifo) {

  if(ppfifo->bCountLimit==0) while(1); // error
  
  if(ppfifo->bCount==0) { // if strand empty: Create the first bit
    ppfifo->pbUp = ppfifo->pbDown = ppfifo->pbLowest; // arbitrary left is the start creation side
  }
  else {  // the strand exist, check fullness
    if(ppfifo->bCount>=ppfifo->bCountLimit) {
      ppfifo->FlagFull=1;
      return 0;
      //while(1); // too big! improve memory allocation, flag, roll over etc... depends on scheme
    }
    // we create one more bit space on the left (lower memory)
    ppfifo->pbUp++;
    if(ppfifo->pbUp>ppfifo->pbHighest) // rollover if out of range
      ppfifo->pbUp = ppfifo->pbLowest;
  };
  
  // write in memory and increase bit counter safely
  //*(uint8_t*)ppfifo->pbUp = ppfifo->In;
  *(uint32_t*)ppfifo->pbUp = ppfifo->In;
  ppfifo->bCount++;
  if(ppfifo->bCount>ppfifo->bCountMax) ppfifo->bCountMax = ppfifo->bCount;
  
  if(ppfifo->bCount==1) {
    if(ppfifo->fnNoLongerEmpty) { // if the strand not empty, tell someone?
      ppfifo->fnNoLongerEmpty(ppfifo->ctNoLongerEmpty); //!!! this can activate interrupt IRQ...
    }else{
      ppfifo->FlagNoLongerEmpty = 1;
    };
  }

  // one item was added, check if someone is ready to empty it
//TBD  if(pu8fifo->fnOut) ((uint32_t(*)(uint32_t))pu8fifo->fnOut)(pu8fifo->ctOut);

  return ppfifo->pbUp;
}



uint32_t AddTopfifo(pfifo_t* ppfifo, uint32_t In) {
  
  ppfifo->In = In;
  return Gluepfifo_Up(ppfifo);
}


// reading the strand content: The strand does not kill the bits when cut!

// here we make the interrupt disable wrapper for fifo functions. not optimized.
// good enough to make progress


uint32_t Clippfifo_Down(pfifo_t* ppfifo) {
  
  uint32_t result;
  uint32_t tmp = __get_PRIMASK();
  __set_PRIMASK(tmp | 1);
  result = _Clippfifo_Down(ppfifo);
  __set_PRIMASK(tmp);
  return result;
}

uint32_t Gluepfifo_Up(pfifo_t* ppfifo) {
  
  uint32_t result;
  uint32_t tmp = __get_PRIMASK();
  __set_PRIMASK(tmp | 1);
  result = _Gluepfifo_Up(ppfifo);
  __set_PRIMASK(tmp);
  return result;
}



/**
  ******************************************************************************
  * @file    pfifo.h
  * @author  S.Ma
  * @brief   ...
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

#ifndef _PFIFO_H_
#define _PFIFO_H_

// this can be used for FIFO, STACK, etc... and can be used within interrupt or main loop, or between main loop and interrupts. It's a time queue, within a limited RAM segment, a data snake
// can grow up from its head, or from its tail, and like the game, the snake can get longer or shorter from either head or tail.
// Once it becomes empty, a hook can be triggered, or a flag set
// Once it is no longer empty, same phylosophy
// TODO: Full, nolongerfull management
// The bit version is called bitcapillar
// The pointer version is called StuffsArtery

typedef struct {
  // by playing with head and tail, any emulation is possible like FIFO, STACK and any exotic things including duplicates, like DNA Strands
  uint32_t pbLowest; // points to assigned SRAM start address (telomere)
  uint32_t pbHighest;  // pointer to assigned SRAM end address (inclusive) (telomere)
  uint32_t bCountLimit; // This is the SRAM buffer size
  uint32_t bCount; // This is the used SRAM buffer size
  uint32_t bCountMax; // This is for stats, to know how much we use the memory in real case over time
  uint32_t pbDown; // current tail (low address)
  uint32_t pbUp; // current head (high address)

  uint32_t In; // the byte to glue
  uint32_t Out; // the byte that was clipped
  
// these below are for higher end use, later on...
  uint32_t (*fnNoLongerEmpty)(uint32_t);
  uint32_t ctNoLongerEmpty;
  uint32_t (*fnEmptied)(uint32_t);
  uint32_t ctEmptied;
  
//  uint32_t (*fnFull)(uint32_t);
//  uint32_t ctFull;
//  uint32_t (*fnNoLongerFull)(uint32_t);
//  uint32_t ctNoLongerFull;

  uint8_t FlagNoLongerEmpty : 1; // unused, set by FIFO cleared by User
  uint8_t FlagEmptied : 1; // unused, set by FIFO, cleared by USer
  uint8_t FlagFull : 1; // unused, set by FIFO, cleared by User (loss of data)
//  uint8_t FlagNoLongerFull : 1; // unused
  
} pfifo_t;


uint32_t Newpfifo(pfifo_t* ppfifo, uint32_t begin, int32_t size);

uint32_t Hookpfifo_NoLongerEmpty(pfifo_t* ppfifo, uint32_t (*fn)(uint32_t), uint32_t ct);
uint32_t Hookpfifo_Emptied(pfifo_t* ppfifo, uint32_t (*fn)(uint32_t), uint32_t ct);

uint32_t Clippfifo_Down(pfifo_t* ppfifo);
uint32_t Gluepfifo_Up(pfifo_t* ppfifo);

//---------- to access the head and tail

uint32_t AddTopfifo(pfifo_t* ppfifo, uint32_t In);


#endif

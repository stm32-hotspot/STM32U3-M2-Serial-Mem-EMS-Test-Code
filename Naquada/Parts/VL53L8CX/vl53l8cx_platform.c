/**
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "main.h"
#include "vl53l8cx_platform.h"

extern I2C_SlaveDevice_t VL53L8_Slave;

uint8_t RdByte(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_value)
{
	uint8_t status = 255;

	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
  I2C_SlaveDevice_t* pS = &VL53L8_Slave;

  // we use global for now
  pS->SubAdrBytes[1] = RegisterAdress & 0xFF; // LSB
  pS->SubAdrBytes[0] = RegisterAdress >> 8; // MSB
  pS->pWriteByte = (uint8_t*) 0;
  pS->WriteByteCount = 0;
  pS->pReadByte = p_value;
  pS->ReadByteCount = 1;
  status = I2C_MasterIO_AccessSlave(pS);
  return status;
}

uint8_t WrByte(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t value)
{
	uint8_t status = 255;

	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
  I2C_SlaveDevice_t* pS = &VL53L8_Slave;

  // we use global for now
  pS->SubAdrBytes[1] = RegisterAdress & 0xFF; // LSB
  pS->SubAdrBytes[0] = RegisterAdress >> 8; // MSB
  pS->pWriteByte = (uint8_t*) &value;
  pS->WriteByteCount = 1;
  pS->pReadByte = 0;
  pS->ReadByteCount = 0;
  status = I2C_MasterIO_AccessSlave(pS);
  return status;
}

uint8_t WrMulti(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t status = 255;
	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
	
  I2C_SlaveDevice_t* pS = &VL53L8_Slave;
  
  // we use global for now
  pS->SubAdrBytes[1] = RegisterAdress & 0xFF; // LSB
  pS->SubAdrBytes[0] = RegisterAdress >> 8; // MSB
  pS->pWriteByte = p_values;
  pS->WriteByteCount = size;
  pS->pReadByte = 0;
  pS->ReadByteCount = 0;
  status = I2C_MasterIO_AccessSlave(pS);
  return status;
}

uint8_t RdMulti(
		VL53L8CX_Platform *p_platform,
		uint16_t RegisterAdress,
		uint8_t *p_values,
		uint32_t size)
{
	uint8_t status = 255;

	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
  I2C_SlaveDevice_t* pS = &VL53L8_Slave;

  // we use global for now
  pS->SubAdrBytes[1] = RegisterAdress & 0xFF; // LSB
  pS->SubAdrBytes[0] = RegisterAdress >> 8; // MSB
  pS->pWriteByte = (uint8_t*) 0;
  pS->WriteByteCount = 0;
  pS->pReadByte = p_values;
  pS->ReadByteCount = size;
  status = I2C_MasterIO_AccessSlave(pS);

  return status;
}

uint8_t Reset_Sensor(
		VL53L8CX_Platform *p_platform)
{
	uint8_t status = 0;
	
	/* (Optional) Need to be implemented by customer. This function returns 0 if OK */
	
	/* Set pin LPN to LOW */
	/* Set pin AVDD to LOW */
	/* Set pin VDDIO  to LOW */
	/* Set pin CORE_1V8 to LOW */
	VL53L8CX_WaitMs(p_platform, 100);

	/* Set pin LPN to HIGH */
	/* Set pin AVDD to HIGH */
	/* Set pin VDDIO to HIGH */
	/* Set pin CORE_1V8 to HIGH */
	VL53L8CX_WaitMs(p_platform, 100);

	return status;
}

void SwapBuffer(
		uint8_t 		*buffer,
		uint16_t 	 	 size)
{
	uint32_t i, tmp;
	
	/* Example of possible implementation using <string.h> */
	for(i = 0; i < size; i = i + 4) 
	{
		tmp = (
		  buffer[i]<<24)
		|(buffer[i+1]<<16)
		|(buffer[i+2]<<8)
		|(buffer[i+3]);
		
		memcpy(&(buffer[i]), &tmp, 4);
	}
}	

uint8_t VL53L8CX_WaitMs(
		VL53L8CX_Platform *p_platform,
		uint32_t TimeMs)
{
	uint8_t status = 0;

	/* Need to be implemented by customer. This function returns 0 if OK */
        HAL_Delay(TimeMs);	
	return status;
}

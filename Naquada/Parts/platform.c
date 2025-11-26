/* Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 */

/*
 Copyright (c) 2021, STMicroelectronics - All Rights Reserved

 This file : part of VL53L4CD Ultra Lite Driver and : dual licensed, either
 'STMicroelectronics Proprietary license'
 or 'BSD 3-clause "New" or "Revised" License' , at your option.

*******************************************************************************

 'STMicroelectronics Proprietary license'

*******************************************************************************

 License terms: STMicroelectronics Proprietary in accordance with licensing
 terms at www.st.com/sla0081

 STMicroelectronics confidential
 Reproduction and Communication of this document : strictly prohibited unless
 specifically authorized in writing by STMicroelectronics.


*******************************************************************************

 Alternatively, VL53L4CD Ultra Lite Driver may be distributed under the terms of
 'BSD 3-clause "New" or "Revised" License', in which case the following
 provisions apply instead of the ones mentioned above :

*******************************************************************************

 License terms: BSD 3-clause "New" or "Revised" License.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
*/

#include "platform.h"
#include "main.h"

extern I2C_SlaveDevice_t VL53_Slave;

uint8_t VL53L4CD_RdDWord(Dev_t dev, uint16_t RegisterAdress, uint32_t *value)
{
	uint8_t status = 255;
	
	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
  I2C_SlaveDevice_t* pS = &VL53_Slave;
  uint8_t buf[4];
  // we use global for now
  pS->SubAdrBytes[1] = RegisterAdress & 0xFF; // LSB
  pS->SubAdrBytes[0] = RegisterAdress >> 8; // MSB
  pS->pWriteByte = 0;
  pS->WriteByteCount = 0;
  pS->pReadByte = buf;
  pS->ReadByteCount = 4;
  status = I2C_MasterIO_AccessSlave(pS);
  *value = MAKEDWORD_HI_LO(buf[0],buf[1],buf[2],buf[3]);
  return status;
}

uint8_t VL53L4CD_RdWord(Dev_t dev, uint16_t RegisterAdress, uint16_t *value)
{
	uint8_t status = 255;
          uint8_t buf[4];
	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
  I2C_SlaveDevice_t* pS = &VL53_Slave;

  // we use global for now
  pS->SubAdrBytes[1] = RegisterAdress & 0xFF; // LSB
  pS->SubAdrBytes[0] = RegisterAdress >> 8; // MSB
  pS->pWriteByte = (uint8_t*) 0;
  pS->WriteByteCount = 0;
  pS->pReadByte = buf;
  pS->ReadByteCount = 2;
  status = I2C_MasterIO_AccessSlave(pS);
  *value = MAKEWORD_HI_LO(buf[0],buf[1]);
  return status;
}

uint8_t VL53L4CD_RdByte(Dev_t dev, uint16_t RegisterAdress, uint8_t *value)
{
	uint8_t status = 255;
	uint8_t buf[4];
	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
  I2C_SlaveDevice_t* pS = &VL53_Slave;

  // we use global for now
  pS->SubAdrBytes[1] = RegisterAdress & 0xFF; // LSB
  pS->SubAdrBytes[0] = RegisterAdress >> 8; // MSB
  pS->pWriteByte = (uint8_t*) 0;
  pS->WriteByteCount = 0;
  pS->pReadByte = value;
  pS->ReadByteCount = 1;
  status = I2C_MasterIO_AccessSlave(pS);
  return status;
	
}

uint8_t VL53L4CD_WrByte(Dev_t dev, uint16_t RegisterAdress, uint8_t value)
{
	uint8_t status = 255;

	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
  I2C_SlaveDevice_t* pS = &VL53_Slave;

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

uint8_t VL53L4CD_WrWord(Dev_t dev, uint16_t RegisterAdress, uint16_t value)
{
	uint8_t status = 255;
	
	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
	
  I2C_SlaveDevice_t* pS = &VL53_Slave;
  uint8_t buf[4];
  buf[0] = (value >> 8); // MSB
  buf[1] = value & 0xFF; // LSB
  // we use global for now
  pS->SubAdrBytes[1] = RegisterAdress & 0xFF; // LSB
  pS->SubAdrBytes[0] = RegisterAdress >> 8; // MSB
  pS->pWriteByte = buf;
  pS->WriteByteCount = 2;
  pS->pReadByte = 0;
  pS->ReadByteCount = 0;
  status = I2C_MasterIO_AccessSlave(pS);
  return status;

}

uint8_t VL53L4CD_WrDWord(Dev_t dev, uint16_t RegisterAdress, uint32_t value)
{
	uint8_t status = 255;
  uint8_t buf[4];
	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
	
  I2C_SlaveDevice_t* pS = &VL53_Slave;

  buf[0] = (uint8_t) value >>24;
  buf[1] = (uint8_t) value >> 16;
  buf[2] = (uint8_t) value >> 8;
  buf[3] = (uint8_t) value;
  
  // we use global for now
  pS->SubAdrBytes[1] = RegisterAdress & 0xFF; // LSB
  pS->SubAdrBytes[0] = RegisterAdress >> 8; // MSB
  pS->pWriteByte = buf;
  pS->WriteByteCount = 4;
  pS->pReadByte = 0;
  pS->ReadByteCount = 0;
  status = I2C_MasterIO_AccessSlave(pS);
  return status;

}

uint8_t WaitMs(Dev_t dev, uint32_t TimeMs)
{
	uint8_t status = 255;
	/* To be filled by customer */
        HAL_Delay(TimeMs);
	return status;
}

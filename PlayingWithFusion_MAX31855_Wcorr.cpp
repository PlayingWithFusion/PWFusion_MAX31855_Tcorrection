/***************************************************************************
* File Name: PlayingWithFusion_MAX31855-1CH.cpp
* Processor/Platform: Arduino Uno R3 (tested)
* Development Environment: Arduino 1.0.5
*
* Designed for use with with Playing With Fusion MAX31855 thermocouple
* breakout board: SEN-30001 (any type) or SEN-30002 (any type)
*   ---> http://playingwithfusion.com/productview.php?pdid=19
*	---> http://www.playingwithfusion.com/productview.php?pdid=20
*
* Copyright © 2013 Playing With Fusion, Inc.
* SOFTWARE LICENSE AGREEMENT: This code is released under the MIT License.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
* **************************************************************************
* REVISION HISTORY:
* Author			Date		Comments
* J. Steinlage		2013Dec30	Original version
* J. Steinlage      2014Apr06   Temperature Correction
*
* Playing With Fusion, Inc. invests time and resources developing open-source
* code. Please support Playing With Fusion and continued open-source
* development by buying products from Playing With Fusion!
*
* **************************************************************************
* ADDITIONAL NOTES:
* This file contains functions to initialize and run an Arduino Uno R3 in
* order to communicate with a MAX31855 single channel thermocouple breakout
* board. Funcionality is as described below:
*	- Initialize TC channel
*	- Read MAX31855 registers from Playing With Fusion SEN-30001 (any type)
*	- Properly unpack data into internal temp, TC temp and status variables
***************************************************************************/
#include "PlayingWithFusion_MAX31855_Wcorr.h"
#include "PlayingWithFusion_MAX31855_J_correction.h"
#include "PlayingWithFusion_MAX31855_T_correction.h"

PWFusion_MAX31855_TC::PWFusion_MAX31855_TC(int8_t CSx)
{
  // Function to initialize thermocouple channel, load private variables
  _cs = CSx;
  
  // immediately pull CS pin high to avoid conflicts on SPI bus
  digitalWrite(_cs, HIGH);
}

void PWFusion_MAX31855_TC::MAX31855_update(struct var_max31855 *tc_ptr)
{
  // Function to unpack and store MAX31855 data
  uint32_t full_read;
  full_read = spiread_32bits();	// all data is packed into 4 8-bit registers

  int16_t temp_i16;
  uint8_t temp_u8;
  uint8_t tc_type_var;
  
  // start new handling by un-packing MAX31855 internal temp data
  temp_i16 = (int16_t)(full_read >> 4);
  if(0x0800 & temp_i16)				// negative value
  {
	  temp_i16 &= 0x07FF;			// save only first 11 bits of data
	  temp_i16 |= 0xF000;			// handle negative values
  }
  else								// positive value
  {
	  temp_i16 &= 0x07FF;			// no sign, save only first 11 (pos val)
  }
  tc_ptr->ref_jcn_temp = temp_i16;	// save TC temp. Note: int16 with 4 bits of
	  									// resolution (2^-8 = 0.0625 deg C per bit)  
  // un-pack chip fault status
  if(full_read & 0x00010000) // fault detected...
  {
	  temp_u8 = (uint8_t)(full_read & 0x00000007); // get fault bits
	  tc_ptr->status = temp_u8;
	  // leave external (thermocouple) temp channels unchanged
  }
  else // no fault detected, go ahead and update TC temperature
  {
	  // make sure faults are cleared
	  tc_ptr->status = 0;

	  // un-pack TC temp data
	  temp_i16 = (int16_t)(full_read >> 18);
	  if(0x2000 & temp_i16)			// negative value
	  {
		  temp_i16 &= 0x1FFF;		// save only first 13 bits of data
		  temp_i16 |= 0xE000;		// handle sign
	  }
	  else							// positive value
	  {
		  temp_i16 &= 0x1FFF;		// no sign, save only first 13 (pos val)
	  }
	  tc_ptr->value = temp_i16;		// save TC temp. Note: int16 with 2 bits of
	  									// resolution (2^-2 = 0.25 deg C per bit)
	  
	  // now correct output temperature (NIST lookup tables)
	  // need to calculate mV reading of TC
      tc_ptr->Tcorr = TCcorrection(tc_ptr->tc_type, tc_ptr->value, tc_ptr->ref_jcn_temp);
  }

}

uint32_t PWFusion_MAX31855_TC::spiread_32bits(void)
{
  // Function to read 32 bits of SPI data
  uint8_t i;
  uint32_t four_bytes = 0;
  
  digitalWrite(_cs, LOW);	// set CS low
  _delay_ms(1);			// allow state transistion time
  
  four_bytes |= SPI.transfer(0x00);		// read 1st byte
  four_bytes <<= 8;						// shift data 1 byte left
  four_bytes |= SPI.transfer(0x00);		// read 2nd byte
  four_bytes <<= 8;						// shift data 1 byte left
  four_bytes |= SPI.transfer(0x00);		// read 3rd byte
  four_bytes <<= 8;						// shift data 1 byte left
  four_bytes |= SPI.transfer(0x00);		// read 4th byte
  
  digitalWrite(_cs, HIGH);	// set CS high before leaving
  return four_bytes;
}

float PWFusion_MAX31855_TC::TCcorrection(uint8_t _tc_type, int16_t _tc_value, int16_t _ref_jct_tmp)
{
/*  if(1 == _tc_type) // K-type
  {
    float tc_mV = (float)tc_ptr->value * 0.010319; // 41.276/4/1000 (1/4 degC/bit, 1mV/1000uV)
  }
  else if(2 == _tc_type) // J-type
  {
    float tc_mV = (float)tc_ptr->value * 0.014488; // 57.953/4/1000 (1/4 degC/bit, 1mV/1000uV)
  }
  else if(3 == _tc_type) // T-type
  {
*/    
  double temp_val, temp_idx_min1;
  double base_mV, tc_mV, to_0C_mV;
  double temp_idx = -100.00;
  volatile uint16_t index = 0;
	
  if(2 == _tc_type)
  {
	// J-type cal hard coded
	base_mV = (double)_ref_jct_tmp * 0.003259;	// 0.0625*52.136/1000 (ref scaling * ref coeff (uV/C) / 1000)
	tc_mV = ((double)_tc_value-((double)_ref_jct_tmp*0.25)) * 0.01448825; // 57.953/4/1000 (1/4 degC/bit, 1mV/1000uV)
	// it is worth noting that multiplying the ref jct temp by 0.25 is actually 0.0625 C/bit * 4 so it matches the scaling of the TC value
	to_0C_mV = base_mV + tc_mV;
	while((index<1410) && (to_0C_mV > temp_idx)) // in-bounds and mV < table value
	{
	  index++;
	  temp_idx = pgm_read_float(J_voltage + index);
	}
	if(1410==index)
	{
	  return 1200; // last element = max temperature range. 1200C for J-Type.
	}
	else
	{
	  if(0<index)
	  {
	    temp_idx      = pgm_read_float(J_voltage + index);
	    temp_idx_min1 = pgm_read_float(J_voltage + index - 1);
	    temp_val = ((double)index - J_temp_offset - 1) + ((to_0C_mV - temp_idx_min1)/(temp_idx - temp_idx_min1));
	  }
	  else
      {	  
	    temp_val = 0-J_temp_offset;
      }
	  return temp_val;
	}
   }
   else if(3 == _tc_type)
   {
	// T-type
	base_mV = (double)_ref_jct_tmp * 0.0026;	// 0.0625*41.56/1000 (ref scaling * ref coeff (uV/C) / 1000)
	tc_mV = ((double)_tc_value-((double)_ref_jct_tmp*0.25)) * 0.013045; // 52.18/4/1000 (1/4 degC/bit, 1mV/1000uV)
	// it is worth noting that multiplying the ref jct temp by 0.25 is actually 0.0625 C/bit * 4 so it matches the scaling of the TC value
	to_0C_mV = base_mV + tc_mV;
	while((index<670) && (to_0C_mV > temp_idx)) // in-bounds and mV < table value
	{
	  index++;
	  temp_idx = pgm_read_float(T_voltage + index);
	}
	if(670==index) // last element = max temperature range. 400C for T-Type.
	{
	  return 400;
	}
	else
	{
	  if(0<index)
	  {
	    temp_idx      = pgm_read_float(T_voltage + index);
	    temp_idx_min1 = pgm_read_float(T_voltage + index - 1);
	    temp_val = ((double)index - T_temp_offset - 1) + ((to_0C_mV - temp_idx_min1)/(temp_idx - temp_idx_min1));
	  }
	  else
      {	  
	    temp_val = 0-T_temp_offset;
      }
	  return temp_val;
	}
   }
   else // unknown type
   {
      return -555;
   }
}

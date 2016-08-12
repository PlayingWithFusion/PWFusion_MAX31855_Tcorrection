/***************************************************************************
* File Name: PlayingWithFusion_MAX31855_1CH.h
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
* **************************************************************************/

#ifndef PWF_MAX31855_1_h
#define PWF_MAX31855_1_h

#include "Arduino.h"			// use "WProgram.h" for IDE <1.0
#include "avr/pgmspace.h"
#include "util/delay.h"
#include "stdlib.h"
#include "PlayingWithFusion_MAX31855_STRUCT_corr.h"
#include "SPI.h"

class PWFusion_MAX31855_TC
{
 public:
  PWFusion_MAX31855_TC(int8_t CSx);
  void SPIbus_Init(void);
  void MAX31855_update(struct var_max31855 *tc_ptr);
  float TCcorrection(uint8_t tc_type, int16_t _tc_value, int16_t _ref_jct_tmp);

 private:
  int8_t _cs;
  uint32_t spiread_32bits(void);
};

#endif
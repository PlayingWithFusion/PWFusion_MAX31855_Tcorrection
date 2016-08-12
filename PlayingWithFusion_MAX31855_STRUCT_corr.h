/***************************************************************************
* File Name: PlayingWithFusion_MAX31855-1CH.h
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
* J. Steinlage      2014Apr06   Temperature Correction (add type, Tcorr)
*
* Playing With Fusion, Inc. invests time and resources developing open-source
* code. Please support Playing With Fusion and continued open-source
* development by buying products from Playing With Fusion!
*
* **************************************************************************/

#ifndef PWF_MAX31855_STRUCT_H
#define PWF_MAX31855_STRUCT_H

#include "avr/pgmspace.h"

struct var_max31855{
    int16_t  value;			// TC temperature, 0.25 dec C/bit w/ sign
    int16_t  ref_jcn_temp;	// temp of chip ref jcn, 0.0625 deg C/bit +sign
    uint8_t  status;		// TC status - valid/invalid + fault reason
	uint8_t  tc_type;		// Thermocouple type... 0=unk, 1=K, 2=J, 3=T
	float    Tcorr;			// Corrected TC temperature, based in NIST tables
};


#endif /* PWF_MAX31855_STRUCT_H */

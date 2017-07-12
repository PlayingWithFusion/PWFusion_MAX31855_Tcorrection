/***************************************************************************
* File Name: max31855_4ch_Tcorr_R01.ino
* Processor/Platform: Arduino Uno R3 (tested)
* Development Environment: Arduino 1.0.5
*
* Designed for use with with Playing With Fusion MAX31855 thermocouple
* breakout boards: SEN-30001, SEN-30002, SEN-30003 and SEN-30004
*   View all Playing With Fusion temperature produgts here:
*   --> http://www.playingwithfusion.com/productlist.php?catid=1004
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
* Author			      Date       Comments
* J. Steinlage			2013Dec30	 Original version
* J. Steinlage      2014Apr06  Temperature Correction
* J. Steinlage      2014Dec12  Added Mega connections table
* J. Steinlage      2017Jul11  Fixed include for struct file
* 
* Playing With Fusion, Inc. invests time and resources developing open-source
* code. Please support Playing With Fusion and continued open-source 
* development by buying products from Playing With Fusion!
*
* **************************************************************************
* ADDITIONAL NOTES:
* This file configures then runs a program on an Arduino Uno to read a 
* MAX31855 quad channel thermocouple breakout board and print results to
* a serial port.
*    - Configure Arduino Uno
*    - Read temperatures from MAX31855 IC (internal temp, thermocouple temp)
*    - read status information from sensor
*    - Write formatted information to serial port
* 
* Circuit:
*    Arduino Uno   Arduino Mega  -->  SEN-30002/3/4
*    MISO: pin 12  MISO: pin 50  -->  SDO (must not be changed for hardware SPI)
*    SCK:  pin 13  SCK:  pin 52  -->  SCLK (must not be changed for hardware SPI)
*    CS3:  pin  7         ''     -->  CS3
*    CS2:  pin  8         ''     -->  CS2
*    CS1:  pin  9         ''     -->  CS1
*    CS0:  pin  10        ''     -->  CS0
***************************************************************************/
// the sensor communicates using SPI, so include the hardware SPI library:
#include "SPI.h"
// include Playing With Fusion MAX31855 libraries
#include "PlayingWithFusion_MAX31855_Wcorr.h"
#include "PlayingWithFusion_MAX31855_STRUCT_corr.h"
#include "avr/pgmspace.h"

// #defines for thermocouple types
// #define K_type 1   - not currently in code
#define J_type 2
#define T_type 3

// setup CS pins used for the connection with the sensor
// other connections are controlled by the SPI library)
int8_t CS3_PIN =  7;
int8_t CS2_PIN =  8;
int8_t CS1_PIN =  9;
int8_t CS0_PIN = 10;

PWFusion_MAX31855_TC  thermocouple0(CS0_PIN);
PWFusion_MAX31855_TC  thermocouple1(CS1_PIN);
PWFusion_MAX31855_TC  thermocouple2(CS2_PIN);
PWFusion_MAX31855_TC  thermocouple3(CS3_PIN);

void setup()
{
  
  Serial.begin(115200);

  // setup for the the SPI library:
  SPI.begin();                        // begin SPI
  SPI.setDataMode(SPI_MODE1);         // MAX31865 is a Mode 1 device
                                      //    --> clock starts low, read on rising edge
  
  // initalize the chip select pins
  pinMode(CS0_PIN, OUTPUT);
  pinMode(CS1_PIN, OUTPUT);
  pinMode(CS2_PIN, OUTPUT);
  pinMode(CS3_PIN, OUTPUT);
  
  Serial.println("Playing With Fusion: MAX31855-4CH, SEN-30002");

}

void loop()
{
  delay(500);                                   // 500ms delay... can be much faster
  
  static struct var_max31855 TC_CH0 = {0, 0, 0, T_type, 0};
  static struct var_max31855 TC_CH1 = {0, 0, 0, J_type, 0};
  static struct var_max31855 TC_CH2 = {0, 0, 0, T_type, 0};
  static struct var_max31855 TC_CH3 = {0, 0, 0, T_type, 0};
  double tmp;
  struct var_max31855 *tc_ptr;
  
  // update TC0
  tc_ptr = &TC_CH0;
  thermocouple0.MAX31855_update(tc_ptr);        // Update MAX31855 readings 
  
  // update TC1
  tc_ptr = &TC_CH1;
  thermocouple1.MAX31855_update(tc_ptr);        // Update MAX31855 readings 
  
  // update TC2
  tc_ptr = &TC_CH2;
  thermocouple2.MAX31855_update(tc_ptr);        // Update MAX31855 readings 
  
  // update TC3
  tc_ptr = &TC_CH3;
  thermocouple3.MAX31855_update(tc_ptr);        // Update MAX31855 readings 
  
  // Print information to serial port
  
  
  // TC0
  Serial.println("Thermocouple 0:");            // Print TC0 header
  // MAX31855 Internal Temp
  tmp = (double)TC_CH0.ref_jcn_temp * 0.0625;  // convert fixed pt # to double
  Serial.print("Tint = ");                      // print internal temp heading
  if((-100 > tmp) || (150 < tmp)){Serial.println("unknown fault");}
  else{Serial.println(tmp);}
  // MAX31855 External (thermocouple) Temp
  tmp = (double)TC_CH0.value * 0.25;           // convert fixed pt # to double
  Serial.print("TC Temp = ");                   // print TC temp heading
  if(0x00 == TC_CH0.status){Serial.println(tmp);}
  else if(0x01 == TC_CH0.status){Serial.println("OPEN");}
  else if(0x02 == TC_CH0.status){Serial.println("SHORT TO GND");}
  else if(0x04 == TC_CH0.status){Serial.println("SHORT TO Vcc");}
  else{Serial.println("unknown fault");}
  
  
 

  // TC1
  Serial.println("Thermocouple 1:");            // Print TC1 header
  // MAX31855 Internal Temp
  tmp = (double)TC_CH1.ref_jcn_temp * 0.0625;  // convert fixed pt # to double
  Serial.print("Tint = ");                      // print internal temp heading
  if((-100 > tmp) || (150 < tmp)){Serial.println("unknown fault");}
  else{Serial.println(tmp);}
  // MAX31855 External (thermocouple) Temp
  tmp = (double)TC_CH1.value * 0.25;           // convert fixed pt # to double
  Serial.print("TC Temp / Tcorr = ");                   // print TC temp heading
  if(0x00 == TC_CH1.status)
  {
    Serial.print(tmp);
    Serial.print("  /  ");
    Serial.println(TC_CH1.Tcorr, 4);
  }
  else if(0x01 == TC_CH1.status){Serial.println("OPEN");}
  else if(0x02 == TC_CH1.status){Serial.println("SHORT TO GND");}
  else if(0x04 == TC_CH1.status){Serial.println("SHORT TO Vcc");}
  else{Serial.println("unknown fault");}
  
  
  // TC2
  Serial.println("Thermocouple 2:");            // Print TC0 header
  // MAX31855 Internal Temp
  tmp = (double)TC_CH2.ref_jcn_temp * 0.0625;  // convert fixed pt # to double
  Serial.print("Tint = ");                      // print internal temp heading
  if((-100 > tmp) || (150 < tmp)){Serial.println("unknown fault");}
  else{Serial.println(tmp);}
  // MAX31855 External (thermocouple) Temp
  tmp = (double)TC_CH2.value * 0.25;           // convert fixed pt # to double
  Serial.print("TC Temp / Tcorr = ");                   // print TC temp heading
  if(0x00 == TC_CH1.status)
  {
    Serial.print(tmp);
    Serial.print("  /  ");
    Serial.println(TC_CH2.Tcorr, 4);
  }
  else if(0x01 == TC_CH2.status){Serial.println("OPEN");}
  else if(0x02 == TC_CH2.status){Serial.println("SHORT TO GND");}
  else if(0x04 == TC_CH2.status){Serial.println("SHORT TO Vcc");}
  else{Serial.println("unknown fault");}
  
  // TC3
  Serial.println("Thermocouple 3:");            // Print TC0 header
  // MAX31855 Internal Temp
  tmp = (double)TC_CH3.ref_jcn_temp * 0.0625;  // convert fixed pt # to double
  Serial.print("Tint = ");                      // print internal temp heading
  if((-100 > tmp) || (150 < tmp)){Serial.println("unknown fault");}
  else{Serial.println(tmp);}
  // MAX31855 External (thermocouple) Temp
  tmp = (double)TC_CH3.value * 0.25;           // convert fixed pt # to double
  Serial.print("TC Temp / Tcorr = ");                   // print TC temp heading
  if(0x00 == TC_CH1.status)
  {
    Serial.print(tmp);
    Serial.print("  /  ");
    Serial.println(TC_CH3.Tcorr, 4);
  }
  else if(0x01 == TC_CH3.status){Serial.println("OPEN");}
  else if(0x02 == TC_CH3.status){Serial.println("SHORT TO GND");}
  else if(0x04 == TC_CH3.status){Serial.println("SHORT TO Vcc");}
  else{Serial.println("unknown fault");}
}


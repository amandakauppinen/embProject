/*
 * PressureSens.cpp
 *
 *  Created on: 07.03.2019
 *      Author: Fabian Stölzle
 */

#include "PressureSens.h"



#define calPressure 966
#define regPressure  0xF1
#define devNumber 0x40


PressureSens::PressureSens(int AmbPressure) {
	ambPressure = AmbPressure;

	i2c_conf.clock_divider = 40;
//	i2c_conf.device_number = 0;
	i2c_conf.i2c_mode = IOCON_SFI2C_EN; //standard mode (IOCON_SFI2C_EN = 0)
	i2c_conf.speed = 50000; //any information?

	pressureRegister = regPressure;
	device_number = devNumber;


	i2c = new I2C(i2c_conf); //constructor does pinMuxing and enables i2c-pins...

}

PressureSens::~PressureSens() {
	delete i2c;
}

float PressureSens::getRawPressure(){
	i2c->transaction(device_number, &pressureRegister, 1, rxData, 3);
	uint16_t RawPressure = (rxData[0] << 8) + rxData[1];
	int16_t signedRawPressure = RawPressure;
	float pressure = (float)signedRawPressure/240;

	return pressure;
}

float PressureSens::getCompPressure(){
	float corFactor = (float)calPressure / (float)ambPressure;
	float CompPressure = getRawPressure() * corFactor;
	return CompPressure;
}

bool PressureSens::checkCRC(){
	//not implemented yet!!!
	//generator polynomial: x^8+x^5+x^4+x <=> 100110001
	return false;
}

/* example
static uint16_t crc16_update(uint16_t crc, uint8_t a)
{
  int i;

  crc ^= a;
  for (i = 0; i < 8; ++i)
  {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }

  return crc;
}
*/



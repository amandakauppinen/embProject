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
	ambPressure = AmbPressure;			//ambience pressure
										//(assuming the Altitude of operation is 0 meter)
	i2c_conf.clock_divider = 40;

	i2c_conf.i2c_mode = IOCON_SFI2C_EN; //standard mode (IOCON_SFI2C_EN = 0)
	i2c_conf.speed = 50000;

	pressureRegister = regPressure;
	device_number = devNumber;


	i2c = new I2C(i2c_conf); 			//constructor does pinMuxing and enables i2c-pins...

}

PressureSens::~PressureSens() {
	delete i2c;							//never actually called but won't cause mem leak
}

/*
 * Get reading from the sensor and
 * Convert to usable data
 * */
float PressureSens::getRawPressure(){
	//read 3 bytes from the sensor,
	i2c->transaction(device_number, &pressureRegister, 1, rxData, 3);

	//take first 2 byte and convert to uint16_t
	uint16_t RawPressure = (rxData[0] << 8) + rxData[1];

	//change to signed int
	int16_t signedRawPressure = RawPressure;

	//scale factor
	float pressure = (float)signedRawPressure/240;

	return pressure;
}

/*
 * Compensation for the altitude difference
 * Calculated using the formula from the documentation
 * DP(eff) = DP(sensor) *  (P(cal) / P (ambiance))
 * */
float PressureSens::getCompPressure(){
	float corFactor = (float)calPressure / (float)ambPressure;
	float CompPressure = getRawPressure() * corFactor;
	return CompPressure;
}

//CRC not required
bool PressureSens::checkCRC(){
	return false;
}


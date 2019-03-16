/*
 * PressureSens.h
 *
 *  Created on: 07.03.2019
 *      Author: Fabian Stölzle
 */
#include "I2C.h"

#ifndef PRESSURESENS_H_
#define PRESSURESENS_H_

class PressureSens {
public:
	PressureSens(int AmbPressure);
	virtual ~PressureSens();
	float getRawPressure();
	float getCompPressure();
	bool checkCRC();

private:
	int ambPressure;
	I2C* i2c;
	I2C_config i2c_conf;
	uint8_t rxData[3];
	uint8_t pressureRegister;
	uint8_t device_number;
	//how to find out the ambient Pressure:
	// find out height over sea level: https://www.daftlogic.com/sandbox-google-maps-find-altitude.htm
	// calculate local ambient air pressure: https://rechneronline.de/physik/luftdruck-hoehe.php
};

#endif /* PRESSURESENS_H_ */

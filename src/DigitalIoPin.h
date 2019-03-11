/*
 * DigitalIoPin.h
 *
 *  Created on: 22.01.2019
 *      Author: Fabian Stölzle
 */

//version 2

#ifndef DIGITALIOPIN_H_
#define DIGITALIOPIN_H_

class DigitalIoPin {
public:
	DigitalIoPin(int Port, int Pin, bool Input, bool Pullup, bool Invert);
	virtual ~DigitalIoPin();
	bool read();
	void write(bool value);
private:
	bool state;
	int port;
	int pin;
	bool input;
	bool pullup;
	bool invert;
};

#endif /* DIGITALIOPIN_H_ */

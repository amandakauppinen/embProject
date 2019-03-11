/*
 * DigitalIoPin.cpp
 *
 *  Created on: 22.01.2019
 *      Author: Fabian Stölzle
 */

#include "DigitalIoPin.h"
#include "board.h"



DigitalIoPin::DigitalIoPin(int Port, int Pin, bool Input , bool Pullup , bool Invert ) {
	port = Port;
	pin = Pin;
	input = Input;
	pullup = Pullup;
	invert = Invert;



	if (input){
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
		if(pullup){
			if(invert){
				Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN | IOCON_INV_EN));
			}
			else{
				Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN ));
			}
		}
		else{
			if(invert){
				Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, (IOCON_DIGMODE_EN | IOCON_INV_EN));
			}
			else{
				Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, (IOCON_DIGMODE_EN ));
			}
		}
	}
	else{
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
		Chip_GPIO_SetPinState(LPC_GPIO,port,pin,state);
	}
}

DigitalIoPin::~DigitalIoPin() {
	// TODO Auto-generated destructor stub
}

bool DigitalIoPin::read(){
	if(input){
		state=Chip_GPIO_GetPinState(LPC_GPIO,port,pin);
	}
	return state;
}


void DigitalIoPin::write(bool value){
	if(!input){
		state= value ^ invert;
		Chip_GPIO_SetPinState(LPC_GPIO,port,pin,state);
	}
}


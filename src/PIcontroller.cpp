/*
 * PIcontroller.cpp
 *
 *  Created on: 07.03.2019
 *      Author: Fabian Stölzle
 */

#include "PIcontroller.h"

PIcontroller::PIcontroller(float KP, float KI, float k, int SysTickRate, int SysTickDivider) {
	Ki = KI;
	Kp = KP;
	K = k;
	i = 0;
	p = 0;
	sysTickRate = SysTickRate;
	sysTickDivider = SysTickDivider;
	currentPressure = 0;
	targetPressure = 0;
	deltaPressure = 0;
	speed = 0;
}

PIcontroller::~PIcontroller() {
	// TODO Auto-generated destructor stub
}

void PIcontroller::setReading(float CurrentPressure){
	currentPressure = CurrentPressure;
	deltaPressure = currentPressure - targetPressure;
}

void PIcontroller::setTarget(float TargetPressure){
	targetPressure = TargetPressure;
}

void PIcontroller::sysTick(){
	static int counter = 0;
	if (counter >= sysTickDivider){
		i += deltaPressure;
		counter = 0;
	}
	else{
		counter++;
	}


	//ToDo set limit for i!!!!
}

int PIcontroller::getSpeed(){
	p = deltaPressure;
	speed = (p *(-Kp) + i * (-Ki)) * K;
	return (int)speed;
}



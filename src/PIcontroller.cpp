/*
 * PIcontroller.cpp
 *
 *  Created on: 07.03.2019
 *      Author: Fabian Stölzle
 */

#include "PIcontroller.h"

#define maxValue 20000
#define minValue 1300
#define timeOut_s 20
#define timeOutRange 0.5

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
	//limit i
	if (getUnlimitedSpeed() > maxValue){ //
		i =((maxValue/K)-(p*(-Kp))) / (-Ki);
	}
	else if (getUnlimitedSpeed() < minValue){
		i =((minValue/K)-(p*(-Kp))) / (-Ki);
	}

	if(deltaPressure < timeOutRange && deltaPressure > -timeOutRange){
		timeOutCounter = 0;
	}
}

void PIcontroller::setTarget(float TargetPressure){
	targetPressure = TargetPressure;
	timeOutCounter = 0;
}

void PIcontroller::sysTick(){
	timeOutCounter++;
	static volatile int dividerCounter = 0;
	if (dividerCounter >= sysTickDivider){
		i += deltaPressure;
		dividerCounter = 0;
	}
	else{
		dividerCounter++;
	}

}

float PIcontroller::getUnlimitedSpeed(){
	p = deltaPressure;
	speed = (p *(-Kp) + i * (-Ki)) * K;

	return speed;
}

int PIcontroller::getSpeed(){
	float speed = getUnlimitedSpeed();
	if (speed > maxValue){
		speed = maxValue;
	}
	else if (speed < minValue){
		speed = minValue;
	}
	return (int)speed;
}

bool PIcontroller::getTimeOut(){
	if (timeOutCounter >= (sysTickRate * timeOut_s)){
		return true;
	}
	return false;
}

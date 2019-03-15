/*
 * PIcontroller.cpp
 *
 *  Created on: 07.03.2019
 *      Author: Fabian Stölzle
 */

#include "PIcontroller.h"

#define maxValue 20000	//max output value of the getSpeed() function
#define minValue 1300	//max output value of the getSpeed() function
#define timeOut_s 60	//seconds to try to reach the target value before the error-message
#define timeOutRange 1	//the timeOutCounter gets reseted if the current pressure is in a range +-timeOutRange around the target value

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

//update the sensorvalue to the PIcontroller.
void PIcontroller::setReading(float CurrentPressure){
	currentPressure = CurrentPressure;
	//calculate the difference between current and target pressure
	deltaPressure = currentPressure - targetPressure;

	//limit the i value. if the controller cant reach
	//the target value the i value would keep increasing very high.
	if (getUnlimitedSpeed() > maxValue){
		i =((maxValue/K)-(p*(-Kp))) / (-Ki);
	}
	else if (getUnlimitedSpeed() < minValue){
		i =((minValue/K)-(p*(-Kp))) / (-Ki);
	}
	//reset timeOutCounter if the current pressure is in the Range
	if(deltaPressure < timeOutRange && deltaPressure > -timeOutRange){
		timeOutCounter = 0;
	}
}

//set the target pressure for the PIcontroller
void PIcontroller::setTarget(float TargetPressure){
	//reset the timeOutCounter if the target pressure get changed
	if (targetPressure != TargetPressure){
		timeOutCounter = 0;
	}
	targetPressure = TargetPressure;
}

//call this function in the SysTickTimer at a high frequency (at SysTickRate)
void PIcontroller::sysTick(){
	timeOutCounter++;
	static volatile int dividerCounter = 0;
	//a tickdevider is used:
	if (dividerCounter >= sysTickDivider){
		i += deltaPressure;
		dividerCounter = 0;
	}
	else{
		dividerCounter++;
	}

}

//get the raw output value of the PIcontroller
float PIcontroller::getUnlimitedSpeed(){
	p = deltaPressure;
	speed = (p *(-Kp) + i * (-Ki)) * K;

	return speed;
}

//get the limited output value. in this case the frequency
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

//check if a timeout occured
bool PIcontroller::getTimeOut(){
	if (timeOutCounter >= (sysTickRate * timeOut_s)){
		return true;
	}
	return false;
}

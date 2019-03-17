/*
 * PIcontroller.h
 *
 *  Created on: 07.03.2019
 *      Author: Fabian Stölzle
 */

#ifndef PICONTROLLER_H_
#define PICONTROLLER_H_
#include <sys/types.h>

class PIcontroller {
public:
	PIcontroller(float KP, float KI, float k, unsigned int SysTickRate, int SysTickDivider);
	virtual ~PIcontroller();
	void setReading(float CurrentPressure);
	void setTarget(float TargetPressure);
	int getSpeed();
	void sysTick();
	bool getTimeOut();
private:
	float getUnlimitedSpeed();
	unsigned int sysTickRate;
	int sysTickDivider;
	float Ki;
	float Kp;
	float K;
	volatile float i;
	float p;
	float currentPressure;
	float targetPressure;
	float deltaPressure;
	float speed;
	volatile uint32_t timeOutCounter;
};

#endif /* PICONTROLLER_H_ */

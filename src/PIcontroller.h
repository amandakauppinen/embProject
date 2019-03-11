/*
 * PIcontroller.h
 *
 *  Created on: 07.03.2019
 *      Author: Fabian Stölzle
 */

#ifndef PICONTROLLER_H_
#define PICONTROLLER_H_

class PIcontroller {
public:
	PIcontroller(float KP, float KI, float k, int SysTickRate, int SysTickDivider);
	virtual ~PIcontroller();
	void setReading(float CurrentPressure);
	void setTarget(float TargetPressure);
	int getSpeed();
	void sysTick();
private:
	int sysTickRate;
	int sysTickDivider;
	float Ki;
	float Kp;
	float K;
	float i;
	float p;
	float currentPressure;
	float targetPressure;
	float deltaPressure;
	float speed;
};

#endif /* PICONTROLLER_H_ */

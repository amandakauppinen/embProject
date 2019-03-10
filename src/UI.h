#ifndef UI_h
#define UI_h

#include "DigitalIoPin.h"
#include "LiquidCrystal.h"

class UI {
public:
	UI(DigitalIoPin *b1, DigitalIoPin *b2, DigitalIoPin *b3);
	void clearDisplay();
	void buttonStatus();
	void defaultDisplay();
	void displayFormat (bool manual);
	void buttonStatus();
	void setFan(int fanSpeed);
	void setPressure(int pressure);
	void valueUpdate(bool mode);
	bool errorCheck();
	int getFan();
	int getPressure();
	virtual ~UI();
private:
	DigitalIoPin *b1;
	DigitalIoPin *b2;
	DigitalIoPin *b3;
	
	//true = manual, false = auto
	bool modeSelect = true, error = false;
	int pressure = 0, fanSpeed = 0;
};

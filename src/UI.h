#ifndef UI_h
#define UI_h

#include "DigitalIoPin.h"
#include "LiquidCrystal.h"
#include <string>

class UI {
public:
	UI(LiquidCrystal *screen, DigitalIoPin *b1, DigitalIoPin *b2, DigitalIoPin *b3);
	void clearDisplay();
	void buttonStatus();
	void displayFormat (bool mode);
	void setFan(int fanSpeed);
	void setPressure(int pressure);
	void valueUpdate (bool mode, int value);
	void errorMessage();
	int getFan();
	int getPressure();
	virtual ~UI();
private:
	LiquidCrystal *screen;
	DigitalIoPin *b1;
	DigitalIoPin *b2;
	DigitalIoPin *b3;
	
	//true = manual, false = auto
	bool modeSelect = true, error = false;
	int pressure = 0, fanSpeed = 0;
};

#endif

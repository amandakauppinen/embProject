#ifndef UI_h

#define UI_h


#include "LiquidCrystal.h"

#include <string>


class UI {

public:
	
	UI(LiquidCrystal *screen, DigitalIoPin *b1, DigitalIoPin *b2, DigitalIoPin *b3);
	void clearDisplay();
	void buttonStatus();
	void setFan(int fanSpeed);
	void setPressure(int pressure);
	void errorMessage();
	int getFan();
	int getPressure();
	bool inline getMode() {return manual;};
	virtual ~UI();
private:
	LiquidCrystal *screen;
	DigitalIoPin *b1;
	DigitalIoPin *b2;
	DigitalIoPin *b3;
	//true = manual, false = auto
	bool manual = true, error = false;
	int pressure = 0, fanSpeed = 0;

	void valueUpdate ();
	void displayFormat ();

};
#endif

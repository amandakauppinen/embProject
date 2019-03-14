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

	//void setFan(int newSpeed);
	void setPressure(int newPressure);
	void inline setCurrentPressure (int pa) {
		currentPressure = pa;
	}

	void errorMessage();

	int getFan();
	int getPressure();
	bool inline Mode() {
		return manualMode;
	}
	virtual ~UI();
private:
	LiquidCrystal *screen;
	DigitalIoPin *b1;
	DigitalIoPin *b2;
	DigitalIoPin *b3;
	
	//true = manual, false = auto
	bool manualMode = true;
	int pressure = 90, fanSpeed = 10;
	int currentPressure;

	void displayFormat ();
};

#endif

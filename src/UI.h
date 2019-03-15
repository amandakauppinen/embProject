#ifndef UI_h
#define UI_h

#include "LiquidCrystal.h"
#include <string>

class UI {
public:
	UI(LiquidCrystal *screen, DigitalIoPin *b1, DigitalIoPin *b2, DigitalIoPin *b3);
	void clearDisplay();
	void buttonStatus();

	//void setFan(int newSpeed);
	void setPressure(int newPressure);
	void setCurrentPressure (int pa);

	void write(char* c);
	void write(std::string s);

	void errorMessage();
	void resetError();

	int getFan();
	int getPressure();
	//returns true if the program is in manual mode
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
	int pressure = 90, fanSpeed = 25;
	int currentPressure;
	bool error = false;
	void displayFormat ();
};

#endif

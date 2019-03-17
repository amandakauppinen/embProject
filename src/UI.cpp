#include "UI.h"

UI::UI(LiquidCrystal *screen, DigitalIoPin *b1, DigitalIoPin *b2, DigitalIoPin *b3) {
	this->screen = screen;
	this->b1 = b1; //+
	this->b2 = b2; //-
	this->b3 = b3; //Select
	//sets it to automatically start in manual mode
	screen->clear();
}

void UI::clearDisplay() {
	screen->clear();
}

void UI::write(char* c) {
	screen->setCursor(0,0);
	screen->print(c);
}

void UI::write(std::string s) {
	this->write(s.c_str());
}

void UI::buttonStatus() {
	if (!error) displayFormat();
	if (b1->read()) {
		//if the device is in manual mode
		if (manualMode == true) {
			if (fanSpeed <= 95) {
				fanSpeed += 5;
			}
			else {
				fanSpeed = 100;
			}
		}
		//if the device is in auto mode
		else if (manualMode == false) {
			if (pressure <= 115) {
				pressure += 5;
			}
			else {
				pressure = 120;
			}
		}
		while(b1->read()) {}
	}
	else if (b2->read()) {
		if (manualMode == true) {
			if (fanSpeed >= 5) {
				fanSpeed -= 5;
			}
			else {
				fanSpeed = 0;
			}
		}
		else if (manualMode == false) {
			if (pressure >=5) {
				pressure -= 5;
			}
			else {
				pressure = 0;
			}
		}
		while(b2->read()) {}
	}
	//toggles mode
	else if (b3->read()) {
		screen->clear();
		if (manualMode == true) {
			manualMode = false;
		}
		else if (manualMode == false){
			manualMode = true;
		}
		while(b3->read()) {}
	}
}

void UI::displayFormat () {
	char buffer[16] = {'\0'};

	//change lcd to manual layout
	if (manualMode == true) {
		//line 0
		screen->setCursor(0,0);
		snprintf ( buffer, 16, "Pressure: %d   ", currentPressure);
		screen->print(buffer);
		screen->setCursor(14,0);
		screen->print("pa");
		buffer[0] = {'\0'};


		//line 1
		screen->setCursor(0,1);
		snprintf(buffer, 16, "Set speed: %d  ", fanSpeed);
		screen->print(buffer);
		screen->setCursor(15,1);
		screen->print("%");
		buffer[0] = {'\0'};
	}
	//change device to auto layout
	else {
		//line 0
		if(!error){
			screen->setCursor(0,0);
			snprintf(buffer, 16, "Pressure: %d  ", currentPressure);
			screen->print(buffer);
			screen->setCursor(14,0);
			screen->print("pa");
			buffer[0] = {'\0'};
		}
		//line 1
		screen->setCursor(0,1);
		snprintf(buffer, 16, "Set pr: %d  ", pressure);
		screen->print(buffer);
		screen->setCursor(12,1);
		screen->print("pa");
		buffer[0] = {'\0'};
	}
}

void UI::setPressure(int newPressure) {
	this->pressure = newPressure;
}

//updates UI with current pressure from main
void UI::setCurrentPressure (int pa) {
	currentPressure = pa;
	this->displayFormat();
}

//when called in main, returns fan and pressure set by user
int UI::getFan() {
	return (fanSpeed*200);
}
int UI::getPressure() {
	return pressure;
}

//called in main when pressure is not reached on time
void UI::errorMessage() {
	error = true;
	screen->setCursor(0,0);
	screen->print("Error code: 1   ");
}

//used to keep the error message on the screen until pressure
//is modified or reached
void UI::resetError() {
	error = false;
}

UI::~UI() {}

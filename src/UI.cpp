#include "UI.h"


/*
 * MANUAL = TRUE		AUTO = FALSE
 */

UI::UI(LiquidCrystal *screen, DigitalIoPin *b1, DigitalIoPin *b2, DigitalIoPin *b3) {
	this->screen = screen;
	this->b1 = b1; //+
	this->b2 = b2; //-
	this->b3 = b3; //Select
	//sets it to automatically start in manual mode
	displayFormat();
}

void UI::clearDisplay() {
	screen->clear();
}

void UI::buttonStatus() {
	//this might be better going into the main
	if (b1->read()) {
		//if the device is in manual mode
		if (manualMode == true) {
			if (fanSpeed <= 95) {
				fanSpeed += 5;
			}
			else {
				fanSpeed = 100;
			}
			//update fan speed, send it to lcd
			//Update the LCD readings
			displayFormat();
		}
		//if the device is in auto mode
		else if (manualMode == false) {
			if (pressure <= 115) {
				pressure += 5;
			}
			else {
				pressure = 120;
			}
			//update pressure value, send it to lcd

			//Update the LCD readings
			displayFormat();
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
			//Update the LCD
			displayFormat();
		}
		else if (manualMode == false) {
			if (pressure >=5) {
				pressure -= 5;
			}
			else {
				pressure = 0;
			}
			//Update the LCD
			displayFormat();
		}
		while(b2->read()) {}
	}
	else if (b3->read()) {
		screen->clear();
		if (manualMode == true) {
			manualMode = false;
			displayFormat();
		}
		else if (manualMode == false){
			manualMode = true;
			displayFormat();
		}
		while(b3->read()) {}
	}
}


/*This section is a mess right now until I can figure out the problem with those random characters
 * it works, but it's too much code and messy*/

void UI::displayFormat () {
	char buffer[16] = {'\0'};
	int cx;

	//change lcd to manual layout
	if (manualMode == true) {
		//line 0
		screen->setCursor(0,0);
		cx = snprintf ( buffer, 16, "Pressure: %d", pressure);
		screen->print(buffer);
		screen->setCursor(14,0);
		screen->print("pa");
		buffer[0] = {'\0'};


		//line 1
		screen->setCursor(0,1);
		cx = snprintf(buffer, 16, "Set speed: %d", fanSpeed);
		screen->print(buffer);
		screen->setCursor(15,1);
		screen->print("%");
		buffer['\0'];
	}
	//change device to auto layout
	else {
		//line 0
		screen->setCursor(0,0);
		cx = snprintf(buffer, 16, "Pressure: %d", currentPressure);
		screen->print(buffer);
		screen->setCursor(14,0);
		screen->print("pa");
		buffer['\0'];
		//line 1
		screen->setCursor(0,1);
		cx = snprintf(buffer, 16, "Set pr: %d", fanSpeed);
		screen->print(buffer);
		screen->setCursor(12,1);
		screen->print("pa");
		buffer['\0'];
	}
}


//set fan and pressure speeds
//void UI::setFan(int newSpeed){
//	this->fanSpeed = newSpeed;
//	displayFormat();
//}
void UI::setPressure(int newPressure) {
	this->pressure = newPressure;
	displayFormat();
}


//gets fan and pressure speeds to be displayed
//needs to be updated to match rest of program
int UI::getFan() {
	return (fanSpeed*200);
}
int UI::getPressure() {
	return pressure;
}

//leaving this alone for now, we can call it from wherever we are checking the error
void UI::errorMessage() {
	screen->setCursor(0,0);
	screen->print("                ");
	screen->setCursor(0,0);
	screen->print("Error code: 123");
}

UI::~UI() {}

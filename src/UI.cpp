#include "UI.h"

/*
 * MANUAL = TRUE		AUTO = FALSE
 */

UI::UI(LiquidCrystal *screen, DigitalIoPin *b1, DigitalIoPin *b2, DigitalIoPin *b3) {
	this->screen = screen;
	this->b1 = b1;
	this->b2 = b2;
	this->b3 = b3;
	manual = false;
	//sets it to automatically start in manual mode
	displayFormat();
}

void UI::clearDisplay() {
	screen->clear();
}

void UI::buttonStatus() {
	//update values of pressure and fan speed every time button status is checked
//	getPressure();
//	getFan();

	//this might be better going into the main
//	while(1) {
		if (b1->read()) {
//			Sleep(500);
			//if the device is in manual mode
			if (manual) {
				if (fanSpeed <= 95) {
					fanSpeed += 5;
				}
				else {
					fanSpeed = 100;
				}
				//update fan speed, send it to lcd
//				setFan(fanSpeed);
				valueUpdate();
			}
			//if the device is in auto mode
			else if (!manual) {
				if (pressure <= 115) {
					pressure += 5;
				}
				else {
					pressure = 120;
				}
				//update pressure value, send it to lcd
//				setPressure(pressure);
				valueUpdate();
			}
		}
		else if (b2->read()) {
//			Sleep(500);
			if (manual == true) {
				if (fanSpeed >= 5) {
					fanSpeed -= 5;
				}
				else {
					fanSpeed = 0;
				}
//				setFan(fanSpeed);
				valueUpdate();
			}
			else if (manual == false) {
				if (pressure >=5) {
					pressure -= 5;
				}
				else {
					pressure = 0;
				}
//				setPressure(pressure);
				valueUpdate();
			}
		}
		else if (b3->read()) {
//			Sleep(500);
			//I can make this look nicer later
			//if device is in manual mode, change to auto
			if (manual == true) {
				manual = false;
			}
			else if (manual == false){
				manual = true;
			}
			displayFormat();
		}
//	}
}



/*This section is a mess right now until I can figure out the problem with those random characters
 * it works, but it's too much code and messy*/

void UI::displayFormat () {
	//get most recent pressure and speed values
//	getPressure();
//	getFan();

	//clear display to start new
	clearDisplay();

	//change lcd to manual layout
	if (manual) {
		//line 0
		screen->setCursor(0,0);
		screen->print("Pressure: ");
		screen->print(" ");
		screen->setCursor(10,0);
		screen->print(std::to_string(pressure));

		//This is the part to take care of random characters
		if (pressure >= 100) {
			screen->setCursor(13,0);
		}
		else if (pressure < 10) {
			screen->setCursor(11,0);
			screen->print(" ");
		}
		else {
			screen->setCursor(12,0);
		}
		screen->print(" ");


		screen->setCursor(14,0);
		screen->print("pa");

		//line 1
		screen->setCursor(0,1);
		screen->print("Set speed: ");
		screen->setCursor(11,1);
		screen->print(std::to_string(fanSpeed));

		//This is the part to take care of random characters
		if (fanSpeed >= 100) {
			screen->setCursor(14,1);
		}
		else if (fanSpeed < 10) {
			screen->setCursor(12,1);
			screen->print(" ");
		}
		else {
			screen->setCursor(13,1);
		}
		screen->print(" ");


		screen->setCursor(15,1);
		screen->print("%");
	}
	//change device to auto layout
	else {
		//line 1
		screen->setCursor(0,1);
		screen->print("Set pr: ");
		screen->setCursor(8,1);
		screen->print(std::to_string(pressure));

		//This is the part to take care of random characters
		if (pressure >= 100) {
			screen->setCursor(11,1);
		}
		else if (pressure < 10) {
			screen->setCursor(9,1);
			screen->print(" ");
		}
		else {
			screen->setCursor(10,1);
		}
		screen->print(" ");


		screen->setCursor(12,1);
		screen->print("pa");
	}
}

//updates values on lcd as they're changed
void UI::valueUpdate () {
	//if in manual mode
	if (manual) {
		//I don't know if this is necessary to keep
//		getPressure();
		screen->setCursor(10,0);
		screen->print(std::to_string(pressure));

		screen->setCursor(11,1);
		screen->print(std::to_string(pressure));

		//This is the part to take care of random characters
		if (pressure >= 100) {
			screen->setCursor(14,1);
		}
		else if (pressure < 10) {
			screen->setCursor(12,1);
			screen->print(" ");
		}
		else {
			screen->setCursor(13,1);
		}
		screen->print(" ");
	}
	else {
		screen->setCursor(8,1);

		//This is the part to take care of random characters
		screen->print(std::to_string(pressure));
		if (pressure >= 100) {
			screen->setCursor(11,1);
		}
		else if (pressure < 10) {
			screen->setCursor(9,1);
			screen->print(" ");
		}
		else {
			screen->setCursor(10,1);
		}
		screen->print(" ");
	}
}

//set fan and pressure speeds
/*we need to link these to the main program, I'll also try to add in a way
to check if the value being sent is already set to be that, in which case
it will not have to do anything different*/
void UI::setFan(int fanSpeed){
	this->fanSpeed = fanSpeed;
}
void UI::setPressure(int pressure) {
	this->pressure = pressure;
}


//gets fan and pressure speeds to be displayed
//needs to be updated to match rest of program
int UI::getFan() {
	//fanSpeed = 100;
	return fanSpeed;
}
int UI::getPressure() {
	//pressure = 100;
	return pressure;
}

//leaving this alone for now, we can call it from wherever we are checking the error
void UI::errorMessage() {
	screen->clear();
	screen->setCursor(0,0);
	screen->print("Error code: 123");
}

UI::~UI() {}

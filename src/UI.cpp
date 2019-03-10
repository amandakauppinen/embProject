#include "UI.h"


UI::UI(DigitalIoPin &b1, DigitalIoPin &b2, DigitalIoPin &b3) {
	lcd.begin(16,2);
	this.b1 = b1;
	this.b2 = b2;
	this.b3 = b3;
	//sets it to automatically start in manual mode
	displayFormat(modeSelect);
}

void UI::clearDisplay() {
	lcd.clear();
}

void UI::buttonStatus() {
	//update values of pressure and fan speed every time button status is checked
	getPressure();
	getFan();

	if (b1.read()) {
		//if the device is in manual mode
		if (modeSelect == true) {
			if (pressure <= 115) {
				pressure += 5;
			}
			else {
				pressure = 120;
			}
			//update pressure value, send it to lcd
			setPressure(pressure);
			valueUpdate(modeSelect);
		}
		//if the device is in auto mode
		else if (modeSelect == false) {
			if (fanSpeed <= 95) {
				fanSpeed += 5;
			}
			else {
				fanSpeed = 100;
			}
			//update fan speed, send it to lcd
			setFan(fanSpeed);
			valueUpdate(modeSelect);
		}
	}
	else if (b2.read()) {
		if (modeSelect == true) {
			if (pressure >=5) {
				pressure -= 5;
			}
			else {
				pressure = 0;
			}
			setPressure(pressure);
			valueUpdate(modeSelect);
		}
		else if (modeSelect == false) {
			if (fanSpeed >= 5) {
				fanSpeed -= 5;
			}
			else {
				fanSpeed = 0;
			}
			setFan(fanSpeed);
			valueUpdate(modeSelect);
		}
	}
	else if (b3.read()) {
		//if device is in manual mode, change to auto
		if (modeSelect == true) {
			modeSelect = false;
			displayFormat(modeSelect);
		}
		else {
			modeSelect = true;
			displayFormat(modeSelect);
		}
	}
	
}

void UI::displayFormat (bool mode) {
	//get most recent pressure and speed values
	getPressure();
	getFan();
	//clear display to start new
	clearDisplay();
	//change lcd to manual layout
	if (mode == true) {
		//line 0
		lcd.setCursor(0,0);
		lcd.print("Pressure: ");
		lcd.setCursor(10,0);
		lcd.print(pressure);
		lcd.setCursor(13,0);
		lcd.print("pa");

		//line 1
		lcd.setCursor(0,1);
		lcd.print("Set speed: ");
		lcd.setCursor(11,1);
		lcd.print(fanSpeed);
		lcd.setCursor(14,1);
		lcd.print('%');
	}
	//change device to auto layout
	else {
		//line 1
		lcd.setCursor(0,1);
		lcd.print("Set pr: ");
		lcd.setCursor(8,1);
		lcd.print(pressure);
		lcd.setCursor(11,1);
		lcd.print("pa");
	}
}

//updates values on lcd as they're changed
//needs improvement-doesn't account for 2 to 3 digit conversion
void UI::valueUpdate(bool mode, int value) {
	//if in manual mode
	if (mode == true) {
		getPressure();
		lcd.setCursor(10,0);
		lcd.print(pressure);
		lcd.setCursor(11,1);
		lcd.print(value);
	}
	else {
		lcd.setCursor(8,1);
		lcd.print(pressure);
	}
}

//set fan and pressure speeds
void UI::setFan(int fanSpeed){
	this.fanSpeed = fanSpeed;
}
void UI::setPressure(int pressure) {
	this.pressure = pressure;
}

//gets fan and pressure speeds to be displayed
//needs to be updated to match rest of program
int UI::getFan() {
	return fanSpeed;
}
int UI::getPressure() {
	return pressure;
}

//leaving this alone for now
bool UI::errorCheck() {
	//not sure how to code this, but will return true if there is an error and false if not
	error = true;
}

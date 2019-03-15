/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition test
===============================================================================
*/

//includes:
#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

//basic
#include <cr_section_macros.h>
#include <atomic>
//PressureSensor:
#include "PressureSens.h"
//PI controller:
#include "PIcontroller.h"
//UI:
#include "UI.h"
#include <ritimer_15xx.h> //do we need this here?
#include "LiquidCrystal.h"
//Modbus:
#include "ModbusMaster.h"
#include "ModbusRegister.h"
#include "LpcUart.h"

//defines:
#define SYSTICKRATE_HZ 1000
#define MODBUSINTERVALL_MS 1000
#define MAININTERVALL_MS 30 //needed because the sensor needs some time to get a new value

//global variables:
static volatile std::atomic_int sleepCounter;
static volatile uint32_t millisCounter;
static volatile std::atomic_int ModbusDelayCounter;
static PIcontroller* PIC;

//SysTickHandler in C-code:
#ifdef __cplusplus
extern "C" {
#endif

//* @brief Handle interrupt from SysTick timer
//* @return Nothing

void SysTick_Handler(void)	{
	sleepCounter++; 		//decrement counter for Sleep-function
	ModbusDelayCounter ++; 	//increment counter for delay between Modbus updates
	if (PIC != NULL) PIC->sysTick();
	millisCounter++;
}

#ifdef __cplusplus
}
#endif

void Sleep(uint ms) {
	sleepCounter = 0;
	int threshold = (ms * SYSTICKRATE_HZ) / 1000;
	while(sleepCounter < threshold){
		__WFI();
	}
}

uint32_t millis() {
	return millisCounter;
}

bool setFrequency(ModbusMaster& node, uint16_t freq){
	int result;
	int ctr;
	bool atSetpoint;
	const int delay = 100;

	ModbusRegister Frequency(&node, 1); // reference 1
	ModbusRegister StatusWord(&node, 3);
	//necessary in this function?
	Frequency = freq; // set motor frequency

	// wait until we reach set point or timeout occurs ? former delay?
	ctr = 0;
	atSetpoint = false;
	do {
		// read status word
		result = StatusWord;
		// check if we are at setpoint
		if (result >= 0 && (result & 0x0100)) atSetpoint = true;
		ctr++;
		Sleep(delay);
	} while(ctr < 20 && !atSetpoint);

	return atSetpoint;
}


int main(void) {
	/*-------------------------------------------------------------------------*/
	/*----------------------------------Set up---------------------------------*/
	/*-------------------------------------------------------------------------*/
	//General:
	SystemCoreClockUpdate();
	Board_Init();

	//SysTickTimer:
	uint32_t sysTickClockRate;
	Chip_Clock_SetSysTickClockDiv(1);
	sysTickClockRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickClockRate / SYSTICKRATE_HZ);

	//retarget printf:
	Chip_SWM_MovablePortPinAssign(SWM_SWO_O, 1, 2);
	printf("Started\n");

	//Modbus:
	LpcPinMap none = {-1, -1}; // unused pin has negative values in it
	LpcPinMap txpin = { 0, 18 }; // transmit pin that goes to debugger's UART->USB converter
	LpcPinMap rxpin = { 0, 13 }; // receive pin that goes to debugger's UART->USB converter
	LpcUartConfig cfg = { LPC_USART0, 115200, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, false, txpin, rxpin, none, none };
	LpcUart dbgu(cfg);

	ModbusMaster node(2); // Create Modbus object that connects to slave id 2
	node.begin(9600);	//Start Uart
	ModbusRegister ControlWord(&node, 0);
	ModbusRegister outputFreq(&node,102);
	ModbusRegister StatusWord(&node, 3);

	ControlWord = 0x0406; // prepare for starting
	ControlWord = 0x047F; // set drive to start mode

	//PIcontroller:
	PIcontroller picontroller(1, 0.2, 10, 1000, 100); //1, 0.3, 40, 1000, 10
	PIC = &picontroller;
	picontroller.setTarget(0);

	//PressureSensor:
	PressureSens ps(1000);

	//UI:
	Chip_RIT_Init(LPC_RITIMER); 				//setup RI-Timer for milliseconds delay function for LCD

	DigitalIoPin rs(0, 8, false, false, false); //RS //pins for LCD
	DigitalIoPin en(1, 6, false, false, false); //EN
	DigitalIoPin d4(1, 8, false, false, false); //D4
	DigitalIoPin d5(0, 5, false, false, false); //D5
	DigitalIoPin d6(0, 6, false, false, false); //D6
	DigitalIoPin d7(0, 7, false, false, false); //D7

	DigitalIoPin b1(1, 3, true, true, true); 		// + button
	DigitalIoPin b2(0, 9, true, true, true);		// - button
	DigitalIoPin b3(0, 10,true, true, true);		// switch button

	LiquidCrystal lcd(&rs, &en, &d4, &d5, &d6, &d7); //LCD-setup
	lcd.begin(16, 2);
	lcd.clear();

	UI menu(&lcd, &b1, &b2, &b3); 	//Hook up LCD and buttons.
									//LCD defaulted to manual mode

	//Initial config to get a stable pressure inside the tube first
	setFrequency(node, 5000);
	menu.write("Initializing.   ");
	Sleep(1000);
	setFrequency(node, 10000);
	menu.write("Initializing..  ");
	Sleep(1000);
	setFrequency(node, 5000);
	menu.write("Initializing... ");
	Sleep(1000);
	menu.clearDisplay();

	//main-function variables:
	int freq = 0, status;
	float pressure = 0;
	const int ModbusDelayThreshold = (MODBUSINTERVALL_MS * SYSTICKRATE_HZ) / 1000; //threshold for the Modbus delay

	/*-------------------------------------------------------------------------*/
	/*------------------------------Working loop-------------------------------*/
	/*-------------------------------------------------------------------------*/
	while (1) {
	/*-------------------------------------------------------------------------*/
	/*-----------------------Set up menu and calculations----------------------*/
	/*-------------------------------------------------------------------------*/
		//Polling button for inputs
		menu.buttonStatus();

		//Get pressure readings from the
		pressure = ps.getCompPressure();

		if(menu.Mode()){								//if the system is in manual mode
			picontroller.setTarget(0);					//PIcontroller not in use
			picontroller.setReading(0);					//PIcontroller not in use
			freq = menu.getFan();						//Set speed to be sent to the fan to be
														//the UI readings
		}
		else {											//if the system is in automatic mode
			picontroller.setTarget(menu.getPressure());	//if we switch back to automatic mode the motor starts from 0
			picontroller.setReading(pressure);			//PIcontroller setReading(0)
			freq = picontroller.getSpeed();				//calculate the speed from the PIcontroller

		}
		/*-------------------------------------------------------------------------*/
		/*------------------------Modbus communication-----------------------------*/
		/*-------------------------------------------------------------------------*/
		//only try to communicate through the modbus once every second
		if (ModbusDelayCounter >= ModbusDelayThreshold){
			Board_LED_Set(0, true);
			Board_LED_Set(1, false);
			ModbusDelayCounter = 0; 					//reset counter;
			menu.setCurrentPressure((int)(pressure+0.5));
			if(!menu.Mode()) {							//if the system is running in automatic mode
				if(picontroller.getTimeOut()){
					menu.errorMessage();
					printf("PIcontroller Error: not able to reach pressure! \n");
				}
				else menu.resetError();
			}
			else {										//if the system is running in manual mode
				//no error checking for manual mode
			}
			status = StatusWord;
			if (status > 0 && (status & 0x100)){
				setFrequency(node,freq);
			}
			printf("pressure= %f.2 freq= %d \n", pressure, freq); //maybe target as well?

		}
		/*-------------------------------------------------------------------------*/
		//Wait for the sensor to read values
		Board_LED_Set(1, true);
		Board_LED_Set(0, false);
		Sleep(MAININTERVALL_MS);
	}

    return 1;
}

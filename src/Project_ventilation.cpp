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

#include <cr_section_macros.h>
#include <ritimer_15xx.h>
#include "LiquidCrystal.h"
#include <atomic>
#include "UI.h"
#include "PressureSens.h"
#include "PIcontroller.h"
#include "DigitalIoPin.h"

//defines:
#define SYSTICKRATE_HZ 1000
#define MODBUSINTERVALL_MS 1000
#define MAININTERVALL_MS 30 //needed because the sensor needs some time to get a new value

//global variables:
static volatile std::atomic_int sleepCounter;
static volatile std::atomic_int ModbusDelayCounter;

//SysTickHandler in C-code:
#ifdef __cplusplus
extern "C" {
#endif

//* @brief Handle interrupt from SysTick timer
//* @return Nothing

	void SysTick_Handler(void)
	{
		sleepCounter--; //decrement counter for Sleep-function
		ModbusDelayCounter ++; //increment counter for delay between Modbus updates
		//call PIcontroller-Tick
		//Stuff for millis()
	}

#ifdef __cplusplus
}
#endif

void Sleep(uint ms){
	sleepCounter = 0;
	int threshold = (ms * SYSTICKRATE_HZ) /1000;
	while(sleepCounter < threshold){
		__WFI();
	}
}


int main(void) {

//Setup
	//general:
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

	//PIcontroller:

	//PressureSensor:

	//UI:
	Chip_RIT_Init(LPC_RITIMER); //setup RI-Timer for milliseconds delay function for LCD

	DigitalIoPin rs(0, 8, false, false, false); //RS //pins for LCD
	DigitalIoPin en(1, 6, false, false, false); //EN
	DigitalIoPin d4(1, 8, false, false, false); //D4
	DigitalIoPin d5(0, 5, false, false, false); //D5
	DigitalIoPin d6(0, 6, false, false, false); //D6
	DigitalIoPin d7(0, 7, false, false, false); //D7

	DigitalIoPin b1(1,3,true, true, true); //buttons
	DigitalIoPin b2(0,9,true, true, true);
	DigitalIoPin b3(0,10,true, true, true);

	LiquidCrystal lcd(&rs, &en, &d4, &d5, &d6, &d7); //LCD-setup
	lcd.begin(16, 2);
	lcd.clear();

	UI menu(&lcd, &b1, &b2, &b3);

	//main-function variables:
	int freq = 0;
	float pressure = 0;
	const int ModbusDelayThreshold = (MODBUSINTERVALL_MS * SYSTICKRATE_HZ) / 1000; //threshold for the Modbus delay

//loop:
	while (1) {
		//menu.buttonStatus(); //check buttons and update UI //remove while(1) from UI.cpp and use this while(1)
		//pressure = PressureSensor read
		//if(UI automaticMode){
		//	PIcontroller setTarget(UIgetTarget)
		//	PIcontroller setReading(pressure)
		//}
		//else{
		//	PIcontroller setTarget(0) //if we switch back to automatic mode the motor starts from 0
		//	PIcontroller setReading(0)
		//}
		//UI setPressure(pressure)
		if (ModbusDelayCounter >= ModbusDelayThreshold){
			ModbusDelayCounter = 0; //reset counter;
			//if(UI automaticMode){
			//	if(PIcontroller TimeOut){
			//		UI printErrorMessage
			//	}
			//	freq = PIcontroller getOutput
			//}
			//else{
			//
			//}
			//Modbus setFrequency(freq)
			//UI setFrequency(freq)
			//printStatus(freq, pressure) maybe target?
			printf("ModbusUpdate \n");

		}
		Sleep(MAININTERVALL_MS);
	}


//program should never leave the while(1)
    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}

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
#include "DigitalIoPin.h"
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

	void SysTick_Handler(void)
	{
		sleepCounter--; //decrement counter for Sleep-function
		ModbusDelayCounter ++; //increment counter for delay between Modbus updates
		if (PIC != NULL) PIC->sysTick();
		millisCounter++;
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

uint32_t millis() {
	return millisCounter;
}

bool setFrequency(ModbusMaster& node, uint16_t freq){
	int result;
	int ctr;
	bool atSetpoint;
	const int delay = 500;

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
	//delay needed?
	ControlWord = 0x047F; // set drive to start mode

	//PIcontroller:
	PIcontroller picontroller(1, 0.3, 40, 1000, 10); //1, 0.3, 40, 1000, 10
	PIC = &picontroller;
	picontroller.setTarget(0);

	//PressureSensor:
	PressureSens PS(1000);

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
		pressure = PS.getCompPressure();
		//if(UI automaticMode){
			picontroller.setTarget(0);//	PIcontroller setTarget(UIgetTarget)
			picontroller.setReading(pressure); //PIcontroller setReading(pressure)
		//}
		//else{
			picontroller.setTarget(0); //	PIcontroller setTarget(0) //if we switch back to automatic mode the motor starts from 0
			picontroller.setReading(0);//	PIcontroller setReading(0)
		//}
		//UI setPressure(pressure)
		if (ModbusDelayCounter >= ModbusDelayThreshold){
			ModbusDelayCounter = 0; //reset counter;
			//if(UI automaticMode){
				if(picontroller.getTimeOut()){
			//		UI printErrorMessage
					printf("PIcontroller Error: not able to reach pressure! \n");
				}
				freq = picontroller.getSpeed();
			//}
			//else{
			//
			//}
			if(!setFrequency(node,freq)){ //Modbus setFrequency(freq)
				printf("Modbus Error: failed to write frequency \n");
			}
			//UI setFrequency(freq) //not necessary

			printf("pressure= %f.2 freq= %d \n", pressure, freq); //maybe target as well?

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

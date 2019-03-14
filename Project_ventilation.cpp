/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#include <cstring>
#include <cstdio>
#include <ritimer_15xx.h>

#include "ModbusMaster.h"
#include "ModbusRegister.h"
#include "LpcUart.h"
#include "PIcontroller.h"
#include "PressureSens.h"
#include "UI.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/


/*****************************************************************************
 * Public functions
 ****************************************************************************/
static volatile int counter;
static volatile uint32_t systicks;
static PIcontroller* PIC;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	systicks++;
	if(counter > 0) counter--;
	if (PIC != NULL) PIC -> sysTick();

}
#ifdef __cplusplus
}
#endif

void Sleep(int ms)
{
	counter = ms;
	while(counter > 0) {
		__WFI();
	}
}

/* this function is required by the modbus library */
uint32_t millis() {
	return systicks;
}

bool setFrequency(ModbusMaster& node, uint16_t freq)
{
	int result;
	int ctr;
	bool atSetpoint;
	const int delay = 500;

	ModbusRegister Frequency(&node, 1); // reference 1
	ModbusRegister StatusWord(&node, 3);

	Frequency = freq; // set motor frequency

	// wait until we reach set point or timeout occurs
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

/**
 * @brief	Main UART program body
 * @return	Always returns 1
 */
int main(void)
{

#if defined (__USE_LPCOPEN)
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
	// Set up and initialize all required blocks and
	// functions related to the board hardware
	Board_Init();
    Chip_RIT_Init(LPC_RITIMER);
	// Set the LED to the state of "On"
	Board_LED_Set(0, true);
#endif
#endif
	LpcPinMap none = {-1, -1}; // unused pin has negative values in it
	LpcPinMap txpin = { 0, 18 }; // transmit pin that goes to debugger's UART->USB converter
	LpcPinMap rxpin = { 0, 13 }; // receive pin that goes to debugger's UART->USB converter
	LpcUartConfig cfg = { LPC_USART0, 115200, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, false, txpin, rxpin, none, none };
	LpcUart dbgu(cfg);

	/* Set up SWO to PIO1_2 */
	Chip_SWM_MovablePortPinAssign(SWM_SWO_O, 1, 2); // Needed for SWO printf

	/* Enable and setup SysTick Timer at a periodic rate */
    uint32_t sysTickRate;
	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / 1000);

	Board_LED_Set(0, false);
	Board_LED_Set(1, true);

	printf("Started\n"); // goes to ITM console if retarget_itm.c is included
	dbgu.write("Hello, world\n");

	//Modbus Stuff:
	ModbusMaster node(2); // Create modbus object that connects to slave id 2
	node.begin(9600);
	ModbusRegister ControlWord(&node, 0);
	ModbusRegister outputFreq(&node,102);
	ModbusRegister StatusWord(&node, 3);

	ControlWord = 0x0406; // prepare for starting

	ControlWord = 0x047F; // set drive to start mode

	//Set up pins for the LCD screen
    DigitalIoPin rs(0, 8, false); //RS
	DigitalIoPin en(1, 6, false); //EN
	DigitalIoPin d4(1, 8, false); //D4
	DigitalIoPin d5(0, 5, false); //D5
	DigitalIoPin d6(0, 6, false); //D6
	DigitalIoPin d7(0, 7, false); //D7

	DigitalIoPin b1(1, 3, true, true, true);
	DigitalIoPin b2(0, 9, true, true, true);
	DigitalIoPin b3(0, 10, true, true, true);


	LiquidCrystal lcd(&rs, &en, &d4, &d5, &d6, &d7);

	lcd.begin(16, 2);
	lcd.clear();

	UI menu(&lcd, &b1, &b2, &b3);
	//Set up the pressure sensor
	PressureSens ps(1000);

	//PC-controller stuff:
	PIcontroller picontroller(0.8, 0.1, 40, 1000, 100); //1, 0.3, 40, 1000, 100
	PIC = &picontroller;
	uint16_t freq;
	int status;
	float pressure;

	//Initial config to get a stable pressure inside the tube first
	setFrequency(node, 5000);
	Sleep(3000);
	pressure = ps.getCompPressure();

	menu.setFan(5000);
	menu.setPressure(pressure);

	picontroller.setTarget(40);

	while (1) {
//		menu.buttonStatus();
//		if (menu.getMode())
		/*
		 * Button polling
		 * */
		/*
		 * Manual mode
		 * */
		{

		}
		/*
		 * Auto mode
		 * */
		{
			pressure = ps.getCompPressure();
			picontroller.setReading(ps.getCompPressure());
			freq = picontroller.getSpeed();
		}
//		else
		/*
		 * Modbus communication
		 * */
		//Only call onc pr scond
		{
			//Set pressure value
//			if (target != menu.getPressure()) {
//				target = menu.getPressure();
//				picontroller.setTarget(target);
//			}

			// just print the value without checking if we got a -1
			//printf("F=%4d, I=%4d, v=%4d\n", (int) OutputFrequency, (int) Current, (int) Speed);
//			menu.setPressure(pressure);


			status = StatusWord;
			if (status > 0 && (status & 0x100)){
				setFrequency(node,freq);
//				menu.setFan(freq);
			}
			if (status & 0x8) {
				printf("ERROR");
			} else {
				printf("pressure: %f freq: %d output: %d \n",pressure+0.5, freq, (int) outputFreq);
			}
//			Sleep(1000);
		}
	}



	return 1;
}


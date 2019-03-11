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

#include "ModbusMaster.h"
#include "ModbusRegister.h"
#include "LpcUart.h"
#include "PIcontroller.h"
#include "PressureSens.h"

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
	const int delay = 50;

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


void init_Modbus(ModbusMaster* nodeP)
{
	nodeP->begin(9600); // set transmission rate - other parameters are set inside the object and can't be changed here

	ModbusRegister ControlWord(nodeP, 0);
	ModbusRegister StatusWord(nodeP, 3);
	ModbusRegister OutputFrequency(nodeP, 102);
	ModbusRegister Current(nodeP, 103);
	ControlWord = 0x0406; // prepare for starting

	ControlWord = 0x047F; // set drive to start mode

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
	SysTick_Config(SystemCoreClock / 1000);

	Board_LED_Set(0, false);
	Board_LED_Set(1, true);

	printf("Started\n"); // goes to ITM console if retarget_itm.c is included
	dbgu.write("Hello, world\n");


	//Modbus Stuff:
	ModbusMaster node(2); // Create modbus object that connects to slave id 2
	init_Modbus(&node);
	ModbusRegister outputFreq(&node,102);


	//PC-controller stuff:
	PIcontroller picontroller(1, 0.5, 40, 1000, 100); //1, 0.3, 40, 1000, 100
	PIC = &picontroller;
	picontroller.setTarget(10);
	uint16_t freq;
	int i =0;

	PressureSens ps(1000);

	while (1) {

			// just print the value without checking if we got a -1
			//printf("F=%4d, I=%4d, v=%4d\n", (int) OutputFrequency, (int) Current, (int) Speed);

			float pressure = ps.getCompPressure();
			picontroller.setReading(pressure);
			freq = picontroller.getSpeed();

			if (freq > 20000){
				setFrequency(node, 20000);
			}
			else if(freq < 0){
				setFrequency(node, 0);
			}
			else{
				setFrequency(node,freq);
			}

			if (i>0){
				printf("pressure: %f freq: %d output: %d \n",pressure+0.5, freq, (int) outputFreq);
				i = 0;
			}
			i++;
			Sleep(100);
	}



	return 1;
}


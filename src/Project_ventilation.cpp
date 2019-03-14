/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition test
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
#include <ritimer_15xx.h>
#include "LiquidCrystal.h"
#include <atomic>
#include "UI.h"

static volatile std::atomic_int counter;

#ifdef __cplusplus
extern "C" {
#endif

//* @brief Handle interrupt from SysTick timer
//* @return Nothing

void SysTick_Handler(void)
{
	if(counter > 0) counter--;
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


int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    uint32_t sysTickRate;

	Chip_Clock_SetSysTickClockDiv(1);
	sysTickRate = Chip_Clock_GetSysTickClockRate();
	SysTick_Config(sysTickRate / 1000);

    Chip_RIT_Init(LPC_RITIMER);

    //Board_LED_Set(0, true);
#endif
#endif

    // TODO: insert code here

    DigitalIoPin rs(0, 8, false); //RS
	DigitalIoPin en(1, 6, false); //EN
	DigitalIoPin d4(1, 8, false); //D4
	DigitalIoPin d5(0, 5, false); //D5
	DigitalIoPin d6(0, 6, false); //D6
	DigitalIoPin d7(0, 7, false); //D7

	DigitalIoPin b1(1,3,true, true, true);
	DigitalIoPin b2(0,9,true, true, true);
	DigitalIoPin b3(0,10,true, true, true);


	LiquidCrystal lcd(&rs, &en, &d4, &d5, &d6, &d7);

	lcd.begin(16, 2);
	lcd.clear();


	UI menu(&lcd, &b1, &b2, &b3);

	while (1) {
		menu.buttonStatus();
	}



    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}

#include "stm32l476xx.h"
#include "lcd.h"

/* Spencer Goulette
	 02/15/18
	 ECE 271 LAB4
	 Objective: Have the Motor Turn using the half step and full step method. Also be able to enter degree that you want the motor to rotate and half it rotate that amount of degrees
	 Clock Frequency: 32KHz
*/

void System_Clock_Init(void);
void printLCD(void);
extern int timespan;

void printLCD(void){
	char str[6] = {' ',' ',' ',' ',' ',' '};
	//sprintf(str, "%d", (timespan/100));				// 1 Hz Signal
	sprintf(str, "%d", (timespan/148));					// Distance
	LCD_DisplayString((uint8_t*)str);
}
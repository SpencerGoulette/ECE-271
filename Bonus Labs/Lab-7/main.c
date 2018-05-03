#include "stm32l476xx.h"
#include "lcd.h"

/* Spencer Goulette
	 05/03/18
	 ECE 271 Bonus lab 7
	 Objective: Get RTC to blink LED every second and show time (also date for bonus)
	 Pins: Port B pins used for red LED and EXTI3 used for date and EXTI20 used for interrupt for RTC and Alarm interrupt used to toggle every second
	 Clock Frequency: 16MHz
	 LSE: 32.768KHz
*/

void System_Clock_Init(void);
void LCD_RTC_Clock_Enable(void);
void EXTI_Init(void);
void RTC_Init(void);
void RTC_Set_Alarm(void);
void RTC_Alarm_Enable(void);
void RTC_Alarm_IRQHandler(void);
void Enter_SleepMode(void);
void RTC_Wakeup_Configuration(void);
void EXTI_Init(void);
void EXTI3_IRQHandler(void);

uint32_t time;	// Variable for the time
uint32_t date;	// Variable for the date
uint8_t showTime = 1;	// To figure out whether to show the time or date
char str[6] = {' ',' ',' ',' ',' ',' '};

int main(void)
{	
	LCD_Initialization();	// Initialize everything needed
	System_Clock_Init();
	EXTI_Init();
	
	// Enable the clock to GPIO Port A	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;   

	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOA->MODER &= ~0x00000CFF;   // Clear bits 0-7, and bit 10 and bit 11
  
  // PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOA->PUPDR &= ~0x00000CFF;  // Clear bits 0-7, and bit 10 and bit 11
	GPIOA->PUPDR |= 0x000008AA; // Set bits 1, 3, 5, 7, and 11
	
	// Enable the clock to GPIO Port B	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;   

	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOB->MODER &= ~0x00000030 ;   // Clear bit 4 and bit 5
  GPIOB->MODER |= 0x00000010;   // Set bit 4
  
  // PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOB->PUPDR &= ~0x00000030;  // Clear bit 4 and bit 5
	GPIOB->ODR ^= GPIO_ODR_ODR_2;
	RTC_Alarm_Enable();	// Enable the RTC alarm
	RTC_Set_Alarm();	// Set RTC alarm so it will go off every second
	RTC_Wakeup_Configuration();
	while(1)
	{
		Enter_SleepMode(); // Used to save power
	}
}

void System_Clock_Init(void){
	
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 );
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;  //01: HSI16 oscillator used as system clock

  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 );
		
}

void LCD_RTC_Clock_Enable(void)
{
	// Enable write access to the backup domain
	if ((RCC->APB1ENR1 & RCC_APB1ENR1_PWREN) == 0)
	{
		// Enable power interface clock
		RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
		
		//Short delay after enabling on RCC peripheral clock
		(void) RCC->APB1ENR1;
	}
	
	// Select LSE as RTC clock source
	// RTC/LCD Clock; (1) LSE is in the backup domain (2) HSE and LSI are not
	if ((PWR->CR1 & PWR_CR1_DBP) == 0)
	{
		// Enable write access to RTC and registers in backup domain
		PWR->CR1 |= PWR_CR1_DBP;
		
		// Wait until the backup domain write protection has been disabled
		while((PWR->CR1 & PWR_CR1_DBP) == 0);
	}
	
	// Reset LSEON and LSEBYP bits before configuring LSE
	// BDCR = Backup Domain Control Register
	RCC->BDCR &= ~(RCC_BDCR_LSEON | RCC_BDCR_LSEBYP);
	
	// RTC Clock selection can be changed only if the backup domain is reset
	RCC->BDCR |= RCC_BDCR_BDRST;
	RCC->BDCR &= ~RCC_BDCR_BDRST;
	
	// Wait until LSE clock is ready
	while((RCC->BDCR & RCC_BDCR_LSERDY) == 0)
	{
		RCC->BDCR |= RCC_BDCR_LSEON; // Enable LSE oscillator
	}
	
	// Select LSE as RTC clock source
	// RTCSEL[1:0]: 00 = No Clock, 01 - LSE, 10 = LSI, 11 = HSE
	RCC->BDCR &= ~RCC_BDCR_RTCSEL; // Clear RTCSEL bits
	RCC->BDCR |= RCC_BDCR_RTCSEL_0; // Select LSE as RTC clock
	RCC->BDCR |= RCC_BDCR_RTCEN;
	
	// Disable power interface clock
	RCC->APB1ENR1 &= ~RCC_APB1ENR1_PWREN;
	
	// Enable LCD Peripheral Clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_LCDEN;
}

void RTC_Init(void)
{
	// Enable RTC Clock
	LCD_RTC_Clock_Enable();
	
	// Enable power interface clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
	
	// Disable write protection of RTC registers by writing disarm keys
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	
	// Enter initialization mode to program TR and DR registers
	RTC->ISR |= RTC_ISR_INIT;
	
	// Wait until INITF has been set
	while((RTC->ISR & RTC_ISR_INITF) == 0);
	
	// Generate a 1Hz clock for the RTC time counter
	// LSE = 32.768 kHz = 2^15 Hz	
	RTC->PRER |= (127) << 16;
	RTC->PRER |= (255);
	
	RTC->CR &= ~RTC_CR_BYPSHAD;
	
	// Set time as 11:32:00 am
	RTC->TR = ((0<<22) | (1<<20) | (1<<16) | (3<<12) | (2<<8));
	
	// Set date as 2016/05/27
	RTC->DR = ((1<<20) | (8<<16) | (0<<12) | (5<<8) | (0<<4) | (3));
	
		// Hour format: 0 = 24 hour/day; 1 = AM/PM hour
	RTC->CR &= ~RTC_CR_FMT;
	
	// Exit initialization mode
	RTC->ISR &= ~RTC_ISR_INIT;
	
	// Enable write protection for RTC registers
	RTC->WPR = 0xFF;
	
}

void RTC_Set_Alarm(void)
{
	uint32_t AlarmTimeReg;
	
	// Disable alarm A
	RTC->CR &= ~RTC_CR_ALRBE;
	
	RTC->CR &= ~RTC_CR_OSEL;
	
	// Remove write-protection of RTC registers by writing "0xCA"
	// and then "0x53" into the WPR register
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	
	// Disable alarm A and its interrupt
	RTC->CR &= ~RTC_CR_ALRBE;
	RTC->CR &= ~RTC_CR_ALRBIE;
	
	// Wait until access to alarm registers is allowed
	// Write flag (ALRAWF) is set by hardware if alarm A can be changed
	while((RTC->ISR & RTC_ISR_ALRBWF) == 0);
	
	// Set off alarm A if the seconds is 30
	// Bits[6:4] = Ten's digit for the second in BCD format
	// Bits[3:0] = Unit's digit for the second in BCD format
	AlarmTimeReg = 0;
	
	// Set alarm mask field to compare only the second
	AlarmTimeReg |= RTC_ALRMBR_MSK4;	// 1: Ignore day of week in comparison
	AlarmTimeReg |= RTC_ALRMBR_MSK3;	// 1: Ignore hour in comparison
	AlarmTimeReg |= RTC_ALRMBR_MSK2;	// 1: Ignore m inute in alarm comparison
	AlarmTimeReg |= RTC_ALRMBR_MSK1;	// 1: Alarm sets off every second
	
	// RTC alarm A register (ALRMAR)
	RTC->ALRMBR = AlarmTimeReg;
	
	// Enable alarm A and its interrupt
	RTC->CR |= RTC_CR_ALRBE;
	RTC->CR |= RTC_CR_ALRBIE;
	
	// Enable write protection for RTC registers
	RTC->WPR = 0xFF;
}

void RTC_Alarm_Enable(void)
{
	RTC_Init();
	
	// Configure EXTI 18
	// Select triggering edge
	EXTI->RTSR1 |= EXTI_RTSR1_RT18; // 1 = Trigger at rising edge
	
	// Interrupt mask register 
	EXTI->IMR1 |= EXTI_IMR1_IM18; // 1 = Enable EXTI 18 line
	
	// Event mask register
	EXTI->EMR1 |= EXTI_EMR1_EM18;	// 1 = Enable EXTI 18 line
	
	// Interrupt pending register
	EXTI->PR1 |= EXTI_PR1_PIF18; // Write 1 to clear pending request
	
	//Enable RTC interrupt
	NVIC->ISER[1] |= 1<<9;	// RTC_Alarm_IRQn = 41, See Chapter 11.6.1
	// It is equivalent to: NVIC_EnableIRQ(RTC_WKUP_IRQn);
	
	// Set interrupt priority as the most urgent
	NVIC_SetPriority(RTC_Alarm_IRQn, 0);
}

void RTC_Alarm_IRQHandler(void)
{
	
	// RTC initialization and status register (RTC_ISR)
	// Hardware sets the Alarm A flag (ALRAF) when the time/date registers
	// (RTC_TR and RTC_DR) match the alarm A register (RTC_ALRMAR), according to the mask bits
	
	if(RTC->ISR & RTC_ISR_ALRBF)
	{
		// Turns TR and DR into 6 digit numbers
		time = (RTC->TR & 0xF) + (((RTC->TR & 0x70) >> 4) * 10) + (((RTC->TR & 0xF00) >> 8) * 100) + (((RTC->TR & 0x7000) >> 12) * 1000) + (((RTC->TR & 0xF0000) >> 16) * 10000) + (((RTC->TR & 0x300000) >> 20) * 100000);
		date = ((RTC->DR & 0xF0000) >> 16) + (((RTC->DR & 0xF00000) >> 20) * 10) + ((RTC->DR & 0xF) * 100) + (((RTC->DR & 0x03) >> 4) * 1000) + (((RTC->DR & 0xF00) >> 8) * 10000) + (((RTC->DR & 0x1000) >> 12) * 100000);
		
		//Figures out whether to display time or date
		if(showTime == 1)
		{
			sprintf(str, "%6d", time);
		}
		else
		{
			sprintf(str, "%6d", date);
		}
		
		// Display
		LCD_DisplayString(str);
		GPIOB->ODR ^= GPIO_ODR_ODR_2;	// Toggle the GPIO PB.2
		RTC->ISR &= ~(RTC_ISR_ALRBF); // Clear alarm A interrupt flag
	}
	
	// Clear the EXTI line 18
	EXTI->PR1 |= EXTI_PR1_PIF18;	// Write 1 to clear pending interrupt
}

void RTC_Wakeup_Configuration(void)
{
	// Wakeup initialization can only be performed when wakeup is disabled
	RTC->CR &= ~RTC_CR_WUTE;	// Disable wakeup counter
	
	// WUTWF: Wakeup timer write flag
	// 0: Wakeup timer configuration update not allowed
	// 1: Wakeup timer configuration update allowed
	while((RTC->ISR & RTC_ISR_WUTWF) == 0);
	
	// WUCKSEL[2:0]: Wakeup clock selection
	// 10x: ck_spre (usually 1Hz) clock is selected
	RTC->CR &= ~RTC_CR_WUCKSEL;
	RTC->CR |= RTC_CR_WUCKSEL_2; // Select ck_pre (1Hz)
	
	// RTC wakeup timer register (Max = 0xFFFF)
	RTC->WUTR = 1; // The Counter decrements by 1 every pulse of the clock selected by WUCKSEL
	
	// Enabke wake up counter and wake up interrupt
	RTC->CR |= RTC_CR_WUTIE; // Enable wake up interrupt
	RTC->CR |= RTC_CR_WUTE;  // Enable wake up counter
}

void Enter_SleepMode(void)
{
	
	// Cortex system control register
	SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;	// 0 = sleep, 1 = deep sleep
	
	// SLEEPONEXIT: Indicates sleep-on-exit when returning from handler
	// mode to thread mode:
	// 0 = do not sleep when returning to thread mode.
	// 1 = enter sleep, or deep sleep, on return from an ISR.
	SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
	
	// RTC wakeup interrupt is connected EXTI line 20 internally
	// Enable EXTI20 interrupt
	EXTI->IMR1 |= EXTI_IMR1_IM20;
	
	// Enable EXTI20 event
	EXTI->EMR1 |= EXTI_EMR1_EM20;
	
	// Select rising-edge trigger
	EXTI->RTSR1 |= EXTI_RTSR1_RT20;
	
	NVIC_EnableIRQ(RTC_WKUP_IRQn);
	NVIC_SetPriority(RTC_WKUP_IRQn, 0);
	
	__DSB();	// Ensure that the last store takes effect
	__WFI();	// Switch Processor into the sleep mode
}

void EXTI_Init(void)
{
	// Enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	// Select PA.3 as the trigger source of EXTI 3
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA;
	SYSCFG->EXTICR[0] &= ~(0x000F);
	
	// Enable rising edge trigger for EXTI3
	// Rising trigger selection register (RSTR)
	// 0 = disabled, 1 = enabled
	EXTI->RTSR1 |= EXTI_RTSR1_RT3;
	
	// Disable falling edge trigger for EXTI3
	// Falling trigger selection register (FSTR)
	// 0 = disabled, 1 = enabled
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT3;
	
	// Enable EXTI 3 interrupt
	// Interupt mask register: 0 = masked, 1 = unmasked
	// "Masked" means that processor ignores the corresponding interrupt.
	EXTI->IMR1 |= EXTI_IMR1_IM3;
	
	// Enable EXTI 3 interrupt
	NVIC_EnableIRQ(EXTI3_IRQn);
	
	// Set EXTI 3 priority to 1
	NVIC_SetPriority(EXTI3_IRQn, 1);
	
}

void EXTI3_IRQHandler(void)
{
	// Check for EXTI 3 interrupt flag
	if((EXTI->PR1 & EXTI_PR1_PIF3) == EXTI_PR1_PIF3)
	{
		// Toggle LED
		showTime ^= 0x1;
		
		// Clear interrupt pending request
		EXTI->PR1 |= EXTI_PR1_PIF3;
	}
}

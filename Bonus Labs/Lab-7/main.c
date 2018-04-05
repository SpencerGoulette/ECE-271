#include "stm32l476xx.h"
#include "lcd.h"

/* Spencer Goulette
	 03/01/18
	 ECE 271 LAB6
	 Objective: Get LED to blink with
	 Pins: Port B pins used for red LED
	 Clock Frequency: 8MHz
	 SysTick Clock Frequency: 1MHz
*/

void LCD_RTC_Clock_Enable(void);
void EXTI_Init(void);
void EXTI20_IRQHandler(void);
void RTC_Init(void);
void RTC_Set_Alarm(void);
void RTC_Alarm_Enable(void);
void RTC_Alarm_IRQHandler(void);
void Enter_SleepMode(void);
void RTC_Wakeup_Configuration(void);

int main(void)
{	
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
	RTC_Alarm_Enable();
	RTC_Set_Alarm();
	//RTC_Wakeup_Configuration();
	while(1)
	{
		//Enter_SleepMode();
	}
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
	
	// Disable power interface clock
	RCC->APB1ENR1 &= ~RCC_APB1ENR1_PWREN;
	
	// Enable LCD Peripheral Clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_LCDEN;
}

void RTC_Init(void)
{
	// Enable RTC Clock
	LCD_RTC_Clock_Enable();
	
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
	RTC->DR = ((1<<20) | (6<<16) | (0<<12) | (5<<8) | (2<<4) | (7));
	
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
	AlarmTimeReg = 0x3 << 4;
	
	// Set alarm mask field to compare only the second
	AlarmTimeReg |= RTC_ALRMBR_MSK4;	// 1: Ignore day of week in comparison
	AlarmTimeReg |= RTC_ALRMBR_MSK3;	// 1: Ignore hour in comparison
	AlarmTimeReg |= RTC_ALRMBR_MSK2;	// 1: Ignore m inute in alarm comparison
	AlarmTimeReg &= ~RTC_ALRMBR_MSK1;	// 0: Alarm sets off if the seconds match
	
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

void EXTI120_IRQHandler(void)
{
	// Check for EXTI 3 interrupt flag
	if((EXTI->PR1 & EXTI_PR1_PIF20) == EXTI_PR1_PIF20)
	{
		// Toggle LED
		GPIOB->ODR ^= GPIO_ODR_ODR_2;
		
		// Clear interrupt pending request
		EXTI->PR1 |= EXTI_PR1_PIF20;
	}
}


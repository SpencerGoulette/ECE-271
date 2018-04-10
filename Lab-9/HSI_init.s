; Spencer Goulette
; 02/22/18
; ECE 271 LAB5
; Objective: Have the Motor Turn using Assembly.
; Clock Frequency: 32KHz

	INCLUDE core_cm4_constants.s		; Load Cortex-M4 Definitions
	INCLUDE stm32l476xx_constants.s     ; Load STM32L4 Definitions 

	AREA    hsi, CODE
	EXPORT	HSI_init
	ALIGN			

HSI_init  PROC
			
		; Turn on HSI oscillator
		LDR r0, =RCC_BASE
		LDR r1, [r0, #RCC_CR]
		ORR r1, r1, #RCC_CR_HSION
		STR r1, [r0, #RCC_CR]
		
		; Select HSI as system clock
		LDR r1, [r0, #RCC_CFGR]
		BIC r1, r1, #RCC_CFGR_SW
		ORR r1, r1, #RCC_CFGR_SW_HSI
		STR r1, [r0, #RCC_CFGR]
		
		; Wait for HSI stable
waitHSI	LDR r1, [r0, #RCC_CR]
		AND r1, r1, #RCC_CR_HSIRDY
		CMP r1, #0
		BEQ waitHSI
		BX LR
		ENDP	
					
		ALIGN			

		AREA    myData, DATA, READWRITE
		ALIGN
array	DCD    1, 2, 3, 4
		END

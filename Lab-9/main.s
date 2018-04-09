; Spencer Goulette
; 02/22/18
; ECE 271 LAB5
; Objective: Have the Motor Turn using Assembly.
; Clock Frequency: 32KHz

	INCLUDE core_cm4_constants.s		; Load Cortex-M4 Definitions
	INCLUDE stm32l476xx_constants.s     ; Load STM32L4 Definitions 

	AREA    main, CODE
	EXPORT	__main				; make __main visible to linker
	EXPORT	timespan
	EXPORT	lastcounter
	EXPORT	overflow	
	IMPORT	GPIO_init
	IMPORT	HSI_init
	IMPORT	TIM4_init
	IMPORT	TIM4_IRQHandler
	IMPORT	keyPad
	ALIGN
	ENTRY			
				
__main	PROC
		
		BL HSI_init
		BL GPIO_init
		BL TIM4_init
		
inf		B inf	;infinite while loop
		
stop 	B 		stop     ; dead loop & program hangs here
			
		ENDP
					
		ALIGN			

		AREA    myData, DATA, READWRITE
		ALIGN
array	DCD    1, 2, 3, 4
timespan	DCD    0	; Pulse width
lastcounter	DCD    0	; Timer counter value of last capture event
overflow	DCD	   0	; Counter for the number of overflows
		END

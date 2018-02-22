; Spencer Goulette
; 02/22/18
; ECE 271 LAB5
; Objective: Have the Motor Turn using Assembly.
; Clock Frequency: 32KHz

	INCLUDE core_cm4_constants.s		; Load Cortex-M4 Definitions
	INCLUDE stm32l476xx_constants.s     ; Load STM32L4 Definitions 

	AREA    main, CODE, READONLY
	EXPORT	__main				; make __main visible to linker
	ENTRY			
				
__main	PROC
	
		; Enable the clock to GPIO Port B	
		LDR r0, =RCC_BASE
		LDR r1, [r0, #RCC_AHB2ENR]
		ORR r1, r1, #RCC_AHB2ENR_GPIOBEN
		STR r1, [r0, #RCC_AHB2ENR]

		; MODE: 00: Input mode,              01: General purpose output mode
		;       10: Alternate function mode, 11: Analog mode (reset state)
		; Setting GPIOB MODER
		LDR r0, =GPIOB_BASE
		LDR r1, [r0, #GPIO_MODER]
		AND r1, #0
		ORR r1, r1, #GPIO_MODER_MODER2_0
		ORR r1, r1, #GPIO_MODER_MODER3_0
		ORR r1, r1, #GPIO_MODER_MODER6_0
		ORR r1, r1, #GPIO_MODER_MODER7_0
		STR r1, [r0, #GPIO_MODER]
	
		; Sets values for for loops
		MOV r2, #511
		MOV r3, #0
		MOV r4, #0
		MOV	r5, #0
		MOV r6, #0
		
loopi	CMP r3, r2	;rotations loop
		BGT done3
		
loopj	CMP	r4, #3	;The 4 cases of Full Step loop
		BGT done2
		
loopk	CMP r5, #1400	;Time Delay
		BGT loopl
		ADD r5, r5, #1
		B loopk
		
loopl	CMP r6, #1400	;Time Delay
		BGT done1
		ADD r6, r6, #1
		B loopl

done1	LDR r1, [r0, #GPIO_ODR]	;Sets 2, 3, 6, and 7 ODR to 0
		AND r1, r1, #0
		STR r1, [r0, #GPIO_ODR]
		
		MOV r5, #0	;Sets Time Delays back to 0
		MOV r6, #0
		
		CMP r4, #0	;Checks first case if it is equal to 0
		BEQ if1
		BNE alt1
		
if1		LDR r1, [r0, #GPIO_ODR]	;If r4 == 0, set ODR 2 and 7
		ORR r1, r1, #GPIO_ODR_ODR_2
		ORR r1, r1, #GPIO_ODR_ODR_7
		STR r1, [r0, #GPIO_ODR]

alt1	CMP r4, #1	;Checks second case if it is equal to 1
		BEQ if2
		BNE alt2
		
if2		LDR r1, [r0, #GPIO_ODR]	;If r4 == 0, set ODR 2 and 6
		ORR r1, r1, #GPIO_ODR_ODR_2
		ORR r1, r1, #GPIO_ODR_ODR_6
		STR r1, [r0, #GPIO_ODR]
		
alt2	CMP r4, #2	;Checks third case if it is equal to 2
		BEQ if3
		BNE alt3
		
if3		LDR r1, [r0, #GPIO_ODR]	;If r4 == 0, set ODR 6 and 3
		ORR r1, r1, #GPIO_ODR_ODR_6
		ORR r1, r1, #GPIO_ODR_ODR_3
		STR r1, [r0, #GPIO_ODR]
		
alt3	CMP r4, #3	;Checks fourth case if it is equal to 3
		BEQ if4
		BNE alt4
		
if4		LDR r1, [r0, #GPIO_ODR]	;If r4 == 0, set ODR 3 and 7
		ORR r1, r1, #GPIO_ODR_ODR_3
		ORR r1, r1, #GPIO_ODR_ODR_7
		STR r1, [r0, #GPIO_ODR]
		
alt4	ADD r4, r4, #1	;Increment Case r4
		B	loopj
		
done2	MOV r4, #0	;Change r4 back to 0 and increment r3 loop
		ADD r3, r3, #1
		B	loopi
		
done3	MOV r3, #0	;Clear ODR
		LDR r1, [r0, #GPIO_ODR]
		AND r1, r1, #0
		STR r1, [r0, #GPIO_ODR]
		
stop 	B 		stop     		; dead loop & program hangs here

	ENDP
					
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 4
	END

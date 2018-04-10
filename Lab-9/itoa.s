; Spencer Goulette
; 02/22/18
; ECE 271 LAB5
; Objective: Have the Motor Turn using Assembly.
; Clock Frequency: 32KHz

	INCLUDE core_cm4_constants.s		; Load Cortex-M4 Definitions
	INCLUDE stm32l476xx_constants.s     ; Load STM32L4 Definitions 

	AREA    myData, DATA
	ALIGN
str	SPACE	20
	
	AREA	my_itao, CODE
	EXPORT	itoa
	ALIGN
	ENTRY			
				
itoa	PROC
		PUSH {r4-r6, lr}
		MOV	r2, r0
		MOV r3, r1
		
		MOV r6, #10
loop1	CBZ r2, done
		UDIV r5, r2, r6
		MLS r4, r6, r5, r2
		ADD r4, r4, #0x30
		STRB r4, [r3], #1
		UDIV r2, r2, r6
		B	loop1
		
done	MOV	r4, #0
		STRB r4, [r3]
		SUB	r3, r3, #1
		
loop2	CMP	r3, r1
		BLE exit
		LDRB r4, [r3]
		LDRB r5, [r1]
		STRB r4, [r1]
		STRB r5, [r3]
		ADD	r1, r1, #1
		SUB r3, r3, #1
		B	loop2
		
exit	POP	{r4-r6, pc}
		ENDP
		END
		
			
		ENDP
		END

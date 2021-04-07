/*
	This code was written to support the Wednesday book, "ARM Assembly for Embedded Applications",
	by Daniel W. Lewis. Permission is granted to freely share this software provided
	that this notice is not removed. This software is intended to be used with a run-time
	library adapted by the author from the STM Cube Library for the 32F429IDISCOVERY 
	board and available for download from http://www.engr.scu.edu/~dlewis/book3.
*/
		.syntax     unified
		.cpu        cortex-m4
		.text

		// uint32_t Mul32X10(uint32_t multiplicand) ;
		.global		Mul32X10
		.thumb_func
		.align
Mul32X10:			
		ADD 		R0,R0,R0,LSL2		//Adds (R0 + 4R0) = 5R0
		LSL 		R0,R0,1				//Multiples 5R0 * 2 = 10R0
		BX			LR					//Return

		// uint32_t Mul64X10(uint32_t multiplicand) ;
		.global		Mul64X10
		.thumb_func
		.align
Mul64X10:			// R1.R0 = multiplicand
		LSLS		R0,R0,1				//Logical Shift Left of LS 32 bits, R0 = 2R0, and also captures C
		ADC 		R1,R1,R1			//Logical Shift Left of MS 32 bits by 1, R1 = 2R1, adds C
		LSL			R3,R1,2				//R3 = 2R1 * 4 = 8R1
		ADD			R3,R3,R0,LSR 30		//Fix 3 LS bits 
		LSL			R2,R0,2				//R2 = 2R0 * 4= 8R0
		ADDS 		R0,R0,R2			//R0 = 2R0 + 8R0 = 10R0
		ADC 		R1,R1,R3			//R0 = 2R1 + 8R1 = 10R1
		BX			LR					//Return

		// uint32_t Div32X10(uint32_t dividend) ;
		.global		Div32X10
		.thumb_func
		.align
Div32X10:			
		LDR			R1,=3435973837		//Loads R1 with (2^35 + 2) / 10
		UMULL		R2,R1,R1,R0			//unsigned multiplication (32 bit * 32 bit) returning a 64 bit to R2.R1
		LSR			R0,R1,3				//Logical Shift Right by 8 bits
		BX			LR					//Return
		.end

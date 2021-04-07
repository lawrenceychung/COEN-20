/*
	This code was written to support the book, "ARM Assembly for Embedded Applications",
	by Daniel W. Lewis. Permission is granted to freely share this software provided
	that this notice is not removed. This software is intended to be used with a run-time
	library adapted by the author from the STM Cube Library for the 32F429IDISCOVERY 
	board and available for download from http://www.engr.scu.edu/~dlewis/book3.
*/
		.syntax		unified
		.cpu		cortex-m4
		.text

		// int Sum(int Ai, int Bi, Cin) ;
		.global		Sum1
		.thumb_func
		.align
		// Sum = Ai ^ Bi ^ Cin
Sum1:	
		EOR			R0,R0,R1		//Exclusive Bitwise or between Ai and Bi
		EOR 		R0,R0,R2		//Exclusive Bitwise or between (Ai ^ Bi) and Cin
		BX			LR				//Return

		.global		Sum2
		.thumb_func
		.align
		// Sum = (Ai + Bi + Cin) & 1
Sum2:
		ADD 		R0,R0,R1		//Adds Ai + Bi
		ADD 		R0,R0,R2		//Adds (Ai + Bi) + Cin
		AND  		R0,R0,1			//Bitwise AND by 1
		BX			LR				//Return

		.global		Sum3
		.thumb_func
		.align
		// Sum = ((0b10010110 >> ((Cin << 2) | (Bi << 1) | Ai)) & 1
Sum3:	
		LSL			R2,R2,2			//Logical Left Shift Cin by 2
		LSL			R1,R1,1			//Logical Left Shift Bi by 1
		ORR			R0,R0,R1		//Bitwise OR Ai and Bi << 1
		ORR			R0,R0,R2		//Bitwise OR (Ai | Bi << 1) and Cin
		LDR 		R3,=0b10010110	//Loads 0b10010110 in R3
		LSR			R0,R3,R0		//Logical Rigth Shift 0b10010110 by (Cin << 2) | (Bi << 1) | Ai)
		AND			R0,R0,1			//Bitwise AND by 1
		BX			LR				//Return

		.global		Sum4
		.thumb_func
		.align
		// Sum = sum[(Cin << 2) | (Bi << 1) | Ai)]
Sum4:	
		LSL			R2,R2,2			//Logical Left Shift Cin by 2
		LSL			R1,R1,1			//Logical Left Shfit Bi by 1
		ORR			R2,R2,R1		//Bitwise OR (Cin << 2) | (Bi << 1)
		ORR			R1,R2,R0		//Bitwise OR (Cin << 2) | (Bi << 1) | Ai)
		LDR			R0,=sum			//Load R0 with sum array
		LDRB		R0,[R0,R1]		//Load R0 with the byte specified 
		BX			LR				//Return
sum:	.byte		0,1,1,0,1,0,0,1

		// int Cout(int Ai, int Bi, Cin) ;
		.global		Cout1
		.thumb_func
		.align
		// Cout = Ai&Bi | Ai&Cin | Bi&Cin
Cout1:	
		AND			R3,R0,R1		//Bitwise AND Ai and Bi, store into R3
		AND 		R0,R0,R2		//Bitwise AND Ai and Cin
		AND 		R1,R1,R2		//Bitwise AND Bi and Cin
		ORR			R0,R3,R0		//Bitiwse OR Ai&Bi | Ai&Cin
		ORR			R0,R0,R1		//Bitwise OR (Ai&Bi | Ai&Cin) | Bi&Cin
		BX			LR				//Return

		.global		Cout2
		.thumb_func
		.align
		// Cout = (Ai + Bi + Cin) >> 1
Cout2:	
		ADD 		R0,R0,R1		//Adds Ai + Bi
		ADD 		R0,R0,R2		//Adds (Ai + Bi) + Cin
		LSR  		R0,R0,1			//Logical Shfit Right everything by 1
		BX			LR				//Return

		.global		Cout3
		.thumb_func
		.align
		// Cout = ((0b11101000 >> ((Cin << 2) | (Bi << 1) | Ai)) & 1
Cout3:	
		LSL			R2,R2,2			//Logical Left Shift Cin by 2
		LSL			R1,R1,1			//Logical Left Shift Bi by 1
		ORR			R0,R0,R1		//Bitwise OR Ai and Bi << 1
		ORR			R0,R0,R2		//Bitwise OR (Ai | (Bi << 1)) | (Cin << 2)
		LDR 		R3,=0b11101000	//Load 0b11101000 in R3
		LSR			R0,R3,R0		//Logical Right Shift 0b11101000 by (Ai | (Bi << 1)) | (Cin << 2)
		AND			R0,R0,1			//Bitwse AND by 1
		BX			LR				//Return 

		.global		Cout4
		.thumb_func
		.align
		// Cout = carry[(Cin << 2) | (Bi << 1) | Ai)]
Cout4:	
		LSL			R2,R2,2			//Logical Left Shift Cin by 2
		LSL			R1,R1,1			//Logical Left Shfit Bi by 1
		ORR			R2,R2,R1		//Bitwise OR (Cin << 2) | (Bi << 1)
		ORR			R1,R2,R0		//Bitwise OR (Cin << 2) | (Bi << 1) | Ai)
		LDR			R0,=carry		//Load R0 with carry array
		LDRB		R0,[R0,R1]		//Load R0 with the byte specified
		BX			LR				//Return
		
carry:	.byte		0,0,0,1,0,1,1,1
		BX			LR
		.end

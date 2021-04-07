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
	
						// Q16 Normalize(Q16 divisor, int zeros) ;
						.global		Normalize
						.thumb_func
						.align
Normalize:				// R0 = divisor, R1 = zeros
						CMP			R1,16					//compare R1 with 16
						ITTEE		GE 						//IT Block if greater than
						SUBGE		R1,R1,16				//if greater than or equal to, then shift R0 left by (R1-16)
						LSLGE		R0,R0,R1
						RSBLT		R1,R1,16				//if less than, then shift R0 right by (16-R1)
						LSRLT		R0,R0,R1
						BX			LR						//Return

						// Q16 Denormalize(Q16 estimate, int zeros) ;
						.global		Denormalize
						.thumb_func
						.align
Denormalize:			// R0 = estimate, R1 = zeros
						CMP			R1,16					//compare R1 with 16
						ITTEE		GE 						//IT Block if greater than
						SUBGE		R1,R1,16				//if greater than or equal to, then shift R0 left by (R1-16)
						LSRGE		R0,R0,R1
						RSBLT		R1,R1,16				//if less than, then shift R0 right by (16-R1)
						LSLLT		R0,R0,R1
						BX			LR						//Return

						// Q16 NormalizedEstimate(Q16 divisor) ;

						.global		NormalizedEstimate
						.thumb_func
						.align
NormalizedEstimate:		// R0 = divisor
						LDR			R2,=185043				//load R2 with (48/17) * 65536 + 0.5; floating point to Q16
						LDR			R1,=123362				//load R1 with (32/17) * 65536 + 0.5; floating point to Q16
						SMULL		R0,R1,R0,R1				//multiples R0 with R1 into a 64 bit; multiply Q16 x Q16
						LSR 		R0,R0,16				//extract middle 32 bits
						ORR 		R0,R0,R1,LSL 16
						SUB 		R0,R2,R0				//subtracts (48/17) by R0; 
						BX			LR						//Return

						// Q16 InitialEstimate(Q16 divisor) ;
						.global		InitialEstimate
						.thumb_func
						.align
InitialEstimate:		// R0 = original divisor
						PUSH		{R4,LR}					//push R4, LR
						CLZ			R1,R0					//counts leading zeros, saves into R1
						MOV			R4,R1					//makes copy of R1 to R4
						BL 			Normalize				//calls Normalize function passing in R0 and R1
						BL			NormalizedEstimate		//calls NormalizedEstimate passing in R0 returned from Normalize
						MOV			R1,R4					//makes copy of R4 to R1
						BL 			Denormalize				//calls Denormalize passing in R0 returned by NormalizedEstimate and R1
						POP			{R4,PC}					//pops R4, PC, and returns

						// Q16 Reciprocal(Q16 divisor) ;
						.global		Reciprocal
						.thumb_func
						.align
Reciprocal:				// R0 = divisor
						PUSH		{R4,LR}					//push R4,LR
						MOV			R4,R0					//makes copy of R0 to R4 to preserve divisor
						BL 			InitialEstimate			//calls InitialEstimate passing in R0
f1:
						MOV			R1,R0					//makes copy of R0 to R1 (prev = curr)
						SMULL		R2,R3,R4,R1				//multiplies R4 with R1 into a 64 bit; multiply Q16 x Q16
						LSR 		R2,R2,16				//extract middle 32 bits
						ORR 		R2,R2,R3,LSL 16			//saves middle 32 bits into R2
						RSB 		R2,R2,131072			//subtracts R2 from (2 * 65536 + 0.5)  = temp
						SMULL		R2,R3,R2,R1				//multiples R1 with R2 into a 64 bit; multiply Q16 x Q16
						LSR 		R2,R2,16				//extract middle 32 bits
						ORR 		R0,R2,R3,LSL 16			//saves middle 32 bits into R0 = curr
						SUB			R2,R0,R1				//subtracts R1 from R0, saves into R2 = diff

						ADD 		R2,R2,66				//adds R2 + (0.001 * 65536 + 0.5) == 66.036
						CMP			R2,132					//compares R2 with [(0.001 * 65536 + 0.5) * 2]
						BGT			f1						//if greater than, then branch to f1
						POP			{R4,PC}					//pop PC,R4,R5,R6, and return
						.end

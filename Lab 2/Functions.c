#include <stdint.h>

 // Converts bits to an unsigned integer
 uint32_t Bits2Unsigned(int8_t bits[8]) {
	uint32_t n = 0; // initializes n to 0
	
	for(int i = 7; i >= 0; i--) { //loop backwards thru the bits array
		n = 2 * n + bits[i]; // compute the value associated from the binary bits
	}
	return n; // returns value
 }
 
 // Converts bits to a signed integer
 int32_t Bits2Signed(int8_t bits[8]) {
	int32_t n = 0; //initializes n to 0
	
	if (bits[7] == 1) { //if the most significant bit is a 1 = value should be negative
		n = Bits2Unsigned(bits); // computes the positive value
		
		return (n - 256); //subtracts 256 from the computed positive value, which makes the most significant bit negative in polynomial evaulation
						  //(if most significant bit == 1, then its negative. So in this case (2^7)*1 = 128, thus subtracting 256 makes -128) 
	}
	else { // the most significant bit is a 0 = value is positive
		n = Bits2Unsigned(bits); // uses above polyonomial evaulation
		return n; // returns value
	}
 }
 
 //Increments an array of bits
 void Increment(int8_t bits[8]) {
	for(int i = 0; i < 8; i++) { //loop from beginning to end
		if(bits[i] == 1) { // if a bit is 1 means adding 1 will cause a carry over
			bits[i] = 0; // which equals to setting that bit to 0
		}
		else { // when it hits a bit that is 0, then add 1 will not cause carry over
			bits[i] = 1; // which equals to setting that bit to 1
			break; // break because the bit has been correctly incremented by 1
		}
	}
 }

// Converts an unsigned integer into an array of bits 
 void Unsigned2Bits(uint32_t n, int8_t bits[8]) {
	 int number = n; //initialize number as n

	 for(int i = 0; i < 8; i++) { // loop from beginnning to end
		int remainder = number % 2; // checks if there's a remainder when dividing by 2 = repeated division
		number = number / 2; // gets the whole number part of the division
		
		if (remainder == 1) { // if there was a remainder,
			bits[i] = 1; // the bit at i is 1
		}
		else { // if the remainder is 0
			bits[i] = 0; // the bit at i is 0
		}
	 }
 }
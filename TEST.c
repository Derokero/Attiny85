/*
 * TEST.c
 *
 * Created: 22.08.2019 14:53:40
 *  Author: Derokero
 */ 


#define F_CPU 1000000	// 1MHz
#include <avr/io.h>
#include "LCD_I2C.h"

uint8_t debounce(uint8_t PBx);
char *intToStr(uint8_t num);

int main(){
	
	// Set PB2 to input, enable internal pullup
	MCUCR &= ~(1<<PUD);
	DDRB &= ~(1<<PB2);
	PORTB |= (1<<PB2);
	
	
	START();
	sendAddress(0x27, 0);
	
	initLCD();
	sendToLCD(0b00101000, 0, 0);	// Display and font
	sendToLCD(0b00001100, 0, 0);	// Display Control: Turn on display
	sendToLCD(0b00000001, 0, 0);	// Clear display
	sendToLCD(0b00000110, 0, 0);	// Entry Mode: Increment
	sendToLCD(0b10000000, 0, 0);	// Set DDRAM to 0x00
	
	uint8_t counter = 0, block = 0;
	
	while(1){
			if(!debounce(PB2) && !block){
				block = 1;
				LCD_Write(intToStr(counter++));
				sendToLCD(0b10000000, 0, 0);	// Set DDRAM to 0x00
			}
			else if(debounce(PB2) && block){
				block = 0;
			}	
		}
		
	STOP();
}

// Integer (uint8_t) to string
char *intToStr(uint8_t num){
	static char str[10];		// Define string buffer
	volatile uint8_t count = 0, temp = num;
	
	// Count number of digits (using do-while because number of digits can't be 0)
	do{
		temp /= 10;
		count++;
	}while(temp != 0);
	
	// Convert to string
	for(uint8_t i = 0; i < count; i++){
		str[(count-1) - i] = (num % 10) + '0';		// Get most significant digit and convert to char
		num /= 10;
	}
	
	return str;		// Return string
}

// Debounce using software delay (counter)
uint8_t debounce(uint8_t PBx){
	uint16_t FilterHigh = 0, FilterLow = 0;		// Counters
	
	while(1){
		// Check if high
		if((PINB & (1<<PBx))){
			FilterLow = 0;		// Reset low counter
			FilterHigh++;
			
			if(FilterHigh >= 2500)	// Threshold
				return 1;		// Input should be high
		}
		
		// Check if low
		if(!(PINB & (1<<PBx))){
			FilterHigh = 0;		// Reset high counter
			FilterLow++;
			
			if(FilterLow >= 2500)	// Threshold
				return 0;		// Input should be low
		}		
	}
}

/*
uint8_t debounce(uint8_t PBx){
	volatile uint8_t Filter;
	for(uint8_t i = 0; i < 2; i++){
		
		Filter = 1;
		
		while(Filter < 0xff && Filter > 0x00){
			Filter <<= 1;
			Filter |= (PINB & (1<<PBx)) >> PBx;
		}	
		
	}

	return Filter;
}
*/

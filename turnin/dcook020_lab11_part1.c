/*	Author: Daniel Cook - Dcook020@ucr.edu
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://www.youtube.com/watch?v=67-NIArvJ7U
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif

unsigned char output; 
unsigned char aval;

void transmit_data(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
   	 // Sets SRCLR to 1 allowing data to be set
   	 // Also clears SRCLK in preparation of sending data
   	 PORTC = 0x08;
   	 // set SER = next bit of data to be sent.
   	 PORTC |= ((data >> i) & 0x01);
   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
   	 PORTC |= 0x02;  
    }
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    PORTC |= 0x04;
    // clears all lines in preparation of a new transmission
    PORTC = 0x00;
}

enum NumChangeStates{NumChangeStart, NumChangeWait, NumChangeVal, NumChangeReset}numchangestate;

void NumChangeTick(){
	switch(numchangestate){
		case NumChangeStart:
		numchangestate = NumChangeWait;
		break;

		case NumChangeWait:	
		aval = (~PINA&0x03);
		if(aval == 0x01){
		if(output<0xFF){
		output++;
		transmit_data(output);
		}	
		numchangestate = NumChangeVal;
		}else if(aval == 0x02){
		if(output>0){
		output--;
		transmit_data(output);
		}	
		numchangestate = NumChangeVal;
		}else if(aval == 0x03){
		output = 0x00;
		transmit_data(output);	
		numchangestate = NumChangeReset;
		}else{
		numchangestate = NumChangeWait;
		}
		break;

		case NumChangeVal:
		aval = (~PINA&0x03);
		if(aval == 0x00){
		numchangestate = NumChangeWait;
		}else if(aval == 0x03){
		output = 0x00;
                transmit_data(output);
                numchangestate = NumChangeReset;
		}else{
		numchangestate = NumChangeVal;
		}
		break;

		case NumChangeReset:
		output = 0x00;
                transmit_data(output);
		aval = (~PINA&0x03);
		if(aval == 0x00){	
		numchangestate = NumChangeWait;
		}else{
		numchangestate = NumChangeReset;
		}
		break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRC = 0xFF; PORTC = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	output = 0x00;
	numchangestate = NumChangeStart;
	TimerSet(50);
	TimerOn();
    /* Insert your solution below */
    while (1) {
	NumChangeTick(); 
     	while(!TimerFlag);
	TimerFlag = 0;	
    }
    return 1;
}

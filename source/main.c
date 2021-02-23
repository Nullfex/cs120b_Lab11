/*	Author: Daniel Cook - Dcook020@ucr.edu
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif

unsigned short output; 
unsigned char aval;
unsigned char whichregister = 0x01;


void transmit_data(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
         // Sets SRCLR to 1 allowing data to be set
         // Also clears SRCLK in preparation of sending data
        if(whichregister == 0x00){
         PORTC = 0x08;
        }else{
        PORTC = 0x20;
         }
         // set SER = next bit of data to be sent.
         PORTC |= ((data >> i) & 0x01);
         // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
         PORTC |= 0x02;
    }
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    if(whichregister == 0x00){
    PORTC |= 0x04;
    }else{
    PORTC |= 0x10;
    }
    // clears all lines in preparation of a new transmission
    PORTC = 0x00;
}


void transmit_short(unsigned short numba){
short number = numba;
char vOut = (char)number;
transmit_data(vOut);
whichregister = 0x00;
number = (number>>8);
vOut = (char)number;
transmit_data(vOut);
whichregister = 0x01;
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
		if(output<0xFFFF){
		output++;
		transmit_short(output);
		}	
		numchangestate = NumChangeVal;
		}else if(aval == 0x02){
		if(output>0){
		output--;
		transmit_short(output);
		}	
		numchangestate = NumChangeVal;
		}else if(aval == 0x03){
		output = 0x00;
		transmit_short(output);	
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
                transmit_short(output);
                numchangestate = NumChangeReset;
		}else{
		numchangestate = NumChangeVal;
		}
		break;

		case NumChangeReset:
		output = 0x00;
                transmit_short(output);
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
	output = 0x02FF;
	numchangestate = NumChangeStart;
	TimerSet(50);
	TimerOn();
	transmit_short(output);
    /* Insert your solution below */
    while (1) {
	NumChangeTick(); 
     	while(!TimerFlag);
	TimerFlag = 0;	
    }
    return 1;
}

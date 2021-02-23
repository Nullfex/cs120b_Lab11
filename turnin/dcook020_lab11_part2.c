/*	Author: Daniel Cook - Dcook020@ucr.edu
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *
 *
 *
 *	Ignore the commented out code its for part3 I forgot to copy the files
 *	so I had to go back and comment some stuff out.
 *
 *	Demo Link: https://www.youtube.com/watch?v=oFHvLLzLPcE
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif


unsigned char patternone[] = {0x81,0x042,0x24,0x18};
unsigned char patterntwo[] = {0xAA,0x55};
unsigned char patternthree[] = {0x80,0x40,0xA0,0x50,0x28,0x14,0x0A,0x05,0x02,0x01,0x00};
unsigned char setonecount = 0x00;
unsigned char setthreecount = 0x00;
unsigned char pattern = 0x00;
unsigned char onepattern =0x00;
unsigned char twopattern = 0x00;
unsigned char output; 
unsigned char aval;
unsigned char whichregister = 0x00;

void transmit_data(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
   	 // Sets SRCLR to 1 allowing data to be set
   	 // Also clears SRCLK in preparation of sending data
	// if(whichregister == 0x00){
   	 PORTC = 0x08;
	// }else{
	// PORTC = 0x20;
	// }
   	 // set SER = next bit of data to be sent.
   	 PORTC |= ((data >> i) & 0x01);
   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
   	 PORTC |= 0x02;  
    }
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    //if(whichregister == 0x00){ 
    PORTC |= 0x04;
    //}else{
    //PORTC |= 0x10;
    //}
    // clears all lines in preparation of a new transmission
    PORTC = 0x00;
}

enum NumChangeStates{NumChangeStart, NumChangeWait, NumChangeVal, NumChangeReset,NumChangeResetRelease,NumChangeResetTwo}numchangestate;

void NumChangeTick(){
	switch(numchangestate){
		case NumChangeStart:
		numchangestate = NumChangeWait;
		pattern = 0x01;
		break;

		case NumChangeWait:	
		aval = (~PINA&0x03);
		if(aval == 0x01){
		if(pattern<0x03){
		pattern++;
		}	
		numchangestate = NumChangeVal;
		}else if(aval == 0x02){
		if(pattern>1){
		pattern--;
		}	
		numchangestate = NumChangeVal;
		}else if(aval == 0x03){
		pattern = 0x00;	
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
		pattern = 0x00;
                numchangestate = NumChangeReset;
		}else{
		numchangestate = NumChangeVal;
		}
		break;

		case NumChangeReset:
		aval = (~PINA&0x03);
		if(aval == 0x00){	
		numchangestate = NumChangeResetRelease;
		}else{
		numchangestate = NumChangeReset;
		}
		break;

		case NumChangeResetRelease:
		aval = (~PINA&0x03);
                if(aval == 0x03){
		numchangestate = NumChangeResetTwo;
		pattern = 0x01;
		}else{
		numchangestate = NumChangeResetRelease;
		}
		break;

		case NumChangeResetTwo:
		aval = (~PINA&0x03);
		if(aval == 0x00){
		numchangestate = NumChangeWait;
		}else{
		numchangestate = NumChangeResetTwo;
		}
		break;

	}
}



enum PatternStates{PatternStart,PatternFirst,PatternSecond,PatternThird,PatternOff}patternstate;
void PatternTick(){
/*	
if(whichregister == 0x00){
pattern = onepattern;	
setonecount = settwocount;
}else{
pattern = twopattern;	
setonecount = setthreecount;
}
*/
	
        switch(patternstate){
		case PatternStart:
		patternstate = PatternFirst;
		setonecount = 0x00;
		break;

		case PatternFirst:
		if(pattern == 0x00){
		patternstate = PatternOff;
		setonecount = 0x00;
		output = 0x00;
		transmit_data(output);
		}else if(pattern>0x01){
		patternstate = PatternSecond;
		setonecount = 0x00;
		output = 0x00;
		transmit_data(output);
		}else{
		patternstate = PatternFirst;
		output = patternone[setonecount];
		transmit_data(output);
		if(setonecount>2){
		setonecount = 0x00;
		}else{
		setonecount++;
		}
		}
		break;

		case PatternSecond:
		if(pattern == 0x00){
                patternstate = PatternOff;
                setonecount = 0x00;
                output = 0x00;
                transmit_data(output);
                }else if(pattern<0x02){
                patternstate = PatternFirst;
                setonecount = 0x00;
                output = 0x00;
                transmit_data(output);
                }else if(pattern>0x02){
		patternstate = PatternThird;
		setonecount = 0x00;
		output = 0x00;
		transmit_data(output);
		}else{
                patternstate = PatternSecond;
                output = patterntwo[setonecount];
                transmit_data(output);
                if(setonecount>0){
                setonecount = 0x00;
                }else{
                setonecount++;
                }
                }
		break;

		case PatternThird:
		if(pattern == 0x00){
                patternstate = PatternOff;
                setonecount = 0x00;
                output = 0x00;
                transmit_data(output);
                }else if(pattern<0x03){
                patternstate = PatternSecond;
                setonecount = 0x00;
                output = 0x00;
                transmit_data(output);
                }else{
                patternstate = PatternThird;
                output = patternthree[setonecount];
                transmit_data(output);
                if(setonecount>9){
                setonecount = 0x00;
                }else{
                setonecount++;
                }
                }
		break;

		case PatternOff:
		if(pattern>0x00){
		patternstate = PatternFirst;
		}else{
		patternstate = PatternOff;	
		}
		break;

	}
/*
if(whichregister == 0x00){
onepattern = pattern;	
settwocount = setonecount;	
whichregister = 0x01;
}else{
twopattern = pattern;	
setthreecount = setonecount;	
whichregister = 0x00;

}
*/
}	

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRC = 0xFF; PORTC = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	output = 0x00;
	transmit_data(output);
	numchangestate = NumChangeStart;
	patternstate = PatternStart;
	TimerSet(100);
	TimerOn();
    /* Insert your solution below */
    while (1) {
	NumChangeTick();
       	PatternTick();
	//NumTwoChangeTick();
	//PatternTick();	
     	while(!TimerFlag);
	TimerFlag = 0;	
    }
    return 1;
}

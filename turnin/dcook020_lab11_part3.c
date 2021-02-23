/*	Author: Daniel Cook - Dcook020@ucr.edu
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #3
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
 *	Demo Link: https://www.youtube.com/watch?v=LY1nCskO2Ko
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
unsigned char settwocount = 0x00;
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

enum NumChangeStates{NumChangeStart, NumChangeWait, NumChangeVal, NumChangeReset,NumChangeResetRelease,NumChangeResetTwo}numchangestate;

void NumChangeTick(){
	switch(numchangestate){
		case NumChangeStart:
		numchangestate = NumChangeWait;
		onepattern = 0x01;
		break;

		case NumChangeWait:	
		aval = (~PINA&0x03);
		if(aval == 0x01){
		if(onepattern<0x03){
		onepattern++;
		}	
		numchangestate = NumChangeVal;
		}else if(aval == 0x02){
		if(onepattern>1){
		onepattern--;
		}	
		numchangestate = NumChangeVal;
		}else if(aval == 0x03){
		onepattern = 0x00;	
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
		onepattern = 0x00;
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
		onepattern = 0x01;
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


enum NumTwoChangeStates{NumTwoChangeStart, NumTwoChangeWait, NumTwoChangeVal, NumTwoChangeReset,NumTwoChangeResetRelease,NumTwoChangeResetTwo}numtwochangestate;

void NumTwoChangeTick(){

//I should have done the same things as I did with the pattern SM but its too late to go back now so looks like we are using two SM	

aval = (~PINA&0x0C);	
        switch(numtwochangestate){
                case NumTwoChangeStart:
                numtwochangestate = NumTwoChangeWait;
                twopattern = 0x01;
                break;

                case NumTwoChangeWait:
                if(aval == 0x04){
                if(twopattern<0x03){
                twopattern++;
                }
                numtwochangestate = NumTwoChangeVal;
                }else if(aval == 0x08){
                if(twopattern>1){
                twopattern--;
                }
                numtwochangestate = NumTwoChangeVal;
                }else if(aval == 0x0C){
                twopattern = 0x00;
                numtwochangestate = NumTwoChangeReset;
                }else{
                numtwochangestate = NumTwoChangeWait;
                }
                break;

                case NumTwoChangeVal:
                if(aval == 0x00){
                numtwochangestate = NumTwoChangeWait;
                }else if(aval == 0x0C){
                twopattern = 0x00;
                numtwochangestate = NumTwoChangeReset;
                }else{
                numtwochangestate = NumTwoChangeVal;
                }
                break;
		case NumTwoChangeReset:
                if(aval == 0x00){
                numtwochangestate = NumTwoChangeResetRelease;
                }else{
                numtwochangestate = NumTwoChangeReset;
                }
                break;

                case NumTwoChangeResetRelease:
                if(aval == 0x0C){
                numtwochangestate = NumTwoChangeResetTwo;
                twopattern = 0x01;
                }else{
                numtwochangestate = NumTwoChangeResetRelease;
                }
                break;

                case NumTwoChangeResetTwo:
                if(aval == 0x00){
                numtwochangestate = NumTwoChangeWait;
                }else{
                numtwochangestate = NumTwoChangeResetTwo;
                }
                break;

        }
}




enum PatternStates{PatternStart,PatternFirst,PatternSecond,PatternThird,PatternOff}patternstate,patternonestate,patterntwostate;


void PatternTick(){

//This is terrible code but it works 
if(whichregister == 0x00){
pattern = onepattern;	
setonecount = settwocount;
patternstate = patternonestate;
}else{
pattern = twopattern;	
setonecount = setthreecount;
patternstate = patterntwostate;
}


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

if(whichregister == 0x00){
onepattern = pattern;	
settwocount = setonecount;
patternonestate = patternstate;
whichregister = 0x01;
}else{
twopattern = pattern;	
setthreecount = setonecount;
patterntwostate = patternstate;	
whichregister = 0x00;
}

}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRC = 0xFF; PORTC = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	output = 0x00;
	transmit_data(output);
	numchangestate = NumChangeStart;
	//patternstate = PatternStart;
	patternonestate = PatternStart;
	TimerSet(100);
	TimerOn();
    /* Insert your solution below */
    while (1) {
	NumChangeTick();
       	PatternTick();
	NumTwoChangeTick();
	PatternTick();	
     	while(!TimerFlag);
	TimerFlag = 0;	
    }
    return 1;
}

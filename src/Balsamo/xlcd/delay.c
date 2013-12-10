#include "xlcd.h"
#include "delay.h"
#include "..\\common.h"

#ifdef _RD10

/******************************************************************************
    This file provides the delay routines required XLCD operation.

The following are the delay routines provided: 
    1. DelayEPulseWidth - Delay for width of the E pulse
    2. DelayXLCD    - Delay for atleast 5msec 
    3. Delay600nsXLCD - Delay for 100Tcy
    4. DelayPORXLCD - Delay for atleast 15msec

The loop counts used in these routines needs to be changed based on the 
exact clock frequency.
******************************************************************************/

// TODO: Poner conteos correctos


void DelayEPulseWidth(void)   	/* provides delay for 18Tcy */
{
	/* This is the width of the E signal		*/
	/* required by the LCD controlller			*/
	SwDelayNCyc(DELAY_NS_COUNT(230));
/*	int i;
	for(i=0;i<18;i++)
		asm("clrwdt");*/
}

void DelayPORXLCD(void) 	/* provides delay for atleast 15msec */
{
    /* Give a delay of atleast 15 msec after power up	*/
   
    SwDelayNCyc(DELAY_MS_COUNT(15));
/*   long i;
    for(i=0;i < 2000000;i++)
        asm("clrwdt");*/
}

void DelayXLCD(void)  		/* provides delay for atleast 5msec */
{
    SwDelayNCyc(DELAY_MS_COUNT(5));
/*  long i;
    for(i=0;i < 200000;i++)
        asm("clrwdt");*/
}

void Delay600nsXLCD(void)		/* provides delay for 600ns */
{
	/* This delay is required during the read operation	*/
	
    SwDelayNCyc(DELAY_NS_COUNT(600));
/*    int i;
    for(i=0;i<10000;i++)
        asm("clrwdt");*/
}

//For 33F and 24H devices

void Delay200usXLCD(void)	/* provides delay for 200uS */
{
    SwDelayNCyc(DELAY_US_COUNT(200));
	
/*    int i;
    for(i=0;i<8000;i++)
        asm("clrwdt");*/
}



#else
#warning "Does not build on this target"
#endif

#include "xlcd.h"
#include "delay.h"

#ifdef _RD10

/*******************************************************************
Function Name:          setCGRamAddr
Description:            This function sets the character generator 
                        address of the LCD controller.The user must 
                        first check if the controller is busy or not 
                        by calling the busyXLCD() function.
Input parametes:        char address
Return value:           void
********************************************************************/

void SetCGRamAddr( char address)
{
    RW_PIN = 0;  /* enable write */
    RS_PIN = 0;  /* Select Instruction Reg */

    /* configure the common data pins as output */
    TRIS_DATA_PIN_7 = 0;
    TRIS_DATA_PIN_6 = 0;
    TRIS_DATA_PIN_5 = 0;
    TRIS_DATA_PIN_4 = 0;
    
    DATA_PIN_7 = 0;

    /* DB6 must be 1 for setting CGRam ADDR */
    DATA_PIN_6 = 1;
    
    /* address is only of 6 bits in length */
    DATA_PIN_5 = (unsigned int)((address & 0x20)>>5);
    DATA_PIN_4 = (unsigned int)((address & 0x10)>>4);
    
# ifdef EIGHT_BIT_INTERFACE
    /* configure the other pins as output */
    TRIS_DATA_PIN_3 = 0;
    TRIS_DATA_PIN_2 = 0;
    TRIS_DATA_PIN_1 = 0;
    TRIS_DATA_PIN_0 = 0;

    DATA_PIN_3 = (unsigned int)(( address & 0x08)>>3);
    DATA_PIN_2 = (unsigned int)(( address & 0x04)>>2);
    DATA_PIN_1 = (unsigned int)(( address & 0x02)>>1);
    DATA_PIN_0 = (unsigned int)(( address & 0x01)>>0);

    E_PIN = 1;
    DelayEPulseWidth();
    E_PIN = 0;
# else
    E_PIN = 1;
    DelayEPulseWidth();
    E_PIN = 0;

    RW_PIN = 0;  /* enable write */
    RS_PIN = 0;  /* Instruction Reg selected */

    DATA_PIN_7 = (unsigned int)((address & 0x08)>>3);
    DATA_PIN_6 = (unsigned int)((address & 0x04)>>2);
    DATA_PIN_5 = (unsigned int)((address & 0x02)>>1);
    DATA_PIN_4 = (unsigned int)((address & 0x01)>>0);
    E_PIN = 1;
   DelayEPulseWidth();
    E_PIN = 0;
#endif

    /*configure all pins to input mode */
    TRIS_DATA_PIN_7 = 1;
    TRIS_DATA_PIN_6 = 1;
    TRIS_DATA_PIN_5 = 1;
    TRIS_DATA_PIN_4 = 1;
# ifdef EIGHT_BIT_INTERFACE
    TRIS_DATA_PIN_3 = 1;
    TRIS_DATA_PIN_2 = 1;
    TRIS_DATA_PIN_1 = 1;
    TRIS_DATA_PIN_0 = 1;
#endif
	while(BusyXLCD());
} /* end of function */

#else
#warning "Does not build on this target"
#endif

#include "xlcd.h"
#include "delay.h"

#ifdef _RD10

/*******************************************************************
Function Name   :   ReadAddrXLCD
Description     :   This function reads a byte which is a 7 bit 
                    address and the BF forms the eight bit .
                    The user must first check the busy status 
                    of the LCD cntrlr by calling the BusyXLCD()
Input parameters:   none
Return value    :   char address
********************************************************************/

char ReadAddrXLCD(void)
{
    char address = 0;
    char temp[7];
	unsigned char i;

    /* Configure all pins as input */
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
    
    RW_PIN = 1;                 /* for read operation */
    RS_PIN = 0;                 /* for selecting AC for read */
    DelayEPulseWidth();
    E_PIN = 1;                  /* for enabling read/write */
  DelayEPulseWidth();

    temp[6] = (char) READ_PIN_6;
    temp[6] = temp[6] << 6;
    temp[5] = (char) READ_PIN_5;
    temp[5] = temp[5] << 5;
    temp[4] = (char) READ_PIN_4;
    temp[4] = temp[4] << 4;

#ifdef EIGHT_BIT_INTERFACE
    temp[3] = (char) READ_PIN_3;
    temp[3] = temp[3] << 3;
    temp[2] = (char) READ_PIN_2;
    temp[2] = temp[2] << 2;
    temp[1] = (char) READ_PIN_1;
    temp[1] = temp[1] << 1;
    temp[0] = (char) READ_PIN_0;
    E_PIN = 0;                  /* disable read/write */
#else
    E_PIN = 0;                  /* disable read/write */

    RW_PIN = 1;                 /* for read operation */
    RS_PIN = 0;                 /* for selecting AC for read */
    E_PIN = 1;                  /* for enabling read/write */
   DelayEPulseWidth();
    temp[3] = (char) READ_PIN_7;
    temp[3] = temp[3] << 3;
    temp[2] = (char) READ_PIN_6;
    temp[2] = temp[2] << 2;
    temp[1] = (char) READ_PIN_5;
    temp[1] = temp[1] << 1;
    temp[0] = (char) READ_PIN_4;
    E_PIN = 0;                  /* disable read/write */
#endif
    for (i=0;i<8;i++)
    {
        address |= temp[i];
    }

    return address;
} 

#else
#warning "Does not build on this target"
#endif

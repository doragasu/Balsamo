#include "xlcd.h"
#include "delay.h"

#ifdef _RD10

/*********************************************************************
Function Name:             BusyXLCD
Description:               This function returns the busy status
                           of the LCD Controller.The Busy flag is
                           is MS bit when the Instruction Reg is read
Input parameters:          none
Return value:              char- returns 1 if Busy flag is set, else 0
*********************************************************************/

char BusyXLCD(void)
{
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

     RW_PIN = 1;           /* for read operation */
     RS_PIN = 0;           /* for selecting AC for read */
     DelayEPulseWidth();

#ifdef EIGHT_BIT_INTERFACE
     E_PIN = 1;            /* for enabling read/write */
   DelayEPulseWidth();
    

     if(READ_PIN_7)        /* busy flag */
     {
         E_PIN = 0;
         Delay600nsXLCD();
          return 1;
     }
     else
     {
          E_PIN = 0;
         Delay600nsXLCD();
          return 0;
     }
#else

 /* FOUR_BIT_INTERFACE   */ 

     E_PIN = 1;            /* for enabling read/write */
     DelayEPulseWidth();

     if(READ_PIN_7)        /* busy flag */
     {
          E_PIN = 0;       /* disable read/write */
          return 1;
     }
     else
     {
          E_PIN = 0;       /* disable read/write */
          return 0;
     }
#endif
} /* end of function */

#else
#warning "Does not build on this target"
#endif

/********************************************************************/
/*            Header for XLCD module library functions for          */
/*            P-tec PCOG1602B LCD controller                        */
/*------------------------------------------------------------------*/
/* Modificado por Jesús Alonso para su uso con la placa BALSAMO 1.0 */
/* y el display NHD-0216HZ-FSW-FBW-3V3C basado en el controlador    */
/* SPLC780D. Adapted for Rev.B of BALSAMO PCB.                      */
/********************************************************************/

#if defined(__dsPIC30F__)
#include <p30fxxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#elif defined(__PIC24H__)
#include <p24Hxxxx.h>
#endif

#if defined(_RD10)

#ifndef _XLCD_H
#define _XLCD_H

/* Comment out the following line if 4-bit interface is being used */
#define EIGHT_BIT_INTERFACE

/* #defines of the data pins and the corresponding tris pins */
/* The READ_PIN_X #defines must match the corresponding DATA_PIN_X defines */
#define DATA_PIN_7      LATBbits.LATB7
#define DATA_PIN_6      LATBbits.LATB8
#define DATA_PIN_5      LATBbits.LATB9
#define DATA_PIN_4      LATBbits.LATB10

#define READ_PIN_7		PORTBbits.RB7
#define READ_PIN_6		PORTBbits.RB8
#define READ_PIN_5		PORTBbits.RB9
#define READ_PIN_4		PORTBbits.RB10

#ifdef EIGHT_BIT_INTERFACE
#define DATA_PIN_3      LATBbits.LATB11
#define DATA_PIN_2      LATBbits.LATB12
#define DATA_PIN_1      LATBbits.LATB13
#define DATA_PIN_0      LATBbits.LATB14

#define READ_PIN_3		PORTBbits.RB11
#define READ_PIN_2		PORTBbits.RB12
#define READ_PIN_1		PORTBbits.RB13
#define READ_PIN_0		PORTBbits.RB14


#endif

#define TRIS_DATA_PIN_7       TRISBbits.TRISB7
#define TRIS_DATA_PIN_6       TRISBbits.TRISB8
#define TRIS_DATA_PIN_5       TRISBbits.TRISB9
#define TRIS_DATA_PIN_4       TRISBbits.TRISB10

#ifdef EIGHT_BIT_INTERFACE
#define TRIS_DATA_PIN_3       TRISBbits.TRISB11
#define TRIS_DATA_PIN_2       TRISBbits.TRISB12
#define TRIS_DATA_PIN_1       TRISBbits.TRISB13
#define TRIS_DATA_PIN_0       TRISBbits.TRISB14
#endif

#if defined(_RD8) && defined(_RD9) && defined(_RD10)

/* #defines of the control pins and the corresponding tris pins for 30F */
#define BL_PIN                LATBbits.LATB6       /* Backlight del LCD */
#define E_PIN                 LATBbits.LATB15      /* PORT for E */
#define RW_PIN                LATDbits.LATD14      /* PORT for RW */
#define RS_PIN                LATDbits.LATD15      /* PORT for RS */

#define TRIS_BL               TRISBbits.TRISB6    /* TRIS del BL */
#define TRIS_E                TRISBbits.TRISB15   /* TRIS for E */
#define TRIS_RW               TRISDbits.TRISD14   /* TRIS for RW */
#define TRIS_RS               TRISDbits.TRISD15   /* TRIS for RS */

#endif

/* Display ON/OFF Control defines */
#define DON                   	0x0F 	/* Display on     		*/
#define DOFF                  	0x0b 	/* Display off     		*/
#define CURSOR_ON           	0x0f  	/* Cursor on       		*/
#define CURSOR_OFF          	0x0d  	/* Cursor off      		*/
#define BLINK_ON              	0x0f  	/* Cursor Blink    		*/
#define BLINK_OFF             	0x0e  	/* Cursor No Blink 		*/

/* Some Command Type defines							*/
#define CLEAR_XLCD		0x01		/* Clears the LCD		*/
#define RETURN_CURSOR_HOME	0x02		/* Returns the cursor to the HOME position	*/


/* Entry Mode Options	*/
#define INCR_MODE		0x07		/* Increment mode		*/
#define DECR_MODE		0x05		/* Decrement Mode	*/
#define ENTIRE_SHIFT_OFF	0x06		/* Shift Entire Display 	*/
#define ENTIRE_SHIFT_ON	0x07		/* Dont Shift Entire Display	*/

/* Cursor or Display Shift defines */
#define SHIFT_CUR_LEFT        0x13  /* Cursor shifts to the left   */
#define SHIFT_CUR_RIGHT       0x17  /* Cursor shifts to the right  */
#define SHIFT_DISP_LEFT       0x1b  /* Display shifts to the left  */
#define SHIFT_DISP_RIGHT      0x1f  /* Display shifts to the right */

/* Function Set defines */
#define FOUR_BIT              0x2f  /* 4-bit Interface  */
#define EIGHT_BIT             0x3f  /* 8-bit Interface  */

/* defines for no of lines */
#define SINGLE_LINE           0x37  /* Single line display  */
#define TWO_LINE              0x3F  /* Two line display  */

/* defines for segment data transfer direction */
#define SEG1_50_SEG51_100     0x39  /* SEG1->SEG50 ,SEG51->SEG100 */
#define SEG1_50_SEG100_51     0x3d  /* SEG1->SEG50 ,SEG100->SEG51 */
#define SEG100_51_SEG50_1     0x3b  /* SEG100->SEG51,SEG50->SEG1 */
#define SEG100_51_SEG1_50     0x3f  /* SEG100->SEG51,SEG1->SEG50 */

/* defines for COM data transfer direction */

#define COM1_COM16            0x3e  /* Data transfer direction is COM1 to COM16 */

#define COM16_COM1            0x3f  /* Data transfer direction is COM16 to COM1 */

/* Function prototypes */

/* OpenXLCD
 * Configures I/O pins for external LCD
 */
void OpenXLCD(char) __attribute__ ((section (".libperi")));

/* SetCGRamAddr
 * Sets the character generator address
 */
void SetCGRamAddr(char) __attribute__ ((section (".libperi")));

/* SetDDRamAddr
 * Sets the display data address
 */
void SetDDRamAddr(char) __attribute__ ((section (".libperi")));

/* BusyXLCD
 * Returns the busy status of the LCD
 */
char BusyXLCD(void)    __attribute__ ((section (".libperi")));

/* ReadAddrXLCD
 * Reads the current address
 */
char ReadAddrXLCD(void) __attribute__ ((section (".libperi")));

/* ReadDataXLCD
 * Reads a byte of data
 */
char ReadDataXLCD(void) __attribute__ ((section (".libperi")));

/* WriteCmdXLCD
 * Writes a command to the LCD
 */
void WriteCmdXLCD(char) __attribute__ ((section (".libperi")));

/* WriteDataXLCD
 * Writes a data byte to the LCD
 */
void WriteDataXLCD(char) __attribute__ ((section (".libperi")));

/* putsXLCD
 * Writes a string of characters to the LCD
 */
void PutsXLCD(char *) __attribute__ ((section (".libperi")));

// Encender el backlight del LCD
#define BacklightOn()	BL_PIN = 1;
#define BacklightOff()	BL_PIN = 0;

#define putrsXLCD   PutsXLCD
#define getcXLCD    ReadDataXLCD
#define putcXLCD    WriteDataXLCD

/* prototypes of three delay routines used in XLCD functions */
void DelayEPulseWidth()   __attribute__ ((section (".libperi")));
void DelayPORXLCD() __attribute__ ((section (".libperi")));
void DelayXLCD()    __attribute__ ((section (".libperi")));
void Delay600nsXLCD() __attribute__((section (".libperi")));
void Delay200usXLCD(void) __attribute__((section (".libperi")));
#endif

#endif

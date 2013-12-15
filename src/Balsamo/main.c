/************************************************************************//**
 * \file main.c
 * \brief BALSAMO: Bloqueador Avanzado de Llamadas Sistemáticas, Automáticas,
 * Molestas y Ocultas.
 *
 * This project allows to block and automatically answer incoming calls.
 * Calls can be filtered using either a blacklist or a whitelist with
 * telephone number. Also unidentified callers can be either allowed or
 * blocked. An acoustic message can be played to filtered calls, and other
 * different message can be used for unallowed unidentified callers.
 *
 * \author Jesus Alonso Fernandez (doragasu)
 * \date 2013
 * \todo Add a way to unmount SD card
 * \license GPL-3.0+ <http://www.gnu.org/licenses/gpl.html>
 *****************************************************************************/
/* This file is part of BALSAMO source package.
 *
 * BALSAMO is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Some open source application is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BALSAMO.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common.h"
#include <p30F6014.h>
#include <string.h>
#include "system.h"
#include "line_if.h"
#include "cid.h"
#include "leds.h"
#include "keyb.h"
#include "adc.h"
#include "user_if.h"
#include "ext_uart.h"
#include "tim_evt.h"
#include "fsk_dem.h"
#include "rtc.h"
#include "fatfs/ff.h"
#include "fatfs/diskio.h"
#include "rtc.h"
#include "tel_filt.h"
#include "utils.h"
#include "rawplay/rawplay.h"

// Uncomment to output debug messages to the serial port
//#define _DEBUG
/// Timeout in seconds for receiving CID data since the first RING pattern
#define TIM_TOUT		5
/// Timeout in seconds between RING patterns
#define RING_WAIT_TIM	3
/// Sleep timeout in seconds, mostly used to keep ON the backlight some
/// seconds, and ensure data gets flushed to the SD card.
#define SLEEP_TOUT		5

/// Timer number used for system events (for tim_evt module)
#define SYS_EVT_TIM		0
/// Timer used for the sleep timer (for tim_evt module)
#define SLEEP_EVT_TIM	1

/// Filename of the message to be played for filtered calls
#define FILE_MSG_FILTERED		"FILTER.RAW"
/// Filename of the message to be played for forbidden unidentified calls
#define FILE_MSG_FORBIDDEN		"FORBID.RAW"

/// Puts a character in the UART
#define Put(char)			\
{ 							\
	while(U1STAbits.UTXBF); \
	U1TXREG = (char);		\
}

/// Converts a two byte ascii number to decimal
#define A2Dec(upper, lower)	(((upper)-'0')*10+((lower)-'0'))

// Private prototypes
void SysInit(void);
void CallProcEnd(void);
void TimEvtWait(unsigned int countMs);
char ParseMessages(void);
void FatFsHwInit(void);
void SysFsm(void);
void Log(char str[]);
void LogNumStr(char num[], char str[]);

/// Line 1 of the welcome message
static const char line1[] = "BALSAMO HW Rev.B";
/// Line 2 of the welcome message
static const char line2[] = "BALSAMO FW v0.8 ";
/// Buffer used to temporary store the received telephone number line
static char telNum[17];

/// When going to LPM, if sleep is TRUE, system will Sleep.
/// If false, system will Idle instead.
static char sleep = TRUE;
/// FatFs status
static BYTE fatFsStat;
/// FatFs volume
static FATFS vol;
/// Log file
static FIL fLog;
/// If TRUE, events will be logged to BALSAMO.LOG file
static char evLog;
/// System status
SysStat  sysStat  = SYS_SLEEP;
/// Last occured system event
SysEvent sysEvent;

/// Hex character set, used for binary to hexadecimal conversion
static const char hex[17] = "0123456789ABCDEF";

/// Main function. It just initializes system, and then Sleeps/Idles most of
/// the time. When awake, it just loops on the system state machine SysFsm().
int main(void)
{
	/// Initialize system
	SysInit();

	/// System loop
	while(TRUE)
	{
		///- Get an event from the system queue.
		sysEvent = SysQueueGet();
		switch (sysEvent)
		{
			///- If no event to process, Sleep/Idle again
			case SYS_NONE:
				// Nothing to do, just Sleep or Idle depending on status
				if (sleep) {Sleep();}
				else Idle();
				break;

			///- If a SLEEP event is received, go to Sleep mode
			case SYS_SLEEP_TIM:
				// Power backlight OFF and go to Sleep mode
				BacklightOff();
				// Wait until TMR1 != 0 (see 12.12.1 in the datasheet)
				while (!TMR1);
				sleep = TRUE;
				Sleep();
				break;

			///- Any other event must be handled by the system state machine
			default:
				SysFsm();
		} // switch
	} // while(TRUE)
} // main

/************************************************************************//**
 * System state machine. Processes incoming events and executes actions
 * depending on the system status and the received event.
 ****************************************************************************/
void SysFsm(void)
{
	/// ADC raw data buffer
	fractional* dataBuf;
	/// Demodulated bytes buffer
	BYTE recvBuf[NS/FSK_SPB/10 + 1];
	/// Handles return codes
	static BYTE reason = 0;
	/// Lenght of the received packet
	int recvLen;
#ifdef _DEBUG
	/// Used for some debug loops
	int i;
#endif

	// Process the event depending on the system status
	switch(sysStat)
	{
		case SYS_SLEEP:
			switch(sysEvent)
			{
				case SYS_RING:
					// RING received, launch 500 ms wait timer
					TimEvtRun(SYS_EVT_TIM, 500);
					// Turn backlight ON
					BacklightOn();

					// Turn ON the RING LED
					SetD201(LED_ON);
					// Switch to the ring timer wait status
					sysStat = SYS_RING_TIM;
					// Disable sleep. We will idle instead because
					// we need the timer clock to be enabled
					sleep = FALSE;
					/// Pass event to user interface
					UifEventParse(SYS_RING, NULL, 0);
					break;

				case SYS_KEY_UP:
				case SYS_KEY_DOWN:
				case SYS_KEY_ENTER:
				case SYS_KEY_ESC:
				case SYS_KEY_FN:
					// Key events temporary turn ON backlight and are handled
					// by the user interface state machine UifEventParse()
					BacklightOn();
					UifEventParse(sysEvent, NULL, 0);
					sleep = FALSE;
					TimEvtRun(SLEEP_EVT_TIM, SLEEP_TOUT * 1000);
					break;

				case SYS_RTC_MINUTE:
					// Call UI FSM to refresh date and time count
					UifEventParse(sysEvent, NULL, 0);
				default:
					break;
			}// switch(sysEvent)
			break;

		case SYS_RING_TIM:
			switch(sysEvent)
			{
				case SYS_TIM_EVT:
					// Completed 500 ms wait.
					// D202 will blink when receiving ADC data
					SetD202(LED_ON);
					// Launch FSK pattern timeout
					sysEvent = SYS_NONE;
					TimEvtRun(SYS_EVT_TIM, TIM_TOUT * 1000);
					// Start ADC
					AdcStart();
					// Switch to ADC data receive status
					sysStat = SYS_DATA_RECV;
					break;
				default:
					break;
			}// switch(sysEvent)
			break;

		case SYS_DATA_RECV:
			switch(sysEvent)
			{
				case SYS_DATA:
					// Blink D202
					ToggleD202();
					// Get and demodulate received audio data
					dataBuf = AdcGetBuf();
					recvLen = FskDemod(dataBuf, recvBuf);
#ifdef _DEBUG
					for (i = 0; i < recvLen; i++) Put(recvBuf[i]);
#endif
					// Handle demodulated data to the CID decoder
					if (recvLen)
					{
						switch(CidParse(recvBuf, recvLen))
						{
							case CID_OK:
								// OK, but still not finished, continue
								break;

							case CID_ERROR:
								// Error, end call process and idle
								CallProcEnd();
								break;

							case CID_END:
								// CID finished. Parse received messages
								switch (reason = ParseMessages())
								{
									case 0:
										// Call allowed, end process and idle
										CallProcEnd();
										LogNumStr(telNum, "ALLOWED");
										UifEventParse(SYS_CALL_ALLOWED,
											telNum, 16);
										break;

									// Reject call because of black/whitelist
									case 1:
									// Reject call because of private/unknown
									case 2:
										// Pick up
										LinePickUp();
										SetD204(LED_ON);
										sysStat = SYS_LINE_HANG_WAIT;
										/// \todo Play message from SD card
										AdcStop();
										TimEvtRun(SYS_EVT_TIM, 1 * 1000);
										LogNumStr(telNum, "BLOCKED");
										/// \todo Send message to user_if
										UifEventParse(SYS_CALL_RESTRICTED,
											telNum, 16);
										break;
								} // switch(ParseMessages())
								break;
						} // switch(CidParse(recvBuf, recvLen))
					} // if (recvLen)
					break;

				case SYS_TIM_EVT:
					// Timeout end call process and idle/sleep
					CallProcEnd();
					break;

				case SYS_RING:
					// Another ring pattern received, so the FSK data was
					// not sent, or we missed it, end call and idle/sleep
					sysStat = SYS_RING_END_WAIT;
					AdcStop();
					/// \todo Inform UIF module
					XLCD_CLEAR();
					XLCD_PUTS("NOT SENT!");
					TimEvtRun(SYS_EVT_TIM, RING_WAIT_TIM * 1000);
					Log("NOT SENT!");
					break;
				default:
					break;
			}// switch(sysEvent)
			break;

		case SYS_RING_END_WAIT:
			// Wait untile we stop receiving ring patterns,
			// and go back to sleep
			switch (sysEvent)
			{
				case SYS_RING:
					// Reset the ring timer
					TimEvtRun(SYS_EVT_TIM, RING_WAIT_TIM * 1000);
					break;

				case SYS_TIM_EVT:
					// We finished receiving RINGs, end call process
					CallProcEnd();
					break;
				default:
					break;
			}
			break;

		case SYS_LINE_HANG_WAIT:
			if (sysEvent == SYS_TIM_EVT)
			{
				// Play message and end call
				RawPlayFile(reason == 1?FILE_MSG_FILTERED:FILE_MSG_FORBIDDEN);
				LineHang();
				CallProcEnd();
			}
			break;
		default:
			// We should never reach here
			break;
	}// switch(sysStat)
}

/// Returns 0 if there's nothing to do, 1 if we have to filter the call
/// because of the blacklist/whitelest, and 2 if we have to filter the call
/// because the number is unknown/private.
char ParseMessages(void)
{
	/// Received message
	char *msg;
	/// Loop control variable
	int i;
	/// Received message length
	int msgLen;
	/// Decoded message code
	unsigned char msgCode;
	/// Used for error handling
	unsigned char lastErr = 0;
	/// Used to obtain function return values
	char retVal = 0;
	/// String used to print date and time
	char dateTime[17] = "DD-MM, hh:mm    ";

	/// Clear telephone number
	for (i = 0; i < 16; i++) telNum[i] = ' ';
	telNum[16] = '\0';

	/// Analyse received message code
	while ((msgCode = CidPlMsgParse(&msgLen, &msg)))
	{
		switch(msgCode)
		{
			case CID_MSG_DATE_TIME:
				/// Obtain date and time
				if (msgLen == 8)
				{
					dateTime[0]  = msg[2];
					dateTime[1]  = msg[3];
					dateTime[3]  = msg[0];
					dateTime[4]  = msg[1];
					dateTime[7]  = msg[4];
					dateTime[8]  = msg[5];
					dateTime[10] = msg[6];
					dateTime[11] = msg[7];
					/// Set the time
					RtcSetTime(A2Dec(msg[0], msg[1]),
						A2Dec(msg[2], msg[3]), A2Dec(msg[4], msg[5]),
						A2Dec(msg[6], msg[7]), 0);
				}
				else lastErr = msgCode;
				break;

			case CID_MSG_CLI_A:
			case CID_MSG_CLI_B:
				/// Obtain telephone number
				if (msgLen <= CID_TELNUM_MAX_LEN)
				{
					for (i = 0; (i < msgLen) && (i < 16); i++)
						telNum[i] = msg[i];
					telNum[i] = '\0';
					if (TfNumCheck(telNum)) retVal = 1;
				} else lastErr = msgCode;
				break;

			case CID_MSG_CLI_ABS_REASON:
				/// Obtain reason for telephone number absence
				if (msgLen == CID_CLI_ABS_REASON_LEN)
				{
					if (TfFilterHidden()) retVal = 2;
					switch(*msg)
					{
						case CID_ABS_UNAVAILABLE:
							strcpy(telNum, "UNAVAILABLE");
							break;

						case CID_ABS_PRIVATE:
							strcpy(telNum, "PRIVATE");
							break;

						default:
							strcpy(telNum, "UNKNOWN");
							break;
					}
				}
				else lastErr = msgCode;
				break;
			case CID_MSG_CP_NAME:
				// Ignored right now
				/// \todo Implement message parsing
			default:
				lastErr = msgCode;
				break;
		} // switch();
	} // while();
	/// If there were errors, prepare to print them
	if (lastErr)
	{
		dateTime[14] = hex[msgCode>>4];
		dateTime[15] = hex[msgCode&15];
	}
	return retVal;
}

/// System initialization
void SysInit(void)
{
	FRESULT retVal;

	/// Timer initialization
	TimEvtInit();
	TimEvtConfig(SYS_EVT_TIM, SYS_TIM_EVT);
	TimEvtConfig(SLEEP_EVT_TIM, SYS_SLEEP_TIM);

	/// Start RTC
	RtcStart();

	/// Initialize 2x16 LCD
#ifndef EIGHT_BIT_INTERFACE
	OpenXLCD(FOUR_BIT & TWO_LINE & SEG1_50_SEG51_100 & COM1_COM16);
#else
	OpenXLCD(EIGHT_BIT & TWO_LINE & SEG1_50_SEG51_100 & COM1_COM16);
#endif

	/// Initialize LEDs and make them blink once
	LedInit();
	SetD13(LED_ON);
	TimEvtWait(250);
	SetD13(LED_OFF);
	SetD14(LED_ON);
	TimEvtWait(250);
	SetD14(LED_OFF);
	SetD16(LED_ON);
	TimEvtWait(250);
	SetD16(LED_OFF);
	SetD15(LED_ON);
	TimEvtWait(250);
	SetD15(LED_OFF);

	BacklightOn();
	XLCD_BUSY_WAIT();
	WriteCmdXLCD(DON & CURSOR_OFF & BLINK_OFF);
	/// Write hardware and firmware version
	XLCD_PUTS(line1);
	XLCD_LINE2();
	XLCD_PUTS(line2);
	TimEvtWait(2000);
	BacklightOff();

	/// Configure UART1 for debug stdout/stdin
#ifdef _DEBUG
	ExtUart1Init();
	ExtUart1Enable();
#endif

	/// Initialise FatFs
	FatFsHwInit();
	fatFsStat = disk_initialize(0);
	if (!fatFsStat) fatFsStat = f_mount(0, &vol);
	if (fatFsStat)
	{
		XLCD_CLEAR();
		XLCD_PUTS("SD CARD DAMAGED");
		XLCD_LINE2();
		XLCD_PUTS("OR NOT INSERTED!");
		PANIC();
	}
	/// Parse configuration file in SD Card
	if (TfParseConfig())
	{
		XLCD_CLEAR();
		XLCD_PUTS("BALSAMO.CFG FILE");
		XLCD_LINE2();
		XLCD_PUTS("NOT VALID/FOUND!");
		PANIC();
	}

	/// Open log file and place cursor at its end.
	/// \warning If log file opening fails, the system will not warn user.
	retVal = f_open(&fLog, "BALSAMO.LOG", FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
	if (!retVal) retVal = f_lseek(&fLog, f_size(&fLog));
	evLog = !retVal;

	/// User interface initialization
	UifInit();
	/// Demodulation and CID interpreter initialization
	FskDemodInit();
	CidReset();
	// ADC initialization
	AdcInit();
	/// Telephone line interface initialization
	LineInit();
	LineRingDetEnable();

	/// Keypad initialization
	KeybInit();
	KeybIntsEnable();

	/// PWM player module initialization
	RawPlayInit();
}

/// End call process. Stops ADC, resets FSK demodulator and CID decoder, and
/// launches the Sleep timer.
void CallProcEnd(void)
{
	TimEvtStop(SYS_EVT_TIM);
	AdcStop();
	FskReset();
	CidReset();
	SetD13(LED_OFF);
	SetD14(LED_OFF);
	SetD15(LED_OFF);
	SetD16(LED_OFF);
	TimEvtWait(5000);
	UifEventParse(SYS_CALL_END, NULL, 0);
	sysStat = SYS_SLEEP;
	TimEvtRun(SLEEP_EVT_TIM, SLEEP_TOUT * 1000);
}

/// Waits until TIMER4 reaches a specified count value
/// \param[in] countMs Number of milliseconds to wait
void TimEvtWait(unsigned int countMs)
{
	TimEvtRun(SYS_EVT_TIM, countMs);

	do
	{
		Idle();
	} while(SysQueueGet() != SYS_TIM_EVT);
}

/// Initialises the hardware needed to drive the SD Card using FatFs library
void FatFsHwInit(void)
{
	// Set outputs for SCK2, SDO2 and SS2
	LATG  |=   BIT6 | BIT8 | BIT9;
	TRISG &= ~(BIT6 | BIT8 | BIT9);
	/// Enable internal pullup for SDI2 (CN9)
	CNPU1 |= BIT9;
}

/************************************************************************//**
 * \brief Logs a string to log file, preceded by the date and time.
 *
 * \param[in] str String to log along with num.
 ****************************************************************************/
void Log(char str[])
{
	WORD y;
	BYTE mo, d, h, mi, s;

	_DI();
	RtcGetDate(&y, &mo, &d);
	RtcGetTime(&h, &mi, &s);

	f_printf(&fLog, "%02d/%02d/%d, %02d:%02d --> %s\n", d, mo, y, h, mi, str);
	f_sync(&fLog);
}

/************************************************************************//**
 * \brief Logs a number and an action, preceded by date and time.
 *
 * \param[in] num String containing the number to log.
 * \param[in] str String to log along with num.
 ****************************************************************************/
void LogNumStr(char num[], char str[])
{
	WORD y;
	BYTE mo, d, h, mi, s;

	_DI();
	RtcGetDate(&y, &mo, &d);
	RtcGetTime(&h, &mi, &s);

	f_printf(&fLog, "%02d/%02d/%d, %02d:%02d --> %s %s\n", d, mo, y, h, mi,
			 num, str);
	f_sync(&fLog);	
}

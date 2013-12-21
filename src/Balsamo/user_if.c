/************************************************************************//**
 * \file  user_if.c
 * \brief Manages user interface. This module receives keyboard and system
 * events and controls the LCD to give the user useful information about
 * what's going on.
 *
 * \author Jesus Alonso Fernandez (doragasu)
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

#include "user_if.h"
#include "tel_filt.h"
#include "utils.h"
#include <string.h>
#include "tim_evt.h"
#include "rtc.h"
#include "types.h"

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

/// Maximum number of recent numbers in list
#define UIF_NUM_RECENT_NUMS		10

/// Code used to bounce the LCD position back one character
#define UIF_CHR_BACK			0x7F
/// Character representing the end of the string
#define UIF_CHR_END				0x2E
/// Blank (space) character
#define UIF_CHR_BLANK			0x20

/// User Interface states available
typedef enum
{
	UIF_GREETS,					///< Print greeting
	UIF_IDLE,					///< Default status
	UIF_OPT_ENABLE_DISABLE,		///< Enable/disable call filter
	UIF_OPT_ADD_LAST_NUM,		///< Add last received number to filter file
	UIF_OPT_ADD_NUM,			///< Add arbitrary number to filter file
	UIF_ADD_NUM,				///< Edit number to add to filter file
	UIF_OPT_CALL_LIST,			///< List recent received calls
	UIF_CALL_LIST,				///< Navigate recent received calls
	UIF_OPT_TEL_LIST,			///< Browse stored telephones
	UIF_TEL_LIST,				///< Navigate stored telephones
	UIF_TEL_DELETE,				///< Delete stored telephone
	UIF_OPT_YEAR_SET,			///< Set current year
	UIF_YEAR_SET,				///< Edit year to set
	UIF_INCOMING_CALL           ///< Incoming call
} UifState;

/// User interface flags. Right now only filter_enabled
typedef union
{
	unsigned char flags;                ///< Raw access to all flags
	struct
	{
		unsigned char filter_enabled:1; ///< Call filter enabled if TRUE
		unsigned char unused:7;         ///< No more flags used
	};
} UifFlags;

/// Data needed for the user to input a string
typedef struct
{
	unsigned char idx;	   ///< Index in string, used also for YES/NO
                           /// selection (0=YES, 1=NO)
	unsigned char chr;     ///< Character index (in available characters)
	unsigned char maxLen;  ///< Maximum allowed string length
	unsigned char first;   ///< Index to first availabla character
	unsigned char last;    ///< Index to last available character
	char *buf;             ///< Buffer holding the string
} UifStrEntry;

/// Holds the static data needed by the module
typedef struct
{
	UifState s;                           ///< Module state
	char recNum[UIF_NUM_RECENT_NUMS][17]; ///< Received call list (circ. buf)
	unsigned char numFirst;               ///< First num in list
	unsigned char numLast;                ///< Last num in list
	unsigned char numPos;                 ///< Position in list
	char full;                            ///< TRUE if list full
	UifFlags f;                           ///< Module flags
	UifStrEntry str;                      ///< String entry data
} UifData;

/*
static const char chWheel[12] =
{
	// 0,    1,    2,    3,    4,    5,    6,    7,    8,    9, Back,  End
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x7F, 0x2E
};
*/

/// Greeting first line
/// \todo duplicated shit in main.c, fix.
static const char greetL1[] = "BALSAMO HW v1.0 ";
/// Greeting second line
static const char greetL2[] = "BALSAMO FW v0.6 ";

/// String to indicate something is enabled (or ON)
static const char sEnabled[]  = "ON ";
/// String to indicate something is disabled (or OFF)
static const char sDisabled[] = "OFF";

/// String to indicate "YES" (or accept) option
static const char sYes[] = "YES";
/// String to indicate "NO" (or reject) option
static const char sNo[] = "NO ";

/// Module data
static UifData ud;
/// Temporal buffer to input numbers
static char sNum[14];

/// Advances one position in the received call list
/// \warning: use only for list insertions
#define UifAdvance(cursor)									\
{															\
	if (++cursor == UIF_NUM_RECENT_NUMS) cursor = 0;		\
}

/// Goes back one position in the received call list
#define UifBack(cursor)										\
{															\
	cursor = cursor?cursor - 1:UIF_NUM_RECENT_NUMS - 1;		\
}

/// Returns the number of available items in the list
#define UifAvailable()	 									\
{															\
	ud.full?UIF_NUM_RECENT_NUMS:ud.numLast >= ud.numFirst?	\
		ud.numLast - ud.NumFirst:							\
		UIF_NUM_RECENT_NUMS - ud.numFirst + ud.numLast;		\
}

/************************************************************************//**
 * \brief Prints a string in line 2 of the LCD. If the string has less than
 * 16 characters, blanks are printed until 16 characters are filled.
 *
 * \param[in] str String to print.
 ****************************************************************************/
static void UifPrintStrLine2(char* str)
{
	char length;

	if (str)
	{
		XLCD_LINE2();
		length = strlen(str);
		XLCD_PUTS(str);
		for (; length < 16; length++) XLCD_PUTC(' ');
	}
}

/************************************************************************//**
 * \brief Insert a number in the received call list. Overwrites the older
 * number if the list is full.
 *
 * \param[in] num Number to insert in the list
 ****************************************************************************/
static void UifInsert(char num[])
{
	strncpy((char*)&ud.recNum[ud.numLast], num, 16);
	UifAdvance(ud.numLast);
	if (ud.full) ud.numFirst = ud.numLast;
	else if (ud.numFirst == ud.numLast) ud.full = TRUE;
}

/************************************************************************//**
 * \brief Returns the first (i.e. oldest) number in the received call list.
 *
 * \return First number in the list, or NULL if there is none.
 ****************************************************************************/
static char *UifNumGetFirst(void)
{
	char *retVal;

	ud.numPos = ud.numFirst;

	/// Check there is at least a number
	if (!ud.full && ud.numPos == ud.numLast) return NULL;

	/// Get the number and advance
	retVal = ud.recNum[ud.numPos];
	UifAdvance(ud.numPos);

	return retVal;
}

/************************************************************************//**
 * \brief Gets next number in the received call list.
 *
 * \return Next number in the call list.
 * \warning If called with numPos pointing to the first number, it wraps
 * back to the first number.
 ****************************************************************************/
static char *UifNumGetNext(void)
{
	char *retVal;

	/// Check if we reached end of list
	if (ud.numPos == ud.numLast) return UifNumGetFirst();

	/// Get the number and advance.
	retVal = ud.recNum[ud.numPos];
	UifAdvance(ud.numPos);

	return retVal;
}

/************************************************************************//**
 * \brief Gets last (i.e. most recent) number in the received call list.
 *
 * \return Last number in the call list.
 ****************************************************************************/
static char *UifNumGetLast(void)
{
	ud.numPos = ud.numLast;

	/// Check there is at least a number
	if (!ud.full && ud.numPos == ud.numFirst) return NULL;

	/// Get the number
	UifBack(ud.numPos);

	return ud.recNum[ud.numPos];
}

/************************************************************************//**
 * \brief Gets previous number in the received call list
 *
 * \return Previous number in the call list.
 * \warning If called with numPos pointing to the first number, it wraps
 * back to the last number.
 ****************************************************************************/
static char *UifNumGetPrev(void)
{
	if (ud.numPos == ud.numFirst) return UifNumGetLast();

	UifBack(ud.numPos);
	return ud.recNum[ud.numPos];
}

/************************************************************************//**
 * \brief Prepares the interface for the user to input a string.
 *
 * \param[in] str    String buffer
 * \param[in] first  Index to the first allowed character code
 * \param[in] last   Index to the last allowed character code
 * \param[in] maxLen Maximum string length
 *
 * \warning str buffer length must be at least maxLen + 1 characters, to
 * hold the ending '\0'.
 ****************************************************************************/
void UifStrInputStart(char str[], unsigned char first, unsigned char last,
					  unsigned char maxLen)
{
	unsigned char i;

	/// Enable blinking cursor
	XLCD_CMD(DON & CURSOR_ON & BLINK_ON);
	/// Initialize the buffer holding the string
	for (i = 0; i < maxLen; i++) str[i] = first;
	/// Initialize data structure and print initial character
	ud.str.buf = str;
	ud.str.first = ud.str.chr = first;
	ud.str.last = last;
	ud.str.maxLen = maxLen;
	ud.str.idx = 0;
	XLCD_PUTC(first);
	XLCD_SHIFT_LEFT();
}

/************************************************************************//**
 * \brief Prepares the interface for the user to edit a string.
 *
 * \param[in] str    String buffer with the string to edit
 * \param[in] first  Index to the first allowed character code
 * \param[in] last   Index to the last allowed character code
 * \param[in] maxLen Maximum string length
 *
 * \warning str buffer length must be at least maxLen + 1 characters, to
 * hold the ending '\0'.
 ****************************************************************************/
void UifStrInputEdit(char str[], unsigned char first, unsigned char last,
					 unsigned char maxLen, unsigned char pos)
{
	unsigned char i;

	/// Enable blinking cursor
	XLCD_CMD(DON & CURSOR_ON & BLINK_ON);
	/// Initialize the buffer holding the string
	for (i = strlen(str); i < maxLen; i++) str[i] = first;
	str[maxLen] = '\0';
	/// Initialize data structure and print initial character
	ud.str.buf = str;
	ud.str.first = first;
	ud.str.last = last;
	ud.str.maxLen = maxLen;
	ud.str.chr = str[pos];
	ud.str.idx = pos;
	XLCD_PUTS(str);
	for (i = pos; i < strlen(str); i++) XLCD_SHIFT_LEFT();
}

/************************************************************************//**
 * \brief Changes current character one position forward
 ****************************************************************************/
void UifStrInputForward(void)
{
	unsigned char printChr;

	ud.str.chr++;
	if (ud.str.chr <= ud.str.last)
	{
		printChr = ud.str.chr;
	}
	else if (ud.str.chr == (ud.str.last + 1))
	{
		printChr = UIF_CHR_BACK;
	}
	else if (ud.str.chr == (ud.str.last + 2))
	{
		printChr = UIF_CHR_END;
	}
	else
	{
		ud.str.chr = ud.str.first;
		printChr = ud.str.first;
	}
	XLCD_PUTC(printChr);
	XLCD_SHIFT_LEFT();
}

/************************************************************************//**
 * \brief Changes current character one position backwards
 ****************************************************************************/
void UifStrInputBackward(void)
{
	unsigned char printChr;

	if (ud.str.chr == ud.str.first)
	{
		ud.str.chr = ud.str.last + 2;
		printChr = UIF_CHR_END;
	}
	else
	{
		ud.str.chr--;
		printChr = ud.str.chr == (ud.str.last + 1)?UIF_CHR_BACK:ud.str.chr;
	}

	XLCD_PUTC(printChr);
	XLCD_SHIFT_LEFT();
}

/************************************************************************//**
 * \brief Prints date and time in the LCD screen.
 ****************************************************************************/
void UifUpdateDateTime(void)
{
	// Date-time string, 16 characters + '\0'
	char dt[17];
	// Hour, minute, second, day, Month, temporal
	BYTE h, m, s, d, M;
	// year
	WORD y, tmp;

	RtcGetTime(&h, &m, &s);
	RtcGetDate(&y, &M, &d);

	// Lots of divisions, maybe I should optimize this a bit
	tmp = d / 10;
	dt[0] = '0' + tmp;
	d -= tmp * 10;
	dt[1] = '0' + d;
	dt[2] = '/';

	tmp = M / 10;
	dt[3] = '0' + tmp;
	M -= tmp * 10;
	dt[4] = '0' + M;
	dt[5] = '/';

	tmp = y / 1000;
	dt[6] = '0' + tmp;
	y -= tmp * 1000;
	tmp = y / 100;
	dt[7] = '0' + tmp;
	y -= tmp * 100;
	tmp = y / 10;
	dt[8] = '0' + tmp;
	y -= tmp * 10;
	dt[9] = '0' + y;
	dt[10] = ' ';

	tmp = h / 10;
	dt[11] = '0' + tmp;
	h -= tmp * 10;
	dt[12] = '0' + h;
	dt[13] = ':';

	tmp = m / 10;
	dt[14] = '0' + tmp;
	m -= tmp * 10;
	dt[15] = '0' + m;
	dt[16] = '\0';

	XLCD_LINE1();
	XLCD_PUTS(dt);
}

/************************************************************************//**
 * \brief Adds current character to the string.
 *
 * \return The string length if when finished, or 0 if user has not yet
 * finished entering the string.
 ****************************************************************************/
unsigned char UifStrInputEnter(void)
{
	// Check end
	if (ud.str.chr == (ud.str.last + 2))
	{
		ud.str.buf[ud.str.idx] = '\0';
		return ud.str.idx;
	}
	// Check back
	if (ud.str.chr == (ud.str.last + 1))
	{
		if (ud.str.idx > 0)
		{
			ud.str.idx--;
			ud.str.chr = ud.str.buf[ud.str.idx];
			XLCD_PUTC(UIF_CHR_BLANK);
			XLCD_SHIFT_LEFT();
			XLCD_SHIFT_LEFT();
		}
		return 0;
	}
	if (ud.str.idx < ud.str.maxLen)
	{
		// Enter number
		XLCD_SHIFT_RIGHT();
		ud.str.buf[ud.str.idx++] = ud.str.chr;
		// if max length reached, put end chr, put first chr otherwise
		if (ud.str.idx == ud.str.maxLen)
		{
			ud.str.chr = ud.str.last + 2;
			XLCD_PUTC(UIF_CHR_END);
		}
		else
		{
			ud.str.chr = ud.str.buf[ud.str.idx];
			XLCD_PUTC(ud.str.chr);
		}
		XLCD_SHIFT_LEFT();
	}
	return 0;
}

/************************************************************************//**
 * \brief Deals with initialization code and screen changes associated with
 * entering a new state.
 *
 * \param[in] ns New state for the FSM to enter.
 ****************************************************************************/
static void UifStateEnter(UifState ns)
{
	ud.s = ns;
	char *num;

	// Clear screen
	if (ns != UIF_TEL_DELETE /*&& ns != UIF_IDLE*/) XLCD_CLEAR();

	switch (ns)
	{
		case UIF_GREETS:
			// Print greetings string
			XLCD_PUTS(greetL1);
			XLCD_LINE2();
			XLCD_PUTS(greetL2);
			break;

		case UIF_IDLE:
			// Print date and time
			/// \todo Add more information in line 2?
			UifUpdateDateTime();
			break;

		case UIF_OPT_ENABLE_DISABLE:
			// Print option + option status
			XLCD_PUTS("CALL FILTER?");
			XLCD_LINE2();
			XLCD_PUTS(ud.f.filter_enabled?sEnabled:sDisabled);
			break;

		case UIF_OPT_ADD_LAST_NUM:
			// Print option + number
			XLCD_PUTS("ADD LAST NUMBER?");
			XLCD_LINE2();
			XLCD_PUTS(UifNumGetLast());
			break;

		case UIF_OPT_ADD_NUM:
			// Print option
			XLCD_PUTS("ADD NEW NUMBER?");
			break;

		case UIF_ADD_NUM:
			// Print number entry default screen
			XLCD_PUTS("ENTER NEW NUMBER");
			XLCD_LINE2();
			XLCD_CMD(DON & CURSOR_ON & BLINK_ON);
			UifStrInputStart(sNum, 0x30, 0x39, 13);
			break;

		case UIF_OPT_CALL_LIST:
			// Print option
			XLCD_PUTS("BROWSE RECENT?");
			break;

		case UIF_CALL_LIST:
			// Print the last number in the list
			XLCD_PUTS("RECENT CALL LIST");
			XLCD_LINE2();
			XLCD_PUTS(UifNumGetLast());
			break;

		case UIF_OPT_TEL_LIST:
			XLCD_PUTS("BROWSE/DEL NUMS?");
			break;

		case UIF_TEL_LIST:
			XLCD_PUTS("STORED NUM LIST");
			if ((num = TfNumGetFirst()))
			{
				XLCD_LINE2();
				XLCD_PUTS(num);
			}
			break;

		case UIF_TEL_DELETE:
			XLCD_LINE1();
			XLCD_PUTS("DELETE?: NO     ");
			break;

		case UIF_OPT_YEAR_SET:
			XLCD_PUTS("SET YEAR?");
			break;

		case UIF_YEAR_SET:
			// Print number entry default screen
			XLCD_PUTS("ENTER YEAR");
			XLCD_LINE2();
			XLCD_CMD(DON & CURSOR_ON & BLINK_ON);
			strncpy(sNum, RTC_DEF_YEAR_STR, 4);
			UifStrInputEdit(sNum, 0x30, 0x39, 4, 3);
			break;

		case UIF_INCOMING_CALL:
			// Print call message
			XLCD_CLEAR();
			XLCD_PUTS("RING!");
			break;
	}
}

/************************************************************************//**
 * \brief Deals with code associated with leaving a state.
 *
 * \param[in] os Old state we are leaving.
 ****************************************************************************/
static void UifStateLeave(UifState os)
{
	/// Nothing to do!
}

/************************************************************************//**
 * \brief Must be called each time there is a state transition. This macro
 * is just a shortcut to UifStateLeave() and UifStateEnter().
 *
 * \param[in] ns New state to enter.
 ****************************************************************************/
#define UifStateChange(ns)				\
{										\
	UifStateLeave(ud.s);				\
	UifStateEnter(ns);					\
}

/************************************************************************//**
 * \brief Module initialization. Must be called before using anything else
 * in this module.
 *
 * \return UIF_OK if everything went file.
 ****************************************************************************/
int UifInit(void)
{
	int i;

	ud.numFirst = ud.numLast = ud.numPos = 0;
	ud.full = FALSE;
	ud.f.filter_enabled = TRUE;	// Filter enabled by default
	// Start in the year set state, to avoid working with a wrong year
	UifStateEnter(UIF_YEAR_SET);
	// Just to ensure we are safe when using strncpy().
	for (i = 0; i < UIF_NUM_RECENT_NUMS; i++) ud.recNum[i][16] = '\0';

	return UIF_OK;
}

/************************************************************************//**
 * \brief Processes events while the system is idle.
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifIdleProc(SysEvent e)
{
	switch (e)
	{
		case SYS_KEY_UP:
			UifStateChange(UIF_OPT_YEAR_SET);
			break;
		case SYS_KEY_DOWN:
			UifStateChange(UIF_OPT_ENABLE_DISABLE);
			break;
		case SYS_KEY_ENTER:
			break;
		case SYS_KEY_ESC:
			break;
		case SYS_RTC_MINUTE:
			UifUpdateDateTime();
		default:
			break;
	}
}

/************************************************************************//**
 * \brief Processes events while in the call filter enable/disable screen.
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifOptEnableDisable(SysEvent e)
{
	switch (e)
	{
		case SYS_KEY_UP:
		case SYS_KEY_ESC:
			UifStateChange(UIF_IDLE);
			break;
		case SYS_KEY_DOWN:
			UifStateChange(UIF_OPT_ADD_LAST_NUM);
			break;
		case SYS_KEY_ENTER:
			XLCD_LINE2();
			if (ud.f.filter_enabled)
			{
				ud.f.filter_enabled = FALSE;
				TfDisable();
				XLCD_PUTS(sDisabled);
			}
			else
			{
				ud.f.filter_enabled = TRUE;
				TfEnable();
				XLCD_PUTS(sEnabled);
			}
			break;
		default:
			break;
	}
}

/************************************************************************//**
 * \brief Processes events while in the Add Last Number screen.
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifOptAddLastNum(SysEvent e)
{
	switch (e)
	{
		case SYS_KEY_UP:
			UifStateChange(UIF_OPT_ENABLE_DISABLE);
			break;
		case SYS_KEY_DOWN:
			UifStateChange(UIF_OPT_ADD_NUM);
			break;
		case SYS_KEY_ENTER:
			// ADD LAST NUMBER TO LIST
			TfNumAdd(ud.recNum[ud.numLast]);
			TfCfgSave();
			UifStateChange(UIF_IDLE);
			break;
		case SYS_KEY_ESC:
			UifStateChange(UIF_IDLE);
			break;
		default:
			break;
	}
}

/************************************************************************//**
 * \brief Processes events while in the Add Number screen.
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifOptAddNum(SysEvent e)
{
	switch (e)
	{
		case SYS_KEY_UP:
			UifStateChange(UIF_OPT_ADD_LAST_NUM);
			break;
		case SYS_KEY_DOWN:
			UifStateChange(UIF_OPT_CALL_LIST);
			break;
		case SYS_KEY_ENTER:
			UifStateChange(UIF_ADD_NUM);
			break;
		case SYS_KEY_ESC:
			UifStateChange(UIF_IDLE);
			break;
		default:
			break;
	}
}

/************************************************************************//**
 * \brief Processes events while in the new number input screen
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifAddNum(SysEvent e)
{
	switch (e)
	{
		case SYS_KEY_UP:
			// Digit move routine
			UifStrInputForward();
			break;
		case SYS_KEY_DOWN:
			// Digit move routine
			UifStrInputBackward();
			break;
		case SYS_KEY_ENTER:
			// Check if number complete, and add to list
			if (UifStrInputEnter())
			{
				// Add number to the list
				XLCD_CMD(DON & CURSOR_OFF & BLINK_OFF);
				TfNumAdd(ud.str.buf);
				TfCfgSave();
				UifStateChange(UIF_IDLE);
			}
			break;
		case SYS_KEY_ESC:
			// Abort number entry, return to IDLE state.
			XLCD_CMD(DON & CURSOR_OFF & BLINK_OFF);
			UifStateChange(UIF_OPT_ADD_NUM);
			break;
		default:
			break;
	}
}

/************************************************************************//**
 * \brief Processes events while in the Call List screen.
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifOptCallList(SysEvent e)
{
	switch (e)
	{
		case SYS_KEY_UP:
			UifStateChange(UIF_OPT_ADD_NUM);
			break;
		case SYS_KEY_DOWN:
			UifStateChange(UIF_OPT_TEL_LIST);
			break;
		case SYS_KEY_ENTER:
			UifStateChange(UIF_CALL_LIST);
			break;
		case SYS_KEY_ESC:
			UifStateChange(UIF_IDLE);
			break;
		default:
			break;
	}
}

/************************************************************************//**
 * \brief Processes events while browsing the call list.
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifCallList(SysEvent e)
{
	switch (e)
	{
		case SYS_KEY_UP:
			// Show prev number
			UifPrintStrLine2(UifNumGetPrev());
			break;
		case SYS_KEY_DOWN:
			// Show next number
			UifPrintStrLine2(UifNumGetNext());
			break;
		case SYS_KEY_ENTER:
			UifStateChange(UIF_IDLE);
			break;
		case SYS_KEY_ESC:
			UifStateChange(UIF_OPT_CALL_LIST);
			break;
		default:
			break;
	}
}

/************************************************************************//**
 * \brief Processes events while in the Telephone List option.
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifOptTelList(SysEvent e)
{
	switch (e)
	{
		case SYS_KEY_UP:
			UifStateChange(UIF_OPT_CALL_LIST);
			break;
		case SYS_KEY_DOWN:
			UifStateChange(UIF_OPT_YEAR_SET);
			break;
		case SYS_KEY_ENTER:
			UifStateChange(UIF_TEL_LIST);
			break;
		case SYS_KEY_ESC:
			UifStateChange(UIF_IDLE);
			break;
		default:
			break;
	}
}

/************************************************************************//**
 * \brief Processes events while browsing the telephone list.
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifTelList(SysEvent e)
{
	switch (e)
	{
		case SYS_KEY_UP:
			UifPrintStrLine2(TfNumGetPrev());
			break;
		case SYS_KEY_DOWN:
			UifPrintStrLine2(TfNumGetNext());
			break;
		case SYS_KEY_ENTER:
			UifStateChange(UIF_TEL_DELETE);
			ud.str.idx = 1;	// Set selection to "NO"
			break;
		case SYS_KEY_ESC:
			UifStateChange(UIF_OPT_TEL_LIST);
			break;
		default:
			break;
	}
}

/************************************************************************//**
 * \brief Processes events while in the Telephone delete screen.
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifTelDelete(SysEvent e)
{
	switch (e)
	{
		case SYS_KEY_UP:
		case SYS_KEY_DOWN:
			XLCD_SETPOS(9);
			if (ud.str.idx)
			{
				ud.str.idx = 0;
				XLCD_PUTS(sYes);
			}
			else
			{
				ud.str.idx = 1;
				XLCD_PUTS(sNo);
			}
			break;
		case SYS_KEY_ENTER:
			if (!ud.str.idx)
			{
				TfNumDelete();
				TfCfgSave();
			}
			UifStateChange(UIF_IDLE);
			break;
		case SYS_KEY_ESC:
			UifStateChange(UIF_IDLE);
			break;
		default:
			break;
	}
}

/************************************************************************//**
 * \brief Processes events while in the year set option screen.
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifOptYearSet(SysEvent e)
{
	switch (e)
	{
		case SYS_KEY_UP:
			UifStateChange(UIF_OPT_TEL_LIST);
			break;
		case SYS_KEY_DOWN:
			UifStateChange(UIF_IDLE);
			break;
		case SYS_KEY_ENTER:
			UifStateChange(UIF_YEAR_SET);
			break;
		case SYS_KEY_ESC:
			UifStateChange(UIF_IDLE);
			break;
		default:
			break;
	}
}

/************************************************************************//**
 * \brief Processes events while in the year set screen.
 *
 * \param[in] e Received event to process
 ****************************************************************************/
static inline void UifYearSet(SysEvent e)
{
	WORD tmp;

	switch (e)
	{
		case SYS_KEY_UP:
			// Digit move routine
			UifStrInputForward();
			break;
		case SYS_KEY_DOWN:
			// Digit move routine
			UifStrInputBackward();
			break;
		case SYS_KEY_ENTER:
			// Check number and set time if OK
			if ((tmp = UifStrInputEnter()))
			{
				if (tmp != 4)
				{
					// Years must have 4 digits, retry otherwise
					XLCD_CLEAR();
					XLCD_PUTS("WRONG YEAR, RETR")
					strncpy(sNum, RTC_DEF_YEAR_STR, 4);
					XLCD_LINE2();
					UifStrInputEdit(sNum, 0x30, 0x39, 4, 3);
				}
				else
				{
					// Set year
					XLCD_CMD(DON & CURSOR_OFF & BLINK_OFF);
					// ud.str.buf
					tmp = (ud.str.buf[0] - '0') * 1000 +
						  (ud.str.buf[1] - '0') * 100  +
						  (ud.str.buf[2] - '0') * 10   +
						  (ud.str.buf[3] - '0');
					/// \todo Check year is between 1980 and 2235
					RtcSetYear(tmp);
					UifStateChange(UIF_IDLE);
				}
			}
			break;
		case SYS_KEY_ESC:
			/// Abort number entry, return to IDLE state.
			XLCD_CMD(DON & CURSOR_OFF & BLINK_OFF);
			UifStateChange(UIF_OPT_YEAR_SET);
			break;
		default:
			break;
	}
}

/************************************************************************//**
 * \brief User interface state machine. Processes key press events, and
 * also other system events, updating user interface status accordingly.
 *
 * \param[in] sysEvt Received system event
 * \param[in] eventData Data associated to the event, or NULL if none.
 * \param[in] dataLen Length of eventData field, or 0 if no data available.
 ****************************************************************************/
void UifEventParse(SysEvent sysEvt, char eventData[], int dataLen)
{
	// Incoming call event has priority over the rest of events
	if (sysEvt == SYS_RING)
	{
		UifStateChange(UIF_INCOMING_CALL);
		return;
	}
	else if (sysEvt == SYS_CALL_END)
	{
		/// \todo: Launch a timer instead of idling immediately
		UifStateChange(UIF_IDLE);
		return;
	}

	switch (ud.s)
	{
		case UIF_GREETS:
			break;
		case UIF_IDLE:
			UifIdleProc(sysEvt);
			break;
		case UIF_OPT_ENABLE_DISABLE:
			UifOptEnableDisable(sysEvt);
			break;
		case UIF_OPT_ADD_LAST_NUM:
			UifOptAddLastNum(sysEvt);
			break;
		case UIF_OPT_ADD_NUM:
			UifOptAddNum(sysEvt);
			break;
		case UIF_ADD_NUM:
			UifAddNum(sysEvt);
			break;
		case UIF_OPT_CALL_LIST:
			UifOptCallList(sysEvt);
			break;
		case UIF_CALL_LIST:
			UifCallList(sysEvt);
			break;
		case UIF_OPT_TEL_LIST:
			UifOptTelList(sysEvt);
			break;
		case UIF_TEL_LIST:
			UifTelList(sysEvt);
			break;
		case UIF_TEL_DELETE:
			UifTelDelete(sysEvt);
			break;
		case UIF_OPT_YEAR_SET:
			UifOptYearSet(sysEvt);
			break;
		case UIF_YEAR_SET:
			UifYearSet(sysEvt);
			break;
		case UIF_INCOMING_CALL:
			switch (sysEvt)
			{
				case SYS_CALL_ALLOWED:
					XLCD_CLEAR();
					XLCD_PUTS(eventData);
					XLCD_LINE2();
					XLCD_PUTS("ALLOWED");
//					UifStateChange(UIF_IDLE);
					break;
				case SYS_CALL_RESTRICTED:
					XLCD_CLEAR();
					XLCD_PUTS(eventData);
					XLCD_LINE2();
					XLCD_PUTS("FORBIDDEN");
//					UifStateChange(UIF_IDLE);
					break;
				case SYS_CALL_NOT_SENT:
					XLCD_CLEAR();
					XLCD_PUTS("NOT SENT!");
				default:
					break;
			}
			break;
	}
}

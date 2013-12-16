/************************************************************************//**
 * \file  tel_filt.c
 * \brief Telephone keeping and filtering module. Allows to store telephone
 * numbers in a rudimentary phone book, and also to filter them.
 *
 * \author Jesus Alonso Fernandez (doragasu)
 * \todo Add voice playback support???
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

#include "tel_filt.h"
#include "fatfs/ff.h"
#include <string.h>

/// Length of the phone book
#define TF_BUFLEN	1024

/// Telephone numbers black/white-listed
static char nums[TF_BUFLEN];
/// Blacklist/whitelist mode
static char mode;
/// End of the phone book
static unsigned char end;
/// Current position to read in the phone book
static unsigned char readPos;
/// Temporary stores phone book position
static unsigned char pos;
/// FALSE if hidden callers should be allowed
static char filtHidden;
/// TRUE if call filter is disabled
static char filtDisabled;

/************************************************************************//**
 * \brief Module initialization. Must be called before using any other
 * function.
 *
 * \param[in]  filterMode Filtering mode, either TF_MODE_BLACKLIST or
 *             TF_MODE_WHITELIST.
 ****************************************************************************/
void TfInit(char filterMode)
{
	mode = filterMode;
	nums[0] = '\0';
	end = 0;
	readPos = 0;
	filtDisabled = FALSE;
}

/************************************************************************//**
 * \brief Adds a number to the phone book
 *
 * \param[in] number Telephone number to add to the phone book.
 *
 * \return 0 if OK, nonzero otherwise.
 ****************************************************************************/
char TfNumAdd(char number[])
{
	char numLen = strlen(number);

	/// Remove the '\n' ending character
	if ('\n' == number[numLen - 1]) number[numLen - 1] = '\0';
	else numLen++;
	/// Check the number fits the buffer
	if ((end + numLen) > TF_BUFLEN) return 1;
	/// Copy the number and update end position
	strcpy(&nums[end], number);
	end += numLen;

	return 0;
}

/************************************************************************//**
 * \brief Checks if a telephone number is blacklisted.
 *
 * \param[in] number Telephone number to check.
 *
 * \return TF_NUM_OK if the number is allowed, TF_NUM_REJECT if the number
 * is blacklisted (or not in the whitelist), TF_NUM_FILTER_DISABLED if the
 * number should be rejected, but call filter is disabled.
 ****************************************************************************/
char TfNumCheck(char number[])
{
	int i = 0;

	// Search number
	while ((i < end) && strcmp(&nums[i], number))
		i += strlen(&nums[i]) + 1;

	if (i < end)
	{
		// Number found
		if (TF_MODE_BLACKLIST == mode)
			return filtDisabled?TF_FILTER_DISABLED:TF_NUM_REJECT;
		else return TF_NUM_OK;
	}

	// Number not found
	if (TF_MODE_BLACKLIST == mode) return TF_NUM_OK;
	else return filtDisabled?TF_FILTER_DISABLED:TF_NUM_REJECT;
}

/************************************************************************//**
 * \brief Gets the first number stored in the telephone book.
 *
 * \return The first number stored in the telephone book, or NULL if there
 * is no one.
 ****************************************************************************/
char *TfNumGetFirst(void)
{
	readPos = 0;

	return TfNumGetNext();
}

/************************************************************************//**
 * \brief Gets the next number stored in the telephone book.
 *
 * \return The next number stored in the telephone book, or NULL if there
 * are no more numbers in the telephone book.
 ****************************************************************************/
char *TfNumGetNext(void)
{
	/// Check if we have reached the end
	if (readPos >= end) return NULL;

	/// Advance one number
	pos = readPos;
	readPos += strlen(nums + readPos) + 1;
	return nums + pos;
}

/************************************************************************//**
 * \brief Gets the previous number stored in the telephone book.
 *
 * \return The previous number stored in the telephone book, or NULL if there
 * are no more numbers in the telephone book.
 ****************************************************************************/
char* TfNumGetPrev(void)
{
	/// Check we are not at the beginning
	if (0 == pos) return NULL;

	readPos = pos;
	pos -= 2;	// Skip ending '\0' from previous number
	while (nums[pos - 1] && (pos > 0)) pos--;
	return nums + pos;
}

/************************************************************************//**
 * \brief Deletes the current telephone number from the telephone book.
 ****************************************************************************/
void TfNumDelete(void)
{
	int i;

	for (i = pos; readPos < end; i++, readPos++) nums[i] = nums[readPos];
	end = i;
}

/// Temporal buffer length
#define TMP_BUFLEN	20
/************************************************************************//**
 * \brief Parses configuration file stored inside the microSD card. It
 * configures the blacklist/whitelist mode and adds previously stored
 * numbers to the phone book.
 *
 * \return 0 if OK, nonzero otherwise.
 ****************************************************************************/
char TfParseConfig(void)
{
	/// FatFs file to store system configuration
	/// \warning Test if making this file local can overflow stack.
	FIL fCfg;
	BYTE retVal;
	char tmpBuf[TMP_BUFLEN];

	/// Open configuration file for reading
	if ((retVal = f_open(&fCfg, "BALSAMO.CFG", FA_READ | FA_OPEN_EXISTING)))
		return retVal;
	/// First line is filter behaviour: either BLACKLIST or WHITELIST
	if (!f_gets(tmpBuf, TMP_BUFLEN, &fCfg)) return 1;
	if (!strcmp(tmpBuf, "BLACKLIST\n")) TfInit(TF_MODE_BLACKLIST);
	else if (!strcmp(tmpBuf, "WHITELIST\n")) TfInit(TF_MODE_WHITELIST);
	else return 2;
	/// Second line is BLACKLIST_UNKNOWN to blacklist unknown numbers, or
	/// ALLOW_UNKNOWN to allow unknown numbers
	if (!f_gets(tmpBuf, TMP_BUFLEN, &fCfg)) return 1;
	if (!strcmp(tmpBuf, "BLACKLIST_UNKNOWN\n")) filtHidden = TRUE;
	else if(!strcmp(tmpBuf, "ALLOW_UNKNOWN\n")) filtHidden = FALSE;
	else return 2;

	/// Remaining lines are the filtered telephone numbers
	while (f_gets(tmpBuf, TMP_BUFLEN, &fCfg))
	{
		/// Add a number, checking for errors
		/// \todo Enhance error handling (e.g. show a warning)
		if (TfNumAdd(tmpBuf)) break;
	}
	f_close(&fCfg);
	return 0;
}

/************************************************************************//**
 * \brief Saves the current configuration and phone book to the SD card.
 *
 * \return 0 if OK, nonzero otherwise.
 ****************************************************************************/
char TfCfgSave(void)
{
	FIL fCfg;
	char retVal, retVal2;
	char *num;

	/// Open configuration file for writing
	if ((retVal = f_open(&fCfg, "BALSAMO.CFG", FA_WRITE | FA_CREATE_ALWAYS)))
		return retVal;

	/// First line is filter behaviour: either BLACKLIST or WHITELIST
	if (TF_MODE_BLACKLIST == mode) retVal = f_puts("BLACKLIST\n", &fCfg);
	else retVal = f_puts("WHITELIST\n", &fCfg);
	if (retVal < 0)
	{
		f_close(&fCfg);
		return 1;
	}
	/// Second line is BLACKLIST_UNKNOWN to blacklist unknown numbers, or
	/// ALLOW_UNKNOWN to allow unknown numbers
	if (TRUE == filtHidden) retVal = f_puts("BLACKLIST_UNKNOWN\n", &fCfg);
	else retVal = f_puts("ALLOW_UNKNOWN\n", &fCfg);
	if (retVal < 0)
	{
		f_close(&fCfg);
		return 2;
	}
	/// Remaining lines are the filtered telephone numbers
	num = TfNumGetFirst();
	while (num)
	{
		retVal  = f_puts(num, &fCfg);
		retVal2 = f_putc('\n', &fCfg);
		if (retVal < 0 || retVal2 < 0)
		{
			f_close(&fCfg);
			return 3;
		}
		num = TfNumGetNext();
	}

	f_close(&fCfg);
	return 0;
}

/************************************************************************//**
 * \brief Tells if hidden numbers are either allowed or filtered out.
 *
 * \return TF_HID_OK if hidden calls must not be filtered, TF_HID_REJECT if
 * hidden calls must be rejected, or TF_HID_DISABLED if hidden call should
 * be rejected but call filter is disabled.
 ****************************************************************************/
char TfFilterHidden(void)
{
	if (filtHidden) return filtDisabled?TF_HID_DISABLED:TF_HID_REJECT;
	else return TF_HID_OK;
}

/************************************************************************//**
 * \brief Disables call filtering.
 ****************************************************************************/
void TfDisable(void)
{
	filtDisabled = TRUE;
}

/************************************************************************//**
 * \brief Enables call filtering.
 ****************************************************************************/
void TfEnable(void)
{
	filtDisabled = FALSE;
}

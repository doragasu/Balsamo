/************************************************************************//**
 * \file  utils.h
 * \brief Some utilities, mostly to drive the LCD.
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

#ifndef _UTILS_H_
#define _UTILS_H_

#include "xlcd/xlcd.h"

/** \defgroup utils_api utils
 *
 * Some utilities, mostly to drive the LCD.
 * \{ */


/// Wait until the LCD is not busy
#define XLCD_BUSY_WAIT()	do{while(BusyXLCD());}while(0)

/// Write a string to the LCD, in the current pos
#define XLCD_PUTS(str)		\
{							\
	XLCD_BUSY_WAIT();		\
	PutsXLCD((char*)(str));	\
}

/// Move LCD cursor to the start of line 1
#define XLCD_LINE1()		\
{							\
	XLCD_BUSY_WAIT();		\
    SetDDRamAddr(0);		\
}

/// Move LCD cursor to the start of line 2
#define XLCD_LINE2()		\
{							\
	XLCD_BUSY_WAIT();		\
    SetDDRamAddr(0x40);		\
}

/// Write a character to the LCD
#define XLCD_PUTC(chr)		\
{							\
	XLCD_BUSY_WAIT();		\
	putcXLCD(chr);			\
}

/// Send a command to the LCD
#define XLCD_CMD(cmd)		\
{							\
	XLCD_BUSY_WAIT();		\
	WriteCmdXLCD(cmd);		\
}

/// Clear LCD and send cursor to origin
#define XLCD_CLEAR()		XLCD_CMD(CLEAR_XLCD);SwDelayNCyc(DELAY_MS_COUNT(7))

/// Shift the LCD cursor a position to the left
#define XLCD_SHIFT_LEFT()	XLCD_CMD(SHIFT_CUR_LEFT)

/// Shift the LCD a position to the right
#define XLCD_SHIFT_RIGHT()	XLCD_CMD(SHIFT_CUR_RIGHT)

/// Set LCD cursor position
#define XLCD_SETPOS(pos)	\
{							\
	XLCD_BUSY_WAIT();		\
	SetDDRamAddr(pos);		\
}

/** \} */

#endif /*_UTILS_H_*/

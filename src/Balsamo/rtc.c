/************************************************************************//**
 * \file rtc.c
 * \brief Real Time Clock module
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

#include <p30F6014.h>
#include "rtc.h"
#include "common.h"

/************************************************************************//**
 * \brief Enables low power 32768 Hz oscillator
 *
 * \note  Coded in assembly inside lposcen.s
 ****************************************************************************/
void LPOSCEnable(void);

/// Time keeping variables
static BYTE rtcYear = 2013-1980, rtcMon = 1, rtcMday = 23, rtcHour, rtcMin, rtcSec;

/// RTC interrupt handler. Called once each second to keep clock counting
void __attribute__((interrupt, auto_psv)) _T1Interrupt (void)
{
	static const BYTE samurai[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	BYTE maxDay;

	_T1IF = 0;			///< Clear T1 flag

    /// Update date and time
	if (++rtcSec >= 60)
	{
		rtcSec = 0;
		if (++rtcMin >= 60)
		{
			rtcMin = 0;
			if (++rtcHour >= 24)
			{
				rtcHour = 0;
				maxDay = samurai[rtcMon - 1];
				if ((maxDay == 28) && !(rtcYear & 3)) maxDay++;
				if (++rtcMday > maxDay)
				{
					rtcMday = 1;
					if (++rtcMon > 12)
					{
						rtcMon = 1;
						rtcYear++;
					}
				}
			}
		}
	}
}

/************************************************************************//**
 * \brief Starts the RTC.
 ****************************************************************************/
void RtcStart(void)
{
	/// Write OSCCONL unlock sequence, and enable LP Oscillator
	LPOSCEnable();
	PR1 = 32768 - 1;
	T1CON = 0x8002;
	/// Enable TIMER1 interrupt
	_T1IE = 1;
}

/************************************************************************//**
 * \brief Sets date and time
 *
 * \param[in] year Year to set (e.g. 2013).
 * \param[in] mon  Month to set (1 ~ 12).
 * \param[in] day  Day to set (1 ~ 31).
 * \param[in] hour Hour to set (0 ~ 23).
 * \param[in] min  Minute to set (0 ~ 59).
 * \param[in] sec  Second to set (0 ~ 59).
 ****************************************************************************/
void RtcSetTime(WORD year, BYTE mon, BYTE day, BYTE hour, BYTE min, BYTE sec)
{
	_DI();

	rtcYear = year - 1980;
	rtcMon = mon;
	rtcMday = day;
	rtcHour = hour;
	rtcMin = min;
	rtcSec = sec;

	_EI();
}

/************************************************************************//**
 * \brief Gets time
 *
 * \param[out] hour Hour.
 * \param[out] min  Minute.
 * \param[out] sec  Second.
 ****************************************************************************/
void RtcGetTime(BYTE *hour, BYTE *min, BYTE *sec)
{
	_DI();

	*hour = rtcHour;
	*min = rtcMin;
	*sec = rtcSec;

	_EI();
}

/************************************************************************//**
 * \brief Gets date
 *
 * \param[out] year Year.
 * \param[out] mon  Month.
 * \param[out] day  Day.
 ****************************************************************************/
void RtcGetDate(WORD *year, BYTE *month, BYTE *day)
{
	_DI();

	*year = 1980 + rtcYear;
	*month = rtcMon;
	*day = rtcMday;

	_EI();
}

/************************************************************************//**
 * \brief Get timestamp formatted for fatfs
 *
 * \return Timestamp using fatfs format
 ****************************************************************************/
DWORD get_fattime(void)
{
	DWORD tmr;

	_DI();

	/* Pack date and time into a DWORD variable */
	tmr =	  (((DWORD)rtcYear - 80) << 25)
			| ((DWORD)rtcMon << 21)
			| ((DWORD)rtcMday << 16)
			| (WORD)(rtcHour << 11)
			| (WORD)(rtcMin << 5)
			| (WORD)(rtcSec >> 1);
	_EI();

	return tmr;
}

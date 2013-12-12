/************************************************************************//**
 * \file rtc.h
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

#ifndef _RTC_H_
#define _RTC_H_

#include "types.h"

#define RTC_DEF_YEAR_STR	"2014"
#define RTC_DEF_YEAR_NUM	 2014

/** \defgroup rtc_api rtc
 *
 * Real Time Clock module.
 * \{ */

/************************************************************************//**
 * \brief Starts the RTC.
 ****************************************************************************/
void RtcStart(void);

/************************************************************************//**
 * \brief Sets date and time (excepting year).
 *
 * \param[in] mon  Month to set (1 ~ 12).
 * \param[in] day  Day to set (1 ~ 31).
 * \param[in] hour Hour to set (0 ~ 23).
 * \param[in] min  Minute to set (0 ~ 59).
 * \param[in] sec  Second to set (0 ~ 59).
 ****************************************************************************/
void RtcSetTime(BYTE mon, BYTE day, BYTE hour, BYTE min, BYTE sec);

/************************************************************************//**
 * \brief Sets year.
 *
 * \param[in] year Year to set (e.g. 2014).
 ****************************************************************************/
void RtcSetYear(WORD year);

/************************************************************************//**
 * \brief Gets time
 *
 * \param[out] hour Hour.
 * \param[out] min  Minute.
 * \param[out] sec  Second.
 ****************************************************************************/
void RtcGetTime(BYTE *hour, BYTE *min, BYTE *sec);

/************************************************************************//**
 * \brief Gets date
 *
 * \param[out] year Year.
 * \param[out] mon  Month.
 * \param[out] day  Day.
 ****************************************************************************/
void RtcGetDate(WORD *year, BYTE *month, BYTE *day);

/************************************************************************//**
 * \brief Get timestamp formatted for fatfs
 *
 * \return Timestamp using fatfs format
 ****************************************************************************/
DWORD get_fattime(void);

/** \} */

#endif //_RTC_H_

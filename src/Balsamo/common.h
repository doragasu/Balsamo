/************************************************************************//**
 * \file common.h
 * \brief Some common definitions used by other modules.
 *
 * \author Jesus Alonso Fernandez (doragasu)
 * \todo Use __delay_ms and __delay_us from libpic30.h
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


#ifndef _SW_DELAY_H_
#define _SW_DELAY_H_

/** \defgroup common_api common
 *
 * Some common definitions used by other modules.
 * \{ */

/// Cycle frequency, equals Fosc/4
#define Fcy		(22118400L/4)
/// Alternative cycle frequency definition
#define FCY		Fcy

/// Calculates delay cycles to sleep millisecons
#define DELAY_MS_COUNT(ms) (ms*(Fcy/(8*1000L)))
/// Calculates delay cycles to sleep microseconds
#define DELAY_US_COUNT(us) (us*(Fcy/(8*1000000L)))
/// Calculates delay cycles to sleep nanoseconds
#define DELAY_NS_COUNT(ns) ((ns*(Fcy/8))/(1000000000L))

/// Temporary disables interrupts
#define _DI()		__asm__ volatile("disi #0x3FFF")
/// Enables interrupts
#define _EI()		__asm__ volatile("disi #0")

/// Disables all interrupts and puts system to sleep
#define PANIC()				\
{							\
	IEC2 = IEC1 = IEC0 = 0;	\
	Sleep();				\
}

/** \defgroup bit_names Definitions used for bit masks.
 * \{
 */
/// Bit 0 mask
#define BIT0		0x0001
/// Bit 1 mask
#define BIT1		0x0002
/// Bit 2 mask
#define BIT2		0x0004
/// Bit 3 mask
#define BIT3		0x0008
/// Bit 4 mask
#define BIT4		0x0010
/// Bit 5 mask
#define BIT5		0x0020
/// Bit 6 mask
#define BIT6		0x0040
/// Bit 7 mask
#define BIT7		0x0080
/// Bit 8 mask
#define BIT8		0x0100
/// Bit 9 mask
#define BIT9		0x0200
/// Bit 10 mask
#define BIT10		0x0400
/// Bit 11 mask
#define BIT11		0x0800
/// Bit 12 mask
#define BIT12		0x1000
/// Bit 13 mask
#define BIT13		0x2000
/// Bit 14 mask
#define BIT14		0x4000
/// Bit 15 mask
#define BIT15		0x8000
/** \} */

/************************************************************************//**
 * \brief Software delay
 *
 * \param[in] nCycles Number of cycles to delay.
 ****************************************************************************/
void SwDelayNCyc(long nCycles);

/** \} */

#endif /*_SW_DELAY_H_*/

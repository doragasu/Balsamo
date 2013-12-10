/*************************************************************************//**
 * \file  keyb.c
 * \brief Implements interfaces to handle the keyboard of the Balsamo PCB
 *
 * \author Jesus Alonso Fernandez (doragasu)
 *
 * \note Updated for Rev.B PCB
 * \warning User must implement the key callback functions that he wishes to
 * use (using KEYB_XXX_INT definitions).
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

#include "keyb.h"

/** \defgroup port_vals Port values since the last CN interrupt
 * \{
 */
static unsigned int lastPA; ///< Port A
static unsigned int lastPB; ///< Port B
static unsigned int lastPD; ///< Port D
/** \} */

/** \defgroup int_dummy_func Dummy functions for unused interrupts
 * \{
 */
#ifndef KEYB_UP_LEFT_PRESS_INT
void KeybUpLeftPress(void){}
#endif
#ifndef KEYB_UP_LEFT_RELEASE_INT
void KeybUpLeftRelease(void){}
#endif
#ifndef KEYB_DOWN_RIGHT_PRESS_INT
void KeybDownRightPress(void){}
#endif
#ifndef KEYB_DOWN_RIGHT_RELEASE_INT
void KeybDownRightRelease(void){}
#endif
#ifndef KEYB_ENTER_PRESS_INT
void KeybEnterPress(void){}
#endif
#ifndef KEYB_ENTER_RELEASE_INT
void KeybEnterRelease(void){}
#endif
#ifndef KEYB_ESC_PRESS_INT
void KeybEscPress(void){}
#endif
#ifndef KEYB_ESC_RELEASE_INT
void KeybEscRelease(void){}
#endif
/** \} */

/** \defgroup port_constants Some constants used to handle the keyboard pins
 * \{
 */
#define KEYB_CN_CFG			0
#if defined(KEYB_UP_LEFT_PRESS_INT) || defined(KEYB_UP_LEFT_RELEASE_INT)
#define KEYB_CN22       0x0040	// CNXX2
#define KEYB_RA6		0x0040
#else
#define KEYB_CN22       0x0000
#define KEYB_RA6		0x0000
#endif
#if defined(KEYB_DOWN_RIGHT_PRESS_INT) || defined(KEYB_DOWN_RIGHT_RELEASE_INT)
#define KEYB_CN23       0x0080	// CNXX2
#define KEYB_RA7		0x0080
#else
#define KEYB_CN23       0x0000
#define KEYB_RA7		0x0000
#endif
#if defined(KEYB_ENTER_PRESS_INT) || defined(KEYB_ENTER_RELEASE_INT)
#define KEYB_CN19		0x0008	// CNXX2
#define KEYB_RD13		0x2000
#else
#define KEYB_CN19		0x0000
#define KEYB_RD13		0x0000
#endif
#if defined(KEYB_ESC_PRESS_INT) || defined(KEYB_ESC_RELEASE_INT)
#define KEYB_CN7        0x0080	// CNXX1
#define KEYB_RB5		0x0020
#else
#define KEYB_CN7        0x0000
#define KEYB_RB5		0x0000
#endif
/** \} */

/** \defgroup keyb_port_mask Masks used to configure ports and CN function
 * \{
 */
#define KEYB_PA_CN_MASK		(KEYB_RA6 | KEYB_RA7)
#define KEYB_PB_CN_MASK		(KEYB_RB5)
#define KEYB_PD_CN_MASK		(KEYB_RD13)
#define KEYB_PB_SHIFT_MASK	0x0010
#define KEYB_PA_MASK		(KEYB_PA_CN_MASK)
#define KEYB_PB_MASK		(KEYB_PB_CN_MASK | KEYB_PB_SHIFT_MASK)
#define KEYB_PD_MASK		(KEYB_PD_CN_MASK)
/** \} */

/*************************************************************************//**
 * \brief Initializes the keyboard. S203, S204, S205 and S206 will trigger
 * CN interrupts. S202 must be explicitly readed.
 *****************************************************************************/
void KeybInit(void)
{
	/// Initialize input pins
	PORTA |= KEYB_PA_MASK;	// RA6 y 7 (CN22 y 23)
	PORTB |= KEYB_PB_MASK;	// RB4 y 5 (CN6 y 7)
	PORTD |= KEYB_PD_MASK;	// RD13 (CN19)
	/// Enable CN function for pins that will trigger interrupts
	CNEN1 |= KEYB_CN7;
	CNEN2 |= KEYB_CN19 | KEYB_CN22 | KEYB_CN23;
	/// Enable pullups
	/// \todo Enable pullup for S202?
	CNPU1 |= KEYB_CN7;
	CNPU2 |= KEYB_CN19 | KEYB_CN22 | KEYB_CN23;
	/// Set CN interrupt priority
	IPC3bits.CNIP = KEYB_CN_INT_PRIO;
}

/*************************************************************************//**
 * \brief Enables the configured keyboard interrupts.
 *****************************************************************************/
void KeybIntsEnable(void)
{
	/// Refresh port values
	lastPA = PORTA;
	lastPB = PORTB;
	lastPD = PORTD;
	/// Clear CN interrupt flag
	IFS0bits.CNIF = 0;
	/// Enable CN interrupt
	IEC0bits.CNIE = 1;
}

/************************************************************************//**
 * \brief CN interrupt service routine. Determines what
 ****************************************************************************/
void __attribute__((__interrupt__)) _CNInterrupt(void)
{
	// Temporal port values
	unsigned int currPA;
	unsigned int currPB;
	unsigned int currPD;
	// Holds the changed bits
	unsigned int changed;

	/// Read ports
	currPA = PORTA;
	currPB = PORTB;
	currPD = PORTD;

	/// Determine the changed bits from PORT A and make relevant callbacks
	changed = (currPA ^ lastPA) & KEYB_PA_CN_MASK;

#ifdef KEYB_UP_LEFT_PRESS_INT
	if ((changed & KEYB_RA6)  && !(currPA & KEYB_RA6))  KeybUpLeftPress();
#endif
#ifdef KEYB_UP_LEFT_RELEASE_INT
	if ((changed & KEYB_RA6)  &&  (currPA & KEYB_RA6))  KeybUpLeftRelease();
#endif
#ifdef KEYB_DOWN_RIGHT_PRESS_INT
	if ((changed & KEYB_RA7)  && !(currPA & KEYB_RA7))  KeybDownRightPress();
#endif
#ifdef KEYB_DOWN_RIGHT_RELEASE_INT
	if ((changed & KEYB_RA7)  &&  (currPA & KEYB_RA7))  KeybDownRightRelease();
#endif

	/// Determine the changed bits from PORT D and make relevant callbacks
	changed = (currPD ^ lastPD) & KEYB_PD_CN_MASK;
#ifdef KEYB_ENTER_PRESS_INT
	if ((changed & KEYB_RD13) && !(currPD & KEYB_RD13)) KeybEnterPress();
#endif
#ifdef KEYB_ENTER_RELEASE_INT
	if ((changed & KEYB_RD13) &&  (currPD & KEYB_RD13)) KeybEnterRelease();
#endif

	/// Determine the changed bits from PORT B and make relevant callbacks
	changed = (currPB ^ lastPB) & KEYB_PB_CN_MASK;

#ifdef KEYB_ESC_PRESS_INT
	if ((changed & KEYB_RB5)  && !(currPB & KEYB_RB5))  KeybEscPress();
#endif
#ifdef KEYB_ESC_RELEASE_INT
	if ((changed & KEYB_RB5)  &&  (currPB & KEYB_RB5))  KeybEscRelease();
#endif

	/// Update last known value of the ports
	lastPA = currPA;
	lastPB = currPB;
	lastPD = currPD;
	/// Clear CN interrupt flag
	IFS0bits.CNIF = 0;
}

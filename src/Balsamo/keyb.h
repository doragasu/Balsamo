/*************************************************************************//**
 * \file  keyb.h
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

#ifndef _KEYB_H_
#define _KEYB_H_

#include <p30f6014.h>
#include "types.h"

/** \defgroup keyb_api keyb
 *
 * Implements interfaces to handle the keyboard of the Balsamo PCB.
 * \{ */

/** \defgroup used_key_defs Definitions for activating the required keys.
 * Uncomment unused functions.
 * \{
 */
#define KEYB_UP_LEFT_PRESS_INT
//#define KEYB_UP_LEFT_RELEASE_INT
#define KEYB_DOWN_RIGHT_PRESS_INT
//#define KEYB_DOWN_RIGHT_RELEASE_INT
#define KEYB_ENTER_PRESS_INT
//#define KEYB_ENTER_RELEASE_INT
#define KEYB_ESC_PRESS_INT
//#define KEYB_ESC_RELEASE_INT
/** \} */

/// Keyboard interrupt default priority
#define KEYB_CN_INT_PRIO_DEFAULT		4
/// Keyboard interrupt priority
#define KEYB_CN_INT_PRIO				KEYB_CN_INT_PRIO_DEFAULT

/*************************************************************************//**
 * \brief Initializes the keyboard. S203, S204, S205 and S206 will trigger
 * CN interrupts. S202 must be explicitly readed.
 *****************************************************************************/
void KeybInit(void);

/*************************************************************************//**
 * \brief Reads UP_LEFT key value.
 *
 * \return 0 if the key is pressed, non zero otherwise
 *****************************************************************************/
#define KeybUpLeftRead()		PORTAbits.RA6

/*************************************************************************//**
 * \brief Reads DOWN_RIGHT key value.
 *
 * \return 0 if the key is pressed, non zero otherwise
 *****************************************************************************/
#define KeybDownRightRead()		PORTAbits.RA7

/*************************************************************************//**
 * \brief Reads ENTER key value.
 *
 * \return 0 if the key is pressed, non zero otherwise
 *****************************************************************************/
#define KeybEnterRead()			PORTDbits.RD13

/*************************************************************************//**
 * \brief Reads ESC key value.
 *
 * \return 0 if the key is pressed, non zero otherwise
 *****************************************************************************/
#define KeybEscRead()			PORTBbits.RB5

/*************************************************************************//**
 * \brief Reads SHIFT key value.
 *
 * \return 0 if the key is pressed, non zero otherwise
 *****************************************************************************/
#define KeybShiftRead()			PORTBbits.RB4

/*************************************************************************//**
 * \brief Enables configured key interrupts
 *****************************************************************************/
void KeybIntsEnable(void);

/*************************************************************************//**
 * \brief Disables configured key interrupts
 *****************************************************************************/
#define KeybIntsDisable()		(CNIEbits.CN = 0)

/** \defgroup keyb_user_proto Function prototypes to be user implemented.
 * \warning User must implement the functions of the prototypes corresponding
 * to the interrupts he has enabled via KEYB_XX_INT definitions. These
 * functions are called in interrupt context, so they should return as fast
 * as possible, and should never block.
 * \{
 */
#ifdef KEYB_UP_LEFT_PRESS_INT
extern void KeybUpLeftPress(void);
#endif
#ifdef KEYB_UP_LEFT_RELEASE_INT
extern void KeybUpLeftRelease(void);
#endif
#ifdef KEYB_DOWN_RIGHT_PRESS_INT
extern void KeybDownRightPress(void);
#endif
#ifdef KEYB_DOWN_RIGHT_RELEASE_INT
extern void KeybDownRightRelease(void);
#endif
#ifdef KEYB_ENTER_PRESS_INT
extern void KeybEnterPress(void);
#endif
#ifdef KEYB_ENTER_RELEASE_INT
extern void KeybEnterRelease(void);
#endif
#ifdef KEYB_ESC_PRESS_INT
extern void KeybEscPress(void);
#endif
#ifdef KEYB_ESC_RELEASE_INT
extern void KeybEscRelease(void);
#endif
/** \} */

/** \} */

#endif /*_KEYB_H_*/

/************************************************************************//**
 * \file  line_if.h
 * \brief Telephone line interface. Encapsulates telephone line control
 * functions such as ring detection and pick-up/hang.
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

#ifndef _LINE_IF_H_
#define _LINE_IF_H_

#include "types.h"
#include "ports.h"

/** \defgroup line_if_api line_if
 *
 * Telephone line interface. Encapsulates telephone line control
 * functions such as ring detection and pick-up/hang.
 * \{ */

/************************************************************************//**
 * \brief Picks up the phone.
 ****************************************************************************/
//#define LinePickUp()		LATCbits.LATC4=1
// Stub for debugging purposes
#define LinePickUp()

/************************************************************************//**
 * \brief Hangs call.
 ****************************************************************************/
#define LineHang()			LATCbits.LATC4=0

/************************************************************************//**
 * \brief Module intialization. Must be called prior to using other module
 * functions/macros.
 ****************************************************************************/
void LineInit(void);

/// Enables Ring detection interrupt
#define LineRingDetEnable()		EnableINT0

/// Disables Ring detection interrupt
#define LineRingDetDisable()	DisableINT0

/************************************************************************//**
 * \brief Tells if there is a RING notification pending.
 *
 * \return
 * - TRUE : RING notification pending.
 * - FALSE: No RING notifications pending.
 *
 * \note Calling this function also clears pending RING notifications.
 ****************************************************************************/
char LineRing(void);

/** \} */

#endif /*_LINE_IF_H_*/

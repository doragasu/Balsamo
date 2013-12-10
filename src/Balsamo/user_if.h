/************************************************************************//**
 * \file  user_if.h
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

#ifndef _USER_IF_H_
#define _USER_IF_H_

#include "system.h"

/** \defgroup user_if_api user_if
 *
 * Manages user interface. This module receives keyboard and system
 * events and controls the LCD to give the user useful information about
 * what's going on.
 * \{ */

/// Errorless return code
#define UIF_OK		0

/************************************************************************//**
 * \brief Module initialization. Must be called before using anything else
 * in this module.
 *
 * \return UIF_OK if everything went file.
 ****************************************************************************/
int UifInit(void);

/************************************************************************//**
 * \brief User interface state machine. Processes key press events, and
 * also other system events, updating user interface status accordingly.
 *
 * \param[in] sysEvt Received system event
 * \param[in] eventData Data associated to the event, or NULL if none.
 * \param[in] dataLen Length of eventData field, or 0 if no data available.
 ****************************************************************************/
void UifEventParse(SysEvent sysEvt, char eventData[], int dataLen);

/** \} */

#endif /*_USER_IF_H_*/

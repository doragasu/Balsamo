/************************************************************************//**
 * \file ext_uart.h
 * \brief Manages UART1
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

#ifndef _EXT_UART_H_
#define _EXT_UART_H_

#include "common.h"
#include <p30f6014.h>

/** \defgroup ext_uart_api ext_uart
 *
 * Manages UART1
 * \{ */

/************************************************************************//**
 * \brief Configures UART1 to 115200,N,1.
 ****************************************************************************/
void ExtUart1Init(void);

/************************************************************************//**
 * \brief Enables sending and receiving data through UART1.
 ****************************************************************************/
#define ExtUart1Enable()	U1MODEbits.UARTEN=1

/************************************************************************//**
 * \brief Disables sending and receiving data through UART1.
 ****************************************************************************/
#define ExtUart1Disable()	U1MODEbits.UARTEN=0

/** \} */

#endif /*_EXT_UART_H_*/

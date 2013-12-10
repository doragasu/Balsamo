/************************************************************************//**
 * \file ext_uart.c
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

#include "ext_uart.h"

/************************************************************************//**
 * \brief Configures UART1 to 115200,N,1.
 ****************************************************************************/
void ExtUart1Init(void)
{
	/// Configure U1MODE register
	U1MODEbits.USIDL = 1;	// Stop UART in Idle mode

	/// Configure U1STA register
	U1STAbits.UTXEN = 1;	// Enable transmission
	U1STAbits.OERR = 0;		// Clear Overrun error

	/// Configure U1BRG register
	U1BRG = (FCY/(16 * 115200)) - 1;
}

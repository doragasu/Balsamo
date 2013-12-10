/************************************************************************//**
 * \file  line_if.c
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

#include "line_if.h"
#include "system.h"
#ifndef USE_AND_OR
/// To enable AND/OR masks
#define USE_AND_OR
#endif
#include "xlcd/xlcd.h"

/// Indicates if there is a pending RING notification
static volatile char ringPending = FALSE;

/************************************************************************//**
 * \brief Module intialization. Must be called prior to using other module
 * functions/macros.
 ****************************************************************************/
void LineInit(void)
{
	/// Initialize GPIO (PORTx, TRISx, LATx)
	LineHang();
	TRISCbits.TRISC4 = 0;	///< RC4 as output to pick-up/hang calls

	/// Initialize INT0 for RING detection
	ConfigINT0(RISING_EDGE_INT & EXT_INT_ENABLE & EXT_INT_PRI_4);
}

/************************************************************************//**
 * \brief Tells if there is a RING notification pending.
 *
 * \return
 * - TRUE : RING notification pending.
 * - FALSE: No RING notifications pending.
 *
 * \note Calling this function also clears pending RING notifications.
 ****************************************************************************/
char LineRing(void)
{
	char tmp;

	tmp = ringPending;
	ringPending = FALSE;

	return tmp;
}

/// RING detector interrupt ISR
void __attribute__((__interrupt__)) _INT0Interrupt(void)
//void _ISR _INT0Interrupt(void)
{
	/// Signal a pending RING
	ringPending = TRUE;
	/// Generate RING system event
	SysIQueuePut(SYS_RING);
	/// Clear interrupt flag
	IFS0bits.INT0IF = 0;
}

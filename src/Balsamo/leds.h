/************************************************************************//**
 * \file  leds.h
 * \brief LEDs control module
 *
 * \author Jesus Alonso Fernandez (doragasu)
 * \note Updated for Rev.B PCB
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

#ifndef _LEDS_H_
#define _LEDS_H_

#include <p30fxxxx.h>

/** \defgroup leds_api leds
 *
 * LEDs control module.
 * \{ */

/// Value to turn ON a LED
#define LED_ON		0
/// Value to turn OFF a LED
#define LED_OFF		1

/// Initialization. Configures LED pins as outputs and turns the LEDs OFF.
#define LedInit()	LATFbits.LATF0 = 1;\
					LATFbits.LATF1 = 1;\
					LATGbits.LATG0 = 1;\
					LATGbits.LATG1 = 1;\
					TRISFbits.TRISF0 = 0;\
					TRISFbits.TRISF1 = 0;\
					TRISGbits.TRISG0 = 0;\
					TRISGbits.TRISG1 = 0

/** \defgroup leds_set_reva Sets a LED to the specified state (either LED_ON
 * or LED_OFF).
 * \note Definitions for Rev.A PCB.
 * \{
 */
#define SetD13(state)	(LATFbits.LATF0 = state)
#define SetD14(state)	(LATFbits.LATF1 = state)
#define SetD15(state)	(LATGbits.LATG0 = state)
#define SetD16(state)	(LATGbits.LATG1 = state)
/** \} */

/** \defgroup leds_toggle_reva Toggle specified LED status.
 * \note Definitions for Rev.A PCB.
 * \{
 */
#define ToggleD13()		(LATFbits.LATF0 ^= 1)
#define ToggleD14()		(LATFbits.LATF1 ^= 1)
#define ToggleD15()		(LATGbits.LATG0 ^= 1)
#define ToggleD16()		(LATGbits.LATG1 ^= 1)
/** \} */

/** \defgroup leds_set_revb Sets a LED to the specified state (either LED_ON
 * or LED_OFF). Definitions for Rev.B PCB.
 * \{
 */
#define SetD201(state)	(LATFbits.LATF0 = state)
#define SetD202(state)	(LATFbits.LATF1 = state)
#define SetD203(state)	(LATGbits.LATG0 = state)
#define SetD204(state)	(LATGbits.LATG1 = state)
/** \} */

/** \defgroup leds_toggle_revb Toggle specified LED status.
 * \note Definitions for Rev.B PCB.
 * \{
 */
#define ToggleD201()	(LATFbits.LATF0 ^= 1)
#define ToggleD202()	(LATFbits.LATF1 ^= 1)
#define ToggleD203()	(LATGbits.LATG0 ^= 1)
#define ToggleD204()	(LATGbits.LATG1 ^= 1)
/** \} */

/** \} */

#endif /*_LEDS_H_*/

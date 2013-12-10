/************************************************************************//**
 * \brief Allows to play a raw audio data stream using PWM.
 *
 * Audio data must be 8 kHz, 8 bit and must be supplied each time the
 * callback function specified in PwmPlayStart() is called. PWM frequency is
 * 32 kHz, so Audio sample values must be in the range [0 ~ Fcy/32000]. For
 * Fcy = 22118400/4, this is between 0 and 172, with an offset of 172/2.
 *
 * \author Jesus Alonso Fernandez (doragasu)
 * \note wavconv.m Octave/Matlab script can be used to convert a wav file to
 * the format used by this module.
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

#ifndef _PWMPLAY_H_
#define _PWMPLAY_H_

#include <p30F6014.h>
#include "../types.h"

//#define _SPEAKER_TEST

/// \defgroup PwmPlayRegs Timer and Output Compare registers used by PwmPlay
/// module. x can be from 1 to 8. y can be 2 or 3.
/// \{
/// Undef this if using Timer 3
#define _IS_TIMER_2		1
#ifdef _SPEAKER_TEST
/// Output compare x control register
#define OCxCON			OC1CON
/// Secondary data register for the output compare channel
#define OCxRS			OC1RS
#else // _SPEAKER_TEST
/// Output compare x control register
#define OCxCON			OC2CON
/// Secondary data register for the output compare channel
#define OCxRS			OC2RS
#endif // _SPEAKER_TEST
/// Timer y base register
#define TyCON			T2CON
/// Timer y period register
#define PRy				PR2
/// Timer counter
#define TMRy			TMR2
/// Timer interrupt flag
#define _TyIF			_T2IF
/// Timer interrupt enable
#define _TyIE			_T2IE
/// Timer handler
#define _TyInterrupt	_T2Interrupt
/// \}

/************************************************************************//**
 * \brief Module initialization. Must be called once before using the other
 * functions in the module.
 ****************************************************************************/
void PwmPlayInit(void);

/************************************************************************//**
 * \brief Starts playback.
 *
 * \param[in] Callback function to be called each time new data is needed.
 * \warning To achieve uninterrupted playback, if data is retrieved using a
 * slow peripheral (e.g. a SD card), it must be retrieved using a double
 * buffered algorithm, so the callback returns the pointer to previously
 * fetched data and starts the retrieval of a new buffer immediately.
 ****************************************************************************/
void PwmPlayStart(BYTE*(*DataCallback)(UINT *dataLen));

/************************************************************************//**
 * \brief Stops the playback.
 *
 * \note An alternative way to stop playback, is returning a null pointer
 * when the data callback function is called.
 ****************************************************************************/
void PwmPlayStop(void);

#endif /*_PWMPLAY_H_*/

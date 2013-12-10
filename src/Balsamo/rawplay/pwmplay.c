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

#include "pwmplay.h"
#include "../common.h"

/// \defgroup PwmPlayBuf Holds all the data needed to handle the audio buffer
/// \{
typedef struct
{
	BYTE* (*dCb)(UINT *len);	///< Callback function to get new data
	BYTE *b;					///< Pointer to the data buffer
	UINT bLen;					///< Buffer length
	UINT i;						///< Index in the buffer
	BYTE prev;					///< Previous data value
	BYTE sAvg;					///< Status for the averaging algorithm
} PwmPlayBuf;
/// \}

/// Buffer used to play the stream
static PwmPlayBuf pb;

/************************************************************************//**
 * \brief Module initialization. Must be called once before using the other
 * functions in the module.
 ****************************************************************************/
void PwmPlayInit(void)
{
	/// Initialize timer for FS = 32 kHz
	TyCON = 0;
	PRy = FCY / 32000 - 1;	// 32055 Hz for 22118400/4 FCY.
	/// Initialize Output Compare for PWM mode (OCM = 110) and Timer 2
#if (_IS_TIMER_2)
	OCxCON = 0x0006;
#else
	OCxCON = 0x000E;
#endif
}

/************************************************************************//**
 * \brief Starts playback.
 *
 * \param[in] Callback function to be called each time new data is needed.
 * \warning To achieve uninterrupted playback, if data is retrieved using a
 * slow peripheral (e.g. an SD card), it must be retrieved using a double
 * buffered algorithm, so the callback returns the pointer to previously
 * fetched data and starts the retrieval of a new buffer immediately.
 ****************************************************************************/
void PwmPlayStart(BYTE*(*DataCallback)(UINT *dataLen))
{
	/// Initialize structure for data transfer
	pb.dCb = DataCallback;
	pb.i = 0;
	pb.b = pb.dCb(&pb.bLen);
	pb.prev = FCY / 32000 / 2;		// Prev value half way
	if (pb.b && pb.bLen)
	{
		/// Reset timer, enable it and interrupts
		TMRy = 0;
		_TyIF = 0;
		_TyIE = 1;
		TyCON |= 0x8000;
	}
}

/************************************************************************//**
 * \brief Stops the playback.
 *
 * \note An alternative way to stop playback, is returning a null pointer
 * when the data callback function is called.
 ****************************************************************************/
void PwmPlayStop(void)
{
	/// Stop the timer and disable its interrupts
	TyCON &= 0x7FFF;
	_TyIE = 0;
}

/************************************************************************//**
 * \ Timer interrupt handler. Writes the correct value to the PWM for audio
 * generation.

 * 4 values per sample are written, so linear averaging is used
 * to calculate 3 of the 4 bytes needed per sample.
 ****************************************************************************/
void __attribute__((interrupt, auto_psv)) _TyInterrupt (void)
{
	_TyIF = 0;

	switch(pb.sAvg++ & 0x3)
	{
		case 0:
			/// Output the previous sample, as is
			OCxRS = pb.prev;
			break;

		case 1:
			/// \todo Multiply should be faster than 3 additions (or one
			/// shift and one addition), but haven't tested it.
			OCxRS = (3 * pb.prev + pb.b[pb.i])>>2;
			break;

		case 2:
			OCxRS = (pb.prev + pb.b[pb.i])>>1;
			break;

		case 3:
			OCxRS = (pb.prev + 3 * pb.b[pb.i])>>2;
			pb.prev = pb.b[pb.i++];
			if (pb.i >= pb.bLen)
			{
				/// We need more wood!
				pb.b = pb.dCb(&pb.bLen);
				/// Stop if no more data retrieved
				if ((!pb.b) || (!pb.bLen)) PwmPlayStop();
				else pb.i = 0;
			}
			break;
	}

	/// \todo disk_timerproc handling?
//	disk_timerproc();
}

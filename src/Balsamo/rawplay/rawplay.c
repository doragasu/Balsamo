/************************************************************************//**
 * \brief Uses FatFs and pwmplay modules to play raw audio files.
 *
 * For the audio file format and restrictions, read pwmplay documentation.
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

#include "rawplay.h"
#include "pwmplay.h"
#include "../fatfs/ff.h"

/// Number of samples of the data read buffer. 512 bytes is OK for SD cards
#define RAWP_BUF_NS		512

/// FatFs file
static FIL f;
/// Read buffer. Doble buffering is used to avoid losing samples
static BYTE buf[2][RAWP_BUF_NS];
/// Its b0 bit records the frame in use of the read buffer
static BYTE frm;
/// Number of readed bytes in the last read operation
static UINT readed;
/// Signals if a data read operation is needed
static BYTE readNext;
/// Signals if we are playing a file
static BYTE play;

/************************************************************************//**
 * \brief This callback is called when more audio data is needed. It returns
 * a pointer to new data, and prepares the module to read a new chunck.
 *
 * \param[out] dataLen Lenght of the returned data buffer.
 *
 * \return Pointer to a new block of audio data.
 * \note This callback is ran in interrupt context
 ****************************************************************************/
BYTE* DataReadCallback(UINT* dataLen)
{
	/// Signal main() routine to read the next audio data chunck
	readNext = TRUE;
	/// Return the pointer and data length of the data
	*dataLen = readed;
	return buf[frm & 1];
}

void AmpEnable(void)
{
	TRISDbits.TRISD10 = 0;
	TRISDbits.TRISD11 = 0;

	LATDbits.LATD11 = 0;
	LATDbits.LATD10 = 1;
}

/************************************************************************//**
 * \brief Module initialization. Must be called once before using the other
 * functions in the module.
 *
 * \warning This module does NOT initialize FatFs. FatFs must be properly
 * initialized before using RawPlayFile() function.
 ****************************************************************************/
void RawPlayInit(void)
{
	readed = 0;
	readNext = FALSE;
	play = FALSE;
	frm = 0;
	/// pwmplay module initialization
	PwmPlayInit();
	// Just to do some tests
	AmpEnable();
}

/************************************************************************//**
 * \brief Plays an audio file.
 *
 * \params[in] Null terminated string with the name of the file to play.
 *
 * \return 0 if the file has been successfully played, nonzero otherwise.
 * \warning This function blocks using the Idle() function until playback
 * ends or is cancelled using RawPlayStop().
 ****************************************************************************/
BYTE RawPlayFile(char file[])
{
	/// Open audio file
	if (f_open(&f, file, FA_READ)) return 1;

	/// Pre allocate the first chunck of data
	if (f_read(&f, buf[frm], RAWP_BUF_NS, &readed))
	{
		f_close(&f);
		return 2;
	}

	/// Start playing the audio file
	PwmPlayStart(DataReadCallback);

	play = TRUE;

	while (play)
	{
		/// Wait until there is a data read event
		Idle();
		if (readNext)
		{
			/// Read the next data chunck
			readNext = FALSE;
			if (f_read(&f, buf[++frm&1], RAWP_BUF_NS, &readed))
			{
				f_close(&f);
				play = FALSE;
				return 3;
			}
			/// Check if finished playing
			if (readed < RAWP_BUF_NS) play = FALSE;
		}
	}
	/// Everything OK, close file and exit
	f_close(&f);
	return 0;
}

/************************************************************************//**
 * \brief Aborts a currently active playback.
 ****************************************************************************/
void RawPlayStop(void)
{
	PwmPlayStop();
	readNext = FALSE;
	play = FALSE;
	f_close(&f);
}

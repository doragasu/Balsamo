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

#ifndef _RAWPLAY_H_
#define _RAWPLAY_H_

#include "../types.h"

/************************************************************************//**
 * \brief Module initialization. Must be called once before using the other
 * functions in the module.
 *
 * \warning This module does NOT initialize FatFs. FatFs must be properly
 * initialized before using RawPlayFile() function.
 ****************************************************************************/
void RawPlayInit(void);

/************************************************************************//**
 * \brief Plays an audio file.
 *
 * \params[in] Null terminated string with the name of the file to play.
 *
 * \return 0 if the file has been successfully played, nonzero otherwise.
 * \warning This function blocks using the Idle() function until playback
 * ends or is cancelled using RawPlayStop().
 ****************************************************************************/
BYTE RawPlayFile(char file[]);

/************************************************************************//**
 * \brief Aborts a currently active playback.
 ****************************************************************************/
void RawPlayStop(void);

#endif /*_RAWPLAY_H_*/

/************************************************************************//**
 * \file  fsk_dem.h
 * \brief FSK demodulator module
 *
 * FSK demodulator consists of three blocks:
 * - Dephasor filter: Multiplies input signal by its ND samples delayed
 *   version. Output will have the demodulated signal, with a high frequency
 *   component.
 * - Low pass filter: Removes the high frequency component of the signal.
 * - Decisor block: Analyzes the demodulated signal, converting it into
 *   a series of output bytes.
 *              __________        __________        _________
 *             |          |      |          |      |         |
 *   INPUT ____| DEPAHSOR |______| LOW-PASS |______| DECISOR |_____ OUTPUT
 *             |  FILTER  |      |  FILTER  |      |  BLOCK  |
 *             |__________|      |__________|      |_________|
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

#ifndef _FSK_DEM_H_
#define _FSK_DEM_H_

#include "types.h"
#include "adc.h"

/** \defgroup fsk_dem_api fsk_dem
 *
 * FSK demodulator module, consists of three blocks:
 * - Dephasor filter: Multiplies input signal by its ND samples delayed
 *   version. Output will have the demodulated signal, with a high frequency
 *   component.
 * - Low pass filter: Removes the high frequency component of the signal.
 * - Decisor block: Analyzes the demodulated signal, converting it into
 *   a series of output bytes.
 * \{ */

/// Bitrate of the FSK signal in bps
#define FSK_BR			1200
/// Number of samples per bit
#define FSK_SPB			(FS/FSK_BR)
/// Number of cycles without carrier to trigger NO CARRIER condition
#define FSK_NO_CARRIER_CYCLES	(FSK_SPB * 10)

/************************************************************************//**
 * \brief Initializes the FSK demodulator. Must be called before starting
 * the demodulation process, and also each time the demodulation process
 * needs to be restarted
 ****************************************************************************/
void FskDemodInit(void);

/// Alias to FskDemodInit()
#define FskReset()		FskDemodInit()

/************************************************************************//**
 * \brief FSK demodulates a data block. Demodulated bytes are copied to the
 * output buffer.
 *
 * \param[in]  dataIn  FSK data from the ADC, to be demodulated
 * \param[out] dataOut Demodulated data bytes
 *
 * \return The number of bytes obtained and copied to dataOut buffer.
 *
 * \note
 * - dataIn must be located in X-data memory or the function will fail.
 * - Returned number of bytes will be at most NS/FSK_SPB + 1.
 ****************************************************************************/
int FskDemod(int dataIn[], BYTE dataOut[]);

/** \} */

#endif /*_FSK_DEM_H_*/

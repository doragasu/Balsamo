/*************************************************************************//**
 * \file adc.h
 * \brief Manages the internal 12 bit ADC. It configures the ADC for the 16
 * bit fractional mode and 16 samples per interrupt. Sampling frequency is
 * set to 7200 Hz (1600 * 6), generated by TIMER3. Only pin AN3 is sampled.
 *
 * \author Jesus Alonso Fernandez (doragasu)
 * \warning The module is not generalized for more than two frames.
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

#ifndef _ADC_H_
#define _ADC_H_

#include <dsp.h>

/** \defgroup adc_api adc
 *
 * Controls the internal 12 bit ADC. It configures the ADC for the 16
 * bit fractional mode and 16 samples per interrupt. Sampling frequency is
 * set to 7200 Hz (1600 * 6), generated by TIMER3. Only pin AN3 is sampled.
 * \{
 */

/// Sampling frequency
#define FS		7200
/// Number of samples of a block to be processed
#define NS		64
/// Number of frames of the input buffer
#define NF		2
/// Number of delays of the FSK demodulator
#define ND		3

/// Buffer for captured data. Length is the number of samples per frame by the
/// number of frames plus the number of delays used by the FSK demodulator
extern int data[NF*NS + ND];
/// Index pointing to the input samples of a COMPLETE buffer, starting from
/// the positions of the delays
extern int framePos;

/************************************************************************//**
 * \brief Initialises ADC module, including TIMER3.
 ****************************************************************************/
void AdcInit(void);

/************************************************************************//**
 * \brief Starts ADC (and indirectly, also TIMER3)
 ****************************************************************************/
void AdcStart(void);

/************************************************************************//**
 * \brief Stops ADC (and indirectly also TIMER3).
 ****************************************************************************/
void AdcStop(void);

/************************************************************************//**
 * \brief Gets the pointer to the last received pointer
 ****************************************************************************/
fractional* AdcGetBuf(void);

/** \} */

#endif /*_ADC_H_*/

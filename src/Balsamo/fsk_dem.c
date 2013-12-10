/************************************************************************//**
 * \file  fsk_dem.c
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

#include "fsk_dem.h"
#include <p30F6014.h>	// Chip definitions
#include <dsp.h>		// dsplib

/// Number of second order sections for the IIR filter
#define NUM_SEC		2

/// Returns the bigger out of two numbers
#define Max(a,b)	(((a)>(b))?a:b)
/// Returns the smaller out of two numbers
#define Min(a,b)	(((a)<(b))?a:b)

/// Decisor status
typedef enum
{
	DEC_WAIT_START,		///< Awaiting START bit
	DEC_DATA_RECV,		///< Receiving data
	DEC_WAIT_STOP		///< Awaiting STOP bit
}FskDecStat;

/// Information about the decision levels
typedef struct
{
	fractional thr;		///< Threshold
	fractional max;		///< Maximum value
	fractional min;		///< Minimum value
} DecLevel;

/// Filter coefficients, located in X-data memory.
/// Format: a2, a1, b2, b1, b0
fractional _XDATA(2) flpCoeff[NUM_SEC * 5] =
{
	  -1819,  9102,   2751,   5501,   2750,
	  -8227, 12306,  16382,  32766,  16384
};
/// Internal filter variables, located in Y-data memory.
fractional _YDATA(4) flpState[NUM_SEC * 2 * 2];

/// Filter data
IIRCanonicStruct flp =
{
	1,				///< numSectionsLess1
	flpCoeff,		///< coeffsBase (in X-Data or P-MEM)
	COEFFS_IN_DATA,	///< coeffsPage (coeficients in X-Data)
	flpState,		///< delayBase (filter internals, in Y-Data)
	8773,			///< initialGain (input gain in Q0.15
	1				///< finalShift (output shifts to the left)
};

/// Resets demodulator maximum and minimum to previous threshold value
#define FskLimitsReset()	(d.max = d.min = d.thr)

/// Updates demodulator maximum and minimum values
#define FskLimitsUpdate(data)	\
{								\
	d.max = Max(data, d.max);	\
	d.min = Min(data, d.min);	\
}

/// Updates demodulator threshold, using obtained maximum and minimum values
#define FskThrUpdate()		(d.thr = (d.max + d.min)>>1)


/// Buffer holding the output of the dephasor filter
static fractional _XDATA(2) dephased[64];
/// Buffer holding the output of the low-pass filter
static fractional tmp[NS];
/// Number of processed sample
static int count;
/// Received value (0, 1 or 2. 2 indicates nothing has been received).
static int recvVal;
/// Decisor state
static FskDecStat stat;
/// Temporal character
static BYTE tmpChar;
/// Number of bits received from a character
static int nBit;
/// Number of cycles without detecting carrier
static int noCarrier;
/// Decision threshold
static DecLevel d;

/*
 * PRIVATE FUNCTIONS
 */

/************************************************************************//**
 * \brief FSK coherent demodulation implementation.
 *
 * \param[in]  x ADC output data, to be FSK demodulated
 * \param[out] y Demodulated output data
 *
 * \note
 * - Implementation is assembly language coded inside fsk_dem.s file.
 * - x should contain NS+ND samples, with the first ND samples corresponding
 *   to the previous frame, and the following NS samples to the frame to be
 *   demodulated. y must contain NS elements.
 * - The same buffer can be used to hold input and output samples.
 ****************************************************************************/
void FskCoherentDemod(int x[], int y[]);

int FskDecisor(int dataIn[], BYTE dataOut[]);

/*
 * PUBLIC FUNCTIONS
 */

/************************************************************************//**
 * \brief Initializes the FSK demodulator. Must be called before starting
 * the demodulation process, and also each time the demodulation process
 * needs to be restarted
 ****************************************************************************/
void FskDemodInit(void)
{
	/// Initialize the low-pass filter
	IIRCanonicInit(&flp);
	/// Initialize the decisor block
	count = 0;
	stat = DEC_WAIT_START;
	noCarrier = ND;
	d.thr = 0;
	d.max = d.min = 0;
}

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
int FskDemod(int dataIn[], BYTE dataOut[])
{
	/// Dephasor filter
	FskCoherentDemod(dataIn, dephased);
	/// Low pass filter
	IIRCanonic(NS, tmp, dephased, &flp);
	/// Decisor
	return FskDecisor(tmp, dataOut);
}

/************************************************************************//**
 * \brief FSK detector decisor. Receives demodulated samples, detects if
 * their value is either 0 or 1, and groups them in bytes.
 *
 * \param[in]  dataIn  Demodulated samples (passed through dephasor and LPF)
 * \param[out] dataOut Obtained data bytes
 *
 * \return The number of bytes obtained from the input samples. It will be
 * at most NS/FSK_SPB + 1 bytes.
 ****************************************************************************/
int FskDecisor(int dataIn[], BYTE dataOut[])
{
	int i;
	int nChar = 0;

	/// \todo Implement hysteresis?
	/// \todo This CD algorithm is almost BROKEN, implement a decent one.
	for (i = 0; i < NS; i++)
	{
		FskLimitsUpdate(dataIn[i]);
		// Check for carrier
		if (dataIn[i] > d.thr)
		{
			noCarrier = 0;
			recvVal = 0;
		}
		else if (dataIn[i] < d.thr)
		{
			noCarrier = 0;
			recvVal = 1;
		}
		else
		{
			if (noCarrier < FSK_NO_CARRIER_CYCLES) noCarrier++;
			else
			{
				// No carrier, reset receiver status
				stat = DEC_WAIT_START;
				count = 0;
				d.thr = 0;
			}
		}

		if (noCarrier < FSK_NO_CARRIER_CYCLES)
		{
			// Treat received byte depending on receiver status
			switch (stat)
			{
				case DEC_WAIT_START:
					// Count zero bits to determine START condition
					if (recvVal == 0)
					{
						count++;
						if (count >= 4)
						{
							// START received
							stat = DEC_DATA_RECV;
							count = 0;
							nBit = 0;
							tmpChar = 0;
						}
					}
					else count = 0;
					break;

				case DEC_DATA_RECV:
					// Receive data bit by bit to complete a byte
					// TODO: Implement voting?
					count++;
					if (count == FSK_SPB)
					{
						tmpChar |= recvVal<<nBit;
						nBit++;
						count = 0;
						if (nBit == 8)
							// Received 8 data bits, wait for stop bit
							stat = DEC_WAIT_STOP;
					}
					break;

				case DEC_WAIT_STOP:
					// Just waits FSK_SPB bits
					count++;
					if (count > 4)
					{
						if (count > (FSK_SPB + 4))
						{
							// Failed to receive the stop bit
							stat = DEC_WAIT_START;
							count = 0;
						}
						else if (recvVal == 1)
						{
							// A complete byte has been received
							count = 0;
							stat = DEC_WAIT_START;
							dataOut[nChar++] = tmpChar;
							FskThrUpdate();
							FskLimitsReset();
						}
					}
					break;
			} // switch(stat)
		} // if (nocarrier < FSK_SPB)
	} // for (i ...)
	return nChar;
}

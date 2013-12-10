/************************************************************************//**
 * \file cid.c
 * \brief This module receives FSK data, demodulates it, and extracts the
 * Caller ID (CID) fields
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

#include "cid.h"
#include <stdio.h>

/// Seizure character
#define CID_SEIZURE_CHR		0x55
/// Number of seizure bytes
#define CID_SEIZURE_BYTES	27
/// Call setup identifier
#define CID_CALL_SETUP		0x80
/// Maximum number of tries receiving message type
#define CID_MAX_WRONG_MSG_TYPE	4

/// Possible machine states for the CID parser
typedef enum
{
	CID_SEIZURE_WAIT,	///<- Awaiting channel seizure
	CID_SEIZURE,		///<- Channel seizure in process
	CID_MSG_TYPE,		///<- Message Type receive in process
	CID_DATALEN,		///<- Data length receive in process
	CID_DATA			///<- Receiving payload data
} CidState;

/// Holds the required data for the CID state machine
typedef struct
{
	int idx;				///<- Index	in buffer, also used to count seizure
	int dataLen;			///<- Data length
	CidState state;			///<- machine state
	char complete;			///<- Signals when a complete frame is received
	unsigned char csum;		///<- checksum
	char buf[CID_BUFLEN];	///<- RX buffer
} Cid;

/// Caller ID data used for the module
static Cid cid;

/************************************************************************//**
 * \brief Resets the CID state machine to its default state. Must be done
 *        at least once before the FSK data of EACH call arrives.
 ****************************************************************************/
void CidReset(void)
{
	cid.state = CID_SEIZURE_WAIT;
	cid.complete = FALSE;
}

/************************************************************************//**
 * \brief Parse messages from the Presentation Layer.
 *
 * This function must be called only when a full message has been received
 * (i.e. CidParse returns CID_END). Each time it is called, it will parse a
 * message from the Presentation Layer (if available), and will advance to
 * the next message. It can be called until no more messages are available.
 *
 * \param[in]  msgLen Length of the data in the message buffer.
 * \param[out] msg    Pointer to the buffer storing the message data.
 * \return     Message identifier, or 0 if there are no more messages.
 * \warning Output buffer is NOT null terminated, and should NOT be modified.
 ****************************************************************************/
unsigned char CidPlMsgParse(int *msgLen, char **msg)
{
	unsigned char code;

	if (cid.idx < cid.dataLen)
	{
		code = cid.buf[cid.idx++];
		*msgLen = cid.buf[cid.idx++];
		*msg = &cid.buf[cid.idx];
		cid.idx += *msgLen;
		return code;
	}
	return 0;
}

/************************************************************************//**
 * \brief Processes received bytes to extract CID data sent by provider.
 *
 * \param[in] data Pointer to the buffer with received data.
 * \param[in] dataLen Length in octets of the data buffer.
 * \return
 * - CID_OK: Received data successfully processed. Awaiting more data.
 * - CID_ERROR: There was an error while processing data.
 * - CID_END: All CID data has been successfully processed.
 ****************************************************************************/
int CidParse(BYTE data[], int dataLen)
{
	int i;

	for (i = 0; i < dataLen; i++)
	{
		switch(cid.state)
		{
			case CID_SEIZURE_WAIT:
				if (data[i] == CID_SEIZURE_CHR)
				{
					cid.state = CID_SEIZURE;
					cid.idx = 1;
				}
				break;

			case CID_SEIZURE:
				// Count the number of seizure coincidences
				if (data[i] == CID_SEIZURE_CHR) cid.idx++;
				else if (cid.idx >= CID_SEIZURE_BYTES)
				{
					cid.idx = 0;
					if (data[i] == CID_CALL_SETUP)
					{
						// Received Call Setup message!
						cid.csum = data[i];
						cid.state = CID_DATALEN;
					}
					else
					{
						// Not received Call Setup message. Try receiving
						// it again, because the first received char after
						// seizure could be wrong because of the combination
						// of seizure bits and mark bits.
						/// \todo Check for other message types
						cid.state = CID_MSG_TYPE;
					}
				}
				else
				{
					// Error: not enough seizure bits received. Restart.
					cid.state = CID_SEIZURE_WAIT;
				}
				break;

			case CID_MSG_TYPE:
				if (data[i] == CID_CALL_SETUP)
				{
					// Received Call Setup message!
					cid.idx = 0;
					cid.csum = data[i];
					cid.state = CID_DATALEN;
				}
				else
				{
					cid.idx++;
					if (cid.idx >= CID_MAX_WRONG_MSG_TYPE)
						cid.state = CID_SEIZURE_WAIT;
				}
				break;

			case CID_DATALEN:
				// If the first received byte is CID_CALL_SETUP, then we
				// are receiving MSG_TYPE. Ignore byte to try again.
				if (data[i] != CID_CALL_SETUP)
				{
					// Check if data fits in the buffer
					if (data[i] > CID_BUFLEN) cid.state = CID_SEIZURE_WAIT;
					else
					{
						cid.dataLen = data[i];
						cid.csum += (unsigned char)data[i];
						cid.state = CID_DATA;
					}
				}
				break;

			case CID_DATA:
				if (cid.idx < cid.dataLen)
				{
					cid.buf[cid.idx++] = data[i];
					cid.csum += (unsigned char)data[i];
				}
				else
				{
					// Whatever happens, we will return to default state
					cid.state = CID_SEIZURE_WAIT;
					/// \todo Test computed checksum
					if (!((cid.csum + (unsigned char)data[i]) & 0xFF))
					{
						// Checksum OK!
						cid.complete = TRUE;
						cid.idx = 0;
						return CID_END;
					}
				}
				break;
		}
	}
	return CID_OK;
}

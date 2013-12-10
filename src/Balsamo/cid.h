/************************************************************************//**
 * \file cid.h
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

#ifndef _CID_H_
#define _CID_H_

#include "types.h"

/** \defgroup cid_api cid
 *
 * This module receives FSK data, demodulates it, and extracts the
 * Caller ID (CID) fields
 * \{ */

/// Buffer length for the CID demodulated data
#define CID_BUFLEN	128

/// The function call succeeded
#define CID_OK       0
/// The function call errored
#define CID_ERROR   -1
/// The function call succeeded and CID parsing is complete
#define CID_END      1

//
/** \defgroup presentation_codes Some useful Presentation Layer message codes
 * \{
 */
/// Date and time
#define CID_MSG_DATE_TIME		0x01
/// Calling Line Identity
#define CID_MSG_CLI_A			0x02
/// Calling Line Identity
#define CID_MSG_CLI_B			0x03
/// Reason for the CLI absence
#define CID_MSG_CLI_ABS_REASON	0x04
/// Calling party name
#define CID_MSG_CP_NAME			0x07
/** \} */

/** \defgroup cli_abs_reason Possible reasons for CLI absence
 * \{
 */
/// CLI unavailable
#define CID_ABS_UNAVAILABLE		0x4F
/// CLI private
#define CID_ABS_PRIVATE			0x50
/** \} */

/** \defgroup cid_msg_lengths Some message lengths
 * \{
 */
/// Maximum telephone number length
#define CID_TELNUM_MAX_LEN		20
/// Date and time length
#define CID_DATE_TIME_LEN		 8
/// Length of CLI absence reason
#define CID_CLI_ABS_REASON_LEN	 1
/// Maximum length of Calling Party Name
#define CID_CP_NAME_MAX_LEN		50
/** \} */

/************************************************************************//**
 * \brief Resets the CID state machine to its default state. Must be done
 *        at least once before the FSK data of EACH call arrives.
 ****************************************************************************/
void CidReset(void);

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
int CidParse(BYTE data[], int dataLen);

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
unsigned char CidPlMsgParse(int *msgLen, char **msg);

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
unsigned char CidPlMsgParse(int *msgLen, char **msg);

/** \} */

#endif /*_CID_H_*/

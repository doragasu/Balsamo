/************************************************************************//**
 * \file  tel_filt.h
 * \brief Telephone keeping and filtering module. Allows to store telephone
 * numbers in a rudimentary phone book, and also to filter them.
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

#ifndef _TEL_FILT_H_
#define _TEL_FILT_H_

/** \defgroup tel_filt_api tel_filt
 *
 * Telephone keeping and filtering module. Allows to store telephone
 * numbers in a rudimentary phone book, and also to filter them.
 * \} */

/// Blacklist mode. Each number in the list must be blacklisted
#define TF_MODE_BLACKLIST	0
/// Whitelist mode. Each number NOT in the list must be blacklisted
#define TF_MODE_WHITELIST	1

/// The checked number is not blacklisted
#define TF_NUM_OK			0
/// The checked number is blacklisted or not in the whitelist
#define TF_NUM_REJECT		1
/// The checked number should be rejected, but call filter is disabled
#define TF_FILTER_DISABLED	2
/// Hidden calls are OK
#define TF_HID_OK			3
/// Hidden calls must be rejected
#define TF_HID_REJECT		4
/// Hidden calls should be rejected but filter is disabled.
#define TF_HID_DISABLED		5
/************************************************************************//**
 * \brief Module initialization. Must be called before using any other
 * function.
 *
 * \param[in]  filterMode Filtering mode, either TF_MODE_BLACKLIST or
 *             TF_MODE_WHITELIST.
 ****************************************************************************/
void TfInit(char filterMode);

/************************************************************************//**
 * \brief Adds a number to the phone book
 *
 * \param[in] number Telephone number to add to the phone book.
 *
 * \return 0 if OK, nonzero otherwise.
 ****************************************************************************/
char TfNumAdd(char number[]);

/************************************************************************//**
 * \brief Checks if a telephone number is blacklisted.
 *
 * \param[in] number Telephone number to check.
 *
 * \return TF_NUM_OK if the number is allowed, TF_NUM_REJECT if the number
 * is blacklisted (or not in the whitelist), TF_NUM_FILTER_DISABLED if the
 * number should be rejected, but call filter is disabled.
 ****************************************************************************/
char TfNumCheck(char number[]);

/************************************************************************//**
 * \brief Parses configuration file stored inside the microSD card. It
 * configures the blacklist/whitelist mode and adds previously stored
 * numbers to the phone book.
 *
 * \return 0 if OK, nonzero otherwise.
 ****************************************************************************/
char TfParseConfig(void);

/************************************************************************//**
 * \brief Tells if hidden numbers are either allowed or filtered out.
 *
 * \return TF_HID_OK if hidden calls must not be filtered, TF_HID_REJECT if
 * hidden calls must be rejected, or TF_HID_DISABLED if hidden call should
 * be rejected but call filter is disabled.
 ****************************************************************************/
char TfFilterHidden(void);

/************************************************************************//**
 * \brief Gets the first number stored in the telephone book.
 *
 * \return The first number stored in the telephone book, or NULL if there
 * is no one.
 ****************************************************************************/
char *TfNumGetFirst(void);

/************************************************************************//**
 * \brief Gets the next number stored in the telephone book.
 *
 * \return The next number stored in the telephone book, or NULL if there
 * are no more numbers in the telephone book.
 ****************************************************************************/
char *TfNumGetNext(void);

/************************************************************************//**
 * \brief Gets the previous number stored in the telephone book.
 *
 * \return The previous number stored in the telephone book, or NULL if there
 * are no more numbers in the telephone book.
 ****************************************************************************/
char* TfNumGetPrev(void);

/************************************************************************//**
 * \brief Deletes the current telephone number from the telephone book.
 ****************************************************************************/
void TfNumDelete(void);

/************************************************************************//**
 * \brief Saves the current configuration and phone book to the SD card.
 *
 * \return 0 if OK, nonzero otherwise.
 ****************************************************************************/
char TfCfgSave(void);

/************************************************************************//**
 * \brief Disables call filtering.
 ****************************************************************************/
void TfDisable(void);

/************************************************************************//**
 * \brief Enables call filtering.
 ****************************************************************************/
void TfEnable(void);

/** \} */

#endif /*_TEL_FILT_H_*/

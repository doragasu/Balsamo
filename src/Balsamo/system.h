/************************************************************************//**
 * \file  system.h
 * \brief State machine implementation to control the system.
 *
 * \author Jesus Alonso Fernandez (doragasu)
 * \note   Queue implementation is LIFO. A FIFO implementation makes more
 *         sense, but takes a few more lines to implement and I'm lazy.
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

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

/** \defgroup system_api system
 *
 * State machine implementation to control the system.
 * \{ */

/// Maximum number of queued system events
#define SYS_MAX_EV	10

/// Available system states
typedef enum
{
	SYS_SLEEP,				///< Sleep state, nothing to do
	SYS_RING_TIM,			///< RING detected, wait 500 ms
	SYS_DATA_RECV,			///< Receive FSK data
	SYS_LINE_HANG_WAIT,		///< Wait state before hanging
	SYS_RING_END_WAIT		///< Wait until ringing stops to return to sleep
} SysStat;

/// Events parsed by the system state machine
typedef enum
{
	SYS_NONE,               ///< No event
	SYS_RING,               ///< RING detected
	SYS_TIM_EVT,            ///< Timer event
	SYS_DATA,               ///< FSK data received from ADC
	SYS_CID_INFO,           ///< CID information (Unused)
	SYS_CALL_ALLOWED,		///< Call allowed (used only for UI)
	SYS_CALL_RESTRICTED,	///< Call restricted (used only for UI)
	SYS_CALL_END,			///< End of call (used only for UI)
	SYS_KEY_UP,             ///< UP-LEFT keyboard event
	SYS_KEY_DOWN,           ///< DOWN-RIGHT keyboard event
	SYS_KEY_ENTER,          ///< ENTER keyboard event
	SYS_KEY_ESC,            ///< ESC keyboard event
	SYS_KEY_FN,             ///< FN keboard event (unused)
	SYS_SLEEP_TIM,			///< Sleep timer event
	SYS_RTC_MINUTE			///< RTC has incremented a minute
} SysEvent;

/// System status
extern SysStat  sysStat;

/************************************************************************//**
 * \brief Initializes the queue used to send system events to the FSM.
 ****************************************************************************/
void SysQueueInit(void);

/************************************************************************//**
 * \brief Enqueues an event
 *
 * \param[in] event Event to add to the system queue
 *
 * \warning Must NOT be called from interrupt context.
 ****************************************************************************/
void SysQueuePut(SysEvent event);

/************************************************************************//**
 * \brief Enqueues an event
 *
 * \param[in] event Event to add to the system queue
 *
 * \warning Must ONLY be called from interrupt context.
 ****************************************************************************/
void SysIQueuePut(SysEvent event);

/************************************************************************//**
 * \brief Extracts an event from the system queue.
 *
 * \return The extracted event. If no event was in the queue when this
 * function is called, SYS_NONE will be returned.
 ****************************************************************************/
SysEvent SysQueueGet(void);

/** \} */

#endif /*_SYSTEM_H_*/

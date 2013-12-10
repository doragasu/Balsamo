/************************************************************************//**
 * \file  system.c
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

#include "system.h"
#include "types.h"
#include "common.h"
#include "keyb.h"

/// System queue
typedef struct
{
	SysEvent ev[SYS_MAX_EV];    ///< Array holding the queued events
	BYTE idx;                   ///<
} SysEvQueue;

/// The actual system queue
static SysEvQueue q;

/************************************************************************//**
 * \brief Initializes the queue used to send system events to the FSM.
 ****************************************************************************/
void SysQueueInit(void)
{
	q.idx = 0;
}

/************************************************************************//**
 * \brief Enqueues an event
 *
 * \param[in] event Event to add to the system queue
 *
 * \warning Must ONLY be called from interrupt context.
 ****************************************************************************/
void SysIQueuePut(SysEvent event)
{
    /// \todo Make sure other higher priority interrupts are not dispatched
    /// while in here.
	q.ev[q.idx++] = event;
}

/************************************************************************//**
 * \brief Enqueues an event
 *
 * \param[in] event Event to add to the system queue
 *
 * \warning Must NOT be called from interrupt context.
 ****************************************************************************/
void SysQueuePut(SysEvent event)
{
	/// Ensure queue manipulation is atomic
	_DI();
	/// Add the event to the queue
	q.ev[q.idx++] = event;
	/// \todo: It should be possible to avoid the _EI() macro by
	/// carefully timing the time interrupts are disabled via _DI()
	_EI();
}

/************************************************************************//**
 * \brief Extracts an event from the system queue.
 *
 * \return The extracted event. If no event was in the queue when this
 * function is called, SYS_NONE will be returned.
 ****************************************************************************/
SysEvent SysQueueGet(void)
{
	SysEvent ret;
	/// Ensure queue manipulation is atomic
	_DI();
	/// Extract the event (if available)
	if (q.idx) ret = q.ev[--q.idx];
	else ret = SYS_NONE;
	_EI();
	return ret;
}

/** \defgroup keyb_handlers Keyboard callback handlers. They just add to the
 * system queue an event corresponding to the pressed key
 * \{
 */
/// UP-LEFT key press handler
void KeybUpLeftPress(void)
{
	SysIQueuePut(SYS_KEY_UP);
}

/// DOWN-RIGHT key press handler
void KeybDownRightPress(void)
{
	SysIQueuePut(SYS_KEY_DOWN);
}

/// ENTER key press handler
void KeybEnterPress(void)
{
	SysIQueuePut(SYS_KEY_ENTER);
}

/// ESC key press handler
void KeybEscPress(void)
{
	SysIQueuePut(SYS_KEY_ESC);
}
/** \} */

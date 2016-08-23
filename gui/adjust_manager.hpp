/********************************************************************************
 *
 *  Adjust Manager:
 *
 *  Manage the sending and acknowledgement of ProcedureAdjustment messages.
 *
 *  Any screen can use the 'sendAdustment' function call.  However, screens
 *  wishing to get callbacks need to explicitly 'attach' and set the callbacks
 *  when coming up, and 'detach' when shutting down.
 *
 ********************************************************************************/

#ifndef ADJUST_MANAGER_HPP
#define ADJUST_MANAGER_HPP

#include "adjust.h"
#include "callback.h"
#include "manager_base.hpp"

class Screen_TROUBLE;


namespace AdjustManager
{

void sendAdjustment (troubleshooting_adjustments type, int line, const char* fn, const char* file);

// Note: attaching to the AdjustManager automatically sets up the correct
// callbacks into the attaching screen.  NOTE: it is safe for a given screen to
// attach more than once.
void attach (Screen_TROUBLE*);

// Note: if a screen other than the one currently attached tries to detach, it's
// a no-op (that is, it's safe to do); the screen trying to detach has, in this
// case, already been detached (it got preempted when the other guy attached).
void detach (Screen_TROUBLE*);

void reset ();

bool waiting ();
}

#endif

/* FORMAT HASH 382fe2c3bd5b32f347b77a512c781aab */

/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.2  2002/08/07 17:45:37Z  pn02526
 * Add needleMonitor::_myTimer to use the new timer message class definition, rather than deriving from a timer message class.
 * Revision 1.1  2000/03/15 11:28:39  BS04481
 * Initial revision
 *
 * TITLE:      needle.hpp
 *             Safety needle monitor class definitions.
 *
 *
 * ABSTRACT:   This file contains the class definitions for the
 *             needle coagulation monitor that makes sure the pump moves
 *             during pauses and PFRs
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/

#ifndef NEEDLE
#define NEEDLE

#include "timermessage.h"
#include "callback.h"

#include "s_cont.hpp"


/*******************************************************************************
*
*  needleMonitor CLASS DEFINITION
*
******************************************************************************/
class needleMonitor
{
public:
   needleMonitor(InletContainer* Inlet,
                 ReturnContainer* Return);
   ~needleMonitor();
   void initializeForState (long newstate);
   void turnOnTimer (long duration);
   bool isTimerRunning ();
   bool isEnoughPumped ();
   void restoreFromPFR ();

private:
   void turnOffTimer ();
   void saveToPFR ();
   void notify (void);

   InletContainer*  _Inlet;
   ReturnContainer* _Return;
   TimerMessage     _myTimer;
   MON_TYPE         _monType;
   long             _startTime;
   long             _duration;
   bool             _timerHasFired;
   float            _pumpTimerInletAccumulator;
   float            _pumpTimerReturnAccumulator;
};

#endif   // NEEDLE

/* FORMAT HASH 2a7816feefef7faddf5d644f910c1afe */

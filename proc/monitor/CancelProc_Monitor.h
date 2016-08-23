/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      CancelProc_Monitor.h
 *
 * ABSTRACT:  This is the include file for the Cancel Procedure monitor.
 *
 */


#ifndef CancelProc_MONITOR_H
#define CancelProc_MONITOR_H

#include "aPQMonitor.h"
#include "objdictionary.h"
#include "trimamessages.h"

class CancelProc_Monitor
   : public aPQMonitor
{
   // Declare this class to the object dictionary
   DECLARE_OBJ(CancelProc_Monitor);

public:

   // Constructor, called at the time the "states" file is read in.
   CancelProc_Monitor();

   // Destructor
   virtual ~CancelProc_Monitor();

   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();

   // Notify function, called whenever a CancelProcedureMsg is received from GUI,
   // indicating that the operator has pressed the "Cancel Procedure" button.
   void Notify ();

   // Initialization, called by the state machine AFTER the Initialization()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void Initialize ();

   // Function to enable monitoring, called by the state machine AFTER the preEnter()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void enable ();

   // Function to disable monitoring, called by the state machine BEFORE the postExit()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void disable ();

private:

   CancelProcedureMsg _CancelProcedureMsg;
};


#endif

/* FORMAT HASH 4f65cd9d87358e8bab0d7c5577f8f0c4 */

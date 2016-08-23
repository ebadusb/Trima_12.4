/*********************************************************************
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      InletVolmAlarm.h
 *
 *   This alarm monitors the total volm run by the inlet pump and
 *   alarms if it exceeds the CVR value.  During MSS only.
 *
 **********************************************************************
 *  $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/alarm/rcs/inletVolmAlarm.h 1.4 2006/05/19 19:03:06Z jheiusb Exp jheiusb $
 *  $Log: inletVolmAlarm.h $
 *  Revision 1.4  2006/05/19 19:03:06Z  jheiusb
 *  7549  -- fix cassette state in leeak detecter for MSS
 *  Revision 1.3  2006/03/30 20:49:59Z  jheiusb
 *  7350 -- MSS PFR
 *  Revision 1.2  2006/03/03 17:27:59Z  jheiusb
 *  initial merge
 *  Revision 1.1  2006/03/03 17:13:17Z  jheiusb
 *  7428
 *  Revision 1.2  2005/11/03 22:58:43Z  jheiusb
 *  7230 -- inletVolmAlarm added
 *  Revision 1.1  2005/11/03 18:24:27Z  jheiusb
 *  Initial revision
 *
 *
 ***********************************************************************/

#ifndef _INLET_VOLM_ALARM_H
#define _INLET_VOLM_ALARM_H

#include "monitorbase.h"
#include "objdictionary.h"
#include "an_alarm.h"
#include "trimamessages.h"
#include "haldefs.h"

class InletVolmAlarm
   : public MonitorBase
{
   DECLARE_OBJ(InletVolmAlarm);


public:
   InletVolmAlarm ();
   virtual ~InletVolmAlarm ();

   virtual void Monitor ();

   void setup ();


   // Function to enable monitoring, called by the state machine AFTER the preEnter()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void enable ();

   // Function to disable monitoring, called by the state machine BEFORE the postExit()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void disable ();

private:
   anAlarmMsg _inletVolmAlarm;
   float      _accumulated;
   bool       _alarmed;
   float      _cvl;
   float      _cvr;
   bool       _setupDone;
};


#endif

/* FORMAT HASH 49f776d4e0b0729a56db8486b8fc9597 */

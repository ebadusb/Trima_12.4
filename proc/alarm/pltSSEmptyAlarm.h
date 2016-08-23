/*********************************************************************
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ss_prime2soon_alarm.h
 *
 * ABSTRACT:  This alarm will be active during the MSS Prime substate.  It has two
 * alarms: SS Prime too soon, and SS prime delay.
 *
 * SS prime too soon is triggered by a reservoir status change prior to
 * mss_rbc_ss_min_prime_vol (CC). If this alarm is triggered the only choice is to
 * go to the SS Disconnect substate.  Storage solution must be added offline. If
 * PLT MSS is also occurring, do not goto MSS Disconnect substate, continue with
 * MSS for Platelets.
 *
 **********************************************************************
 *
 *  $Log: pltSSEmptyAlarm.h $
 *  Revision 1.2  2006/03/03 17:28:11Z  jheiusb
 *  initial merge
 *  Revision 1.1  2006/03/03 17:13:27Z  jheiusb
 *  7428
 *  Revision 1.2  2006/01/21 21:36:11Z  jheiusb
 *  7058 -- add plt bag running empty alarm
 *  Revision 1.1  2006/01/20 22:36:56Z  jheiusb
 *  Initial revision
 *  Revision 1.2  2005/10/06 14:56:37Z  dslausb
 *  IT 7202 - Metered Storage
 *  - Fixed MKS Revision History Tag
 *  - Implemented Monitor() Function
 *  - Improved Logging
 *
 *
 ***********************************************************************/

#ifndef _SS_PLT_SS_EMPTY_H_
#define _SS_PLT_SS_EMPTY_H_

#include "monitorbase.h"
#include "objdictionary.h"
#include "an_alarm.h"
#include "trimamessages.h"
#include "haldefs.h"

class PltSSEmpty
   : public MonitorBase
{
   DECLARE_OBJ(PltSSEmpty);
public:
   PltSSEmpty ();
   virtual ~PltSSEmpty ();

   void         Init ();
   virtual void Monitor ();

protected:
   void remainingPASneeded ();

private:
   anAlarmMsg _empty;
   int        _cnt;


   Gui2ProcRequestRemainingPasVolMsg _requestPASVolmMsg;
};


#endif

/* FORMAT HASH 649b5e9ae6791d37776eb094d85b2014 */

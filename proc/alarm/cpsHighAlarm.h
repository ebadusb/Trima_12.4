/***********************************************************
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cpsHighAlarm.h
 *
 * ABSTRACT:  This alarm will be active during Metered Storage State.  It is triggered
 * anytime the CPS pressure exceeds mss cps_high (CC).  If this alarm is triggered the
 * only choice is to goto the SS Disconnect substate.  Storage solution must be added
 * offline.
 ************************************************************
 *
 *  $Log: cpsHighAlarm.h $
 *  Revision 1.3  2007/10/26 16:49:32Z  jheiusb
 *  8082 -- PTF CPS alarms end product based
 *  Revision 1.2  2007/10/18 20:00:40Z  jheiusb
 *  8254 -- remove the  Autosplit
 *  Revision 1.1  2006/03/03 23:06:28Z  jheiusb
 *  Initial revision
 *  Revision 1.1  2005/10/06 19:07:55Z  dslausb
 *  Initial revision
 *  Revision 1.3  2005/10/05 20:10:20Z  dslausb
 *  IT 7202 - Metered Storage
 *  - Fixed MKS Revision History Tag
 *  - Improved Logging
 *
 *
 ************************************************************/

#ifndef _CPS_HIGH_ALARM_H_
#define _CPS_HIGH_ALARM_H_

#include "monitorbase.h"
#include "objdictionary.h"
#include "an_alarm.h"

class cpsHighAlarm
   : public MonitorBase
{
   DECLARE_OBJ(cpsHighAlarm);
public:
   cpsHighAlarm ();
   virtual ~cpsHighAlarm ();

   virtual void Monitor ();

private:

   anAlarmMsg CPShighAlarm;
};


#endif

/* FORMAT HASH b81aebe5e2fcc3bb54090c3bca763632 */

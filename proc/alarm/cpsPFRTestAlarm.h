/***********************************************************
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      .h
 *.
 ************************************************************
 *
 *  $Log: cpsPFRTestAlarm.h $
 *  Revision 1.1  2007/07/03 15:31:33Z  jheiusb
 *  Initial revision
 *
 *
 ************************************************************/

#ifndef _CPS_PFR_TEST_H_
#define _CPS_PFR_TEST_H_

#include "monitorbase.h"
#include "objdictionary.h"
#include "an_alarm.h"

class cpsPFRTestAlarm
   : public MonitorBase
{
   DECLARE_OBJ(cpsPFRTestAlarm);
public:
   cpsPFRTestAlarm ();
   virtual ~cpsPFRTestAlarm ();

   virtual void Monitor ();

private:
   anAlarmMsg _PFRTestAlarm;
   bool       _done;
};


#endif

/* FORMAT HASH f9775435546b6ffe14f4c3964ef2194c */

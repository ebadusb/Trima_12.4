/*********************************************************************
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:
 *
 * ABSTRACT:
 *
 **********************************************************************
 *
 *  $Log: rbcSSEmptyAlarm.h $
 *  Revision 1.2  2007/02/16 22:12:52Z  jheiusb
 *  add RBC ss bag alarm
 *  Revision 1.1  2007/01/31 23:26:42Z  jheiusb
 *  Initial revision
 *
 *
 ***********************************************************************/

#ifndef _SS_RBC_SS_EMPTY_H_
#define _SS_RBC_SS_EMPTY_H_

#include "monitorbase.h"
#include "objdictionary.h"
#include "an_alarm.h"
#include "trimamessages.h"
#include "haldefs.h"

class RbcSSEmpty
   : public MonitorBase
{
   DECLARE_OBJ(RbcSSEmpty);
public:
   RbcSSEmpty ();
   virtual ~RbcSSEmpty ();

   virtual void Monitor ();

private:
   anAlarmMsg _emptySrbc, _emptyDrbc;
   int        _cnt;
};


#endif

/* FORMAT HASH e70b2db860b4f448bc2e1b4b5ce3df0e */

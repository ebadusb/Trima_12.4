/*
 * Copyright (c) 2004 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cpsLogMonitor.h
 *
 * ABSTRACT:  This is the include file for the cps periodic log monitor class.  It is derived of class MonitorBase.
 *
 */


#ifndef CPS_LOG_MONITOR_H
#define CPS_LOG_MONITOR_H

#include "datalog.h"
#include "monitorbase.h"
#include "objdictionary.h"
#include "trimamessages.h"


class cpsLogMonitor
   : public MonitorBase
{
   DECLARE_OBJ(cpsLogMonitor);

public:

   cpsLogMonitor();

   // Destructor
   virtual ~cpsLogMonitor();

   // Initialization
   virtual void Initialize ();

   virtual void Monitor ();

   // Callback for FeatureChanged message
   void featureChanged ();

protected:

   bool _WasNotLogging;

private:
   DataLog_SetHandle _DataLogSet;
   FeatureChanged    _FeatureChanged; // gets notified on Feature changes
   bool              _featureEnabled;
};

#endif

/* FORMAT HASH f42bb0fed28d9900ddfb99c9d5f38b9e */

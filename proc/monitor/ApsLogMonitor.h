/*
 * Copyright (c) 2004 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ApsLogMonitor.h
 *
 * ABSTRACT:  This is the include file for the aps periodic log monitor class.
 *            It is derived from the class MonitorBase.
 *
 */


#ifndef APS_LOG_MONITOR_H
#define APS_LOG_MONITOR_H


#include "datalog.h"
#include "monitorbase.h"
#include "objdictionary.h"
#include "trimamessages.h"


// Constants
static const int BUFFER_SIZE = 256;

class ApsLogMonitor
   : public MonitorBase
{
   DECLARE_OBJ(ApsLogMonitor);

public:

   ApsLogMonitor();

   // Destructor
   virtual ~ApsLogMonitor();

   // Initialization
   virtual void Initialize ();

   virtual void Monitor ();

   // Callback for FeatureChanged message
   void featureChanged ();

protected:
   bool  _logNow;
   bool  _wasNotLogging;
   float _currentAPS;
   float _apsHighLimit;
   float _apsLowLimit;

private:
   DataLog_SetHandle _DataLogSet;
   FeatureChanged    _FeatureChanged; // gets notified on Feature changes
   bool              _featureEnabled;
};

#endif

/* FORMAT HASH 99f509ef610fe07c80e8d6008a78e13b */

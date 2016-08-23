/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      aPQMonitor.h
 *
 * ABSTRACT:  Utility class for PQI.  Contains common code
 *
 */


#ifndef PQ_MONITOR_H
#define PQ_MONITOR_H

#include <strstream.h>

#include "datalog.h"
#include "monitorbase.h"
#include "pqcontrol.h"
#include "procdata.h"


// Constants
static const int BUFFER_SIZE = 256;

class aPQMonitor
   : public MonitorBase
{

protected:

   static PQControl     _pqi;

   static DataLog_Level _pqLog;
   static char          _logBuffer[BUFFER_SIZE];
   static ostrstream    _sout;

   static bool          _Initialized;

public:

   // Constructor, called at the time the "states" file is read in.
   aPQMonitor();

   // Destructor
   virtual ~aPQMonitor();

   // Monitor function, called each dispatcher cycle
   // virtual void Monitor( );

   // Initialization
   virtual void Initialize ();

   // Function to enable monitoring ...
   // virtual void enable();

   // Function to disable monitoring ...
   // virtual void disable();

private:

   const char* theFlagName (ENUMERATED_FLAG theFlag);

   const char* theReasonName (ENUMERATED_REASON theReason);

protected:

   // Logs the given reason only if not logged before.
   void LogReason (ENUMERATED_REASON theReason, const char* file = __FILE__, int line = __LINE__);

   // Logs the given reason for the given flag only if not logged before.
   void LogReason (ENUMERATED_REASON theReason, ENUMERATED_FLAG theFlag, const char* file = __FILE__, int line = __LINE__);

   // This method computes and returns the flow that has been commanded through the LRS chamber
   float LRSChamberCmdFlow () const;

   // This method computes and returns the flow that has been commanded through the RBC line
   float RbcLineCmdFlow () const;

   // This method returns "true" if the procedure has been directing fluid into the platelet bags.  Otherwise, it returns "false".
   bool flowingIntoPlateletBag () const;

   // This method returns "true" if the procedure has been collecting platelets actively.  Otherwise, it returns "false".
   bool nowCollectingPlatelets () const;

   // This method returns "true" if the procedure has been diluting platelets actively.  Otherwise, it returns "false"0.
   bool nowAdjustingPlateletConcentration () const;

   // This method returns "true" if the procedure has been collecting plasma actively.  Otherwise, it returns "false"0.
   bool nowCollectingPlasma () const;

   // This method returns "true" if the procedure has been collecting RBCs actively.  Otherwise, it returns "false"0.
   bool nowCollectingRbc () const;

   // This method returns "true" if all pumps have been commanded paused (0.0 ml/min).
   bool nowPaused () const;

   bool plasmaRinsebackEnabled () const;

   // Calculate the current instantaneous Red/Green ratio of the RBC detector,
   // making sure not to commit a "divide by 0" error.
   float RGratio () const;

   // Upon being called, processes the particular reason "Run Ended Early" by setting reasons for verify flag(s).
   void RunEndedEarly ();

   // This method returns "true" if the procedure is in a Purge Recovery.  Otherwise, it returns "false".
   //
   inline bool nowPurging ()
   {
      ProcData pd;
      return( RecoveryTypes::RecoveryId(pd.RecoveryName().c_str() ) == RecoveryTypes::WBCChamberPurge );
   }

   // This method returns "true" if the procedure is in a Centrifuge Stop Recovery.  Otherwise, it returns "false".
   //
   inline bool nowRecoveringCentrifugeStop ()
   {
      ProcData pd;
      return( RecoveryTypes::RecoveryId(pd.RecoveryName().c_str() ) == RecoveryTypes::CentrifugeStopScaled );
   }


   // This method returns "true" if the procedure is in an Air Block Recovery.  Otherwise, it returns "false".
   //
   inline bool nowRecoveringAirBLock ()
   {
      ProcData pd;
      return( RecoveryTypes::RecoveryId(pd.RecoveryName().c_str() ) == RecoveryTypes::AirBlock );
   }

   // derived:
   // Set/Get the enable monitoring flag
   //   void enableMonitoring( const int flag );
   //   const int enableMonitoring();

};

// Note.  Must call logging statement before setting flag or logging statement will be muted.
#define LogIt(reason) LogReason(reason, __FILE__, __LINE__);
#define LogIt2(reason, flag) LogReason(reason, flag, __FILE__, __LINE__);

#endif

/* FORMAT HASH f563b62cfe1a9315ed535c2623bc4dd7 */

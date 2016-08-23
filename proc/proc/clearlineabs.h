/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      clearlineabs.h
 *
 */

#ifndef __CLEAR_LINE_ABS_H__
#define __CLEAR_LINE_ABS_H__

#include "recoveryabs.h"
#include "trimamessages.h"
#include "procdata.h"

class ClearLineAbs
   : public RecoveryAbs
{
public:

   //
   // Constructor
   ClearLineAbs();

   //
   // Copy constructor
   ClearLineAbs(const ClearLineAbs& rec);

   //
   // Destructor
   virtual ~ClearLineAbs();

   //
   // Initialize function
   //
   virtual int init ();

   //
   // function which does the recovery, or at least a step of
   //  the recovery.  When the recovery is complete, the
   //  recoveryNeeded function will reflect the information.
   //
   virtual int doRecovery ();

   //
   // function to control whether this recovery may be performed
   //  given the current system state and data
   //
   virtual int mayEnter () const;

   //
   // Function to reset the spillover if it gets preempted by another
   //  recovery
   //
   virtual void reset ();

   //
   // Callback for the spillover request message
   //
   virtual void spilloverRequest ();

   //
   // Callback for the clear line request message
   //
   virtual void clearLineRequest ();

   //
   // Function which may be overridden if the recovery is an exception which
   //  is expected and needed during the run ...
   virtual RecoveryTypes::InRecovery recoveryExpected ();

protected:

   //
   // Function to copy the dynamic data for the object
   //
   virtual void copyOver (const ClearLineAbs& rec);

   //
   // Function to delete the objects dynamic memory
   //
   virtual void cleanup ();

   //
   // Function to set the pump speeds
   //
   virtual void setPumpSpeeds (ProcData& pd) = 0;

protected:

   //
   // Spillover request message
   //
   SpilloverRecoveryMsg _SpilloverReqMsg;

   //
   // Clear line request message
   //
   ClearLineMsg _ClearLineReqMsg;

   //
   // Flag to signify a spillover recovery has been requested
   //
   int _SpilloverRequested;

   //
   // Flag to signify a clear line recovery has been requested
   //
   int _ClearLineRequested;

   //
   // Flag to signify the spillover has been cleared
   //
   int _LineCleared;

   //
   // Volume processed thru the collect line
   //
   float _CollectVolume;

   //
   // Flag to signify initial clearlineabs volume has been set
   //
   int _VinClearLineSet;
};

#endif

/* FORMAT HASH 26ee3d531062cc8ce1aab8b5298f1d36 */

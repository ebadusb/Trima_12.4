/*
 * Copyright (c) 2011 by CaridianBCT, Inc.  All rights reserved.
 *
 *
 */


#ifndef _PLASMALINEOCCLUSIONPCA_H_
#define _PLASMALINEOCCLUSIONPCA_H_

// Include files
#include <vxworks.h>
#include "aPQMonitor.h"
#include "pfrmsg.h"
#include "trimamessages.h"


class PlasmaLineOcclusionPCA_Monitor
   : public aPQMonitor
{
   DECLARE_OBJ(PlasmaLineOcclusionPCA_Monitor);

public:
   // Default constructor
   PlasmaLineOcclusionPCA_Monitor();

   // Destructor
   virtual ~PlasmaLineOcclusionPCA_Monitor();

   // Initialize the class
   virtual void Initialize ();

   virtual void enable ();

   virtual void disable ();

   //
   // Function called every status update ...
   virtual void Monitor ();

protected:
   void HandleSubstateChangeMsg ();
   void HandlePfrStatusMsg ();
   void Deactivate ();

private:
   static ProcData* _pd;

   float            _pcaEndPltVolWindow;
   float            _pcaStartPltVolWindow;
   float            _redReflectanceTrigger;


   ProcSubstateChangeMsg _substateChangeMsg;     // Substate change message
   Message<PFR_Status>   _pfrStatusMsg;          // PFR status message
   bool                  _featureEnabled;
};


#endif

/* FORMAT HASH 578b71aed84abdb86a7c69e02dfc6a9b */

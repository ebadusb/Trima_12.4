/*
 * Copyright (c) 2008 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pasFinalFlowVerification.h
 *
 */

/*
$Header:
*/


#ifndef PAS_FINAL_FLOW_H  //  Prevents multiple inclusions.
#define PAS_FINAL_FLOW_H

#include <stateabs.h>
#include <an_alarm.h>
#include "trimamessages.h"

// Class Declaration
class PasFinalFlowVerification
   : public StateAbs
{
   DECLARE_STATE(PasFinalFlowVerification);
public:

   // Default constructor
   PasFinalFlowVerification();

   // Copy constructor
   PasFinalFlowVerification(const PasFinalFlowVerification& state);

   // Default destructor
   virtual ~PasFinalFlowVerification();

   virtual StateAbs* clone () const { return ( new PasFinalFlowVerification(*this)); }

protected:

   //
   // Functions to do this states processing ...
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();


   virtual int transitionStatus ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const PasFinalFlowVerification& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:


   // flag is set when conditions are met to transition from this state
   bool       transitionNow;

   float      plasmaVol;

   float      initPlasmaPumpVolume;

   anAlarmMsg fluidTooLateAlert;
};


#endif

/* FORMAT HASH 87119cd094ef83f46a6f3411c62ae17f */

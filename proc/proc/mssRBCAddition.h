/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssRBCAddition.h
 *
 *    This state will deliver the SS to the RBC products.
 */

/*
$Header $
$Log $
*/

#ifndef MSS_RBC_ADDITION_H  //  Prevents multiple inclusions.
#define MSS_RBC_ADDITION_H

#include <stateabs.h>
#include <an_alarm.h>

#include "trimamessages.h"

// Class Declaration
class MSSRBCAddition
   : public StateAbs
{
   DECLARE_STATE(MSSRBCAddition);
public:

   // Default constructor
   MSSRBCAddition();

   // Copy constructor
   MSSRBCAddition(const MSSRBCAddition& state);

   // Default destructor
   virtual ~MSSRBCAddition();

   virtual StateAbs* clone () const { return ( new MSSRBCAddition(*this)); }

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
   virtual void copyOver (const MSSRBCAddition& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   // the addition pump speed is based on volume
   float calculatePumpSpeed (float volume);

protected:
   bool       transitionNow;

   float      solutionToBeMeteredOut;
   float      initialACVolume;

   anAlarmMsg aps2hiPTFalert;
   anAlarmMsg aps2hiPTFalarmProd1Ok;
   anAlarmMsg aps2hiPTFalarm;
   anAlarmMsg aps2hiPTFalarmWithPAS;
   anAlarmMsg drbcSplit;

   anAlarmMsg _acClampNotClosedAlarm;
   anAlarmMsg _acClampNotClosedAlarm_butContinuePAS;


   int  apsInstanceCount;        // number they received
   int  apsInstanceCount_double; // number they received
   bool apsHigh ;                // is active

   bool recoveryFromProductTwo;


   bool thisIsDRBC;
   bool thisIsPartialDRBC;
   // float firstHalfvolume;
   // bool  shortSecondRBCBagDose;
   // float adjustedSecondVolume;

   float doubleMultiplier;
};


#endif

/* FORMAT HASH c777eaa35082b09e0c239715f9d6b45a */

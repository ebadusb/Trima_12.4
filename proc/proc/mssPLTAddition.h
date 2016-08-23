/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssPLTAddition.h
 *
 *    This state will deliver the SS to the PLT products.
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssPLTAddition.h 1.8 2007/03/06 23:53:00Z dslausb Exp jheiusb $
$Log $
*/

#ifndef MSS_PLT_ADDITION_H  //  Prevents multiple inclusions.
#define MSS_PLT_ADDITION_H

#include <stateabs.h>

#include "an_alarm.h"
#include "trimamessages.h"
// #include "timermessage.h"


// Class Declaration
class MSSPLTAddition
   : public StateAbs
{
   DECLARE_STATE(MSSPLTAddition);
public:

   // Default constructor
   MSSPLTAddition();

   // Copy constructor
   MSSPLTAddition(const MSSPLTAddition& state);

   // Default destructor
   virtual ~MSSPLTAddition();

   virtual StateAbs* clone () const { return ( new MSSPLTAddition(*this)); }

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
   virtual void copyOver (const MSSPLTAddition& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   // handle RBC detected Purges
   virtual void doAPurgePre () ;
   virtual void doAPurgePost () ;
   virtual void endAPurge () ;
   // virtual void  moveValve();

   float calculatePumpSpeed (float v) ;


   // bool notArmed; // flag to keep from re-amring the alarm
   // TimerMessage valveTimer;  // want the valve to move in x seconds
protected:

   bool  transitionNow;

   float solutionToBeMeteredOut;
   float initialCollectVolume;
   float initialPlasmaVolume;

   float purgeplasmaVol;
   float purgeplateletVol;

   float prepurgeplasmaVol;

   bool  part1done;
   bool  part2done;
   bool  part3done;

   int   cycle;
   float _setupplasmaPumped;

   int   numberOfPurges;


   anAlarmMsg _VerifyClosureAlarm;
};


#endif

/* FORMAT HASH f7a3803b4b8b6bc3ad753a09b57d5af2 */

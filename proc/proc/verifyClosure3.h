/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE: verifyClosure3.h
 *
 * Return and Inlet pump @ 50 mls/min until Reservoir Empty
 *
 * Log:
 *    $Log: verifyClosure3.h $
 *    Revision 1.3  2006/03/30 20:49:19Z  jheiusb
 *    7350 -- Metered storage PFR
 *    Revision 1.2  2006/03/03 17:33:57Z  jheiusb
 *    initial merge
 *    Revision 1.1  2006/03/03 17:16:08Z  jheiusb
 *    Initial revision
 *    Revision 1.3  2006/01/19 23:09:15Z  jheiusb
 *    7355 -- rework clamp closure tests
 *    Revision 1.2  2006/01/18 23:42:45Z  jheiusb
 *    7355 -- fix contamination
 *    Revision 1.1  2005/10/21 21:50:55Z  rm70006
 *    Initial revision
 *
 */

#ifndef VERIFY_CLOSURE3_H  //  Prevents multiple inclusions.
#define VERIFY_CLOSURE3_H

#include <stateabs.h>

#include "ProcData.h"



// Class Declaration
class VerifyClosure3
   : public StateAbs
{
   DECLARE_STATE(VerifyClosure3);
public:

   // Default constructor
   VerifyClosure3();

   // Copy constructor
   VerifyClosure3(const VerifyClosure3& state);

   // Default destructor
   virtual ~VerifyClosure3();

   virtual StateAbs* clone () const { return ( new VerifyClosure3(*this)); }

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
   virtual void copyOver (const VerifyClosure3& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   // test the volume
   void testRPVolume (const float rpVol);

protected:
   ProcData   _pd;

   anAlarmMsg _channelClampAlert;


   float _initialVin;
   float _initialVrp;
   float _initialVp ;
   float _initialVc ;

   bool  _transitionNow;

   bool  _testPassed;
   bool  _testFailed;
   bool  _testPumpingDone;
   int   _testAttempts; // only do the test twice then it will go on even if failed

   bool  _endPurgeDone;
   bool  _doingSalvageCase;

};
#endif

/* FORMAT HASH 573f1dc21a4aea07bb19dbcafac0d18c */

/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MSSRBCPrime1.h
 *
 *     This state will do the second prime of the
 *      system with RBC storage solution.
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssRBCPrime1.h 1.2 2006/12/11 20:19:29Z jheiusb Exp jheiusb $
$Log: mssRBCPrime1.h $
Revision 1.2  2006/12/11 20:19:29Z  jheiusb
7458 -- fix MSS RBC delay alarm
Revision 1.1  2006/11/20 22:49:32Z  jheiusb
Initial revision
Revision 1.3  2006/01/27 16:34:44Z  jheiusb
7376 -- fix prime too soon
Revision 1.2  2005/10/30 21:12:30Z  jheiusb
7230 -- RBC prime and addition for MSS (NO PTF)

*/


#ifndef MSS_RBCPrime_1  //  Prevents multiple inclusions.
#define MSS_RBCPrime_1


#include <an_alarm.h>
#include <stateabs.h>


// Class Declaration
class MSSRBCPrime1
   : public StateAbs
{
   DECLARE_STATE(MSSRBCPrime1);
public:

   // Default constructor
   MSSRBCPrime1();

   // Copy constructor
   MSSRBCPrime1(const MSSRBCPrime1& state);

   // Default destructor
   virtual ~MSSRBCPrime1();

   virtual StateAbs* clone () const { return ( new MSSRBCPrime1(*this)); }

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
   virtual void copyOver (const MSSRBCPrime1& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   bool       transitionNow;

   float      initReturnPumpVolume;
   float      initACPumpVolume;
   float      initInletPumpVolume;

   anAlarmMsg fluidTooLateAlert;
   anAlarmMsg fluidTooSoonAlarm;

   int        alertCounter;
   bool       primeTooSoonFlag;
   bool       primeTooLateFlag;

   anAlarmMsg apsPres2HighAlert;
   anAlarmMsg apsPres2HighAlarm;
   int        numberOfAPSAlerts;

   int        apsInstanceCount;
   bool       apsHigh;

};


#endif

/* FORMAT HASH 411deee1ac17abbb1f5430b268bb22b0 */

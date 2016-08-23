/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MSSPLTPrime1.h
 *
 *     This state will prime the system with storage solution
 */
/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssPLTPrime1.h 1.6 2006/06/28 20:14:58Z dslausb Exp jheiusb $
$Log: mssPLTPrime1.h $
Revision 1.6  2006/06/28 20:14:58Z  dslausb
Removed a bunch of obsolete strings and a couple unused alarms that were calling them so that we aren't wasting resources translating them.
Revision 1.5  2006/05/09 20:59:18Z  jheiusb
7523 -- fix prime too soons recovery
Revision 1.4  2006/04/13 17:22:48Z  jheiusb
7484 -- APS checking for Prime and addition enabled
Revision 1.3  2006/04/07 16:55:01Z  jheiusb
7350 -- fix primeing return pump
Revision 1.2  2006/03/03 17:32:28Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:17:06Z  jheiusb
Initial revision
Revision 1.6  2005/11/29 22:58:27Z  jheiusb
7233 -- fix the prime too soon to be one continue then fail
Revision 1.5  2005/11/03 23:00:45Z  jheiusb
7230 -- fix prime too soon alaert
Revision 1.4  2005/10/30 21:12:02Z  jheiusb
7230 -- RBC prime and addition for MSS (NO PTF)
Revision 1.3  2005/10/27 16:40:47Z  jheiusb
7202 -- add prime too late alarms/alerts
Revision 1.2  2005/10/25 20:58:14Z  jheiusb
7202 -- initial PLT mss path
Revision 1.1  2005/10/24 18:32:08Z  jheiusb
Initial revision
Revision 1.1  2005/10/24 17:32:14Z  jheiusb
Initial revision
Revision 1.3  2005/10/07 22:06:41Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:27Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/


#ifndef MSS_PLTPrime_1  //  Prevents multiple inclusions.
#define MSS_PLTPrime_1


#include <an_alarm.h>
#include <stateabs.h>


// Class Declaration
class MSSPLTPrime1
   : public StateAbs
{
   DECLARE_STATE(MSSPLTPrime1);
public:

   // Default constructor
   MSSPLTPrime1();

   // Copy constructor
   MSSPLTPrime1(const MSSPLTPrime1& state);

   // Default destructor
   virtual ~MSSPLTPrime1();

   virtual StateAbs* clone () const { return ( new MSSPLTPrime1(*this)); }

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
   virtual void copyOver (const MSSPLTPrime1& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:
   bool transitionNow;

   // initial volm... these get reset if we fail prime
   // initial volm... these get reset if we fail prime
   float      initReturnPumpVolume;
   float      initPlasmaPumpVolume;
   float      initCollectPumpVolume;
   float      initInletPumpVolume;

   anAlarmMsg fluidTooLateAlert;
   // anAlarmMsg  fluidTooLateAlarm;
   anAlarmMsg fluidTooSoonAlert;
   // anAlarmMsg  fluidTooSoonAlarm;

   bool primeTooSoonFlag;
   bool primeTooLateFlag;

   int  alertCounter;
   int  alertCounterPrime2soon;
   bool holdPumpsDueToPressure;

   bool cleanRes;
};


#endif

/* FORMAT HASH 0049b7a14b02585ad6d6e05cb3996359 */

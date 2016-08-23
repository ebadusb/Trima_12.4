/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      VacPurge.h
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/vacPurge.h 1.4 2006/07/21 16:22:04Z jheiusb Exp jheiusb $
$Log: vacPurge.h $
Revision 1.4  2006/07/21 16:22:04Z  jheiusb
7601 -- salvage case added
Revision 1.3  2006/03/31 22:12:44Z  jheiusb
7350 -- PFR for Vac purge
Revision 1.2  2006/03/03 17:33:39Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:17:56Z  jheiusb
Initial revision
Revision 1.5  2006/01/19 23:08:53Z  jheiusb
7355 -- rework clamp closure tests
Revision 1.4  2005/12/21 21:39:04Z  jheiusb
7202 -- fine tune speeds for PLT metering
Revision 1.3  2005/10/14 16:34:05Z  jheiusb
7202 -- add initial vacuum purge logic
Revision 1.2  2005/10/05 20:35:51Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/


#ifndef VAC_PURGE_H  //  Prevents multiple inclusions.
#define VAC_PURGE_H

#include <stateabs.h>
#include "timermessage.h"

// Class Declaration
class VacPurge
   : public StateAbs
{
   DECLARE_STATE(VacPurge);
public:

   // Default constructor
   VacPurge();

   // Copy constructor
   VacPurge(const VacPurge& state);

   // Default destructor
   virtual ~VacPurge();

   virtual StateAbs* clone () const { return ( new VacPurge(*this)); }

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
   virtual void copyOver (const VacPurge& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   void clearPause ();
   void clearPause2 ();
   void clearPause3 ();

protected:

   // flag is set when conditions are met to transition from this state
   bool  transitionNow;

   float plateletVol;
   float plasmaVol;

   bool  part1done;
   bool  part2done;
   bool  part3done;

   bool  holdPumpsDueToPressure;

   int   cycle;


   TimerMessage pauseTimer;

   bool         pauseFlag;
   bool         subpause1, subpause2 ;
};


#endif

/* FORMAT HASH 6e0f9a87abcdc4e047ed96a679181486 */

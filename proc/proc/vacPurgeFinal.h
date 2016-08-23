/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      VacPurgeFinal.h
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.1x/Trima/proc/proc/rcs/vacPurgeFinal.h 1.1 2006/03/03 17:18:03Z jheiusb Exp jheiusb $
$Log: vacPurgeFinal.h $
Revision 1.1  2006/03/03 17:18:03Z  jheiusb
Initial revision
Revision 1.4  2005/10/25 20:58:54Z  jheiusb
7202 -- initial PLT mss path
Revision 1.3  2005/10/20 22:15:01Z  jheiusb
add valve timer
Revision 1.2  2005/10/14 16:34:30Z  jheiusb
7202 -- add initial vacuum purge logic
Revision 1.1  2005/10/13 22:27:19Z  jheiusb
Initial revision

*/


#ifndef VAC_PURGE_FINAL_H  //  Prevents multiple inclusions.
#define VAC_PURGE_FINAL_H

#include <stateabs.h>
#include "timermessage.h"
#include "trimamessages.h"

// Class Declaration
class VacPurgeFinal
   : public StateAbs
{
   DECLARE_STATE(VacPurgeFinal);
public:

   // Default constructor
   VacPurgeFinal();

   // Copy constructor
   VacPurgeFinal(const VacPurgeFinal& state);

   // Default destructor
   virtual ~VacPurgeFinal();

   virtual StateAbs* clone () const { return ( new VacPurgeFinal(*this)); }

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
   virtual void copyOver (const VacPurgeFinal& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   void moveValve ();

   // flag is set when conditions are met to transition from this state
   bool         transitionNow;

   float        plateletVol;
   float        plasmaVol;

   bool         notArmed;   // flag to keep from re-amring the alarm
   TimerMessage valveTimer; // want the valve to move in 0.5 seconds



   float InitialPlateletVol;
   float InitialPlasmaVol;
};


#endif

/* FORMAT HASH b48df70464a3003eaa154cb05dbb3d8c */

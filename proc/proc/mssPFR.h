/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssPFR.h
 *
 *     This state will decide what MSS needs to be done in a PFR.
 */
/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssPFR.h 1.4 2007/04/02 20:21:16Z jheiusb Exp jheiusb $
$Log: mssPFR.h $
Revision 1.4  2007/04/02 20:21:16Z  jheiusb
7579 -- patch up PFR for 5.R
Revision 1.3  2007/01/24 21:13:04Z  dslausb
Fixed MSS PFR gui messages
Revision 1.2  2006/03/27 17:52:13Z  jheiusb
7350 -- PFR for MSS
*/


#ifndef MSS_PFR_H  //  Prevents multiple inclusions.
#define MSS_PFR_H

#include <stateabs.h>
#include "guiproc.h"


// Class Declaration
class MSSPFR
   : public StateAbs
{
   DECLARE_STATE(MSSPFR);
public:

   // Default constructor
   MSSPFR();

   // Copy constructor
   MSSPFR(const MSSPFR& state);

   // Default destructor
   virtual ~MSSPFR();

   virtual StateAbs* clone () const { return ( new MSSPFR(*this)); }

protected:



   //
   enum { TransitionToVerifyClosure = 1,
          TransitionToMSSConnect    = 2,
          TransitionToMSSPrime      = 3,
          TransitionToMSSParallel   = 4,
          TransitionToMSSDisconnect = 5};

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
   virtual void copyOver (const MSSPFR& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   //
   // Make sure GUI is on the same page as we are, MSS-procedure-wise.
   //
   virtual void setGuiMssStatus ();

   virtual void sendGuiTransitionMsg (AAS_PFR_SCREEN scrnTran);

protected:
   bool transitionNow;
   int  transitionTo;

};


#endif

/* FORMAT HASH e863f4beeb449ff4d802268b1168ba1d */

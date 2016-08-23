/*
 * Copyright (c) 2009 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssRBCPrimeSalvage.h
 *
 */
/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssRBCPrimeSalvage.h 1.1 2009/05/26 20:11:10Z jheiusb Exp jheiusb $
*/


#ifndef MSSRBCPRIMESALVAGE__H  //  Prevents multiple inclusions.
#define MSSRBCPRIMESALVAGE__H


#include "an_alarm.h"
#include <stateabs.h>

// Class Declaration
class MSSRBCPrimeSalvage
   : public StateAbs
{
   DECLARE_STATE(MSSRBCPrimeSalvage);
public:

   // Default constructor
   MSSRBCPrimeSalvage();

   // Copy constructor
   MSSRBCPrimeSalvage(const MSSRBCPrimeSalvage& state);

   // Default destructor
   virtual ~MSSRBCPrimeSalvage();

   virtual StateAbs* clone () const { return ( new MSSRBCPrimeSalvage(*this)); }

protected:

   virtual int transitionStatus ();
   //
   // Functions to do this states processing ...
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const MSSRBCPrimeSalvage& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();



private:


   bool       transitionNow;
   float      rasPumpVol;

   anAlarmMsg apsPres2HighAlarm;
};


#endif

/* FORMAT HASH 7aca18d09a425b0870a7bc4971a4610f */

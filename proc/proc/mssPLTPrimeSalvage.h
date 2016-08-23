/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MSSPLTPrimeSalvage.h
 *
 *     This state will prime the system with storage solution
 *     in the event of rinseback failure
 */
/*
$Header: //BCTquad3/home/BCT_Development/Trima5.1x/Trima/proc/proc/rcs/mssPLTPrimeSalvage.h 1.1 2006/07/07 20:52:18Z jheiusb Exp jheiusb $
$Log: mssPLTPrimeSalvage.h $

*/


#ifndef MSS_PLTPrimeSalvage_H  //  Prevents multiple inclusions.
#define MSS_PLTPrimeSalvage_H


#include <an_alarm.h>
#include <stateabs.h>


// Class Declaration
class MSSPLTPrimeSalvage
   : public StateAbs
{
   DECLARE_STATE(MSSPLTPrimeSalvage);
public:

   // Default constructor
   MSSPLTPrimeSalvage();

   // Copy constructor
   MSSPLTPrimeSalvage(const MSSPLTPrimeSalvage& state);

   // Default destructor
   virtual ~MSSPLTPrimeSalvage();

   virtual StateAbs* clone () const { return ( new MSSPLTPrimeSalvage(*this)); }

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
   virtual void copyOver (const MSSPLTPrimeSalvage& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:
   bool transitionNow;

   // initial volm..
   float initReturnPumpVolume;
   float initPlasmaPumpVolume;
   float initCollectPumpVolume;
   float initInletPumpVolume;



   bool holdPumpsDueToPressure;
};


#endif

/* FORMAT HASH 6c0f11822ee70c056f8f1e6c3dd3eba4 */

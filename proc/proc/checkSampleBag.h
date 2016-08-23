/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      CHECKSAMPLEBAG_H
 *
 */


#ifndef CHECKSAMPLEBAG_H  //  Prevents multiple inclusions.
#define CHECKSAMPLEBAG_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "CatalogChecker.h"

#include "software_cds.h"     // Get Optional sw.dat settings


// Class Declaration
class CheckSampleBag
   : public StateAbs
{
   DECLARE_STATE(CheckSampleBag);
public:

   // Default constructor
   CheckSampleBag();

   // Copy constructor
   CheckSampleBag(const CheckSampleBag& state);

   // Default destructor
   virtual ~CheckSampleBag();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback function for valve timer ...
   //

   virtual StateAbs* clone () const { return ( new CheckSampleBag(*this)); }

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

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();


   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const CheckSampleBag& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:
   bool  _MSSRBCset;
   float _initialAPS;
   bool  _transitionNow;
   float _initialRetvolm; // air evac uses this
   float _initialACvolm;  // non - atd mitigation
   bool  _bailOut;        // to the donor disco
   int   _alrmCnt;


   anAlarmMsg
      _InletPressureAlarmFinal,
      _InletPressureAlarmNonARMFinal,
      _InletPressureAlarm,
      _InletPressureAlarmNonRasSet;  // Same as above, except it's possible the operator loaded the wrong set, too.


};


#endif

/* FORMAT HASH a35431a38ef9319fcb8f315abf21ef79 */

#ifndef DOORLOCKCHECK_H  //  Prevents multiple inclusions.
#define DOORLOCKCHECK_H

#include "stateabs.h"
#include "an_alarm.h"

class DoorLockCheck
   : public StateAbs
{
   DECLARE_STATE(DoorLockCheck);
public:

   // Default constructor
   DoorLockCheck();

   // Copy constructor
   DoorLockCheck(const DoorLockCheck& state);

   // Default destructor
   virtual ~DoorLockCheck();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new DoorLockCheck(*this)); }

protected:

   //
   // Functions to do this state's processing ...
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
   // Reset the state upon termination
   //
   virtual void reset ();

   //
   // Function to stop all pumps
   //
   void stopPumps ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const DoorLockCheck& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   anAlarmMsg _CloseTheDoorAlarm;

};


#endif

/* FORMAT HASH 7ca598725c7e136808b029b64bb93ebc */

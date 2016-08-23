#ifndef AIR_2_CHANNEL_2_H  //  Prevents multiple inclusions.
#define AIR_2_CHANNEL_2_H

 #include <stateabs.h>
 #include "an_alarm.h"

// Class Declaration
class Air2Channel2
   : public StateAbs
{
   DECLARE_STATE(Air2Channel2);
public:

   // Default constructor
   Air2Channel2();

   // Copy constructor
   Air2Channel2(const Air2Channel2& state);

   // Default destructor
   virtual ~Air2Channel2();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new Air2Channel2(*this)); }

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
   virtual void copyOver (const Air2Channel2& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   //
   bool  _transition;
   bool  _airEvacMitigation;
   float _currentInletVolm;
   float _currentReturnVolm;
   bool  _whiteStamp;
   int   _failureCnt;


   anAlarmMsg _closeClampAlarm;


};
#endif

/* FORMAT HASH 054c9ee1677ce92461e0009213e1c490 */

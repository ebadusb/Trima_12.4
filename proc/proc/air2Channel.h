#ifndef AIR_2_CHANNEL_H  //  Prevents multiple inclusions.
#define AIR_2_CHANNEL_H

 #include <stateabs.h>
 #include "an_alarm.h"

// Class Declaration
class Air2Channel
   : public StateAbs
{
   DECLARE_STATE(Air2Channel);
public:

   // Default constructor
   Air2Channel();

   // Copy constructor
   Air2Channel(const Air2Channel& state);

   // Default destructor
   virtual ~Air2Channel();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new Air2Channel(*this)); }

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
   virtual void copyOver (const Air2Channel& s);

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


   anAlarmMsg _closeClampAlarmNonRasSet;


};
#endif

/* FORMAT HASH ad75fe26d7d03ccf54d1a0d5001bbd06 */

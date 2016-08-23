#ifndef AIR_2_CHANNEL_PRIME_H  //  Prevents multiple inclusions.
#define AIR_2_CHANNEL_PRIME_H

 #include <stateabs.h>
 #include "an_alarm.h"



// Class Declaration
class Air2ChannelPrime
   : public StateAbs
{
   DECLARE_STATE(Air2ChannelPrime);
public:

   // Default constructor
   Air2ChannelPrime();

   // Copy constructor
   Air2ChannelPrime(const Air2ChannelPrime& state);

   // Default destructor
   virtual ~Air2ChannelPrime();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new Air2ChannelPrime(*this)); }

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
   virtual void copyOver (const Air2ChannelPrime& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:


   bool       _transition;
   bool       _airEvacMitigation;
   float      _currentReturnVolm;
   anAlarmMsg _alarm;
};
#endif

/* FORMAT HASH 6cbe9b2b6497e4869f83124f2da2a0f2 */

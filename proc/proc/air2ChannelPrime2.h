#ifndef AIR_2_CHANNEL_PRIME2_H  //  Prevents multiple inclusions.
#define AIR_2_CHANNEL_PRIME2_H

 #include <stateabs.h>
 #include "an_alarm.h"



// Class Declaration
class Air2ChannelPrime2
   : public StateAbs
{
   DECLARE_STATE(Air2ChannelPrime2);
public:

   // Default constructor
   Air2ChannelPrime2();

   // Copy constructor
   Air2ChannelPrime2(const Air2ChannelPrime2& state);

   // Default destructor
   virtual ~Air2ChannelPrime2();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new Air2ChannelPrime2(*this)); }

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
   virtual void copyOver (const Air2ChannelPrime2& s);

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

/* FORMAT HASH e9cad78d40df77c7e48de6d88da25098 */

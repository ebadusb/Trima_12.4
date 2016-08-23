#ifndef PLS_EVAC_FINISH_H  //  Prevents multiple inclusions.
#define PLS_EVAC_FINISH_H

#include <stateabs.h>


// Class Declaration
class PlsEvacFinished
   : public StateAbs
{
   DECLARE_STATE(PlsEvacFinished);
public:

   // Default constructor
   PlsEvacFinished();

   // Copy constructor
   PlsEvacFinished(const PlsEvacFinished& state);

   // Default destructor
   virtual ~PlsEvacFinished();

   virtual StateAbs* clone () const { return ( new PlsEvacFinished(*this)); }

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
   virtual void copyOver (const PlsEvacFinished& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();



};


#endif

/* FORMAT HASH a5a6e5950030e81d2a26c386519b8e81 */

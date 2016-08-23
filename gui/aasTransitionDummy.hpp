/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      aasTransitionDummy.hpp
 *
 *
* DESCRIPTION: This is just a dummy screen.  It is "allocated" when the system transitions to
* the metered storage state.  The allocation function, however, does not allocate any text or
* graphics, it simply makes the decision where to go from here.  The options are the flagging
* screens for RBC or platelet, and the clamp channel screen.
*
* In other words, though this class is allocated by the screen commutator based on a state
* transition, it has no graphical components, and it's sole purpose is to determine what
* screen is displayed next and transition to it.
*/

// Single include define guardian
#ifndef _AAS_TRANSITION_DUMMY_HPP_
#define _AAS_TRANSITION_DUMMY_HPP_



// CLASS INCLUDE
#include "aphwind.hpp"

// MESSAGE DECLARES
#include "pqinformation.h"
#include "runsummary.h"

class aasTransitionDummy
   : public Base_Apheresis_Window
{
protected:

   // The Product Quality CDS data access class
   PQInformation               _pqa;

   Message<RUN_SUMMARY_STRUCT> _msgRecieveRunStats;

   void runStatsRecieved ();


public:

   aasTransitionDummy();

   virtual ~aasTransitionDummy();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();
};

#endif

/* FORMAT HASH a7e35ebd9030d0733d467348923403dc */

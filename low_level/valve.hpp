/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      valve.hpp, Focussed System control hardware valve logic
 *
 * ABSTRACT:   Interface to the control hardware valves
 *
 * FUNCTIONS
 *  processValves() - check orders and status and move valves to proper positions
 *
 * CLASSES:
 *
 *  class cassetteRunLogic - runs cassette up and down
 *     cassetteRunLogic() - constructor
 *     ~cassetteRunLogic() - destructor
 *     update() - update loop
 *
 *  class readValve - read a valve position
 *     readValve() - constructor
 *     ~readValve() - destructor
 *     update() - read current position
 *
 *  class valveRunLogic - run valves to new positions
 *     valveRunLogic() - constructor
 *     ~valveRunLogic() - destructor
 *     update() - update loop
 */

#ifndef _VALVE_INCLUDE
#define _VALVE_INCLUDE

#include "chw.hpp"
#include "common_hw_private.h"

// SPECIFICATION:    read valve positions, this class reads the current
//                   position of a valve
//
// ERROR HANDLING:   none.

class readValve
{
public:
   readValve(HWValve valve);
   virtual ~readValve();

   HW_VALVE_STATES    update ();          // update
   HW_CASSETTE_STATES updateCassette ();  // convert to cassette values

private:
   HWValve         _valve;  // valve selection
   HW_VALVE_STATES _state;  // valve position
   unsigned short  _count;  // debounce count
};

#endif /* ifndef _VALVE_INCLUDE */

/* FORMAT HASH 1cfafff4c1bf69e7af52e2bb2a63ed00 */

/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>

#include "valve.hpp"

// SPECIFICATION:    read valve positions, constructor
//
// ERROR HANDLING:   none.

readValve::readValve(HWValve valve)
   : _valve(valve),
     _state(HW_VALVE_UNKNOWN),
     _count(0)
{
   update();
}

// SPECIFICATION:    read valve positions, destructor
//
// ERROR HANDLING:   none.

readValve::~readValve()
{}

// SPECIFICATION:    read valve positions, update
//
// ERROR HANDLING:   none.

HW_VALVE_STATES readValve::update ()
{
   switch (hw_valveGetPosition(_valve))
   {
      case hw_valveOpen :
         _count = 0;
         _state = HW_VALVE_OPEN;
         break;

      case hw_valveCollect :
         _count = 0;
         _state = HW_VALVE_COLLECT;
         break;

      case hw_valveReturn :
         _count = 0;
         _state = HW_VALVE_RETURN;
         break;

      case hw_valveUnknown :
         _state = ( ++_count < 5 ) ? _state : HW_VALVE_UNKNOWN;
         break;

      case hw_valveIllegal :
         _state = HW_VALVE_UNKNOWN;
         break;
   }

   return _state;
}

HW_CASSETTE_STATES readValve::updateCassette ()
{
   HW_CASSETTE_STATES cassetteState;

   update();

   switch (_state)
   {
      case HW_VALVE_COLLECT :
         cassetteState = HW_CASSETTE_DOWN;
         break;

      case HW_VALVE_OPEN :
         cassetteState = HW_CASSETTE_UP;
         break;

      default :
         cassetteState = HW_CASSETTE_UNKNOWN;
         break;
   }

   return cassetteState;
}

/* FORMAT HASH 8fcc5aedf8f0a844fce396e5e777c553 */

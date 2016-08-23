/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      c_sound.hpp, Focussed System sound level
 *
 * ABSTRACT:   Interface to the control hardware sound level
 *
 * PUBLIC FUNCTIONS:
 *
 * class temp
 *    sound() - constructor
 *    ~sound() - destructor
 *    update() - update loop
 *
 */

#ifndef _C_SOUND_INCLUDE
#define _C_SOUND_INCLUDE

#include "chw.hpp"

// classes

class soundLevelInterface
{
public:
   soundLevelInterface(CHW_SOUND_LEVELS& setSound);

   virtual ~soundLevelInterface();

   inline void changeLevel (CHW_SOUND_LEVELS setSound) { _setSound = setSound; }

   void update ();             // read values

   void actual (CHW_SOUND_LEVELS& getSound);             // return current states

private:
   CHW_SOUND_LEVELS& _setSound;
};


#endif // ifndef _C_SOUND_INCLUDE

/* FORMAT HASH 07a51274071a1320f35145aa84687198 */

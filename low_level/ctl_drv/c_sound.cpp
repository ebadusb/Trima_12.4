/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      Focussed System sound level control and status
 *
 * ABSTRACT:   Interface to the control sound level
 *
 */

#include <vxWorks.h>

#include "c_sound.hpp"
#include "ctl_hw_private.h"
#include "trima_datalog.h"


// SPECIFICATION:   Sound level control and sense constructor.
// ERROR HANDLING:  None

soundLevelInterface::soundLevelInterface(CHW_SOUND_LEVELS& setSound)
   : _setSound(setSound)
{
   _setSound = CHW_SOUND_LOUD;
}

// SPECIFICATION:   Sound level control and sense destructor.
// ERROR HANDLING:  None
soundLevelInterface::~soundLevelInterface()
{}


// SPECIFICATION:   Set sound level.
// ERROR HANDLING:  None

void soundLevelInterface::update ()
{
   chw_soundSetLevel((unsigned short)_setSound & 0x03);
}

// SPECIFICATION:   Read sound level.
// ERROR HANDLING:  None

void soundLevelInterface::actual (CHW_SOUND_LEVELS& getSound)
{
   getSound = (CHW_SOUND_LEVELS)chw_soundGetLevel();
}

/* FORMAT HASH d4931549beddef444b5cc266b6ad6d25 */

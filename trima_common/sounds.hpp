/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Log: sounds.hpp $
 * Revision 1.7  2008/08/10 16:43:25Z  ssunusb
 * IT8044 - Alternate Sound for DRBC Split notification
 * Revision 1.6  2008/04/18 18:57:28Z  ssunusb
 * IT8482 - Make trima sounds configurable
 * Revision 1.5  2007/03/01 21:44:35Z  dslausb
 * IT 7875 - Added new sound for end of AAS addition
 * Revision 1.4  2003/01/07 21:28:29Z  jl11312
 * - updated comments
 * Revision 1.3  2002/06/13 18:23:42  jl11312
 * - modifications for initial control driver port to vxWorks
 * Revision 1.2  2001/12/07 21:30:51  rm70006
 * IT 5151.
 * Add new sound for barcode invalid.
 * Revision 1.1  2000/04/06 21:56:02  BD10648
 * Initial revision
 * Revision 1.7  1998/09/22 18:33:09  bs04481
 * Allow sounds.dat file to be written in musical notes instead of
 * frequencies.
 * Revision 1.6  1998/08/10 18:33:40  bs04481
 * New sound for First Return Imminent
 * Revision 1.5  1997/02/13 22:34:35  bs04481
 * Changed the sound associated with slay/restart pointer.
 * Revision 1.4  1996/07/24 19:50:17  SS03309
 * fix MKS
 * Revision 1.3  1996/07/01 20:22:35  SS03309
 * Added comment of sounds.dat match
 * Revision 1.2  1996/06/27 16:23:42  SS03309
 * Added comments
 *
 * TITLE:      sounds.hpp, Focussed System sound definitions
 *             This file must match sounds.dat
 *
 * ABSTRACT:   Defines structures and enums for sounds from PC speaker.
 *
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 */

#ifndef _SOUNDS_INCLUDE
#define _SOUNDS_INCLUDE

enum SOUND_CODE
{
   ALARM_SOUND,                 // safety alarms
   ALERT_SOUND,                 // control alerts
   BUTTON_CLICK_SOUND,          // button clicks
   END_PROCEDURE_SOUND,         // end of procedure
   END_AAS_SOUND,               // end of Additive Solution Addition
   RINSE_BACK_SOUND,            // start of rinse back
   SQUEEZE_SOUND,               // tell donor to squeeze
   FIRST_RETURN_SOUND,          // start of first return
   BARCODE_INVALID,             // Invalid Barcode
   DRBC_SPLIT_SOUND,            // Drbc Split
   ATTENTION1_SOUND,            // operator attention sound
   MAX_SOUNDS,                  // entry for sound manager [must be last sound in enum]
   NO_SOUND                     // make no sound
};

enum SOUND_MODE
{
   SOUND_CLEAR,                  // clear continuous sound
   SOUND_CLEAR_ALL,              // clear all sounds.  Set sound code to NO_SOUND
   SOUND_CONTINUOUS,             // schedule sound continuously
   SOUND_IMMEDIATE,              // send now, override any other tone
   SOUND_ONCE,                   // schedule sound once

   SOUND_MAX_MODES               // entry for sound manager [must be last in enum]
};

#ifndef __WIN32__
// These strings correspond to the section names in the sounds.dat file
static const char* SoundCodeStrings[] = {
   "ALARM",                   // safety alarms
   "ALERT",                   // control alerts
   "BUTTON_CLICK",            // button clicks
   "END_PROCEDURE",           // end of procedure
   "END_AAS",                 // end of Additive Solution Addition
   "RINSE_BACK",              // start of rinse back
   "SQUEEZE",                 // tell donor to squeeze
   "FIRST_RETURN",            // start of first return
   "BARCODE_INVALID",         // Invalid Barcode
   "DRBC_SPLIT",              // Drbc Split alert sound
   "ATTENTION1",              // operator attention sound
   "MAX_SOUNDS",
   "NO_SOUND"                 // make no sound
};

static const char* SoundModeStrings[] = {
   "SOUND_CLEAR",                // clear continuous sound
   "SOUND_CLEAR_ALL",            // clear all sounds
   "SOUND_CONTINUOUS",           // continuous sound
   "SOUND_IMMEDIATE",            // immediate sound
   "SOUND_ONCE"                  // sound once
};

#endif /* __WIN32__ */

// SPECIFICATION:    orders used for sound request
//
// ERROR HANDLING:   none.
struct SoundOrders
{
   SOUND_CODE soundCode;         // sound code, see above
   SOUND_MODE mode;              // scheduling mode
};

#endif /* ifndef _SOUNDS_INCLUDE */

/* FORMAT HASH aae3c2779e4e9173deab33ab495b0877 */

/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.16  1998/10/20 20:37:14Z  bs04481
 * Change power fail recovery to restore PF data and resume all
 * active monitoring immediately after initialization if Safety determines
 * that the procedure is recoverable.
 * Following state transitions are now allowed as PFR transitions:
 * Blood Run to Blood Run; Rinseback to Rinseback
 * Following state transitions are allowed during a PFR run but
 * Safety will not carry out the actions for the transition:  Blood Run
 * to Self Test; Blood Run to Startup; Rinseback to Self Test;
 * Rinseback to Startup.
 * Following transitions are illegal: Startup to Blood Run; Startup to
 * Rinseback.
 * Revision 1.15  1998/09/29 19:49:34  bs04481
 * Fix SIGSEV which occurred when Safety received
 * unexpected data in the state change message
 * Revision 1.14  1997/12/02 15:59:58  bs04481
 * Changes to implement power fail recovery
 * Revision 1.13  1997/02/11 18:00:33  bs04481
 * Changes due to enumerated types in the hardware structures
 * Revision 1.12  1996/11/28 00:07:59  SM02805
 * Revision 1.11  1996/09/22 18:08:04  SM02805
 * Respond to 09/19/96 code review action items.
 * Revision 1.10  1996/09/15 00:25:18  SM02805
 * Significantly reduce print instrumentation in source files.
 * Implement _LOG_ERROR processing for cases where
 * data is important for post-run safety analysis.
 * Revision 1.9  1996/07/24 18:58:34  SM02805
 * Revision 1.8  1996/07/24 13:56:50  SM02805
 * Mods to remove absolute paths for include files, includes source
 * code modifications and makefile ( safety.mak) modifications.
 *
 * safety.mak mods use CPPFLAGS , now getting warnings level 3.
 *
 * Sorce code mods to implement donor disconnect test, only partially tested
 * - works as expected when test is passed first time, ie access line
 * properly clamped on first test try...
 * Revision 1.7  1996/07/22 17:13:36  SM02805
 * Update documentation for code review.
 *
 * Corrections to pump monitoring : but for platelet pump due to two magnet rotor.
 *
 * stest.mak target changed to be  saf_exec.
 *
 * TITLE:      s_defs.hpp, FS Safety System Common INterface FIle
 *
 * ABSTRACT:   defines common for safety files.
 *
 *
 * DOCUMENTS
 *
 *
 */

#ifndef S_DEFS_HPP
#define S_DEFS_HPP

#include "s_type.h"


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Safety Status Structure Defines : Start
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

// safety internal state cassette position defines
#define CASSETTE_MUST_REMAIN_DOWN                  1
#define CASSETTE_MAY_BE_MOVED                      2

#define VALVE_LED_TEST_NOMINAL                       1   // all other values -> error in valve blink test


// safety internal state door defines
#define DOOR_MUST_BE_CLOSED_AND_LOCKED               1
#define DOOR_MAY_BE_UNLOCKED_AND_OPENED              2


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Safety Status Structure Defines : End
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++
        Safety Safe State Defines : Start
++++++++++++++++++++++++++++++*/

#define OFF_24V                                      1    // turn off 24V for safe state rqd
#define OFF_64V                                      2    // turn off 64V for safe state rqd
#define OFF_BOTH                                     3    // turn off 24V and 64V for safe state rqd.

/*+++++++++++++++++++++++++++++
        Safety Safe State Defines : End
++++++++++++++++++++++++++++++*/

#endif // S_DEFS_HPP

/* FORMAT HASH c224f53a3e30111d5e6e6be6ef320367 */

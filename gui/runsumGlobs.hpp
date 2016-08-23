/*******************************************************************************
 *
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      runsumGlobs.hpp
 *             Run Summary Globals
 *
 * ABSTRACT:   This is a place for data and functions common to all the run summary screens
 *				and classes.
 *
 * HISTORY:    $Log: runsumGlobs.hpp $
 * HISTORY:    Revision 1.5  2008/10/03 16:19:44Z  dslausb
 * HISTORY:    Move away from dynamic memory where possible
 * HISTORY:    Revision 1.4  2007/10/19 16:28:09Z  dslausb
 * HISTORY:    - Added AAS start time to the run summary screen.
 * HISTORY:    - Add AAS icons to progress bars on runproc screen
 * HISTORY:    - Fix spurious air removal message when air removal off
 * HISTORY:    - Fix MSS disconnect screen transition
 * HISTORY:    Revision 1.3  2007/09/11 21:34:59Z  dslausb
 * HISTORY:    Update to run summary and flagging functionality
 * HISTORY:    Revision 1.2  2007/08/30 21:13:04Z  dslausb
 * HISTORY:    Fixed displaying of RBC salvage volume
 * HISTORY:    Revision 1.1  2007/08/29 21:39:03Z  dslausb
 * HISTORY:    Initial revision
 *
 *******************************************************************************/


#ifndef __RUNSUMM_GLOBS_HPP__
#define __RUNSUMM_GLOBS_HPP__

#include "runsummary.h"
#include "pqinformation.h"
#include "vxworks.h"

class RunsumGlobs
{
private:
   static PQInformation _pqa;

public:
   static void requestRunsumStats ();

   static bool shouldShowAasSalvage (const RUN_SUMMARY_STRUCT& runSummary);

   static bool pasHasSalvageVol (const RUN_SUMMARY_STRUCT& runSummary);
   static bool rasHasSalvageVol (const RUN_SUMMARY_STRUCT& runSummary);
   static bool rasHasSalvageVolProd1 (const RUN_SUMMARY_STRUCT& runSummary);
   static bool rasHasSalvageVolProd2 (const RUN_SUMMARY_STRUCT& runSummary);

   static int getPasSalvageVol (const RUN_SUMMARY_STRUCT& runSummary);
   static int getRasSalvageVolProd1 (const RUN_SUMMARY_STRUCT& runSummary);
   static int getRasSalvageVolProd2 (const RUN_SUMMARY_STRUCT& runSummary);

   static void gotoNextRunSummaryScreen ();
   static void gotoNextRunSummaryScreen (const RUN_SUMMARY_STRUCT& runSummary);

   static void completeRunsumStatsProcessing ();
};

#endif // #ifndef __RUNSUMM_GLOBS_HPP__

/* FORMAT HASH 95908b09448ff16b42ed8fb522191d83 */

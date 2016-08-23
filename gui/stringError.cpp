/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      stringError.cpp
 *             String Error Reporter
 *
 * ABSTRACT:   I was going nuts trying to track down which string was causing the
 *		system to reboot, so I decided to create this big function which will tell
 *		you what the actual ID of the string is in the enumeration (not just the
 *		integer value).
 *
 * HISTORY:    $Log: stringError.cpp $
 * HISTORY:    Revision 1.106  2007/12/07 22:34:08Z  dslausb
 * HISTORY:    IT 8241 - Subtract filterloss when necessary.
 * HISTORY:    Revision 1.105  2007/12/04 16:44:57Z  dslausb
 * HISTORY:    Add "continue with next product" alarms
 * HISTORY:    Revision 1.104  2007/12/03 23:17:31Z  dslausb
 * HISTORY:    Revision 1.103  2007/11/26 23:01:12Z  dslausb
 * HISTORY:    IT 8257
 * HISTORY:    Revision 1.102  2007/10/30 21:15:13Z  dslausb
 * HISTORY:    Simplified code
 * HISTORY:    Revision 1.101  2007/10/30 16:14:57Z  dslausb
 * HISTORY:    IT 8109 - Fix load set status line
 * HISTORY:    Revision 1.100  2007/10/29 20:55:40Z  dslausb
 * HISTORY:    IT 8276 - Fix status line text on clamp crossover screen.
 * HISTORY:    Revision 1.99  2007/10/25 22:10:23Z  dslausb
 * HISTORY:    IT 8263 - Revert RAS confirmation, and edit RAS connection.
 * HISTORY:    Revision 1.98  2007/10/24 22:04:46Z  dslausb
 * HISTORY:    IT 8246 - Change run summary 2 for procedures with two RBC products.
 * HISTORY:    Revision 1.97  2007/10/19 16:28:29Z  dslausb
 * HISTORY:    - Added AAS start time to the run summary screen.
 * HISTORY:    - Add AAS icons to progress bars on runproc screen
 * HISTORY:    - Fix spurious air removal message when air removal off
 * HISTORY:    - Fix MSS disconnect screen transition
 * HISTORY:    Revision 1.94  2007/09/20 14:25:27Z  dslausb
 * HISTORY:    Add different graphic for DRBC set
 * HISTORY:    Revision 1.93  2007/09/17 21:42:10Z  dslausb
 * HISTORY:    IT 7992 - Add Male-Only PLT and Male-Only Plasma config options for TRALI.
 * HISTORY:    Revision 1.92  2007/09/14 23:18:01Z  dslausb
 * HISTORY:    Revision 1.91  2007/09/04 22:27:10Z  dslausb
 * HISTORY:    IT 8055 - Added alarm for CPS zero out of range.
 * HISTORY:    Revision 1.90  2007/08/29 21:41:29Z  dslausb
 * HISTORY:    - Changed some RBC CPS High alarm functionality and appearance
 * HISTORY:    - Changed salvage volume stuff
 * HISTORY:    Revision 1.89  2007/08/15 22:44:53Z  dslausb
 * HISTORY:    Separate flags for two RBC products
 * HISTORY:    Revision 1.88  2007/08/14 18:18:16Z  dslausb
 * HISTORY:    IT7986: Combined cassette bit for PTF and RAS into one bit
 * HISTORY:    Revision 1.87  2007/08/10 15:27:33Z  dslausb
 * HISTORY:    Screen changed for DRBC Automated Split
 * HISTORY:    Revision 1.86  2007/06/01 20:09:19Z  dslausb
 * HISTORY:    IT 7959 - Added Dlog ID to the 1st Run Summary Screen
 * HISTORY:    Revision 1.85  2007/05/14 22:09:43Z  dslausb
 * HISTORY:    IT 7960 - Add config option for pre-mss flag screens
 * HISTORY:    Revision 1.84  2007/04/17 17:57:02Z  dslausb
 * HISTORY:    IT 7472 - Added confirm box for RAS connection.
 * HISTORY:    Revision 1.83  2007/04/05 23:00:13Z  dslausb
 * HISTORY:    Split up DRBC split alarm into its three respective versions.
 * HISTORY:    Revision 1.82  2007/03/15 20:42:19Z  dslausb
 * HISTORY:    IT 7379 - Split up relevant alarms into one, three, and zero channel clamps for more intuitive GUI.
 * HISTORY:    Revision 1.81  2007/03/13 17:40:19Z  dslausb
 * HISTORY:    IT 7379 - Added alternate version of clamp channel screen for black stamp set.
 * HISTORY:    Revision 1.80  2007/03/12 23:05:54Z  dslausb
 * HISTORY:    Added graphics and text for single-filter sets
 * HISTORY:    Revision 1.79  2007/03/12 18:46:06Z  dslausb
 * HISTORY:    Revised PTF setup and crossover clamp screens
 * HISTORY:    Revision 1.78  2007/03/08 19:31:07Z  dslausb
 * HISTORY:    Removed obsolete strings
 * HISTORY:    Revision 1.77  2007/02/27 00:09:08Z  dslausb
 * HISTORY:    Split channel clamp closure error alarm into two
 * HISTORY:    Revision 1.76  2007/02/22 18:56:12Z  dslausb
 * HISTORY:    Edit appearance of clamp crossover failure alarm.
 * HISTORY:    Revision 1.75  2007/02/20 23:57:26Z  dslausb
 * HISTORY:    - IT 7105 - Changed text of safety failure startup alarm
 * HISTORY:    - Added change RAS bag alarm
 * HISTORY:    Revision 1.74  2007/02/19 19:56:24Z  dslausb
 * HISTORY:    IT 5687 - Made air evacuation configurable
 * HISTORY:    Revision 1.73  2007/02/15 21:40:38Z  dslausb
 * HISTORY:    GUI review changes - don't show progress bar for undelivered products.
 * HISTORY:    Revision 1.72  2007/02/13 21:38:55Z  dslausb
 * HISTORY:    Revision 1.71  2007/02/12 20:13:49Z  dslausb
 * HISTORY:    IT 7869 - Combined RAS and PTF options and icons
 * HISTORY:    Revision 1.70  2007/01/30 00:11:34Z  dslausb
 * HISTORY:    Revision 1.69  2007/01/16 16:06:58Z  dslausb
 * HISTORY:    -- IT 7482 - Add RBC leukoreduction message and flag
 * HISTORY:    -- Changed screen transitions for RBC run summary screen
 * HISTORY:    -- Changed cassette listbox filtering to allow for PTF and RAS
 * HISTORY:    Revision 1.68  2007/01/10 23:42:34Z  dslausb
 * HISTORY:    Added PTF load screen.
 * HISTORY:    Revision 1.67  2007/01/10 20:28:28Z  dslausb
 * HISTORY:    - Added PTF graphic to alarms
 * HISTORY:    - Added RAS info to MSS abort screen
 * HISTORY:    Revision 1.66  2006/12/13 16:01:51Z  dslausb
 * HISTORY:    Added RAS value to 2nd runsum screen
 * HISTORY:    Revision 1.65  2006/12/08 20:35:25Z  dslausb
 * HISTORY:    Added RAS meter to addition screen
 * HISTORY:    Revision 1.64  2006/12/08 16:40:34Z  dslausb
 * HISTORY:    Added old RAS and PTF alarms back in.
 * HISTORY:    Revision 1.63  2006/12/07 20:56:16Z  dslausb
 * HISTORY:    - separated "Clamp PAS line" stuff from sysclamp (it's now pasInitSplit.)
 * HISTORY:    - added the clamp crossover screen.
 * HISTORY:    - added functions to guiglobs to determine which of these should be shown.
 * HISTORY:    Revision 1.62  2006/12/07 15:12:24Z  dslausb
 * HISTORY:    Updates to RAS-related config screens
 * HISTORY:    Revision 1.61  2006/12/06 18:19:44Z  dslausb
 * HISTORY:    Added buttons to Automated Processes config, and added ras connect screen.
 * HISTORY:    Revision 1.60  2006/10/11 18:34:09Z  dslausb
 * HISTORY:    Removed obsolete strings
 * HISTORY:    Revision 1.59  2006/10/06 17:19:03Z  dslausb
 * HISTORY:    Removed obsolete strings and alarms
 * HISTORY:    Revision 1.58  2006/09/25 19:52:17Z  dslausb
 * HISTORY:    IT 7766 - Added alternate donor disconnect alarm and alert for MSS (don't seal products, cassette remains down).
 * HISTORY:    Revision 1.57  2006/09/20 23:38:54Z  dslausb
 * HISTORY:    IT 7757 - Put IP address and Trima serial number on the service screen.
 * HISTORY:    Revision 1.56  2006/09/20 17:05:19Z  dslausb
 * HISTORY:    IT 7754 - Added additional error checking for cassette files
 * HISTORY:    Revision 1.55  2006/09/11 19:24:41Z  dslausb
 * HISTORY:    Revision 1.54  2006/08/21 22:31:11Z  dslausb
 * HISTORY:    IT 7715 - Changed "end run" to "end addition" on alarm screens during metered storage.
 * HISTORY:    Revision 1.53  2006/08/21 21:40:17Z  dslausb
 * HISTORY:    IT 7649 - Added "more info" screen instructing operator to clamp PAS line.
 * HISTORY:    Revision 1.52  2006/08/17 22:50:58Z  dslausb
 * HISTORY:    IT 7626 - Changed alarm text.
 * HISTORY:    IT 7689 - Changed mss connect screen
 * HISTORY:    Revision 1.51  2006/08/17 15:49:50Z  dslausb
 * HISTORY:    IT 7613 - Overhaul of Set Identification screen.
 * HISTORY:    Revision 1.50  2006/08/15 23:15:59Z  dslausb
 * HISTORY:    IT 7598 - Operator info when AAS is aborted.
 * HISTORY:    Revision 1.49  2006/08/15 21:41:33Z  dslausb
 * HISTORY:    IT 7598 - Added new alarm for mss incomplete
 * HISTORY:    Revision 1.48  2006/08/09 16:53:06Z  dslausb
 * HISTORY:    IT 7682 - Changed to 2-file cassette entry functionality
 * HISTORY:    Revision 1.47  2006/07/31 19:33:00Z  dslausb
 * HISTORY:    IT 7689 - Made "override bag volume" configurable.
 * HISTORY:    Revision 1.46  2006/07/31 17:03:32Z  dslausb
 * HISTORY:    IT 7690 - Changed "end run" to "end addition" during MSS.
 * HISTORY:    Revision 1.45  2006/07/28 23:29:29Z  dslausb
 * HISTORY:    Redesigned cassette entry
 * HISTORY:    Revision 1.44  2006/07/21 15:45:38Z  dslausb
 * HISTORY:    Revision 1.43  2006/06/30 17:47:13Z  dslausb
 * HISTORY:    Revision 1.42  2006/06/30 15:08:49Z  dslausb
 * HISTORY:    IT 7618 - Cleaned up catalog entry
 * HISTORY:    Revision 1.41  2006/06/29 20:52:48Z  dslausb
 * HISTORY:    IT 7607 - Reformatting on end run summary screens
 * HISTORY:    Revision 1.40  2006/06/29 18:46:44Z  dslausb
 * HISTORY:    IT 7607 - Fixed up labeling in end run summary screens
 * HISTORY:    Revision 1.39  2006/06/29 16:11:10Z  dslausb
 * HISTORY:    IT 7614 - Cleaned up procedure priority screen (took off yield, volume, and dose labels)
 * HISTORY:    Revision 1.38  2006/06/28 20:15:37Z  dslausb
 * HISTORY:    Removed a bunch of obsolete strings and a couple unused alarms that were calling them so that we aren't wasting resources translating them.
 * HISTORY:    Revision 1.37  2006/06/28 18:42:08Z  dslausb
 * HISTORY:    IT 7609 - Changed "solutions" to "addition" in progress bar.
 * HISTORY:    Revision 1.36  2006/06/28 17:46:35Z  dslausb
 * HISTORY:    Cleaned up and changed AAS disconnect screen.
 * HISTORY:    Revision 1.35  2006/06/27 22:53:17Z  dslausb
 * HISTORY:    Changes before gui freeze:
 * HISTORY:    - Revert to old tyvec picture off to the side for catalog entry
 * HISTORY:    - Remove frangible 'bubbles' from PAS connection screen
 * HISTORY:    Revision 1.34  2006/06/23 16:17:41Z  dslausb
 * HISTORY:    Revision 1.33  2006/06/20 16:10:24Z  dslausb
 * HISTORY:    Changed PAS line to clamp instead of frangible
 * HISTORY:    Revision 1.32  2006/06/16 22:24:46Z  dslausb
 * HISTORY:    - Changed end run screen to use the template used by other screens.
 * HISTORY:    - Added screen instructing operator to clamp PLT bag before procedure if PLT split notification is on and procedure is MSS.
 * HISTORY:    Revision 1.31  2006/06/13 17:42:53Z  dslausb
 * HISTORY:    IT 7503 - Added procedure information to PLT, Plas, and RBC buttons in the config procedure priority screen.
 * HISTORY:    Revision 1.30  2006/06/09 22:35:29Z  dslausb
 * HISTORY:    Platelet listbox has units with label.
 * HISTORY:    Revision 1.29  2006/06/09 20:04:00Z  dslausb
 * HISTORY:    IT 7545: Added message to tell user "Load an Auto P.A.S. Set"
 * HISTORY:    Revision 1.28  2006/06/09 15:36:21Z  dslausb
 * HISTORY:    PLT screen now adjusts percent carryover value if mss is toggled on and bag volume exceeds bag capacity.
 * HISTORY:    Revision 1.27  2006/06/07 23:50:32Z  dslausb
 * HISTORY:    - Added soft lower limit for percent carryover value
 * HISTORY:    - Added additional error messages for concentration and bag volume out of range
 * HISTORY:    Revision 1.26  2006/06/07 21:12:13Z  dslausb
 * HISTORY:    Updated PLT Config Screen to show both collect concentration and final concentration.
 * HISTORY:    Revision 1.25  2006/06/05 22:49:23Z  dslausb
 * HISTORY:    Removed Fixed Volume
 * HISTORY:    Revision 1.24  2006/05/31 20:34:36Z  dslausb
 * HISTORY:    Changes on mss_begin.  Lots of reformatting, got rid of vac purge screen, etc
 * HISTORY:    Revision 1.23  2006/05/30 15:02:45Z  dslausb
 * HISTORY:    Machine Screen shows number for digits needed for set / admin codes.
 * HISTORY:    Revision 1.22  2006/05/26 22:29:41Z  dslausb
 * HISTORY:    Reformatted Platelet Screen
 * HISTORY:    Revision 1.21  2006/05/26 16:40:41Z  dslausb
 * HISTORY:    Formatting on config_plt screen
 * HISTORY:    Revision 1.20  2006/05/25 22:28:24Z  dslausb
 * HISTORY:    - Made text changes from 5-25 gui review
 * HISTORY:    - Added config option for blood diversion
 * HISTORY:    Revision 1.19  2006/05/19 19:09:31Z  dslausb
 * HISTORY:    Changed endrun summary screens and sysclamp screen
 * HISTORY:    Revision 1.18  2006/05/12 21:12:22Z  dslausb
 * HISTORY:    Fixed calc stuff on PLT config screen, updated predict screen and mach screen (all for 5.P stuff)
 * HISTORY:    Revision 1.17  2006/05/11 23:15:39Z  dslausb
 * HISTORY:    Added MSS icon to status bar
 * HISTORY:    Revision 1.16  2006/05/08 15:50:11Z  dslausb
 * HISTORY:    IT 7151 - Changed new screens so that only necessary stuff shows up.
 * HISTORY:    Revision 1.15  2006/04/27 15:53:16Z  dslausb
 * HISTORY:    IT 7468 - Percent carryover is now configurable for each procedure if turned on.  Toggle is now on PLT screen.  Also, text changes.
 * HISTORY:    Revision 1.14  2006/04/21 22:41:50Z  dslausb
 * HISTORY:    Removed VerifyCassette screen
 * HISTORY:    Revision 1.13  2006/04/21 20:41:35Z  dslausb
 * HISTORY:    IT 7272 - Smoothed out functionality for admin and operator cassette entry.
 * HISTORY:    Revision 1.12  2006/04/12 22:16:22Z  dslausb
 * HISTORY:    - Changed and deleted some strings for MSS
 * HISTORY:    - Fixed a glitch with the status bar on syscassette
 * HISTORY:    Revision 1.11  2006/04/11 16:14:20Z  dslausb
 * HISTORY:    Revision 1.10  2006/04/07 19:18:44Z  dslausb
 * HISTORY:    First draft of cassette.dat file i/o stuff
 * HISTORY:    Revision 1.9  2006/03/31 17:20:21Z  dslausb
 * HISTORY:    Added button on mss config screen for PLT MSS split notification
 * HISTORY:    Revision 1.8  2006/03/30 21:35:09Z  dslausb
 * HISTORY:    IT 7148 - Reformatted leukoreduction labeling screen
 * HISTORY:    Revision 1.7  2006/03/27 22:25:23Z  dslausb
 * HISTORY:    Revision 1.6  2006/03/24 21:04:37Z  dslausb
 * HISTORY:    Updated alarms and added MSS status box on Platelet Config screen.
 * HISTORY:    Revision 1.5  2006/03/22 23:26:57Z  dslausb
 * HISTORY:    Added and edited a bunch of alarm stuff.
 * HISTORY:    Revision 1.4  2006/03/17 23:09:53Z  dslausb
 * HISTORY:    Updated text stuff after review with ubiquity
 * HISTORY:    Revision 1.3  2006/03/09 18:06:24Z  dslausb
 * HISTORY:    Fixed formatting on the 2nd runsum data screen
 * HISTORY:    Revision 1.2  2006/03/06 23:47:58Z  dslausb
 * HISTORY:    IT 7434 - Rearranged the MSS config screen
 *
 *******************************************************************************/


#if (CPU==SIMNT)
#include "string_info.h"
#include "stringError.h"
#include "trima_datalog.h"
#include "guistring.h"

void printStringErrorInfo (TEXT_STRING_ID string_id)
{
   const char* StringId = convertStringIdToString(string_id);

   DataLog (log_level_gui_error) << "String error. ID = "
                                 << StringId << " ("
                                 << string_id << ")." << endmsg;
}

DataLog_Stream& operator << (DataLog_Stream& os, TEXT_STRING_ID id)
{
   os << convertStringIdToString(id) << "(" << ((int)id) << ")";
   return os;
}

/*
  this function is now generated automatically by build_string_enum

const char* convertStringIdToString(TEXT_STRING_ID string_id)
{
    [ ... ]
}
*/

#endif

/* FORMAT HASH 23acbf1eb8162f79e8e9179c7698ed17 */

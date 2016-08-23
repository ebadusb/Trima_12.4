/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      aasAddition.hpp
 *
 */

/*
$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/aasAddition.hpp 1.3 2007/02/15 21:40:17Z dslausb Exp dslausb $
$Log: aasAddition.hpp $
Revision 1.3  2007/02/15 21:40:17Z  dslausb
GUI review changes - don't show progress bar for undelivered products.
Revision 1.2  2006/12/08 20:35:13Z  dslausb
Added RAS meter to addition screen
Revision 1.1  2006/12/04 20:53:47Z  dslausb
Initial revision
Revision 1.20  2006/09/11 19:23:07Z  dslausb
Revision 1.19  2006/08/17 22:50:38Z  dslausb
IT 7626 - Changed alarm text.
IT 7689 - Changed mss connect screen
Revision 1.18  2006/07/31 19:32:37Z  dslausb
IT 7689 - Made "override bag volume" configurable.
Revision 1.17  2006/07/26 22:32:35Z  jheiusb
7647 -- fix bag number calc
Revision 1.16  2006/07/21 16:55:32Z  dslausb
IT 7623 - Resequenced PLT flag screen
Revision 1.15  2006/06/28 20:15:30Z  dslausb
Removed a bunch of obsolete strings and a couple unused alarms that were calling them so that we aren't wasting resources translating them.
Revision 1.14  2006/06/16 22:25:04Z  dslausb
- Changed end run screen to use the template used by other screens.
- Added screen instructing operator to clamp PLT bag before procedure if PLT split notification is on and procedure is MSS.
Revision 1.13  2006/06/06 19:41:10Z  dslausb
Added blinking graphics for MSS channel test and prime screens.
Revision 1.12  2006/06/05 20:57:28Z  jheiusb
7350 -- distinguish between the two MSS setup screens in PFR
Revision 1.11  2006/05/31 20:34:30Z  dslausb
Changes on mss_begin.  Lots of reformatting, got rid of vac purge screen, etc
Revision 1.10  2006/05/18 16:18:34Z  dslausb
More updated text and graphics
Revision 1.9  2006/05/10 21:22:30Z  dslausb
Added a bunch of graphics to the GUI.
Revision 1.8  2006/05/05 23:02:41Z  dslausb
IT 7223 - Added some updated graphics stuff.
Revision 1.7  2006/05/03 17:25:00Z  jheiusb
7515 -- PFR for MSS Disconnect
Revision 1.6  2006/03/31 22:25:13Z  dslausb
PLT SS split screen is now conditional based on the configuration.
Revision 1.5  2006/03/23 22:53:10Z  jheiusb
7350 -- add some PFR driven GUI changes
Revision 1.4  2006/03/17 23:09:50Z  dslausb
Updated text stuff after review with ubiquity
Revision 1.3  2006/03/16 15:13:17Z  dslausb
Implemented the MSS progress bar.
Revision 1.2  2006/03/07 22:13:29Z  dslausb
Revision 1.1  2006/03/03 17:40:28Z  dslausb
Initial revision
Revision 1.16  2005/12/20 21:41:41Z  dslausb
7202 - Metered Storage
- Moved 'clamp crossover' screen to load
- Added 'clamp channel' screen
- Changed alarm wording for AC closure alarm/alert
Revision 1.15  2005/12/16 19:53:18Z  dslausb
IT 7202 - Metered Storage - Added screens at the end, fixed override default bag volume button so it doesn't reboot the machine.
Revision 1.15  2005/12/14 15:52:16Z  dslausb
IT 7202 - Metered Storage - Changed graphics and text on later screens for MSS.
Revision 1.14  2005/12/09 22:59:09Z  dslausb
IT 7202 - Metered Storage - Fixed override default bag volume and number of bags needed for PLT and MSS connect screens
Revision 1.13  2005/12/09 17:52:30Z  dslausb
IT 7202 - Metered Storage - Got rid of extra completed screen and state.
Revision 1.12  2005/12/09 15:44:31Z  dslausb
IT 7202 - Metered Storage - More GUI changes: layout tweaking, improved keypad functionality (still needs work) and confirm button on RBC connect screen.
Revision 1.11  2005/12/07 23:39:53Z  dslausb
IT 7202 - More MSS Gui Changes
Revision 1.10  2005/12/07 18:26:19Z  dslausb
IT 7202 - Metered Storage - Updated GUI for MSS process.
Revision 1.9  2005/11/16 19:51:13Z  jheiusb
7230 -- add  alarm processing to split RBC and PLT runs if one errors
Revision 1.8  2005/11/04 21:04:47Z  jheiusb
7202 -- add pause to PLT after Prime
Revision 1.7  2005/10/19 21:43:02Z  jheiusb
more gui fine tuning
Revision 1.6  2005/10/18 22:43:30Z  jheiusb
7202 -- add text for MSS and misc clean up of MSS gui
Revision 1.5  2005/10/11 21:15:22Z  jheiusb
7202 -- add channel check code
Revision 1.4  2005/10/07 22:08:21Z  jheiusb
7202 ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.3  2005/10/05 20:37:22Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

// Single include define guardian
#ifndef MSSBEGIN_HPP
#define MSSBEGIN_HPP



// CLASS INCLUDE


#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"

// MESSAGE DECLARES

#include "message.h"

// CLASS DECLARES
#include "discmetr.hpp"
#include "troublemeter.h"
#include "trimamessages.h"

class Screen_AASADDITION
   : public Base_Apheresis_Window
{

private:

   int _meterUpdateLoggingCount;      // Only log every 10th update or so.

   // The timebar, pressure, and collect placard bitmap objects
   Bitmap _bmpAasProg;

   // meter widgets
   TroubleMeter* _aasProgBar;

   Display_Text
      _txtAasProg,
      _txtAasLabel;

   // BUILD_ALL_METERS
   // This method defines and controls the construction of all the meter widgets
   void buildAasMeter ();

   void updateAasMeter ();

   void stopListeningToProcUpdates ();

   // BUILD_METER
   // This method builds all meter widgets
   TroubleMeter* constructProgMeter (OSColor color, TroubleMeter::MeterType type, short x, short y, short height, short width);

public:


   // CONSTRUCTOR
   Screen_AASADDITION();


   // DESTRUCTOR
   virtual ~Screen_AASADDITION();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_CANCEL_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the cancel button.
   virtual void process_cancel_button ();


   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

};  // END


// End of single include define guardian
#endif

/* FORMAT HASH 00d3657b36cc632080bb175967966879 */

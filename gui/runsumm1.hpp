/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_RUNSUMM1

*     File name:    runsumm1.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This is a screen derived from the base apheresis class. It
         displays procedure data received from the procedure domain at
         the end of the run.

         Note: We can show residual status or residual volumes on this
         screen, and they're mutually exclusive.  The way that works:
         If features.bin is configured to show residual volumes, those fields
         are displayed.  If not, but if plasma or saline rinseback is
         configured on, and we're using an appropriate set for that,
         it's status (complete or incomplete) is displayed.  Otherwise,
         neither is displayed.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/runsumm1.hpp 1.33 2009/04/06 17:17:50Z dslausb Exp dslausb $
$Log: runsumm1.hpp $
Revision 1.33  2009/04/06 17:17:50Z  dslausb
IT 9151 - Residual volume should not be shown in METERED_STORAGE state.
Revision 1.32  2008/10/03 16:20:06Z  dslausb
Move away from dynamic memory where possible
Revision 1.31  2008/08/16 19:12:23Z  dslausb
Mark's Screenshot Code
Revision 1.31  2008/08/15 20:37:18Z  mereusb
IT 8107: Change for screenshot scripts.
Revision 1.30  2008/06/12 17:37:09  dslausb
IT 8679 - Dashes for zeroes
Revision 1.29  2008/06/10 20:45:44Z  mereusb
IT 8107: Lots of little foundational changes to help ease writing of screenshot scripts.
Revision 1.28  2008/06/06 20:53:55Z  mereusb
IT 8647: Changed End of Run Time and AAS Start Time fields to use the TimeDataField object, which allows for localisable AM/PM.
Revision 1.27  2008/06/02 17:30:37  dslausb
Revision 1.26  2008/06/02 16:45:57Z  dslausb
IT 8464 - Plasma rinseback operator notification
Revision 1.25  2008/05/30 20:56:31Z  dslausb
Removed new configuration for donor 8473
Revision 1.24  2008/05/21 21:45:45Z  dslausb
IT 8473 - Added residual volumes to run summary screen
Revision 1.23  2008/05/06 15:20:17Z  dslausb
IT 8590 - Hgb
Revision 1.22  2008/03/24 21:13:17Z  dslausb
IT 8374 - Change the offline addition screen
Revision 1.21  2007/10/19 16:28:19Z  dslausb
- Added AAS start time to the run summary screen.
- Add AAS icons to progress bars on runproc screen
- Fix spurious air removal message when air removal off
- Fix MSS disconnect screen transition
Revision 1.20  2007/06/01 20:09:13Z  dslausb
IT 7959 - Added Dlog ID to the 1st Run Summary Screen
Revision 1.19  2006/12/07 15:58:29Z  dslausb
Ported over late GUI changes to 5.2
Revision 1.18  2006/09/11 19:24:02Z  dslausb
Revision 1.17  2004/02/27 15:22:14Z  rm70006
IT 6892.  Put guard in to only show flagging screen when rinsback is complete.
Revision 1.16  2003/12/02 20:59:35Z  rm70006
IT 6054.  Add refresh resources routine to windows that modify status bar to work happily with CFR 11 screen.
Revision 1.15  2003/10/16 17:02:26Z  jl11312
- changes for displaying run summary during rinse back (IT 5862)
Revision 1.14  2003/06/17 16:56:18Z  rm10919
Japanese configuration changes.
Revision 1.13  2003/04/24 16:23:08Z  pn02526
Re-enable verify and label screen displays using PQ data from the CDS.
Revision 1.12  2003/01/06 11:31:19  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.11  2002/11/11 07:18:10  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.10  2002/08/15 22:17:19  sb07663
compiling version for Vx
Revision 1.9  2002/05/07 14:00:02  sb07663
First cut at VxWorks compatibility
Revision 1.8  2001/05/16 22:50:58  pn02526
Revise count/verify flags code for new Product Quality data
Revision 1.7  2000/06/22 15:37:44  ms10234
Move the Run Summary structure to the trima common project
Revision 1.6  1999/06/14 23:54:53  TW09453
Change runsummary1 to display the total saline used and to used
count flag info to decide which screen to proceed to.
Revision 1.5  1997/04/22 21:19:37  tw09453
Remove AC Infusion Rate from the Runsumm1 screen.
Revision 1.4  1997/02/25 04:32:09  tw09453
Add the Post HCT value to the screen and detect the type of time
to display (24-hour or AM/PM) via the configuration object.
Revision 1.3  1996/07/08 16:16:48  tw09453
Update the file header

*/


// Single include define guardian
#ifndef RUNSUMM1_HPP
#define RUNSUMM1_HPP



// CLASS INCLUDE

#include <vxworks.h> // For #if (CPU==SIMNT) compiler directive
#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "runsummary.h"
#include "pqinformation.h"
#include "textDataField.hpp"
#include "timeDataField.hpp"
#include "blinkCollection.hpp"


// CLASS DECLARES

//////////////////////////
// ResidualStatusDataField
// This is a one-off class for displaying residual status data.
class ResidualStatusDataField
   : public TextDataField
{
public:
   ResidualStatusDataField(TEXT_STRING_ID labelTxtId,
                           TEXT_STRING_ID dataTxtId);

   virtual void setData (SPECIAL_RINSEBACK_STATUS status);
};
////////////////////////////


class Screen_RUNSUMM1
   : public Base_Apheresis_Window
{
private:

   bool            _usingHemoglobinUnits;

   BlinkCollection _blinkCollection;

   Bitmap
      _bmpTxtBoxen,

   // These can be used for AAS start time or average inlet flow
      _bmpExtraBoxLeftTop,
      _bmpExtraBoxLeftBottom,

   // These are used for residual volumes.
      _bmpExtraBoxRightTop,
      _bmpExtraBoxRightBottom;

   TextDataField
      _dataTotalAcUsed,
      _dataLengthOfRun,
      _dataPostPltCount,
      _dataActualAcToDonor,
      _dataPostCrit,
      _dataTbvProcessed,
      _dataSalineUsed,
      _dataAvgInletFlow,    // Changed to be used as maximum draw flow
      _dataPrbcResidualLoss,
      _dataPlasmaResidualLoss;

   TimeDataField _dataEndOfRunTime;
   TimeDataField _dataAasStartTime;

   ResidualStatusDataField
      _dataSalineRinsebackStatus,
      _dataPlasmaRinsebackStatus;

   Display_Text

   // The Dlog number
      _txtDlogNumber,
   // The screen label
      _txtScreenLabel;


   bool
      _runComplete,  // For stat bar
      _showAvgInletFlow,
      _showResidualVol;

   // The Product Quality CDS data access class
   PQInformation _pqa;

public:
   // CONSTRUCTOR
   Screen_RUNSUMM1();


   // DESTRUCTOR
   virtual ~Screen_RUNSUMM1();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();

   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the goback button.
   virtual void process_goback_button ();


   // REFRESH_RESOURCES
   // This method handles refreshing the screen after allocate resources has occured
   virtual void refresh_resources ();


protected:
   void processRunsumData ();

   void setStatLineAndBar ();

   void outputData ();

   bool checkForRunCompleteAnomoly ();

   // Based on the passed variables, decide what screen layout to use.
   // (Set it up this way because we can call this same function for unit
   // testing.)
   void selectScreenLayout (bool showAasStartTime,
                            bool showAvgInletFlow,
                            bool showResidualLoss,
                            bool plasRinsebackOn,
                            bool salineRinsebackOn);

   ////// MESSAGING DECLARATIONS AND SUPPORTING POINTERS //////
   Message<RUN_SUMMARY_STRUCT> _msgRunSummaryStats;

   void initBlinkCollection ();
    #if (CPU==SIMNT) // For SIMNT unit testing
protected:
   virtual void handleTraverseMessage ();
    #endif // #if (CPU==SIMNT)

}; // END of Screen_RUNSUMM1 Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 889bed79b576d24e11682e191f91c680 */

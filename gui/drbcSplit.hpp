/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2007 Gambro BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_DRBC_SPLIT

*     File name:    drbcSplit.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

    This class represents all three different versions of the DRBC split
    screen: filtered, non-filtered, and RAS split.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.6  2008/08/16 19:11:46Z  dslausb
Mark's Screenshot Code
Revision 1.6  2008/08/15 20:35:24Z  mereusb
IT 8107: Change for screenshot scripts.
Revision 1.5  2008/07/03 09:03:59  mereusb
IT 8107 - Improved screenshot coverage.
Revision 1.4  2008/06/13 10:46:39  dslausb
IT 7819 - Tweaks to alarm layout based on localization request
Revision 1.3  2008/06/11 18:11:23Z  dslausb
IT 8686 - New "pas vol remaining" functionality on the PAS empty screen
Revision 1.2  2007/10/17 20:36:43Z  dslausb
Change DRBC split screens to comply with removal of autosplit
Revision 1.1  2007/04/05 22:58:42Z  dslausb
Initial revision
Revision*/



// Single include define guardian
#ifndef DRBC_SPLIT_HPP__
#define DRBC_SPLIT_HPP__



// CLASS INCLUDE

#include "oipAlarmBase.hpp"
#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"

// alarm include
#include "alarms.h"

// GUI <-> procedure screen include
#include "guiproc.h"

// proc includes
#include "predict_msgs.h"
#include "trimamessages.h"


// CLASS DECLARES

class Screen_DRBC_SPLIT
   : public OipAlarmBase
{

private:

   // The DRBC split bitmap object
   Bitmap
            _bmpSplitDrbcsCollectNonfiltered, // Split during collection with a nonfiltered set
            _bmpSplitDrbcsCollectFiltered,    // Split during collection with a filtered set
            _bmpSplitDrbcsAddition,           // Split during RBC additive solutions (always filtered)
            _bmpBackground;                   // The text box.

   // The display text objects
   Display_Text
      _txtSplitDuringCollectNonfiltered,
      _txtSplitDuringCollectFiltered,
      _txtSplitDuringAddition;

   // The RF continue bitmap button
   Button_Bitmap
      _btnContinue,
      _btnCancel,
      _btnRinseback;

   // The invocation type flag
   enum DISPLAY_MODE
   {
      DISPLAY_MODE_COLLECT_FILTERED,
      DISPLAY_MODE_COLLECT_NONFILTERED,
      DISPLAY_MODE_RAS_ADDITION
   };

   // How this allocation was invoked: either BOLUS or NORMAL
   DISPLAY_MODE _currentDisplayMode;

   static DISPLAY_MODE getDisplayMode ();

public:
   // CONSTRUCTOR
   Screen_DRBC_SPLIT();


   // DESTRUCTOR
   virtual ~Screen_DRBC_SPLIT();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_RF_CANCEL_BUTTON
   // This method is called when the RF cancel button is pressed
   void processCancelBtn ();


   // PROCESS_RF_CONTINUE_BUTTON
   // This method is called when the RF continue button is pressed
   void processContinueBtn ();


   // PROCESS_RF_RINSEBACK_BUTTON
   // This method is called when the RF rinseback button is pressed
   void processRinsebackBtn ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

protected:
   virtual void setUpOipInfo (const char* allocation_parameter);

#if CPU==SIMNT
protected:
   virtual void handleTraverseMessage ();
#endif /* if CPU==SIMNT */

};


#endif

/* FORMAT HASH 49d71755cd5b25b5034d334fd3865170 */

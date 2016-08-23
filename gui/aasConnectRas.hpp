/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      aasConnectRas.hpp
 *
 */

/*
$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/aasConnectRas.hpp 1.12 2008/10/03 16:13:00Z dslausb Exp dslausb $
$Log: aasConnectRas.hpp $
Revision 1.12  2008/10/03 16:13:00Z  dslausb
Move away from dynamic memory where possible
Revision 1.11  2008/09/23 22:33:27Z  dslausb
IT 8867 - Fix timing issue that caused MSS freezup.
Revision 1.10  2008/06/10 20:44:20Z  mereusb
IT 8107: Lots of little foundational changes to help ease writing of screenshot scripts.
Revision 1.9  2008/05/15 16:21:19  dslausb
IT 8463 - Move open crossover instruction to the channel clamp screen
Revision 1.8  2008/04/01 20:58:21Z  dslausb
IT 8077 - Implement separate end run screens for RAS and PAS
Revision 1.7  2007/11/26 23:01:05Z  dslausb
IT 8257
Revision 1.6  2007/10/18 21:33:15Z  dslausb
IT 7210 - Changes to make sure graphics indicate correct RAS bag connection, etc.
Revision 1.5  2007/09/20 14:25:19Z  dslausb
Add different graphic for DRBC set
Revision 1.4  2007/04/17 17:56:33Z  dslausb
IT 7472 - Added confirm box for RAS connection.
Revision 1.3  2007/03/06 23:51:15Z  dslausb
- Changed MSS progress bar to a one-bar system.
- Changed the order of the connect screens
- Changed the way proc updates GUI of the MSS volumes
Revision 1.2  2007/02/15 21:40:26Z  dslausb
GUI review changes - don't show progress bar for undelivered products.
Revision 1.1  2006/12/06 18:17:50Z  dslausb
Initial revision
Revision 1.1  2006/12/04 20:53:54Z  dslausb
Initial revision


*/

// Single include define guardian
#ifndef __AAS_CONNECT_RAS_HPP__
#define __AAS_CONNECT_RAS_HPP__



// CLASS INCLUDE
#include <vxworks.h> // Needed here for #if CPU==SIMNT.

#include "aphwind.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "trimamessages.h"
#include "keypad.h"
#include "timermessage.h"


// MESSAGE DECLARES
#include "message.h"

class Screen_AASCONNECTRAS
   : public Base_Apheresis_Window
{

private:

   Bitmap
            _bmpConnectRas,
            _bmpConnectRasDrbc,
            _bmpConnectRasDrbcOneBag; // In case they're running a DRBC set with an SRBC procedure for some reason.

   Display_Text
      _txtPanel1Srbc,
      _txtPanel1Drbc,
      _txtPanel1DrbcOneBag,

      _txtPanel2Srbc,
      _txtPanel2Drbc,

      _txtPanel3Srbc,
      _txtPanel3Drbc,
      _txtPanel3DrbcOneBag,

      _txtConnectRasOnly,
      _txtNumBags;

   ConfirmBoxConfirmMsg _connectionConfirmed;
   ConfirmBoxCancelMsg  _connectionCanceled;

   void createConfirmCallbacks ();
   void destroyConfirmCallbacks ();
   void gotoNextScreen ();

   void confirmCB ();
   void cancelCB ();

   void callConfirmBox ();
   void deallocateTextAndGraphics ();

   void pickTextAndGraphics (int numBags, int setIsDrbc);

   ///// Set the number of bags.
   // This version choosed the number of bags and then sets it.
   int setNumBags ();
   // This version just sets the number based on the passed argument.
   int setNumBags (int numBags);

   void procReadyForConnect (void);

   // Timer to drive the reminder sound
   TimerMessage _timerReminder;

protected:
   void reminder_timer_handler ();


public:


   // CONSTRUCTOR
   Screen_AASCONNECTRAS();


   // DESTRUCTOR
   virtual ~Screen_AASCONNECTRAS();


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

   // PROCESS_GOBACK_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the go back button.
   // virtual void process_goback_button ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

    #if (CPU==SIMNT)
protected:
   virtual void handleTraverseMessage ();
    #endif // #if (CPU==SIMNT)
}; // END


// End of single include define guardian
#endif

/* FORMAT HASH d9eed0d35df5b6aefd581461842c30c3 */

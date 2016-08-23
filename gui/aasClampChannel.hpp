/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      aasClampChannel.hpp
 *
 */

/*
$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/aasClampChannel.hpp 1.7 2008/08/16 19:10:57Z dslausb Exp $
$Log: aasClampChannel.hpp $
Revision 1.7  2008/08/16 19:10:57Z  dslausb
Mark's Screenshot Code
Revision 1.7  2008/08/15 20:30:18Z  mereusb
IT 8107: Minor change for screenshot scripts.
Revision 1.6  2008/06/10 14:44:08  mereusb
IT 8107: Lots of little foundational changes to help ease writing of screenshot scripts.
Revision 1.5  2008/05/02 14:54:43  dslausb
Added some unit testing stuff to get screenshots of different versions of channel clamp screen
Revision 1.4  2007/05/10 20:31:44Z  dslausb
Added kit-conditional graphics for clamp channel screen
Revision 1.3  2007/03/21 23:14:46Z  dslausb
AAS clamp based on set id PAS/RAS
Revision 1.2  2007/03/13 17:40:13Z  dslausb
IT 7379 - Added alternate version of clamp channel screen for black stamp set.
Revision 1.1  2006/12/04 20:53:09Z  dslausb
Initial revision

*/

// Single include define guardian
#ifndef __AAS_CLAMP_CHANNEL_HPP__
#define __AAS_CLAMP_CHANNEL_HPP__

#include <vxworks.h> // for SIMNT-only compiler directives

// CLASS INCLUDE


#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "trimamessages.h"
#include "timermessage.h"

// MESSAGE DECLARES

#include "message.h"

class Screen_AASCLAMPCHANNEL
   : public Base_Apheresis_Window
{
private:

   Bitmap
            _bmpClampChannelWhitePas,
            _bmpClampChannelWhiteRas,
            _bmpClampChannelWhiteBoth,
            _bmpClampChannelWhiteBlack;

   Display_Text
      _txtClampChannelUnclampCrossover,
      _txtClampChannelClampChannelLines,
      _txtClampChannelClampChannelLine,
      _txtClampChannelClampChannelLinesOnly;

   void chooseTextAndGraphic ();
   void chooseTextAndGraphic (bool setIsPasEquipped, bool setIsRasEquipped, bool setIsWhiteStamp);
   void deallocate_text_and_graphics ();

   // Timer to drive the reminder sound
   TimerMessage _timerReminder ;

protected:
   void reminder_timer_handler ();



public:


   // CONSTRUCTOR
   Screen_AASCLAMPCHANNEL();


   // DESTRUCTOR
   virtual ~Screen_AASCLAMPCHANNEL();


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

       #if CPU==SIMNT
protected:
   virtual void handleTraverseMessage ();
       #endif
};  // END


// End of single include define guardian
#endif

/* FORMAT HASH 2f07b976c3b2e61e7e52cfdb14228cb5 */

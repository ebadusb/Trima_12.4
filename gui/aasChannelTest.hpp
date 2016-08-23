/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      aasChannelTest.hpp
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/gui/rcs/aasChannelTest.hpp 1.6 2008/10/03 16:12:47Z dslausb Exp jheiusb $
$Log: aasChannelTest.hpp $
Revision 1.6  2008/10/03 16:12:47Z  dslausb
Move away from dynamic memory where possible
Revision 1.5  2008/09/23 22:33:20Z  dslausb
IT 8867 - Fix timing issue that caused MSS freezup.
Revision 1.4  2007/08/15 18:22:09Z  dslausb
Moved procrun_cds object to guiglobs so everybody can use it.
Revision 1.3  2007/08/15 16:51:32Z  jheiusb
7930 -- disable RAS if rinseback failes to complete
Revision 1.2  2007/05/08 15:58:30Z  dslausb
Implemented kit-specific graphics, and blinking "do not seal" graphic on rinseback screen
Revision 1.1  2006/12/04 20:53:16Z  dslausb
Initial revision

*/

// Single include define guardian
#ifndef __AAS_CHANNEL_TEST_HPP__
#define __AAS_CHANNEL_TEST_HPP__



// CLASS INCLUDE


#include "aphwind.hpp"
#include "btncanc.hpp"
// #include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "timermessage.h"
#include "trimamessages.h"
#include "cassette_mgr.hpp"

// MESSAGE DECLARES
#include "message.h"

class Screen_AASCHANNELTEST
   : public Base_Apheresis_Window
{

private:

   BITMAP_ID    _graphicA, _graphicB;   // The two bitmap graphics for rotating.

   Bitmap       _bmpChannelTest;
   Display_Text _txtAasChannelTest;

   // Screen Timer Handler pointers
   TimerMessage               _animationTimer ;

   VerifyAcClosureCompleteMsg _msgAcClosureVerified;
   void setupConditionalGraphic ();
   void gotAcClosureVerifiedMsg ();


protected:

   void gotoConnect ();

public:


   // CONSTRUCTOR
   Screen_AASCHANNELTEST();


   // DESTRUCTOR
   virtual ~Screen_AASCHANNELTEST();


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

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

protected:

   // helper funcs
   void rotateGraphicsChannelTest ();

};


// End of single include define guardian
#endif

/* FORMAT HASH 7349e5d246d06953a92d5fdd4882ac86 */

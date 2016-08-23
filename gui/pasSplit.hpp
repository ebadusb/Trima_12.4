/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pasSplit.hpp
 *
 */

/*
$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/pasSplit.hpp 1.1 2006/12/04 20:53:36Z dslausb Exp dslausb $
$Log: pasSplit.hpp $
Revision 1.1  2006/12/04 20:53:36Z  dslausb
Initial revision

*/

// Single include define guardian
#ifndef __PAS_SPLIT_HPP__
#define __PAS_SPLIT_HPP__



// CLASS INCLUDE
#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "trimamessages.h"
#include "timermessage.h"

// MESSAGE DECLARES
#include "message.h"

class Screen_PAS_SPLIT
   : public Base_Apheresis_Window
{

private:

   Bitmap       _pltSplit;
   Display_Text _txtSplitInstructions;

   // Timer to drive the reminder sound
   TimerMessage _timerReminder ;

protected:
   void reminder_timer_handler ();

public:


   // CONSTRUCTOR
   Screen_PAS_SPLIT();


   // DESTRUCTOR
   virtual ~Screen_PAS_SPLIT();


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

}; // END


// End of single include define guardian
#endif

/* FORMAT HASH 8d5636ba768a3ccfea467afb3c995fc9 */

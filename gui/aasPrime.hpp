/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      aasPrime.hpp
 *
 */

/*
$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/aasPrime.hpp 1.4 2007/05/08 15:58:24Z dslausb Exp dslausb $
$Log: aasPrime.hpp $
Revision 1.4  2007/05/08 15:58:24Z  dslausb
Implemented kit-specific graphics, and blinking "do not seal" graphic on rinseback screen
Revision 1.3  2007/03/27 14:53:47Z  dslausb
IT 7924 - Split notification comes at the end of prime.
Revision 1.2  2007/03/06 23:51:39Z  dslausb
- Changed MSS progress bar to a one-bar system.
- Changed the order of the connect screens
- Changed the way proc updates GUI of the MSS volumes
Revision 1.1  2006/12/04 20:53:28Z  dslausb
Initial revision


*/

// Single include define guardian
#ifndef __AAS_PRIME_HPP__
#define __AAS_PRIME_HPP__



// CLASS INCLUDE


#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "timermessage.h"
#include "trimamessages.h"
#include "keypad.h"
#include "message.h"

class Screen_AASPRIME
   : public Base_Apheresis_Window
{

private:

   BITMAP_ID    _graphicA, _graphicB;   // The two bitmap graphics for rotating.

   Bitmap       _bmpAasPrime;

   Display_Text _txtAasPrime;

   // Screen Timer Handler pointers
   TimerMessage           _animationTimer ;

   MSSVacPurgeCompleteMsg _msgVacPurgeComplete;

   void setupConditionalGraphic ();

public:

   // CONSTRUCTOR
   Screen_AASPRIME();


   // DESTRUCTOR
   virtual ~Screen_AASPRIME();


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
   void rotatePrimeGraphics ();
   void showSplitIfNecessary ();
};  // END


// End of single include define guardian
#endif

/* FORMAT HASH c60f88ccfedcde6e8f9e660ff5e9f60a */

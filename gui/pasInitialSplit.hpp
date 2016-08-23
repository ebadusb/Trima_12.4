/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_PAS_INIT_SPLIT

*     File name:    pasInitialSplit.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the screen that directs the operator to
         clamp the PAS line.  It should only be displayed if the PAS split
         notification is enabled, and the set is pas enabled, and, the procedure
         (if selected) is PAS.


*************************< FOCUSSED GUI DOMAIN >****************************
*/



// Single include define guardian
#ifndef __PAS_INITIAL_SPLIT_HPP__
#define __PAS_INITIAL_SPLIT_HPP__


#include <vxworks.h> // Needed for SIMNT compiler directives

// CLASS INCLUDE
#include "aphwind.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "button_bitmap.h"

// CLASS DECLARES



class Screen_PAS_INIT_SPLIT
   : public Base_Apheresis_Window
{

protected:
   // The disposable clamp placard bitmap object
   Bitmap
            _bmpClampPltBag,
            _bmpClampPltBagJp;

   // The display text objects
   Display_Text
      _txtClampPltBag,
      _txtClampPltBagJp,
      _txtDontConnectAc;

   bool          _a2donorMitigation;

   Button_Bitmap _btn_unload;         // sigh!!!!

   void chooseTextAndGraphic (bool jpSampleSetup);

public:

   // CONSTRUCTOR
   Screen_PAS_INIT_SPLIT();

   // DESTRUCTOR
   virtual ~Screen_PAS_INIT_SPLIT();

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
   // the goback button.
   virtual void process_goback_button ();

   void process_unload ();

       #if CPU==SIMNT
   virtual void handleTraverseMessage ();
       #endif // #if CPU==SIMNT

};  // END of Screen_PAS_INIT_SPLIT Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 50cec1d67e45e15a9ce829fcb85c9ecd */

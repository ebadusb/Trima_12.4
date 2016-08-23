/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2013 Terumo BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_JP_CLAMP_PLASMA_AIR_BAG

*     File name:    jpClampPlasmaAirBag.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the screen that directs the operator to
         clamp the plasma air removal bag line.  It should only be displayed in
         Japan for the plasma-only kit.


*************************< FOCUSSED GUI DOMAIN >****************************
*/



// Single include define guardian
#ifndef __JP_CLAMP_PLASMA_AIR_BAG_HPP__
#define __JP_CLAMP_PLASMA_AIR_BAG_HPP__


// CLASS INCLUDE
#include "aphwind.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "button_bitmap.h"


// CLASS DECLARES



class Screen_JP_CLAMP_PLASMA_AIR_BAG
   : public Base_Apheresis_Window
{

protected:
   // The disposable clamp placard bitmap object
   Bitmap _bmpClampPlasmaBagJp;

   // The display text objects
   Display_Text
      _txtClampPltBagJp,
      _txtDontConnectAc;

   bool          _a2donorMitigation;
   Button_Bitmap _btn_unload;         // sigh!!!!


public:

   // CONSTRUCTOR
   Screen_JP_CLAMP_PLASMA_AIR_BAG();

   // DESTRUCTOR
   virtual ~Screen_JP_CLAMP_PLASMA_AIR_BAG();

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


};  // END of Screen_JP_CLAMP_PLASMA_AIR_BAG Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 423d8f4d15561285d9cc7d844a5a9c9f */

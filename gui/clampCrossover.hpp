/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_CLAMP_CROSSOVER

*     File name:    clampCrossover.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the screen that directs the operator to
         clamp the crossover line prior to pressure testing. It is derived
         from the base apheresis class, and will only be displayed for
         disposable sets that have a crossover line (MSS RBC sets).


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/clampCrossover.hpp 1.3 2007/02/22 20:36:07Z dslausb Exp dslausb $
$Log: clampCrossover.hpp $
Revision 1.3  2007/02/22 20:36:07Z  dslausb
GUI changes for new disposable test.
Revision 1.2  2007/02/13 21:38:52Z  dslausb
Revision 1.1  2006/12/07 20:54:58Z  dslausb
Initial revision


*/



// Single include define guardian
#ifndef __CLAMP_CROSSOVER_HPP__
#define __CLAMP_CROSSOVER_HPP__



// CLASS INCLUDE


#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"


// MESSAGE DECLARES

#include "message.h"
#include "timermessage.h"


// CLASS DECLARES



class Screen_CLAMP_CROSSOVER
   : public Base_Apheresis_Window
{
private:
   // The disposable clamp placard bitmap object
   Bitmap
            _bmpClampCrossover;

   // The display text objects
   Display_Text _txtClampCrossover ;

   // Timer to drive the reminder sound
   TimerMessage _timerReminder ;

protected:

   void reminder_timer_handler ();


public:


   // CONSTRUCTOR
   Screen_CLAMP_CROSSOVER();


   // DESTRUCTOR
   virtual ~Screen_CLAMP_CROSSOVER();


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

};  // END of Screen_CLAMP_CROSSOVER Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH e6b6b26697144a03ce71f7995ff9c924 */

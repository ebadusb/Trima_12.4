/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   N/A - associated with the Base_Apheresis_Window class

*     File name:    aphcalbk.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

          This file contains C callback functions that are called when one of the
          four standard Apheresis window buttons is pushed. Each button (goback,
          cancel, continue, lowerleft) has its own callback routine.  When the
          callback routine is connected to its associated button, it is passed as
          the user parameter, the address of the apheresis window "object" instance.
          The C callback routine will then call the appropriate button processing
          routine for that window when the disarm callback event is triggered.

*************************< FOCUSSED GUI DOMAIN >***************************/



// Single include define guardian
#ifndef APHCALBK_HPP
#define APHCALBK_HPP


// APH_CALLBACK_GOBACK
// This C function calls the button handling method for the currently
// active apheresis screen.

void aph_callback_goback (void* data) ;


// APH_CALLBACK_CANCEL
// This C function calls the button handling method for the currently
// active apheresis screen.

void aph_callback_cancel (void* data) ;


// APH_CALLBACK_CONTINUE
// This C function calls the button handling method for the currently
// active apheresis screen.

void aph_callback_continue (void* data) ;



// APH_CALLBACK_LOWLEFT
// This C function calls the button handling method for the currently
// active apheresis screen.

void aph_callback_lowleft (void* data) ;


// End of single include define guardian
#endif

/* FORMAT HASH c3ae32a0159e8b0f141e8323d63a7f1b */

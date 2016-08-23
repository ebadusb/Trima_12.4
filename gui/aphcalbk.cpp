/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  N/A

*     File name:   aphcalbk.cpp

*     Contents:    Implementations of callback routines

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#include <vxworks.h>


// Focussed #INCLUDES


#include "aphwind.hpp"
#include "aphcalbk.hpp"



////////////////////////////////////////////////////////////////////////////////
// APH_CALLBACK_GOBACK

void aph_callback_goback (void* data)
{
   // Actually call the process button routine for the current apheresis
   //  screen object.
   ((Base_Apheresis_Window*)data) -> process_goback_button ();

}      // END of APH_CALLBACK_GOBACK



////////////////////////////////////////////////////////////////////////////////
// APH_CALLBACK_CANCEL

void aph_callback_cancel (void* data)
{
   // Actually call the process button routine for the current apheresis
   //  screen object.
   ((Base_Apheresis_Window*)data) -> process_cancel_button ();

}      // END of APH_CALLBACK_CANCEL


////////////////////////////////////////////////////////////////////////////////
// APH_CALLBACK_CONTINUE

void aph_callback_continue (void* data)
{
   // Actually call the process button routine for the current apheresis
   //  screen object.
   ((Base_Apheresis_Window*)data) -> process_continue_button ();

}      // END of APH_CALLBACK_CONTINUE



////////////////////////////////////////////////////////////////////////////////
// APH_CALLBACK_LOWLEFT

void aph_callback_lowleft (void* data)
{
   // Actually call the process button routine for the current apheresis
   //  screen object.
   ((Base_Apheresis_Window*)data) -> process_lowleft_button ();

}      // END of APH_CALLBACK_LOWLEFT

/* FORMAT HASH afb658ae197503722c27086fb664c01a */

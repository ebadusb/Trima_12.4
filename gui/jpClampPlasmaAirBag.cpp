/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_JP_CLAMP_PLASMA_AIR_BAG

*     File name:   jpClampPlasmaAirBag.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "jpClampPlasmaAirBag.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "CatalogChecker.h"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "software_cds.h"

// GLOBAL FUNCTIONS FOR CALLBACKS ARE DONE, DUDE.
void unload_jpplsclamp_cb_btn (void* data)
{
   ((Screen_JP_CLAMP_PLASMA_AIR_BAG*)data)->process_unload ();
}


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_JP_CLAMP_PLASMA_AIR_BAG::Screen_JP_CLAMP_PLASMA_AIR_BAG()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG),

     _bmpClampPlasmaBagJp(400, 145, BITMAP_PLASMA_CLAMP_JP),
     _txtClampPltBagJp(textMiscClampPlasmaBagJp),
     _txtDontConnectAc(textMiscSysDispDontConnectAc),
     _a2donorMitigation(guiglobs::airToDonorMitigationEnabled()),

     _btn_unload(GUI_BUTTON_JP_PLS_CLAMP_UNLOAD,
                 5,
                 30,
                 textEmpty,
                 BITMAP_BUTTON_GOBACK_TEAL_UP,       // BITMAP_BUTTON_UNLOAD_UP,
                 BITMAP_BUTTON_GOBACK_TEAL_DOWN)     // BITMAP_BUTTON_UNLOAD_DN),

{}    // END of Screen_JP_CLAMP_PLASMA_AIR_BAG CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_JP_CLAMP_PLASMA_AIR_BAG::~Screen_JP_CLAMP_PLASMA_AIR_BAG()
{
   Screen_JP_CLAMP_PLASMA_AIR_BAG::deallocate_resources();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()

void Screen_JP_CLAMP_PLASMA_AIR_BAG::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();

   if (Software_CDS::isJapaneseWorkflow())
   {
      btn_goback.deallocate_resources ();

      _btn_unload.allocate_resources(*this);
      _btn_unload.set_callback(unload_jpplsclamp_cb_btn, (void*)this);
   }


   // refresh cached feature
   _a2donorMitigation = guiglobs::airToDonorMitigationEnabled();

   // goback not needed in the ARM scenario.
   if (_a2donorMitigation)
   {
      btn_goback.deallocate_resources();
   }


   // Now link the status bar/line to this window
   link_status_bar_line();

   _bmpClampPlasmaBagJp.allocate_resources(*this);

   _txtClampPltBagJp.allocate_resources(*this);

   _txtDontConnectAc.allocate_resources(*this);

   // Remove any residual messages
   guiglobs::apheresis_status_line->set_current_text (textEmpty) ;
}    // END of Screen_JP_CLAMP_PLASMA_AIR_BAG ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_JP_CLAMP_PLASMA_AIR_BAG::deallocate_resources ()
{
   // deallocate all constituent objects first
   _bmpClampPlasmaBagJp.deallocate_resources() ;
   _txtClampPltBagJp.deallocate_resources() ;
   _txtDontConnectAc.deallocate_resources() ;

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   if (Software_CDS::isJapaneseWorkflow())
   {
      _btn_unload.deallocate_resources();
      _btn_unload.remove_callbacks();
   }

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}     // END of Screen_JP_CLAMP_PLASMA_AIR_BAG DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_JP_CLAMP_PLASMA_AIR_BAG::process_cancel_button ()
{
   // Call aphwind's default abort confirm messaging
   Base_Apheresis_Window::process_cancel_button();

}     // END of Screen_JP_CLAMP_PLASMA_AIR_BAG PROCESS_CANCEL_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_JP_CLAMP_PLASMA_AIR_BAG::process_continue_button ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   // Tell the procedure that the disposable is clamped, then wait for
   // Procedure to change states to disposable test which will cause
   // the disptest screen to replace this.

   if (!_a2donorMitigation)
   {
      DataLog (log_level_gui_info) << "Sending ClampedForTestingMsg message" << endmsg;
      ClampedForTestingMsg clamped(MessageBase::SEND_GLOBAL);
      clamped.send(0);
   }
   else // ARM is on
   {
      // If air removal is off, clamping is done
      if ( !guiglobs::cdsData.config.Procedure.Get().key_air_removal )
      {
         DataLog (log_level_gui_info) << "AirEvac off: Sending ClampedForTestingMsg message" << endmsg;
         ClampedForTestingMsg clamped(MessageBase::SEND_GLOBAL);
         clamped.send(0);
      }
      else
      {
         DataLog (log_level_gui_info) << "Sending FinishedClampFromARMMsg message" << endmsg;
         FinishedClampFromARMMsg FinishShowClamp(MessageBase::SEND_LOCAL);
         FinishShowClamp.send(0);

         goto_screen(GUI_SCREEN_SYSTEST, "GUI_SCREEN_SYSTEST");
      }
   }

}     // END of Screen_JP_CLAMP_PLASMA_AIR_BAG PROCESS_CONTINUE_BUTTON



////////////////////////////////////////////////////////////////////////////////
void Screen_JP_CLAMP_PLASMA_AIR_BAG::process_unload ()
{

   disable_standard_buttons();

   // DataLog (log_level_gui_info) << "Japanese: Sending  GUIRequestRaiseCassetteMsg message" << endmsg;
   // goto_screen(GUI_SCREEN_SYSCASSETTE, "GUI_SCREEN_SYSCASSETTE");
   //  do this from ProcedureStatus::disposable_raised()


   GUIRequestRaiseCassetteMsg notifyProcToRaise(MessageBase::SEND_LOCAL);
   notifyProcToRaise.send(1);


   // Set the status line to indicate that the disposable is raising
   guiglobs::apheresis_status_line->set_current_status_line_text_by_type
      (ATTENTION_STATUS_LINE, textStatDispRaising);
}

////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON

void Screen_JP_CLAMP_PLASMA_AIR_BAG::process_goback_button ()
{
   goto_screen(GUI_SCREEN_SYSCLAMP, "GUI_SCREEN_SYSCLAMP");
}     // END of Screen_JP_CLAMP_PLASMA_AIR_BAG PROCESS_GOBACK_BUTTON

/* FORMAT HASH e82906eccff07a932150cab02190f669 */

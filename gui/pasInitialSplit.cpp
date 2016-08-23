/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_PAS_INIT_SPLIT

*     File name:   pasInitialSplit.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/pasInitialSplit.cpp 1.5 2009/05/12 15:35:10Z dslausb Exp dslausb $

*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "pasInitialSplit.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "CatalogChecker.h"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "software_cds.h"


// GLOBAL FUNCTIONS FOR CALLBACKS ARE DONE, DUDE.
void unload_passplit_cb_btn (void* data)
{
   ((Screen_PAS_INIT_SPLIT*)data)->process_unload ();
}



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_PAS_INIT_SPLIT::Screen_PAS_INIT_SPLIT()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_PAS_INIT_SPLIT),

     _bmpClampPltBag(400, 145, BITMAP_PLT_CLAMP_BEFORE_SPLIT),
     _bmpClampPltBagJp(400, 145, BITMAP_PLT_CLAMP_BEFORE_SPLIT_JP),
     _txtClampPltBag(textMiscClampPltBagForSplit),
     _txtClampPltBagJp(textMiscClampPltBagJp),
     _txtDontConnectAc(textMiscSysDispDontConnectAc),

     _btn_unload(GUI_BUTTON_PLTSPLIT_UNLOAD,
                 5,
                 30,
                 textEmpty,
                 BITMAP_BUTTON_GOBACK_TEAL_UP,       // BITMAP_BUTTON_UNLOAD_UP,
                 BITMAP_BUTTON_GOBACK_TEAL_DOWN),    // BITMAP_BUTTON_UNLOAD_DN),



     _a2donorMitigation(guiglobs::airToDonorMitigationEnabled())
{}    // END of Screen_PAS_INIT_SPLIT CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_PAS_INIT_SPLIT::~Screen_PAS_INIT_SPLIT()
{
   Screen_PAS_INIT_SPLIT::deallocate_resources();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()

void Screen_PAS_INIT_SPLIT::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();

   // refresh cached feature
   _a2donorMitigation = guiglobs::airToDonorMitigationEnabled();

   const bool Japanese = Software_CDS::isJapaneseWorkflow();

   // goback not needed in the ARM scenario.
   if (_a2donorMitigation || Japanese)
   {
      btn_goback.deallocate_resources();
      _btn_unload.allocate_resources(*this);
      _btn_unload.set_callback(unload_passplit_cb_btn, (void*)this);
   }


   // Now link the status bar/line to this window
   link_status_bar_line();

   chooseTextAndGraphic(Japanese);

   // Remove any residual messages
   guiglobs::apheresis_status_line->set_current_text (textEmpty) ;
}    // END of Screen_PAS_INIT_SPLIT ALLOCATE_RESOURCES


void Screen_PAS_INIT_SPLIT::chooseTextAndGraphic (bool jpSampleSetup)
{
   if (jpSampleSetup)
   {
      _bmpClampPltBag.deallocate_resources();
      _bmpClampPltBagJp.allocate_resources(*this);

      _txtClampPltBag.deallocate_resources();
      _txtClampPltBagJp.allocate_resources(*this);

      _txtDontConnectAc.allocate_resources(*this);
   }
   else
   {
      _bmpClampPltBagJp.deallocate_resources();
      _bmpClampPltBag.allocate_resources(*this);

      _txtClampPltBagJp.deallocate_resources();
      _txtClampPltBag.allocate_resources(*this);

      _txtDontConnectAc.deallocate_resources();
   }
}

////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_PAS_INIT_SPLIT::deallocate_resources ()
{
   // deallocate all constituent objects first
   _bmpClampPltBag.deallocate_resources() ;
   _bmpClampPltBagJp.deallocate_resources() ;
   _txtClampPltBag.deallocate_resources() ;
   _txtClampPltBagJp.deallocate_resources() ;
   _txtDontConnectAc.deallocate_resources() ;

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   if (_a2donorMitigation || Software_CDS::isJapaneseWorkflow())
   {
      _btn_unload.deallocate_resources();
      _btn_unload.remove_callbacks();
   }

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}     // END of Screen_PAS_INIT_SPLIT DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_PAS_INIT_SPLIT::process_cancel_button ()
{
   // Call aphwind's default abort confirm messaging
   Base_Apheresis_Window::process_cancel_button();

}     // END of Screen_PAS_INIT_SPLIT PROCESS_CANCEL_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_PAS_INIT_SPLIT::process_continue_button ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   // Tell the procedure that the disposable is clamped, then wait for
   // Procedure to change states to disposable test which will cause
   // the disptest screen to replace this.

   if (!_a2donorMitigation)
   {
      DataLog (log_level_gui_info) << "ARM off: Sending ClampedForTestingMsg message" << endmsg;
      ClampedForTestingMsg clamped(MessageBase::SEND_GLOBAL);
      clamped.send(0);
   }
   else // ARM is ON
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
         DataLog (log_level_gui_info) << "ARM on: Sending FinishedClampFromARMMsg message" << endmsg;
         FinishedClampFromARMMsg FinishShowClamp(MessageBase::SEND_LOCAL);
         FinishShowClamp.send(0);

         goto_screen(GUI_SCREEN_SYSTEST, "GUI_SCREEN_SYSTEST");
      }
   }

}     // END of Screen_PAS_INIT_SPLIT PROCESS_CONTINUE_BUTTON

////////////////////////////////////////////////////////////////////////////////

void Screen_PAS_INIT_SPLIT::process_goback_button ()
{
   goto_screen(GUI_SCREEN_SYSCLAMP, "GUI_SCREEN_SYSCLAMP");
}     // END of Screen_PAS_INIT_SPLIT PROCESS_GOBACK_BUTTON


////////////////////////////////////////////////////////////////////////////////
void Screen_PAS_INIT_SPLIT::process_unload ()
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


#if (CPU==SIMNT)

void Screen_PAS_INIT_SPLIT::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      PAS_INIT_SPLIT_DISPLAY_TYPE pasInitSplitDisplayType = (PAS_INIT_SPLIT_DISPLAY_TYPE)traverseInfo.parameters[0];
      DataLog (log_level_sim_gui_side_info) << "PAS init split screen got display type " << pasInitSplitDisplayType << endmsg;
      chooseTextAndGraphic(pasInitSplitDisplayType == PAS_INIT_SPLIT_JP);
   }
   // If we didn't get one of our messages, call the super's version to see if it can handle it.
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif // #if (CPU==SIMNT)

/* FORMAT HASH f7efbcec9fd1ad69b71eb20f3d52627e */

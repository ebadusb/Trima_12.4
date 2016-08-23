/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_JP_CLAMP_BLOOD_DIVERSION

*     File name:   jpClampBloodDiversion.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/jpClampBloodDiversion.cpp 1.5 2009/05/14 16:44:24Z dslausb Exp dslausb $

*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "jpClampBloodDiversion.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"

// EXTERNAL REFERENCES

#include "guiglobs.hpp"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_JP_CLAMP_BLOOD_DIVERSION::Screen_JP_CLAMP_BLOOD_DIVERSION()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_JP_CLAMP_BLOOD_DIVERSION),

     _bmpClampBloodDiversion(50, 150, BITMAP_JP_CLAMP_DIVERSION),

     _txtClampBloodDiversion(textMiscClampBloodDiversionJp),

     _txtDontConnectAc(textMiscSysDispDontConnectAc)
{}     // END of Screen_JP_CLAMP_BLOOD_DIVERSION CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_JP_CLAMP_BLOOD_DIVERSION::~Screen_JP_CLAMP_BLOOD_DIVERSION()
{
   Screen_JP_CLAMP_BLOOD_DIVERSION::deallocate_resources();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()

void Screen_JP_CLAMP_BLOOD_DIVERSION::allocate_resources (const char* allocation_parameter)
{
     #if (CPU!=SIMNT)

   if (!(Software_CDS::isJapaneseWorkflow() &&
         guiglobs::cdsData.config.Procedure.Get().key_blood_diversion))
   {
      DataLog (log_level_gui_info) << "Japanese blood diversion screen skipped." << endmsg;
      goto_screen(GUI_SCREEN_SYSACATT, "GUI_SCREEN_SYSACATT");
      return;
   }

     #endif // #if (CPU!=SIMNT)

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources();

   // Now link the status bar/line to this window
   link_status_bar_line();

   _bmpClampBloodDiversion.allocate_resources(*this);
   _txtClampBloodDiversion.allocate_resources(*this);
   _txtDontConnectAc.allocate_resources(*this);

   // Remove any residual messages
   guiglobs::apheresis_status_line->set_current_text (textEmpty) ;
}    // END of Screen_JP_CLAMP_BLOOD_DIVERSION ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_JP_CLAMP_BLOOD_DIVERSION::deallocate_resources ()
{
   // deallocate all constituent objects first
   _bmpClampBloodDiversion.deallocate_resources();
   _txtClampBloodDiversion.deallocate_resources() ;
   _txtDontConnectAc.deallocate_resources();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}     // END of Screen_JP_CLAMP_BLOOD_DIVERSION DEALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_JP_CLAMP_BLOOD_DIVERSION::process_continue_button ()
{
   goto_screen(GUI_SCREEN_SYSACATT, "GUI_SCREEN_SYSACATT");
}     // END of Screen_JP_CLAMP_BLOOD_DIVERSION PROCESS_CONTINUE_BUTTON


#if (CPU==SIMNT)

void Screen_JP_CLAMP_BLOOD_DIVERSION::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   /* This is going to take some work.
   if(traverseInfo.type == UNIVERSAL)
   {
       JP_CLAMP_BLOOD_DIVERSION_DISPLAY_TYPE sysClampDisplayType = (JP_CLAMP_BLOOD_DIVERSION_DISPLAY_TYPE)traverseInfo.parameters[0];
       chooseGraphic( sysClampDisplayType == JP_CLAMP_BLOOD_DIVERSION_AIR_REMOVAL_ON );
   }
   else */
   // If we didn't get one of our messages, call the super's version to see if it can handle it.
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif // #if (CPU==SIMNT)

/* FORMAT HASH 2302fd3ac25c53a032924eddc2929a4d */

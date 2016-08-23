/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1999 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_LABELPRD

*     File name:   labelprd.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/labelprd.cpp 1.18 2008/07/10 20:30:04Z dslausb Exp $
$Log: labelprd.cpp $

*/

#include <vxworks.h>

// SYSTEM INCLUDE
#include <time.h>

// BASE CLASS INCLUDE

#include "labelprd.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "frmtdec.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"

#define BITMAP_TEXTBOX_X 122
#define BITMAP_TEXTBOX_Y 142
#define BITMAP_Y_DELTA 90


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_LABELPRD::Screen_LABELPRD()
   : Base_Apheresis_Window(MAGENTA, GUI_SCREEN_LABELPRD),
     _bitmapPltTextBox  (BITMAP_TEXTBOX_X,
                         BITMAP_TEXTBOX_Y,
                         BITMAP_CARTOUCHE_LABEL_PRODUCT),
     _bitmapPlzTextBox  (BITMAP_TEXTBOX_X,
                         BITMAP_TEXTBOX_Y + BITMAP_Y_DELTA,
                         BITMAP_CARTOUCHE_LABEL_PRODUCT),
     _textHeading    (TEXT_RS2LAB_LEUKOREDUCED),
     _textLblPltBody (textProdVerifLeukMsgPltLeukoreduced),
     _textLblPlzBody (textProdVerifLeukMsgPlasLeukoreduced),

     _msgPtrReceiveRunStats(NULL)
{}   // END of Screen_LABELPRD CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_LABELPRD::~Screen_LABELPRD()
{
   // If we haven't already deallocated, do it now.
   deallocate_resources ();
}   // END of Screen_LABELPRD DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_LABELPRD::allocate_resources (const char* allocation_parameter)
{

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   //  layer allocation from Z-back to Z-front.

   _bitmapPltTextBox.allocate_resources (*this) ;
   _bitmapPlzTextBox.allocate_resources (*this) ;

   // the display label objects
   _textHeading.allocate_resources (*this) ;

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_cancel.deallocate_resources ();
   btn_goback.deallocate_resources ();
   btn_continue.deallocate_resources ();  // until stats received


   // Change the text on the continue button to "next page"
   btn_continue.set_string_id (textButtonLongNextPage);


   // Now link the status bar/line to this window
   link_status_bar_line();

   // Disable all the standard buttons on this screen - they'll be reactivated
   //  when the run stats message comes in.
   disable_standard_buttons();

   // Activate the receive message object to receive the LABELPRD data
   _msgPtrReceiveRunStats = new Message<RUN_SUMMARY_STRUCT>(Callback<Screen_LABELPRD>(this, &Screen_LABELPRD::process_labelprd_data), MessageBase::SNDRCV_RECEIVE_ONLY);

   // Send a message to request the LABELPRD data
   RequestEndRunStatsMsg request(MessageBase::SEND_GLOBAL);
   request.send(0);

}  // END of Screen_LABELPRD ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_LABELPRD::deallocate_resources ()
{

   // deallocate all constituent objects first

   // the LABELPRD data panel
   _bitmapPltTextBox.deallocate_resources() ;
   _bitmapPlzTextBox.deallocate_resources() ;

   // the display label objects
   _textHeading.deallocate_resources() ;
   _textLblPltBody.deallocate_resources() ;
   _textLblPlzBody.deallocate_resources() ;

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // Deallocate the LABELPRD receive message object
   if (_msgPtrReceiveRunStats != NULL)
   {
      delete _msgPtrReceiveRunStats ;
      _msgPtrReceiveRunStats = NULL ;
   }

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();


}   // END of Screen_LABELPRD DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_LABELPRD::process_continue_button ()
{

   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   // If we have any verification flags on, go to the verif product screen
   if (_PQA.AtLeastOneFlag())
   {
      // Display the verifprd screen
      goto_screen (GUI_SCREEN_VERIFPRD, "GUI_SCREEN_VERIFPRD");
   }
   else    // Display the run summary 2: product summary screen
   {
      // Display the runsumm2 screen
      goto_screen (GUI_SCREEN_RUNSUMM2, "GUI_SCREEN_RUNSUMM2");
   }

}   // END of Screen_LABELPRD PROCESS_CONTINUE_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_LABELPRD_DATA
void Screen_LABELPRD::process_labelprd_data ()
{
   // This is invoked when the LABELPRD data message is received

   if (btn_continue.allocated()) return;

   const RUN_SUMMARY_STRUCT& run_summary = _msgPtrReceiveRunStats->getData();

   // Enable the continue button et al.
   enable_standard_buttons();

   //
   //
   //   Allocate text strings for label messages if the appropriate flag is set.
   //
   if (_PQA.LabelPlatelets())
   {
      _textLblPltBody.allocate_resources (*this) ;
      DataLog(log_level_gui_info) << "Label platelet product as leukoreduced." << endmsg;
   }
   else
   {
      DataLog(log_level_gui_info) << "DO NOT Label platelet product as leukoreduced." << endmsg;
   }

   if (_PQA.LabelPlasma())
   {
      _textLblPlzBody.allocate_resources (*this) ;
      DataLog(log_level_gui_info) << "Label plasma product as leukoreduced." << endmsg;
   }
   else
   {
      DataLog(log_level_gui_info) << "DO NOT Label plasma product as leukoreduced." << endmsg;
   }

   // temp patch to make text on top of cartouche label product bitmap
   _bitmapPltTextBox.move_to_back();
   _bitmapPltTextBox.move_to_front();
   _bitmapPlzTextBox.move_to_back();
   _bitmapPlzTextBox.move_to_front();

   // need the continue button to let say ..... continue.
   btn_continue.allocate_resources(*this);

}   // END of Screen_LABELPRD PROCESS_LABELPRD_DATA

/* FORMAT HASH 5f46f7a63e486103794fb3e6156732a8 */

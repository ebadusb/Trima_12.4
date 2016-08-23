/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_POWERUP

*     File name:   powerup.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "powerup.hpp"


// EXTERNAL REFERENCES

#include "trima_info.h"
#include "software_cds.h"



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_POWERUP::Screen_POWERUP()
   : Base_Window(BITMAP_NULL),
     bitmap_placard_powerup (PLAC_POWERUP_X,
                             PLAC_POWERUP_Y,
                             BITMAP_PLACARD_POWERUP),

// text display fields
     text_product_exposition (textMiscSplashProductDescription),
     text_version            (textMiscTrimaVersion),
     text_rights             (textMiscSplashCopyright)
{
   set_screen_id(GUI_SCREEN_POWERUP);
}


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_POWERUP::~Screen_POWERUP()
{
   Screen_POWERUP::deallocate_resources();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_POWERUP::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Window::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   //  layer allocation from Z-back to Z-front.
   bitmap_placard_powerup.allocate_resources(*this);

   // Allocate all the text fields on top of the splash screen bitmap:
   //  1) the product description
   //  2) the product version
   //  3) the product rights
   text_product_exposition.allocate_resources (*this);
   text_version.allocate_resources            (*this);
   text_rights.allocate_resources             (*this);

   // Extract the Version ID from the text object -- only allow 1/5 of the concat string to be used
   guistring text_version_string;
   text_version.get_text_direct (text_version_string);

   if (TrimaInfo::isClinicalBuild())
      strcat (text_version_string, "  Clinical ");
   else
      strcat (text_version_string, " "); // Just add a space so that the build ID isn't butted up against the version number.

   if (TrimaInfo::isClinicalBuild() || TrimaInfo::isTestBuild())
   {
      strcat (text_version_string, TrimaInfo::buildNumber());
   }

   if (TrimaInfo::isTestBuild())
   {
      // Build the string with the Trima, Safety CRC's and the build number
      strcat (text_version_string, "  C=");
      strcat (text_version_string, TrimaInfo::trimaCrc());
      strcat (text_version_string, " Datalog=");
      strcat (text_version_string, TrimaInfo::logFileNameFull());

      // Lower the font size of the version text object so that it won't overwrite the copywrite
      text_version.set_font_size(12);
   }

   // Put the Version ID plus the various CRC's and build ID back into the version object
   text_version.set_text_direct(text_version_string.length(), text_version_string);

   // allocate and activate the notify object for incoming messages
   guiAliveMsg.init(Callback<Screen_POWERUP>(this, &Screen_POWERUP::screen_notify_handler), MessageBase::SNDRCV_RECEIVE_ONLY);

}  // END of Screen_POWERUP ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_POWERUP::deallocate_resources ()
{
   // deallocate all constituent objects first
   // text objects
   text_product_exposition.deallocate_resources ();
   text_version.deallocate_resources ();
   text_rights.deallocate_resources  ();

   // bitmap objects
   bitmap_placard_powerup.deallocate_resources();

   //  stop the message response
   guiAliveMsg.deregisterMsg();

   // deallocate parent's resources
   Base_Window::deallocate_resources ();
}   // END of Screen_POWERUP DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER
void Screen_POWERUP::screen_notify_handler ()
{
   // Respond to the Procedure with an "I'm Alive" message
   AnswerGUIAliveMsg response(MessageBase::SEND_LOCAL);
   response.send(0);

   //  stop the message response
   guiAliveMsg.deregisterMsg();

   taskPrioritySet(taskIdSelf(), 90);
}   // END of Screen_POWERUP SCREEN_NOTIFY_HANDLER


void Screen_POWERUP::draw (UGL_GC_ID gc)
{
   DataLog (log_level_gui_info) << "Drawing the powerup screen." << endmsg;

   OSWindow::draw(gc);

   // If screenshots are enabled by features.bin,
   // and the software is clinical or test, snap a screenshot of the powerup
   // screen.
   if ( Software_CDS::GetInstance().getFeature(EnableScreenshotButton) &&
        (TrimaInfo::isClinicalBuild() || TrimaInfo::isTestBuild())
        )
   {
      DataLog (log_level_gui_info) << "Snapping splash screen" << endmsg;
      guistring filename("splashScreen");
      screenshot(filename);
   }
}

/* FORMAT HASH dbb71c7b912190037d4c4c66318265a4 */

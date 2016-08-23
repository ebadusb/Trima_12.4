/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_CASSETTE_DELETE_CONFIRM_BOX

*     File name:   cassetteDeleteConfirmBox.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.6  2009/04/07 20:30:38Z  dslausb
IT 9101 - Use GUI_POOL_MALLOC where possible
Revision 1.5  2009/03/20 15:26:01Z  dslausb
- Transparent bitmaps
- Stand-alone graphics packages
- String.info CRCs
- Font mapping for Asian languages
Revision 1.4  2008/07/14 20:25:25Z  dslausb
More stringId changes requested by localization
Revision 1.3  2008/07/09 23:49:29Z  dslausb
More string ID standardization as requested by Marita
Revision 1.2  2008/07/07 18:24:07Z  dslausb
More localization string id standardization
Revision 1.1  2007/10/18 20:34:10Z  dslausb
Initial revision

*/

#include <vxworks.h>

#include "cassetteDeleteConfirmBox.hpp"
#include "guiglobs.hpp"
#include "trimamessages.h"
#include "cfg_look.h"

const char* const STRING_TOKEN      = "~";
static const int  HORIZONTAL_MARGIN = 20;
static const int  VERTICAL_MARGIN   = 20;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_CASSETTE_DELETE_CONFIRM_BOX::Screen_CASSETTE_DELETE_CONFIRM_BOX()
   : ConfirmBoxBase(),

     _confirmText(textEmpty)
{
   set_screen_id(GUI_SCREEN_CONFIRM_BOX);
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CASSETTE_DELETE_CONFIRM_BOX::~Screen_CASSETTE_DELETE_CONFIRM_BOX()
{
   Screen_CASSETTE_DELETE_CONFIRM_BOX::deallocate_resources ();

}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_CASSETTE_DELETE_CONFIRM_BOX::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   OSRegion       regionVar;
   TEXT_STRING_ID textId;
   bool           showCassetteId;

   int            window_xPos = 0,
                  window_yPos = 0;

   guistring        allocstr(allocation_parameter);
   char*            tokstr    = allocstr.make_buffer();

   char*            nextToken = 0;

   CONFIG_DATA_KEYS dataKey   = (CONFIG_DATA_KEYS)atoi(strtok_r(tokstr, STRING_TOKEN, &nextToken));
   window_xPos = atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));
   window_yPos = atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));

   GUI_POOL_FREE(tokstr);
   DataLog (log_level_gui_info) << "Parsed: (x=" << window_xPos
                                << "; y=" << window_yPos
                                << "; dataKey=" << _dataKey
                                << endmsg;

   setConfirmBoxData(dataKey,
                     BITMAP_CONFIRM_BOX_TEAL,
                     window_xPos, window_yPos,
                     CFG_Look::BUTTON_UP_NAME,
                     CFG_Look::BUTTON_DOWN_NAME,
                     20, 23, 30);

   ConfirmBoxBase::allocate_resources (allocation_parameter);

   switch (dataKey)
   {
      case KEY_MACH_CONFIRM_DELETE_ONE :
         showCassetteId = true;
         textId         = textConfirmBodyCfgMachRefDeleteOne;
         break;

      case KEY_MACH_CONFIRM_DELETE_ALL :
         showCassetteId = false;
         textId         = textConfirmBodyCfgMachRefDeleteAll;
         break;

      default :
         DataLog (log_level_gui_error) << "Unrecognized data key: " << dataKey << ". Unable to proceed." << endmsg;
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();
         return;
   }

   // Figure out the dimensions of the text box.
   get_region(regionVar);
   // Start at the margins
   regionVar.x       = HORIZONTAL_MARGIN;
   regionVar.y       = VERTICAL_MARGIN;
   // Width is window width minus the margins and button area.
   regionVar.width  -= ( 2 * HORIZONTAL_MARGIN);
   regionVar.height -= ( getBtnHeight() + ( 3 * VERTICAL_MARGIN));

   _confirmText.set_string_id(textId);
   _confirmText.set_alignment(CENTER);
   _confirmText.set_requested_region(regionVar);

   // Add extra input string to current string, if necessary.
   if (showCassetteId)
   {
      guistring confirmTextString;
      _confirmText.get_text_direct(confirmTextString);

      confirmTextString += "\n\n";
      confirmTextString += guiglobs::cassette_mgr->getCassetteToDelete();

      _confirmText.set_text_direct(confirmTextString.length(), confirmTextString);
   }

   _confirmText.allocate_resources(*this);
}  // END of Screen_CASSETTE_DELETE_CONFIRM_BOX ALLOCATE_RESOURCES


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CASSETTE_DELETE_CONFIRM_BOX::deallocate_resources ()
{
   if (!_allocated) return;

   _confirmText.deallocate_resources();

   ConfirmBoxBase::deallocate_resources ();
}   // END of Screen_CASSETTE_DELETE_CONFIRM_BOX DEALLOCATE_RESOURCES



guistring Screen_CASSETTE_DELETE_CONFIRM_BOX::createAllocationString (CONFIG_DATA_KEYS dataKey,
                                                                      int xPos,
                                                                      int yPos)
{
   guistring allocationString;
   allocationString.initialize();

   sprintf(allocationString, "%d~%d~%d",
           dataKey,
           xPos,
           yPos);

   return allocationString;
}

/* FORMAT HASH cbcb217e53893954bbe443a5598bce46 */

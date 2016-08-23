/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plas_listbox.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See header file
 *
 ******************************************************************************
 *
 *	$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/plas_listbox.cpp 1.7 2008/07/14 20:28:28Z dslausb Exp dslausb $
 *
 *	$Log: plas_listbox.cpp $
 *	Revision 1.7  2008/07/14 20:28:28Z  dslausb
 *	More stringId changes requested by localization
 *	Revision 1.6  2008/07/10 19:02:23Z  dslausb
 *	Last tweaks on string ID standardization
 *	Revision 1.5  2008/07/09 23:51:33Z  dslausb
 *	More string ID standardization as requested by Marita
 *	Revision 1.4  2008/07/07 18:26:42Z  dslausb
 *	More localization string id standardization
 *	Revision 1.3  2006/06/29 16:10:36Z  dslausb
 *	IT 7614 - Cleaned up procedure priority screen (took off yield, volume, and dose labels)
 *	Revision 1.2  2006/06/14 20:28:18Z  dslausb
 *	IT 7503 - Fixed slight formatting issues with new button info.
 *	Revision 1.1  2006/06/13 17:39:57Z  dslausb
 *	Initial revision
 *
 ******************************************************************************/

#include "plas_listbox.hpp"
#include "guiglobs.hpp"
#include "cfg_look.h"
#include "screen_config.hpp"

Screen_PLAS_LISTBOX::Screen_PLAS_LISTBOX()
   : Screen_EXTENDED_LISTBOX(GUI_SCREEN_PLAS_LISTBOX)
{
   for (int i = 0; i < MAX_BUTTONS_IN_LIST + 1; i++)
   {
      _txtVolValue[i].set_string_id(textEmptyFormatPlasListboxVolInit);
      _txtVolUnits[i].set_string_id(textButtonCfgPrioExtraPlasUnits);
   }
}

void Screen_PLAS_LISTBOX::deallocate_resources ()
{
   for (int i = 0; i < MAX_BUTTONS_IN_LIST + 1; i++)
   {
      _txtVolValue[i].deallocate_resources();
      _txtVolUnits[i].deallocate_resources();
   }

   Screen_EXTENDED_LISTBOX::deallocate_resources();
}

void Screen_PLAS_LISTBOX::addBtnExtras ()
{
   char volString[80];
   int  stringLen = 0;

   for (int btn_index = 0; btn_index < MAX_BUTTONS_IN_LIST + 1; btn_index++)
   {
      float volVal = -1;

      int   index  = config_list_table[button_array[btn_index]].txtid - textListboxItemCfgPlsProc1;

      if (index >= 0 && index < 6)
      {
         volVal    = guiglobs::cdsData.rwConfig.templates.key_pls_volume[index];
         stringLen = sprintf(volString, "%.0f", volVal);

         OSRegion textArea;
            #define __PLAS_LISTBOX_TXT_OFFSET__ 2

         textArea.x      = 0;
         textArea.y      = ((p_buttons[btn_index]->height() * 2) / 3);
         textArea.width  = (p_buttons[btn_index]->width() / 2) + __PLAS_LISTBOX_TXT_OFFSET__;
         textArea.height = p_buttons[btn_index]->height() / 3;

         _txtVolValue[btn_index].set_text_direct(stringLen, volString);
         _txtVolValue[btn_index].set_requested_region(textArea);
         _txtVolValue[btn_index].allocate_resources(*p_buttons[btn_index]);

         textArea.x = ((p_buttons[btn_index]->width() / 2) + __PLAS_LISTBOX_TXT_OFFSET__ + 2);

         _txtVolUnits[btn_index].set_requested_region(textArea);
         _txtVolUnits[btn_index].allocate_resources(*p_buttons[btn_index]);
      }
      else
      {
         _txtVolValue[btn_index].deallocate_resources();
         _txtVolUnits[btn_index].deallocate_resources();
      }
   }
}

/* FORMAT HASH 132606638450f91cf7d6ebb4c6347e80 */

/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rbc_listbox.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See header file
 *
 ******************************************************************************
 *
 *	$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/rbc_listbox.cpp 1.9 2008/07/14 20:28:47Z dslausb Exp dslausb $
 *
 *	$Log: rbc_listbox.cpp $
 *	Revision 1.9  2008/07/14 20:28:47Z  dslausb
 *	More stringId changes requested by localization
 *	Revision 1.8  2008/07/10 19:02:42Z  dslausb
 *	Last tweaks on string ID standardization
 *	Revision 1.7  2008/07/09 23:52:19Z  dslausb
 *	More string ID standardization as requested by Marita
 *	Revision 1.6  2008/07/07 18:27:06Z  dslausb
 *	More localization string id standardization
 *	Revision 1.5  2008/03/14 20:53:03Z  dslausb
 *	Fixed case statement that was depending on text string IDs.
 *	Revision 1.4  2008/02/18 21:09:04Z  dslausb
 *	Clear up uninitialized variable warning in I80486 compiler
 *	Revision 1.3  2008/02/18 19:54:02Z  mereusb
 *	Added new icon for RAS in procedure priority screen.
 *	Revision 1.2  2006/06/29 10:10:59  dslausb
 *	IT 7614 - Cleaned up procedure priority screen (took off yield, volume, and dose labels)
 *	Revision 1.1  2006/06/13 17:39:51Z  dslausb
 *	Initial revision
 *
 ******************************************************************************/

#include "rbc_listbox.hpp"
#include "guiglobs.hpp"
#include "cfg_look.h"
#include "screen_config.hpp"

Screen_RBC_LISTBOX::Screen_RBC_LISTBOX()
   : Screen_EXTENDED_LISTBOX(GUI_SCREEN_RBC_LISTBOX)
{
   DataLog (log_level_gui_info) << "Entering Screen_RBC_LISTBOX constructor." << endmsg;

   for (int i = 0; i < MAX_BUTTONS_IN_LIST + 1; i++)
   {
      _txtDoseValue[i].set_string_id(textEmptyFormatRbcListboxDoseInit);
      _txtDoseUnits[i].set_string_id(textButtonCfgPrioExtraDoseUnits);
   }
}

void Screen_RBC_LISTBOX::deallocate_resources ()
{
   for (int i = 0; i < MAX_BUTTONS_IN_LIST + 1; i++)
   {
      _txtDoseValue[i].deallocate_resources();
      _txtDoseUnits[i].deallocate_resources();
   }

   Screen_EXTENDED_LISTBOX::deallocate_resources();
}

void Screen_RBC_LISTBOX::addBtnExtras ()
{
   DataLog (log_level_gui_info) << "Entering Screen_RBC_LISTBOX::addBtnExtras()." << endmsg;
   char doseString[80];
   int  stringLen = 0;

   for (int btn_index = 0; btn_index < MAX_BUTTONS_IN_LIST + 1; btn_index++)
   {
      bool showDose       = true;

      bool masterPtfOn    = guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on;
      bool procedurePtfOn = false;

      switch (config_list_table[button_array[btn_index]].txtid)
      {
         case textListboxItemCfgRbcProc1 :
            stringLen      = sprintf(doseString, "%.0f", guiglobs::cdsData.rwConfig.templates.key_rbc_dose[0]);
            procedurePtfOn = masterPtfOn && guiglobs::cdsData.rwConfig.templates.key_rbc_mss[0];
            break;

         case textListboxItemCfgRbcProc2 :
            stringLen      = sprintf(doseString, "%.0f", guiglobs::cdsData.rwConfig.templates.key_rbc_dose[1]);
            procedurePtfOn = masterPtfOn && guiglobs::cdsData.rwConfig.templates.key_rbc_mss[1];
            break;

         case textListboxItemCfgRbcProc3 :
            stringLen      = sprintf(doseString, "%.0f", guiglobs::cdsData.rwConfig.templates.key_rbc_dose[2]);
            procedurePtfOn = masterPtfOn && guiglobs::cdsData.rwConfig.templates.key_rbc_mss[2];
            break;
         case textListboxItemCfgRbcProc4 :
            stringLen      = sprintf(doseString, "%.0f", guiglobs::cdsData.rwConfig.templates.key_rbc_dose[3]);
            procedurePtfOn = masterPtfOn && guiglobs::cdsData.rwConfig.templates.key_rbc_mss[3];
            break;

         case textListboxItemCfgRbcProc5 :
            stringLen      = sprintf(doseString, "%.0f", guiglobs::cdsData.rwConfig.templates.key_rbc_dose[4]);
            procedurePtfOn = masterPtfOn && guiglobs::cdsData.rwConfig.templates.key_rbc_mss[4];
            break;

         case textListboxItemCfgRbcProc6 :
            stringLen      = sprintf(doseString, "%.0f", guiglobs::cdsData.rwConfig.templates.key_rbc_dose[5]);
            procedurePtfOn = masterPtfOn && guiglobs::cdsData.rwConfig.templates.key_rbc_mss[5];
            break;

         default :
            DataLog (log_level_gui_info) << "No button extras necessary (StringID=" << config_list_table[button_array[btn_index]].txtid << ");" << endmsg;
            showDose = false;
            break;
      }

      if (showDose)
      {
         if (procedurePtfOn)
         {
            p_buttons[btn_index]->set_up_bitmap(BITMAP_CONFIG_SUB_UP_RAS);
         }
         else
         {
            p_buttons[btn_index]->set_up_bitmap(CFG_Look::BUTTON_UP_NAME);
         }


         OSRegion textArea;
            #define __RBC_LISTBOX_TXT_OFFSET__ 2

         textArea.x      = 0;
         textArea.y      = ((p_buttons[btn_index]->height() * 2) / 3);
         textArea.width  = (p_buttons[btn_index]->width() / 2) + __RBC_LISTBOX_TXT_OFFSET__;
         textArea.height = p_buttons[btn_index]->height() / 3;

         _txtDoseValue[btn_index].set_text_direct(stringLen, doseString);
         _txtDoseValue[btn_index].set_requested_region(textArea);
         _txtDoseValue[btn_index].allocate_resources(*p_buttons[btn_index]);

         textArea.x = ((p_buttons[btn_index]->width() / 2) + __RBC_LISTBOX_TXT_OFFSET__ + 2);

         _txtDoseUnits[btn_index].set_requested_region(textArea);
         _txtDoseUnits[btn_index].allocate_resources(*p_buttons[btn_index]);
      }
      else
      {
         _txtDoseValue[btn_index].deallocate_resources();
         _txtDoseUnits[btn_index].deallocate_resources();
      }
   }
}

/* FORMAT HASH e42d0a2fe585d4b9c112232ce5a704c7 */

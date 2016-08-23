/*******************************************************************************
 *
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plt_listbox.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See header file
 *
 ******************************************************************************
 *
 *	$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/plt_listbox.cpp 1.17 2009/04/10 16:34:45Z jsylusb Exp dslausb $
 *
 *	$Log: plt_listbox.cpp $
 *	Revision 1.17  2009/04/10 16:34:45Z  jsylusb
 *	Created a new CDS for software.dat settings.
 *	Revision 1.16  2008/10/15 17:05:59Z  dslausb
 *	IT 8841 - Add PAS icon to procedure priority buttons
 *	Revision 1.15  2008/08/28 17:16:43Z  dslausb
 *	Fix inconsistent decimal delimiters
 *	Revision 1.14  2008/07/14 20:28:34Z  dslausb
 *	More stringId changes requested by localization
 *	Revision 1.13  2008/07/10 19:02:31Z  dslausb
 *	Last tweaks on string ID standardization
 *	Revision 1.12  2008/07/09 23:51:39Z  dslausb
 *	More string ID standardization as requested by Marita
 *	Revision 1.11  2008/07/09 16:29:50Z  dslausb
 *	IT 8735 - Fix a procedure bug
 *	Revision 1.10  2008/07/07 18:26:47Z  dslausb
 *	More localization string id standardization
 *	Revision 1.9  2008/01/23 17:27:49Z  dslausb
 *	IT 8435 - VxWorks 5.5 Checkin
 *	Revision 1.8  2006/08/10 18:20:31Z  dslausb
 *	IT 7660 - Hyper icon on plt buttons in procedure priority
 *	Revision 1.7  2006/06/29 16:10:47Z  dslausb
 *	IT 7614 - Cleaned up procedure priority screen (took off yield, volume, and dose labels)
 *	Revision 1.6  2006/06/14 20:28:25Z  dslausb
 *	IT 7503 - Fixed slight formatting issues with new button info.
 *	Revision 1.5  2006/06/09 22:35:06Z  dslausb
 *	Platelet listbox has units with label.
 *	Revision 1.4  2006/06/05 22:49:05Z  dslausb
 *	Removed Fixed Volume
 *	Revision 1.3  2006/05/26 16:29:03Z  dslausb
 *	Initialized variable
 *	Revision 1.2  2006/05/25 22:28:18Z  dslausb
 *	- Made text changes from 5-25 gui review
 *	- Added config option for blood diversion
 *	Revision 1.1  2006/05/23 18:56:34Z  dslausb
 *	Initial revision
 *
 ******************************************************************************/

#include "plt_listbox.hpp"
#include "guiglobs.hpp"
#include "cfg_look.h"
#include "screen_config.hpp"
#include "software_cds.h"

Screen_PLT_LISTBOX::Screen_PLT_LISTBOX()
   : Screen_EXTENDED_LISTBOX(GUI_SCREEN_PLT_LISTBOX)
{
   for (int i = 0; i < MAX_BUTTONS_IN_LIST + 1; i++)
   {
      _txtYieldValue[i].set_string_id(textEmptyFormatPltListboxYieldInit);
      _txtYieldx10[i].set_string_id(textButtonCfgPrioExtraPltX10);
      _txtYieldtoThe11th[i].set_string_id(textButtonCfgPrioExtraPltExponent);
   }
}

void Screen_PLT_LISTBOX::deallocate_resources ()
{
   for (int i = 0; i < MAX_BUTTONS_IN_LIST + 1; i++)
   {
      _txtYieldValue[i].deallocate_resources();
      _txtYieldx10[i].deallocate_resources();
      _txtYieldtoThe11th[i].deallocate_resources();
   }

   Screen_EXTENDED_LISTBOX::deallocate_resources();
}

void Screen_PLT_LISTBOX::addBtnExtras ()
{

   for (int btn_index = 0; btn_index < MAX_BUTTONS_IN_LIST + 1; btn_index++)
   {
      int index = config_list_table[button_array[btn_index]].txtid - textListboxItemCfgPltProc1;

      DataLog(log_level_gui_debug) << "key_name: " << config_list_table[button_array[btn_index]].key_name << " list_name: " << config_list_table[button_array[btn_index]].list_name << " txtid: " << config_list_table[button_array[btn_index]].txtid << endmsg;

      if (index >= 0 && index < NUM_YIELD_VOLUME)
      {
         float yieldVal    = guiglobs::cdsData.rwConfig.templates.key_plt_yield[index];
         float collectVol  = guiglobs::cdsData.rwConfig.templates.key_plt_volume[index];
         float collectConc = (yieldVal * 1.0E5f) / (collectVol);

         bool  isPas       = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on &&
                             guiglobs::cdsData.rwConfig.templates.key_plt_mss[index];

         guistring yieldString;
         int       stringLen = sprintf_l10n(yieldString, "%.1f", yieldVal);

         BITMAP_ID pltBmpId  = CFG_Look::BUTTON_UP_NAME;

         if (isPas) pltBmpId = BITMAP_CONFIG_SUB_UP_PAS;
         else if (Software_CDS::isPPC(collectConc))
            pltBmpId = BITMAP_CONFIG_SUB_UP_HYPER;

         p_buttons[btn_index]->set_up_bitmap(pltBmpId);

         OSRegion textArea;
            #define __PLT_LISTBOX_TXT_OFFSET__ 6

         textArea.x      = 0;
         textArea.y      = ((p_buttons[btn_index]->height() * 2) / 3);
         textArea.width  = (p_buttons[btn_index]->width() / 2) - __PLT_LISTBOX_TXT_OFFSET__;
         textArea.height = p_buttons[btn_index]->height() / 3;

         _txtYieldValue[btn_index].set_text_direct(stringLen, yieldString);
         _txtYieldValue[btn_index].set_requested_region(textArea);
         _txtYieldValue[btn_index].allocate_resources(*p_buttons[btn_index]);

         textArea.x = ((p_buttons[btn_index]->width() / 2) - __PLT_LISTBOX_TXT_OFFSET__ + 3);

         _txtYieldx10[btn_index].set_requested_region(textArea);
         _txtYieldx10[btn_index].allocate_resources(*p_buttons[btn_index]);
         _txtYieldtoThe11th[btn_index].allocate_resources(*p_buttons[btn_index]);
      }
      else
      {
         _txtYieldValue[btn_index].deallocate_resources();
         _txtYieldx10[btn_index].deallocate_resources();
         _txtYieldtoThe11th[btn_index].deallocate_resources();

         if (config_list_table[button_array[btn_index]].key_name == KEY_PRI_PLATELET_AMAP_PAS_LIST &&
             config_list_table[button_array[btn_index]].list_name > PLATELET_3 )
         {
            p_buttons[btn_index]->set_up_bitmap(BITMAP_CONFIG_SUB_UP_PAS);
         }
         else
            p_buttons[btn_index]->set_up_bitmap(CFG_Look::BUTTON_UP_NAME);
      }
   }
}

/* FORMAT HASH 0032072dca182644bd805c25e0c6150e */

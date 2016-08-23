/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_DONOR

*     File name:   cfg_dono.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/cfg_dono.hpp 1.13 2008/04/08 17:02:13Z dslausb Exp dslausb $
*/

// Single include define guardian
#ifndef SYSCONFIGDONO_HPP
#define SYSCONFIGDONO_HPP

/** include files **/

// CLASS INCLUDE
#include "guiglobs.hpp"
#include "cfg_butt.hpp"
#include "screen_config.hpp"

//
///////////////////////////////////////////////////////////////////
//
class Screen_CONFIG_DONOR
   : public Screen_Config
{

protected:

   // The display text objects

   Config_button_with_text config_btn_ac_rate ;
   Config_button_with_text config_btn_post_crit ;
   Config_button_with_text config_btn_post_plat ;
   Config_button_with_text config_btn_max_draw_flow;
   Config_button_with_text config_btn_inlet_flow_ramp;
   Config_button_with_text config_btn_inlet_management;
   Config_button_with_text config_btn_return_management;
   ConfigButton            config_btn_autoflow;
   Display_Text            config_lbl_autoflow;

   const CCfgVarInfo&      _key_post_crit;
   const CCfgVarInfo&      _key_post_plat;


public:

   // CONSTRUCTOR
   Screen_CONFIG_DONOR();

   // DESTRUCTOR
   virtual ~Screen_CONFIG_DONOR();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // CONFIG BUTTONS
   virtual void process_ac_rate ();
   virtual void process_post_crit ();
   virtual void process_post_plat ();
   virtual void process_max_draw_flow ();
   virtual void process_inlet_flow_ramp ();
   virtual void process_inlet_management ();
   virtual void process_return_management ();
   virtual void process_autoflow ();


   void chooseToggleButtonAppearance (bool isOn, ConfigButton& btn,
                                      TEXT_STRING_ID onTxt = textListboxItemCfgOn,
                                      TEXT_STRING_ID offTxt = textListboxItemCfgOff,
                                      BITMAP_ID onBmp = CFG_Look::BUTTON_DOWN_NAME,
                                      BITMAP_ID offBmp = CFG_Look::BUTTON_UP_NAME);



protected:

   // MESSAGE NOTIFIES
   virtual void update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);
   virtual void update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer);

   void checkForTimeOnlyConfig ();

   void dealloc_widgets ();

}; // END of Screen_CONFIG_donor Class Declaration

// End of single include define guardian
#endif

//
///////////////////////////////////////////////////////////////////
// end of cfg_dono.hpp file

/* FORMAT HASH e7ab5dfcfdd67f32606ad98e2e9b3ef8 */

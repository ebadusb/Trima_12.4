/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_PLASMA

*     File name:   cfg_plas.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************
*/

// Single include define guardian
#ifndef SYSCONFIG_PLASMA_HPP
#define SYSCONFIG_PLASMA_HPP

// CLASS INCLUDE
#include "screen_config.hpp"
#include "cfg_butt.hpp"

void cb_pls_volume_1 (void* data);
void cb_pls_volume_2 (void* data);
void cb_pls_volume_3 (void* data);
void cb_pls_volume_4 (void* data);
void cb_pls_volume_5 (void* data);
void cb_pls_volume_6 (void* data);
void cb_pls_amap_min (void* data);
void cb_pls_amap_max (void* data);
void cb_pls_male_only_plasma (void* data);
void cb_pls_ffp_volume (void* data);

///////////////////////////////////////////////////////////////////
//
class Screen_CONFIG_PLASMA
   : public Screen_Config
{
public:

   // CONSTRUCTOR
   Screen_CONFIG_PLASMA();

   // DESTRUCTOR
   virtual ~Screen_CONFIG_PLASMA();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // Config Buttons
   virtual void process_pls_volume_1 ();
   virtual void process_pls_volume_2 ();
   virtual void process_pls_volume_3 ();
   virtual void process_pls_volume_4 ();
   virtual void process_pls_volume_5 ();
   virtual void process_pls_volume_6 ();
   virtual void process_pls_amap_min ();
   virtual void process_pls_amap_max ();
   virtual void process_pls_male_only ();
   virtual void process_pls_ffp_volume ();

   // MESSAGE NOTIFIES
   virtual void update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);
   virtual void update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer);

   void checkForTimeOnlyConfig ();

private:
   // Buttons
   Config_button_with_text btn_plasma_volume_1;
   Config_button_with_text btn_plasma_volume_2;
   Config_button_with_text btn_plasma_volume_3;
   Config_button_with_text btn_plasma_volume_4;
   Config_button_with_text btn_plasma_volume_5;
   Config_button_with_text btn_plasma_volume_6;
   Config_button_with_text btn_amap_min;
   Config_button_with_text btn_amap_max;
   Config_button_with_text btn_male_only;
   Config_button_with_text btn_ffp_volume;

   Bitmap                  _horizontalLine;

   // Range Constants
   const float pls_volume_1_min;
   const float pls_volume_1_max;

   const float pls_volume_2_min;
   const float pls_volume_2_max;

   const float pls_volume_3_min;
   const float pls_volume_3_max;

   const float pls_volume_4_min;
   const float pls_volume_4_max;

   const float pls_volume_5_min;
   const float pls_volume_5_max;

   const float pls_volume_6_min;
   const float pls_volume_6_max;

   const float pls_amap_min_min;
   const float pls_amap_min_max;

   const float pls_amap_max_min;
   const float pls_amap_max_max;

   const float pls_ffp_volume_min;
   const float pls_ffp_volume_max;

}; // END of Screen_CONFIG_PLASMA Class Declaration

#endif

/* FORMAT HASH b5bb86e8e55e615f32456dfca460792e */

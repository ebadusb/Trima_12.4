/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_PROCEDURE

*     File name:   cfg_proc.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

*/

// Single include define guardian
#ifndef SYSCONFIGPROC_HPP
#define SYSCONFIGPROC_HPP

/** include files **/

// CLASS INCLUDE
#include "screen_config.hpp"
#include "cfg_butt.hpp"

//
///////////////////////////////////////////////////////////////////
//
class Screen_CONFIG_PROCEDURE
   : public Screen_Config
{
public:
   Screen_CONFIG_PROCEDURE();
   virtual ~Screen_CONFIG_PROCEDURE();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // CONFIG BUTTONS
   virtual void processProcTime ();
   virtual void processReturnPressure ();
   virtual void processDrawPressure ();
   virtual void processCustomRatio ();
   virtual void processRbcRatio ();
   virtual void processPlasmaRatio ();
   virtual void processPlasmaRinseback ();
   virtual void processSalineRinseback ();
   virtual void processBloodDiversion ();
   virtual void processAirRemoval ();

protected:
   void chooseToggleButtonAppearance (bool isOn, ConfigButton& btn,
                                      TEXT_STRING_ID onTxt = textListboxItemCfgOn,
                                      TEXT_STRING_ID offTxt = textListboxItemCfgOff,
                                      BITMAP_ID onBmp = CFG_Look::BUTTON_DOWN_NAME,
                                      BITMAP_ID offBmp = CFG_Look::BUTTON_UP_NAME);

   // MESSAGE NOTIFIES
   virtual void update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);
   virtual void update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer);

   void checkForTimeOnlyConfig ();
   void updateDecDelimBtn ();

protected:

   // The config button items
   ConfigButton
      _btnProcTime,
      _btnReturnPress,
      _btnDrawPress,
      _btnCustomRatio,
      _btnPlasmaRatio,
      _btnRbcRatio,
      _btnBloodDiversion,
      _btnAirRemoval,
      _btnPlasmaRinseback,
      _btnSalineRinseback;

   Display_Text
      _txtLblProcTime,
      _txtLblReturnPress,
      _txtLblDrawPress,
      _txtLblCustomRatio,
      _txtLblRbcRatio,
      _txtLblPlasmaRatio,
      _txtLblBloodDiversion,
      _txtLblAirRemoval,
      _txtLblPlasmaRinseback,
      _txtLblSalineRinseback;

};
#endif

/* FORMAT HASH 4135dc6f57fc3fdbb287f56569473a46 */

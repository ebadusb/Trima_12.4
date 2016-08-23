/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_PROCEDURE

*     File name:   cfg_proc.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/
#include <vxworks.h>

/** include files **/
// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "an_alarm.h"
#include "software_cds.h"

// BASE CLASS INCLUDE
#include "cfg_proc.hpp"

/** defines **/
#define CFG_PROC_BUTTONS_FIRST_Y_COORD 115
#define CFG_PROC_BUTTONS_Y_DELTA 65
#define CFG_PROC_BUTTONS_Y_COORD(CFG_PROC_BUTTONS_Y_INDEX) CFG_PROC_BUTTONS_FIRST_Y_COORD + ((CFG_PROC_BUTTONS_Y_INDEX - 1) * CFG_PROC_BUTTONS_Y_DELTA)

#define CFG_PROC_BUTTONS_LEFT_COL 190
#define CFG_PROC_BUTTONS_RIGHT_COL 510

static void cbCfgProcTime (void* data);
static void cbCfgReturnPressure (void* data);
static void cbCfgDrawPressure (void* data);
static void cbCfgCustomRatio (void* data);
static void cbCfgRbcRatio (void* data);
static void cbCfgPlasmaRatio (void* data);
static void cbCfgPlasmaRinseback (void* data);
static void cbCfgSalineRinseback (void* data);
static void cbCfgBloodDiversion (void* data);
static void cbCfgAirRemoval (void* data);

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_CONFIG_PROCEDURE::Screen_CONFIG_PROCEDURE()
   : Screen_Config(TEAL, GUI_SCREEN_CONFIG_PROCEDURE, textCfgScreenLabelProc),

     _btnProcTime(CFG_PROC_BUTTONS_LEFT_COL,
                  CFG_PROC_BUTTONS_Y_COORD(1),
                  guiglobs::cdsData.rwConfig.procedure.key_proc_time,
                  KEY_PROC_TIME,
                  CFG_Look::BUTTON_UP_NAME,
                  CFG_Look::BUTTON_DOWN_NAME,
                  GUI_BUTTON_CONFIG_PROCEDURE_TIME),

     _btnReturnPress (CFG_PROC_BUTTONS_LEFT_COL,
                      CFG_PROC_BUTTONS_Y_COORD(2),
                      guiglobs::cdsData.rwConfig.procedure.key_return_press,
                      KEY_RETURN_PRESS,
                      CFG_Look::BUTTON_UP_NAME,
                      CFG_Look::BUTTON_DOWN_NAME,
                      GUI_BUTTON_CONFIG_RETURN_PRESSURE),

     _btnDrawPress   (CFG_PROC_BUTTONS_LEFT_COL,
                      CFG_PROC_BUTTONS_Y_COORD(3),
                      guiglobs::cdsData.rwConfig.procedure.key_draw_press,
                      KEY_DRAW_PRESS,
                      CFG_Look::BUTTON_UP_NAME,
                      CFG_Look::BUTTON_DOWN_NAME,
                      GUI_BUTTON_CONFIG_DRAW_PRESSURE),

     _btnBloodDiversion(CFG_PROC_BUTTONS_LEFT_COL,
                        CFG_PROC_BUTTONS_Y_COORD(4),
                        guiglobs::cdsData.rwConfig.procedure.key_blood_diversion,
                        KEY_BLOOD_DIVERSION,
                        CFG_Look::BUTTON_UP_NAME,
                        CFG_Look::BUTTON_DOWN_NAME,
                        GUI_BUTTON_CONFIG_BLOOD_DIVERSION),

     _btnPlasmaRinseback (CFG_PROC_BUTTONS_LEFT_COL,
                          CFG_PROC_BUTTONS_Y_COORD(5),
                          guiglobs::cdsData.rwConfig.procedure.key_plasma_rinseback,
                          KEY_PLASMA_RINSEBACK,
                          CFG_Look::BUTTON_UP_NAME,
                          CFG_Look::BUTTON_DOWN_NAME,
                          GUI_BUTTON_CONFIG_PLASMA_RINSEBACK),

     _btnCustomRatio (CFG_PROC_BUTTONS_RIGHT_COL,
                      CFG_PROC_BUTTONS_Y_COORD(1),
                      guiglobs::cdsData.rwConfig.procedure.key_custom_ratio,
                      KEY_CUSTOM_RATIO,
                      CFG_Look::BUTTON_UP_NAME,
                      CFG_Look::BUTTON_DOWN_NAME,
                      GUI_BUTTON_CONFIG_CUSTOM_RATIO),

     _btnRbcRatio    (CFG_PROC_BUTTONS_RIGHT_COL,
                      CFG_PROC_BUTTONS_Y_COORD(2),
                      guiglobs::cdsData.rwConfig.predict.key_rbc_ratio,
                      KEY_RBC_RATIO,
                      CFG_Look::BUTTON_UP_NAME,
                      CFG_Look::BUTTON_DOWN_NAME,
                      GUI_BUTTON_CONFIG_RBC_RATIO),

     _btnPlasmaRatio (CFG_PROC_BUTTONS_RIGHT_COL,
                      CFG_PROC_BUTTONS_Y_COORD(3),
                      guiglobs::cdsData.rwConfig.procedure.key_plasma_ratio,
                      KEY_PLASMA_RATIO,
                      CFG_Look::BUTTON_UP_NAME,
                      CFG_Look::BUTTON_DOWN_NAME,
                      GUI_BUTTON_CONFIG_PLASMA_RATIO),


     _btnAirRemoval  (CFG_PROC_BUTTONS_RIGHT_COL,
                      CFG_PROC_BUTTONS_Y_COORD(4),
                      guiglobs::cdsData.rwConfig.procedure.key_air_removal,
                      KEY_AIR_REMOVAL,
                      CFG_Look::BUTTON_UP_NAME,
                      CFG_Look::BUTTON_DOWN_NAME,
                      GUI_BUTTON_CONFIG_AIR_REMOVAL),

     _btnSalineRinseback (CFG_PROC_BUTTONS_RIGHT_COL,
                          CFG_PROC_BUTTONS_Y_COORD(5),
                          guiglobs::cdsData.rwConfig.procedure.key_saline_rinseback,
                          KEY_SALINE_RINSEBACK,
                          CFG_Look::BUTTON_UP_NAME,
                          CFG_Look::BUTTON_DOWN_NAME,
                          GUI_BUTTON_CONFIG_SALINE_RINSEBACK),

     _txtLblProcTime(textButtonLabelCfgLeftProcTime),
     _txtLblReturnPress(textButtonLabelCfgLeftProcReturnPressure),
     _txtLblDrawPress(textButtonLabelCfgLeftProcDrawPressure),
     _txtLblCustomRatio(textButtonLabelCfgLeftProcCustomRatio),
     _txtLblRbcRatio(textButtonLabelCfgLeftProcRbcRatio),
     _txtLblPlasmaRatio(textButtonLabelCfgLeftProcPlasmaRatio),
     _txtLblBloodDiversion(textButtonLabelCfgLeftProcBloodDiversion),
     _txtLblAirRemoval(textButtonLabelCfgLeftProcAirRemoval),
     _txtLblPlasmaRinseback(textButtonLabelCfgLeftProcPlsRinseback),
     _txtLblSalineRinseback(textButtonLabelCfgLeftProcSalineRinseback)

{ }



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CONFIG_PROCEDURE::~Screen_CONFIG_PROCEDURE()
{
   Screen_CONFIG_PROCEDURE::deallocate_resources ();
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_PROCEDURE::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   // layer allocation from Z-back to Z-front.

   // Allocate the buttons

   _btnProcTime.allocate_resources (*this);
   _btnProcTime.set_callback (cbCfgProcTime, (void*)this);

   _btnReturnPress.allocate_resources (*this);
   _btnReturnPress.set_callback (cbCfgReturnPressure, (void*)this);

   _btnDrawPress.allocate_resources (*this);
   _btnDrawPress.set_callback (cbCfgDrawPressure, (void*)this);

   _btnCustomRatio.allocate_resources (*this);
   _btnCustomRatio.set_callback (cbCfgCustomRatio, (void*)this);

   _btnRbcRatio.allocate_resources (*this);
   _btnRbcRatio.set_callback (cbCfgRbcRatio, (void*)this);

   _btnPlasmaRatio.allocate_resources (*this);
   _btnPlasmaRatio.set_callback (cbCfgPlasmaRatio, (void*)this);

   _btnBloodDiversion.allocate_resources(*this);
   _btnBloodDiversion.set_callback(cbCfgBloodDiversion, (void*)this);

   if (Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      _btnAirRemoval.allocate_resources (*this);
      _btnAirRemoval.set_callback (cbCfgAirRemoval, (void*)this);
      _txtLblAirRemoval.allocate_resources(*this);
   }

   _btnPlasmaRinseback.allocate_resources (*this);
   _btnPlasmaRinseback.set_callback (cbCfgPlasmaRinseback, (void*)this);

   _btnSalineRinseback.allocate_resources (*this);
   _btnSalineRinseback.set_callback (cbCfgSalineRinseback, (void*)this);

   _txtLblProcTime.allocate_resources(*this);
   _txtLblReturnPress.allocate_resources(*this);
   _txtLblDrawPress.allocate_resources(*this);
   _txtLblCustomRatio.allocate_resources(*this);
   _txtLblRbcRatio.allocate_resources(*this);
   _txtLblPlasmaRatio.allocate_resources(*this);
   _txtLblBloodDiversion.allocate_resources(*this);
   _txtLblPlasmaRinseback.allocate_resources(*this);
   _txtLblSalineRinseback.allocate_resources(*this);

   updateDecDelimBtn();

   // Make sure toggle buttons have the correct bitmap and text.
   chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.procedure.key_blood_diversion, _btnBloodDiversion);
   chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.procedure.key_air_removal,      _btnAirRemoval);
   chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.procedure.key_plasma_rinseback, _btnPlasmaRinseback);
   chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.procedure.key_saline_rinseback, _btnSalineRinseback);

   checkForTimeOnlyConfig();

} // END of Screen_CONFIG_procedure ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIG_PROCEDURE::deallocate_resources ()
{
   // deallocate all constituent objects first
   _btnProcTime.deallocate_resources ();
   _btnReturnPress.deallocate_resources ();
   _btnDrawPress.deallocate_resources ();
   _btnCustomRatio.deallocate_resources ();
   _btnRbcRatio.deallocate_resources();
   _btnPlasmaRatio.deallocate_resources();
   _btnBloodDiversion.deallocate_resources();

   if (Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      _btnAirRemoval.deallocate_resources();
      _txtLblAirRemoval.deallocate_resources();
   }
   _btnPlasmaRinseback.deallocate_resources();
   _btnSalineRinseback.deallocate_resources();

   _txtLblProcTime.deallocate_resources();
   _txtLblReturnPress.deallocate_resources();
   _txtLblDrawPress.deallocate_resources();
   _txtLblCustomRatio.deallocate_resources();
   _txtLblRbcRatio.deallocate_resources();
   _txtLblPlasmaRatio.deallocate_resources();
   _txtLblBloodDiversion.deallocate_resources();
   _txtLblAirRemoval.deallocate_resources();
   _txtLblPlasmaRinseback.deallocate_resources();
   _txtLblSalineRinseback.deallocate_resources();

   // deallocate parent's resources
   Screen_Config::deallocate_resources ();
}  // END of Screen_CONFIG_PROCEDURE DEALLOCATE_RESOURCES


void Screen_CONFIG_PROCEDURE::chooseToggleButtonAppearance (bool isOn, ConfigButton& btn,
                                                            TEXT_STRING_ID onTxt = textListboxItemCfgOn,
                                                            TEXT_STRING_ID offTxt = textListboxItemCfgOff,
                                                            BITMAP_ID onBmp = CFG_Look::BUTTON_DOWN_NAME,
                                                            BITMAP_ID offBmp = CFG_Look::BUTTON_UP_NAME)
{
   btn.set_up_bitmap((isOn) ? onBmp : offBmp);
   btn.set_string_id((isOn) ? onTxt : offTxt);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// callbacks for the PROCEDURE TIME buttons
void cbCfgProcTime (void* data)
{
   ((Screen_CONFIG_PROCEDURE*)data)->processProcTime();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the PROCEDURE TIME button
void Screen_CONFIG_PROCEDURE::processProcTime ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_PROC_TIME).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_PROC_TIME).Max();

   display_min_max_keypad_limits(min, max, "%.0f");

   _btnProcTime.create_keypad_string
      (keypad_string, KEY_PROC_TIME, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "###", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the return_press buttons
void cbCfgReturnPressure (void* data)
{
   ((Screen_CONFIG_PROCEDURE*)data)->processReturnPressure();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the return_press button
void Screen_CONFIG_PROCEDURE::processReturnPressure ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_RETURN_PRESS).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_RETURN_PRESS).Max();

   display_min_max_keypad_limits(min, max, "%.0f");

   _btnReturnPress.create_keypad_string
      (keypad_string, KEY_RETURN_PRESS, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y + CFG_Look::Y_DIFF, "###", max, min);

   goto_child_screen (GUI_SCREEN_KEYPAD, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the draw_press buttons
void cbCfgDrawPressure (void* data)
{
   ((Screen_CONFIG_PROCEDURE*)data)->processDrawPressure();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the draw_press button
void Screen_CONFIG_PROCEDURE::processDrawPressure ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_DRAW_PRESS).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_DRAW_PRESS).Max();

   display_min_max_keypad_limits(min, max, "%.0f");

   _btnReturnPress.create_keypad_string
      (keypad_string, KEY_DRAW_PRESS, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y + CFG_Look::Y_DIFF, "-###", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the custom_ratio buttons
void cbCfgCustomRatio (void* data)
{
   ((Screen_CONFIG_PROCEDURE*)data)->processCustomRatio();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the start_ratio button
void Screen_CONFIG_PROCEDURE::processCustomRatio ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_CUSTOM_RATIO).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_CUSTOM_RATIO).Max();

   display_min_max_keypad_limits(min, max, "%.1f");

   _btnCustomRatio.create_keypad_string
      (keypad_string, KEY_CUSTOM_RATIO, CFG_Look::FIRST_X, CFG_Look::FIRST_Y, "##.#", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the _btnRbcRatio buttons
void cbCfgRbcRatio (void* data)
{
   ((Screen_CONFIG_PROCEDURE*)data)->processRbcRatio();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the _btnRbcRatio button
void Screen_CONFIG_PROCEDURE::processRbcRatio ()
{
   _btnRbcRatio.create_param_string
      (keypad_string, KEY_RBC_RATIO, CFG_Look::KEYPAD_X_2, CFG_Look::FIRST_Y + (2 * CFG_Look::Y_DIFF));

   goto_child_screen(GUI_SCREEN_LISTBOX, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the _btnPlasmaRatio buttons
void cbCfgPlasmaRatio (void* data)
{
   ((Screen_CONFIG_PROCEDURE*)data)->processPlasmaRatio();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the _btnPlasmaRatio button
void Screen_CONFIG_PROCEDURE::processPlasmaRatio ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_PLASMA_RATIO).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_PLASMA_RATIO).Max();

   display_min_max_keypad_limits(min, max, "%.1f");

   _btnPlasmaRatio.create_keypad_string (keypad_string, KEY_PLASMA_RATIO, CFG_Look::FIRST_X, CFG_Look::FIRST_Y, "##.#", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the _btnPlasmaRinseback buttons
void cbCfgPlasmaRinseback (void* data)
{
   ((Screen_CONFIG_PROCEDURE*)data)->processPlasmaRinseback();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the _btnPlasmaRinseback button
void Screen_CONFIG_PROCEDURE::processPlasmaRinseback ()
{
   guiglobs::cdsData.rwConfig.procedure.key_plasma_rinseback = (bool)(!guiglobs::cdsData.rwConfig.procedure.key_plasma_rinseback);

   chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.procedure.key_plasma_rinseback,
                                _btnPlasmaRinseback);
}

//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the _btnSalineRinseback buttons
void cbCfgSalineRinseback (void* data)
{
   ((Screen_CONFIG_PROCEDURE*)data)->processSalineRinseback();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the _btnSalineRinseback button
void Screen_CONFIG_PROCEDURE::processSalineRinseback ()
{
   guiglobs::cdsData.rwConfig.procedure.key_saline_rinseback = (bool)(!guiglobs::cdsData.rwConfig.procedure.key_saline_rinseback);

   chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.procedure.key_saline_rinseback,
                                _btnSalineRinseback);
}

//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the _btnBloodDiversion button
void cbCfgBloodDiversion (void* data)
{
   ((Screen_CONFIG_PROCEDURE*)data)->processBloodDiversion();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the _btnBloodDiversion button
void Screen_CONFIG_PROCEDURE::processBloodDiversion ()
{
   guiglobs::cdsData.rwConfig.procedure.key_blood_diversion = (bool)(!guiglobs::cdsData.rwConfig.procedure.key_blood_diversion);

   chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.procedure.key_blood_diversion,
                                _btnBloodDiversion);
}

void cbCfgAirRemoval (void* data)
{
   ((Screen_CONFIG_PROCEDURE*)data)->processAirRemoval();
}

void Screen_CONFIG_PROCEDURE::processAirRemoval ()
{
   guiglobs::cdsData.rwConfig.procedure.key_air_removal = (bool)(!guiglobs::cdsData.rwConfig.procedure.key_air_removal);

   chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.procedure.key_air_removal,
                                _btnAirRemoval);
}
//
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Message handler for KEYPADS
void Screen_CONFIG_PROCEDURE::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer)
{
   // printf("CFG_PROC: keypad= %d \n",return_keypad_buffer -> keypad_return_value);
   turn_off_min_max_keypad_text();

   switch (return_keypad_buffer.config_data_type)
   {
      case KEY_PROC_TIME :
      {
         if ((return_keypad_buffer.keypad_return_value) == -1)
         {
            return;
         }

         _btnProcTime.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         int the_data = atoi(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.procedure.key_proc_time = the_data;
         break;
      }

      case KEY_RETURN_PRESS :
      {
         if ((return_keypad_buffer.keypad_return_value) == -1)
         {
            return;
         }

         _btnReturnPress.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.procedure.key_return_press = the_data;
         break;
      }

      case KEY_DRAW_PRESS :
      {
         if ((return_keypad_buffer.keypad_return_value) == -1)
         {
            return;
         }

         _btnDrawPress.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.procedure.key_draw_press = the_data;

         break;
      }

      case KEY_CUSTOM_RATIO :
      {
         if ((return_keypad_buffer.keypad_return_value) == -1)
         {
            return;
         }

         _btnCustomRatio.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.procedure.key_custom_ratio = the_data;
         break;
      }

      case KEY_PLASMA_RATIO :
      {
         if ((return_keypad_buffer.keypad_return_value) == -1)
         {
            return;
         }

         _btnPlasmaRatio.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.procedure.key_plasma_ratio = the_data;
         break;
      }

      default :
      {
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "could not find PROCEDURE keypad index" << endmsg;
      }
   }
}



//
///////////////////////////////////////////////////////////////////////////////
// Message handler for LISTBOXS
void Screen_CONFIG_PROCEDURE::update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer)
{
   switch (rtn_listbox_buffer.key_name)
   {
      case KEY_RBC_RATIO :
         _btnRbcRatio.display_the_text_string (CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
         guiglobs::cdsData.rwConfig.predict.key_rbc_ratio = int(rtn_listbox_buffer.key_value);
         updateDecDelimBtn();
         break;

      case KEY_AIR_REMOVAL :
         _btnAirRemoval.display_the_text_string (CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
         guiglobs::cdsData.rwConfig.procedure.key_air_removal = int(rtn_listbox_buffer.key_value);
         break;

      default :
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "Could not find the return key upon a return from the button list box" << endmsg;
   }  // end of switch

} // end of update_screen_with_date method
  //

void Screen_CONFIG_PROCEDURE::checkForTimeOnlyConfig ()
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_info) << "Buttons should be disabled." << endmsg;

      const bool bloodDiversionOn  = (guiglobs::cdsData.rwConfig.procedure.key_blood_diversion != 0);
      const bool airRemovalOn      = (guiglobs::cdsData.rwConfig.procedure.key_air_removal != 0);
      const bool plasmaRinsebackOn = (guiglobs::cdsData.rwConfig.procedure.key_plasma_rinseback != 0);
      const bool salineRinsebackOn = (guiglobs::cdsData.rwConfig.procedure.key_saline_rinseback != 0);

      disableConfigBtn(_btnProcTime);
      disableConfigBtn(_btnReturnPress);
      disableConfigBtn(_btnDrawPress);
      disableConfigBtn(_btnCustomRatio);
      disableConfigBtn(_btnRbcRatio);
      disableConfigBtn(_btnPlasmaRatio);

      disableConfigBtn(_btnPlasmaRinseback,   CHOOSE_DISABLED_BITMAP(plasmaRinsebackOn));
      disableConfigBtn(_btnSalineRinseback,   CHOOSE_DISABLED_BITMAP(salineRinsebackOn));
      disableConfigBtn(_btnAirRemoval,        CHOOSE_DISABLED_BITMAP(airRemovalOn)    );
      disableConfigBtn(_btnBloodDiversion,    CHOOSE_DISABLED_BITMAP(bloodDiversionOn));

      disableConfigTxt(_txtLblProcTime);
      disableConfigTxt(_txtLblReturnPress);
      disableConfigTxt(_txtLblDrawPress);
      disableConfigTxt(_txtLblCustomRatio);
      disableConfigTxt(_txtLblRbcRatio);
      disableConfigTxt(_txtLblPlasmaRatio);
      disableConfigTxt(_txtLblBloodDiversion);
      disableConfigTxt(_txtLblAirRemoval);
      disableConfigTxt(_txtLblPlasmaRinseback);
      disableConfigTxt(_txtLblSalineRinseback);
   }
}

void Screen_CONFIG_PROCEDURE::updateDecDelimBtn ()
{
   if (guiglobs::cdsData.rwConfig.predict.key_rbc_ratio == RBC_814)
      _btnRbcRatio.set_string_id(textListboxItemCfgRbcRatio814);
   else
      _btnRbcRatio.set_string_id(textListboxItemCfgRbcRatio11);

   if (guiglobs::cdsData.config.LangUnit.Get().key_decimal_delimiter == DECIMAL_DELIMIT)
   {
      guistring delimString;
      _btnRbcRatio.get_text_direct(delimString);
      delimString.swapOutDelimiter();
      _btnRbcRatio.set_text_direct(delimString);
   }
}

/* FORMAT HASH f3fe046e8bcdae3b3f0ff15d410a93f0 */

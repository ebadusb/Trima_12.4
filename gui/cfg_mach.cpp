/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_MACHINE

*     File name:   cfg_mach.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
*/

#include <vxworks.h>


/** include files **/

// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "an_alarm.h"
#include "trima_time.h"
#include "cassetteDeleteConfirmBox.hpp"
#include "set_listbox.hpp"

// BASE CLASS INCLUDE
#include "cfg_mach.hpp"
#include "statline.hpp"

#define __MACH_BUTTON_X__ 140
#define __MACH_BUTTON_FIRST_Y__ 110
#define __MACH_BUTTON_Y_DELTA__ 65
#define __MACH_BUTTON_Y__(__Y_LEVEL__) __MACH_BUTTON_FIRST_Y__ + (__Y_LEVEL__ * __MACH_BUTTON_Y_DELTA__)

#define __MAX_BARCODE_PRINT_LENGTH__ 19

static void cb_cfg_set_time (void* data);
static void cb_cfg_set_date (void* data);
static void cb_cfg_set_audio (void* data);
static void cb_cfg_date_format (void* data);
static void cb_cfg_time_format (void* data);
static void cb_cfg_set_id (void* data);
static void cb_cfg_set_code (void* data);
static void cb_cfg_add_cassette (void* data);
static void cb_cfg_remove_cassette (void* data);

/** defines **/


///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_CONFIG_MACHINE::Screen_CONFIG_MACHINE()
   : Screen_Config (TEAL, GUI_SCREEN_CONFIG_MACHINE, textCfgScreenLabelMach),

     _cfgBtnDateFormat(__MACH_BUTTON_X__,
                       __MACH_BUTTON_Y__(0),
                       guiglobs::cdsData.rwConfig.lang.key_date_format,
                       KEY_DATE_FORMAT,
                       CFG_Look::BUTTON_UP_NAME,
                       CFG_Look::BUTTON_DOWN_NAME,
                       GUI_BUTTON_CONFIG_DATE_FORMAT,
                       textButtonLabelCfgLeftMachDateFmt,
                       CFG_Look::FONT_COLOR,
                       CFG_Look::FONT_ALIGN,
                       CFG_Look::FONT_SIZE),

     _cfgBtnSetDate  (__MACH_BUTTON_X__,
                      __MACH_BUTTON_Y__(1),
                      guiglobs::cdsData.rwConfig.machine.key_set_date,
                      KEY_SET_DATE,
                      CFG_Look::BUTTON_UP_NAME,
                      CFG_Look::BUTTON_DOWN_NAME,
                      GUI_BUTTON_CONFIG_MACHINE_SET_DATE,
                      textButtonLabelCfgLeftMachDate,
                      CFG_Look::FONT_COLOR,
                      CFG_Look::FONT_ALIGN,
                      CFG_Look::FONT_SIZE),

     _cfgBtnTimeFormat(__MACH_BUTTON_X__,
                       __MACH_BUTTON_Y__(2),
                       guiglobs::cdsData.rwConfig.lang.key_time_format,
                       KEY_TIME_FORMAT,
                       CFG_Look::BUTTON_UP_NAME,
                       CFG_Look::BUTTON_DOWN_NAME,
                       GUI_BUTTON_CONFIG_TIME_FORMAT,
                       textButtonLabelCfgLeftMachTimeFmt,
                       CFG_Look::FONT_COLOR,
                       CFG_Look::FONT_ALIGN,
                       CFG_Look::FONT_SIZE),

     _cfgBtnSetTime  (__MACH_BUTTON_X__,
                      __MACH_BUTTON_Y__(3),
                      guiglobs::cdsData.rwConfig.machine.key_set_time,
                      KEY_SET_TIME,
                      CFG_Look::BUTTON_UP_NAME,
                      CFG_Look::BUTTON_DOWN_NAME,
                      GUI_BUTTON_CONFIG_MACHINE_SET_TIME,
                      textButtonLabelCfgLeftMachTime,
                      CFG_Look::FONT_COLOR,
                      CFG_Look::FONT_ALIGN,
                      CFG_Look::FONT_SIZE),

     _cfgBtnSetAudio  (__MACH_BUTTON_X__,
                       __MACH_BUTTON_Y__(4),
                       guiglobs::cdsData.rwConfig.machine.key_set_audio,
                       KEY_SET_AUDIO,
                       CFG_Look::BUTTON_UP_NAME,
                       CFG_Look::BUTTON_DOWN_NAME,
                       GUI_BUTTON_CONFIG_MACHINE_SET_AUDIO,
                       textButtonLabelCfgLeftMachAudio,
                       CFG_Look::FONT_COLOR,
                       CFG_Look::FONT_ALIGN,
                       CFG_Look::FONT_SIZE),

     _btnRefNum  (GUI_BUTTON_CONFIG_MACHINE_SET_ID,
                  277,
                  125,
                  textButtonCfgMachSetId,
                  CFG_Look::BUTTON_UP_NAME,
                  CFG_Look::BUTTON_DOWN_NAME
                  ),

     _btnSetCode (GUI_BUTTON_CONFIG_MACHINE_SET_CODE,
                  277,
                  180,
                  textButtonCfgMachAdminCode,
                  CFG_Look::BUTTON_UP_NAME,
                  CFG_Look::BUTTON_DOWN_NAME
                  ),

     _btnAddSet  (GUI_BUTTON_CONFIG_MACHINE_ADD_SET,
                  314,
                  290,
                  textButtonCfgLongMachAddCassette,
                  BITMAP_BUTTON_LARGE_BOT_GREY_UP,
                  BITMAP_BUTTON_LARGE_BOT_TEAL_DOWN
                  ),

     _btnRemoveSet   (GUI_BUTTON_CONFIG_MACHINE_REMOVE_SET,
                      314,
                      372,
                      textButtonCfgLongMachRemoveCassette,
                      BITMAP_BUTTON_LARGE_BOT_TEAL_UP,
                      BITMAP_BUTTON_LARGE_BOT_TEAL_DOWN
                      ),

     _bmpCassetteBoxBg   (267, 115, BITMAP_CFG_MACH_ADD_SET_BG),

     _txtRefNum (textEmptyFormatCfgMachSetIdInit),
     _txtSetCode (textEmptyFormatCfgMachAdminCodeInit),
     _txtBarcode (textEmptyFormatCfgMachBarcodeInit),
     _txtBarcodeLbl(textMiscCfgMachBarcode),
     _txtNumDigits (textMiscCfgMachCatNumDigits),
     _haveAdminCode (false),
     _haveCatalogNumber (false),
     _catalogNumber(""),
     _adminCode(""),
     _barcodeString("")
{
   initListboxStrings();
}



///////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_CONFIG_MACHINE::~Screen_CONFIG_MACHINE()
{
   Screen_CONFIG_MACHINE::deallocate_resources();
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_MACHINE::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   _msgBarcode.init(Callback<Screen_CONFIG_MACHINE> (this, &Screen_CONFIG_MACHINE::HandleBarcodeMsg), MessageBase::SNDRCV_RECEIVE_ONLY);


   // then allocate constituent objects with this window as parent widget
   // layer allocation from Z-back to Z-front.

   // Allocate the buttons

   _bmpCassetteBoxBg.allocate_resources(*this);

   _cfgBtnSetTime.allocate_resources (*this);
   _cfgBtnSetTime.set_callback (cb_cfg_set_time, (void*)this);

   _cfgBtnSetDate.allocate_resources (*this);
   _cfgBtnSetDate.set_callback (cb_cfg_set_date, (void*)this);

   _cfgBtnSetAudio.allocate_resources (*this);
   _cfgBtnSetAudio.set_callback (cb_cfg_set_audio, (void*)this);

   _cfgBtnDateFormat.allocate_resources (*this);
   _cfgBtnDateFormat.set_callback (cb_cfg_date_format, (void*)this);

   _cfgBtnTimeFormat.allocate_resources (*this);
   _cfgBtnTimeFormat.set_callback (cb_cfg_time_format, (void*)this);

   _btnRefNum.allocate_resources(*this);
   _btnRefNum.set_callback (cb_cfg_set_id, (void*)this);

   _btnSetCode.allocate_resources(*this);
   _btnSetCode.set_callback (cb_cfg_set_code, (void*)this);

   _btnAddSet.allocate_resources(*this);
   _btnAddSet.set_callback (cb_cfg_add_cassette, (void*)this);

   _btnRemoveSet.allocate_resources(*this);
   _btnRemoveSet.set_callback (cb_cfg_remove_cassette, (void*)this);

   _txtRefNum.allocate_resources(*this);
   _txtSetCode.allocate_resources(*this);
   _txtBarcode.allocate_resources(*this);
   _txtBarcodeLbl.allocate_resources(*this);

   setAddButtonIsActive(_haveAdminCode && _haveCatalogNumber);     // inactive until both catalog id and set code are entered
   setRemoveButtonIsActive(guiglobs::cassette_mgr->numSets() > 0); // inactive until both catalog id and set code are entered

   _bmpCassetteBoxBg.move_to_back();
   _bmpCassetteBoxBg.move_to_front();

   _msgListboxData.init(Callback<Screen_CONFIG_MACHINE>(this, &Screen_CONFIG_MACHINE::listboxCB));
   _msgDeletionConfirmed.init(Callback<Screen_CONFIG_MACHINE>(this, &Screen_CONFIG_MACHINE::confirmCB));

   checkForTimeOnlyConfig();
}  // END of Screen_CONFIG_MACHINE ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIG_MACHINE::deallocate_resources ()
{
   // Make sure there isn't some lingering pending deletion.
   guiglobs::cassette_mgr->clearCassetteToDelete();
   clearCassetteBox();

   _msgDeletionConfirmed.deregisterMsg();
   _msgListboxData.deregisterMsg();
   _msgBarcode.deregisterMsg();

   clearStatusAttention();

   // deallocate all constituent objects first
   _cfgBtnSetDate.deallocate_resources ();
   _cfgBtnSetTime.deallocate_resources ();
   _cfgBtnSetAudio.deallocate_resources ();
   _cfgBtnDateFormat.deallocate_resources ();
   _cfgBtnTimeFormat.deallocate_resources ();
   _btnRefNum.deallocate_resources();
   _btnSetCode.deallocate_resources();
   _btnAddSet.deallocate_resources();
   _txtRefNum.deallocate_resources();
   _txtSetCode.deallocate_resources();
   _txtBarcode.deallocate_resources();
   _txtBarcodeLbl.deallocate_resources();
   _txtNumDigits.deallocate_resources();

   // deallocate parent's resources
   Screen_Config::deallocate_resources ();
}   // END of Screen_CONFIG_MACHINE DEALLOCATE_RESOURCES



//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the DATE buttons
void cb_cfg_date_format (void* data)
{
   ((Screen_CONFIG_MACHINE*)data)->processDataFormatBtn();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the date button
void Screen_CONFIG_MACHINE::processDataFormatBtn ()
{
   clearStatusAttention();
   _cfgBtnDateFormat.create_param_string(keypad_string, KEY_DATE_FORMAT, CFG_Look::LISTBOX_X_2, CFG_Look::FIRST_Y);
   goto_child_screen(GUI_SCREEN_LISTBOX, keypad_string);
}

//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the time_format buttons
void cb_cfg_time_format (void* data)
{
   ((Screen_CONFIG_MACHINE*)data)->processTimeFormatBtn();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the time_format button
void Screen_CONFIG_MACHINE::processTimeFormatBtn ()
{
   clearStatusAttention();
   _cfgBtnTimeFormat.create_param_string(keypad_string, KEY_TIME_FORMAT, CFG_Look::LISTBOX_X_2, CFG_Look::FIRST_Y + CFG_Look::Y_DIFF);
   goto_child_screen(GUI_SCREEN_LISTBOX, keypad_string);
}

///////////////////////////////////////////////////////////////////////////////
// callbacks for the set_time buttons
void cb_cfg_set_time (void* data)
{
   ((Screen_CONFIG_MACHINE*)data)->processSetTimeBtn();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the set_time button
void Screen_CONFIG_MACHINE::processSetTimeBtn ()
{
   clearStatusAttention();
   _cfgBtnSetTime.create_keypad_string
      (keypad_string, KEY_SET_TIME, CFG_Look::FIRST_X, CFG_Look::FIRST_Y, "##:##", 2359, 0);
   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}

//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the set_date buttons
void cb_cfg_set_date (void* data)
{
   ((Screen_CONFIG_MACHINE*)data)->processSetDateBtn();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the set_date button
void Screen_CONFIG_MACHINE::processSetDateBtn ()
{
   clearStatusAttention();

   switch (guiglobs::cdsData.rwConfig.lang.key_date_format)
   {
      case MMDDYY_DATE : //  mm/dd/YYYY
         _cfgBtnSetDate.create_keypad_string
            (keypad_string, KEY_SET_DATE, CFG_Look::FIRST_X, CFG_Look::FIRST_Y, "##/##/##", -1, -1);
         break;

      case DDMMYY_DATE : // d/m/Y
         _cfgBtnSetDate.create_keypad_string
            (keypad_string, KEY_SET_DATE, CFG_Look::FIRST_X, CFG_Look::FIRST_Y, "##/##/##", -1, -1);
         break;

      case YYMMDD_DATE : // Y/m/d
         _cfgBtnSetDate.create_keypad_string
            (keypad_string, KEY_SET_DATE, CFG_Look::FIRST_X, CFG_Look::FIRST_Y, "##/##/##", -1, -1);
         break;

      default :
      {
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "could not find the correct DATE FORMAT to display the current date" << endmsg;
         break;
      }

   }

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}

//
///////////////////////////////////////////////////////////////////////////////
// callbacks for the set_audio buttons
void cb_cfg_set_audio (void* data)
{
   ((Screen_CONFIG_MACHINE*)data)->processSetAudioBtn();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the set_audio button
void Screen_CONFIG_MACHINE::processSetAudioBtn ()
{
   clearStatusAttention();

   _cfgBtnSetAudio.create_param_string
      (keypad_string, KEY_SET_AUDIO, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y);

   goto_child_screen(GUI_SCREEN_LISTBOX, keypad_string);
}

//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the REF number button
void cb_cfg_set_id (void* data)
{
   ((Screen_CONFIG_MACHINE*)data)->processRefNumBtn();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the REF number button
void Screen_CONFIG_MACHINE::processRefNumBtn ()
{
   clearStatusAttention();
   setMessage(textMiscCfgMachCatNumDigits);

   displayStatusAttention(textStatCfgMachEnterCatNum);

   keypad_string.initialize();  // empty out string
   char temp_str[200] = "";

   build_keypad_string(
      keypad_string,
      KEY_MACH_CATALOG_NUM,
      35,
      175,
      "######",
      999999,
      0);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}

//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the add cassette button
void cb_cfg_set_code (void* data)
{
   ((Screen_CONFIG_MACHINE*)data)->processSetCodeBtn();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the add cassette button
void Screen_CONFIG_MACHINE::processSetCodeBtn ()
{
   clearStatusAttention();
   setMessage(textMiscCfgAdminNumDigits);

   displayStatusAttention(textStatCfgMachEnterSetCode);

   keypad_string.initialize();  // empty out string

   build_keypad_string(
      keypad_string,
      KEY_MACH_ADMIN_CODE,
      35,
      175,
      "##########",
      double(0xFFFFFFFF), // 4,294,967,295
      0,
      guistring(),
      textEmptyFormatKeypadAdminCodeInit);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}

//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the add cassette button
void cb_cfg_add_cassette (void* data)
{
   ((Screen_CONFIG_MACHINE*)data)->processAddSetBtn();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the add cassette button
void Screen_CONFIG_MACHINE::processAddSetBtn ()
{
   if (guiglobs::cassette_mgr->canFitCassetteIn(_catalogNumber))
   {
      clearStatusAttention();
      DataLog (log_level_gui_info) << "Calling AddNewCassette(" << _catalogNumber
                                   << ", " << _adminCode << ")" << endmsg;

      CassetteMgr::ADD_RETURN_STATUS addStatus =
         guiglobs::cassette_mgr->AddNewCassette(_catalogNumber, _adminCode, _barcodeString);

      switch (addStatus)
      {
         case CassetteMgr::CASSETTE_ADD_SUCCESS :
            clearCassetteBox();
            DataLog (log_level_gui_info) << "New entry sucessfully added: (Catalog Number:"
                                         << _catalogNumber << ", Admin Code:" << _adminCode << ")." << endmsg;
            displayStatusAttention(textStatCfgMachSuccess);

            setRemoveButtonIsActive(guiglobs::cassette_mgr->numSets() > 0);
            break;

         case CassetteMgr::CASSETTE_ADD_INVALID :
            DataLog (log_level_gui_info) << "Invalid entry: (Catalog Number:"
                                         << _catalogNumber << ", Admin Code:" << _adminCode << ")." << endmsg;
            displayStatusAttention(textStatCfgMachBadPair);
            break;

         case CassetteMgr::CASSETTE_ADD_INVALID_BARCODE :
            DataLog (log_level_gui_info) << "Invalid barcode (REF number must be a substring in barcode).  Addition aborted." << endmsg;
            displayStatusAttention(textStatCfgMachBadBarcode);
            break;
      }
   }
   else
   {
      DataLog (log_level_gui_info) << "Maximum number of cassettes exceeded." << endmsg;
      displayStatusAttention(textStatMaxEntriesExceeded);
   }
}

//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the remove cassette button
//
void cb_cfg_remove_cassette (void* data)
{
   ((Screen_CONFIG_MACHINE*)data)->processRemoveSetBtn();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the add cassette button
void Screen_CONFIG_MACHINE::processRemoveSetBtn ()
{
   clearStatusAttention();
   DataLog (log_level_gui_info) << "Processing remove cassette button" << endmsg;
   goto_child_screen(GUI_SCREEN_SET_LISTBOX, _setIdAllocationString);
}

//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Message handler for KEYPADS
void Screen_CONFIG_MACHINE::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& keypad_buffer)
{
   char eString[256];

   clearMessage();
   //
   //
   //  Copy into a local buffer since we tokenize the data below.
   //
   KEYPAD_RETURN_STRUCT return_keypad_buffer = keypad_buffer;

   // check for both a 0 and -1 because pressing ENTER without data on the DATE keypad
   // returns a -1.  And doing the same on the TIME keypad returns a 0.

   if ((return_keypad_buffer.keypad_return_value == -1))
   {
      clearStatusAttention();
      DataLog(log_level_gui_error) << "Bad keypad return value" << endmsg;
      return;
   }


   switch (return_keypad_buffer.config_data_type)
   {
      case KEY_MACH_CATALOG_NUM :
      {
         guistring cassette_id_string;
         cassette_id_string.initialize();

         cassette_id_string = return_keypad_buffer.keypad_return_string;
         int cassette_id_length = cassette_id_string.length();

         // cassette_id = atoi(return_keypad_buffer.keypad_return_string);
         DataLog (log_level_gui_info) << "Entered cassette ID \"" << cassette_id_string << "\" is of length " << cassette_id_length << "." << endmsg;

         if (cassette_id_length == 5|| cassette_id_length == 6)
         {
            clearStatusAttention();
            _txtRefNum.set_text_direct(cassette_id_length, cassette_id_string);
            _haveCatalogNumber = true;
            _catalogNumber     = cassette_id_string;
            DataLog (log_level_gui_info) << "Set _catalogNumber = " << _catalogNumber << endmsg;

            int newAdminNum = guiglobs::cassette_mgr->findAdminNum(cassette_id_string);

            if (newAdminNum != 0)
            {
               sprintf(_adminCode, "%08d", newAdminNum);
               _haveAdminCode = true;
            }
            else
            {
               sprintf(_adminCode, "");
               _haveAdminCode = false;
            }

            _txtSetCode.set_text_direct(_adminCode.buffersize, _adminCode);

            guistring newBarcode = guiglobs::cassette_mgr->findBarcode(cassette_id_string);

            if (newBarcode.length() > 8 && newBarcode.length() < 25)
            {
               _barcodeString = newBarcode;
               newBarcode.strip_unprintable_chars();

               if (newBarcode.length() > __MAX_BARCODE_PRINT_LENGTH__)
               {
                  DataLog (log_level_gui_info) << "Barcode exceeds max size.  Truncating before printing." << endmsg;
                  newBarcode.truncate(__MAX_BARCODE_PRINT_LENGTH__);
               }

            }
            else
            {
               newBarcode.initialize();
               sprintf(_barcodeString, "");
            }

            _txtBarcode.set_text_direct(newBarcode.length(), newBarcode);

            checkAddButtonStatus();
         }
         else
         {
            displayStatusAttention(textStatCfgMachBadCatNum);
            DataLog (log_level_gui_info) << "Cassette ID length is incorrect.  Returning." << endmsg;
            return;
         }
         break;
      }

      case KEY_MACH_ADMIN_CODE :
      {
         guistring admin_code_string;

         admin_code_string = return_keypad_buffer.keypad_return_string;
         int admin_code_length = admin_code_string.length();

         // cassette_id = atoi(return_keypad_buffer.keypad_return_string);
         DataLog (log_level_gui_info) << "Entered cassette functionality code \"" << admin_code_string << "\" is of length " << admin_code_length << "." << endmsg;

         if (admin_code_length >= 8 && admin_code_length <= 10)
         {
            clearStatusAttention();
            _txtSetCode.set_text_direct(admin_code_length, admin_code_string);
            _haveAdminCode = true;
            _adminCode     = admin_code_string;
            DataLog (log_level_gui_info) << "Set _adminCode = " << _adminCode << endmsg;
            checkAddButtonStatus();
         }
         else
         {
            displayStatusAttention(textStatCfgMachBadSetCode);
            DataLog (log_level_gui_info) << "Cassette admin code length is incorrect.  Returning." << endmsg;
            return;
         }
         break;
      }

      case KEY_SET_TIME :
      {
         if (strlen(return_keypad_buffer.keypad_return_string) < 4)
         {
            return;
         }

         char* strtokbuff = return_keypad_buffer.keypad_return_string.make_buffer();
         char* nextToken  = 0;
         int   hour_int   = atoi(strtok_r(strtokbuff, ":", &nextToken));
         int   min_int    = atoi(strtok_r(NULL, ":", &nextToken));
         GUI_POOL_FREE(strtokbuff);

         if (hour_int > 23)
         {
            hour_int = 23;
            min_int  = 59;
         }

         if (min_int > 59)
         {
            min_int = 59;
         }

         // get the current date
         time_t timeRaw   = time(NULL);
         tm*    timeOfDay = localtime(&timeRaw);

         // change the min, hour, sec parts
         timeOfDay->tm_sec  = 0;
         timeOfDay->tm_min  = min_int;
         timeOfDay->tm_hour = hour_int;

         // convert it back
         timeRaw = mktime(timeOfDay);

         // and send it
         trima_time::set(timeRaw);

         sprintf(eString, "Time of day: %d.%d.%d.%d.%d.%d",
                 timeOfDay->tm_year,
                 timeOfDay->tm_mon,
                 timeOfDay->tm_mday,
                 timeOfDay->tm_hour,
                 timeOfDay->tm_min,
                 timeOfDay->tm_sec);

         DataLog(log_level_gui_info) << eString << endmsg;

         // get the current date again
         timeRaw   = time(NULL);
         timeOfDay = localtime(&timeRaw);

         // display it
         guistring time_str;
         sprintf(time_str, "%02d:%02d", timeOfDay->tm_hour, timeOfDay->tm_min);
         _cfgBtnSetTime.set_text_direct (10, time_str);

         break;
      }

      case KEY_SET_DATE :
      {
         int  month        = 0;
         int  day          = 0;
         int  year         = 0;

         char the_date[80] = "";
         int  first_int, second_int, third_int;

         if (strlen(return_keypad_buffer.keypad_return_string) != 8)
         {
            return;
         }

         char* nextToken  = 0;
         char* strtokbuff = return_keypad_buffer.keypad_return_string.make_buffer();
         first_int  = atoi(strtok_r(strtokbuff, "/", &nextToken));
         second_int = atoi(strtok_r(NULL, "/", &nextToken));
         third_int  = atoi(strtok_r(NULL, "/", &nextToken));
         GUI_POOL_FREE(strtokbuff);

         switch (guiglobs::cdsData.rwConfig.lang.key_date_format)
         {
            case MMDDYY_DATE : // mm/dd/yy
               month = first_int;
               day   = second_int;
               year  = third_int;
               break;

            case DDMMYY_DATE : // dd/mm/yy
               month = second_int;
               day   = first_int;
               year  = third_int;
               break;

            case YYMMDD_DATE : // yy/mm/dd
               month = second_int;
               day   = third_int;
               year  = first_int;
               break;

            default :
            {
               anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
               DataLog(log_level_gui_error) << "could not find the correct DATE FORMAT to display the current date" << endmsg;
               alarm.setAlarm();
               break;
            }
         }

         if (year >= 90 && year <=99)
            year += 1900;
         else
            year += 2000;

         if (month > 12
             || month < 1
             || day > 31
             || day < 1
             || year > 2037)
         {
            DataLog (log_level_gui_info) << "Entered date (shown here in MM/DD/YYYY format: ["
                                         << month << "/" << day << "/" << year
                                         << "]) is out of range." << endmsg;
            _cfgBtnSetDate.set_text_direct (10, "##/##/##");
            return;
         }

         month -= 1;
         year  -= 1900;

         // get the current date
         time_t timeRaw   = time(NULL);
         tm*    timeOfDay = localtime(&timeRaw);

         // change the day, month, year parts
         timeOfDay->tm_mday = day;
         timeOfDay->tm_mon  = month;
         timeOfDay->tm_year = year;

         // convert it back
         timeRaw = mktime(timeOfDay);

         // and send it
         trima_time::set(timeRaw);

         DataLog(log_level_gui_info) << the_date << endmsg;

         sprintf(eString, "Time of day: %d.%d.%d.%d.%d.%d"
                 , timeOfDay->tm_year
                 , timeOfDay->tm_mon
                 , timeOfDay->tm_mday
                 , timeOfDay->tm_hour
                 , timeOfDay->tm_min
                 , timeOfDay->tm_sec);

         DataLog(log_level_gui_info) << eString << endmsg;

         // get the current date again
         timeRaw   = time(NULL);
         timeOfDay = localtime(&timeRaw);

         int displayYear = timeOfDay->tm_year;
         if (displayYear > 100) displayYear -= 100;

         // display it
         switch (guiglobs::cdsData.rwConfig.lang.key_date_format)
         {
            case MMDDYY_DATE : // mm/dd/yy
               sprintf(the_date, "%02d/%02d/%02d",
                       (timeOfDay->tm_mon + 1),
                       timeOfDay->tm_mday,
                       displayYear);
               break;

            case DDMMYY_DATE : // dd/mm/yy
               sprintf(the_date, "%02d/%02d/%02d",
                       timeOfDay->tm_mday,
                       (timeOfDay->tm_mon + 1),
                       displayYear);
               break;

            case YYMMDD_DATE : // yy/mm/dd
               sprintf(the_date, "%02d/%02d/%02d",
                       displayYear,
                       (timeOfDay->tm_mon + 1),
                       timeOfDay->tm_mday);
               break;

            default :
            {
               anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
               alarm.setAlarm();
               DataLog(log_level_gui_error) << "could not find the correct DATE FORMAT to display the current date" << endmsg;
               break;
            }
         }

         _cfgBtnSetDate.set_text_direct (10, the_date);

         break;
      } // case KEY_SET_DATE

      default :
      {
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();
         DataLog(log_level_gui_error) << "could not find MACHINE keypad index upon return from keypad" << endmsg;
         break;
      }

   }  // end of switch

}  // end of method



//
///////////////////////////////////////////////////////////////////////////////
// Message handler for LISTBOXS
void Screen_CONFIG_MACHINE::update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer)
{
   switch (rtn_listbox_buffer.key_name)
   {
      case KEY_SET_AUDIO :
         _cfgBtnSetAudio.display_the_text_string (CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
         guiglobs::cdsData.rwConfig.machine.key_set_audio = rtn_listbox_buffer.key_value;
         break;

      case KEY_DATE_FORMAT :
         _cfgBtnDateFormat.display_the_text_string(CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
         guiglobs::cdsData.rwConfig.lang.key_date_format = rtn_listbox_buffer.key_value;
         _cfgBtnSetDate.redisplay();
         break;

      case KEY_TIME_FORMAT :
         _cfgBtnTimeFormat.display_the_text_string(CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
         guiglobs::cdsData.rwConfig.lang.key_time_format = rtn_listbox_buffer.key_value;
         _cfgBtnSetTime.redisplay();
         break;

      default :
      {
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();
         DataLog(log_level_gui_error) << "Could not find the MACHINE return key upon a return from the button list box" << endmsg;
         break;
      }

   } // end of switch
}    // end of update_screen_with_date method

//
///////////////////////////////////////////////////////////////////////////////
// setAddButtonIsActive()
// Activate / Deactiveate the Add Cassette Button
void Screen_CONFIG_MACHINE::setAddButtonIsActive (bool isActive)
{
   if (isActive)
   {
      _btnAddSet.set_disabled(false);
      _btnAddSet.set_up_bitmap(BITMAP_BUTTON_LARGE_BOT_TEAL_UP);
      _btnAddSet.set_button_text_color(YELLOW);
   }
   else
   {
      _btnAddSet.set_disabled(true);
      _btnAddSet.set_up_bitmap(BITMAP_BUTTON_LARGE_BOT_GREY_UP);
      _btnAddSet.set_button_text_color(GREY_LIGHT);
   }
}

//
///////////////////////////////////////////////////////////////////////////////
// setRemoveButtonIsActive()
// Activate / Deactiveate the Remove Cassette Button
void Screen_CONFIG_MACHINE::setRemoveButtonIsActive (bool isActive)
{
   if (isActive)
   {
      _btnRemoveSet.set_disabled(false);
      _btnRemoveSet.set_up_bitmap(BITMAP_BUTTON_LARGE_BOT_TEAL_UP);
      _btnRemoveSet.set_button_text_color(YELLOW);
   }
   else
   {
      _btnRemoveSet.set_disabled(true);
      _btnRemoveSet.set_up_bitmap(BITMAP_BUTTON_LARGE_BOT_GREY_UP);
      _btnRemoveSet.set_button_text_color(GREY_LIGHT);
   }
}

//
///////////////////////////////////////////////////////////////////////////////
// checkAddButtonStatus()
// Check the status of the cassette fields to determine
// the status of the add cassette button
void Screen_CONFIG_MACHINE::checkAddButtonStatus ()
{
   setAddButtonIsActive(_haveAdminCode && _haveCatalogNumber);
}

//
///////////////////////////////////////////////////////////////////////////////
// Here are some functions that deal with the status bar.
void Screen_CONFIG_MACHINE::displayStatusAttention (TEXT_STRING_ID warningMsg)
{
   guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, warningMsg);
}

void Screen_CONFIG_MACHINE::clearStatusAttention ()
{
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
}

void Screen_CONFIG_MACHINE::clearCassetteBox ()
{
   DataLog (log_level_gui_info) << "Clearing new cassette entry box." << endmsg;
   setAddButtonIsActive(false);
   _txtRefNum.set_string_id(textEmptyFormatCfgMachSetIdInit);
   _txtSetCode.set_string_id(textEmptyFormatCfgMachAdminCodeInit);
   _txtBarcode.set_string_id(textEmptyFormatCfgMachBarcodeInit);
   _catalogNumber     = "";
   _adminCode         = "";
   _barcodeString     = "";
   _haveCatalogNumber = false;
   _haveAdminCode     = false;
}

void Screen_CONFIG_MACHINE::setMessage (TEXT_STRING_ID stringID)
{
   DataLog(log_level_gui_info) << "Displaying num digits label." << endmsg;
   _txtNumDigits.set_string_id(stringID);
   _txtNumDigits.allocate_resources(*this);
   background_bitmap.move_to_back();
   background_bitmap.move_to_front();
   _bmpCassetteBoxBg.move_to_front();
}

void Screen_CONFIG_MACHINE::clearMessage ()
{
   DataLog(log_level_gui_info) << "Clearing num digits label." << endmsg;
   _txtNumDigits.deallocate_resources();
}


////////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_MACHINE::initListboxStrings ()
{
   _setIdAllocationString.initialize();
   _confirmBoxOneAllocationString.initialize();
   _confirmBoxAllAllocationString.initialize();

   _setIdAllocationString =
      Screen_SET_LISTBOX::createAllocationString(370, false, true);

   _confirmBoxOneAllocationString =
      Screen_CASSETTE_DELETE_CONFIRM_BOX::createAllocationString(KEY_MACH_CONFIRM_DELETE_ONE, 281, 121);
   _confirmBoxAllAllocationString =
      Screen_CASSETTE_DELETE_CONFIRM_BOX::createAllocationString(KEY_MACH_CONFIRM_DELETE_ALL, 281, 121);
}


void Screen_CONFIG_MACHINE::listboxCB ()
{
   DataLog (log_level_gui_info) << "Listbox Callback." << endmsg;
   Screen_SET_LISTBOX::LISTBOX_CALLBACK_VAL callbackVal =
      (Screen_SET_LISTBOX::LISTBOX_CALLBACK_VAL)_msgListboxData.getData();

   switch (callbackVal)
   {
      case Screen_SET_LISTBOX::DELETE_ALL :
      case Screen_SET_LISTBOX::DELETE_ONE :
         callConfirmDeletionBox(callbackVal);
         break;

      case Screen_SET_LISTBOX::CANCEL :
         guiglobs::cassette_mgr->clearCassetteToDelete();
         break;

      case Screen_SET_LISTBOX::EMPTY :
         break; // Do nothing.


      case Screen_SET_LISTBOX::SELECT :
      default :
         DataLog (log_level_gui_error) << "Error: bad set listbox callback value: " << callbackVal << endmsg;
         break;
   }
}

void Screen_CONFIG_MACHINE::callConfirmDeletionBox (Screen_SET_LISTBOX::LISTBOX_CALLBACK_VAL callbackVal)
{
   DataLog (log_level_gui_info) << "Call the confirm deletion box." << endmsg;
   if (callbackVal == Screen_SET_LISTBOX::DELETE_ONE)
   {
      DataLog (log_level_gui_info) << "Putting up a confirmation message." << endmsg;
      goto_child_screen(GUI_SCREEN_CONFIRM_BOX, _confirmBoxOneAllocationString);
   }
   else if (callbackVal == Screen_SET_LISTBOX::DELETE_ALL)
   {
      DataLog (log_level_gui_info) << "Putting up a confirmation message." << endmsg;
      goto_child_screen(GUI_SCREEN_CONFIRM_BOX, _confirmBoxAllAllocationString);
   }
}


void Screen_CONFIG_MACHINE::confirmCB ()
{
   DataLog (log_level_gui_info) << "Listbox Callback." << endmsg;
   CONFIG_DATA_KEYS dataKey = (CONFIG_DATA_KEYS)_msgDeletionConfirmed.getData();

   switch (dataKey)
   {
      case KEY_MACH_CONFIRM_DELETE_ONE :
         DataLog (log_level_gui_info) << "Delete one confirmed." << endmsg;
         deleteOne();
         break;

      case KEY_MACH_CONFIRM_DELETE_ALL :
         DataLog (log_level_gui_info) << "Delete all confirmed." << endmsg;
         deleteAll();
         break;

      default :
         DataLog (log_level_gui_error) << "Error: bad data key: " << dataKey << endmsg;
         break;
   }
}

void Screen_CONFIG_MACHINE::deleteAll ()
{
   DataLog (log_level_gui_info) << "Deleting all." << endmsg;

   guiglobs::cassette_mgr->DeleteAllCassettes();

   setRemoveButtonIsActive(false);
}

void Screen_CONFIG_MACHINE::deleteOne ()
{
   guiglobs::cassette_mgr->ConfirmDeleteOne();
   setRemoveButtonIsActive(guiglobs::cassette_mgr->numSets() > 0);
}

void Screen_CONFIG_MACHINE::HandleBarcodeMsg ()
{
   BADGE_BARCODE_MSG msg = _msgBarcode.getData();
   clearStatusAttention();

   // Terminate the string

   msg.badgeString[msg.badgeLength] = 0;

   DataLog(log_level_gui_cfr11_info) << "REF number barcode " << msg.badgeString << "(" << msg.badgeLength << " characters)" << endmsg;

   sprintf(_barcodeString, msg.badgeString);

   // Save badge string
   guistring printString;
   printString.initialize();

   sprintf(printString, msg.badgeString);
   printString.strip_unprintable_chars();

   if (printString.length() > __MAX_BARCODE_PRINT_LENGTH__)
   {
      DataLog (log_level_gui_info) << "Barcode exceeds max size.  Truncating before printing." << endmsg;
      printString.truncate(__MAX_BARCODE_PRINT_LENGTH__);
   }

   _txtBarcode.set_text_direct(printString.length(), printString);
}


void Screen_CONFIG_MACHINE::checkForTimeOnlyConfig ()
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_info) << "Buttons should be disabled." << endmsg;

      disableConfigBtn(_cfgBtnDateFormat);
      disableConfigBtn(_cfgBtnTimeFormat);
      disableConfigBtn(_cfgBtnSetAudio);
      // disableConfigBtn(_cfgBtnSetDate);
      disableConfigBtn(_btnRefNum);
      disableConfigBtn(_btnSetCode);
      disableConfigBtn(_btnAddSet, BITMAP_BUTTON_LARGE_BOT_GREY_UP);
      disableConfigBtn(_btnRemoveSet, BITMAP_BUTTON_LARGE_BOT_GREY_UP);

      disableConfigTxt(_txtBarcodeLbl);
   }
}


//
///////////////////////////////////////////////////////////////////
// end of cfg_mach.cpp file

/* FORMAT HASH 5768898c22de978a21b449c2858e2306 */

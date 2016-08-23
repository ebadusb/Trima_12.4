/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael J Cobb

*     Class name:  N/A  keypad declarations and attributes table

*     File name:   keypadtb.cpp

*     Contents:    keypad declarations, attributes table,

*     Description:

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/keypadtb.cpp 1.17 2008/07/09 23:50:51Z dslausb Exp $
$Log: keypadtb.cpp $
Revision 1.17  2008/07/09 23:50:51Z  dslausb
More string ID standardization as requested by Marita
Revision 1.16  2008/07/07 18:26:18Z  dslausb
More localization string id standardization
Revision 1.15  2008/04/24 16:56:14Z  dslausb
IT 8590 - Configureable HGB units
Revision 1.14  2006/05/30 21:49:31Z  dslausb
Moved keypad on cassette entry screen.
Revision 1.13  2006/05/12 23:15:30Z  dslausb
Changed set ID to 5 & 6 digit codes (was 3 & 5).
Revision 1.12  2006/04/10 23:07:06Z  dslausb
IT 7272 - Fixed many issues with set identification, added keypad set entry page before dispload
Revision 1.11  2004/01/30 15:08:03Z  rm70006
IT 6566.  Fixed bug with not allowing a leading zero for the security keypad.
Revision 1.10  2003/11/14 20:09:03Z  rm70006
IT 6360.  Add capability for security keypad for CFR 11.
Revision 1.9  2003/07/18 20:03:52Z  jl11312
- updated power response defines
- removed references to safproc.hpp
Revision 1.8  2002/08/16 16:34:34Z  sb07663
added <vxworks.h> include
Revision 1.7  2002/05/07 13:58:10  sb07663
First cut at VxWorks compatibility
Revision 1.6  2000/02/28 23:21:07  SB07663
IT4604: include directive changes for safproc.hpp
Revision 1.5  1997/05/29 00:14:40  MC03015
Modification of max and min management to float
Revision 1.4  1997/05/27 16:42:14  MC03015
Modifications to allow for YSF float value and a redefinition of text entry max and min
Revision 1.3  1997/03/17 17:48:48  MC03015
Bitmap location modifications for beta 2.5F
Revision 1.2  1997/03/15 02:27:09  MC03015
Addition of new keypad table and parsing logic

*/
#include <vxworks.h>

// INCLUDE the GUI MANIFESTS
#include "gui.h"
#include "keypad.h"
#include "safproc.h"
#include "run_defs.h"


KEYPAD_TABLE keypads [] =
{


// *********************************************************************************
// the following is the keypad table structure to be used in defining the individual
// keypads and there components
// {
// typedef struct
// {
//   KEYPAD_IDS                keypad_name,
//   float                     keypad_max_value,
//   float                     keypad_min_value,
//   short                     keypad_location_x,
//   short                     keypad_location_y,
//   char                      keypad_format_1[STRING_BUFFER_1C],
//   char                      keypad_format_2[STRING_BUFFER_1C],
//   float                     keypad_initial_value_1,
//   float                     keypad_initial_value_2,
//   int                       keypad_exponent,
//   int                       keypad_zero_ok,
//   KEYPAD_ENTRY_DIRECTION    keypad_entry_type;
//   CONFIG_DATA_KEYS          keypad_data_type;
//   TEXT_STRING_ID            keypad_units_text_id_1;
//   TEXT_STRING_ID            keypad_units_text_id_2;
//   TEXT_STRING_ID            keypad_button_enter_text_id,
//   TEXT_STRING_ID            keypad_button_0_text_id,
//   TEXT_STRING_ID            keypad_button_1_text_id,
//   TEXT_STRING_ID            keypad_button_2_text_id,
//   TEXT_STRING_ID            keypad_button_3_text_id,
//   TEXT_STRING_ID            keypad_button_4_text_id,
//   TEXT_STRING_ID            keypad_button_5_text_id,
//   TEXT_STRING_ID            keypad_button_6_text_id,
//   TEXT_STRING_ID            keypad_button_7_text_id,
//   TEXT_STRING_ID            keypad_button_8_text_id,
//   TEXT_STRING_ID            keypad_button_9_text_id,
//   TEXT_STRING_ID            keypad_button_clear_text_id,
// } KEYPAD_TABLE;
//
// the folowing is an enumeration of all the keypad types
// enum KEYPAD_IDS
// {
//    STANDARD,
//    STANDARD_FLOAT,
//    HEIGHT_US,
//    HEIGHT_CM,
//    WEIGHT_LB,
//    WEIGHT_KG,
//    BLOOD_TYPE,
//    HEMATOCRIT,
//    PLATELET_PRECOUNT,
//    DONOR_TIME,
//    SECURITY,
//    CASSETTE_CAT_NUM,
//    NULL_KEYPAD
// };


   {
      STANDARD,
      0.0,
      0.0,
      0,
      0,
      "###",
      "",
      0.0,
      0.0,
      0,
      FALSE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textEmpty,                    // " "
      textEmpty,                    // " "
      textButtonKeypadClear,        // "C"
      textButtonKeypad9,            // "9"
      textButtonKeypad8,            // "8"
      textButtonKeypad7,            // "7"
      textButtonKeypad6,            // "6"
      textButtonKeypad5,            // "5"
      textButtonKeypad4,            // "4"
      textButtonKeypad3,            // "3"
      textButtonKeypad2,            // "2"
      textButtonKeypad1,            // "1"
      textButtonKeypad0,            // "0"
      textButtonKeypadEnter,        // "enter"
      0,                            // Security option
   },

   {
      STANDARD_FLOAT,
      0.0,
      0.0,
      0,
      0,
      "#.#",
      "",
      0.0,
      0.0,
      0,
      FALSE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textEmpty,                    // " "
      textEmpty,                    // " "
      textButtonKeypadClear,        // "C"
      textButtonKeypad9,            // "9"
      textButtonKeypad8,            // "8"
      textButtonKeypad7,            // "7"
      textButtonKeypad6,            // "6"
      textButtonKeypad5,            // "5"
      textButtonKeypad4,            // "4"
      textButtonKeypad3,            // "3"
      textButtonKeypad2,            // "2"
      textButtonKeypad1,            // "1"
      textButtonKeypad0,            // "0"
      textButtonKeypadEnter,        // "enter"
      0,                            // Security option
   },

   {
      HEIGHT_US,
      MAX_ALLOWABLE_HEIGHT_IN,
      MIN_ALLOWABLE_HEIGHT_IN,
      229,
      169,
      "##",
      "#",
      0.0,
      0.0,
      0,
      FALSE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textKeypadUnitsInches, // "in"
      textKeypadUnitsFeet,   // "ft"
      textButtonKeypadClear, // "C"
      textButtonKeypad9,     // "9"
      textButtonKeypad8,     // "8"
      textButtonKeypad7,     // "7"
      textButtonKeypad6,     // "6"
      textButtonKeypad5,     // "5"
      textButtonKeypad4,     // "4"
      textButtonKeypad3,     // "3"
      textButtonKeypad2,     // "2"
      textButtonKeypad1,     // "1"
      textButtonKeypad0,     // "0"
      textButtonKeypadEnter, // "enter"
      0,                     // Security option
   },

   {
      HEIGHT_CM,
      MAX_ALLOWABLE_HEIGHT_CM,
      MIN_ALLOWABLE_HEIGHT_CM,
      229,
      169,
      "###",
      "",
      0.0,
      0.0,
      0,
      FALSE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textKeypadUnitsCm,     // "cm"
      textEmpty,             // " "
      textButtonKeypadClear, // "C"
      textButtonKeypad9,     // "9"
      textButtonKeypad8,     // "8"
      textButtonKeypad7,     // "7"
      textButtonKeypad6,     // "6"
      textButtonKeypad5,     // "5"
      textButtonKeypad4,     // "4"
      textButtonKeypad3,     // "3"
      textButtonKeypad2,     // "2"
      textButtonKeypad1,     // "1"
      textButtonKeypad0,     // "0"
      textButtonKeypadEnter, // "enter"
      0,                     // Security option
   },

   {
      WEIGHT_LB,
      MAX_ALLOWABLE_WEIGHT_LB,
      MIN_ALLOWABLE_WEIGHT_LB,
      413,
      169,
      "###",
      "",
      0.0,
      0.0,
      0,
      FALSE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textKeypadUnitsLbs,     // "lbs"
      textEmpty,              // " "
      textButtonKeypadClear,  // "C"
      textButtonKeypad9,      // "9"
      textButtonKeypad8,      // "8"
      textButtonKeypad7,      // "7"
      textButtonKeypad6,      // "6"
      textButtonKeypad5,      // "5"
      textButtonKeypad4,      // "4"
      textButtonKeypad3,      // "3"
      textButtonKeypad2,      // "2"
      textButtonKeypad1,      // "1"
      textButtonKeypad0,      // "0"
      textButtonKeypadEnter,  // "enter"
      0,                      // Security option
   },

   {
      WEIGHT_KG,
      MAX_ALLOWABLE_WEIGHT_KG,
      MIN_ALLOWABLE_WEIGHT_KG,
      413,
      169,
      "###",
      "",
      0.0,
      0.0,
      0,
      FALSE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textKeypadUnitsKg,        // "kg"
      textEmpty,                // " "
      textButtonKeypadClear,    // "C"
      textButtonKeypad9,        // "9"
      textButtonKeypad8,        // "8"
      textButtonKeypad7,        // "7"
      textButtonKeypad6,        // "6"
      textButtonKeypad5,        // "5"
      textButtonKeypad4,        // "4"
      textButtonKeypad3,        // "3"
      textButtonKeypad2,        // "2"
      textButtonKeypad1,        // "1"
      textButtonKeypad0,        // "0"
      textButtonKeypadEnter,    // "enter"
      0,                        // Security option
   },

   {
      BLOOD_TYPE,
      TEXT_KEYPAD,
      TEXT_KEYPAD,
      45,
      169,
      "###",
      "",
      0.0,
      0.0,
      0,
      TRUE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textEmpty,                      // " "
      textEmpty,                      // " "
      textButtonKeypadClear,          // "C"
      textButtonKeypadBloodTypeAbPos, // "AB+"
      textButtonKeypadBloodTypeBPos,  // "B+"
      textButtonKeypadBloodTypeAPos,  // "A+"
      textButtonKeypadBloodTypeAbNeg, // "AB-"
      textButtonKeypadBloodTypeBNeg,  // "B-"
      textButtonKeypadBloodTypeANeg,  // "A-"
      TEXT_NULL,                      // ""
      TEXT_NULL,                      // ""
      textButtonKeypadBloodTypeOPos,  // "O+"
      textButtonKeypadBloodTypeONeg,  // "O-"
      textButtonKeypadEnter,          // "enter"
      0,                              // Security option
   },

   {
      HEMATOCRIT,
      MAX_ALLOWABLE_HCT,
      MIN_ALLOWABLE_HCT,
      229,
      169,
      "##",
      "",
      0.0,
      0.0,
      0,
      FALSE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textKeypadUnitsCrit,    // "%"
      textEmpty,              // " "
      textButtonKeypadClear,  // "C"
      textButtonKeypad9,      // "9"
      textButtonKeypad8,      // "8"
      textButtonKeypad7,      // "7"
      textButtonKeypad6,      // "6"
      textButtonKeypad5,      // "5"
      textButtonKeypad4,      // "4"
      textButtonKeypad3,      // "3"
      textButtonKeypad2,      // "2"
      textButtonKeypad1,      // "1"
      textButtonKeypad0,      // "0"
      textButtonKeypadEnter,  // "enter"
      0,                      // Security option
   },


   {
      HEMOGLOBIN,
      MAX_ALLOWABLE_HGB,
      MIN_ALLOWABLE_HGB,
      229,
      169,
      "##.#",
      "",
      0.0,
      0.0,
      0,
      FALSE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textKeypadUnitsHemoglobin,    // "%"
      textEmpty,                    // " "
      textButtonKeypadClear,        // "C"
      textButtonKeypad9,            // "9"
      textButtonKeypad8,            // "8"
      textButtonKeypad7,            // "7"
      textButtonKeypad6,            // "6"
      textButtonKeypad5,            // "5"
      textButtonKeypad4,            // "4"
      textButtonKeypad3,            // "3"
      textButtonKeypad2,            // "2"
      textButtonKeypad1,            // "1"
      textButtonKeypad0,            // "0"
      textButtonKeypadEnter,        // "enter"
      0,                            // Security option
   },

   {
      PLATELET_PRECOUNT,
      MAX_ALLOWABLE_PRECOUNT,
      MIN_ALLOWABLE_PRECOUNT,
      413,
      169,
      "###",
      "",
      0.0,
      0.0,
      0,
      FALSE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textKeypadUnitsPltPrecount, // "x1000"
      textEmpty,                  // " "
      textButtonKeypadClear,      // "C"
      textButtonKeypad9,          // "9"
      textButtonKeypad8,          // "8"
      textButtonKeypad7,          // "7"
      textButtonKeypad6,          // "6"
      textButtonKeypad5,          // "5"
      textButtonKeypad4,          // "4"
      textButtonKeypad3,          // "3"
      textButtonKeypad2,          // "2"
      textButtonKeypad1,          // "1"
      textButtonKeypad0,          // "0"
      textButtonKeypadEnter,      // "enter"
      0,                          // Security option
   },

   {
      DONOR_TIME,
      ABSOLUTE_MAXIMUM_PROCEDURE_TIME,
      MINIMUM_PROCEDURE_TIME,
      417,
      166,
      "###",
      "",
      0.0,
      0.0,
      0,
      FALSE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textKeypadUnitsTime,          // "min"
      textEmpty,                    // " "
      textButtonKeypadClear,        // "C"
      textButtonKeypad9,            // "9"
      textButtonKeypad8,            // "8"
      textButtonKeypad7,            // "7"
      textButtonKeypad6,            // "6"
      textButtonKeypad5,            // "5"
      textButtonKeypad4,            // "4"
      textButtonKeypad3,            // "3"
      textButtonKeypad2,            // "2"
      textButtonKeypad1,            // "1"
      textButtonKeypad0,            // "0"
      textButtonKeypadEnter,        // "enter"
      0,                            // Security option
   },

   {
      CASSETTE_CAT_NUM,
      999999,                        // Biggest PIN
      0,                             // Smallest PIN
      231,                           // X Coord
      170,                           // Y Coord
      "######",
      "",
      0.0,
      0.0,
      0,
      TRUE,
      RIGHTTOLEFT,
      KEY_CASSETTE_LOT_ID,
      textEmpty,                    // " "
      textEmpty,                    // " "
      textButtonKeypadClear,        // "C"
      textButtonKeypad9,            // "9"
      textButtonKeypad8,            // "8"
      textButtonKeypad7,            // "7"
      textButtonKeypad6,            // "6"
      textButtonKeypad5,            // "5"
      textButtonKeypad4,            // "4"
      textButtonKeypad3,            // "3"
      textButtonKeypad2,            // "2"
      textButtonKeypad1,            // "1"
      textButtonKeypad0,            // "0"
      textButtonKeypadEnter,        // "enter"
      0,                            // Security option
   },

   {
      SECURITY,
      999999,                        // Biggest PIN
      0,                             // Smallest PIN
      224,                           // X Coord
      180,                           // Y Coord
      "######",
      "",
      0.0,
      0.0,
      0,
      TRUE,
      RIGHTTOLEFT,
      (CONFIG_DATA_KEYS)0,
      textEmpty,                    // " "
      textEmpty,                    // " "
      textButtonKeypadClear,        // "C"
      textButtonKeypad9,            // "9"
      textButtonKeypad8,            // "8"
      textButtonKeypad7,            // "7"
      textButtonKeypad6,            // "6"
      textButtonKeypad5,            // "5"
      textButtonKeypad4,            // "4"
      textButtonKeypad3,            // "3"
      textButtonKeypad2,            // "2"
      textButtonKeypad1,            // "1"
      textButtonKeypad0,            // "0"
      textButtonKeypadEnter,        // "enter"
      1,                            // Security option
   },

   {
      NULL_KEYPAD,
   }

}; // This is the end of the keypad table declaration

/* FORMAT HASH b6ed48632b89a6044b7a00f92721e47f */

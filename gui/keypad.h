/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Michael J. Cobb

*     Class name:   N/A

*     File name:    keypad.h

*     Contents:     keypad relevant #defines / enums

*     Description:

*************************< FOCUSSED GUI DOMAIN >*****************************
$HEADER$
$LOG$

*/

// Single include define guardian
#ifndef KEYPAD_H
#define KEYPAD_H

#include  "cfg_ids.h"  // added by Barry for access to the CONFIG_DATA_KEYS enum list

#include  "guiproc.h"

// string definitions and methods
#include  "frmtstr.hpp"

// #defines for standard string compares (i.e. feet and inches), and keypad text
// widget location

#define      ZERO         '0'  // zero char for comparison
#define      ONE          '1'  // one char for comparison

#define      TEXT_KEYPAD  -1   // int for text keypad max and min comparison

// #defines for standard text locationsinitialization compares to determine if the allocation parameter
// populated the element

#define      KEYPAD_TEXT1_SIZE_W      106
#define      KEYPAD_TEXT1_SIZE_H       27
#define      KEYPAD_TEXT1_POS_X        61
#define      KEYPAD_TEXT1_POS_Y        12

#define      KEYPAD_TEXT2_SIZE_W       56
#define      KEYPAD_TEXT2_SIZE_H       27
#define      KEYPAD_TEXT2_POS_X        61
#define      KEYPAD_TEXT2_POS_Y        12

#define      KEYPAD_TEXT_EXP_SIZE_W    14
#define      KEYPAD_TEXT_EXP_SIZE_H    12
#define      KEYPAD_TEXT_EXP_POS_X    151
#define      KEYPAD_TEXT_EXP_POS_Y     12


////////////////////////////////////////////////////////////////////////////////
// These are the logical ID's for the Keypad types

enum KEYPAD_IDS // if you add an ID before SECURITY you need to also change cfr11_screen
{
   STANDARD,
   STANDARD_FLOAT,
   HEIGHT_US,
   HEIGHT_CM,
   WEIGHT_LB,
   WEIGHT_KG,
   BLOOD_TYPE,
   HEMATOCRIT,
   HEMOGLOBIN,
   PLATELET_PRECOUNT,
   DONOR_TIME,
   SECURITY, // note enumeration index is hardcoded in cfr11_screen.cpp
   CASSETTE_CAT_NUM,
   NULL_KEYPAD
};


////////////////////////////////////////////////////////////////////////////////
// These are the entry protocol directions for the keypad types

enum KEYPAD_ENTRY_DIRECTION
{
   RIGHTTOLEFT,
   LEFTTORIGHT,
   NULL_ENTRY_TYPE
};


////////////////////////////////////////////////////////////////////////////////
// The following strucure defines all the necessary elements to display all of
// the keypad bitmaps and text

struct KEYPAD_TABLE
{
   KEYPAD_IDS             keypad_name;
   double                 keypad_max_value;
   double                 keypad_min_value;
   short                  keypad_location_x;
   short                  keypad_location_y;
   guistring              keypad_format_1;
   guistring              keypad_format_2;
   double                 keypad_initial_value_1;
   double                 keypad_initial_value_2;
   int                    keypad_exponent;
   int                    keypad_zero_ok;
   KEYPAD_ENTRY_DIRECTION keypad_entry_type;
   CONFIG_DATA_KEYS       keypad_data_type;
   TEXT_STRING_ID         keypad_units_text_id_1;
   TEXT_STRING_ID         keypad_units_text_id_2;
   TEXT_STRING_ID         keypad_button_clear_text_id;
   TEXT_STRING_ID         keypad_button_9_text_id;
   TEXT_STRING_ID         keypad_button_8_text_id;
   TEXT_STRING_ID         keypad_button_7_text_id;
   TEXT_STRING_ID         keypad_button_6_text_id;
   TEXT_STRING_ID         keypad_button_5_text_id;
   TEXT_STRING_ID         keypad_button_4_text_id;
   TEXT_STRING_ID         keypad_button_3_text_id;
   TEXT_STRING_ID         keypad_button_2_text_id;
   TEXT_STRING_ID         keypad_button_1_text_id;
   TEXT_STRING_ID         keypad_button_0_text_id;
   TEXT_STRING_ID         keypad_button_enter_text_id;
   int                    keypad_hide_input;
};


////////////////////////////////////////////////////////////////////////////////
// This is the struct definition responsible for all data transferred back to
// the keypad from the screen that requested it.

struct KEYPAD_RETURN_STRUCT
{
   KEYPAD_IDS       keypad_type;
   CONFIG_DATA_KEYS config_data_type;
   guistring        keypad_return_string;
   float            keypad_return_value_float;
   int              keypad_return_value;
};


////////////////////////////////////////////////////////////////////////////////
// End of single include define guardian
#endif

/* FORMAT HASH 215064c2fb3d1d9aabbae8a604164cad */

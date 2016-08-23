/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  cfg_priority.cpp

*     File name:   Screen_CONFIG_PRIORITY

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
*/
#include <vxworks.h>

/** include files **/

// BASE CLASS INCLUDE
#include "cfg_priority.hpp"

// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "an_alarm.h"
#include "software_cds.h"

#define PP_BUTTON_LABEL_BOX(row) ((row==0) ? (GUI_BUTTON_CONFIG_PP_LABEL_BOX_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PP_LABEL_BOX_2) : (GUI_BUTTON_CONFIG_PP_LABEL_BOX_3)))
#define PP_BUTTON_LABEL_LINE(row) ((row==0) ? (GUI_BUTTON_CONFIG_PP_LABEL_LINE_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PP_LABEL_LINE_2) : (GUI_BUTTON_CONFIG_PP_LABEL_LINE_3)))
#define PP_BUTTON_PLATELETS(row) ((row==0) ? (GUI_BUTTON_CONFIG_PP_PLATELETS_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PP_PLATELETS_2) : (GUI_BUTTON_CONFIG_PP_PLATELETS_3)))
#define PP_BUTTON_PLASMA(row) ((row==0) ? (GUI_BUTTON_CONFIG_PP_PLASMA_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PP_PLASMA_2) : (GUI_BUTTON_CONFIG_PP_PLASMA_3)))
#define PP_BUTTON_RBC(row) ((row==0) ? (GUI_BUTTON_CONFIG_PP_RBC_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PP_RBC_2) : (GUI_BUTTON_CONFIG_PP_RBC_3)))
#define PP_BUTTON_TYPE(row) ((row==0) ? (GUI_BUTTON_CONFIG_PP_TYPE_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PP_TYPE_2) : (GUI_BUTTON_CONFIG_PP_TYPE_3)))
#define PP_BUTTON_TIME(row) ((row==0) ? (GUI_BUTTON_CONFIG_PP_TIME_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PP_TIME_2) : (GUI_BUTTON_CONFIG_PP_TIME_3)))



#define PRIO_ROW_Y(__ROW_NUM__)  187 + ((__ROW_NUM__) * 60)
#define PRIO_LINE_ROW_Y(__ROW_NUM__)  238 + ((__ROW_NUM__) * 60)
#define BITMAP_CFGPP_PROC_BOX_X 8
#define BITMAP_CFGPP_PROC_LINE_X 49
#define __PLT_LISTBOX_TXT_OFFSET__ 6

static void cb_button (void* data);
static void cb_up_priority_button (void* data);
static void cb_down_priority_button (void* data);


/** defines **/
static const int PLATELET_COLUMN      = BITMAP_BUTTON_CFGPP_COL1_X;
static const int PLASMA_COLUMN        = BITMAP_BUTTON_CFGPP_COL2_X;
static const int RBC_COLUMN           = BITMAP_BUTTON_CFGPP_COL3_X;
static const int BLOOD_TYPE_COLUMN    = BITMAP_BUTTON_CFGPP_COL4_X;
static const int TIME_COLUMN          = BITMAP_BUTTON_CFGPP_COL5_X;

static const int LABEL_AREA           = 125;

static const int PROCEDURE_INDICTOR_X = 300;
static const int PROCEDURE_INDICTOR_Y = 40;
static const int PR_KEYPAD_Y          = 150;


///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_CONFIG_PRIORITY::Screen_CONFIG_PRIORITY()
   : Screen_Config(TEAL, GUI_SCREEN_CONFIG_PRIORITY, textCfgScreenLabelPrio),

     plate_label          (textCfgColLabelCfgPrioPlt),
     plasm_label          (textCfgColLabelCfgPrioPls),
     rbc_label            (textCfgColLabelCfgPrioRbc),
     time_label           (textCfgColLabelCfgPrioTime),
     blood_type_label     (textCfgColLabelCfgPrioBloodType),
     procedure_indicator  (textEmptyFormatCfgNumericTextFormatNull),

     up_button   (NULL),

     gray_up_button (GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON,
                     BITMAP_BUTTON_CFGPP_UP_ARROW_X,
                     BITMAP_BUTTON_CFGPP_UP_ARROW_Y,
                     textEmpty,
                     BITMAP_CFGPP_BTN_UP_GREY,
                     BITMAP_CFGPP_BTN_UP_GREY),
     down_button (NULL),

     gray_dn_button (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                     BITMAP_BUTTON_CFGPP_DN_ARROW_X,
                     BITMAP_BUTTON_CFGPP_DN_ARROW_Y,
                     textEmpty,
                     BITMAP_CFGPP_BTN_DOWN_GREY,
                     BITMAP_CFGPP_BTN_DOWN_GREY),

     _top_procedure(0),
     _callback_type(PLATELET),
     _callback_procedure(0)
{
   for (int i = 0; i < MAX_ROWS; i++)
   {
      row_info[i].procedure_row_box.x_pos       = BITMAP_CFGPP_PROC_BOX_X;
      row_info[i].procedure_row_box.y_pos       = PRIO_ROW_Y(i);
      row_info[i].procedure_row_box.button_ptr  = NULL;
      row_info[i].procedure_row_box.bitmap      = NULL;
      row_info[i].procedure_row_box.parent      = this;
      row_info[i].procedure_row_box.btn_type    = INACTIVE;
      row_info[i].procedure_row_box.row         = i;

      row_info[i].procedure_row_line.x_pos      = BITMAP_CFGPP_PROC_LINE_X;
      row_info[i].procedure_row_line.y_pos      = PRIO_LINE_ROW_Y(i);
      row_info[i].procedure_row_line.button_ptr = NULL;
      row_info[i].procedure_row_line.bitmap     = NULL;
      row_info[i].procedure_row_line.parent     = this;
      row_info[i].procedure_row_line.btn_type   = INACTIVE;
      row_info[i].procedure_row_line.row        = i;

      row_info[i].platelet_button.x_pos         = PLATELET_COLUMN;
      row_info[i].platelet_button.y_pos         = PRIO_ROW_Y(i);
      row_info[i].platelet_button.button_ptr    = NULL;
      row_info[i].platelet_button.bitmap        = NULL;
      row_info[i].platelet_button.parent        = this;
      row_info[i].platelet_button.btn_type      = PLATELET;
      row_info[i].platelet_button.row           = i;

      row_info[i].yieldValue.set_string_id(textEmptyFormatPltListboxYieldInit);
      row_info[i].yieldx10.set_string_id(textButtonCfgPrioExtraPltX10);
      row_info[i].yieldtoThe11th.set_string_id(textButtonCfgPrioExtraPltExponent);

      row_info[i].plasma_button.x_pos      = PLASMA_COLUMN;
      row_info[i].plasma_button.y_pos      = PRIO_ROW_Y(i);
      row_info[i].plasma_button.button_ptr = NULL;
      row_info[i].plasma_button.bitmap     = NULL;
      row_info[i].plasma_button.parent     = this;
      row_info[i].plasma_button.btn_type   = PLASMA;
      row_info[i].plasma_button.row        = i;

      row_info[i].volValue.set_string_id(textEmptyFormatPlasListboxVolInit);
      row_info[i].volUnits.set_string_id(textButtonCfgPrioExtraPlasUnits);

      row_info[i].rbc_button.x_pos      = RBC_COLUMN;
      row_info[i].rbc_button.y_pos      = PRIO_ROW_Y(i);
      row_info[i].rbc_button.button_ptr = NULL;
      row_info[i].rbc_button.bitmap     = NULL;
      row_info[i].rbc_button.parent     = this;
      row_info[i].rbc_button.btn_type   = RBC;
      row_info[i].rbc_button.row        = i;

      row_info[i].doseValue.set_string_id(textEmptyFormatRbcListboxDoseInit);
      row_info[i].doseUnits.set_string_id(textButtonCfgPrioExtraDoseUnits);

      row_info[i].blood_type_button.x_pos      = BLOOD_TYPE_COLUMN;
      row_info[i].blood_type_button.y_pos      = PRIO_ROW_Y(i);
      row_info[i].blood_type_button.button_ptr = NULL;
      row_info[i].blood_type_button.bitmap     = NULL;
      row_info[i].blood_type_button.parent     = this;
      row_info[i].blood_type_button.btn_type   = BLOOD_TYPE;
      row_info[i].blood_type_button.row        = i;

      row_info[i].time_button.x_pos            = TIME_COLUMN;
      row_info[i].time_button.y_pos            = PRIO_ROW_Y(i);
      row_info[i].time_button.button_ptr       = NULL;
      row_info[i].time_button.bitmap           = NULL;
      row_info[i].time_button.parent           = this;
      row_info[i].time_button.btn_type         = TIME;
      row_info[i].time_button.row              = i;
   }

   const bool amapPlatelets = guiglobs::amapPlatelets();

   for (int i = 0; i < MAX_PROCEDURES; i++)
   {
      data_info[i].platelet_value = (amapPlatelets ?
                                     &(guiglobs::cdsData.rwConfig.definition[i].key_platelet_amap)
                                     : &(guiglobs::cdsData.rwConfig.definition[i].key_platelet));
      data_info[i].plasma_value     = &(guiglobs::cdsData.rwConfig.definition[i].key_plasma);
      data_info[i].rbc_value        = &(guiglobs::cdsData.rwConfig.definition[i].key_rbc);
      data_info[i].blood_type_value = &(guiglobs::cdsData.rwConfig.definition[i].key_blood_type);
      data_info[i].time_value       = &(guiglobs::cdsData.rwConfig.definition[i].key_time);
   }


   // Data information
   data_info[0].procedure_label  = textCfgRowLabelCfgPrio1;
   data_info[0].platelet_key     = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_A : KEY_PRI_PLATELET_A);
   data_info[0].plasma_key       = KEY_PRI_PLASMA_A;
   data_info[0].rbc_key          = KEY_PRI_RBC_A;
   data_info[0].blood_type_key   = KEY_PRI_BLOOD_TYPE_A;
   data_info[0].time_key         = KEY_PRI_TIME_A;

   data_info[1].procedure_label  = textCfgRowLabelCfgPrio2;
   data_info[1].platelet_key     = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_B : KEY_PRI_PLATELET_B);
   data_info[1].plasma_key       = KEY_PRI_PLASMA_B;
   data_info[1].rbc_key          = KEY_PRI_RBC_B;
   data_info[1].blood_type_key   = KEY_PRI_BLOOD_TYPE_B;
   data_info[1].time_key         = KEY_PRI_TIME_B;

   data_info[2].procedure_label  = textCfgRowLabelCfgPrio3;
   data_info[2].platelet_key     = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_C : KEY_PRI_PLATELET_C);
   data_info[2].plasma_key       = KEY_PRI_PLASMA_C;
   data_info[2].rbc_key          = KEY_PRI_RBC_C;
   data_info[2].blood_type_key   = KEY_PRI_BLOOD_TYPE_C;
   data_info[2].time_key         = KEY_PRI_TIME_C;

   data_info[3].procedure_label  = textCfgRowLabelCfgPrio4;
   data_info[3].platelet_key     = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_D : KEY_PRI_PLATELET_D);
   data_info[3].plasma_key       = KEY_PRI_PLASMA_D;
   data_info[3].rbc_key          = KEY_PRI_RBC_D;
   data_info[3].blood_type_key   = KEY_PRI_BLOOD_TYPE_D;
   data_info[3].time_key         = KEY_PRI_TIME_D;

   data_info[4].procedure_label  = textCfgRowLabelCfgPrio5;
   data_info[4].platelet_key     = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_E : KEY_PRI_PLATELET_E);
   data_info[4].plasma_key       = KEY_PRI_PLASMA_E;
   data_info[4].rbc_key          = KEY_PRI_RBC_E;
   data_info[4].blood_type_key   = KEY_PRI_BLOOD_TYPE_E;
   data_info[4].time_key         = KEY_PRI_TIME_E;

   data_info[5].procedure_label  = textCfgRowLabelCfgPrio6;
   data_info[5].platelet_key     = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_F : KEY_PRI_PLATELET_F);
   data_info[5].plasma_key       = KEY_PRI_PLASMA_F;
   data_info[5].rbc_key          = KEY_PRI_RBC_F;
   data_info[5].blood_type_key   = KEY_PRI_BLOOD_TYPE_F;
   data_info[5].time_key         = KEY_PRI_TIME_F;

   data_info[6].procedure_label  = textCfgRowLabelCfgPrio7;
   data_info[6].platelet_key     = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_G : KEY_PRI_PLATELET_G);
   data_info[6].plasma_key       = KEY_PRI_PLASMA_G;
   data_info[6].rbc_key          = KEY_PRI_RBC_G;
   data_info[6].blood_type_key   = KEY_PRI_BLOOD_TYPE_G;
   data_info[6].time_key         = KEY_PRI_TIME_G;

   data_info[7].procedure_label  = textCfgRowLabelCfgPrio8;
   data_info[7].platelet_key     = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_H : KEY_PRI_PLATELET_H);
   data_info[7].plasma_key       = KEY_PRI_PLASMA_H;
   data_info[7].rbc_key          = KEY_PRI_RBC_H;
   data_info[7].blood_type_key   = KEY_PRI_BLOOD_TYPE_H;
   data_info[7].time_key         = KEY_PRI_TIME_H;

   data_info[8].procedure_label  = textCfgRowLabelCfgPrio9;
   data_info[8].platelet_key     = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_I : KEY_PRI_PLATELET_I);
   data_info[8].plasma_key       = KEY_PRI_PLASMA_I;
   data_info[8].rbc_key          = KEY_PRI_RBC_I;
   data_info[8].blood_type_key   = KEY_PRI_BLOOD_TYPE_I;
   data_info[8].time_key         = KEY_PRI_TIME_I;

   data_info[9].procedure_label  = textCfgRowLabelCfgPrio10;
   data_info[9].platelet_key     = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_J : KEY_PRI_PLATELET_J);
   data_info[9].plasma_key       = KEY_PRI_PLASMA_J;
   data_info[9].rbc_key          = KEY_PRI_RBC_J;
   data_info[9].blood_type_key   = KEY_PRI_BLOOD_TYPE_J;
   data_info[9].time_key         = KEY_PRI_TIME_J;

   data_info[10].procedure_label = textCfgRowLabelCfgPrio11;
   data_info[10].platelet_key    = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_K : KEY_PRI_PLATELET_K);
   data_info[10].plasma_key      = KEY_PRI_PLASMA_K;
   data_info[10].rbc_key         = KEY_PRI_RBC_K;
   data_info[10].blood_type_key  = KEY_PRI_BLOOD_TYPE_K;
   data_info[10].time_key        = KEY_PRI_TIME_K;

   data_info[11].procedure_label = textCfgRowLabelCfgPrio12;
   data_info[11].platelet_key    = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_L : KEY_PRI_PLATELET_L);
   data_info[11].plasma_key      = KEY_PRI_PLASMA_L;
   data_info[11].rbc_key         = KEY_PRI_RBC_L;
   data_info[11].blood_type_key  = KEY_PRI_BLOOD_TYPE_L;
   data_info[11].time_key        = KEY_PRI_TIME_L;

   data_info[12].procedure_label = textCfgRowLabelCfgPrio13;
   data_info[12].platelet_key    = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_M : KEY_PRI_PLATELET_M);
   data_info[12].plasma_key      = KEY_PRI_PLASMA_M;
   data_info[12].rbc_key         = KEY_PRI_RBC_M;
   data_info[12].blood_type_key  = KEY_PRI_BLOOD_TYPE_M;
   data_info[12].time_key        = KEY_PRI_TIME_M;

   data_info[13].procedure_label = textCfgRowLabelCfgPrio14;
   data_info[13].platelet_key    = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_N : KEY_PRI_PLATELET_N);
   data_info[13].plasma_key      = KEY_PRI_PLASMA_N;
   data_info[13].rbc_key         = KEY_PRI_RBC_N;
   data_info[13].blood_type_key  = KEY_PRI_BLOOD_TYPE_N;
   data_info[13].time_key        = KEY_PRI_TIME_N;

   data_info[14].procedure_label = textCfgRowLabelCfgPrio15;
   data_info[14].platelet_key    = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_O : KEY_PRI_PLATELET_O);
   data_info[14].plasma_key      = KEY_PRI_PLASMA_O;
   data_info[14].rbc_key         = KEY_PRI_RBC_O;
   data_info[14].blood_type_key  = KEY_PRI_BLOOD_TYPE_O;
   data_info[14].time_key        = KEY_PRI_TIME_O;

   data_info[15].procedure_label = textCfgRowLabelCfgPrio16;
   data_info[15].platelet_key    = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_P : KEY_PRI_PLATELET_P);
   data_info[15].plasma_key      = KEY_PRI_PLASMA_P;
   data_info[15].rbc_key         = KEY_PRI_RBC_P;
   data_info[15].blood_type_key  = KEY_PRI_BLOOD_TYPE_P;
   data_info[15].time_key        = KEY_PRI_TIME_P;

   data_info[16].procedure_label = textCfgRowLabelCfgPrio17;
   data_info[16].platelet_key    = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_Q : KEY_PRI_PLATELET_Q);
   data_info[16].plasma_key      = KEY_PRI_PLASMA_Q;
   data_info[16].rbc_key         = KEY_PRI_RBC_Q;
   data_info[16].blood_type_key  = KEY_PRI_BLOOD_TYPE_Q;
   data_info[16].time_key        = KEY_PRI_TIME_Q;

   data_info[17].procedure_label = textCfgRowLabelCfgPrio18;
   data_info[17].platelet_key    = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_R : KEY_PRI_PLATELET_R);
   data_info[17].plasma_key      = KEY_PRI_PLASMA_R;
   data_info[17].rbc_key         = KEY_PRI_RBC_R;
   data_info[17].blood_type_key  = KEY_PRI_BLOOD_TYPE_R;
   data_info[17].time_key        = KEY_PRI_TIME_R;

   data_info[18].procedure_label = textCfgRowLabelCfgPrio19;
   data_info[18].platelet_key    = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_S : KEY_PRI_PLATELET_S);
   data_info[18].plasma_key      = KEY_PRI_PLASMA_S;
   data_info[18].rbc_key         = KEY_PRI_RBC_S;
   data_info[18].blood_type_key  = KEY_PRI_BLOOD_TYPE_S;
   data_info[18].time_key        = KEY_PRI_TIME_S;

   data_info[19].procedure_label = textCfgRowLabelCfgPrio20;
   data_info[19].platelet_key    = (amapPlatelets ? KEY_PRI_PLATELET_AMAP_T : KEY_PRI_PLATELET_T);
   data_info[19].plasma_key      = KEY_PRI_PLASMA_T;
   data_info[19].rbc_key         = KEY_PRI_RBC_T;
   data_info[19].blood_type_key  = KEY_PRI_BLOOD_TYPE_T;
   data_info[19].time_key        = KEY_PRI_TIME_T;
}  // END of Screen_CONFIG_PRIORITY CONSTRUCTOR



//
////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CONFIG_PRIORITY::~Screen_CONFIG_PRIORITY()
{
   Screen_CONFIG_PRIORITY::deallocate_resources ();
}   // END of Screen_CONFIG_PRIORITY DESTRUCTOR



//
////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_PRIORITY::allocate_resources (const char* allocation_parameter)
{
   const bool              amapPlatelets = guiglobs::amapPlatelets();
   ProductDefinitionArray& prodDefArray (guiglobs::cdsData.rwConfig.definition);

   // Check the keys in case the language changed.
   data_info[0].platelet_key    = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_A : KEY_PRI_PLATELET_A);
   data_info[0].platelet_value  = ( amapPlatelets ? &(prodDefArray[0].key_platelet_amap) : &(prodDefArray[0].key_platelet));
   data_info[1].platelet_key    = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_B : KEY_PRI_PLATELET_B);
   data_info[1].platelet_value  = ( amapPlatelets ? &(prodDefArray[1].key_platelet_amap) : &(prodDefArray[1].key_platelet));
   data_info[2].platelet_key    = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_C : KEY_PRI_PLATELET_C);
   data_info[2].platelet_value  = ( amapPlatelets ? &(prodDefArray[2].key_platelet_amap) : &(prodDefArray[2].key_platelet));
   data_info[3].platelet_key    = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_D : KEY_PRI_PLATELET_D);
   data_info[3].platelet_value  = ( amapPlatelets ? &(prodDefArray[3].key_platelet_amap) : &(prodDefArray[3].key_platelet));
   data_info[4].platelet_key    = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_E : KEY_PRI_PLATELET_E);
   data_info[4].platelet_value  = ( amapPlatelets ? &(prodDefArray[4].key_platelet_amap) : &(prodDefArray[4].key_platelet));
   data_info[5].platelet_key    = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_F : KEY_PRI_PLATELET_F);
   data_info[5].platelet_value  = ( amapPlatelets ? &(prodDefArray[5].key_platelet_amap) : &(prodDefArray[5].key_platelet));
   data_info[6].platelet_key    = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_G : KEY_PRI_PLATELET_G);
   data_info[6].platelet_value  = ( amapPlatelets ? &(prodDefArray[6].key_platelet_amap) : &(prodDefArray[6].key_platelet));
   data_info[7].platelet_key    = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_H : KEY_PRI_PLATELET_H);
   data_info[7].platelet_value  = ( amapPlatelets ? &(prodDefArray[7].key_platelet_amap) : &(prodDefArray[7].key_platelet));
   data_info[8].platelet_key    = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_I : KEY_PRI_PLATELET_I);
   data_info[8].platelet_value  = ( amapPlatelets ? &(prodDefArray[8].key_platelet_amap) : &(prodDefArray[8].key_platelet));
   data_info[9].platelet_key    = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_J : KEY_PRI_PLATELET_J);
   data_info[9].platelet_value  = ( amapPlatelets ? &(prodDefArray[9].key_platelet_amap) : &(prodDefArray[9].key_platelet));
   data_info[10].platelet_key   = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_K : KEY_PRI_PLATELET_K);
   data_info[10].platelet_value = ( amapPlatelets ? &(prodDefArray[10].key_platelet_amap) : &(prodDefArray[10].key_platelet));
   data_info[11].platelet_key   = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_L : KEY_PRI_PLATELET_L);
   data_info[11].platelet_value = ( amapPlatelets ? &(prodDefArray[11].key_platelet_amap) : &(prodDefArray[11].key_platelet));
   data_info[12].platelet_key   = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_M : KEY_PRI_PLATELET_M);
   data_info[12].platelet_value = ( amapPlatelets ? &(prodDefArray[12].key_platelet_amap) : &(prodDefArray[12].key_platelet));
   data_info[13].platelet_key   = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_N : KEY_PRI_PLATELET_N);
   data_info[13].platelet_value = ( amapPlatelets ? &(prodDefArray[13].key_platelet_amap) : &(prodDefArray[13].key_platelet));
   data_info[14].platelet_key   = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_O : KEY_PRI_PLATELET_O);
   data_info[14].platelet_value = ( amapPlatelets ? &(prodDefArray[14].key_platelet_amap) : &(prodDefArray[14].key_platelet));
   data_info[15].platelet_key   = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_P : KEY_PRI_PLATELET_P);
   data_info[15].platelet_value = ( amapPlatelets ? &(prodDefArray[15].key_platelet_amap) : &(prodDefArray[15].key_platelet));
   data_info[16].platelet_key   = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_Q : KEY_PRI_PLATELET_Q);
   data_info[16].platelet_value = ( amapPlatelets ? &(prodDefArray[16].key_platelet_amap) : &(prodDefArray[16].key_platelet));
   data_info[17].platelet_key   = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_R : KEY_PRI_PLATELET_R);
   data_info[17].platelet_value = ( amapPlatelets ? &(prodDefArray[17].key_platelet_amap) : &(prodDefArray[17].key_platelet));
   data_info[18].platelet_key   = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_S : KEY_PRI_PLATELET_S);
   data_info[18].platelet_value = ( amapPlatelets ? &(prodDefArray[18].key_platelet_amap) : &(prodDefArray[18].key_platelet));
   data_info[19].platelet_key   = ( amapPlatelets ? KEY_PRI_PLATELET_AMAP_T : KEY_PRI_PLATELET_T);
   data_info[19].platelet_value = ( amapPlatelets ? &(prodDefArray[19].key_platelet_amap) : &(prodDefArray[19].key_platelet));



   //
   //
   //  Reset member data to ensure synchronization with widget objects
   //
   _top_procedure      = 0;
   _callback_type      = PLATELET;
   _callback_procedure = 0;

   // allocate parent's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   // layer allocation from Z-back to Z-front.

   OSRegion widget_positions(0, 0, LABEL_AREA, 70);

   plate_label.allocate_resources(*this);
   plasm_label.allocate_resources(*this);
   rbc_label.allocate_resources(*this);
   blood_type_label.allocate_resources(*this);
   time_label.allocate_resources(*this);

   widget_positions.x =  PROCEDURE_INDICTOR_X ;
   widget_positions.y =  PROCEDURE_INDICTOR_Y ;
   procedure_indicator.allocate_resources(*this) ;
   procedure_indicator.set_requested_region(widget_positions);

   gray_up_button.allocate_resources (*this);
   gray_up_button.disable_callbacks();

   gray_dn_button.allocate_resources (*this);
   gray_dn_button.disable_callbacks();

   down_button = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                    BITMAP_BUTTON_CFGPP_DN_ARROW_X,
                                    BITMAP_BUTTON_CFGPP_DN_ARROW_Y,
                                    textEmpty,
                                    BITMAP_CFGPP_BTN_DOWN,
                                    BITMAP_CFGPP_BTN_DOWN_D);

   down_button->allocate_resources (*this);
   down_button->set_callback(cb_down_priority_button, (void*)this);

   for (int row = 0; row < MAX_ROWS; row++)
   {
      row_info[row].procedure_row_box.bitmap =
         new Button_Bitmap (PP_BUTTON_LABEL_BOX(row),
                            (short)row_info[row].procedure_row_box.x_pos,
                            (short)row_info[row].procedure_row_box.y_pos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            BITMAP_CFGPP_PROC_NUM_BOX,
                            BITMAP_NULL
                            );
      row_info[row].procedure_row_box.bitmap->allocate_resources(*this);
      row_info[row].procedure_row_box.button_ptr = row_info[row].procedure_row_box.bitmap;
      row_info[row].procedure_row_box.bitmap->disable_callbacks();

      row_info[row].procedure_row_line.bitmap =
         new Button_Bitmap (PP_BUTTON_LABEL_LINE(row),
                            (short)row_info[row].procedure_row_line.x_pos,
                            (short)row_info[row].procedure_row_line.y_pos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            BITMAP_CFGPP_PROC_ROW_LINE,
                            BITMAP_NULL
                            );

      row_info[row].procedure_row_line.bitmap->allocate_resources(*this);
      row_info[row].procedure_row_line.button_ptr = row_info[row].procedure_row_line.bitmap;
      row_info[row].procedure_row_line.bitmap->disable_callbacks();

      row_info[row].platelet_button.bitmap =
         new Button_Bitmap (PP_BUTTON_PLATELETS(row),
                            (short)row_info[row].platelet_button.x_pos,
                            (short)row_info[row].platelet_button.y_pos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            CFG_Look::BUTTON_UP_NAME,
                            CFG_Look::BUTTON_DOWN_NAME);

      row_info[row].platelet_button.bitmap->allocate_resources(*this);
      row_info[row].platelet_button.button_ptr = row_info[row].platelet_button.bitmap;
      row_info[row].platelet_button.bitmap->set_callback(cb_button, (void*)&row_info[row].platelet_button);

      row_info[row].plasma_button.bitmap =
         new Button_Bitmap (PP_BUTTON_PLASMA(row),
                            (short)row_info[row].plasma_button.x_pos,
                            (short)row_info[row].plasma_button.y_pos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            CFG_Look::BUTTON_UP_NAME,
                            CFG_Look::BUTTON_DOWN_NAME);

      row_info[row].plasma_button.bitmap->allocate_resources(*this);
      row_info[row].plasma_button.button_ptr = row_info[row].plasma_button.bitmap;
      row_info[row].plasma_button.bitmap->set_callback(cb_button, (void*)&row_info[row].plasma_button);

      row_info[row].rbc_button.bitmap =
         new Button_Bitmap (PP_BUTTON_RBC(row),
                            (short)row_info[row].rbc_button.x_pos,
                            (short)row_info[row].rbc_button.y_pos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            CFG_Look::BUTTON_UP_NAME,
                            CFG_Look::BUTTON_DOWN_NAME);

      row_info[row].rbc_button.bitmap->allocate_resources(*this);
      row_info[row].rbc_button.button_ptr = row_info[row].rbc_button.bitmap;
      row_info[row].rbc_button.bitmap->set_callback(cb_button, (void*)&row_info[row].rbc_button);

      row_info[row].blood_type_button.bitmap =
         new Button_Bitmap (PP_BUTTON_TYPE(row),
                            (short)row_info[row].blood_type_button.x_pos,
                            (short)row_info[row].blood_type_button.y_pos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            CFG_Look::BUTTON_UP_NAME,
                            CFG_Look::BUTTON_DOWN_NAME);

      row_info[row].blood_type_button.bitmap->allocate_resources(*this);
      row_info[row].blood_type_button.button_ptr = row_info[row].blood_type_button.bitmap;
      row_info[row].blood_type_button.bitmap->set_callback(cb_button, (void*)&row_info[row].blood_type_button);

      row_info[row].time_button.bitmap =
         new Button_Bitmap (PP_BUTTON_TIME(row),
                            (short)row_info[row].time_button.x_pos,
                            (short)row_info[row].time_button.y_pos,
                            textEmptyFormatCfgNumericTextFormatNull,
                            CFG_Look::BUTTON_UP_NAME,
                            CFG_Look::BUTTON_DOWN_NAME);

      row_info[row].time_button.bitmap->allocate_resources(*this);
      row_info[row].time_button.button_ptr = row_info[row].time_button.bitmap;
      row_info[row].time_button.bitmap->set_callback(cb_button, (void*)&row_info[row].time_button);
   }

   label_the_buttons();

   checkForTimeOnlyConfig();
}  // END of Screen_CONFIG_PRIORITY ALLOCATE_RESOURCES



//
////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIG_PRIORITY::deallocate_resources ()
{
   plate_label.deallocate_resources();
   plasm_label.deallocate_resources();
   rbc_label.deallocate_resources();
   blood_type_label.deallocate_resources();
   time_label.deallocate_resources();
   procedure_indicator.deallocate_resources();

   // default constructor calls deallocate_resources.
   delete up_button;
   up_button = NULL;

   delete down_button;
   down_button = NULL;

   gray_up_button.deallocate_resources ();
   gray_dn_button.deallocate_resources ();

   //
   //  Remove items created during allocation...
   for (int row = 0; row < MAX_ROWS; row++)
   {
      row_info[row].yieldValue.deallocate_resources();
      row_info[row].yieldx10.deallocate_resources();
      row_info[row].yieldtoThe11th.deallocate_resources();

      row_info[row].volValue.deallocate_resources();
      row_info[row].volUnits.deallocate_resources();

      row_info[row].doseValue.deallocate_resources();
      row_info[row].doseUnits.deallocate_resources();

      delete row_info[row].procedure_row_box.bitmap;
      row_info[row].procedure_row_box.bitmap = NULL;

      delete row_info[row].procedure_row_line.bitmap;
      row_info[row].procedure_row_line.bitmap = NULL;

      delete row_info[row].platelet_button.bitmap;
      row_info[row].platelet_button.bitmap = NULL;

      delete row_info[row].plasma_button.bitmap;
      row_info[row].plasma_button.bitmap = NULL;

      delete row_info[row].rbc_button.bitmap;
      row_info[row].rbc_button.bitmap = NULL;

      delete row_info[row].blood_type_button.bitmap;
      row_info[row].blood_type_button.bitmap = NULL;

      delete row_info[row].time_button.bitmap;
      row_info[row].time_button.bitmap = NULL;
   }

   // deallocate parent's resources
   Screen_Config::deallocate_resources ();
}   // END of Screen_CONFIG_PRIORITY DEALLOCATE_RESOURCES



///////////////////////////////////////////////////////////////////////////////
void cb_button (void* data)
{
   guistring button_string;
   button_string.initialize();

   Screen_CONFIG_PRIORITY::BTN_INFO* btn_info = (Screen_CONFIG_PRIORITY::BTN_INFO*)data;
   (btn_info->parent)->set_indicator(btn_info->row);
   (btn_info->parent)->awaiting_callback(btn_info->btn_type, btn_info->row);

   switch (btn_info->btn_type)
   {
      case Screen_CONFIG_PRIORITY::INACTIVE :
         break; // no actions required for inactive buttons

      case Screen_CONFIG_PRIORITY::PLATELET :
         if (guiglobs::amapPlatelets())
            if (guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on)
               Screen_Config::build_param_string(button_string, KEY_PRI_PLATELET_AMAP_PAS_LIST, PLASMA_COLUMN + 30, PR_KEYPAD_Y);
            else
               Screen_Config::build_param_string(button_string, KEY_PRI_PLATELET_AMAP_LIST, PLASMA_COLUMN + 30, PR_KEYPAD_Y);
         else
            Screen_Config::build_param_string(button_string, KEY_PRI_PLATELET_LIST, PLASMA_COLUMN + 30, PR_KEYPAD_Y);

         // Screen_Config::build_param_string(button_string, KEY_PRI_PLATELET_LIST, PLASMA_COLUMN+30, PR_KEYPAD_Y);
         (btn_info->parent)->goto_child_screen(GUI_SCREEN_PLT_LISTBOX, button_string);
         break;

      case Screen_CONFIG_PRIORITY::PLASMA :
         Screen_Config::build_param_string(button_string, KEY_PRI_PLASMA_LIST, RBC_COLUMN + 30, PR_KEYPAD_Y);
         (btn_info->parent)->goto_child_screen(GUI_SCREEN_PLAS_LISTBOX, button_string);
         break;

      case Screen_CONFIG_PRIORITY::RBC :
         Screen_Config::build_param_string(button_string, KEY_PRI_RBC_LIST, BLOOD_TYPE_COLUMN + 30, PR_KEYPAD_Y);
         (btn_info->parent)->goto_child_screen(GUI_SCREEN_RBC_LISTBOX, button_string);
         break;

      case Screen_CONFIG_PRIORITY::BLOOD_TYPE :
         Screen_Config::build_param_string(button_string, KEY_PRI_BLOOD_TYPE_LIST, RBC_COLUMN - 30, PR_KEYPAD_Y);
         (btn_info->parent)->goto_child_screen(GUI_SCREEN_LISTBOX, button_string);
         break;

      case Screen_CONFIG_PRIORITY::TIME :
      {
         //
         //
         //  Calculation for min/max time assumes the limits are the same for all
         //  of the 'time' buttons
         //
         static const float min = guiglobs::config_var_list.Var(KEY_PRI_TIME_A).Min();
         static const float max = guiglobs::config_var_list.Var(KEY_PRI_TIME_A).Max();

         (btn_info->parent)->display_min_max_keypad_limits (min, max, "%.0f");
         Screen_Config::build_keypad_string (button_string,
                                             KEY_PRI_TIME_A, // unused
                                             RBC_COLUMN + 30,
                                             PR_KEYPAD_Y,
                                             "###",
                                             max,
                                             min);

         (btn_info->parent)->goto_child_screen(GUI_SCREEN_KEYPAD, button_string);
      }
      break;
   }
}



////////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_PRIORITY::awaiting_callback (const BUTTON_TYPE type, const int row)
{
   _callback_type      = type;
   _callback_procedure = row + _top_procedure;
}



////////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG_PRIORITY::set_indicator (const int row)
{
   OSRegion widget_pos(PROCEDURE_INDICTOR_X, PROCEDURE_INDICTOR_Y, LABEL_AREA, 70);

   procedure_indicator.set_string_id(data_info[row + _top_procedure].procedure_label);
   int len = 80;
   widget_pos.x = (short)(320 - len / 2);
   procedure_indicator.set_requested_region(widget_pos);
}



//
///////////////////////////////////////////////////////////////////////////////
// label_the_buttons
void Screen_CONFIG_PRIORITY::label_the_buttons ()
{
   char             button_string[80];
   int              data_int, text_len;
   int              proceed_num   = _top_procedure;

   const bool       amapPlatelets = guiglobs::amapPlatelets();
   const bool       pasOn         = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on;

   TEXT_STRING_ID   string_id     = TEXT_NULL;
   CONFIG_DATA_KEYS prodlist      = KEY_INACTIVE;

   if (amapPlatelets)
      if (pasOn)
         prodlist = KEY_PRI_PLATELET_AMAP_PAS_LIST;
      else
         prodlist = KEY_PRI_PLATELET_AMAP_LIST;
   else
      prodlist = KEY_PRI_PLATELET_LIST;

   for (int row = 0; row < MAX_ROWS; row++, proceed_num++)
   {
      text_len = sprintf(button_string, "%d", (proceed_num + 1));
      row_info[row].procedure_row_box.bitmap->set_text_direct (text_len, button_string);

      string_id = get_text_string (prodlist, *data_info[proceed_num].platelet_value);
      DataLog(log_level_gui_debug) << "string_id: " << string_id << " value: " << *data_info[proceed_num].platelet_value << endmsg;

      if (string_id == 0)
      {
         string_id = get_text_string (prodlist, (*data_info[proceed_num].platelet_value) - NUM_PLT_AMAP_PAS);
         DataLog(log_level_gui_debug) << "string_id: " << string_id << " value: " << *data_info[proceed_num].platelet_value << endmsg;
      }
      row_info[row].platelet_button.bitmap->set_string_id(string_id);

      row_info[row].plasma_button.bitmap->set_string_id(get_text_string(KEY_PRI_PLASMA_LIST, *data_info[proceed_num].plasma_value));
      row_info[row].blood_type_button.bitmap->set_string_id(get_text_string(KEY_PRI_BLOOD_TYPE_LIST, *data_info[proceed_num].blood_type_value));
      row_info[row].rbc_button.bitmap->set_string_id(get_text_string(KEY_PRI_RBC_LIST, *data_info[proceed_num].rbc_value));

      // Time buttons
      data_int = (int)(*data_info[proceed_num].time_value);
      text_len = sprintf(button_string, "%d", data_int);
      row_info[row].time_button.bitmap->set_text_direct (text_len, button_string);

      updatePltDataField(row);
      updatePlasDataField(row);
      updateRbcDataField(row);
   }
}



//
///////////////////////////////////////////////////////////////////////////////
// callback for the up button
void cb_up_priority_button (void* data)
{
   ((Screen_CONFIG_PRIORITY*)data) -> process_up_button();
}



///////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_PRIORITY::process_up_button ()
{
   if (_top_procedure > 0)
   {
      _top_procedure--;
      label_the_buttons();

      if (_top_procedure == 0)
      {
         delete up_button;
         up_button = NULL;
      }
   }

   if (down_button == NULL)
   {
      down_button = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                       BITMAP_BUTTON_CFGPP_DN_ARROW_X,
                                       BITMAP_BUTTON_CFGPP_DN_ARROW_Y,
                                       textEmpty,
                                       BITMAP_CFGPP_BTN_DOWN,
                                       BITMAP_CFGPP_BTN_DOWN_D);

      down_button->allocate_resources(*this);
      down_button->set_callback(cb_down_priority_button, (void*)this);
   }

   checkForTimeOnlyConfig();
}



//
///////////////////////////////////////////////////////////////////////////////
// callback for the up button
void cb_down_priority_button (void* data)
{
   ((Screen_CONFIG_PRIORITY*)data)->process_down_button();
}



///////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_PRIORITY::process_down_button ()
{
   if (_top_procedure < (MAX_PROCEDURES - MAX_ROWS))
   {
      ++_top_procedure;
      label_the_buttons();

      if (_top_procedure >= (MAX_PROCEDURES - MAX_ROWS))
      {
         delete down_button;
         down_button = NULL;
      }
   }

   if (up_button == NULL)
   {
      up_button = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON,
                                     BITMAP_BUTTON_CFGPP_UP_ARROW_X,
                                     BITMAP_BUTTON_CFGPP_UP_ARROW_Y,
                                     textEmpty,
                                     BITMAP_CFGPP_BTN_UP,
                                     BITMAP_CFGPP_BTN_UP_D);

      up_button->allocate_resources(*this);
      up_button->set_callback(cb_up_priority_button, (void*)this);
   }

   checkForTimeOnlyConfig();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Message handler for LISTBOXES
void Screen_CONFIG_PRIORITY::update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer)
{
   procedure_indicator.set_string_id (textEmptyFormatCfgNumericTextFormatNull);
   int row = _callback_procedure - _top_procedure;

   switch (_callback_type)
   {
      case PLATELET :
         *data_info[_callback_procedure].platelet_value = rtn_listbox_buffer.key_value;
         row_info[row].platelet_button.bitmap->set_string_id(get_text_string(((guiglobs::amapPlatelets()) ?
                                                                              (guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on ? KEY_PRI_PLATELET_AMAP_PAS_LIST : KEY_PRI_PLATELET_AMAP_LIST)
                                                                              : KEY_PRI_PLATELET_LIST),
                                                                             *data_info[_callback_procedure].platelet_value));
         updatePltDataField(row);
         break;

      case PLASMA :
         *data_info[_callback_procedure].plasma_value = rtn_listbox_buffer.key_value;
         row_info[row].plasma_button.bitmap->set_string_id(get_text_string(KEY_PRI_PLASMA_LIST, *data_info[_callback_procedure].plasma_value));
         updatePlasDataField(row);
         break;

      case RBC :
         *data_info[_callback_procedure].rbc_value = rtn_listbox_buffer.key_value;
         row_info[row].rbc_button.bitmap->set_string_id(get_text_string(KEY_PRI_RBC_LIST, *data_info[_callback_procedure].rbc_value));
         updateRbcDataField(row);
         break;

      case BLOOD_TYPE :
         *data_info[_callback_procedure].blood_type_value = rtn_listbox_buffer.key_value;
         row_info[row].blood_type_button.bitmap->set_string_id(get_text_string(KEY_PRI_BLOOD_TYPE_LIST, *data_info[_callback_procedure].blood_type_value));
         break;

      case INACTIVE :
      case TIME :
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "UNEXPECTED SCREEN CHOICE" << endmsg;
   }

} // end of update listbox method



///////////////////////////////////////////////////////////////////////////////
// Message handler for KEYPADS
void Screen_CONFIG_PRIORITY::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer)
{
   procedure_indicator.set_string_id (textEmptyFormatCfgNumericTextFormatNull);
   turn_off_min_max_keypad_text();

   if ((return_keypad_buffer.keypad_return_value) != -1)
   {
      float the_data = atof(return_keypad_buffer.keypad_return_string);
      *data_info[_callback_procedure].time_value = the_data;
      row_info[_callback_procedure - _top_procedure].time_button.bitmap->set_text_direct (10, return_keypad_buffer.keypad_return_string);
   }
} // end of method

void Screen_CONFIG_PRIORITY::updatePltDataField (const int row)
{
   const bool amapp = guiglobs::amapPlatelets();

   if (amapp)
   {
      bool isPason       = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on;                                                                                //
      int  amapPltNumber =   guiglobs::cdsData.rwConfig.definition[row + _top_procedure].key_platelet_amap;

      DataLog(log_level_gui_debug) << " row: " << (row + _top_procedure) << endmsg;
      DataLog(log_level_gui_debug) << " amapPltNumber: " << amapPltNumber << " isPason: " << isPason << endmsg;

      if (amapPltNumber <= NUM_PLT_AMAP)
      {
         BITMAP_ID pltBmpId = CFG_Look::BUTTON_UP_NAME;

         if (amapPltNumber > PLATELET_3 && isPason)
         {
            pltBmpId = BITMAP_CONFIG_SUB_UP_PAS;
            DataLog(log_level_gui_debug) << " setting pas icon on row " << row << endmsg;
         }
         else
         {
            pltBmpId = CFG_Look::BUTTON_UP_NAME;
            DataLog(log_level_gui_debug) << "setting no icon on row " << row << endmsg;
         }
         row_info[row].platelet_button.bitmap->set_up_bitmap(pltBmpId);
      }
      else // error
      {
         DataLog(log_level_gui_error) << "Invalid procedure: " << amapPltNumber << endmsg;

         row_info[row].platelet_button.bitmap->set_up_bitmap(CFG_Look::BUTTON_UP_NAME);
         row_info[row].yieldValue.deallocate_resources();
         row_info[row].yieldx10.deallocate_resources();
         row_info[row].yieldtoThe11th.deallocate_resources();
      }
   }
   else
   {
      const int  pltProcedure = guiglobs::cdsData.rwConfig.definition[row + _top_procedure].key_platelet;
      const bool isPas        = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on && guiglobs::cdsData.rwConfig.templates.key_plt_mss[pltProcedure - 1];
/*
        DataLog(log_level_gui_debug) << " amapplt isPas at this number: " << guiglobs::cdsData.rwConfig.templates.key_plt_mss[pltProcedure-1] << endmsg;
        DataLog(log_level_gui_debug) << " min   max   pct   amap_conc    volm PAS "  << endmsg;
        DataLog(log_level_gui_debug) << guiglobs::cdsData.rwConfig.templates.key_plt_amap_min_yield[pltProcedure-1]
                    << "  "      <<  guiglobs::cdsData.rwConfig.templates.key_plt_amap_max_yield[pltProcedure-1]
                     << "  "       <<  guiglobs::cdsData.rwConfig.templates.key_plt_pct_carryover[pltProcedure-1]
                     << "  "       << guiglobs::cdsData.rwConfig.templates.key_plt_amap_conc[pltProcedure-1]
                     << "  "       << guiglobs::cdsData.rwConfig.templates.key_plt_volume[pltProcedure-1]
                     << "  "       << guiglobs::cdsData.rwConfig.templates.key_plt_mss[pltProcedure-1]
                     << "  "       << endmsg;
*/
      if (pltProcedure > 0 && pltProcedure <= NUM_YIELD_VOLUME)
      {
         guistring yieldString;

         float     yieldVal    = guiglobs::cdsData.rwConfig.templates.key_plt_yield[pltProcedure - 1];
         float     collectVol  = guiglobs::cdsData.rwConfig.templates.key_plt_volume[pltProcedure - 1];
         float     collectConc = (yieldVal * 1.0E5f) / (collectVol);

         int       stringLen   = sprintf_l10n(yieldString, "%.1f", yieldVal);

         BITMAP_ID pltBmpId    = CFG_Look::BUTTON_UP_NAME;
         if (isPas) pltBmpId = BITMAP_CONFIG_SUB_UP_PAS;
         else if (Software_CDS::isPPC(collectConc))
            pltBmpId = BITMAP_CONFIG_SUB_UP_HYPER;

         row_info[row].platelet_button.bitmap->set_up_bitmap(pltBmpId);

         OSRegion textArea;

         textArea.x      = 0;
         textArea.y      = ((row_info[row].platelet_button.bitmap->height() * 2) / 3);
         textArea.width  = (row_info[row].platelet_button.bitmap->width() / 2) - __PLT_LISTBOX_TXT_OFFSET__;
         textArea.height = row_info[row].platelet_button.bitmap->height() / 3;

         row_info[row].yieldValue.set_text_direct(stringLen, yieldString);
         row_info[row].yieldValue.set_requested_region(textArea);
         row_info[row].yieldValue.allocate_resources(*row_info[row].platelet_button.bitmap);

         textArea.x = ((row_info[row].platelet_button.bitmap->width() / 2) - __PLT_LISTBOX_TXT_OFFSET__ + 3);

         row_info[row].yieldx10.set_requested_region(textArea);
         row_info[row].yieldx10.allocate_resources(*row_info[row].platelet_button.bitmap);
         row_info[row].yieldtoThe11th.allocate_resources(*row_info[row].platelet_button.bitmap);
      }
      else
      {
         row_info[row].platelet_button.bitmap->set_up_bitmap(CFG_Look::BUTTON_UP_NAME);
         row_info[row].yieldValue.deallocate_resources();
         row_info[row].yieldx10.deallocate_resources();
         row_info[row].yieldtoThe11th.deallocate_resources();
      }


   }

}

void Screen_CONFIG_PRIORITY::updatePlasDataField (const int row)
{
   char      volString[80];
   int       stringLen     = 0;
   float     volVal        = -1;

   const int plasProcedure = guiglobs::cdsData.rwConfig.definition[row + _top_procedure].key_plasma;

   if (plasProcedure > 0 && plasProcedure <= NUM_PLASMA_VOLUME)
   {
      volVal    = guiglobs::cdsData.rwConfig.templates.key_pls_volume[plasProcedure - 1];
      stringLen = sprintf(volString, "%.0f", volVal);

      OSRegion textArea;
        #define __PLAS_LISTBOX_TXT_OFFSET__ 2

      textArea.x      = 0;
      textArea.y      = ((row_info[row].plasma_button.bitmap->height() * 2) / 3);
      textArea.width  = (row_info[row].plasma_button.bitmap->width() / 2) + __PLAS_LISTBOX_TXT_OFFSET__;
      textArea.height = row_info[row].plasma_button.bitmap->height() / 3;

      row_info[row].volValue.set_text_direct(stringLen, volString);
      row_info[row].volValue.set_requested_region(textArea);
      row_info[row].volValue.allocate_resources(*row_info[row].plasma_button.bitmap);

      textArea.x = ((row_info[row].plasma_button.bitmap->width() / 2) + __PLAS_LISTBOX_TXT_OFFSET__ + 2);

      row_info[row].volUnits.set_requested_region(textArea);
      row_info[row].volUnits.allocate_resources(*row_info[row].plasma_button.bitmap);
   }
   else
   {
      row_info[row].volValue.deallocate_resources();
      row_info[row].volUnits.deallocate_resources();
   }
}

void Screen_CONFIG_PRIORITY::updateRbcDataField (const int row)
{
   char      doseString[80];
   int       stringLen      = 0;
   bool      showDose       = true;

   const int rbcProcedure   = guiglobs::cdsData.rwConfig.definition[row + _top_procedure].key_rbc;

   bool      masterPtfOn    = guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on;
   bool      procedurePtfOn = masterPtfOn && guiglobs::cdsData.rwConfig.templates.key_rbc_mss[rbcProcedure - 1];

   if (rbcProcedure == 0 || rbcProcedure > NUM_CRIT_DOSE)
   {
      row_info[row].rbc_button.bitmap->set_up_bitmap(CFG_Look::BUTTON_UP_NAME);
      showDose = false;
   }
   else
   {
      stringLen = sprintf(doseString, "%.0f", guiglobs::cdsData.rwConfig.templates.key_rbc_dose[rbcProcedure - 1]);
   }

   if (showDose)
   {
      if (procedurePtfOn)
      {
         row_info[row].rbc_button.bitmap->set_up_bitmap(BITMAP_CONFIG_SUB_UP_RAS);
      }
      else
      {
         row_info[row].rbc_button.bitmap->set_up_bitmap(CFG_Look::BUTTON_UP_NAME);
      }

      OSRegion textArea;
        #define __RBC_LISTBOX_TXT_OFFSET__ 2

      textArea.x      = 0;
      textArea.y      = ((row_info[row].rbc_button.bitmap->height() * 2) / 3);
      textArea.width  = (row_info[row].rbc_button.bitmap->width() / 2) + __RBC_LISTBOX_TXT_OFFSET__;
      textArea.height = row_info[row].rbc_button.bitmap->height() / 3;

      row_info[row].doseValue.set_text_direct(stringLen, doseString);
      row_info[row].doseValue.set_requested_region(textArea);
      row_info[row].doseValue.allocate_resources(*row_info[row].rbc_button.bitmap);

      textArea.x = ((row_info[row].rbc_button.bitmap->width() / 2) + __RBC_LISTBOX_TXT_OFFSET__ + 2);

      row_info[row].doseUnits.set_requested_region(textArea);
      row_info[row].doseUnits.allocate_resources(*row_info[row].rbc_button.bitmap);
   }
   else
   {
      row_info[row].doseValue.deallocate_resources();
      row_info[row].doseUnits.deallocate_resources();
   }
}


void Screen_CONFIG_PRIORITY::checkForTimeOnlyConfig ()
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_info) << "Buttons should be disabled." << endmsg;

      for (int rowNum = 0; rowNum < MAX_ROWS; rowNum++)
      {
         bool showRasIcon = row_info[rowNum].platelet_button.bitmap &&
                            (row_info[rowNum].rbc_button.bitmap->get_up_bitmap_id() == BITMAP_CONFIG_SUB_UP_RAS);

         if (row_info[rowNum].procedure_row_box.bitmap)
            disableConfigBtn(*row_info[rowNum].procedure_row_box.bitmap, BITMAP_CFGPP_PROC_NUM_BOX);

         if (row_info[rowNum].platelet_button.bitmap)
         {
            BITMAP_ID pltBmpId = BITMAP_CONFIG_BUTTON_GRAY;

            if ( rowNum + _top_procedure < NUM_YIELD_VOLUME )
            {
               bool isPas = guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on &&
                            guiglobs::cdsData.rwConfig.templates.key_plt_mss[rowNum + _top_procedure];

               float yieldVal    = guiglobs::cdsData.rwConfig.templates.key_plt_yield[rowNum + _top_procedure];
               float collectVol  = guiglobs::cdsData.rwConfig.templates.key_plt_volume[rowNum + _top_procedure];
               float collectConc = (yieldVal * 1.0E5f) / (collectVol);

               if (isPas)
                  pltBmpId = BITMAP_CONFIG_SUB_UP_AAS_GREY;
               else if (Software_CDS::isPPC(collectConc))
                  pltBmpId = BITMAP_CONFIG_SUB_UP_HYPER_GREY;
            }
            else
            {
               DataLog(log_level_gui_error) << "ROW INDEX OUT-OF-BOUNDS: _top_procedure=" << _top_procedure << endmsg;
            }

            disableConfigBtn(*row_info[rowNum].platelet_button.bitmap, pltBmpId);
         }

         if (row_info[rowNum].plasma_button.bitmap)
            disableConfigBtn(*row_info[rowNum].plasma_button.bitmap);

         if (row_info[rowNum].rbc_button.bitmap)
            disableConfigBtn(*row_info[rowNum].rbc_button.bitmap, ((showRasIcon) ? BITMAP_CONFIG_SUB_UP_AAS_GREY : BITMAP_CONFIG_BUTTON_GRAY));

         if (row_info[rowNum].blood_type_button.bitmap)
            disableConfigBtn(*row_info[rowNum].blood_type_button.bitmap);

         if (row_info[rowNum].time_button.bitmap)
            disableConfigBtn(*row_info[rowNum].time_button.bitmap);

         disableConfigTxt(row_info[rowNum].yieldValue);
         disableConfigTxt(row_info[rowNum].yieldx10);
         disableConfigTxt(row_info[rowNum].yieldtoThe11th);
         disableConfigTxt(row_info[rowNum].volValue);
         disableConfigTxt(row_info[rowNum].volUnits);
         disableConfigTxt(row_info[rowNum].doseValue);
         disableConfigTxt(row_info[rowNum].doseUnits);
      }

      disableConfigTxt(plate_label);
      disableConfigTxt(plasm_label);
      disableConfigTxt(rbc_label);
      disableConfigTxt(blood_type_label);
      disableConfigTxt(time_label);
      disableConfigTxt(procedure_indicator);

   }
}


//
///////////////////////////////////////////////////////////////////////////////
// end of cfg_priority.cpp file

/* FORMAT HASH b74fdb2d2abf394617ca5f3979447b1f */

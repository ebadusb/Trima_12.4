/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/bitmap_info.h 1.13 2008/07/29 16:52:09Z dslausb Exp dslausb $
 * $Log: bitmap_info.h $
 * Revision 1.13  2008/07/29 16:52:09Z  dslausb
 * IT 8775 - Move / Resize end run button so it doesn't overlap date.
 * Revision 1.12  2008/06/25 19:00:54Z  dslausb
 * Combine RAS toggle with procedure label to buy more room for localization
 * Revision 1.11  2008/06/23 18:32:26Z  dslausb
 * Layout tweaks requested by localization
 * Revision 1.10  2007/10/25 16:09:05Z  dslausb
 * Rearrange RBC config screen to accomdate new order (see IT 8244)
 * Revision 1.9  2006/08/08 18:54:35Z  rm70006
 * IT 7429.  Change barcode categories to match PPC and RBC solution addition.
 * Revision 1.8  2006/05/31 00:37:28Z  dslausb
 * Reformatted donconn screen - now configurable, too.
 * Revision 1.7  2006/05/19 19:09:13Z  dslausb
 * Changed endrun summary screens and sysclamp screen
 * Revision 1.6  2006/05/12 21:11:40Z  dslausb
 * Fixed calc stuff on PLT config screen, updated predict screen and mach screen (all for 5.P stuff)
 * Revision 1.5  2006/05/05 21:45:32Z  dslausb
 * IT 7212 - Separated up flag and labeling run summary screens into their respective products
 * Revision 1.4  2006/03/03 17:28:43Z  dslausb
 * First attempt at bringing over MSS stuff
 * Revision 1.3  2003/06/06 20:12:39Z  rm70006
 * IT 6096.
 * Share files with Vista
 * Revision 1.2  2003/02/26 17:10:36Z  rm10919
 * IT 5863, 5891 - lowered rows for CFG_RBC screen and deleted x,y coordinates for second placard on disconnect screen.
 * Revision 1.1  2003/01/06 18:35:19Z  jl11312
 * Initial revision
 *
 */

/**********************************************************************************
 NOTE: This file is now shared with Vista. Take care when adding new dependencies
 **********************************************************************************/

#ifndef _BITMAP_INFO_INCLUDE
#define _BITMAP_INFO_INCLUDE

#ifndef WIN32
#include "filenames.h"
#endif

#include "bitmap_info_id.h"

////////////////////////////////////////////////////////////////////////////////
// Widget and bitmap X/Y coordinate manifests

//// BUTTONS ////

// Cancel Button Location
   #define BUTNCANC_X 537
   #define BUTNCANC_Y 31

// Goback Button Location
   #define BUTNGOBK_X 5
   #define BUTNGOBK_Y 31

// Continue Button Location
   #define BUTNCONT_X 206
   #define BUTNCONT_Y 434

// Help Button Location
   #define BUTNHELP_X 440
   #define BUTNHELP_Y 434

// Help Button Location
   #define BUTNLOWL_X 111
   #define BUTNLOWL_Y 434

// Disconnect Confirm Button Location
   #define BUTNCONF_X  206
   #define BUTNCONF_Y  393




//// PLACARDS - MAIN DESCRIPTIVE GRAPHIC ON APHERESIS SCREENS ////

// Standard PLACARD coordinates (most placards use these)
   #define PLAC_STD_X 45
   #define PLAC_STD_Y 134


// SYSDISP Screen placard
   #define PLAC_DISPSYS_X PLAC_STD_X
   #define PLAC_DISPSYS_Y PLAC_STD_Y
   #define PLAC_CROSSOVER_X 230
   #define PLAC_CROSSOVER_Y 135

// SYSCLAMP Screen placard
   #define PLAC_DISPCLMP_X PLAC_STD_X
   #define PLAC_DISPCLMP_Y PLAC_STD_Y

// SYSTEST Screen placard
   #define PLAC_DISPTEST_X (PLAC_STD_X + 270)  // chopped off left side to eliminate text flash
   #define PLAC_DISPTEST_Y PLAC_STD_Y

// DONCONN Screen placard
   #define PLAC_DONCONN_X 59
   #define PLAC_DONCONN_Y 132

// BLOODSMP Screen placard
   #define PLAC_BLOODSMP_X PLAC_STD_X
   #define PLAC_BLOODSMP_Y PLAC_STD_Y

// SYSACATT Screen placard
   #define PLAC_ACATT_X PLAC_STD_X
   #define PLAC_ACATT_Y (PLAC_STD_Y - 15)

// SYSDONOR Screen system, donor buttons/bitmaps
   #define BUTNSETUP_X 332
   #define BUTNSETUP_Y 166

   #define BUTNINFO_X  108
   #define BUTNINFO_Y  166

   #define BITMAP_DONOR_PICTURE_X     16
   #define BITMAP_DONOR_PICTURE_Y     3

   #define BITMAP_TOP_UP_X            0
   #define BITMAP_TOP_UP_Y            0
   #define BITMAP_BOTTOM_UP_X         0
   #define BITMAP_BOTTOM_UP_Y         211
   #define BITMAP_LEFT_UP_X           0
   #define BITMAP_LEFT_UP_Y           0
   #define BITMAP_RIGHT_UP_X          187
   #define BITMAP_RIGHT_UP_Y          0
   #define BITMAP_TOP_DOWN_X          BITMAP_TOP_UP_X
   #define BITMAP_TOP_DOWN_Y          BITMAP_TOP_UP_Y
   #define BITMAP_BOTTOM_DOWN_X       BITMAP_BOTTOM_UP_X
   #define BITMAP_BOTTOM_DOWN_Y       BITMAP_BOTTOM_UP_Y
   #define BITMAP_LEFT_DOWN_X         BITMAP_LEFT_UP_X
   #define BITMAP_LEFT_DOWN_Y         BITMAP_LEFT_UP_Y
   #define BITMAP_RIGHT_DOWN_X        BITMAP_RIGHT_UP_X
   #define BITMAP_RIGHT_DOWN_Y        BITMAP_RIGHT_UP_Y

// RUNSUMM1 Screen panel
   #define PLAC_RUNSUMM1_X  336
   #define PLAC_RUNSUMM1_Y  105

// RUNSUMM2 Screen panel
   #define PLAC_RUNSUMM2_X  336
   #define PLAC_RUNSUMM2_Y  105

// DISCONN Screen buttons, panels
   #define PLAC_DISCONN_DET_X  45
   #define PLAC_DISCONN_DET_Y  134

   #define PLAC_DISCONN_DIS_X  229
   #define PLAC_DISCONN_DIS_Y  134

   #define PLAC_DISCONN_REM_X  413
   #define PLAC_DISCONN_REM_Y  134

   #define GEN_GRAPHIC_RIGHT_X  350
   #define GEN_GRAPHIC_RIGHT_Y  150

// POWERUP Screen placard
   #define PLAC_POWERUP_X  0
   #define PLAC_POWERUP_Y  0

// ABORTCNF Screen Buttons and Panel
   #define BUTN_ABORTCNF_EXIT_X    154
   #define BUTN_ABORTCNF_EXIT_Y    311
   #define BUTN_ABORTCNF_NOEXIT_X  349
   #define BUTN_ABORTCNF_NOEXIT_Y  311

   #define PANEL_ABORTCNF_X 116
   #define PANEL_ABORTCNF_Y 111

// common screen buttons and panels
   #define SECONDARY_INLAY_X 108
   #define SECONDARY_INLAY_Y 111

// DONVITAL Screen buttons and panels
   #define BUTN_DONOR_HEIGHT_X  229
   #define BUTN_DONOR_HEIGHT_Y  169
   #define BUTN_DONOR_WEIGHT_X  413
   #define BUTN_DONOR_WEIGHT_Y  169
   #define BUTN_DONOR_GENDER_X  45
   #define BUTN_DONOR_GENDER_Y  169

// DONINFO Screen buttons and panels
   #define BUTN_DONOR_BLOOD_TYPE_X         45
   #define BUTN_DONOR_BLOOD_TYPE_Y         169
   #define BUTN_DONOR_HEMATOCRIT_X         229
   #define BUTN_DONOR_HEMATOCRIT_Y         169
   #define BUTN_DONOR_PLATELET_PRECOUNT_X  413
   #define BUTN_DONOR_PLATELET_PRECOUNT_Y  169

// DONORDL Screen buttons and panels
   #define BITMAP_DONOR_FRAME_X        52
   #define BITMAP_DONOR_FRAME_Y       174
   #define BITMAP_DONOR_IMAGE_X        56
   #define BITMAP_DONOR_IMAGE_Y       177
   #define BITMAP_DONOR_STATS_X       412
   #define BITMAP_DONOR_STATS_Y       174

// PREDICT Screen button and text widgets
   #define BUTN_DONOR_TIME_X      495
   #define BUTN_DONOR_TIME_Y      367
   #define BUTN_SELECT_PREVIOUS_X 171
   #define BUTN_SELECT_PREVIOUS_Y 139
   #define BUTN_SELECT_NEXT_X     171
   #define BUTN_SELECT_NEXT_Y     366
// #define PRODUCT_ESTIMATE_X     51  * unused
// #define PRODUCT_ESTIMATE_Y     111 * unused
// #define PRODUCT_OPTIONS_X      35  * unused
// #define PRODUCT_OPTIONS_Y      170 * unused

// KEYPAD Screen buttons and text widgets
   #define BUTN_KPAD_CLEAR_X   10
   #define BUTN_KPAD_CLEAR_Y    6
   #define BUTN_KPAD_9_X      118
   #define BUTN_KPAD_9_Y       49
   #define BUTN_KPAD_8_X       64
   #define BUTN_KPAD_8_Y       49
   #define BUTN_KPAD_7_X       10
   #define BUTN_KPAD_7_Y       49
   #define BUTN_KPAD_6_X      118
   #define BUTN_KPAD_6_Y       92
   #define BUTN_KPAD_5_X       64
   #define BUTN_KPAD_5_Y       92
   #define BUTN_KPAD_4_X       10
   #define BUTN_KPAD_4_Y       92
   #define BUTN_KPAD_3_X      118
   #define BUTN_KPAD_3_Y      135
   #define BUTN_KPAD_2_X       64
   #define BUTN_KPAD_2_Y      135
   #define BUTN_KPAD_1_X       10
   #define BUTN_KPAD_1_Y      135
   #define BUTN_KPAD_0_X       10
   #define BUTN_KPAD_0_Y      178
   #define BUTN_KPAD_ENTER_X   64
   #define BUTN_KPAD_ENTER_Y  178

// RUNPROC Screen button and text widgets
   #define BUTN_CURRENT_TIME_X 206
   #define BUTN_CURRENT_TIME_Y 109
   #define BUTN_TIME_MIN_X     256
   #define BUTN_TIME_MIN_Y     109
   #define TIMEBAR_X           309
   #define TIMEBAR_Y           109
   #define PRESSURE_X           54
   #define PRESSURE_Y          184
   #define PLATELET_X          206
   #define PLATELET_Y          184
   #define PLASMA_X            333
   #define PLASMA_Y            184
   #define RBC_X               460
   #define RBC_Y               184
   #define SQUEEZE_X            80
   #define SQUEEZE_Y           111

// ADJUST Screen button and text widgets
   #define  BUTN_DRAW_INCR_X          36
   #define  BUTN_DRAW_INCR_Y         203
   #define  BUTN_DRAW_DECR_X          36
   #define  BUTN_DRAW_DECR_Y         376
   #define  BUTN_RETURN_INCR_X       139
   #define  BUTN_RETURN_INCR_Y       203
   #define  BUTN_RETURN_DECR_X       139
   #define  BUTN_RETURN_DECR_Y       376
   #define  BUTN_AC_REACTION_INCR_X  259
   #define  BUTN_AC_REACTION_INCR_Y  203
   #define  BUTN_AC_REACTION_DECR_X  259
   #define  BUTN_AC_REACTION_DECR_Y  376
   #define  BUTN_CLUMPING_INCR_X     384
   #define  BUTN_CLUMPING_INCR_Y     203
   #define  BUTN_CLUMPING_DECR_X     384
   #define  BUTN_CLUMPING_DECR_Y     376
   #define  BUTN_SPILLOVER_X         497
   #define  BUTN_SPILLOVER_Y         185
   #define  BUTN_AIR_IN_PLASMA_X     497
   #define  BUTN_AIR_IN_PLASMA_Y     270
   #define  BUTN_SERVICE_DATA_X      497
   #define  BUTN_SERVICE_DATA_Y      355
   #define  BUTN_SALINE_BOLUS_X      497
   #define  BUTN_SALINE_BOLUS_Y      355
   #define  DONATION_TIME_ESTIMATE_X 108
   #define  DONATION_TIME_ESTIMATE_Y 109
   #define  DRAW_RETURN_X             30
   #define  DRAW_RETURN_Y            166
   #define  AC_REACTION_X            248
   #define  AC_REACTION_Y            166
   #define  CLUMPING_X               373
   #define  CLUMPING_Y               166
   #define  DRAW_CAP_X                35
   #define  DRAW_CAP_Y               241
   #define  RETURN_CAP_X             138
   #define  RETURN_CAP_Y             241
   #define  AC_REACTION_CAP_X        258
   #define  AC_REACTION_CAP_Y        241
   #define  CLUMPING_CAP_X           383
   #define  CLUMPING_CAP_Y           241

// ALARM Buttons button and text widgets
   #define  BITMAP_TEXT_BACKGROUND_X           48
   #define  BITMAP_TEXT_BACKGROUND_Y          140
   #define  BITMAP_TEXT_BITMAP_BACKGROUND_X    48
   #define  BITMAP_TEXT_BITMAP_BACKGROUND_Y   140
   #define  BITMAP_ALARM_BITMAP_BACKGROUND_X  265
   #define  BITMAP_ALARM_BITMAP_BACKGROUND_Y  145

//// STATUS BAR ////

// Status Bitmap coordinates
   #define STATBAR_X 107
   #define STATBAR_Y 28


//// STATUS LINE ////

// Status Line Bitmap coordinates
   #define STATLINE_X 0
   #define STATLINE_Y 0

    #define PRODUCT_WARNING_DELTA(__MULTIPLIER__) (59 * (__MULTIPLIER__))


// Coordinates for new bitmaps for Version 3.3a

// Bitmap coordinates for Runsumm3 - product label screen
   #define BITMAP_CARTOUCHE_LABEL_PRODUCT_X 122
   #define BITMAP_CARTOUCHE_LABEL_PRODUCT_Y 142

// Bitmap coordinates for Runsumm4 - product verification screen
   #define BITMAP_CARTOUCHE_PRODUCT_WARNING_X 335
   #define BITMAP_CARTOUCHE_PRODUCT_WARNING_Y 140

   #define BITMAP_BUTTON_PRODUCT_WARNING1_X 10
   #define BITMAP_BUTTON_PRODUCT_WARNING1_Y 140

   #define BITMAP_BUTTON_PRODUCT_WARNING2_X 10
   #define BITMAP_BUTTON_PRODUCT_WARNING2_Y BITMAP_BUTTON_PRODUCT_WARNING1_Y + PRODUCT_WARNING_DELTA(1)

   #define BITMAP_BUTTON_PRODUCT_WARNING3_X 10
   #define BITMAP_BUTTON_PRODUCT_WARNING3_Y BITMAP_BUTTON_PRODUCT_WARNING1_Y + PRODUCT_WARNING_DELTA(2)

   #define BITMAP_BUTTON_PRODUCT_WARNING4_X 10
   #define BITMAP_BUTTON_PRODUCT_WARNING4_Y BITMAP_BUTTON_PRODUCT_WARNING1_Y + PRODUCT_WARNING_DELTA(3)

   #define BITMAP_BUTTON_PRODUCT_WARNING5_X 10
   #define BITMAP_BUTTON_PRODUCT_WARNING5_Y BITMAP_BUTTON_PRODUCT_WARNING1_Y + PRODUCT_WARNING_DELTA(4)

   #define BITMAP_BUTTON_PRODUCT_WARNING6_X 10
   #define BITMAP_BUTTON_PRODUCT_WARNING6_Y BITMAP_BUTTON_PRODUCT_WARNING1_Y + PRODUCT_WARNING_DELTA(5)

// Bitmap coordinates for the Replacement Fluid connect screen
   #define BITMAP_PLACARD_REPLACE_FLUID_CONNECT_X 261
   #define BITMAP_PLACARD_REPLACE_FLUID_CONNECT_Y 140

   #define BITMAP_CARTOUCHE_REPLACE_FLUID_X 48
   #define BITMAP_CARTOUCHE_REPLACE_FLUID_Y 140

   #define BITMAP_BUTTON_CONFIRM_REPLACE_FLUID_X 491
   #define BITMAP_BUTTON_CONFIRM_REPLACE_FLUID_Y 146
   #define BITMAP_BUTTON_CANCEL_REPLACE_FLUID_X  491
   #define BITMAP_BUTTON_CANCEL_REPLACE_FLUID_Y  246

// Bitmap coordinates for the saline bolus button on the adjustment screen
   #define BITMAP_BUTTON_BOLUS_X   497
   #define BITMAP_BUTTON_BOLUS_Y   355


// Bitmap coordinates for the PREDICT screen
   #define BITMAP_PREDICT_NUMERIC_OPTIONS_X  52
   #define BITMAP_PREDICT_NUMERIC_OPTIONS_Y  135

// Bitmaps for re-worked runsumm1
// *** BITMAP_PLACARD_RUNSUMM1 - defined above - this release replaces the graphic

// Bitmap coordinates for the Configuration procedure priority screen
   #define BITMAP_CFGPP_PROC_NUM_BOX1_X 8
   #define BITMAP_CFGPP_PROC_NUM_BOX1_Y 187
   #define BITMAP_CFGPP_PROC_NUM_BOX2_X 8
   #define BITMAP_CFGPP_PROC_NUM_BOX2_Y 247
   #define BITMAP_CFGPP_PROC_NUM_BOX3_X 8
   #define BITMAP_CFGPP_PROC_NUM_BOX3_Y 307


   #define BITMAP_CFGPP_PROC_ROW_LINE1_X 49
   #define BITMAP_CFGPP_PROC_ROW_LINE1_Y 238
   #define BITMAP_CFGPP_PROC_ROW_LINE2_X 49
   #define BITMAP_CFGPP_PROC_ROW_LINE2_Y 298
   #define BITMAP_CFGPP_PROC_ROW_LINE3_X 49
   #define BITMAP_CFGPP_PROC_ROW_LINE3_Y 358

   #define BITMAP_BUTTON_CFGPP_UP_ARROW_X  172
   #define BITMAP_BUTTON_CFGPP_UP_ARROW_Y  117
   #define BITMAP_BUTTON_CFGPP_DN_ARROW_X  172
   #define BITMAP_BUTTON_CFGPP_DN_ARROW_Y  381

   #define BITMAP_BUTTON_CFGPP_COL1_X  55
   #define BITMAP_BUTTON_CFGPP_COL2_X  170
   #define BITMAP_BUTTON_CFGPP_COL3_X  285
   #define BITMAP_BUTTON_CFGPP_COL4_X  400
   #define BITMAP_BUTTON_CFGPP_COL5_X  515

   #define BITMAP_BUTTON_CFGPP_ROW1_Y  187
   #define BITMAP_BUTTON_CFGPP_ROW2_Y  247
   #define BITMAP_BUTTON_CFGPP_ROW3_Y  307

// Bitmap coordinates for the Configuration RBC screen
   #define BITMAP_CFGRBC_LABEL_BOX_X 20
   #define BITMAP_CFGRBC_LABEL_BOX1_Y 168
   #define BITMAP_CFGRBC_LABEL_BOX2_Y 233
   #define BITMAP_CFGRBC_LABEL_BOX3_Y 299

   #define BITMAP_CFGRBC_LABEL_LINE_X 20
   #define BITMAP_CFGRBC_LABEL_LINE1_Y 219
   #define BITMAP_CFGRBC_LABEL_LINE2_Y 284
   #define BITMAP_CFGRBC_LABEL_LINE3_Y 350

// Bitmap coordinates for the Configuration platelet screen
   #define BITMAP_CFGPLT_LABEL_BOX1_X 19
   #define BITMAP_CFGPLT_LABEL_BOX1_Y 168
   #define BITMAP_CFGPLT_LABEL_BOX2_X 19
   #define BITMAP_CFGPLT_LABEL_BOX2_Y 233
   #define BITMAP_CFGPLT_LABEL_BOX3_X 19
   #define BITMAP_CFGPLT_LABEL_BOX3_Y 299

   #define BITMAP_CFGPLT_LABEL_LINE1_X 118
   #define BITMAP_CFGPLT_LABEL_LINE1_Y 219
   #define BITMAP_CFGPLT_LABEL_LINE2_X 118
   #define BITMAP_CFGPLT_LABEL_LINE2_Y 284
   #define BITMAP_CFGPLT_LABEL_LINE3_X 118
   #define BITMAP_CFGPLT_LABEL_LINE3_Y 350

   #define BITMAP_BUTTON_CFGPLT_UP_ARROW_X  19
   #define BITMAP_BUTTON_CFGPLT_UP_ARROW_Y  121
   #define BITMAP_BUTTON_CFGPLT_DN_ARROW_X  19
   #define BITMAP_BUTTON_CFGPLT_DN_ARROW_Y  362

   #define BITMAP_BUTTON_CFGPLT_YSF_X  271
   #define BITMAP_BUTTON_CFGPLT_YSF_Y  369

   #define BITMAP_BUTTON_CFGPLT_COL1_X  144
   #define BITMAP_BUTTON_CFGPLT_COL2_X  304
   #define BITMAP_BUTTON_CFGPLT_COL3_X  495

   #define BITMAP_BUTTON_CFGPLT_ROW1_Y  168
   #define BITMAP_BUTTON_CFGPLT_ROW2_Y  233
   #define BITMAP_BUTTON_CFGPLT_ROW3_Y  299

// Barcode Screen Coordinates
   #define BITMAP_PLACARD_BARCODE_X 59
   #define BITMAP_PLACARD_BARCODE_Y 179
   #define BITMAP_BUTTON_BARCODE_INPUT_CLEAR_X 478
   #define BITMAP_BUTTON_BARCODE_INPUT_CLEAR_Y 138
   #define BITMAP_BUTTON_BARCODE_CATEGORY_CLEAR_X 478
   #define BITMAP_BUTTON_BARCODE_CATEGORY_CLEAR_Y 386
   #define BITMAP_WIDGET_CART_CAT_X 70       // (category cart. x)
   #define BITMAP_WIDGET_CART_CAT_Y 384      // (category cart. y)
   #define BITMAP_WIDGET_CART_IN_X 70        // (input cart. x)
   #define BITMAP_WIDGET_CART_IN_Y 136       // (input cart. y)



//////////////////////// End of X/Y coordinate manifests ////////////////////////

struct BITMAP_DATA_ENTRY
{
   bool                 absolutePath;
   int                  width;
   int                  height;

   const unsigned char* data;
   unsigned long        dataSize;

   const unsigned char* maskData;
   unsigned long        maskDataSize;
};

extern BITMAP_DATA_ENTRY bitmap_data_table[BITMAP_ID_COUNT];

#endif /* ifndef _BITMAP_INFO_INCLUDE */

/* FORMAT HASH bcfca5ef85d01f181c4e355b217dd85b */

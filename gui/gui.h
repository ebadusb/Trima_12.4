/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   N/A

*     File name:    gui.h

*     Contents:     See prototype header descriptions below...

*     Description:

         This file includes all of the general defines, manifests, and
         enums used throughout the GUI domain.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/gui.h 1.155 2009/07/07 21:40:48Z dslausb Exp dslausb $

**********************************************************************************
**********************************************************************************/


// Single include define guardian
#ifndef __GUI_H__
#define __GUI_H__


#ifndef WIN32
#include <vxworks.h> // This is needed here for SIMNT compiler directive.
#include "datalog.h" // Use for datalog stuff
#endif



////////////////////////////////////////////////////////////////////////////////
// This defines the maximum size of a window allocation parameter string

#define  MAXSIZE_BWINDOW_PARAM  101



////////////////////////////////////////////////////////////////////////////////
// This defines the abort screen's allocation parameter type, it precedes any
//  parameters on the allocation parameter string.
// An abort-confirm during an alarm
#define  ABORT_TYPE_ALARM       "ALARM"
// An abort-through-rinseback-confirm during an alarm
#define  ABORT_TYPE_ALARMRINSE  "RINSE"
// An abort confirm via any "cancel" key press
#define  ABORT_TYPE_RINSEBACK   "RINSEBACK"
// An abort confirm via any "cancel" key press
#define  ABORT_TYPE_USER        "USER"
// Abort metered storage
#define ABORT_TYPE_ADDITIVE_SOLUTIONS "ADDITIVE_SOLUTIONS"
// Abort metered storage with alarms
#define ABORT_TYPE_ADDITIVE_SOLUTIONS_ALARM "MSS_ALARM"
// A confirm spillover via key press
#define  CONFIRM_TYPE_SPILLOVER "SPILLOVER"
// A confirm air in plasma via key press
#define  CONFIRM_TYPE_AIR_IN_PLASMA "AIR_IN_PLASMA"
// A confirm saline bolus via key press
#define  CONFIRM_TYPE_SALINE_BOLUS "SALINE_BOLUS"

// If this is a substring somewhere of the allocation string
// for a child screen, you can show that child screen on top
// of a stateless screen.
#define CHILD_OF_STATELESS "childOfStateless"

////////////////////////////////////////////////////////////////////////////////
// These are the standard colors used by the window, status line and button classes

enum WINDOW_COLOR
{
   TEAL,
   BLUE,
   MAGENTA,
   GREY,
};


////////////////////////////////////////////////////////////////////////////////
// The status line enumerated types

enum STATUS_LINE_TYPE
{
   DEFAULT_STATUS_LINE,
   ATTENTION_STATUS_LINE,
   END_RUN_STATUS_LINE,
   ALERT_STATUS_LINE,
   ALARM_STATUS_LINE,
   NULL_STATUS_LINE
} ;

////////////////////////////////////////////////////////////////////////////////
// These are the types of status bars.  There's two types: the bar with the additive
// solution icon, and the one without.

enum STATUS_BAR_TYPE
{
   STATUS_BAR_TYPE_NO_ADDITION, // default
   STATUS_BAR_TYPE_WITH_ADDITION
};


////////////////////////////////////////////////////////////////////////////////
// These are the phase status states for the Apheresis screen's Status Bar
//  - RESET = indicates that the phase has not been activated during this donation
//  - ACTIVE = indicates that the phase is currently active
//  - COMPLETE = indicates that the phase has been completed

enum STATUS_BAR_STATE
{
   STATUS_BAR_STATE_RESET,
   STATUS_BAR_STATE_ACTIVE,
   STATUS_BAR_STATE_COMPLETE
};


////////////////////////////////////////////////////////////////////////////////
// These are the phase status indicators for the Apheresis screen's Status Bar

enum STATUS_BAR_PHASE
{
   FIRST_STATUS_BAR_PHASE,
   STATUS_BAR_LOAD=FIRST_STATUS_BAR_PHASE,
   STATUS_BAR_INFO,
   STATUS_BAR_PREPARE,
   STATUS_BAR_DONATE,
   STATUS_BAR_UNLOAD,
   STATUS_BAR_SOLUTIONS,
   LAST_STATUS_BAR_PHASE=STATUS_BAR_SOLUTIONS
};



////////////////////////////////////////////////////////////////////////////////
// These are the various intra-screen help-phase identifiers. These represent the
//  current logical help-state a screen is in when providing context-sensitive
//  help.

enum GUI_HELP_PHASE
{
   HELPPHASE_SYSDONOR_UNCHECKED,
   HELPPHASE_SYSDONOR_LOADSYS_CHECKED,
   HELPPHASE_SYSDONOR_ALL_CHECKED,

   HELPPHASE_DEFAULT
};


////////////////////////////////////////////////////////////////////////////////
// This is the status typedef'ed structure that will be maintained in the Apheresis
//  screen's Status Bar and also used as the target of a get_phases method call.

struct STATUS_BAR_PHASE_STATES
{
   STATUS_BAR_STATE status_load ;
   STATUS_BAR_STATE status_info ;
   STATUS_BAR_STATE status_prepare ;
   STATUS_BAR_STATE status_donate ;
   STATUS_BAR_STATE status_unload ;
   STATUS_BAR_STATE status_solutions;

   STATUS_BAR_PHASE_STATES()
      : status_load      (STATUS_BAR_STATE_RESET),
        status_info      (STATUS_BAR_STATE_RESET),
        status_prepare   (STATUS_BAR_STATE_RESET),
        status_donate    (STATUS_BAR_STATE_RESET),
        status_unload    (STATUS_BAR_STATE_RESET),
        status_solutions (STATUS_BAR_STATE_RESET)
   {}

};


////////////////////////////////////////////////////////////////////////////////
// These are the logical ID's for the GUI Screens

enum GUI_SCREEN_ID
{
   GUI_SCREEN_NULL_SCREEN,
   GUI_SCREEN_SYSDONOR,
   GUI_SCREEN_SYSDISP,
   GUI_SCREEN_SYSCLAMP,
   GUI_SCREEN_SYSTEST,
   GUI_SCREEN_ABORTCNF,
   GUI_SCREEN_CLUMP=GUI_SCREEN_ABORTCNF + 2, // Preserve 5.1 numbering despite GUI_SCREEN_BLOODSMP being removed.
   GUI_SCREEN_DISCONN,
   GUI_SCREEN_DONCONN,
   GUI_SCREEN_DONINFO,
   GUI_SCREEN_DONVITAL,
   GUI_SCREEN_KEYPAD,
   GUI_SCREEN_LISTBOX,
   GUI_SCREEN_POWERUP,
   GUI_SCREEN_RUNPROC,
   GUI_SCREEN_RUNSUMM1,
   GUI_SCREEN_RUNSUMM2,
   GUI_SCREEN_SYSACATT,
   GUI_SCREEN_TROUBLE,
   GUI_SCREEN_ALARM,
   GUI_SCREEN_PREDICT = (GUI_SCREEN_ALARM + 2),  // Preserve 5.1 numbering despite GUI_SCREEN_SERV1 being removed.
   GUI_SCREEN_CONFIG,
   GUI_SCREEN_CONFIG_MACHINE,
   GUI_SCREEN_CONFIG_LANGUAGE,
   GUI_SCREEN_CONFIG_PLASMA,
   GUI_SCREEN_CONFIG_PLATELET,
   GUI_SCREEN_CONFIG_PROCEDURE,
   GUI_SCREEN_CONFIG_RBC,
   GUI_SCREEN_CONFIG_DONOR,
   GUI_SCREEN_CONFIG_PRIORITY      = (GUI_SCREEN_CONFIG_DONOR + 2), // Preserve 5.1 numbering despite GUI_SCREEN_CONFIG_REPORT being removed.
   GUI_SCREEN_CONFIG_REPLACEMENT_FLUID,
   GUI_SCREEN_CONFIG_VOLUME_LIMITS = (GUI_SCREEN_CONFIG_REPLACEMENT_FLUID + 2),  // Preserve 5.1 numbering despite GUI_SCREEN_CONFIG_REPLACEMENT_FLUID being removed.
   GUI_SCREEN_SERVICE,
   GUI_SCREEN_CONFIRM_DON_INFO,
   GUI_SCREEN_REPFLUID,
   GUI_SCREEN_SETID,
   GUI_SCREEN_CFR11 = (GUI_SCREEN_SETID + 3),  // Preserve 5.1 numbering despite GUI_SCREEN_LABELPRD and GUI_SCREEN_VERIFPRD being removed.
   GUI_SCREEN_BARCODE,

   // New screens for 6.0
   GUI_SCREEN_SYSCASSETTE,
   GUI_SCREEN_PTF_MOUNT,
   GUI_SCREEN_PAS_INIT_SPLIT,
   GUI_SCREEN_CLAMP_CROSSOVER,
   GUI_SCREEN_PLT_LISTBOX,
   GUI_SCREEN_PLAS_LISTBOX,
   GUI_SCREEN_RBC_LISTBOX,
   GUI_SCREEN_SET_LISTBOX,
   GUI_SCREEN_LANG_LISTBOX,
   GUI_SCREEN_CONFIRM_BOX,
   GUI_SCREEN_RAS_CONNECT_CONFIRM_BOX,
   GUI_SCREEN_PAS_CONNECT_CONFIRM_BOX,
   GUI_SCREEN_SALINE_CONNECT_CONFIRM_BOX,
   GUI_SCREEN_AC_CONNECT_CONFIRM_BOX,
   GUI_SCREEN_REBOOT_CONFIRM_BOX,
   GUI_SCREEN_PREDICT_CONFIRM_BOX,
   GUI_SCREEN_RUNSUM_PLT,
   GUI_SCREEN_RUNSUM_PLASMA,
   GUI_SCREEN_RUNSUM_RBC,
   GUI_SCREEN_RUNSUM_RBC_PROD1, // These are separate screens for
   GUI_SCREEN_RUNSUM_RBC_PROD2, // the DRBC autosplit.
   GUI_SCREEN_TOUCHSCREEN_CAL,
   GUI_SCREEN_CONFIG_METERED_SOLUTIONS,
   GUI_SCREEN_AAS_CLAMP_CHANNEL,
   GUI_SCREEN_AAS_CHANNEL_TEST,
   GUI_SCREEN_AAS_CONNECT_PAS,
   GUI_SCREEN_AAS_CONNECT_RAS,
   GUI_SCREEN_AAS_PRIME,
   GUI_SCREEN_AAS_PAS_SPLIT,
   GUI_SCREEN_AAS_ADDITION,
   GUI_SCREEN_MSS_END,
   GUI_SCREEN_AAS_RUN_SUMMARY,
   GUI_SCREEN_DRBC_SPLIT,
   GUI_SCREEN_AAS_TRANSITION_DUMMY,
   GUI_SCREEN_POWERDOWN,
   GUI_SCREEN_PAS_EMPTY,
   GUI_SCREEN_JP_CLAMP_BLOOD_DIVERSION,
   GUI_SCREEN_RBC_SPLIT_INIT,
   GUI_SCREEN_SEAL_AND_DETACH,
   GUI_SCREEN_PREDICT_COMP_PLS_CONFIRM_BOX,
   GUI_SCREEN_CFG_PLT_AMAP,

   // New screens for 6.3
   GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS,
   GUI_SCREEN_KEYBOARD,
   GUI_SCREEN_LISTVIEW,
   GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG,

   // New screens for 6.4
   GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX,

   GUI_SCREEN_TOTAL_SCREENS // last entry
};


#ifndef WIN32
// routine to decode screen names.
DataLog_Stream& operator << (DataLog_Stream& os, GUI_SCREEN_ID id);
#endif


////////////////////////////////////////////////////////////////////////////////
// These identify how a particular screen has been invoked by the screen commutator.
// This status is stored in the Basic Window base class.

enum GUI_SCREEN_INVOCATION
{
   GUI_SCREEN_INVOKE_STATIC,
   GUI_SCREEN_INVOKE_CHILD,
   GUI_SCREEN_INVOKE_STATELESS,
   GUI_SCREEN_INVOKE_NOTACTIVE
};


////////////////////////////////////////////////////////////////////////////////
// These are the logical ID's for the GUI Buttons

enum GUI_BUTTON_ID
{
   GUI_BUTTON_NULL_BUTTON,
   GUI_BUTTON_GOBACK,
   GUI_BUTTON_CANCEL,
   // GUI_BUTTON_HELP,
   GUI_BUTTON_CONTINUE= (GUI_BUTTON_CANCEL + 2),  // Preserve 5.1 numbering even though GUI_BUTTON_HELP is gone
   GUI_BUTTON_LOWLEFT,
   GUI_BUTTON_LOADSYS,
   GUI_BUTTON_DONORINFO,
   GUI_BUTTON_DISCONN_CONFIRM,
   GUI_BUTTON_DONOR_GENDER,
   GUI_BUTTON_DONOR_HEIGHT,
   GUI_BUTTON_DONOR_WEIGHT,
   GUI_BUTTON_DONOR_BLOOD_TYPE,
   GUI_BUTTON_DONOR_HEMATOCRIT,
   GUI_BUTTON_DONOR_PLATELET_PRECOUNT,
   GUI_BUTTON_PRED_DONOR_TIME,
   GUI_BUTTON_PRED_SELECT_PREVIOUS,
   GUI_BUTTON_PRED_SELECT_NEXT,
   GUI_BUTTON_KPAD_CLEAR,
   GUI_BUTTON_KPAD_1,
   GUI_BUTTON_KPAD_2,
   GUI_BUTTON_KPAD_3,
   GUI_BUTTON_KPAD_4,
   GUI_BUTTON_KPAD_5,
   GUI_BUTTON_KPAD_6,
   GUI_BUTTON_KPAD_7,
   GUI_BUTTON_KPAD_8,
   GUI_BUTTON_KPAD_9,
   GUI_BUTTON_KPAD_0,
   GUI_BUTTON_KPAD_ENTER,
   GUI_BUTTON_RUN_CURRENT_TIME,
   GUI_BUTTON_RUN_TIME_MIN,
   GUI_BUTTON_ADJUST_DRAW_UP,
   GUI_BUTTON_ADJUST_DRAW_DN,
   GUI_BUTTON_ADJUST_RETURN_UP,
   GUI_BUTTON_ADJUST_RETURN_DN,
   GUI_BUTTON_ADJUST_AC_UP,
   GUI_BUTTON_ADJUST_AC_DN,
   GUI_BUTTON_ADJUST_CLUMP_UP,
   GUI_BUTTON_ADJUST_CLUMP_DN,
   GUI_BUTTON_ADJUST_SPILLOVER,
   GUI_BUTTON_ADJUST_AIR_PLASMA,
   GUI_BUTTON_ADJUST_SALINE_BOLUS,
   GUI_BUTTON_ADJUST_SERVICE_DATA,
   GUI_BUTTON_ALARM_CONTINUE,
   GUI_BUTTON_ALARM_RINSEBACK,
   GUI_BUTTON_ALARM_EXIT,
   GUI_BUTTON_ALARM_ADJUST,
   GUI_BUTTON_ALARM_UNLOAD,
   GUI_BUTTON_CONFIG_MACHINE,
   GUI_BUTTON_ABORTCNF_EXIT,
   GUI_BUTTON_ABORTCNF_NOEXIT,
   GUI_BUTTON_CONFIG_LISTBOX_1,
   GUI_BUTTON_CONFIG_LISTBOX_2,
   GUI_BUTTON_CONFIG_LISTBOX_3,
   GUI_BUTTON_CONFIG_LISTBOX_4,
   GUI_BUTTON_CONFIG_LISTBOX_5,
   GUI_BUTTON_CONFIG_LISTBOX_6,
   GUI_BUTTON_CONFIG_LISTBOX_7,
   GUI_BUTTON_CONFIG_LISTBOX_8,
   GUI_BUTTON_CONFIG_LISTBOX_9,
   GUI_BUTTON_CONFIG_LISTBOX_10,
   GUI_BUTTON_CONFIG_LANGUAGE,
   GUI_BUTTON_CONFIG_PLASMA,
   GUI_BUTTON_CONFIG_RBC,
   GUI_BUTTON_CONFIG_PROCEDURE,
   GUI_BUTTON_CONFIG_DONOR,
   GUI_BUTTON_CONFIG_VOLUME_LIMITS,
   GUI_BUTTON_CONFIG_PLATELET,
   GUI_BUTTON_CONFIG_SERVICE,
   GUI_BUTTON_CONFIG_PRIORITY,
   GUI_BUTTON_CONFIG_REPLACEMENT_FLUID,
   GUI_BUTTON_CONFIG_SELECT_LANGUAGE,
   GUI_BUTTON_CONFIG_HEIGHT,
   GUI_BUTTON_CONFIG_WEIGHT,
   GUI_BUTTON_CONFIG_DATE_FORMAT,
   GUI_BUTTON_CONFIG_TIME_FORMAT,
   GUI_BUTTON_CONFIG_DECIMAL_DELIMITER,
   // GUI_BUTTON_CONFIG_PP_LABEL_BOX,
   // GUI_BUTTON_CONFIG_PP_LABEL_LINE,
   GUI_BUTTON_CONFIG_DONOR_AC_RATE=GUI_BUTTON_CONFIG_DECIMAL_DELIMITER + 3, // Preserve ordering from 5.1
   GUI_BUTTON_CONFIG_DONOR_POST_CRIT,
   GUI_BUTTON_CONFIG_DONOR_POST_PLAT,
   GUI_BUTTON_CONFIG_DONOR_VOLUME,

#if 1
   GUI_BUTTON_CONFIG_DONOR_WEIGHT,
   GUI_BUTTON_CONFIG_DONOR_WEIGHT_LESS_THAN_VOL,
   GUI_BUTTON_CONFIG_DONOR_WEIGHT_GREATER_THAN_VOL,
#else
   GUI_BUTTON_CONFIG_DONOR_WEIGHT_1,
   GUI_BUTTON_CONFIG_DONOR_WEIGHT_2,
   GUI_BUTTON_CONFIG_DONOR_WEIGHT_3,
   GUI_BUTTON_CONFIG_DONOR_WEIGHT_1_LESS_THAN_VOL,
   GUI_BUTTON_CONFIG_DONOR_WEIGHT_2_LESS_THAN_VOL,
   GUI_BUTTON_CONFIG_DONOR_WEIGHT_3_LESS_THAN_VOL,
   GUI_BUTTON_CONFIG_DONOR_WEIGHT_3_GREATER_THAN_VOL,
#endif

   GUI_BUTTON_CONFIG_DONOR_TBV,
   GUI_BUTTON_CONFIG_DONOR_TBV_PERCENT,
   GUI_BUTTON_CONFIG_DONOR_BODY_WEIGHT,
   GUI_BUTTON_CONFIG_DONOR_ML_PER_KG,
   GUI_BUTTON_CONFIG_DONOR_PLATELET_VOL_MAX,
   GUI_BUTTON_CONFIG_DONOR_PLASMA_VOL_MAX,
   GUI_BUTTON_CONFIG_PLS_VOLUME_1,
   GUI_BUTTON_CONFIG_PLS_VOLUME_2,
   GUI_BUTTON_CONFIG_PLS_VOLUME_3,
   GUI_BUTTON_CONFIG_PLS_VOLUME_4,
   GUI_BUTTON_CONFIG_PLS_VOLUME_5,
   GUI_BUTTON_CONFIG_PLS_VOLUME_6,
   GUI_BUTTON_CONFIG_PLS_AMAP_MIN,
   GUI_BUTTON_CONFIG_PLS_AMAP_MAX,
   GUI_BUTTON_CONFIG_PLT_LABEL_BOX,
   GUI_BUTTON_CONFIG_PLT_LABEL_LINE,
   GUI_BUTTON_CONFIG_YSF,
   // GUI_BUTTON_CONFIG_GUARANTEED_YIELD,
   // GUI_BUTTON_CONFIG_GUARANTEED_YIELD_CV=GUI_BUTTON_CONFIG_YSF,
   // GUI_BUTTON_CONFIG_GUARANTEED_YIELD_CL,
   GUI_BUTTON_CONFIG_PROCEDURE_TIME=GUI_BUTTON_CONFIG_YSF + 4, // Preserve 5.1 numbering
   GUI_BUTTON_CONFIG_RETURN_PRESSURE,
   GUI_BUTTON_CONFIG_DRAW_PRESSURE,
   GUI_BUTTON_CONFIG_CUSTOM_RATIO,
   GUI_BUTTON_CONFIG_RBC_CRIT_1,
   GUI_BUTTON_CONFIG_RBC_DOSE_1,
   GUI_BUTTON_CONFIG_RBC_VOLUME_1,
   GUI_BUTTON_CONFIG_RBC_CRIT_2,
   GUI_BUTTON_CONFIG_RBC_DOSE_2,
   GUI_BUTTON_CONFIG_RBC_VOLUME_2,
   GUI_BUTTON_CONFIG_RBC_CRIT_3,
   GUI_BUTTON_CONFIG_RBC_DOSE_3,
   GUI_BUTTON_CONFIG_RBC_VOLUME_3,
   GUI_BUTTON_CONFIG_RBC_RATIO,
   GUI_BUTTON_CONFIG_PLASMA_RATIO,
   GUI_BUTTON_CONFIG_RBC_REPLACE_FLUID,
   GUI_BUTTON_CONFIG_RBC_FLUID_PERCENT,
   GUI_BUTTON_CONFIG_DRBC_ALERT,
   GUI_BUTTON_CONFIG_MIN_REPLACE_VOL,
   GUI_BUTTON_CONFIG_MAX_DRAW_FLOW,
   GUI_BUTTON_CONFIG_MACHINE_SET_TIME,
   GUI_BUTTON_CONFIG_MACHINE_SET_DATE,
   GUI_BUTTON_CONFIG_INLET_MANAGEMENT,
   GUI_BUTTON_CONFIG_RETURN_MANAGEMENT,
   GUI_BUTTON_CONFIG_INLET_FLOW_RAMP,
   GUI_BUTTON_CONFIG_RSF,
   // GUI_BUTTON_CONFIG_RINSEBACK_PROTOCOL,
   // GUI_BUTTON_CONFIG_AUDIT_TRACKING=GUI_BUTTON_CONFIG_RSF + 2,
   GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON = GUI_BUTTON_CONFIG_RSF + 3,   // Preserve 5.1 numbering
   GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
   // GUI_SERVICE_MAIN_BTN,
   GUI_BUTTON_SETID_PLT_PLS = GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON + 2,  // Preserve 5.1 numbering even though GUI_SERVICE_MAIN_BTN no longer exists
   GUI_BUTTON_SETID_PLT_PLS_RBC,
   GUI_BUTTON_SETID_RBC_PLS,
   GUI_BUTTON_SETID_LOAD_NEW,
   GUI_BUTTON_REPFLUID_CONTINUE,
   GUI_BUTTON_REPFLUID_CANCEL,
   GUI_BUTTON_PRODVERIF_1,
   GUI_BUTTON_PRODVERIF_2,
   GUI_BUTTON_PRODVERIF_3,
   GUI_BUTTON_PRODVERIF_4,
   GUI_BUTTON_PRODVERIF_5,
   GUI_BUTTON_PRODVERIF_6,
   GUI_BUTTON_BARCODE_CAT_NOTE,
   GUI_BUTTON_BARCODE_CAT_AC,
   GUI_BUTTON_BARCODE_CAT_REPLACEMENT_FLUID,
   GUI_BUTTON_BARCODE_CAT_DONOR,
   GUI_BUTTON_BARCODE_CAT_DONATION_ID,
   GUI_BUTTON_BARCODE_CAT_OPERATOR,
   GUI_BUTTON_BARCODE_CAT_SAMPLE_BAG,
   // GUI_BUTTON_BARCODE_CAT_STORAGE_SOL,
   GUI_BUTTON_BARCODE_CAT_BLOOD_TUBING_SET=GUI_BUTTON_BARCODE_CAT_SAMPLE_BAG + 2, // Preserve 5.1 numbering
   GUI_BUTTON_BARCODE_CAT_MISC1,
   GUI_BUTTON_BARCODE_CAT_MISC2,
   GUI_BUTTON_BARCODE_CAT_MISC3,
   GUI_BUTTON_BARCODE_CAT_MISC4,
   GUI_BUTTON_BARCODE_RANK_SWITCH,
   GUI_BUTTON_BARCODE_INPUT_CLEAR,
   GUI_BUTTON_BARCODE_CATEGORY_CLEAR,
   GUI_BUTTON_CFR_SELECTED,
   GUI_BUTTON_CFR_CLEAR_BADGE,
   GUI_BUTTON_CFR_SCROLL_UP,
   GUI_BUTTON_CFR_SCROLL_DN,
   GUI_BUTTON_CFR_LIST_1,
   GUI_BUTTON_CFR_LIST_2,
   GUI_BUTTON_CFR_LIST_3,
   GUI_BUTTON_CFR_LIST_4,
   GUI_BUTTON_RUNSUMMARY2_RESET_MACHINE,

   // 6.0 Buttons
   GUI_BUTTON_SCREENSHOT,
   GUI_BUTTON_PRED_FLOW_RATE,
   GUI_BUTTON_ABORTCNF_ABORT_RAS,
   GUI_BUTTON_ABORTCNF_ABORT_PAS,

   GUI_BUTTON_CASSETTE_LISTBOX_1,
   GUI_BUTTON_CASSETTE_LISTBOX_2,
   GUI_BUTTON_CASSETTE_LISTBOX_3,
   GUI_BUTTON_CASSETTE_LISTBOX_4,
   GUI_BUTTON_CASSETTE_LISTBOX_5,

   GUI_BUTTON_DYNAMIC_LISTBOX_1,
   GUI_BUTTON_DYNAMIC_LISTBOX_2,
   GUI_BUTTON_DYNAMIC_LISTBOX_3,
   GUI_BUTTON_DYNAMIC_LISTBOX_4,
   GUI_BUTTON_DYNAMIC_LISTBOX_5,

   // Configuration Main Menu
   GUI_BUTTON_CONFIG_METERED_STORAGE_SOLUTION,
   GUI_BUTTON_CONFIG_PLT_MSS_SPLIT_NOTIF,
   GUI_BUTTON_CONFIG_SHOW_RESIDUAL_LOSS,
   GUI_BUTTON_CONFIG_OVERRIDE_PAS_BAG_VOL,

   // Other configuration buttons
   GUI_BUTTON_CONFIG_CRIT_OR_GLOB,
   GUI_BUTTON_CONFIG_PP_LABEL_BOX_1,
   GUI_BUTTON_CONFIG_PP_LABEL_LINE_1,
   GUI_BUTTON_CONFIG_PP_PLATELETS_1,
   GUI_BUTTON_CONFIG_PP_PLASMA_1,
   GUI_BUTTON_CONFIG_PP_RBC_1,
   GUI_BUTTON_CONFIG_PP_TYPE_1,
   GUI_BUTTON_CONFIG_PP_TIME_1,
   GUI_BUTTON_CONFIG_PP_LABEL_BOX_2,
   GUI_BUTTON_CONFIG_PP_LABEL_LINE_2,
   GUI_BUTTON_CONFIG_PP_PLATELETS_2,
   GUI_BUTTON_CONFIG_PP_PLASMA_2,
   GUI_BUTTON_CONFIG_PP_RBC_2,
   GUI_BUTTON_CONFIG_PP_TYPE_2,
   GUI_BUTTON_CONFIG_PP_TIME_2,
   GUI_BUTTON_CONFIG_PP_LABEL_BOX_3,
   GUI_BUTTON_CONFIG_PP_LABEL_LINE_3,
   GUI_BUTTON_CONFIG_PP_PLATELETS_3,
   GUI_BUTTON_CONFIG_PP_PLASMA_3,
   GUI_BUTTON_CONFIG_PP_RBC_3,
   GUI_BUTTON_CONFIG_PP_TYPE_3,
   GUI_BUTTON_CONFIG_PP_TIME_3,
   GUI_BUTTON_CONFIG_DONOR_BODY_VOL,
   GUI_BUTTON_CONFIG_MALE_ONLY_PLT,
   GUI_BUTTON_CONFIG_MALE_ONLY_PLASMA,
   GUI_BUTTON_CONFIG_FFP_VOLUME,
   GUI_BUTTON_CONFIG_PLT_PCT_PLAS_CO,
   GUI_BUTTON_CONFIG_PLT_PROD_1,
   GUI_BUTTON_CONFIG_PLT_YIELD_1,
   GUI_BUTTON_CONFIG_PLT_FINAL_CONC_1,
   GUI_BUTTON_CONFIG_PLT_PAS_TOGGLE_1,
   GUI_BUTTON_CONFIG_PLT_PLASMA_PERCENT_1,
   GUI_BUTTON_CONFIG_PLT_VOL_1,
   GUI_BUTTON_CONFIG_PLT_PROD_2,
   GUI_BUTTON_CONFIG_PLT_YIELD_2,
   GUI_BUTTON_CONFIG_PLT_FINAL_CONC_2,
   GUI_BUTTON_CONFIG_PLT_PAS_TOGGLE_2,
   GUI_BUTTON_CONFIG_PLT_PLASMA_PERCENT_2,
   GUI_BUTTON_CONFIG_PLT_VOL_2,
   GUI_BUTTON_CONFIG_PLT_PROD_3,
   GUI_BUTTON_CONFIG_PLT_YIELD_3,
   GUI_BUTTON_CONFIG_PLT_FINAL_CONC_3,
   GUI_BUTTON_CONFIG_PLT_PAS_TOGGLE_3,
   GUI_BUTTON_CONFIG_PLT_PLASMA_PERCENT_3,
   GUI_BUTTON_CONFIG_PLT_VOL_3,
   GUI_BUTTON_CONFIG_RBC_LABEL_LINE_1,
   GUI_BUTTON_CONFIG_RBC_PROD_1,
   GUI_BUTTON_CONFIG_RBC_LABEL_LINE_2,
   GUI_BUTTON_CONFIG_RBC_PROD_2,
   GUI_BUTTON_CONFIG_RBC_LABEL_LINE_3,
   GUI_BUTTON_CONFIG_RBC_PROD_3,
   GUI_BUTTON_CONFIG_MACHINE_SET_ID,
   GUI_BUTTON_CONFIG_MACHINE_SET_CODE,
   GUI_BUTTON_CONFIG_MACHINE_ADD_SET,
   GUI_BUTTON_CONFIG_MACHINE_REMOVE_SET,
   GUI_BUTTON_CONFIG_MACHINE_SET_AUDIO,
   GUI_BUTTON_CONFIG_PLASMA_RINSEBACK,
   GUI_BUTTON_CONFIG_SALINE_RINSEBACK,
   GUI_BUTTON_CONFIG_BLOOD_DIVERSION,
   GUI_BUTTON_CONFIG_AIR_REMOVAL,
   GUI_BUTTON_CONFIG_PPC_ON_OFF,
   GUI_BUTTON_CONFIG_RBC_SOL_TOGGLE,
   GUI_BUTTON_CONFIG_PTF_TOGGLE,

   GUI_BUTTON_CONFIG_DEFAULT_PLT_SS_BAG_VOLM,
   GUI_BUTTON_CONFIG_PLASMA_CARRYOVER,
   GUI_BUTTON_CONFIG_PLASMA_CARRYOVER_VALUE,
   GUI_BUTTON_CONFIG_COMPLEMENTARY_PLASMA,
   // CONFIG Listbox up/down scroll buttons

   GUI_BUTTON_CASSETTE_LISTBOX_UP_BUTTON,
   GUI_BUTTON_CASSETTE_LISTBOX_DOWN_BUTTON,

   GUI_BUTTON_DYNAMIC_LISTBOX_UP_BUTTON,
   GUI_BUTTON_DYNAMIC_LISTBOX_DOWN_BUTTON,

   GUI_BUTTON_CHOOSE_CASSETTE,

   // REPLACEMENT FLUID prompt screen
   GUI_BUTTON_REPFLUID_RINSEBACK,

   // DRBC SPLIT prompt screen
   GUI_BUTTON_DRBC_SPLIT_CONTINUE,
   GUI_BUTTON_DRBC_SPLIT_CANCEL,

   // PAS EMPTY prompt screen
   GUI_BUTTON_PAS_EMPTY_CONTINUE,
   GUI_BUTTON_PAS_EMPTY_CANCEL,

   GUI_BUTTON_BARCODE_CAT_RBC_STORAGE_SOL,
   GUI_BUTTON_BARCODE_CAT_PLT_STORAGE_SOL,
   GUI_BUTTON_BARCODE_CAT_DATA,

   // MSS Buttons
   GUI_BUTTON_MSS_BAG_VOL_PLAT,
   GUI_BUTTON_MSS_BAG_VOL_RBC,


   GUI_BUTTON_CONFIRM_BOX_CONFIRM,
   GUI_BUTTON_CONFIRM_BOX_CANCEL,

   GUI_BUTTON_CONFIG_SHOW_PRE_AAS_FLAGS,

   GUI_BUTTON_CFG_LANG,

   GUI_BUTTON_SERVICE_TOUCHSCREEN_CAL,
   GUI_BUTTON_DON_CONF_REVITALS,
   GUI_BUTTON_CONFIG_DRBC_VOL_LIM,
   GUI_BUTTON_CONFIG_PLATELET_VOL_MAX_TOGGLE,
   GUI_BUTTON_CONFIG_PLASMA_VOL_MAX_TOGGLE,

   GUI_BUTTON_BARCODE_CAT_MISC5,


   GUI_BUTTON_CFG_PLT_AMAP_SINGLE_MIN,
   GUI_BUTTON_CFG_PLT_AMAP_SINGLE_MAX,
   GUI_BUTTON_CFG_PLT_AMAP_DOUBLE_MIN,
   GUI_BUTTON_CFG_PLT_AMAP_DOUBLE_MAX,
   GUI_BUTTON_CFG_PLT_AMAP_TRIPLE_MIN,
   GUI_BUTTON_CFG_PLT_AMAP_TRIPLE_MAX,

   GUI_BUTTON_CFG_PLT_AMAP_SINGLE_CONC,
   GUI_BUTTON_CFG_PLT_AMAP_DOUBLE_CONC,
   GUI_BUTTON_CFG_PLT_AMAP_TRIPLE_CONC,
   GUI_BUTTON_SELECT_DIFFERENT_PRODUCT,
   GUI_BUTTON_PREDICT_MORE_OR_LESS,

   GUI_BUTTON_DON_CONN_START_DRAW,

   GUI_BUTTON_CONFIG_SOFT_OPTION,
   GUI_BUTTON_CONFIG_SOFT_OPTION_KEYBOARD,
   GUI_BUTTON_CONFIG_SOFT_OPTION_UP,
   GUI_BUTTON_CONFIG_SOFT_OPTION_DN,
   /*
      Down, are the ids for the alpha keyboard.
      These need to be contiguous. Do not insert
      in between.
   */
   GUI_BUTTON_KEYBOARD_0,
   GUI_BUTTON_KEYBOARD_1,
   GUI_BUTTON_KEYBOARD_2,
   GUI_BUTTON_KEYBOARD_3,
   GUI_BUTTON_KEYBOARD_4,
   GUI_BUTTON_KEYBOARD_5,
   GUI_BUTTON_KEYBOARD_6,
   GUI_BUTTON_KEYBOARD_7,
   GUI_BUTTON_KEYBOARD_8,
   GUI_BUTTON_KEYBOARD_9,
   GUI_BUTTON_KEYBOARD_A,
   GUI_BUTTON_KEYBOARD_B,
   GUI_BUTTON_KEYBOARD_C,
   GUI_BUTTON_KEYBOARD_D,
   GUI_BUTTON_KEYBOARD_E,
   GUI_BUTTON_KEYBOARD_F,
   GUI_BUTTON_KEYBOARD_G,
   GUI_BUTTON_KEYBOARD_H,
   GUI_BUTTON_KEYBOARD_I,
   GUI_BUTTON_KEYBOARD_J,
   GUI_BUTTON_KEYBOARD_K,
   GUI_BUTTON_KEYBOARD_L,
   GUI_BUTTON_KEYBOARD_M,
   GUI_BUTTON_KEYBOARD_N,
   GUI_BUTTON_KEYBOARD_O,
   GUI_BUTTON_KEYBOARD_P,
   GUI_BUTTON_KEYBOARD_Q,
   GUI_BUTTON_KEYBOARD_R,
   GUI_BUTTON_KEYBOARD_S,
   GUI_BUTTON_KEYBOARD_T,
   GUI_BUTTON_KEYBOARD_U,
   GUI_BUTTON_KEYBOARD_V,
   GUI_BUTTON_KEYBOARD_W,
   GUI_BUTTON_KEYBOARD_X,
   GUI_BUTTON_KEYBOARD_Y,
   GUI_BUTTON_KEYBOARD_Z,
   GUI_BUTTON_KEYBOARD_BK,
   GUI_BUTTON_KEYBOARD_CLR,
   GUI_BUTTON_KEYBOARD_CR,

   // new to 6.3
   GUI_BUTTON_PLTSPLIT_UNLOAD,
   GUI_BUTTON_JP_PLS_CLAMP_UNLOAD,

   // autoflow
   GUI_BUTTON_PRED_AF_ADJUST,
   GUI_BUTTON_PRED_AF_MUTE,
   GUI_BUTTON_CFG_AUTOFLOW,




   /*
      Up, are the ids for the alpha keyboard.
      These need to be contiguous. Do not insert
      in between.
   */
    #if CPU==SIMNT
   GUI_BUTTON_TOTAL_BUTTONS,
    #endif

   GUI_BUTTON_INVALID           // this is our end-of-enum-range sentinel, don't
                                // add anything after it.


};


#ifndef WIN32
// routine to decode screen names.
DataLog_Stream& operator << (DataLog_Stream& os, GUI_BUTTON_ID id);
#endif

const char* btnIdToStr (GUI_BUTTON_ID id);
const char* scrnIdToStr (GUI_SCREEN_ID id);


////////////////////////////////////////////////////////////////////////////////
// These are the logical ID's for router timer objects defined within GUI

// TIMER IDENTIFIERS
enum TIMER_IDS
{
   TIMER_ANIMATION,
   TIMER_HALFSEC_DELAY,
   TIMER_DISP_LOWERED,
   TIMER_DISCONN_CONFIRM_TIMEOUT,
   TIMER_SCREEN_TIMEOUT,
   TIMER_SCREEN_ALARM_TIMEOUT,
   TIMER_MUTE_TIMEOUT
};

#define  TIMER_RESET                   0   // 0 seconds will turn off the period timer.
#define  ANIMATION_PERIOD           1500   // 1.5 Seconds between animation cells
#define  COMPLETION_NOTICE_PERIOD    500   // 0.5 Seconds wait after notifying of test completion
#define  DISP_LOWERED_MSG_PERIOD    1000   // 1.0 Seconds wait after displaying message about disp lowered
#define  DISCONN_CONFIRM_PERIOD    10000   // 10.0 Seconds wait before removing the confirm button.
#define  SCREEN_TIMEOUT_PERIOD     30000   // 30.0 Seconds wait before timing ou to nex screen.
#define  TIMER_SCREEN_ALARM_PERIOD 30000   // 30.0 Seconds wait before alarming on lack of predict screen user confirm.
#define  MUTE_PERIOD              60000    // 60.0 Seconds wait before restarting the alarm sounds.


////////////////////////////////////////////////////////////////////////////////
// Define Include files

// Logical ID's for all the labeling and text
#include "string_info.h"

// Logical ID's for all the bitmap files
#include "bitmap_info.h"

#ifndef WIN32
// Internal GUI to GUI messaging structures
// #include "iguimsgs.h"

// Basic graphics elements
// #include "osgraphics.h"

////////////////////////////////////////////////////////////////////////////////
#endif

////////////////////////////////////////////////////////////////////////////////
// End of single include define guardian
#endif

/* FORMAT HASH ae5d730a5642b172a3bcbb175c7a770f */

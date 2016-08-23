/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael J Cobb

*     Class name:  N/A  alarm declarations and attributes table

*     File name:   alarmtb.cpp

*     Contents:    alarm declarations, attributes table,

*     Description:

*************************< FOCUSSED GUI DOMAIN >****************************
$Header$
**********************************************************************************
 NOTE: This file is shared with Vista. Take care when adding new dependencies
**********************************************************************************/

#include "stdafx.h"

#ifdef WIN32
   #include <stdlib.h>
#else
   #include <vxworks.h> // Needed for #if CPU==SIMNT directives to work
#endif

// INCLUDE the GUI MANIFESTS
#include "gui.h"
#include "alarmtb.h"

#if CPU == SIMNT
   #define NAME_LITERAL(name)   name,
#else
   #define NAME_LITERAL(name)
#endif /* if CPU == SIMNT */

#define OVERRIDE_DEFAULT(__SCREEN_NAME__, __STRING_PARAM__) ALARM_OPT_NONE, textEmpty, textEmpty, BITMAP_NULL, __SCREEN_NAME__, __STRING_PARAM__


#ifdef WIN32
# define ALARM_TABLE_T ALARM_TABLE
#else
# define ALARM_TABLE_T static const ALARM_TABLE
#endif

ALARM_TABLE_T alarms [] =
{


// *********************************************************************************
// the following is the alarm table structure to be used in defining the individual
// alarm screens and their traversal

   {
      plus12V_TOO_HIGH,
      NAME_LITERAL("plus12V TOO HIGH")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmVoltageFailure,
   },

   {
      plus12V_TOO_LOW,
      NAME_LITERAL("plus12V TOO LOW")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmVoltageFailure,
   },

   {
      plus24V_TOO_HIGH,
      NAME_LITERAL("plus24V TOO HIGH")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmVoltageFailure,
   },

   {
      plus24V_TOO_LOW,
      NAME_LITERAL("plus24V TOO LOW")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmVoltageFailure,
   },

   {
      plus64V_TOO_HIGH,
      NAME_LITERAL("plus64V TOO HIGH")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmVoltageFailure,
   },

   {
      plus64V_TOO_LOW,
      NAME_LITERAL("plus64V TOO LOW")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmVoltageFailure,
   },

   //////////////////////////////////////////////////////////////////////

   {
      ACCESS_PRESSURE_DECAY_FAILURE,
      NAME_LITERAL("ACCESS PRESSURE DECAY FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailureCassette,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET
   },

   {
      ACCESS_PRESSURE_DECAY_FAILURE,
      NAME_LITERAL("ACCESS PRESSURE DECAY FAILURE 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure01Ais,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_01
   },

   {
      ACCESS_PRESSURE_DECAY_FAILURE,
      NAME_LITERAL("ACCESS PRESSURE DECAY FAILURE 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure02,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_02
   },

   {
      ACCESS_PRESSURE_DECAY_FAILURE,
      NAME_LITERAL("ACCESS PRESSURE DECAY FAILURE 03")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure03,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_03
   },

   {
      ACCESS_PRESSURE_DECAY_FAILURE,
      NAME_LITERAL("ACCESS PRESSURE DECAY FAILURE 04")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure04,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_04
   },

   {
      ACCESS_PRESSURE_DECAY_FAILURE,
      NAME_LITERAL("ACCESS PRESSURE DECAY FAILURE 05")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure05,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_05
   },

   ////////////////////////////////////////////////////////////////////

   {
      ACCESS_NEGATIVE_ALARM_NOT_REACHED,
      NAME_LITERAL("ACCESS NEGATIVE ALARM NOT REACHED")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure00,
      textStatAlarmInletPressureFailure,
      BITMAP_ACCESS_NEGATIVE_PRESSURE_NOT_REACHED
   },

   {
      ACCESS_NEGATIVE_ALARM_NOT_REACHED,
      NAME_LITERAL("ACCESS NEGATIVE ALARM NOT REACHED 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure01,
      textStatAlarmInletPressureFailure,
      BITMAP_ACCESS_NEGATIVE_PRESSURE_NOT_REACHED_01
   },

   {
      ACCESS_NEGATIVE_ALARM_NOT_REACHED,
      NAME_LITERAL("ACCESS NEGATIVE ALARM NOT REACHED 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure02,
      textStatAlarmInletPressureFailure,
      BITMAP_ACCESS_NEGATIVE_PRESSURE_NOT_REACHED_02
   },

   {
      ACCESS_NEGATIVE_ALARM_NOT_REACHED,
      NAME_LITERAL("ACCESS NEGATIVE ALARM NOT REACHED 03")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure03,
      textStatAlarmInletPressureFailure,
      BITMAP_ACCESS_NEGATIVE_PRESSURE_NOT_REACHED_03
   },

   {
      ACCESS_NEGATIVE_ALARM_NOT_REACHED,
      NAME_LITERAL("ACCESS NEGATIVE ALARM NOT REACHED 04")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure04,
      textStatAlarmInletPressureFailure,
      BITMAP_ACCESS_NEGATIVE_PRESSURE_NOT_REACHED_04
   },

   {
      AC_DETECTED_PRIOR_TO_AC_PRIME_FAILURE,
      NAME_LITERAL("AC DETECTED PRIOR TO AC PRIME FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicAcPriorToAcPrime,
      textStatAlarmAcPriorToPrime
   },

   {
      AC_INFUSION_HIGH_ALARM,
      NAME_LITERAL("AC INFUSION HIGH ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      AC_LEVEL_ALARM,
      NAME_LITERAL("AC LEVEL ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicAcLevel00,
      textStatAlarmAcLevel,
      BITMAP_AC_LEVEL_ALERT
   },

   {
      AC_LEVEL_ALARM,
      NAME_LITERAL("AC LEVEL ALARM 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicAcLevel03,
      textStatAlarmAcLevel,
      BITMAP_AC_LEVEL_ALERT_03
   },

   {
      AC_LEVEL_ALARM,
      NAME_LITERAL("AC LEVEL ALARM 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicAcLevel01,
      textStatAlarmAcLevel,
      BITMAP_AC_LEVEL_ALERT_01
   },

   {
      AC_LEVEL_ALARM,
      NAME_LITERAL("AC LEVEL ALARM 03")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicAcLevel02,
      textStatAlarmAcLevel,
      BITMAP_AC_LEVEL_ALERT_02
   },

   {
      AC_OCCLUSION_DETECTED,
      NAME_LITERAL("AC OCCULSION DETECTED")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicAcOcclusion,
      textStatAlarmOcclusionDetected,
      BITMAP_AC_OCCLUSION_DETECTED
   },

   {
      AC_ON_DURING_RETURN,
      NAME_LITERAL("AC ON DURING RETURN")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmAcDuringReturn
   },

   {
      PRIME_FAILURE,
      NAME_LITERAL("PRIME FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicAcPrimeFailure,
      textStatAlarmAcPrimeInletFailure
   },

   {
      AC_PUMP_HALL_TOO_BIG,
      NAME_LITERAL("AC PUMP HALL TOO BIG")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicAcHallHigh,
      textStatAlarmAcPumpFailure
   },

   {
      AC_PUMP_HALL_TOO_SMALL,
      NAME_LITERAL("AC PUMP HALL TOO SMALL")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicAcHallLow,
      textStatAlarmAcPumpFailure
   },

   {
      AC_RATIO_TOO_HIGH_ALARM,
      NAME_LITERAL("AC RATIO TOO HIGH ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      AC_RATIO_TOO_LOW_ALARM,
      NAME_LITERAL("AC RATIO TOO LOW ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      APS_AUTO_ZERO_FAILED_RANGE,
      NAME_LITERAL("APS AUTO ZERO FAILED RANGE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicApsSensorFailure,
      textStatAlarmApsSensorFailure
   },

   {
      APS_AUTO_ZERO_FAILED_MIN,
      NAME_LITERAL("APS AUTO ZERO FAILED MIN")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicApsSensorFailure,
      textStatAlarmApsSensorFailure
   },

   {
      APS_AUTO_ZERO_FAILED_MAX,
      NAME_LITERAL("APS AUTO ZERO FAILED MAX")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicApsSensorFailure,
      textStatAlarmApsSensorFailure
   },

   {
      APS_TOO_HIGH_ALERT,
      NAME_LITERAL("APS TOO HIGH")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsHigh00,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH
   },

   {
      APS_TOO_HIGH_ALERT,
      NAME_LITERAL("APS TOO HIGH 01")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsHigh01,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH_01
   },

   {
      APS_TOO_HIGH_ALERT,
      NAME_LITERAL("APS TOO HIGH 02")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsHigh02,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH_02,
   },

   {
      APS_TOO_HIGH_ALERT,
      NAME_LITERAL("APS TOO HIGH 03")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsHigh03,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH_03
   },

   {
      APS_TOO_HIGH_ALERT,
      NAME_LITERAL("APS TOO HIGH 04")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyNoGraphicApsHigh04,
      textStatAlarmApsHigh
   },

   {
      APS_TOO_LOW,
      NAME_LITERAL("APS TOO LOW")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow00,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW
   },

   {
      APS_TOO_LOW,
      NAME_LITERAL("APS TOO LOW 01")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow01,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_01
   },

   {
      APS_TOO_LOW,
      NAME_LITERAL("APS TOO LOW 02")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow02,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_02
   },

   {
      APS_TOO_LOW,
      NAME_LITERAL("APS TOO LOW 03")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow03,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_03
   },

   {
      APS_TOO_LOW,
      NAME_LITERAL("APS TOO LOW 04")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow04,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_04
   },

   {
      APS_TOO_LOW,
      NAME_LITERAL("APS TOO LOW 05")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyNoGraphicApsLow05,
      textStatAlarmApsLow
   },

   {
      APS_DECOUPLING,
      NAME_LITERAL("APS DECOUPLING")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicApsDecoupling,
      textStatAlarmApsDecoupling
   },

   {
      APS_DURING_PAUSE_LOW,
      NAME_LITERAL("APS DURING PAUSE")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow00,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW
   },

   {
      APS_DURING_PAUSE_LOW,
      NAME_LITERAL("APS DURING PAUSE 01")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow01,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_01
   },

   {
      APS_DURING_PAUSE_LOW,
      NAME_LITERAL("APS DURING PAUSE 02")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow02,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_02
   },

   {
      APS_DURING_PAUSE_LOW,
      NAME_LITERAL("APS DURING PAUSE 03")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow03,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_03
   },

   {
      APS_DURING_PAUSE_LOW,
      NAME_LITERAL("APS DURING PAUSE 04")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow04,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_04
   },


   {
      APS_DURING_PAUSE_LOW,
      NAME_LITERAL("APS DURING PAUSE 05")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyNoGraphicApsLow05,
      textStatAlarmApsLow
   },



   {
      APS_DURING_PAUSE_HI,
      NAME_LITERAL("APS DURING PAUSE")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsHigh00,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH
   },

   {
      APS_DURING_PAUSE_HI,
      NAME_LITERAL("APS DURING PAUSE 01")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsHigh01,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH_01
   },

   {
      APS_DURING_PAUSE_HI,
      NAME_LITERAL("APS DURING PAUSE 02")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsHigh02,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH_02
   },

   {
      APS_DURING_PAUSE_HI,
      NAME_LITERAL("APS DURING PAUSE 03")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsHigh03,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH_03
   },

   {
      APS_DURING_PAUSE_HI,
      NAME_LITERAL("APS DURING PAUSE 04")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyNoGraphicApsHigh04,
      textStatAlarmApsHigh
   },

   {
      CASSETTE_FAILURE,
      NAME_LITERAL("CASSETTE FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCassetteFailure00,
      textStatAlarmCassetteError,
      BITMAP_CASSETTE_FAILURE
   },

   {
      CASSETTE_FAILURE,
      NAME_LITERAL("CASSETTE FAILURE_01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCassetteFailure01,
      textStatAlarmCassetteError,
      BITMAP_CASSETTE_FAILURE_01
   },

   {
      CASSETTE_FAILURE,
      NAME_LITERAL("CASSETTE FAILURE_02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCassetteFailure02,
      textStatAlarmCassetteError,
      BITMAP_CASSETTE_FAILURE_02
   },

   {
      CASSETTE_NOT_DOWN,
      NAME_LITERAL("CASSETTE NOT DOWN")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmCassetteNotDown,
   },

   {
      CASSETTE_INVALID_ALARM,
      NAME_LITERAL("CASSETTE INVALID ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicRefMismatchDispTest,
      textStatAlarmRefStampMismatch
   },

   {
      CASSETTE_UNRECOGNIZED_ALARM,
      NAME_LITERAL("CASSETTE UNRECOGNIZED ALARM")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicCassetteUnrecognized,
      textStatAlarmCassetteNotRecognized
   },

   {
      CASSETTE_REF_MISMATCH_ALARM,
      NAME_LITERAL("CASSETTE REF MISMATCH ALARM")
      ALARM_OPT_UNLOAD_BTN_ONLY,
      textAlarmBodyNoGraphicRefMismatch,
      textStatAlarmRefStampMismatch
   },

   {
      CENTRIFUGE_ERROR,
      NAME_LITERAL("CENTRIFUGE ERROR")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmCentFailure
   },

   {
      CENTRIFUGE_HARDWARE_FAILURE,
      NAME_LITERAL("CENTRIFUGE HARDWARE FAILURE")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicCentrifugeAlert,
      textStatAlarmCentFailure
   },

   {
      CENTRIFUGE_INTEGRAL_FAILURE,
      NAME_LITERAL("CENTRIFUGE INTEGRAL FAILURE")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicMotorHwFailure,
      textStatAlarmMotorHardwareFailure
   },

   {
      CENTRIFUGE_OVERSPEED,
      NAME_LITERAL("CENTRIFUGE OVERSPEED")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmCentrifugeOverspeedFailure
   },

   {
      CPS_HIGH_DURING_RBC_COLLECT_ALARM,
      NAME_LITERAL("CPS TOO HIGH IN RBC")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicCpsHighRbcCollect,
      textStatAlarmSysPressureHigh
   },

   {
      CPS_PTF_BASELINE_TEST_ALERT,
      NAME_LITERAL("CPS PTF BASELINE TEST ALERT")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPtfPressureHigh,
      textStatAlarmSysPressureHigh,
      BITMAP_PTF_BRACKET
   },

   {
      CPS_PTF_BASELINE_TEST_ALARM,
      NAME_LITERAL("CPS PTF BASELINE TEST ALARM")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicPtfPressureHigh,
      textStatAlarmSysPressureHigh
   },

   {
      CPS_PTF_PRIME_TEST,
      NAME_LITERAL("CPS PTF PRIME TEST FAILED")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyWithGraphicCpsPtfPrimeTest,
      textStatAlarmPtfPrimeError,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH
   },

   {
      CPS_HIGH_DURING_RBC_COLLECT_ALERT,
      NAME_LITERAL("CPS TOO HIGH DURING RBC COLLECT")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHighDuringRbcAlert,
      textStatAlarmSysPressureHigh,
      BITMAP_PTF_BRACKET
   },

   {
      CPS_HIGH_DURING_RBC_COLLECT_ALERT,
      NAME_LITERAL("CPS TOO HIGH DURING RBC COLLECT 03")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHighOneClamp04,
      textStatAlarmSysPressureHigh,
      BITMAP_OPEN_CROSSOVER_ONE_CLAMP_ALRM
   },

   {
      CPS_HIGH_DURING_RBC_COLLECT_ALERT,
      NAME_LITERAL("CPS TOO HIGH DURING RBC COLLECT 04")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHigh03,
      textStatAlarmSysPressureHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH_03
   },

   {
      CPS_HIGH_DURING_RBC_COLLECT_PROD2_OK_ALERT,
      NAME_LITERAL("CPS TOO HIGH DURING RBC COLLECT FIRST PRODUCT")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicCpsHighRbcCollectProd1,
      textStatAlarmSysPressureHigh
   },

   {
      CPS_OUT_OF_RANGE_AT_SET_LOWER,
      NAME_LITERAL("CPS OUT OF RANGE AT SET LOWER")
      ALARM_OPT_UNLOAD_BTN_ONLY | ALARM_OPT_SHOW_ALARM_NUM | ALARM_OPT_4_BTN_SUITE_PRE_DISP_TEST,
      textAlarmBodyNoGraphicCpsRangeAtSetLoad,
      textStatAlarmCpsRangeErrorSetLoad
   },

   {
      CPS_TOO_HIGH_ALERT_NO_CLAMP,
      NAME_LITERAL("CPS TOO HIGH NO CLAMP")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHighNoClamp,
      textStatAlarmCpsHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH
   },

   {
      CPS_TOO_HIGH_ALERT_NO_CLAMP,
      NAME_LITERAL("CPS TOO HIGH NO CLAMP 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHigh01,
      textStatAlarmCpsHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH_01
   },

   {
      CPS_TOO_HIGH_ALERT_NO_CLAMP,
      NAME_LITERAL("CPS TOO HIGH NO CLAMP 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHigh02,
      textStatAlarmCpsHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH_02
   },

   {
      CPS_TOO_HIGH_ALERT_NO_CLAMP,
      NAME_LITERAL("CPS TOO HIGH NO CLAMP 03")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicCpsHigh03,
      textStatAlarmCpsHigh
   },

   {
      CPS_TOO_HIGH_ALERT_ONE_CLAMP,
      NAME_LITERAL("CPS TOO HIGH ONE CLAMP")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHighOneClamp00,
      textStatAlarmSysPressureHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH
   },

   {
      CPS_TOO_HIGH_ALERT_ONE_CLAMP,
      NAME_LITERAL("CPS TOO HIGH ONE CLAMP 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHigh01,
      textStatAlarmSysPressureHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH_01
   },

   {
      CPS_TOO_HIGH_ALERT_ONE_CLAMP,
      NAME_LITERAL("CPS TOO HIGH ONE CLAMP 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHigh02,
      textStatAlarmSysPressureHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH_02
   },

   {
      CPS_TOO_HIGH_ALERT_ONE_CLAMP,
      NAME_LITERAL("CPS TOO HIGH ONE CLAMP 03")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHighOneClamp04,
      textStatAlarmSysPressureHigh,
      BITMAP_OPEN_CROSSOVER_ONE_CLAMP_ALRM
   },

   {
      CPS_TOO_HIGH_ALERT_ONE_CLAMP,
      NAME_LITERAL("CPS TOO HIGH ONE CLAMP 04")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicCpsHigh03,
      textStatAlarmSysPressureHigh,
   },

   {
      CPS_TOO_HIGH_ALERT_THREE_CLAMP,
      NAME_LITERAL("CPS TOO HIGH THREE CLAMP")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHighThreeClamp00,
      textStatAlarmCpsHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH
   },

   {
      CPS_TOO_HIGH_ALERT_THREE_CLAMP,
      NAME_LITERAL("CPS TOO HIGH THREE CLAMP 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHigh01,
      textStatAlarmCpsHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH_01
   },

   {
      CPS_TOO_HIGH_ALERT_THREE_CLAMP,
      NAME_LITERAL("CPS TOO HIGH THREE CLAMP 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHigh02,
      textStatAlarmCpsHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH_02
   },

   {
      CPS_TOO_HIGH_ALERT_THREE_CLAMP,
      NAME_LITERAL("CPS TOO HIGH THREE CLAMP 03")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHighThreeClamp04,
      textStatAlarmCpsHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH_04
   },

   {
      CPS_TOO_HIGH_ALERT_THREE_CLAMP,
      NAME_LITERAL("CPS TOO HIGH THREE CLAMP 04")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicCpsHigh03,
      textStatAlarmCpsHigh
   },

   {
      CPS_TOO_HIGH_ALERT_THREE_CLAMP_PTF,
      NAME_LITERAL("CPS TOO HIGH THREE CLAMP PTF")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHighThreeClamp00,
      textStatAlarmSysPressureHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH
   },

   {
      CPS_TOO_HIGH_ALERT_THREE_CLAMP_PTF,
      NAME_LITERAL("CPS TOO HIGH THREE CLAMP PTF 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHigh01,
      textStatAlarmSysPressureHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH_01
   },

   {
      CPS_TOO_HIGH_ALERT_THREE_CLAMP_PTF,
      NAME_LITERAL("CPS TOO HIGH THREE CLAMP PTF 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHigh02,
      textStatAlarmSysPressureHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH_02
   },

   {
      CPS_TOO_HIGH_ALERT_THREE_CLAMP_PTF,
      NAME_LITERAL("CPS TOO HIGH THREE CLAMP PTF 03")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCpsHighThreeClamp04,
      textStatAlarmSysPressureHigh,
      BITMAP_CENTRIFUGE_PRESSURE_TOO_HIGH_04
   },

   {
      CPS_TOO_HIGH_ALERT_THREE_CLAMP_PTF,
      NAME_LITERAL("CPS TOO HIGH THREE CLAMP PTF 04")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicCpsHigh03,
      textStatAlarmSysPressureHigh
   },

   {
      CENTRIFUGE_SHUTDOWN_FAILURE,
      NAME_LITERAL("CENTRIFUGE SHUTDOWN FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmCentFailure
   },

   {
      COLLECT_VALVE_FAILURE,
      NAME_LITERAL("COLLECT VALVE FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCollectValveFailure,
      textStatAlarmValveError,
      BITMAP_COLLECT_VALVE_FAILURE
   },

   {
      CONCENTRATION_OUT_OF_TOLERANCE,
      NAME_LITERAL("CONCENTRATION OUT OF TOLERANCE")
      ALARM_OPT_CAUSES_WBC_FLAG,
      textAlarmBodyNoGraphicLowPltConc,
      textStatAlarmLowPltConc
   },

   {
      DISCONNECT_TEST_ALERT,
      NAME_LITERAL("DISCONNECT TEST ALERT")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTest00,
      textStatAlarmDisconnectError,
      BITMAP_DISCONNECT_TEST_ALERT
   },

   {
      DISCONNECT_TEST_ALERT,
      NAME_LITERAL("DISCONNECT TEST ALERT 01")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTest01,
      textStatAlarmDisconnectError,
      BITMAP_DISCONNECT_TEST_ALERT_01
   },

   {
      DISCONNECT_TEST_ALERT,
      NAME_LITERAL("DISCONNECT TEST ALERT 02")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTest02,
      textStatAlarmDisconnectError,
      BITMAP_DISCONNECT_TEST_ALERT_03    // 12116: bitmap 03 goes with text 02
   },

   {
      DISCONNECT_TEST_ALERT,
      NAME_LITERAL("DISCONNECT TEST ALERT 03")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTest03,
      textStatAlarmDisconnectError,
      BITMAP_AC_NOT_REMOVED_ALERT
   },

   {
      DISCONNECT_TEST_MSS_ALERT,
      NAME_LITERAL("DISCONNECT TEST MSS ALERT")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTestAas,
      textStatAlarmDisconnectError,
      BITMAP_DISCONNECT_TEST_ALERT

   },

   {
      DISCONNECT_TEST_MSS_ALERT,
      NAME_LITERAL("DISCONNECT TEST MSS ALERT 01")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTest01,
      textStatAlarmDisconnectError,
      BITMAP_DISCONNECT_TEST_ALERT_01
   },

   {
      DISCONNECT_TEST_MSS_ALERT,
      NAME_LITERAL("DISCONNECT TEST MSS ALERT 02")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTest02,
      textStatAlarmDisconnectError,
      BITMAP_DISCONNECT_TEST_ALERT_03    // 12116: bitmap 03 goes with text 02
   },

   {
      DISCONNECT_TEST_ALARM,
      NAME_LITERAL("DISCONNECT TEST ALARM")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTestFail00,
      textStatAlarmDisconnectFailure,
      BITMAP_DISCONNECT_TEST_FAILURE
   },

   {
      DISCONNECT_TEST_ALARM,
      NAME_LITERAL("DISCONNECT TEST ALARM 01")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTestFail01,
      textStatAlarmDisconnectFailure,
      BITMAP_DISCONNECT_TEST_FAILURE_01
   },

   {
      DISCONNECT_TEST_ALARM,
      NAME_LITERAL("DISCONNECT TEST ALARM 02")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTestFail02,
      textStatAlarmDisconnectFailure,
      BITMAP_DISCONNECT_TEST_FAILURE_03    // 12116: bitmap 03 goes with text 02
   },

   {
      DISCONNECT_TEST_ALARM,
      NAME_LITERAL("DISCONNECT TEST ALARM 03")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTestFail03,
      textStatAlarmDisconnectFailure,
      BITMAP_AC_NOT_REMOVED_ALERT
   },

   {
      DISCONNECT_TEST_MSS_ALARM,
      NAME_LITERAL("DISCONNECT TEST MSS ALARM")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTestFailAas,
      textStatAlarmDisconnectFailure,
      BITMAP_DISCONNECT_TEST_FAILURE
   },

   {
      DISCONNECT_TEST_MSS_ALARM,
      NAME_LITERAL("DISCONNECT TEST MSS ALARM 01")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTestFail01,
      textStatAlarmDisconnectFailure,
      BITMAP_DISCONNECT_TEST_FAILURE_01
   },

   {
      DISCONNECT_TEST_MSS_ALARM,
      NAME_LITERAL("DISCONNECT TEST MSS ALARM 02")
      ALARM_OPT_DISALLOW_END_RUN,
      textAlarmBodyWithGraphicDisconnectTestFail02,
      textStatAlarmDisconnectFailure,
      BITMAP_DISCONNECT_TEST_FAILURE_03    // 12116: bitmap 03 goes with text 02
   },

   {
      DONOR_AIR_ALARM,
      NAME_LITERAL("DONOR AIR ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmDonorAir
   },

   {
      DONOR_HCT_RANGE_ERROR,
      NAME_LITERAL("DONOR HCT RANGE ERROR")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      DONOR_HEIGHT_TOO_HIGH,
      NAME_LITERAL("DONOR HEIGHT TOO HIGH")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      DONOR_HEIGHT_TOO_LOW,
      NAME_LITERAL("DONOR HEIGHT TOO LOW")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      DONOR_HYPOVOLEMIA_ALARM,
      NAME_LITERAL("DONOR HYPOVOLEMIA ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      DONOR_PRECOUNT_RANGE_ERROR,
      NAME_LITERAL("DONOR PRECOUNT RANGE ERROR")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      DONOR_SEX_INVALID,
      NAME_LITERAL("DONOR SEX INVALID")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      DONOR_TBV_TOO_HIGH,
      NAME_LITERAL("DONOR TBV TOO HIGH")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      DONOR_TBV_TOO_LOW,
      NAME_LITERAL("DONOR TBV TOO LOW")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      DONOR_WEIGHT_TOO_HIGH,
      NAME_LITERAL("DONOR WEIGHT TOO HIGH")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      DONOR_WEIGHT_TOO_LOW,
      NAME_LITERAL("DONOR WEIGHT TOO LOW")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      DOOR_LATCH_FAILURE,
      NAME_LITERAL("DOOR LATCH FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCentDoor,
      textStatAlarmCentDoorUnlatched,
      BITMAP_DOOR_LATCH_FAILURE
   },

   {
      DOOR_LOCK_ALARM,
      NAME_LITERAL("DOOR LOCK ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCentDoor,
      textStatAlarmCentDoorUnlatched,
      BITMAP_DOOR_LOCK_ALERT
   },

   {
      DOOR_NOT_LOCKED_CENTIFUGE_SPINNING,
      NAME_LITERAL("DOOR NOT LOCKED CENTIFUGE SPINNING")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmCentDoorUnlatched
   },

   {
      DOOR_OPEN_ALARM,
      NAME_LITERAL("DOOR OPEN ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCentDoor,
      textStatAlarmDoorOpen,
      BITMAP_DOOR_OPEN_ALERT
   },

   {
      DRAW_CYCLE_TOO_BIG,
      NAME_LITERAL("DRAW CYCLE TOO BIG")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      FAILURE_TO_CLOSE_CROSSOVER,
      NAME_LITERAL("FAILURE TO CLOSE CROSSOVER CLAMP")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicCrossoverOpen,
      textStatAlarmCrossoverClampClosureError,
      BITMAP_CLOSE_CROSSOVER_ALARM
   },

   {
      FAILURE_TO_PRESSURIZE_INLET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure00,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET
   },

   {
      FAILURE_TO_PRESSURIZE_INLET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure01Ais,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_01
   },

   {
      FAILURE_TO_PRESSURIZE_INLET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure02,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_02
   },

   {
      FAILURE_TO_PRESSURIZE_INLET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET 03")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure03,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_03
   },

   {
      FAILURE_TO_PRESSURIZE_INLET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET 04")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure04,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_04
   },


   ///
   {
      FAILURE_TO_PRESSURIZE_INLET_FINAL,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicBagEvacFailure,
      textStatAlarmInletPressureFailure
   },

/////////////////////////////////////////////////////////////
// Same as FAILURE_TO_PRESSURIZE_INLET, but we also may have loaded
// a RAS set for a non-RAS REF.
   {
      FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (NON-RAS SET)")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailureCassette,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET
   },

   {
      FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (NON-RAS SET) 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure01Ais,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_01
   },

   {
      FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (NON-RAS SET) 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure02,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_02
   },

   {
      FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (NON-RAS SET) 03")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure03,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_03
   },

   {
      FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (NON-RAS SET) 04")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure04,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_04
   },

   {
      FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (NON-RAS SET) 05")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure05,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_05
   },
///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Same as FAILURE_TO_PRESSURIZE_INLET, but we also may have loaded
// a non-RAS set for a RAS REF, or clamped the crossover too early.
   {
      FAILURE_TO_PRESSURIZE_INLET_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (RAS SET)")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailureCrossover,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET
   },

   {
      FAILURE_TO_PRESSURIZE_INLET_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (RAS SET) 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure01Ais,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_01
   },

   {
      FAILURE_TO_PRESSURIZE_INLET_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (RAS SET) 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure02,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_02
   },

   {
      FAILURE_TO_PRESSURIZE_INLET_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (RAS SET) 03")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure03,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_03
   },

   {
      FAILURE_TO_PRESSURIZE_INLET_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (RAS SET) 04")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure04,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_04
   },

   {
      FAILURE_TO_PRESSURIZE_INLET_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (RAS SET) 05")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure05,
      textStatAlarmInletPressureFailure,
      BITMAP_FAILURE_TO_PRESSURIZE_INLET_05
   },

   {
      FAILURE_TO_PRESSURIZE_INLET_RAS_SET,
      NAME_LITERAL("FAILURE TO PRESSURIZE INLET (RAS SET) 06")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPressurizeInletFailure06,
      textStatAlarmInletPressureFailure,
      BITMAP_APS_TOO_HIGH_ALERT_MSS
   },

   {
      FAILURE_TO_RELIEVE_PRESSURE,
      NAME_LITERAL("FAILURE TO RELIEVE PRESSURE")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicFailureToRelievePressure,
      textStatAlarmInletReliefFailure,
      BITMAP_FAILURE_TO_RELIEVE_PRESSURE
   },

   {
      FIRST_CYCLE_TOO_LATE_ALARM,
      NAME_LITERAL("FIRST CYCLE TOO LATE ALARM")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicFirstCycleLate,
      textStatAlarm1stCycleLate
   },

   {
      FIRST_CYCLE_TOO_SOON_ALARM,
      NAME_LITERAL("FIRST CYCLE TOO SOON ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicLowLevelSensor,
      textStatAlarm1stCycleEarly
   },

   {
      FIRST_DRAW_CYCLE_TOO_SMALL,
      NAME_LITERAL("FIRST DRAW CYCLE TOO SMALL")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarm1stDrawCycleShort
   },

   {
      HIGH_CENTRIFUGE_COMMAND,
      NAME_LITERAL("HIGH CENTRIFUGE COMMAND")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicCentrifugeAlert,
      textStatAlarmCentrifugeOverspeed
   },

   {
      HYPOVOLEMIA_ALARM,
      NAME_LITERAL("HYPOVOLEMIA ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicHypovolemia,
      textStatAlarmSysTestFailure
   },

   {
      INAPPROPRIATE_AC_PUMP_MOVEMENT,
      NAME_LITERAL("INAPPROPRIATE AC PUMP MOVEMENT")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmAcPumpMovement
   },

   {
      INLET_PUMP_HALL_TOO_BIG,
      NAME_LITERAL("INLET PUMP HALL TOO BIG")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicInletHallHigh,
      textStatAlarmInletPumpFailure
   },

   {
      INLET_PUMP_HALL_TOO_SMALL,
      NAME_LITERAL("INLET PUMP HALL TOO SMALL")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicInletHallLow,
      textStatAlarmInletPumpFailure
   },

   {
      INAPPROPRIATE_CASSETTE_MOVEMENT,
      NAME_LITERAL("INAPPROPRIATE CASSETTE MOVEMENT")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicCassetteMovement,
      textStatAlarmCassetteMovement
   },

   {
      LEAK_DETECTOR_ERROR_FAILURE,
      NAME_LITERAL("LEAK DETECTOR ERROR FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicLeakDetected,   // IT4692
      textStatAlarmLeakDetectorFailure
   },

   {
      LEAK_DETECTED_ALERT,
      NAME_LITERAL("LEAK DETECTED ALERT")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicLeakDetected,
      textStatAlarmLeakDetected,
      BITMAP_LEAK_DETECTED
   },

   {
      LEAK_TEST_FAILURE,
      NAME_LITERAL("LEAK TEST FAILURE")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicLeakTestFailure,
      textStatAlarmLeakTestFailure
   },

   {
      LOW_LEVEL_DETECTED_PRIOR_TO_EXPECTED,
      NAME_LITERAL("LOW LEVEL DETECTED PRIOR TO EXPECTED")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicLowLevelPriorToExpected,
      textStatAlarmLowLevelPriorToExpected
   },

   {
      LOW_LEVEL_SENSOR_SEEN_TOO_LATE_FAILURE,
      NAME_LITERAL("LOW LEVEL SENSOR SEEN TOO LATE FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicLowLevelLate,
      textStatAlarmLowLevelLate
   },

   {
      LOW_LEVEL_SENSOR_SEEN_TOO_SOON_FAILURE,
      NAME_LITERAL("LOW LEVEL SENSOR SEEN TOO SOON FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicLowLevelSensor,
      textStatAlarmLowLevelEarly
   },

   {
      MAINTAIN_DONOR_ACCESS_ALERT,
      NAME_LITERAL("MAINTAIN DONOR ACCESS ALERT")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicMaintainDonorAccess,
      textStatAlarmMaintainDonorAccessError
   },

   {
      minus12V_TOO_HIGH,
      NAME_LITERAL("minus12V TOO HIGH")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmVoltageFailure
   },

   {
      minus12V_TOO_LOW,
      NAME_LITERAL("minus12V TOO LOW")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmVoltageFailure,
   },

   {
      MOTOR_FAILED_TO_MAINTAIN_SPEED,
      NAME_LITERAL("MOTOR HARDWARE FAILED TO MAINTAIN SPEED")
      ALARM_OPT_SHOW_ALARM_NUM | ALARM_OPT_4_BTN_SUITE_PRE_DISP_TEST,
      textAlarmBodyNoGraphicMotorFailedToMaintainSpeed,
      textStatAlarmMotorFailedToMaintainSpeed
   },

   {
      MOTOR_INTEGRATION_ERROR,
      NAME_LITERAL("MOTOR HARDWARE INTEGRATION ERROR")
      ALARM_OPT_SHOW_ALARM_NUM | ALARM_OPT_4_BTN_SUITE_PRE_DISP_TEST,
      textAlarmBodyNoGraphicMotorIntegrationError,
      textStatAlarmMotorIntegrationError
   },

   {
      MOTOR_VOLUME_ERROR,
      NAME_LITERAL("MOTOR HARDWARE VOLUME ERROR")
      ALARM_OPT_SHOW_ALARM_NUM | ALARM_OPT_4_BTN_SUITE_PRE_DISP_TEST,
      textAlarmBodyNoGraphicMotorVolumeError,
      textStatAlarmMotorVolumeError
   },

   {
      MOTOR_DISABLED,
      NAME_LITERAL("MOTOR HARDWARE DISABLED")
      ALARM_OPT_SHOW_ALARM_NUM | ALARM_OPT_4_BTN_SUITE_PRE_DISP_TEST,
      textAlarmBodyNoGraphicMotorDisabled,
      textStatAlarmMotorDisabled
   },

   {
      ONE_MINUTE_PAUSE,
      NAME_LITERAL("ONE MINUTE PAUSE")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphic1MinPause,
      textStatAlarm1MinPause,
      BITMAP_ONE_MINUTE_PAUSE_ALERT
   },

   {
      PAUSE_BUTTON_ALARM,
      NAME_LITERAL("PAUSE BUTTON ALARM")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicPauseButton,
      textStatAlarmPauseButton,
      BITMAP_PAUSE_BUTTON_ALERT
   },

   {
      PAUSE_SW_FAILED,
      NAME_LITERAL("PAUSE SW FAILED")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      PLASMA_OUT_OF_POSITION,
      NAME_LITERAL("PLASMA OUT OF POSITION")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmValveError
   },

   {
      PLASMA_PUMP_HALL_TOO_BIG,
      NAME_LITERAL("PLASMA PUMP HALL TOO BIG")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicPlasmaHallHigh,
      textStatAlarmPlasmaPumpFailure
   },

   {
      PLASMA_PUMP_HALL_TOO_SMALL,
      NAME_LITERAL("PLASMA PUMP HALL TOO SMALL")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicPlasmaHallLow,
      textStatAlarmPlasmaPumpFailure
   },

   {
      PLASMA_VALVE_FAILURE,
      NAME_LITERAL("PLASMA VALVE FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPlasmaValveFailure,
      textStatAlarmValveError,
      BITMAP_PLASMA_VALVE_FAILURE
   },

   {
      LOW_PLATELET_CONCENTRATION_ALERT,
      NAME_LITERAL("LOW PLATELET CONCENTRATION ALERT")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_UNOBTRUSIVE_ALERT | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyNoGraphicPltConcLow,
      textStatAlarmPltConcLow
   },

   {
      PLATELET_OUT_OF_POSITION,
      NAME_LITERAL("PLATELET OUT OF POSITION")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmValveError
   },

   {
      PLATELET_PUMP_HALL_TOO_BIG,
      NAME_LITERAL("PLATELET PUMP HALL TOO BIG")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicPltHallHigh,
      textStatAlarmPltPumpFailure
   },

   {
      PLATELET_PUMP_HALL_TOO_SMALL,
      NAME_LITERAL("PLATELET PUMP HALL TOO SMALL")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicPltHallLow,
      textStatAlarmPltPumpFailure
   },

   {
      POWER_FAIL_CONTINUE,
      NAME_LITERAL("POWER FAIL CONTINUE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicPowerFail,
      textStatAlarmPowerFailureAlert
   },

   {
      POWER_FAIL_RINSEBACK,
      NAME_LITERAL("POWER FAIL RINSEBACK")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicPowerFail,
      textStatAlarmPowerFailureAlert
   },

   {
      POWER_FAIL_ABORT,
      NAME_LITERAL("POWER FAIL ABORT")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicPowerFailAbort,
      textStatAlarmPowerFailureAlert
   },

   {
      POWER_TEST_FAILURE,
      NAME_LITERAL("POWER TEST FAILURE")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicPowerTestFailure,
      textStatAlarmPowerTestFailure
   },

   {
      PRIME_TOO_SOON,
      NAME_LITERAL("PRIME TOO SOON")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmPrimeTooSoon
   },

   {
      PROCEDURE_MODIFIED,
      NAME_LITERAL("PROCEDURE MODIFIED")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicProcedureModified,
      textStatAlarmProcedureModified
   },

   {
      PUMP_RPM_COMMAND_FAILURE,
      NAME_LITERAL("PUMP RPM COMMAND FAILURE")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicPumpRpmCommand,
      textStatAlarmPumpRpmCommand
   },

   {
      PWR_SWITCH_24V_FAILED,
      NAME_LITERAL("PWR SWITCH 24V FAILED")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      PWR_SWITCH_64V_FAILED,
      NAME_LITERAL("PWR SWITCH 64V FAILED")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      PWR_SWITCH_BOTH_FAILED,
      NAME_LITERAL("PWR SWITCH BOTH FAILED")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      RBC_DETECTOR_CAL_FAILURE,
      NAME_LITERAL("RBC DETECTOR CAL FAILURE")
      ALARM_OPT_4_BTN_SUITE | ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicRbcDetectorCal,
      textStatAlarmCalDetectorFailure
   },

   {
      RBC_OUT_OF_POSITION,
      NAME_LITERAL("RBC OUT OF POSITION")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmValveError
   },

   {
      RBC_VALVE_FAILURE,
      NAME_LITERAL("RBC VALVE FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicRbcValve,
      textStatAlarmValveError,
      BITMAP_RBC_VALVE_FAILURE
   },

   {
      RBC_WB_PROCESSED_TOO_LARGE,
      NAME_LITERAL("RBC WB PROCESSED TOO LARGE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      RESERVOIR_ALARM,
      NAME_LITERAL("RESERVOIR ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicResAlert,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_ALERT
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_NO_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM NO CLAMP")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyNoGraphicResDrawNoClamp,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_NO_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM NO CLAMP 01")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicResDrawAlert01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_LONG
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM ONE CLAMP")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyNoGraphicResDrawOneClamp,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM ONE CLAMP 01")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicResDrawAlert01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_LONG
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM ONE CLAMP 02")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicResDrawAlert1Clamp04,
      textStatAlarmLevelSensorErrorA,
      BITMAP_OPEN_CROSSOVER_ONE_CLAMP_ALRM
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM THREE CLAMP")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyNoGraphicResDrawRbcPlThreeClamp,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM THREE CLAMP 01")
      ALARM_OPT_CAUSES_WBC_FLAG,
      textAlarmBodyWithGraphicResDrawAlert01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_LONG
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM THREE CLAMP 02")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicResDrawAlert3Clamp04,
      textStatAlarmLevelSensorErrorA,
      BITMAP_OPEN_CROSSOVER_THREE_CLAMP_ALRM
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM THREE CLAMP 03")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicPasLineOpen,
      textStatAlarmLevelSensorErrorA,
      BITMAP_CLOSE_PAS_LINE_ALRM
   },

//  IT4823
   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_NO_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM RBC PL NO CLAMP")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyNoGraphicResDrawRbcPlNoClamp,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_NO_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM RBC PL NO CLAMP 01")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicResDrawAlert02,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_LONG_02
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_NO_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM RBC NO CLAMP PL 02")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicResDrawAlert03,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_LONG_02
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_NO_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM RBC PL NO CLAMP 03")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicResDrawAlertRbcPl01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_LONG_RBC_PL
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM RBC PL ONE CLAMP")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyNoGraphicResDrawRbcPlOneClamp,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM RBC PL ONE CLAMP 01")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicResDrawAlert02,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_LONG_02
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM RBC PL ONE CLAMP 02")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicResDrawAlert03,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_LONG_02
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM RBC PL ONE CLAMP 03")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicResDrawAlertRbcPl01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_LONG_RBC_PL
   },

   {
      RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO LONG ALARM RBC PL ONE CLAMP 04")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicResDrawAlert1Clamp04,
      textStatAlarmLevelSensorErrorA,
      BITMAP_OPEN_CROSSOVER_ONE_CLAMP_ALRM
   },

   {
      DRBC_SPLIT_ALERT,
      NAME_LITERAL("DRBC SPLIT ALERT")
      OVERRIDE_DEFAULT(GUI_SCREEN_DRBC_SPLIT, "")
   },

   {
      RETURN_LINE_OCCLUSION,
      NAME_LITERAL("RETURN LINE OCCLUSION")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicApsHigh00,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH
   },

   {
      RETURN_LINE_OCCLUSION,
      NAME_LITERAL("RETURN LINE OCCLUSION 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicApsHigh01,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH_01
   },

   {
      RETURN_LINE_OCCLUSION,
      NAME_LITERAL("RETURN LINE OCCLUSION 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicApsHigh02,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH_02
   },

   {
      RETURN_LINE_OCCLUSION,
      NAME_LITERAL("RETURN LINE OCCLUSION 03")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicApsHigh03,
      textStatAlarmApsHigh,
      BITMAP_ACCESS_PRESSURE_TOO_HIGH_03
   },

   {
      RETURN_LINE_OCCLUSION,
      NAME_LITERAL("RETURN LINE OCCLUSION 04")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicApsHigh04,
      textStatAlarmApsHigh
   },

   // This alarm,TOO_MANY_RESERVOIR_DRAW_ALARMS, is using the same text/bitmaps as the RESERVOIR_DRAW_ALARM
   // In subsequent releases, this alarm may have unique text / bitmaps developed for it.
   // I.T. 3296

   {
      TOO_MANY_RESERVOIR_DRAW_ALARMS,
      NAME_LITERAL("TOO MANY RESERVOIR DRAW ALARMS")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_FAILED_T0,
      NAME_LITERAL("RESERVOIR FAILED T0")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      RESERVOIR_RETURN_TOO_LONG_ALARM_NO_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO LONG ALARM NO CLAMP")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyNoGraphicResReturnNoClamp,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_RETURN_TOO_LONG_ALARM_NO_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO LONG ALARM NO CLAMP 01")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicResReturn01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_RETURN_ALERT_01
   },

   {
      RESERVOIR_RETURN_TOO_LONG_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO LONG ALARM ONE CLAMP")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyNoGraphicResReturnOneClamp,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_RETURN_TOO_LONG_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO LONG ALARM ONE CLAMP 01")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicResReturn01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_RETURN_ALERT_01
   },

   {
      RESERVOIR_RETURN_TOO_LONG_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO LONG ALARM ONE CLAMP 02")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicResReturn1Clamp,
      textStatAlarmLevelSensorErrorA,
      BITMAP_OPEN_CROSSOVER_ONE_CLAMP_ALRM
   },

   {
      RESERVOIR_RETURN_TOO_LONG_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO LONG ALARM THREE CLAMP")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyNoGraphicResReturnThreeClamp,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_RETURN_TOO_LONG_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO LONG ALARM THREE CLAMP 01")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicResReturn01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_RETURN_ALERT_01
   },

   {
      RESERVOIR_RETURN_TOO_LONG_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO LONG ALARM THREE CLAMP 02")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicResReturn3Clamp,
      textStatAlarmLevelSensorErrorA,
      BITMAP_OPEN_CROSSOVER_THREE_CLAMP_ALRM
   },

   {
      RESERVOIR_RETURN_TOO_LONG_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO LONG ALARM THREE CLAMP 03")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicPasLineOpen,
      textStatAlarmLevelSensorErrorA,
      BITMAP_CLOSE_PAS_LINE_ALRM
   },

   // This alarm,TOO_MANY_RESERVOIR_RETURN_ALARMS, is using the same text/bitmaps as the RESERVOIR_RETURN_ALARM -- all three panels.
   // In V5.0, this alarm has unique text developed for it.
   // I.T. 3296

   {
      TOO_MANY_RESERVOIR_RETURN_ALARMS,
      NAME_LITERAL("TOO MANY RESERVOIR RETURN ALARMS")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_FULL_AFTER_RETURN,
      NAME_LITERAL("RESERVOIR FULL AFTER RETURN CYCLE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmLevelSensorErrorA
   },

   {
      RETURN_PUMP_HALL_TOO_BIG,
      NAME_LITERAL("RETURN PUMP HALL TOO BIG")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicReturnHallHigh,
      textStatAlarmReturnPumpFailure
   },

   {
      RETURN_PUMP_HALL_TOO_SMALL,
      NAME_LITERAL("RETURN PUMP HALL TOO SMALL")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicReturnHallLow,
      textStatAlarmReturnPumpFailure
   },

   {
      RETURN_PUMP_SPEED_ALARM_RB,
      NAME_LITERAL("RETURN PUMP SPEED ALARM RB")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmReturnPumpFailure
   },

   {
      SAFETY_STATUS_COMM_ALARM,
      NAME_LITERAL("SAFETY STATUS COMM ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      SAFETY_T1_FAILURE,
      NAME_LITERAL("SAFETY T1 FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      SPILLOVER_DETECTED,
      NAME_LITERAL("SPILLOVER DETECTED")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicSpilloverDetected,
      textStatAlarmSpilloverDetected,
      BITMAP_SPILLOVER_DETECTED
   },

   {
      STATE_TRANSITION_INVALID,
      NAME_LITERAL("STATE TRANSITION INVALID")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      STOP_BUTTON_ALARM,
      NAME_LITERAL("STOP BUTTON ALARM")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicStopButtonAlert,
      textStatAlarmStopButton,
      BITMAP_STOP_BUTTON_ALERT
   },

   {
      STOP_SW_FAILED,
      NAME_LITERAL("STOP SW FAILED")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      SW_FAULT,
      NAME_LITERAL("SW FAULT")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      TBV_COMPARISON_FAILED,
      NAME_LITERAL("TBV COMPARISON FAILED")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      TBV_UNITS_UNKNOWN,
      NAME_LITERAL("TBV UNITS UNKNOWN")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      TEN_MINUTE_PAUSE,
      NAME_LITERAL("TEN MINUTE PAUSE")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphic10MinPause,
      textStatAlarm10MinPause,
      BITMAP_TEN_MINUTE_PAUSE_ALERT
   },

   {
      TEN_MINUTE_PAUSE_ALARM,
      NAME_LITERAL("TEN MINUTE PAUSE ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphic10MinPause,
      textStatAlarm10MinPauseFailure
   },

   {
      UNLOADING_BAD_CASSETTE_POSITION,
      NAME_LITERAL("UNLOADING BAD CASSETTE POSITION")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicUnloadingBadSetPosition,
      textStatAlarmUnloadingBagCassettePosition,
      BITMAP_UNLOADING_BAD_CASSETTTE_POSITION
   },

   {
      VALVE_HARDWARE_FAILURE,
      NAME_LITERAL("VALVE HARDWARE FAILURE")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicValveHwFailure,
      textStatAlarmValveError
   },

   {
      VALVE_LED_ERROR,
      NAME_LITERAL("VALVE LED ERROR")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmValveLedError
   },

   {
      VALVE_TEST_FAILURE,
      NAME_LITERAL("VALVE TEST FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicValveTestFailure,
      textStatAlarmValveError,
      BITMAP_VALVE_TEST_FAILURE
   },

   {
      SET_IDENTIFICATION_ALARM,
      NAME_LITERAL("SET IDENTIFICATION ALARM")
      OVERRIDE_DEFAULT(GUI_SCREEN_SETID, "")
   },

   {
      REPLACEMENT_FLUID_ALARM,
      NAME_LITERAL("REPLACEMENT FLUID ALARM")
      OVERRIDE_DEFAULT(GUI_SCREEN_REPFLUID, "normal")
   },

   {
      REPLACEMENT_FLUID_ALARM_RINSEBACK,
      NAME_LITERAL("REPLACEMENT FLUID ALARM RINSEBACK")
      OVERRIDE_DEFAULT(GUI_SCREEN_REPFLUID, "rinseback")
   },

   {
      BARCODE_INFO_ALARM,       // BARCODE
      NAME_LITERAL("BARCODE INFO ALARM")
      OVERRIDE_DEFAULT(GUI_SCREEN_BARCODE, "")
   },

   {
      REPLACEMENT_FLUID_BOLUS_ALARM,
      NAME_LITERAL("REPLACEMENT FLUID BOLUS ALARM")
      OVERRIDE_DEFAULT(GUI_SCREEN_REPFLUID, "bolus")
   },

   {
      REPLACEMENT_FLUID_NEW_BAG_ALARM,
      NAME_LITERAL("REPLACEMENT FLUID NEW BAG ALARM")
      OVERRIDE_DEFAULT(GUI_SCREEN_REPFLUID, "newbag")
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_NO_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM NO CLAMP")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyNoGraphicDrawCycleShortNoClamp,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_NO_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM NO CLAMP 01")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicDrawCycleShort01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_SHORT_01
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_NO_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM NO CLAMP 02")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicDrawCycleShort02,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_SHORT_02
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM ONE CLAMP")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyNoGraphicDrawCycleShortOneClamp,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM ONE CLAMP 01")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicDrawCycleShort01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_SHORT_01
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM ONE CLAMP 02")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicDrawCycleShort02,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_SHORT_02
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM ONE CLAMP 03")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicDrawCycleShortOneClamp,
      textStatAlarmLevelSensorErrorA,
      BITMAP_OPEN_CROSSOVER_ONE_CLAMP_ALRM
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM THREE CLAMP")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyNoGraphicDrawCycleShortThreeClamp,
      textStatAlarmLevelSensorErrorA
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM THREE CLAMP 01")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicDrawCycleShort01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_SHORT_01
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM THREE CLAMP 02")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicDrawCycleShort02,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_DRAW_TOO_SHORT_02
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM THREE CLAMP 03")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicDrawCycleShortThreeClamp,
      textStatAlarmLevelSensorErrorA,
      BITMAP_OPEN_CROSSOVER_THREE_CLAMP_ALRM
   },

   {
      RESERVOIR_DRAW_TOO_SHORT_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR DRAW TOO SHORT ALARM THREE CLAMP 04")
      ALARM_OPT_DISALLOW_ADJUST,
      textAlarmBodyWithGraphicPasLineOpen,
      textStatAlarmLevelSensorErrorA,
      BITMAP_CLOSE_PAS_LINE_ALRM
   },

   {
      RESERVOIR_RETURN_TOO_SHORT_ALARM_NO_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO SHORT ALARM NO CLAMP")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicReturnShortNoClamp,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_RETURN_TOO_SHORT
   },

   {
      RESERVOIR_RETURN_TOO_SHORT_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO SHORT ALARM ONE CLAMP")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicReturnShortOneClamp00,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_RETURN_TOO_SHORT
   },

   {
      RESERVOIR_RETURN_TOO_SHORT_ALARM_ONE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO SHORT ALARM ONE CLAMP 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicReturnShortOneClamp01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_OPEN_CROSSOVER_ONE_CLAMP_ALRM
   },

   {
      RESERVOIR_RETURN_TOO_SHORT_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO SHORT ALARM THREE CLAMP")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicReturnShortThreeClamp00,
      textStatAlarmLevelSensorErrorA,
      BITMAP_RESERVOIR_RETURN_TOO_SHORT
   },

   {
      RESERVOIR_RETURN_TOO_SHORT_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO SHORT ALARM THREE CLAMP 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicReturnShortThreeClamp01,
      textStatAlarmLevelSensorErrorA,
      BITMAP_OPEN_CROSSOVER_THREE_CLAMP_ALRM
   },

   {
      RESERVOIR_RETURN_TOO_SHORT_ALARM_THREE_CLAMP,
      NAME_LITERAL("RESERVOIR RETURN TOO SHORT ALARM THREE CLAMP 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPasLineOpen,
      textStatAlarmLevelSensorErrorA,
      BITMAP_CLOSE_PAS_LINE_ALRM
   },


   {
      HYPERVOLEMIA_DETECTED,
      NAME_LITERAL("HYPERVOLEMIA DETECTED")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicDonorHypovolemia,
      textStatAlarmSysTestFailure
   },

   {
      OVERTEMP_ALARM,
      NAME_LITERAL("OVERTEMP ALARM")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicOvertemp,
      textStatAlarmOvertemp
   },

   {
      OVERTEMP_FAILURE,
      NAME_LITERAL("OVERTEMP FAILURE")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicOvertemp,
      textStatAlarmOvertemp
   },

   {
      EVACUATION_PRESSURE_ALERT,
      NAME_LITERAL("EVACUATION PRESSURE ALERT")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicBagEvac,
      textStatAlarmBagEvac,
      BITMAP_AIR_EVAC_ALERT
   },



   {
      EVACUATION_PRESSURE_ALERT_JP,
      NAME_LITERAL("EVACUATION PRESSURE ALERT")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicBagEvac,
      textStatAlarmBagEvac,
      BITMAP_AIR_EVAC_ALERT_JAP
   },


   {
      EVACUATION_PRESSURE_ALERT_FINAL,
      NAME_LITERAL("EVACUATION PRESSURE ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicBagEvacFailure,
      textStatAlarmBagEvacFailure
   },

   {
      APS_PUMPS_SLOW,
      NAME_LITERAL("APS PUMPS SLOW")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicAccessPumpsSlowed,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW
   },

   {
      APS_PUMPS_SLOW,
      NAME_LITERAL("APS PUMPS SLOW 01")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow01,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_01
   },

   {
      APS_PUMPS_SLOW,
      NAME_LITERAL("APS PUMPS SLOW 02")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicApsLow02,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_02
   },

   {
      APS_PUMPS_SLOW,
      NAME_LITERAL("APS PUMPS SLOW 03")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyWithGraphicApsLow03,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_03
   },

   {
      APS_PUMPS_SLOW,
      NAME_LITERAL("APS PUMPS SLOW 04")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicApsLow04,
      textStatAlarmApsLow,
      BITMAP_ACCESS_PRESSURE_TOO_LOW_04
   },

   {
      APS_PUMPS_SLOW,
      NAME_LITERAL("APS PUMPS SLOW 05")
      ALARM_OPT_4_BTN_SUITE,
      textAlarmBodyNoGraphicApsLow05,
      textStatAlarmApsLow
   },

   {
      CFR11_TRACKING_ALARM,
      NAME_LITERAL("CFR11 TRACKING ALARM")
      OVERRIDE_DEFAULT(GUI_SCREEN_CFR11, "")
   },

   {
      AC_PRIME_INLET_ALARM,
      NAME_LITERAL("AC PRIME INLET ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicAcPrimeInletOcclusion,
      textStatAlarmPrimeInletOcclusion
   },

   {
      RESERVOIR_RETURN_TOO_LONG_ALARM_A1,
      NAME_LITERAL("RESERVOIR RETURN TOO LONG ALARM A1")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA2,
      textStatAlarmSysTestFailure
   },

   {
      AC_CLOSURE_FAILED_ALERT,
      NAME_LITERAL("AC CLOSURE FAILED ALERT")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicAcClosFailAas,
      textStatAlarmAcClosureError,
      BITMAP_AC_CLOSURE_FAILED_ALERT
   },

   {
      AC_CLOSURE_FAILED_ALARM,
      NAME_LITERAL("AC CLOSURE FAILED ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicAcClosFailAas,
      textStatAlarmAcClosureFailure
   },


   {
      AC_CLOSURE_FAILED_CONTINUE_PAS_ALARM,
      NAME_LITERAL("AC CLOSURE FAILED CONTINUE PAS ALARM")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicAcClosFailAasPasOk,
      textStatAlarmAcClosureFailure
   },

   {
      APS_TOO_HIGH_ALARM_MSS,
      NAME_LITERAL("APS TOO HIGH ALARM MSS")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicApsHighAas,
      textStatAlarmFilterPressureHigh
   },

   {
      APS_TOO_HIGH_ALERT_MSS,
      NAME_LITERAL("APS TOO HIGH ALERT MSS")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicApsHighAas,
      textStatAlarmFilterPressureHigh,
      BITMAP_APS_TOO_HIGH_ALERT_MSS
   },


   {
      APS_TOO_HIGH_ALARM_MSS_WITH_PAS,
      NAME_LITERAL("APS TOO HIGH ALERT MSS WITH PAS")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicApsHighAas,
      textStatAlarmFilterPressureHigh,
      BITMAP_APS_TOO_HIGH_ALERT_MSS
   },


   {
      APS_HIGH_MSS_P1_OK_ALERT,
      NAME_LITERAL("APS TOO HIGH ALERT MSS P1 OK")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicApsHighAasP2Ok,
      textStatAlarmFilterPressureHigh
   },

   {
      APS_TOO_HIGH_PTF_ALERT,
      NAME_LITERAL("APS TOO HIGH ALERT PTF ALERT")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicApsHighPtf,
      textStatAlarmFilterPressureHigh,
      BITMAP_PTF_BRACKET
   },

   {
      APS_TOO_HIGH_PTF_ALERT,
      NAME_LITERAL("APS TOO HIGH ALERT PTF ALERT 01")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicApsHighAas,
      textStatAlarmFilterPressureHigh,
      BITMAP_APS_TOO_HIGH_ALERT_MSS
   },

   {
      CHANNEL_VOLUME_REMAINING_HIGH,
      NAME_LITERAL("CHANNEL VOLUME REMAINING HIGH")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicCrvHigh,
      textStatAlarmCrvHigh
   },

   {
      CPS_TOO_HIGH_ALARM,
      NAME_LITERAL("CPS TOO HIGH ALARM")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicCpsHighAas,
      textStatAlarmSysPressureHigh
   },

   {
      AC_NOT_REMOVED_ALERT,
      NAME_LITERAL("AC NOT REMOVED ALERT")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicFluidAtAcAas,
      textStatAlarmFluidAtAcAas,
      BITMAP_AC_NOT_REMOVED_ALERT
   },

   {
      MSS_PRIME_TOO_SOON_RBC_ALARM,
      NAME_LITERAL("MSS RBC PRIME TOO SOON ALARM")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicRasPrimeEarly,
      textStatAlarmRasPrimeEarly
   },

   {
      MSS_PRIME_TOO_SOON_PLT_ALERT,
      NAME_LITERAL("MSS PLT PRIME TOO SOON ALERT")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyWithGraphicPasPrimeEarly,
      textStatAlarmPasPrimeEarly,
      BITMAP_CHANNEL_CLOSURE_ERROR
   },

   {
      MSS_PRIME_DELAY_RBC_ALERT,
      NAME_LITERAL("MSS RBC PRIME DELAY ALERT")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicRasPrimeLate,
      textStatAlarmRasPrimeLate
   },

   {
      MSS_PLT_SS_EMPTY,
      NAME_LITERAL("MSS PLT SS EMPTY")
      OVERRIDE_DEFAULT(GUI_SCREEN_PAS_EMPTY, "normalVersion")
   },

   {
      MSS_RBC_SS_EMPTY,
      NAME_LITERAL("MSS RBC SS EMPTY")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicRasEmpty,
      textStatAlarmRasEmpty,
      BITMAP_CONNECT_NEW_RAS_BAG
   },

   {
      MSS_DRBC_SS_EMPTY,
      NAME_LITERAL("MSS DRBC SS EMPTY")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicRasEmpty,
      textStatAlarmRasEmpty,
      BITMAP_CONNECT_NEW_RAS_BAG
   },

   {
      INLET_VOLM_ALARM,
      NAME_LITERAL("MSS INLET VOLUME ALARM")
      ALARM_OPT_SHOW_ALARM_NUM,
      textAlarmBodyNoGraphicInletVolAas,
      textStatAlarmInletVolmAas
   },

   {
      CHANNEL_CLAMP_NOT_CLOSED_ALERT,
      NAME_LITERAL("CHANNEL CLAMP NOT CLOSED ALERT")
      ALARM_OPT_CAUSES_WBC_FLAG,
      textAlarmBodyWithGraphicAasClampClosAdditionFailure,
      textStatAlarmAasClampClosureError,
      BITMAP_CHANNEL_CLOSURE_ERROR
   },

   {
      CHANNEL_CLAMP_NOT_CLOSED_ADDITION_ALERT,
      NAME_LITERAL("CHANNEL CLAMP NOT CLOSED ADDITION ALERT")
      ALARM_OPT_CAUSES_WBC_FLAG,
      textAlarmBodyWithGraphicAasClampClosAddition,
      textStatAlarmAasClampClosureError,
      BITMAP_CHANNEL_CLOSURE_ERROR
   },

   {
      MSS_PRIME_DELAY_PLT_ALERT,
      NAME_LITERAL("MSS PLT PRIME DELAY ALERT")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPasPrimeLate,
      textStatAlarmPasPrimeLate,
      BITMAP_MSS_PRIME_DELAY_PLT_ALERT
   },

   {
      MSS_MISCONNECTED_2ND_PAS_BAG,
      NAME_LITERAL("MSS MISCONNECTED 2ND PAS BAG")
      ALARM_OPT_NONE,
      textAlarmBodyWithGraphicPasMisconnected,
      textStatAlarmPasPrimeLate,
      BITMAP_MSS_PRIME_DELAY_PLT_ALERT
   },

   {
      SAFETY_NOT_RESPONDING,
      NAME_LITERAL("SAFETY NOT RESPONDING TO STARTUP PROTOCOL")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicSafetyNotResponding,
      textStatAlarmSafetyNotResponding
   },

   {
      HEMOLYSIS_DETECTED_ALARM,
      NAME_LITERAL("HEMOLYSIS DETECTED")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicHemolysisDetected,
      textStatAlarmHemolysisDetected
   },

   {
      AC_DETECTION_ALARM,
      NAME_LITERAL("AC NOT DETECTED STILL")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicAcStillNotDetected,
      textStatAlarmAcStilNotDetected
   },

   {  // IT 7960
      GENERIC_PLT_WBC_CONTAMINATION_ALERT,
      NAME_LITERAL("GENERIC PLT WBC CONTAMINATION ALERT")
      ALARM_OPT_CAUSES_WBC_FLAG | ALARM_OPT_UNOBTRUSIVE_ALERT,
      textAlarmBodyNoGraphicPltWbcContamination,
      textStatAlarmGenericPltContam
   },


   {  // IT 11037
      GENERIC_PLT_WBC_CONTAMINATION_OBTRUSIVE_ALERT,
      NAME_LITERAL("GENERIC PLT WBC CONTAMINATION OBTRUSIVE ALERT")
      ALARM_OPT_CAUSES_WBC_FLAG,
      textAlarmBodyNoGraphicPltWbcContamination,
      textStatAlarmGenericPltContam
   },

   {
      SAL_RINSEBACK_RETURN_TOO_HIGH,
      NAME_LITERAL("SALINE RINSEBACK RETURN TOO HIGH")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicGenericA1,
      textStatAlarmSysTestFailure
   },

   {
      AUTOFLOW_TIME_ALERT,
      NAME_LITERAL("AUTOFLOW TIME ALERT")
      ALARM_OPT_NONE,
      JPHtextAlarmBodyAFtenMin,
      JPHtextStatAlarmAFtenMin,
      BITMAP_AUTO_FLOW_TEN_MIN_INCREASE
   },


   {
      PERSISTENT_SPILLOVER_ALARM,
      NAME_LITERAL("PERSISTENT SPILLOVER")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicHypovolemia,
      textStatAlarmSpilloverDetected
   },

   {
      CONTROL_SOFT_WATCHDOG,
      NAME_LITERAL("CONTROL SOFT WATCHDOG")
      ALARM_OPT_NONE,
      textAlarmBodyNoGraphicControlSoftWatchdogAlarm,
      textStatAlarmSysTestFailure
   },

#if 0
   {
      CONTROL_PRE_WATCHDOG_PAUSE,
      NAME_LITERAL("CONTROL PRE SOFT WATCHDOG PAUSE")
      ALARM_OPT_NONE,
      TEXT_NULL,
      TEXT_NULL
   },
#endif

   // DO NOT ADD ALARMS AFTER THE NULL_ALARM ENTRY
   {
      NULL_ALARM,
   }
}; // This is the end of the alarm table declaration

//
//
// Number of entries in alarm table (excluding NULL_ALARM entry at end)
//
const int tableSize = (sizeof(alarms) / sizeof(ALARM_TABLE)) - 1;


//
// GetEntry
//
const ALARM_TABLE* GuiAlarmTable::GetEntry (ALARM_VALUES id)
{
   for (int i = 0; alarms[i].alarm_name != NULL_ALARM; i++)
   {
      if (alarms[i].alarm_name == id)
      {
         index = i;
         return &alarms[i];
      }
   }

   // Fail case.  Can't find alarm.
   return 0;
}


//
// GetCurrentEntry
//
const ALARM_TABLE* GuiAlarmTable::GetCurrentEntry ()
{
   if ((index < 0) || (index >= tableSize)) return 0;
   else return &alarms[index];
}

//
// GetNextEntry
//
const ALARM_TABLE* GuiAlarmTable::PeekNextEntry ()
{
   if ((index + 1 < 0) || (index + 1 >= tableSize)) return 0;
   else return &alarms[index + 1];
}


//
// GetNextEntry
//
const ALARM_TABLE* GuiAlarmTable::GetNextEntry ()
{
   if ((index + 1 < 0) || (index + 1 >= tableSize)) return 0;
   else
   {
      ++index;
      return &alarms[index];
   }
}

//
// GetPreviousEntry
//
const ALARM_TABLE* GuiAlarmTable::PeekPreviousEntry ()
{
   if ((index - 1 < 0) || (index - 1 >= tableSize)) return 0;
   else return &alarms[index - 1];
}

//
// GetPreviousEntry
//
const ALARM_TABLE* GuiAlarmTable::GetPreviousEntry ()
{
   if ((index - 1 < 0) || (index - 1 >= tableSize)) return 0;
   else
   {
      --index;
      return &alarms[index];
   }
}

/* FORMAT HASH e082743951fe08fa6905b9eb4948a8f7 */

/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      screenIdMacros.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This is a set of macros that make it easier to define functions fire
 *	happen when a certain screen is displayed.
 *
 ******************************************************************************/

//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// GLOBAL DEFINITIONS ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GLOBAL_DEFINITIONS__
#define __GLOBAL_DEFINITIONS__

#define SCREENSHOTPREFIX "manualShots"

#define SCREEN_CHANGE_NAV(__SCREEN_NAME__) screenChangeNav_ ## __SCREEN_NAME__ ## ()
#define SCREEN_CHANGE_SNAP(__SCREEN_NAME__) screenChangeSnap_ ## __SCREEN_NAME__ ## ()
#define RUNSUMM_STATS_NAV(__SCREEN_NAME__) runSumStatsNav_ ## __SCREEN_NAME__ ## ()
#define RUNSUMM_STATS_SNAP(__SCREEN_NAME__) runSumStatsSnap_ ## __SCREEN_NAME__ ## ()

#define UNIMPLEMENTED(__PREFIX__, __SCREEN_NAME__)	\
__PREFIX__##__SCREEN_NAME__##()						\
{													\
	DataLog (log_level_sim_main_routine_error)		\
		<< #__PREFIX__ << #__SCREEN_NAME__ 			\
		<<  "() not implemented at this level."		\
		<< endmsg;									\
}

#define SCREEN_CHANGE_NAV_UNIMPLEMENTED(__SCREEN_NAME__)	UNIMPLEMENTED(screenChangeNav_,		__SCREEN_NAME__)
#define SCREEN_CHANGE_SNAP_UNIMPLEMENTED(__SCREEN_NAME__)	UNIMPLEMENTED(screenChangeSnap_,	__SCREEN_NAME__)
#define RUNSUMM_STATS_NAV_UNIMPLEMENTED(__SCREEN_NAME__)	UNIMPLEMENTED(runSumStatsNav_,		__SCREEN_NAME__)
#define RUNSUMM_STATS_SNAP_UNIMPLEMENTED(__SCREEN_NAME__)	UNIMPLEMENTED(runSumStatsSnap_,		__SCREEN_NAME__)

#endif // #ifndef __GLOBAL_DEFINITIONS__


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////__SNAP_FUNC_PROTOTYPES__////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __SNAP_FUNC_PROTOTYPES__
#define SCREEN_ID_MACRO(__SCREEN_NAME__)			\
virtual void SCREEN_CHANGE_SNAP(__SCREEN_NAME__);	\
virtual void RUNSUMM_STATS_SNAP(__SCREEN_NAME__);
#endif // #ifdef __SNAP_FUNC_PROTOTYPES__

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////__FUNC_PROTOTYPES__/////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __FUNC_PROTOTYPES__
#define SCREEN_ID_MACRO(__SCREEN_NAME__)			\
virtual void SCREEN_CHANGE_NAV(__SCREEN_NAME__);	\
virtual void SCREEN_CHANGE_SNAP(__SCREEN_NAME__);	\
virtual void RUNSUMM_STATS_NAV(__SCREEN_NAME__);	\
virtual void RUNSUMM_STATS_SNAP(__SCREEN_NAME__);
#endif // #ifdef __FUNC_PROTOTYPES__

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////__SCREEN_CHANGE_SWITCH__////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __SCREEN_CHANGE_SWITCH__
#define SCREEN_ID_MACRO(__SCREEN_NAME__)	\
case __SCREEN_NAME__ :						\
    SCREEN_CHANGE_SNAP(__SCREEN_NAME__);	\
	SCREEN_CHANGE_NAV(__SCREEN_NAME__);		\
	break;
#endif // #ifdef __SCREEN_CHANGE_SWITCH__

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////__RUN_SUM_SWITCH__//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __RUN_SUM_SWITCH__
#define SCREEN_ID_MACRO(__SCREEN_NAME__)	\
case __SCREEN_NAME__ :						\
    RUNSUMM_STATS_SNAP(__SCREEN_NAME__);	\
	RUNSUMM_STATS_NAV(__SCREEN_NAME__);		\
	break;
#endif // #ifdef __RUN_SUM_SWITCH__

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////__UNIMPLEMENTED_SHOTS_FUNCS__////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __UNIMPLEMENTED_SHOTS_FUNCS__
// This macro defines an error message that shows when a function
// is called without being defined in the subclass.
#define SCREEN_ID_MACRO(__SCREEN_NAME__)											\
void ScreenshotsForOpsManualNav::SCREEN_CHANGE_SNAP_UNIMPLEMENTED(__SCREEN_NAME__);	\
void ScreenshotsForOpsManualNav::RUNSUMM_STATS_SNAP_UNIMPLEMENTED(__SCREEN_NAME__);
#endif // #ifdef __UNIMPLEMENTED_SHOTS_FUNCS__

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////__NAV_FUNC_IMPLEMENTATIONS__////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __NAV_FUNC_IMPLEMENTATIONS__
// This macro defines an error message that shows when a function
// is called without being defined in the subclass.
#define SCREEN_ID_MACRO(__SCREEN_NAME__)											\
void BaseShots::SCREEN_CHANGE_NAV_UNIMPLEMENTED(__SCREEN_NAME__);	\
void BaseShots::SCREEN_CHANGE_SNAP_UNIMPLEMENTED(__SCREEN_NAME__);	\
void BaseShots::RUNSUMM_STATS_NAV_UNIMPLEMENTED(__SCREEN_NAME__);	\
void BaseShots::RUNSUMM_STATS_SNAP_UNIMPLEMENTED(__SCREEN_NAME__);
#endif // #ifdef __NAV_FUNC_IMPLEMENTATIONS__

//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// DEFAULT MACRO DEFINITION //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SCREEN_ID_MACRO
#define SCREEN_ID_MACRO(__SCREEN_NAME__)
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// MACRO CALLS ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

SCREEN_ID_MACRO( GUI_SCREEN_SYSDONOR					);
SCREEN_ID_MACRO( GUI_SCREEN_SYSDISP						);
SCREEN_ID_MACRO( GUI_SCREEN_SYSCLAMP					);
SCREEN_ID_MACRO( GUI_SCREEN_SYSTEST						);
SCREEN_ID_MACRO( GUI_SCREEN_ABORTCNF					);
SCREEN_ID_MACRO( GUI_SCREEN_DISCONN						);
SCREEN_ID_MACRO( GUI_SCREEN_DONCONN						);
SCREEN_ID_MACRO( GUI_SCREEN_DONINFO						);
SCREEN_ID_MACRO( GUI_SCREEN_DONVITAL					);
SCREEN_ID_MACRO( GUI_SCREEN_POWERUP						);
SCREEN_ID_MACRO( GUI_SCREEN_RUNPROC						);
SCREEN_ID_MACRO( GUI_SCREEN_RUNSUMM1					);
SCREEN_ID_MACRO( GUI_SCREEN_RUNSUMM2					);
SCREEN_ID_MACRO( GUI_SCREEN_SYSACATT					);
SCREEN_ID_MACRO( GUI_SCREEN_TROUBLE						);
SCREEN_ID_MACRO( GUI_SCREEN_ALARM						);
SCREEN_ID_MACRO( GUI_SCREEN_PREDICT						);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG						);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG_MACHINE				);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG_LANGUAGE				);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG_PLASMA				);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG_PLATELET				);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG_PROCEDURE			);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG_RBC					);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG_DONOR				);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG_PRIORITY				);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG_REPLACEMENT_FLUID	);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG_VOLUME_LIMITS		);
SCREEN_ID_MACRO( GUI_SCREEN_REPFLUID					);
SCREEN_ID_MACRO( GUI_SCREEN_SETID						);
SCREEN_ID_MACRO( GUI_SCREEN_CFR11						);
SCREEN_ID_MACRO( GUI_SCREEN_BARCODE						);
SCREEN_ID_MACRO( GUI_SCREEN_SYSCASSETTE					);
SCREEN_ID_MACRO( GUI_SCREEN_PTF_MOUNT					);
SCREEN_ID_MACRO( GUI_SCREEN_PAS_INIT_SPLIT				);
SCREEN_ID_MACRO( GUI_SCREEN_CLAMP_CROSSOVER				);
SCREEN_ID_MACRO( GUI_SCREEN_RUNSUM_PLT					);
SCREEN_ID_MACRO( GUI_SCREEN_RUNSUM_PLASMA				);
SCREEN_ID_MACRO( GUI_SCREEN_RUNSUM_RBC					);
SCREEN_ID_MACRO( GUI_SCREEN_RUNSUM_RBC_PROD1			);
SCREEN_ID_MACRO( GUI_SCREEN_RUNSUM_RBC_PROD2			);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIG_METERED_SOLUTIONS	);
SCREEN_ID_MACRO( GUI_SCREEN_AAS_CLAMP_CHANNEL			);
SCREEN_ID_MACRO( GUI_SCREEN_AAS_CHANNEL_TEST			);
SCREEN_ID_MACRO( GUI_SCREEN_AAS_CONNECT_PAS				);
SCREEN_ID_MACRO( GUI_SCREEN_AAS_CONNECT_RAS				);
SCREEN_ID_MACRO( GUI_SCREEN_AAS_PRIME					);
SCREEN_ID_MACRO( GUI_SCREEN_AAS_PAS_SPLIT				);
SCREEN_ID_MACRO( GUI_SCREEN_AAS_ADDITION				);
SCREEN_ID_MACRO( GUI_SCREEN_MSS_END						);
SCREEN_ID_MACRO( GUI_SCREEN_AAS_RUN_SUMMARY				);
SCREEN_ID_MACRO( GUI_SCREEN_DRBC_SPLIT					);
SCREEN_ID_MACRO( GUI_SCREEN_AAS_TRANSITION_DUMMY		);
SCREEN_ID_MACRO( GUI_SCREEN_POWERDOWN					);
SCREEN_ID_MACRO( GUI_SCREEN_PAS_EMPTY					);
SCREEN_ID_MACRO( GUI_SCREEN_JP_CLAMP_BLOOD_DIVERSION	);
SCREEN_ID_MACRO( GUI_SCREEN_RBC_SPLIT_INIT				);
SCREEN_ID_MACRO( GUI_SCREEN_SEAL_AND_DETACH				);
SCREEN_ID_MACRO( GUI_SCREEN_CONFIRM_DON_INFO			);

#undef SCREEN_ID_MACRO


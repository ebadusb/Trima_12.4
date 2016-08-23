/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simAlarms.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 ******************************************************************************/

#include <vxworks.h>
#if CPU == SIMNT
#include "trima_datalog.h"
#include "simAlarms.hpp"
#include "guiglobs.hpp"
#include "simDelay.hpp"
#include "string_screen_mapper.hpp"
#include "bwindow.hpp"
#include "alarmcontainer.h"
#include "simScreenTouch.hpp"
#include "substatemap.h"
#include "screen_config.hpp"
#include "statline.hpp"
#include "simScreenCapture.hpp"
#include "simUtilities.hpp"
#include "simMsgSys.hpp"

SimAlarms::alarm_state_mapping SimAlarms::alarm_state_mappings[] =
{
	{
		STARTUP_TESTS,
		{
			DOOR_LATCH_FAILURE, LEAK_TEST_FAILURE, VALVE_TEST_FAILURE, LAST_PROCEDURE_ALARM
		}
	},
	{
		DISPOSABLE_TEST,
		{
			AC_DETECTED_PRIOR_TO_AC_PRIME_FAILURE, ACCESS_NEGATIVE_ALARM_NOT_REACHED, ACCESS_PRESSURE_DECAY_FAILURE,
			APS_AUTO_ZERO_FAILED_MAX, APS_AUTO_ZERO_FAILED_MIN, APS_AUTO_ZERO_FAILED_RANGE, CASSETTE_FAILURE,
			CASSETTE_INVALID_ALARM, CASSETTE_UNRECOGNIZED_ALARM, CASSETTE_REF_MISMATCH_ALARM, CONCENTRATION_OUT_OF_TOLERANCE,
			EVACUATION_PRESSURE_ALERT, EVACUATION_PRESSURE_ALERT_FINAL, FAILURE_TO_CLOSE_CROSSOVER, FAILURE_TO_PRESSURIZE_INLET,
			FAILURE_TO_PRESSURIZE_INLET_RAS_SET, FAILURE_TO_PRESSURIZE_INLET_NON_RAS_SET, FAILURE_TO_RELIEVE_PRESSURE,
			PUMP_RPM_COMMAND_FAILURE,SET_IDENTIFICATION_ALARM, LAST_PROCEDURE_ALARM
		}
	},
	{
		AC_PRIME,
		{
			AC_PRIME_INLET_ALARM, AC_DETECTION_ALARM, PRIME_FAILURE, LAST_PROCEDURE_ALARM
		}
	},
	{
		BLOOD_PRIME,
		{
			AC_LEVEL_ALARM, AC_OCCLUSION_DETECTED, APS_DECOUPLING, APS_DECOUPLING_PAUSE, APS_DURING_PAUSE_HI, APS_DURING_PAUSE_LOW,APS_PAUSE,APS_PUMPS_SLOW,
			APS_TOO_HIGH_ALERT, APS_TOO_LOW, CPS_PTF_BASELINE_TEST_ALARM, CPS_PTF_BASELINE_TEST_ALERT, CPS_PTF_PRIME_TEST,
			CPS_TOO_HIGH_ALERT_NO_CLAMP, CPS_TOO_HIGH_ALERT_ONE_CLAMP, CPS_TOO_HIGH_ALERT_THREE_CLAMP, CPS_TOO_HIGH_ALERT_THREE_CLAMP, CPS_OUT_OF_RANGE_AT_SET_LOWER,
			CPS_HIGH_DURING_RBC_COLLECT_PROD2_OK_ALERT, LOW_LEVEL_DETECTED_PRIOR_TO_EXPECTED,
			LOW_LEVEL_SENSOR_SEEN_TOO_LATE_FAILURE, LOW_LEVEL_SENSOR_SEEN_TOO_SOON_FAILURE, LAST_PROCEDURE_ALARM
		}
	},
	{
		BLOOD_RUN,
		{
			CENTRIFUGE_HARDWARE_FAILURE, CENTRIFUGE_INTEGRAL_FAILURE, CENTRIFUGE_SHUTDOWN_FAILURE, COLLECT_VALVE_FAILURE, DOOR_LOCK_ALARM,
			DOOR_OPEN_ALARM, FIRST_CYCLE_TOO_LATE_ALARM, FIRST_CYCLE_TOO_SOON_ALARM, HIGH_CENTRIFUGE_COMMAND, HYPOVOLEMIA_ALARM,
			LEAK_DETECTED_ALERT, LEAK_DETECTOR_ERROR_FAILURE, LOW_PLATELET_CONCENTRATION_ALERT, MAINTAIN_DONOR_ACCESS_ALERT,
			MOTOR_FAILED_TO_MAINTAIN_SPEED, MOTOR_INTEGRATION_ERROR, MOTOR_VOLUME_ERROR, MOTOR_DISABLED,
			ONE_MINUTE_PAUSE, OVERTEMP_ALARM, OVERTEMP_FAILURE, PAUSE_BEFORE_SPILLOVER, PAUSE_BUTTON_ALARM,
			PLASMA_VALVE_FAILURE, POWER_FAIL_ABORT, POWER_FAIL_CONTINUE, POWER_FAIL_RINSEBACK, POWER_TEST_FAILURE, PREDICTION_PAUSE,
			PROCEDURE_MODIFIED, RBC_DETECTOR_CAL_FAILURE, RBC_VALVE_FAILURE, REPLACEMENT_FLUID_ALARM, REPLACEMENT_FLUID_BOLUS_ALARM,
			REPLACEMENT_FLUID_NEW_BAG_ALARM, REPLACEMENT_FLUID_ALARM_RINSEBACK, RESERVOIR_ALARM, RESERVOIR_DRAW_TOO_LONG_ALARM_NO_CLAMP,
			RESERVOIR_DRAW_TOO_LONG_ALARM_ONE_CLAMP, RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_NO_CLAMP,
			RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_ONE_CLAMP, RESERVOIR_DRAW_TOO_LONG_ALARM_THREE_CLAMP,
			RESERVOIR_DRAW_TOO_SHORT_ALARM_NO_CLAMP, RESERVOIR_DRAW_TOO_SHORT_ALARM_ONE_CLAMP, RESERVOIR_DRAW_TOO_SHORT_ALARM_THREE_CLAMP,
			RESERVOIR_FULL_AFTER_RETURN, RESERVOIR_RETURN_TOO_LONG_ALARM_A1, RESERVOIR_RETURN_TOO_LONG_ALARM_NO_CLAMP,
			RESERVOIR_RETURN_TOO_LONG_ALARM_ONE_CLAMP, RESERVOIR_RETURN_TOO_LONG_ALARM_THREE_CLAMP, RESERVOIR_RETURN_TOO_SHORT_ALARM_NO_CLAMP,
			RESERVOIR_RETURN_TOO_SHORT_ALARM_ONE_CLAMP, RESERVOIR_RETURN_TOO_SHORT_ALARM_THREE_CLAMP, RETURN_LINE_OCCLUSION,
			RETURN_LINE_OCCLUSION_A1, SPILLOVER_DETECTED, STOP_BUTTON_ALARM, TEN_MINUTE_PAUSE, TEN_MINUTE_PAUSE_ALARM,
			TOO_MANY_RESERVOIR_DRAW_ALARMS, TOO_MANY_RESERVOIR_RETURN_ALARMS, VALVE_HARDWARE_FAILURE, minus12V_TOO_HIGH,
			minus12V_TOO_LOW, plus12V_TOO_HIGH, plus12V_TOO_LOW, plus24V_TOO_HIGH, plus24V_TOO_LOW, plus64V_TOO_HIGH, plus64V_TOO_LOW,
            APS_TOO_HIGH_PTF_ALERT, CPS_HIGH_DURING_RBC_COLLECT_ALARM, CPS_HIGH_DURING_RBC_COLLECT_ALERT, GENERIC_PLT_WBC_CONTAMINATION_ALERT, 
			GENERIC_PLT_WBC_CONTAMINATION_OBTRUSIVE_ALERT, HEMOLYSIS_DETECTED_ALARM, SAFETY_NOT_RESPONDING, AUTOFLOW_TIME_ALERT, 
			SILENT_PAUSE_FOR_RET_TOO_SHORT_CHECK, LAST_PROCEDURE_ALARM
		}
	},
	{
		POST_RUN,
		{
			DISCONNECT_TEST_ALARM, DISCONNECT_TEST_ALERT, DISCONNECT_TEST_MSS_ALARM, DISCONNECT_TEST_MSS_ALERT,
			UNLOADING_BAD_CASSETTE_POSITION, LAST_PROCEDURE_ALARM
		}
	},
	{
		METERED_STORAGE,
		{
			AC_CLOSURE_FAILED_ALARM, AC_CLOSURE_FAILED_ALERT, AC_NOT_REMOVED_ALERT, APS_TOO_HIGH_ALARM_MSS,
			APS_TOO_HIGH_ALERT_MSS, CHANNEL_CLAMP_NOT_CLOSED_ADDITION_ALERT, CHANNEL_CLAMP_NOT_CLOSED_ALERT,
			CHANNEL_VOLUME_REMAINING_HIGH, CPS_TOO_HIGH_ALARM, INLET_VOLM_ALARM, MSS_PLT_SS_EMPTY, MSS_PRIME_DELAY_PLT_ALERT,
			MSS_PRIME_DELAY_RBC_ALERT, MSS_PRIME_TOO_SOON_PLT_ALERT, MSS_PRIME_TOO_SOON_RBC_ALARM, MSS_RBC_SS_EMPTY,
			MSS_DRBC_SS_EMPTY, APS_HIGH_MSS_P1_OK_ALERT, AC_CLOSURE_FAILED_CONTINUE_PAS_ALARM, MSS_MISCONNECTED_2ND_PAS_BAG, 
			APS_TOO_HIGH_ALARM_MSS_WITH_PAS, LAST_PROCEDURE_ALARM
		}
	},
	{
	    DEFAULT_STATE,
	    {
	    	CONTROL_SOFT_WATCHDOG, LAST_PROCEDURE_ALARM
	    }
	},
	{
		LAST_PROCEDURE_STATE,
		{
			LAST_PROCEDURE_ALARM
		}
	}
};


void SimAlarms::walkThroughControlAlarms()
{
	SimMsgSys msgSys;
    SimUtilities::autoNavOn(false);
	
	SimUtilities::setSystemState(STARTUP_TESTS);

	DataLog (log_level_sim_alarm_info) << "walkThroughControlAlarms() begin" << endmsg;

	AlarmTable alrmMap;
	GuiAlarmTable alarmTable;
	ALARM_VALUES alarm_id;
	State_names state;
	for (int i=0; (state=alarm_state_mappings[i].state) != LAST_PROCEDURE_STATE; ++i)
	{
		SimUtilities::setSystemState(state);
		for (int j=0; (alarm_id=alarm_state_mappings[i].alarms[j]) != LAST_PROCEDURE_ALARM; ++j)
		{
			AlarmStruct& alrmEntry = alrmMap[alarm_id];
				//Special case for DRBC Split Alert.
				if((alarm_state_mappings[i].alarms[j] == DRBC_SPLIT_ALERT) && (alarm_state_mappings[i].alarms[j] == alarm_state_mappings[i].alarms[j-1]))
				{
					if (!guiglobs::cassette_mgr)
					{
						DataLog (log_level_sim_alarm_error) << "Cassette manager object is NULL." << endmsg;
						return;
					}

					clearAlarm();
					//If this is the second one in a row, change the chosen cassette so we get a different version.
					if(guiglobs::cassette_mgr->currentSetIsRas())
					{
						//364780
						guiglobs::cassette_mgr->LookupCassette("800500");
					}
					else
					{
						//6634117
						guiglobs::cassette_mgr->LookupCassette("80520");
					}
					if(state == BLOOD_RUN)
						guiglobs::cdsData.run.FirstDrawComplete.Set(true);
				}

				DataLog (log_level_sim_alarm_info) <<"Alarm " << alarm_id << " description: \""<< alrmEntry.getDescription() << "\"" << endmsg;

				ALARM_TYPES alrmType = alrmEntry.getType();
				const ALARM_TABLE *alarmItem = alarmTable.GetEntry(alarm_id);
				if (alarmItem == NULL)
					DataLog (log_level_sim_alarm_error) <<"WARNING: Alarm " << alarm_id << " Null alarm table entry.  Skipping it." << endmsg;
				else
				{
					sendAlarm(alarm_id,alrmType);
					SimDelay::seconds(1);
					if ((state >= DONOR_CONNECTED) && (state <= DONOR_DISCONNECT))
						SimUtilities::setRinseback();
					SimDelay::milliseconds(500);
					GUI_SCREEN_ID screen_id = alarmItem->goto_screen_gui;
					if (screen_id == GUI_SCREEN_NULL_SCREEN)
						screen_id = GUI_SCREEN_ALARM;
					SimScreenCapture::screenshot(screen_id, "%05d_%s", alarm_id, alarmItem->alarm_name_literal);

					while (alarmTable.PeekNextEntry() &&
						   alarmTable.PeekNextEntry()->alarm_name == alarmItem->alarm_name)
					{
						SimScreenTouch::touch(GUI_BUTTON_CONTINUE, screen_id);
						alarmItem = alarmTable.GetNextEntry();
						SimDelay::seconds(1);
						SimScreenCapture::screenshot(screen_id, "%05d_%s", alarm_id, alarmItem->alarm_name_literal);
					}
					if (screen_id != GUI_SCREEN_ALARM)
						SimScreenTouch::touch(GUI_BUTTON_CONTINUE, screen_id);
					else
						SimScreenTouch::touch(GUI_BUTTON_ALARM_CONTINUE, screen_id);
				}
		}
	}
	clearAlarm();

	DataLog (log_level_sim_alarm_info) <<"walkThroughControlAlarms() end" << endmsg;
} 

void SimAlarms::walkThroughSafetyAlarms()
{
	SimMsgSys msgSys;
	SimUtilities::autoNavOn(false);
	
	SimUtilities::setSystemState(STARTUP_TESTS);

	DataLog (log_level_sim_alarm_info) <<"walkThroughSafetyAlarms() begin" << endmsg;
	
	GuiAlarmTable alarmTable;
	for (unsigned int i = FIRST_SAFETY_ALARM+1; i < LAST_SAFETY_ALARM; ++i)
	//for (ALARM_VALUES i = (ALARM_VALUES)(FIRST_SAFETY_ALARM+1); i < LAST_SAFETY_ALARM; i = (ALARM_VALUES)(i + 1))
	{		
		ALARM_VALUES alarm = (ALARM_VALUES)i;		
		
		if (alarm != AC_OFF_DURING_DRAW)			// why???
		{
			const ALARM_TABLE *alarmItem = alarmTable.GetEntry(alarm);
			if (NULL != alarmItem)
			{
				sendAlarm(alarm,A2);
			    SimDelay::seconds(1);
			
				SimScreenCapture::screenshot(GUI_SCREEN_ALARM, "%05d_%s", i, alarmItem->alarm_name_literal);
			}
		}
	}
	//
	//	Clear alarm screens
	//
	DataLog (log_level_sim_alarm_info) <<"Done with the safety alarms. Clearing the screen." << endmsg;
	clearAlarm();


	DataLog (log_level_sim_alarm_info) <<"walkThroughSafetyAlarms() end" << endmsg;
}


void SimAlarms::sendAlarm(ALARM_VALUES a_name, ALARM_TYPES a_type = R1, ALARM_STATES a_state = ACTIVE)
{
	SimMsgSys tmpMsgSys;

	DataLog (log_level_sim_alarm_info) << "sendAlarm(" << a_name << "; " << a_type << "; " << a_state << ")" << endmsg;

	GUI_Alarm_struct  aData;
	aData.alarm_name = a_name;
	aData.alarm_type = a_type;
	aData.alarm_state = a_state;

	Message<GUI_Alarm_struct>  aMsg;
	aMsg.init(MessageBase::SEND_LOCAL);
	aMsg.send(aData);

	SimDelay::seconds(1);
}

void SimAlarms::clearAlarm()
{
	DataLog (log_level_sim_alarm_info) << "clearAlarm()" << endmsg;

	sendAlarm(CASSETTE_FAILURE, R1);
	SimDelay::milliseconds(750);
	SimScreenTouch::touch(GUI_BUTTON_ALARM_CONTINUE, GUI_SCREEN_ALARM);
	SimDelay::milliseconds(750);
}

void SimAlarms::alarmRoutine()
{
	DataLog (log_level_sim_alarm_info) << "Starting alarms routine." << endmsg;
	SimMsgSys alarmRoutineMsgSystem;

	SimUtilities::setSystemState(STARTUP_TESTS);
	
	walkThroughSafetyAlarms();
	walkThroughControlAlarms();
}


#endif /* CPU == SIMNT */




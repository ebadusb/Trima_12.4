/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simUtilities.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 *******************************************************************************/

#include <vxworks.h>
#if CPU == SIMNT

// From Trima General
#include "trima_datalog.h"
#include "alarmcontainer.h"
#include "substatemap.h"
#include "pqinformation.h"

// From CDS
#include "config_cds.h"
#include "procadjust_cds.h"
#include "guiUnitTestCds.h"
#include "software_cds.h"

// From GUI
#include "gui.h"
#include "guiglobs.hpp"
#include "bwindow.hpp"
#include "scrncomm.hpp"
#include "statline.hpp"
#include "statBarBase.hpp"
#include "operator_config.h"
#include "screen_config.hpp"
#include "text_utils.h"
#include "cfr_list.h"
#include "string_screen_mapper.hpp"
#include "runsum_product.hpp"

// From unit_test
#include "simUtilities.hpp"
#include "simDelay.hpp"
#include "simScreenTouch.hpp"
#include "simScreenCapture.hpp"
#include "simMsgSys.hpp"
#include "simAlarms.hpp"

GuiUnitTest_CDS SimUtilities::cds::unitTest(ROLE_RW);
Config_CDS SimUtilities::cds::writeableConfig(ROLE_RW);
Operator_Config SimUtilities::cds::config;

SimScreenCount SimUtilities::screenCounter;
SimScreenCount SimUtilities::runsumResponseCounter;
SimScreenCount SimUtilities::procStatusResponseCounter;

bool SimUtilities::_cfr11EventsAdded=false;

void SimUtilities::init()
{
	log_level_sim_info.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_error.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_timing_error.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_screen_touch_info.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_screen_touch_error.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_responder_info.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_responder_error.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_main_routine_info.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_main_routine_error.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_utility_info.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_utility_error.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_mapper_error.consoleOutput(DataLog_ConsoleEnabled);
	log_level_sim_gui_side_error.consoleOutput(DataLog_ConsoleEnabled);

	cds::init();
	UnitSystem::init();
}

void SimUtilities::cds::init()
{
	config.lang       = guiglobs::cdsData.config.LangUnit.Get();
	config.machine    = guiglobs::cdsData.config.Machine.Get();
	config.procedure  = guiglobs::cdsData.config.Procedure.Get();
	config.predict    = guiglobs::cdsData.config.Predict.Get();
	config.templates  = guiglobs::cdsData.config.Templates.Get();
	config.definition = guiglobs::cdsData.config.Definition.Get();
}

void SimUtilities::cds::writeConfig()
{
	writeableConfig.LangUnit.Set(config.lang);
	writeableConfig.Machine.Set(config.machine);
	writeableConfig.Procedure.Set(config.procedure);
	writeableConfig.Predict.Set(config.predict);
	writeableConfig.Templates.Set(config.templates);
	writeableConfig.Definition.Set(config.definition);

	writeableConfig.Write();
}

const timespec SimUtilities::delay={0,500000000};

SimUtilities::UnitSystem::UNIT_SYSTEM SimUtilities::UnitSystem::_currentUnits=SimUtilities::UnitSystem::UNITS_ENGLISH;

DataLog_Stream & operator << (DataLog_Stream &os, State_names stateName)
{
   switch (stateName)
   {
   case FIRST_STATE:				os << "FIRST_STATE";				break;
   case SELF_TEST:					os << "SELF_TEST";					break;
   case STARTUP_TESTS:				os << "STARTUP_TESTS";				break;
   case DISPOSABLE_TEST:			os << "DISPOSABLE_TEST";			break;
   case AC_CONNECTED:				os << "AC_CONNECTED";				break;
   case AC_PRIME:					os << "AC_PRIME";					break;
   case DONOR_CONNECTED:			os << "DONOR_CONNECTED";			break;
   case BLOOD_PRIME:				os << "BLOOD_PRIME";				break;
   case BLOOD_RUN:					os << "BLOOD_RUN";					break;
   case BLOOD_RINSEBACK:			os << "BLOOD_RINSEBACK";			break;
   case DONOR_DISCONNECT:			os << "DONOR_DISCONNECT";			break;
   case POST_RUN:					os << "POST_RUN"; 					break;
   case METERED_STORAGE:			os << "METERED_STORAGE";			break;
   case METERED_STORAGE_DISCONNECT:	os << "METERED_STORAGE_DISCONNECT";	break;
   case SERVICE_MODE:				os << "SERVICE_MODE";				break;
   case DEFAULT_STATE:				os << "DEFAULT_STATE";				break;
   default:							os << "UNKNOWN";					break;
   }

   os << "(" << (int)stateName << ")";
   return os;
};

bool SimUtilities::autoNavOn(bool isOn) { cds::unitTest.autoScreenshots.Set(isOn); return isOn; }
bool SimUtilities::autoNavOn()       	{ return cds::unitTest.autoScreenshots.Get(); 			}

void SimUtilities::changeScreens(GUI_SCREEN_ID newScreen, const char* screenParam="", invocation_method invocMethod=INVOKE_SCREEN)
{
	switch (invocMethod)
	{
	case DISPLAY_AS_CHILD:				Base_Apheresis_Window::goto_child_screen(newScreen,screenParam);				break;
	case INVOKE_SCREEN:					Base_Apheresis_Window::goto_screen(newScreen,screenParam);						break;
	case DISPLAY_AS_STATELESS:			Base_Apheresis_Window::goto_stateless_screen(newScreen,screenParam);			break;
	case SWAP_CHILD_SCREEN:				Base_Apheresis_Window::swap_child_screen(newScreen,screenParam);				break;
	case SWAP_STATELESS_SCREEN:			Base_Apheresis_Window::swap_stateless_screen(newScreen,screenParam);			break;
	case SWAP_CHILD_WITH_SCREEN:		Base_Apheresis_Window::swap_child_stack_for_screen(newScreen,screenParam);		break;
	case SWAP_STATELESS_WITH_SCREEN:	Base_Apheresis_Window::swap_stateless_stack_for_screen(newScreen,screenParam);	break;
	}

	SimDelay::seconds(1);
}

const char* SimUtilities::stateIdToStr(State_names state)
{
	const char *result = SubstateMap::ssStr(state);
	if (result == NULL)
		return "UNKNOWN_STATE";
	else
		return result;
}

void SimUtilities::toggleSplitNotif(bool request)
{
	if (guiglobs::cdsData.rwConfig.procedure.key_plt_mss_split_notif != request)
	{
		// You can't work the switch notification button if MSS is off.
		toggleMasterPas(true);
		SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLT_MSS_SPLIT_NOTIF, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);
	}
}

void SimUtilities::toggleMasterPas(bool request)
{
	if (guiglobs::cdsData.rwConfig.procedure.key_mss_plt_on != request)
		SimScreenTouch::touch(GUI_BUTTON_CONFIG_PPC_ON_OFF, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);
}

void SimUtilities::toggleMasterRas(bool request)
{
	if (guiglobs::cdsData.rwConfig.procedure.key_mss_rbc_on != request)
		SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC_SOL_TOGGLE, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);
}

void SimUtilities::toggleDonorVolumeLimit(VOLUME_REMOVAL_TYPE which, bool request)
{
	int setting = guiglobs::cdsData.rwConfig.procedure.key_tbv_vol_setting;
	if (((setting & which) == which) != request)
	{
		switch (which)
		{
		case VOLUME_REMOVAL_TBV: SimScreenTouch::touch(GUI_BUTTON_CONFIG_DONOR_TBV, GUI_SCREEN_CONFIG_VOLUME_LIMITS); break;
		case VOLUME_REMOVAL_WEIGHT: SimScreenTouch::touch(GUI_BUTTON_CONFIG_DONOR_VOLUME, GUI_SCREEN_CONFIG_VOLUME_LIMITS); break;
		case VOLUME_REMOVAL_ML_PER_KG: SimScreenTouch::touch(GUI_BUTTON_CONFIG_DONOR_BODY_WEIGHT, GUI_SCREEN_CONFIG_VOLUME_LIMITS); break;
		case VOLUME_REMOVAL_DRBC_BODY_VOL: SimScreenTouch::touch(GUI_BUTTON_CONFIG_DRBC_VOL_LIM, GUI_SCREEN_CONFIG_VOLUME_LIMITS); break;
		default: DataLog (log_level_sim_utility_error) << "ERROR: Unknown removal type " << which << endmsg; break;
		}
	}
}


void SimUtilities::setRinseback()
{
	State_names system_state = guiglobs::screen_status.get_current_system_state();
	if ((system_state >= DONOR_CONNECTED) && (system_state <= DONOR_DISCONNECT))
	{
		SimMsgSys msgSys;
		DataLog (log_level_sim_utility_info)  << "setRinseback()" << endmsg;
		RinsebackStatusMsg rinsebackStatusMsg;
		rinsebackStatusMsg.init(MessageBase::SEND_LOCAL);
		rinsebackStatusMsg.send(RINSEBACK_OK);
	}
}

void SimUtilities::setSystemState(State_names newSystemState)
{
	SimMsgSys msgSys;

	if (newSystemState <= FIRST_STATE || newSystemState >= SERVICE_MODE)
	{
		DataLog (log_level_sim_utility_error) << "Requested system state " << newSystemState << " out of range." << endmsg;
		return;
	}
	
    if (guiglobs::screen_status.get_current_system_state() == newSystemState)
	{
		DataLog (log_level_sim_utility_error) << "setSystemState(" << newSystemState << "). Current state not changed." << endmsg;
		return;
	}

	DataLog (log_level_sim_utility_info) << "setSystemState(" << newSystemState << "). Changing state from " << guiglobs::screen_status.get_current_system_state() << "." << endmsg;

	SystemStateChangeMsg msg(MessageBase::SEND_GLOBAL);
	msg.send(newSystemState);
	
	SimDelay::seconds(1);
	
	guiglobs::apheresis_status_bar->setState(newSystemState);

	SimDelay::seconds(3);
}

/*void SimUtilities::toggleOption(char * optionName)
{
	if(guiglobs::cdsData.config.SWOptionEnabled(optionName))
		guiglobs::cdsData.config.SWOptionSet(optionName, false);
	else
		guiglobs::cdsData.config.SWOptionSet(optionName, true);
}*/

string SimUtilities::findAndReplace(const string& source, const string& text, const string& replace)
{
	std::string result = source;
	
	for(unsigned int i = result.find(text); i != string::npos; i = result.find(text))
	{
		result.replace(i,text.length(), replace);
	}
	
	return result;
}

CONFIG_DATA_LISTBOX_STRINGS SimUtilities::getCurrentLanguage()
{
	return (CONFIG_DATA_LISTBOX_STRINGS) guiglobs::cdsData.config.LangUnit.Get().key_lang;
}

bool SimUtilities::checkScreen(GUI_SCREEN_ID correctScreen, bool careAboutFailure=true)
{
	GUI_SCREEN_ID currentScreen = guiglobs::screen_status.get_current_displayed_screen();
	if (currentScreen != correctScreen)
	{
		if (careAboutFailure)
			DataLog (log_level_sim_utility_error) << "Expected " << correctScreen << ", actually on " << currentScreen << "." << endmsg;

		return false;
	}
	else
	{
		return true;
	}
}

int SimUtilities::getAasRunType()
{
	return MSS_BOTH;
	
	// Default to all being true.
	bool runIsPas = true;
	bool runIsRas = true;

	if (guiglobs::procedure_selected)
	{	// If a procedure is selected, base the run type on that.
		runIsPas = guiglobs::cdsData.procedures.getProcedure(CURR).metered_platelet;
		runIsRas = guiglobs::cdsData.procedures.getProcedure(CURR).metered_rbc;
	}
	else if (guiglobs::cassette_mgr && guiglobs::cassette_mgr->HasCurrentCassetteSet())
	{	// If no procedure is selected, but a cassette is selected, base the run type on that.
		runIsPas = guiglobs::cassette_mgr->currentSetIsPas();
		runIsRas = guiglobs::cassette_mgr->currentSetIsRas();
	}

	int runType = NON_MSS;

	if (runIsRas)
	{
		if (runIsPas) runType = MSS_BOTH;
		else runType = MSS_RBC;
	}
	else if (runIsPas) runType = MSS_PLT;
	else runType = NON_MSS;

    return runType;
}

bool SimUtilities::verifyStartingPoint()
{
	bool startingPointIsOk = 
		(guiglobs::screen_status.get_current_screen() == GUI_SCREEN_SYSDONOR &&
		guiglobs::screen_status.get_current_system_state() == STARTUP_TESTS);

	if (!startingPointIsOk)
		DataLog (log_level_sim_utility_error) << "ERROR: screenshot routine must begin on the two-button screen in the STARTUP_TESTS state." << endmsg;

	return startingPointIsOk;
}

void SimUtilities::enableLoadBtn()
{
	SimMsgSys msgSys;

   EnableLoadSystemMsg queryGUIMsg( MessageBase::SEND_LOCAL );
   queryGUIMsg.send( 4200 );
}

void SimUtilities::loadRefCodes()
{
	char *entries[][3] = {
		{"10001", "13505915", "h413777100011A"},
		{"10002", "10409360", "h413777100021A"},
		{"10003", "14603742", "h413777100031A"},
		{"10004", "09344118", "h413777100041E"},
		{"20001", "04545123", "h413777200011F"},
		{"20002", "00384678", "h413777200021H"}
	};
	guiglobs::cassette_mgr->DeleteAllCassettes();				// make room for these cassettes
	for (int i = 0; i < (sizeof(entries)/(3*sizeof(char*))); ++i)
		if (guiglobs::cassette_mgr->canFitCassetteIn(entries[i][0]))
			guiglobs::cassette_mgr->AddNewCassette(entries[i][0], entries[i][1], entries[i][2]);

	SimDelay::seconds(1);
}

void SimUtilities::toggleAmapPlatelet()
{
	bool value = Software_CDS::GetInstance().getFeature(AmapPlateletsAvailable);
	Software_CDS::GetInstance().SetFeature(AmapPlateletsAvailable, !value, AUTH_R_N_D, true);
}

bool SimUtilities::isAmapPlateletEnabled()
{
	return Software_CDS::GetInstance().getFeature(AmapPlateletsAvailable);
}

void SimUtilities::UnitSystem::toggle()
{
	units((units()==UNITS_ENGLISH)? UNITS_METRIC : UNITS_ENGLISH);
}

SimUtilities::UnitSystem::UNIT_SYSTEM SimUtilities::UnitSystem::units(UNIT_SYSTEM newUnits)
{
	SimMsgSys msgSys;
	DVitalsFromGUI msgDonVitals(MessageBase::SEND_GLOBAL);
	SDonorVitals     donVitalsStruct;
	bool configChanged=false,
		 donVitalsUnitsChanged=false;

	if (newUnits == UNITS_ENGLISH)
	{
		_currentUnits = UNITS_ENGLISH;

		if (cds::config.lang.key_crit_or_glob != RBC_UNIT_CRIT)
		{
			configChanged=true;
			cds::config.lang.key_crit_or_glob = RBC_UNIT_CRIT;
		}

		if (cds::config.lang.key_weight != LB_WEIGHT)
		{
			donVitalsUnitsChanged=true;
			configChanged=true;
			cds::config.lang.key_weight = LB_WEIGHT;
		}

		if (cds::config.lang.key_height != FT_HEIGHT)
		{
			donVitalsUnitsChanged=true;
			configChanged=true;
			cds::config.lang.key_height = FT_HEIGHT;
		}

		if (cds::config.lang.key_decimal_delimiter != COMMA_DELIMIT)
		{
			configChanged=true;
			cds::config.lang.key_decimal_delimiter = COMMA_DELIMIT;
		}

		if (donVitalsUnitsChanged)
		{
			guiglobs::gui_donor_data_repository.get_donor_vitals_struct (&donVitalsStruct);
			donVitalsStruct.cDonorHeightUnits = HEIGHT_UNITS_IN;
			donVitalsStruct.cDonorWeightUnits = WEIGHT_UNITS_LB;            
		}
	}
	else
	{
		_currentUnits = UNITS_METRIC;

		if (cds::config.lang.key_crit_or_glob != RBC_UNIT_HEMOGLOBIN)
		{
			configChanged=true;
			cds::config.lang.key_crit_or_glob = RBC_UNIT_HEMOGLOBIN;
		}

		if (cds::config.lang.key_weight != KG_WEIGHT)
		{
			donVitalsUnitsChanged=true;
			configChanged=true;
			cds::config.lang.key_weight = KG_WEIGHT;
		}

		if (cds::config.lang.key_height != CENTI_HEIGHT)
		{
			donVitalsUnitsChanged=true;
			configChanged=true;
			cds::config.lang.key_height = CENTI_HEIGHT;
		}

		if (cds::config.lang.key_decimal_delimiter != DECIMAL_DELIMIT)
		{
			configChanged=true;
			cds::config.lang.key_decimal_delimiter = DECIMAL_DELIMIT;
		}

		if (donVitalsUnitsChanged)
		{
            guiglobs::gui_donor_data_repository.get_donor_vitals_struct (&donVitalsStruct);
			donVitalsStruct.cDonorHeightUnits = HEIGHT_UNITS_CM;
			donVitalsStruct.cDonorWeightUnits = WEIGHT_UNITS_KG;
		}
	}
	

	// Don't bother writing this if nothing changed.
	if (configChanged)
	{
        cds::writeConfig();

		DataLog (log_level_sim_utility_info)
			<< "Setting units to " << ((_currentUnits==UNITS_METRIC)? "METRIC" : "ENGLISH")
			<< " (height=" << ((cds::config.lang.key_height==CENTI_HEIGHT)? "CM" : "FT")
			<< "; weight=" << ((cds::config.lang.key_weight==KG_WEIGHT)? "KG" : "LB")
			<< "; rbc=" << ((cds::config.lang.key_crit_or_glob==RBC_UNIT_HEMOGLOBIN)? "HGB" : "HCT")
			<< "; delim=" << ((cds::config.lang.key_decimal_delimiter==COMMA_DELIMIT)? "COMMA" : "DECIMAL")
            << ")" << endmsg;
	}

	if (donVitalsUnitsChanged)
	{
        DataLog (log_level_sim_utility_info) << "Sending updated donor vitals struct after units toggle." << endmsg;
		msgDonVitals.send(donVitalsStruct);
	}

	return _currentUnits;
}

void SimUtilities::UnitSystem::init() { units(UNITS_ENGLISH); }


void SimUtilities::sendProcedureData(RUNSTATE runState, unsigned short runInfo)
{
	SimMsgSys msgSys;
    Message<CURRENT_PROCEDURE_STATUS_STRUCT> procStatusMsg(MessageBase::SEND_LOCAL);
    CURRENT_PROCEDURE_STATUS_STRUCT procStatusStruct;

	unsigned int additionSetting = 0;

	if ((runInfo & PLT) && (runInfo & PAS)) additionSetting |= AasRunInfo::AAS_INFO_PAS;

	if ((runInfo & RAS))
	{
		if (runInfo & DRBC) additionSetting |= AasRunInfo::AAS_INFO_RAS_DOUBLE;
		else if (runInfo & SRBC) additionSetting |= AasRunInfo::AAS_INFO_RAS_SINGLE;
	}

	guiglobs::aasRunInfo.setAasInfo(additionSetting);

	procStatusStruct.alarm_condition			= 0;
	procStatusStruct.current_inlet_pressure_max	= 310.0;
	procStatusStruct.current_inlet_pressure_min	= -250.0;
	procStatusStruct.estimated_platelets = (runInfo & PLT)?  3.5f	: 0.0f;
	procStatusStruct.estimated_plasma	 = (runInfo & PLAS)?  200.0f: 0.0f;
	procStatusStruct.estimated_rbcs		 = (runInfo & DRBC)? 500.0f	: (runInfo & SRBC)? 250.0f : 0.0f;
    
	switch (runState)
	{
	case PRIME:
		procStatusStruct.return_pump_cycle			 = RETURN_PUMP_CYCLE_DRAW;
		procStatusStruct.use_startup_pressure_meter	 = 1;
		procStatusStruct.current_inlet_pressure		 = -82.95;
		procStatusStruct.current_platelets_collected = 0.0;
		procStatusStruct.current_plasma_collected	 = 0.0;
		procStatusStruct.current_rbc_collected		 = 0.0;
		break;

	case DRAW:
		procStatusStruct.return_pump_cycle			 = RETURN_PUMP_CYCLE_DRAW;
		procStatusStruct.use_startup_pressure_meter	 = 0;
		procStatusStruct.current_inlet_pressure		 = -82.95;
		procStatusStruct.current_platelets_collected = (runInfo & PLT)? 2.4f : 0.0f;
		procStatusStruct.current_plasma_collected	 = (runInfo & PLAS)? 85.0f : 0.0f;
		procStatusStruct.current_rbc_collected		 = ((runInfo & (SRBC | DRBC)) && !(runInfo & (PLT | PLAS)))? 173.0f : 0.0f;
        break;

	case RETURN:
		procStatusStruct.return_pump_cycle = RETURN_PUMP_CYCLE_RETURN;
		procStatusStruct.use_startup_pressure_meter   =    0;
		procStatusStruct.current_inlet_pressure	= 82.95;
		procStatusStruct.current_platelets_collected = (runInfo & PLT)? 2.4f : 0.0f;
		procStatusStruct.current_plasma_collected = (runInfo & PLAS)? 85.0f : 0.0f;
		procStatusStruct.current_rbc_collected = ((runInfo & (SRBC|DRBC)) && !(runInfo & (PLT|PLAS)))? 173.0f : 0.0f;
        break;

	case RINSEBACK:
		procStatusStruct.return_pump_cycle = RETURN_PUMP_CYCLE_RETURN;
		procStatusStruct.use_startup_pressure_meter   =    0;
		procStatusStruct.current_inlet_pressure	= 82.95;
		procStatusStruct.current_platelets_collected = procStatusStruct.estimated_platelets;
		procStatusStruct.current_plasma_collected = procStatusStruct.estimated_plasma;
		procStatusStruct.current_rbc_collected = procStatusStruct.estimated_rbcs;
        break;
	}

	DataLog (log_level_sim_utility_info) << "Sending proc data to GUI." << endmsg;

    procStatusMsg.send(procStatusStruct);
	SimDelay::seconds(2);
}

void SimUtilities::setAasVolume(int vol)
{
	SimMsgSys msgSys;
	AasAdditionVolumeUpdate volUpdateMsg(MessageBase::SEND_LOCAL);
	volUpdateMsg.send(vol);
	SimDelay::seconds(1);
}

void SimUtilities::setRunStats(RUN_SUMMARY_STRUCT& runSumStruct)
{
	time_t t;
	time( &t ); 
	struct tm *endtime = localtime( &t );
	struct tm *aas_time = endtime;
	aas_time->tm_min += 5;

	runSumStruct.rs1_end_of_run_time			  = *endtime;

	runSumStruct.rs1_total_AC_used              = (long) 550.0; // Units: mL
	runSumStruct.rs1_length_of_run              = (long)  74.0; // Units: minutes
	runSumStruct.rs1_post_platelet_count        = (long) 179.0; // Units: "n" where: n x 1000/uL
	runSumStruct.rs1_post_hct                   = (long)  40.0; // Units: %
	runSumStruct.rs1_actual_AC_to_donor         = (long) 387.0; // Units: mL
	runSumStruct.rs1_total_blood_vol_processed  = (long)3429.0; // Units: mL
	runSumStruct.rs1_residual_loss_plasma       = (long) 69.0;
	runSumStruct.rs1_residual_loss_prbc         = (long) 96.0;
	runSumStruct.replacement_fluid_volume       = (long)1200.0; // Units: ml
	runSumStruct.rs1_last_adjust_flow_rate 		= 11;
	runSumStruct.rs1_start_of_aas_time 			= *aas_time;

	runSumStruct.plasma_rinseback_status = SPECIAL_RINSEBACK_STATUS_COMPLETE;
	runSumStruct.saline_rinseback_status = SPECIAL_RINSEBACK_STATUS_INCOMPLETE;

	// Set all label flags to "TRUE"
	runSumStruct.PQdata.label_platelets_leukoreduced  = true;
	runSumStruct.PQdata.label_plasma_leukoreduced     = true;
	runSumStruct.PQdata.label_rbcs_leukoreduced	    = true;
	runSumStruct.PQdata.label_rbcs_leukoreduced_prod2 = true;

	//runSumStruct.PQdata.verify_platelet_wbc_reasons.Initialize(SPILL_RECOVERY);

	runSumStruct.PQdata.verify_platelet_wbc_reasons.Initialize(SPILL_DETECTION, SPILL_RECOVERY, STOPPED_CENTRIFUGE, RBC_DETECTOR_ERROR, HIGH_PLATELET_CONCENTRATION, HIGH_DELTA_HCT, AIR_BLOCK_RECOVERY, HIGH_HCT, LOW_DETECTED_PLATELET_CONCENTRATION, PLATELET_CONTAMINANT_DETECTION, RINSEBACK_NOT_COMPLETED, CHANNEL_CLAMP_ERROR);
	runSumStruct.PQdata.verify_platelet_wbc_reasons.Put(TRIPLE_PPC_AS_NON_LEUKOREDUCED);
	runSumStruct.PQdata.verify_platelet_yield_volume_reasons.Initialize(SPILL_DETECTION, SPILL_RECOVERY, STOPPED_CENTRIFUGE, HIGH_PLATELET_CONCENTRATION, AIR_BLOCK_RECOVERY, RUN_ENDED_EARLY, LOW_DETECTED_PLATELET_CONCENTRATION, HIGH_PLATELET_YIELD_DEVIATION, METERED_SOLUTION_VOLUME_ERROR, FINAL_CONC_NOT_ACHIEVED, METERED_SOLUTION_PRIME_ERROR);
	runSumStruct.PQdata.platelet_concentration_reasons.Initialize(PLATELET_CONCENTRATION_BELOW_STORAGE_RANGE, PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE);

	runSumStruct.PQdata.verify_plasma_wbc_reasons.Initialize(SPILL_DETECTION, SPILL_RECOVERY, STOPPED_CENTRIFUGE, RBC_DETECTOR_ERROR, AIR_BLOCK_RECOVERY, LOW_DETECTED_PLATELET_CONCENTRATION);
	runSumStruct.PQdata.verify_plasma_volume_reasons.Initialize(STOPPED_CENTRIFUGE, AIR_BLOCK_RECOVERY, RUN_ENDED_EARLY, HIGH_PLATELET_YIELD_DEVIATION);

	runSumStruct.PQdata.verify_rbc_volume_reasons.Initialize(STOPPED_CENTRIFUGE, AIR_BLOCK_RECOVERY, RUN_ENDED_EARLY, HIGH_PLATELET_YIELD_DEVIATION, METERED_SOLUTION_VOLUME_ERROR, APS_HIGH);
	runSumStruct.PQdata.verify_rbc_wbc_reasons.Initialize(CPS_HIGH, APS_HIGH);
	runSumStruct.PQdata.verify_rbc_quality_reasons.Initialize(CPS_HIGH, APS_HIGH);
	runSumStruct.PQdata.verify_rbc_volume_reasons_prod2.Initialize(STOPPED_CENTRIFUGE, AIR_BLOCK_RECOVERY, RUN_ENDED_EARLY, HIGH_PLATELET_YIELD_DEVIATION, METERED_SOLUTION_VOLUME_ERROR, APS_HIGH);
	runSumStruct.PQdata.verify_rbc_wbc_reasons_prod2.Initialize(CPS_HIGH, APS_HIGH);
	runSumStruct.PQdata.verify_rbc_quality_reasons_prod2.Initialize(CPS_HIGH, APS_HIGH);

	ProcPQI_CDS q(ROLE_RW);
	q.PQdata.Set(runSumStruct.PQdata);

	runSumStruct.rs2_platelet_volume      = (long) 250;  // Units: mL
	runSumStruct.rs2_yield_of_platelets   = (long) 3.5;  // Units: "n" where: n x 10^11
	runSumStruct.rs2_vol_AC_in_platelets  = (long) 57;  // Units: mL
	runSumStruct.rs2_SS_plat_vol          = (long) 0;   // Units: mL
	runSumStruct.rs2_plasma_volume        = (long) 200.0;  // Units: mL
	runSumStruct.rs2_vol_AC_in_plasma     = (long) 65.0;  // Units: mL
	runSumStruct.rs2_RBC_volume           = (long) 0;  // Units: mL
	runSumStruct.rs2_RBC_volume_2		  = (long) 0;
	runSumStruct.rs2_vol_AC_in_RBC        = (long) 0;  // Units: mL
	runSumStruct.rs2_vol_AC_in_RBC_2      = (long) 0;  // Units: mL													   
	runSumStruct.rs2_SS_rbc_vol			  = (long) 0;  // Units: mL
	runSumStruct.rs2_SS_rbc_vol_2         = (long) 0;

	runSumStruct.drbcAutoSplit = true;
	runSumStruct.run_complete = true;

	runSumStruct.platelet_salvage_volume = 15;
	runSumStruct.rbc_salvage_volume = 20;
	runSumStruct.rbc_salvage_volume_2 = 15;
}

void SimUtilities::sendRunsumm1Setup(bool showAasStartTime,
                                     bool showAvgInletFlow,
                                     bool showResidualLoss,
                                     bool plasmaRinsebackOn,
                                     bool salineRinsebackOn)
{
	sendTraverseMsg(RUNSUMM1_SETUP,
					showAasStartTime,
                    showAvgInletFlow,
                    showResidualLoss,
					plasmaRinsebackOn,
                    salineRinsebackOn);
}

void SimUtilities::sendRunStats()
{
	SimMsgSys msgSys;
	// The Product Quality CDS data access class
	PQInformation PQA;

	Message<RUN_SUMMARY_STRUCT> RunSumMsg; // update adjust info to GUI
	RUN_SUMMARY_STRUCT run_summary; // Update info for GUI
	setRunStats(run_summary);

	RunSumMsg.init(MessageBase::SEND_LOCAL);
    RunSumMsg.send(run_summary);
}


void SimUtilities::enableLoadSystem()
{
	SimMsgSys msgSys;
	EnableLoadSystemMsg queryGUIMsg( MessageBase::SEND_LOCAL );
	queryGUIMsg.send(0);
	SimDelay::seconds(1);
}

void SimUtilities::setRunComplete()
{
	SimMsgSys msgSys;
	RunCompleteMsg runCompleteMsg(MessageBase::SEND_LOCAL);
	runCompleteMsg.send();
	SimDelay::seconds(1);
}


void SimUtilities::setStatLine(TEXT_STRING_ID txt=TEXT_NULL, STATUS_LINE_TYPE type=ATTENTION_STATUS_LINE)
{
	GUI_SCREEN_ID originalOwningScreen = guiglobs::apheresis_status_line->get_owning_screen();
	GUI_SCREEN_ID currentScreen = guiglobs::screen_status.get_current_displayed_screen();

	if (originalOwningScreen != currentScreen)
		guiglobs::apheresis_status_line->reparent_status_line(*guiglobs::scrnComm->getScreenInstancePointer(currentScreen));

	if (txt==TEXT_NULL || txt==textEmpty) guiglobs::apheresis_status_line->reset_status_line();
	else guiglobs::apheresis_status_line->set_current_status_line_text_by_type(type, txt);

	//if (originalOwningScreen != currentScreen)
	//	guiglobs::apheresis_status_line->unparent_status_line(currentScreen);

	SimDelay::seconds(1);
}


void SimUtilities::sendTraverseMsg(TRAVERSE_TYPE type=UNIVERSAL, long param0=0, long param1=0, long param2=0, long param3=0,
								    long param4=0, long param5=0, long param6=0, long param7=0, long param8=0, long param9=0)
{
    TRAVERSE_INFO traverseInfo;
	traverseInfo.type = type;
	traverseInfo.parameters[0] = param0;
	traverseInfo.parameters[1] = param1;
	traverseInfo.parameters[2] = param2;
	traverseInfo.parameters[3] = param3;
	traverseInfo.parameters[4] = param4;
	traverseInfo.parameters[5] = param5;
	traverseInfo.parameters[6] = param6;
	traverseInfo.parameters[7] = param7;
	traverseInfo.parameters[8] = param8;
	traverseInfo.parameters[9] = param9;

	{
		SimMsgSys msgSys;
		SimntTraverseMsg msg(MessageBase::SEND_GLOBAL);
		msg.send(traverseInfo);
	}

	SimDelay::seconds(1);
}

void SimUtilities::initCFR11()
{
	static bool initialized = false;
	if( initialized )
		return;
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrTimeDelim);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrCommaDelim);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrTimeEventSeparator);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrActionDrawUp);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrActionDrawDown);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrActionReturnUp);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrActionReturnDown);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrActionAcReactionDown);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrActionClumpingUp);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrActionClumpingDown);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrDonInfoHeight);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrDonInfoWeight);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textDataCfrBloodType);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrDonInfoCrit);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrDonInfoHgb);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrDonInfoPltPrecount);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrDonInfoGender);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textDataCfrGenderMale);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textDataCfrGenderFemale);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscCfrActionNewProcedureSelected);

	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textDataUnitsCfrFeet);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textDataUnitsCfrCm);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textDataUnitsCfrHgb);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textDataUnitsCfrInches);

	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textDataUnitsCfrKg);
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textDataUnitsCfrLbs);

    string_screen_mapper::AddConcatMap(textEmptyFormatCfrTimeOfDay,textDataUnitsCfrAm);
    string_screen_mapper::AddConcatMap(textEmptyFormatCfrTimeOfDay,textDataUnitsCfrPm);


	addCFR11Events();
}

void SimUtilities::subWalkThroughCFR11Common()
{
	setStatLine(textStatEnterPin, DEFAULT_STATUS_LINE);
	SimScreenCapture::screenshot(SHOT_OPTS_NO_PRUNING | SHOT_OPTS_LBL_VISTA_ONLY, "cfr11IdEnterPin");

	setStatLine(textStatMissingBadgeBoop);
	SimScreenCapture::screenshot(SHOT_OPTS_NO_PRUNING | SHOT_OPTS_LBL_VISTA_ONLY, "cfr11MissingBadge");

	setStatLine(textStatNoItemSelected);
	SimScreenCapture::screenshot(SHOT_OPTS_NO_PRUNING | SHOT_OPTS_LBL_VISTA_ONLY, "cfr11NoItemSelected");

	setStatLine(textStatIdVerified);
	SimScreenCapture::screenshot(SHOT_OPTS_NO_PRUNING | SHOT_OPTS_LBL_VISTA_ONLY, "cfr11IdVerified");

	setStatLine(textStatWaitingForVista);
	SimScreenCapture::screenshot(SHOT_OPTS_NO_PRUNING | SHOT_OPTS_LBL_VISTA_ONLY, "cfr11Waiting4Vista");

	setStatLine(textStatIdNotVerified);
	SimScreenCapture::screenshot(SHOT_OPTS_NO_PRUNING | SHOT_OPTS_LBL_VISTA_ONLY, "cfr11IdNotVerified");

	guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

	setStatLine(textStatAuthEvents, DEFAULT_STATUS_LINE);
	SimScreenCapture::screenshot(SHOT_OPTS_NO_PRUNING | SHOT_OPTS_LBL_VISTA_ONLY, "cfr11IdAuthorizeEvents");

	guiglobs::apheresis_status_line->deactivate_status_line_type (DEFAULT_STATUS_LINE);
	setStatLine();
}

void SimUtilities::subWalkThroughCFR11Run()
{
	SimScreenCapture::screenshot(SHOT_OPTS_NO_PRUNING | SHOT_OPTS_LBL_VISTA_ONLY, "cfr11Event1");
	CFR_LIST::iterator iter=guiglobs::cfr_list.begin();
	iter++;

	for(int i = 2; iter != guiglobs::cfr_list.end(); ++iter, ++i)
	{
		SimScreenTouch::touch(GUI_BUTTON_CFR_SCROLL_UP, GUI_SCREEN_CFR11);
		SimScreenCapture::screenshot(SHOT_OPTS_NO_PRUNING | SHOT_OPTS_LBL_VISTA_ONLY, "cfr11Event%d", i);
	}
}

void SimUtilities::subWalkThroughCFR11PostRun()
{
	int listSize = guiglobs::cfr_list.size();
	unsigned int buttonIndex;
	int step, limit, blockNum;

	for(buttonIndex = 0, step = 4, limit = listSize-step, blockNum = 0;
		SimScreenCapture::screenshot(SHOT_OPTS_NO_PRUNING | SHOT_OPTS_LBL_VISTA_ONLY, "cfr11EventPostRun%d", blockNum), buttonIndex <= limit;
		buttonIndex += step, ++blockNum)
		for(unsigned int scrollDownCount = 0; scrollDownCount < step && scrollDownCount < (limit-blockNum*step); ++scrollDownCount)
			SimScreenTouch::touch(GUI_BUTTON_CFR_SCROLL_DN, GUI_SCREEN_CFR11);
}

void SimUtilities::walkThroughCFR11Screens()
{
	SimMsgSys msgSys;
	SimScreenCapture::autoNavOn(false);
	
	initCFR11();
	setSystemState(STARTUP_TESTS);

	SimAlarms::sendAlarm(CFR11_TRACKING_ALARM);
    SimScreenCapture::screenshot(GUI_SCREEN_CFR11, SHOT_OPTS_NO_PRUNING | SHOT_OPTS_LBL_VISTA_ONLY, "cfr11");
    subWalkThroughCFR11Common();
    subWalkThroughCFR11Run();
    setSystemState(POST_RUN);
    subWalkThroughCFR11PostRun();
    //SimUtilities::UnitSystem::toggle();
    //subWalkThroughCFR11Run();
    //SimUtilities::UnitSystem::toggle();
	SimScreenTouch::touch(GUI_BUTTON_CFR_SELECTED, GUI_SCREEN_CFR11);
	SimDelay::seconds(1);
}


void SimUtilities::addCFR11Events()
{
	if (_cfr11EventsAdded) return;

	_cfr11EventsAdded = true;
	SimMsgSys tmpMsgSys;
    
	_cfr11EventsAdded = true;
	
	GuiTextUtils guiTextUtils;
	TriggerDataType data;

	DataLog (log_level_sim_utility_info) << "Adding bio manager logging events." << endmsg;

	data.value = ACCESS_PRESSURE_UP;
	guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(ACCESS_PRESSURE_UP));
	data.value = ACCESS_PRESSURE_DOWN;
	guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(ACCESS_PRESSURE_DOWN));

	data.value = RETURN_PRESSURE_UP;
	guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(RETURN_PRESSURE_UP));
	data.value = RETURN_PRESSURE_DOWN;
	guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(RETURN_PRESSURE_DOWN));

	data.value = TINGLING_UP;
	guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(TINGLING_UP));
	data.value = TINGLING_DOWN;
	guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(TINGLING_DOWN));

	data.value = CLUMPING_UP;
	guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(CLUMPING_UP));
	data.value = CLUMPING_DOWN;
	guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(CLUMPING_DOWN));
	
	//Build basic donor vitals stuff.  This will remain unchanged for the most part.
	SDonorVitals vitals;
	guistring txtString;

	//guiglobs::gui_donor_data_repository.get_donor_vitals_struct(&vitals);

	vitals.cDonorDownloadStatus = DONOR_DOWNLOAD_OPERATOR_COMMIT;
	vitals.cDonorInfoStatus = DONOR_INFO_OPERATOR_COMFIRMATION;
	vitals.cDonorVitalsStatus = DONOR_VITAL_OPERATOR_COMFIRMATION;
	
	data.donor_vitals_a.height = 178;
	data.donor_vitals_a.weight = (int)84;

	vitals.DonorDataCRC = 0;
	vitals.fDonorSampleVolume = 100;
	vitals.fProcDonorTBV = 6579.0;
	vitals.fSafetyDonorTBV    = 6579.0;

	// Our second donor, Ms. "Jane Doe"
	sprintf(vitals.cDonorName, "Jane Doe");
	sprintf(vitals.cDonorDateofBirth, "19750123");
	vitals.cDonorSex = DONOR_SEX_IS_FEMALE;
	data.donor_vitals_a.gender = (int)2;
	vitals.fDonorHct = 44.0;
	data.donor_vitals_b.hematocrit    = (int)44;
	vitals.fDonorPltPrecount = 333;
	data.donor_vitals_b.plateletCount = (int)333;
	vitals.DonorBloodType = BT_O_POS;
	data.donor_vitals_b.bloodType     = (int)4;

	//guiglobs::gui_donor_data_repository.get_donor_vitals_struct(&vitals);

	vitals.fDonorHeight = 177;
	vitals.fDonorWeight = 83;

	guiglobs::bioMgr().BuildDonorVitalsAMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_A_CHANGE, data, txtString);

	guiglobs::bioMgr().BuildDonorVitalsBMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_B_CHANGE, data, txtString);
	
	// Our first donor, Mr. "John Doe"
	sprintf(vitals.cDonorName, "John Doe");
	sprintf(vitals.cDonorDateofBirth, "19631121");
	vitals.cDonorSex = DONOR_SEX_IS_MALE;
	data.donor_vitals_a.gender = (int)1;
	vitals.fDonorHct = 38.0;
	data.donor_vitals_b.hematocrit    = (int)38;

	vitals.fDonorPltPrecount = 350;
	data.donor_vitals_b.plateletCount = (int)350;
	vitals.DonorBloodType = BT_O_NEG;
	data.donor_vitals_b.bloodType     = (int)2;
	
	vitals.fDonorHeight = 179;
	vitals.fDonorWeight = 85;

	guiglobs::bioMgr().BuildDonorVitalsAMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_A_CHANGE, data, txtString);

	guiglobs::bioMgr().BuildDonorVitalsBMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_B_CHANGE, data, txtString);
	
	// Our third donor, Ms. "Joan Doe"
	sprintf(vitals.cDonorName, "Jane Doe");
	sprintf(vitals.cDonorDateofBirth, "19750123");
	vitals.cDonorSex = DONOR_SEX_IS_FEMALE;
	data.donor_vitals_a.gender = (int)2;
	vitals.fDonorHct = 44.0;
	data.donor_vitals_b.hematocrit    = (int)44;
	vitals.fDonorPltPrecount = 333;
	data.donor_vitals_b.plateletCount = (int)333;
	vitals.DonorBloodType = BT_O_POS;
	data.donor_vitals_b.bloodType     = (int)4;

	//use hemoglobin
	vitals.cDonorRbcCount = RBC_COUNT_HEMOGLOBIN;
	vitals.cDonorHeightUnits = HEIGHT_UNITS_CM;
	vitals.cDonorWeightUnits =  WEIGHT_UNITS_KG;

	vitals.fDonorHeight = 178;
	vitals.fDonorWeight = 84;
	
	guiglobs::bioMgr().BuildDonorVitalsAMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_A_CHANGE, data, txtString);

	guiglobs::bioMgr().BuildDonorVitalsBMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_B_CHANGE, data, txtString);
	
	// A- donor
	vitals.DonorBloodType = BT_A_NEG;
	data.donor_vitals_b.bloodType     = (int)32;
	
	guiglobs::bioMgr().BuildDonorVitalsBMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_B_CHANGE, data, txtString);
	
	// A+ donor (would do business again lol)
	vitals.DonorBloodType = BT_A_POS;
	data.donor_vitals_b.bloodType     = (int)64;
	
	guiglobs::bioMgr().BuildDonorVitalsBMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_B_CHANGE, data, txtString);
	
	// B- donor
	vitals.DonorBloodType = BT_B_NEG;
	data.donor_vitals_b.bloodType     = (int)128;
	
	guiglobs::bioMgr().BuildDonorVitalsBMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_B_CHANGE, data, txtString);
	
	// B+ donor
	vitals.DonorBloodType = BT_B_POS;
	data.donor_vitals_b.bloodType     = (int)256;
	
	guiglobs::bioMgr().BuildDonorVitalsBMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_B_CHANGE, data, txtString);
	
	// AB- donor
	vitals.DonorBloodType = BT_AB_NEG;
	data.donor_vitals_b.bloodType     = (int)8;
	
	guiglobs::bioMgr().BuildDonorVitalsBMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_B_CHANGE, data, txtString);
	
	// AB+ donor
	vitals.DonorBloodType = BT_AB_POS;
	data.donor_vitals_b.bloodType     = (int)16;
	
	guiglobs::bioMgr().BuildDonorVitalsBMsg(vitals, txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_DONOR_VITALS_B_CHANGE, data, txtString);

	data.value = DOOR_LATCH_FAILURE;
	Display_Text doorLatchFailure(textStatAlarmCentDoorUnlatched);
	doorLatchFailure.get_text_direct(txtString);
	guiglobs::bioMgr().LogEvent(TRIGGER_ALARM, data, txtString);

	Display_Text new_procedure(textMiscCfrActionNewProcedureSelected);
	new_procedure.get_text_direct(txtString);
	data.value = 1;
	guiglobs::bioMgr().LogEvent(TRIGGER_PROCEDURE_SELECTED, data, txtString);
}

void SimUtilities::walkThroughReplacementFluidAlarm()
{
	SimScreenCapture::autoNavOn(false);
	setSystemState(STARTUP_TESTS);
	
	SimAlarms::sendAlarm(REPLACEMENT_FLUID_ALARM);
	SimScreenCapture::screenshot("repFluidAlarm");

	SimScreenTouch::touch(GUI_BUTTON_REPFLUID_CONTINUE, GUI_SCREEN_REPFLUID);
	SimDelay::seconds(1);
	SimScreenCapture::screenshot("repFluidAlarmConfirm");
	
	SimScreenTouch::touch(GUI_BUTTON_CONFIRM_BOX_CANCEL, GUI_SCREEN_SALINE_CONNECT_CONFIRM_BOX);
    SimScreenTouch::touch(GUI_BUTTON_REPFLUID_CANCEL, GUI_SCREEN_REPFLUID);
}

void SimUtilities::walkThroughDrbcSplitScreen()
{
	SimMsgSys msgSys;

    SimScreenCapture::autoNavOn(false);
	
	setSystemState(STARTUP_TESTS);
	
	SimAlarms::sendAlarm(DRBC_SPLIT_ALERT);

	sendTraverseMsg(UNIVERSAL, DRBC_SPLIT_COLLECT_FILTERED);
	SimScreenCapture::screenshot("drbcSplitCollectFiltered");

	sendTraverseMsg(UNIVERSAL, DRBC_SPLIT_COLLECT_NONFILTERED);
	SimScreenCapture::screenshot("drbcSplitCollectUnfiltered");

	sendTraverseMsg(UNIVERSAL, DRBC_SPLIT_RAS_ADDITION);
	SimScreenCapture::screenshot("drbcSplitRasAddition");

	SimScreenTouch::touch(GUI_BUTTON_DRBC_SPLIT_CONTINUE, GUI_SCREEN_DRBC_SPLIT);
}


void SimUtilities::walkThroughBarcodeScreens()
{
	string_screen_mapper::AddConcatMap(textMiscCfrActionAcReactionUp,textMiscBarcodeMoreCategoriesElipses);
	
	SimMsgSys tmpMsgSys;							// Start the message system for this thread

    SimScreenCapture::autoNavOn(false);
    setSystemState(STARTUP_TESTS);

	SimAlarms::sendAlarm(BARCODE_INFO_ALARM);
	SimScreenCapture::screenshot(GUI_SCREEN_BARCODE, SHOT_OPTS_LBL_VISTA_ONLY, "barcode1");
	
	SimScreenTouch::touch(GUI_BUTTON_BARCODE_RANK_SWITCH, GUI_SCREEN_BARCODE);
	SimScreenCapture::screenshot(GUI_SCREEN_BARCODE, SHOT_OPTS_LBL_VISTA_ONLY, "barcode2");

	setStatLine(textStatBarcodeScanOverrun);
	SimScreenCapture::screenshot(GUI_SCREEN_BARCODE, SHOT_OPTS_LBL_VISTA_ONLY, "barcodeScanOverrun");

	setStatLine(textStatBarcodeDeletionDisallowed);
	SimScreenCapture::screenshot(GUI_SCREEN_BARCODE, SHOT_OPTS_LBL_VISTA_ONLY, "barcodeDeletionDisallowed");

	setStatLine(textStatBarcodeBadScan);
	SimScreenCapture::screenshot(GUI_SCREEN_BARCODE, SHOT_OPTS_LBL_VISTA_ONLY, "barcodeBadScan");

	setStatLine(textStatBarcodeSegDupe);
	SimScreenCapture::screenshot(GUI_SCREEN_BARCODE, SHOT_OPTS_LBL_VISTA_ONLY, "barcodeSegmentDupe");

	setStatLine(textStatBarcode2ManyScans);
	SimScreenCapture::screenshot(GUI_SCREEN_BARCODE, SHOT_OPTS_LBL_VISTA_ONLY, "barcode2ManyScans");

	setStatLine(textStatBarcodeMoveInput);
	SimScreenCapture::screenshot(GUI_SCREEN_BARCODE, SHOT_OPTS_LBL_VISTA_ONLY, "barcodeIdentify");

	setStatLine();
	SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_BARCODE);
}

void SimUtilities::walkThroughSplashScreen()
{
    SimScreenCapture::autoNavOn(false);
    setSystemState(SELF_TEST);
	SimScreenCapture::screenshot("splashScreen");
}

void SimUtilities::setScreenshotName(const char* info)
{
	if (info && guiglobs::scrnComm) guiglobs::scrnComm->setScreenshotName(info);
	else DataLog (log_level_sim_utility_error) << "ERROR: Unable to set string due to NULL pointer." << endmsg;
}

void SimUtilities::removeScreenshotName()
{
	if (guiglobs::scrnComm) guiglobs::scrnComm->removeScreenshotName();
	else DataLog (log_level_sim_utility_error) << "ERROR: Unable to set string due to NULL pointer." << endmsg;
}

void SimUtilities::setScreenshotInfo(const char* name)
{
	if (name && guiglobs::scrnComm) guiglobs::scrnComm->setScreenshotInfo(name);
	else DataLog (log_level_sim_utility_error) << "ERROR: Unable to set string due to NULL pointer." << endmsg;
}

void SimUtilities::removeScreenshotInfo()
{
	if (guiglobs::scrnComm) guiglobs::scrnComm->removeScreenshotInfo();
	else DataLog (log_level_sim_utility_error) << "ERROR: Unable to set string due to NULL pointer." << endmsg;
}


void SimUtilities::showAdjustScreen()
{
    guistring allocStr; //newly constructed alarm data buffer

    // initialize the allocation buffer
    allocStr.initialize();

	DataLog (log_level_sim_utility_info) << "Changing to adjust screen" << endmsg;

	changeScreens(GUI_SCREEN_TROUBLE, allocStr, DISPLAY_AS_STATELESS);
}

void SimUtilities::sendAdjustData(bool isPltRun)
{
	SimMsgSys tmpMsgSys;							// Start the message system for this thread
	
	ProcAdjust_CDS * adjustcds = new ProcAdjust_CDS(ROLE_RW);
	ProcedureAdjustmentStatusMsg _StatusMsg( MessageBase::SEND_LOCAL );

	adjustcds->Draw.Maximum				= 142.0;
	adjustcds->Draw.Minimum				= 0.0;
	adjustcds->Draw.CurrentCap			= 71.0;
	adjustcds->Draw.CurrentValue		= 71.0;
	adjustcds->Draw.Active				= true;

	adjustcds->Return.Maximum			= 400.0;
	adjustcds->Return.Minimum			= 0.0;
	adjustcds->Return.CurrentCap		= 200.0;
	adjustcds->Return.CurrentValue		= 200.0;
	adjustcds->Return.Active			= true;

	adjustcds->Tingling.Maximum			= 1.2;
	adjustcds->Tingling.Minimum			= 0.6;
	adjustcds->Tingling.CurrentCap		= 0.9;
	adjustcds->Tingling.CurrentValue	= 0.9;
	adjustcds->Tingling.Active			= true;

	adjustcds->Clumping.Maximum			= 13.7;
	adjustcds->Clumping.Minimum			= 6.0;
	adjustcds->Clumping.CurrentCap		= 9.85;
	adjustcds->Clumping.CurrentValue	= 9.85;
	
	if(isPltRun)
	{
		adjustcds->Spillover				= BUTTON_VISIBLE;
		adjustcds->AirBlock					= BUTTON_VISIBLE;
		adjustcds->SalineBolus				= BUTTON_INVISIBLE;
		adjustcds->Clumping.Active			= true;
	}
	else
	{
		adjustcds->Spillover				= BUTTON_INVISIBLE;
		adjustcds->AirBlock					= BUTTON_VISIBLE;
		adjustcds->SalineBolus				= BUTTON_VISIBLE;
		adjustcds->Clumping.Active			= false;
	}

	DataLog(log_level_sim_utility_info) << "Sending ProcedureAdjustmentStatusMsg message." << endmsg;
	_StatusMsg.send(1);

	SimDelay::seconds(1);
}














void SimUtilities::cycleThroughScreenTypes(GUI_SCREEN_ID correctScreen, int lastValue, SCREENSHOTINFO screenshotNames[], bool doMappingStuff=true, TRAVERSE_TYPE traverseType = UNIVERSAL, int delaySeconds=1)
{
	if (guiglobs::screen_status.get_current_displayed_screen() != correctScreen)
	{
		DataLog (log_level_sim_main_routine_error) << "You're not on the right screen to call this function (current="
			<< guiglobs::screen_status.get_current_displayed_screen() << "; expected="
			<< correctScreen << ")" << endmsg;
		return;
	}

	for (int index = 0; index <= lastValue; index++)
	{
		if (screenshotNames[index].id != index)
		{
			DataLog (log_level_sim_main_routine_error) << "Something's fishy about screenshot "
				<< screenshotNames[index].name << "'s info. Index("
				<< index << ") != Id(" << screenshotNames[index].id << ")" << endmsg;
		}

		sendTraverseMsg(traverseType, index);
		SimDelay::seconds(delaySeconds);

		SimScreenCapture::screenshot(correctScreen, screenshotNames[index].options, screenshotNames[index].name);
		SimDelay::seconds(delaySeconds);
	}
	
	guiglobs::apheresis_status_line->reset_status_line();
}

void SimUtilities::cycleThroughConfirmDonInfoDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_CONFIRM_DON_INFO,
                            LAST_CONFIRM_DON_INFO_DISPLAY_TYPE,
							CONFIRM_DON_INFO_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughDrbcSplitDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_DRBC_SPLIT,
                            LAST_DRBC_SPLIT_DISPLAY_TYPE,
							DRBC_SPLIT_SCREENSHOT_INFO,
							doMappingStuff);
}


void SimUtilities::cycleThroughAasRunsumDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_AAS_RUN_SUMMARY,
                            LAST_AAS_RUNSUM_VALUE,
							AAS_RUNSUM_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughRasConnectDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_AAS_CONNECT_RAS,
                            LAST_RAS_CONNECT_DISPLAY_TYPE,
							RAS_CONNECT_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughPasConnectDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_AAS_CONNECT_PAS,
                            LAST_PAS_CONNECT_DISPLAY_TYPE,
							PAS_CONNECT_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughChannelClampDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_AAS_CLAMP_CHANNEL,
                            LAST_CHANNEL_CLAMP_DISPLAY_TYPE,
							CHANNEL_CLAMP_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughAbortConfirmDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_ABORTCNF,
                            LAST_ABORT_DISPLAY_TYPE,
							ABORT_TYPE_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughMssEndDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_MSS_END,
                            LAST_MSS_END_DISPLAY_TYPE,
							MSS_END_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughRs1DisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_RUNSUMM1,
                            LAST_RS1_DISPLAY_TYPE,
							RS1_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughRs2DisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_RUNSUMM2,
                            LAST_RS2_DISPLAY_TYPE,
							RS2_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughPtfLoadTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_PTF_MOUNT,
                            LAST_PTF_LOAD_DISPLAY_TYPE,
							PTF_LOAD_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughSysDispTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_SYSDISP,
                            LAST_SYSDISP_DISPLAY_TYPE,
							SYSDISP_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughSysClampDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_SYSCLAMP,
                            LAST_SYSCLAMP_DISPLAY_TYPE,
							SYSCLAMP_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughJpClampDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG,
                            0,
							JPSYSCLAMP_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughPasInitSplitTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_PAS_INIT_SPLIT,
                            LAST_PAS_INIT_DISPLAY_TYPE,
							PAS_INIT_SPLIT_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughDonConnTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_DONCONN,
                            LAST_DON_CONN_DISPLAY_TYPE,
							DON_CONN_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughPowerDownDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_POWERDOWN,
                            LAST_POWERDOWN_DISPLAY_TYPE,
							POWERDOWN_SCREENSHOT_INFO,
							doMappingStuff);
}

void SimUtilities::cycleThroughSysDonorDisplayTypes(bool doMappingStuff=true)
{
	cycleThroughScreenTypes(GUI_SCREEN_SYSDONOR,
							LAST_SYSDONOR_DISPLAY_TYPE,
							SYSDONOR_SCREENSHOT_INFO,
							doMappingStuff,
							SYSDONOR_UNIVERSAL);
}

// This one is kind of specific, so let's leave it here (as opposed to putting it in the base class).
void SimUtilities::cycleThroughRunsumLabels(const char * screenshotPrefix)
{
	sendTraverseMsg(CHANGE_LABELS, DONT_LABEL_PRODUCT);
	SimScreenCapture::screenshot("%sDontLabel", screenshotPrefix);

	sendTraverseMsg(CHANGE_LABELS, LABEL_PRODUCT);
	SimScreenCapture::screenshot(SHOT_OPTS_LBL_INCORRECT, "%sLabelProduct", screenshotPrefix);

	//sendTraverseMsg(CHANGE_LABELS, DONT_SHOW_LABEL_BOX);
	//SimScreenCapture::screenshot("%NoLabelBox", screenshotPrefix);
}

void SimUtilities::walkThroughAbortScreens()
{
	SimMsgSys tmpMsgSys;
	SimScreenCapture::autoNavOn(false);
	setSystemState(STARTUP_TESTS);
	changeScreens(GUI_SCREEN_ABORTCNF, ABORT_TYPE_USER, DISPLAY_AS_STATELESS);
    cycleThroughAbortConfirmDisplayTypes();	
}

void SimUtilities::walkThroughPowerDownScreens()
{
	SimMsgSys tmpMsgSys;
	setSystemState(POST_RUN);
	changeScreens(GUI_SCREEN_AAS_RUN_SUMMARY, "Run summary");
	changeScreens(GUI_SCREEN_RUNSUMM1, "Run summary");
	changeScreens(GUI_SCREEN_RUNSUM_PLT, "Run summary");
	changeScreens(GUI_SCREEN_RUNSUM_PLASMA, "Run summary");
	changeScreens(GUI_SCREEN_RUNSUMM2, "Run summary");
	changeScreens(GUI_SCREEN_REBOOT_CONFIRM_BOX, "Run summary", DISPLAY_AS_CHILD);
	changeScreens(GUI_SCREEN_POWERDOWN, "Run summary");
	cycleThroughPowerDownDisplayTypes();
}



#endif /* CPU == SIMNT */



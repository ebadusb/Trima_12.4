/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simResponder.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 *
 *******************************************************************************/


#include "trima_datalog.h"
#include "runinfo.h"
#include "aphwind.hpp"
#include "drv/timer/timerDev.h"
#include "guiglobs.hpp"
#include "simResponder.hpp"
#include "guiglobs.hpp"
#include "simDelay.hpp"
#include "simUtilities.hpp"
#include "chw.hpp"
#include "procdonor_cds.h"
#include "procadjust_cds.h"
#include "simScreenTouch.hpp"

/*********************************************************************************************
									Constructor
**********************************************************************************************/

SimResponder::SimResponder()
{
	DataLog (log_level_sim_responder_info) << "Entering responder constructor. Calling responder init." << endmsg;
    init();
	DataLog (log_level_sim_responder_info) << "Responder init completed. Leaving responder constructor." << endmsg;
}

/*********************************************************************************************
									Utility Functions
**********************************************************************************************/

#define NEW_MSG_OBJ( __MSG_TYPE__, __CALLBACK_FUNCTION__ ) \
	new __MSG_TYPE__ (Callback<SimResponder> ( this, &SimResponder::##__CALLBACK_FUNCTION__	))

void SimResponder::init()
{
	DataLog (log_level_sim_responder_info) << "Creating new proc run CDS (writeable)." << endmsg;
	_ProcRunCDS = new ProcRun_CDS(ROLE_RW);
	DataLog (log_level_sim_responder_info) << "Creating of proc run CDS complete.  Writing some stuff GUI might need." << endmsg;

	// Make sure everybody thinks it's OK for us to meter.
	_ProcRunCDS->MeterRbcs_OK.Set(true);
	_ProcRunCDS->MeterPlatelets_OK.Set(true);

	DataLog (log_level_sim_responder_info) << "Proc run CDS stuff written." << endmsg;

	// Intialize message recievers
	_msgPtrDispLoaded =				NEW_MSG_OBJ(ConfirmDisposableLoadedMsg,	gotConfirmDisposableLoadedMsg	);
	_msgPtrClamped4Testing =		NEW_MSG_OBJ(ClampedForTestingMsg,		gotClamped4TestingMsg			);
	_msgPtrAcConnected =			NEW_MSG_OBJ(ACConnectedMsg,				gotAcConnectedMsg				);
	_msgPtrBeginRun =				NEW_MSG_OBJ(GUIRequestBeginRunMsg,		gotBeginRunMsg					);
	_msgPtrDonorConnected =			NEW_MSG_OBJ(DonorConnectedMsg,			gotDonorConnectedMsg			);
	_msgPtrRunStatsRequested =		NEW_MSG_OBJ(RequestEndRunStatsMsg,		gotRequestRunStatsMsg			);
	_msgPtrDonorDisco =				NEW_MSG_OBJ(DonorDisconnectedMsg,		gotDonorDisconnectedMsg			);
	_msgPtrClampChannelComplete =	NEW_MSG_OBJ(MSSSetupMsg,				gotClampChannelCompleteMsg		);
	_msgPtrAasConnectionsComplete =	NEW_MSG_OBJ(MSSConnectedMsg,			gotAasConnectionsCompleteMsg	);
	_msgPtrCrossoverClampComplete =	NEW_MSG_OBJ(CrossoverClampedMsg,		finishSysTest					);
	_msgPtrCassetteFuncBits		  = NEW_MSG_OBJ(Message<CASSETTE_MESSAGE>,	recieveCassetteFuncBits			);
	_msgPtrDonVitalsFromProc =		NEW_MSG_OBJ(DVitalsFromProc, 			gotDonVitalsFromProc			);
	_msgPtrSafetyAlive =			NEW_MSG_OBJ(QuerySafetyAliveMsg, 		gotSafetyAliveRequest			);
	_msgPtrDonVitalsFromGui =		NEW_MSG_OBJ(DVitalsFromGUI, 			gotDonVitalsFromGui				);
    _msgPtrRemainingPasVolRequested = NEW_MSG_OBJ(Gui2ProcRequestRemainingPasVolMsg, gotRemainingPasVolRequestedMsg);

	SimScreenTouch::init();
}

void SimResponder::sendAasData(int aasLevel = 27)
{
	AasAdditionVolumeUpdate updateMsg(MessageBase::SEND_LOCAL);
	updateMsg.send(aasLevel);
}

char* SimResponder::getSuffix(CONFIG_DATA_LISTBOX_STRINGS lang)
{
	switch (lang)
	{
	case ENGLISH_LANG:		return "_EN";
	case FRENCH_LANG:		return "_FR";
	case GERMAN_LANG:		return "_GE";
	case SPANISH_LANG:		return "_SP";
	case ITALIAN_LANG:		return "_IT";
	case JAPAN_LANG:		return "_JA";
	case DUTCH_LANG:		return "_DU";
	case PORTUGUESE_LANG:	return "_PO";
	case SWEDISH_LANG:		return "_SW";
	case KOREAN_LANG:		return "_KO";
	case RUSSIAN_LANG:		return "_RU";
	case CZECH_LANG:		return "_CZ";
	case POLISH_LANG:		return "_PL";
	case CHINESE_LANG:		return "_CN";
	default:			return "";
	}
}

/*********************************************************************************************
									Message Handlers
**********************************************************************************************/

void SimResponder::gotDonVitalsFromProc()
{
	DataLog (log_level_sim_responder_info) << "Fudging safety TBV" << endmsg;
	
	SDonorVitals data = _msgPtrDonVitalsFromProc->getData();
	data.fSafetyDonorTBV = data.fProcDonorTBV;
	
	DVitalsFromSafety response(MessageBase::SEND_LOCAL);
	response.send(data);
}

void SimResponder::gotSafetyAliveRequest()
{
	AnswerSafetyAliveMsg safetyalive(MessageBase::SEND_LOCAL);
	safetyalive.send(1);
}

void SimResponder::gotDonVitalsFromGui()
{
	SDonorVitals data = _msgPtrDonVitalsFromGui->getData();

	DataLog (log_level_sim_responder_info) << "Responder got GUI vitals: " << data << endmsg;

	// Make sure the units make sense.
	data.cDonorHeightUnits = guiglobs::cdsData.config.LangUnit.Get().key_height;
	data.cDonorWeightUnits = guiglobs::cdsData.config.LangUnit.Get().key_weight;
	data.cDonorRbcCount    = guiglobs::cdsData.config.LangUnit.Get().key_crit_or_glob;
    
	//
	//  Really the control TBV eqn
	//
	CDonor donor(data);
	data.fSafetyDonorTBV = donor.CalTBV();
	
	DVitalsFromSafety response(MessageBase::SEND_LOCAL);
	response.send(data);
	
	//
	//
	//   Save data in the CDS
	//
	ProcDonor_CDS m_donorcds(ROLE_RW);
	m_donorcds.Female.Set(donor.Female());
	m_donorcds.Height.Set(donor.Height());
	m_donorcds.Weight.Set(donor.Weight());
	m_donorcds.TBV.Set(donor.TBV());
	m_donorcds.TBVOgawa.Set(donor.TBV());
	m_donorcds.PreCnt.Set(donor.PreCnt());
	m_donorcds.Hct.Set(donor.Hct()/100.0f);
	m_donorcds.BloodType.Set(donor.BloodType());
	m_donorcds.SampleVolume.Set(donor.SampleVolume());
	
	/*
	if (data.cDonorInfoStatus == DONOR_INFO_OPERATOR_COMFIRMATION)
	{
		DoPredictionMsg req(MessageBase::SEND_LOCAL);
		req.send((int)DONOR_INFO_PRECONNECT);
	}
	*/
	
	DoPrediction_t request = DONOR_INFO_CHANGED;
	
	DoPredictionPayload pred_payload(request, 1);
	DoPredictionMsg _doPredictionMsg(MessageBase::SEND_LOCAL);
	_doPredictionMsg.send(pred_payload);
}

void SimResponder::gotConfirmDisposableLoadedMsg()
{
	DataLog (log_level_sim_responder_info) << "Message responder recieved confirm disposable loaded message. Sending disposable lowered message." << endmsg;
	DisposableLoweredMsg msg(MessageBase::SEND_LOCAL);
	msg.send();
}

void SimResponder::gotClamped4TestingMsg()
{
	int runType = SimUtilities::getAasRunType();

	DataLog (log_level_sim_responder_info) << "Message responder recieved clamped for testing message. Transitioning to DISPOSABLE_TEST state." << endmsg;
	SimUtilities::setSystemState(DISPOSABLE_TEST);

	switch (runType)
	{
	// If RAS is on...
	case MSS_RBC:
	case MSS_BOTH:
		{
			SimDelay::seconds(10);

			DataLog (log_level_sim_responder_info) << "Message responder sending close crossover message." << endmsg;
			CloseCrossoverClampMsg msg(MessageBase::SEND_LOCAL);
			msg.send();
		}
		break;

	// If RAS is off...
	default:
		{
			finishSysTest();
		}
	}
}

void SimResponder::finishSysTest()
{
	SimDelay::seconds(10);

	// Send test completed message
	DataLog (log_level_sim_responder_info) << "Message responder sending test completed message." << endmsg;
	TestCompletedMsg msg(MessageBase::SEND_LOCAL);
	msg.send(1);

	SimDelay::seconds(5);

	// Pause for Ten Seconds
	DataLog (log_level_sim_responder_info) << "Message responder transitioning to AC_CONNECTED state." << endmsg;
	SimUtilities::setSystemState(AC_CONNECTED);
}

void SimResponder::gotAcConnectedMsg()
{
	DataLog (log_level_sim_responder_info) << "Message responder recieved AC connected message. Transition to AC_PRIME state" << endmsg;   
	SimUtilities::setSystemState(AC_PRIME);

	// Pause for Ten Seconds
	SimDelay::seconds(10);

	DataLog (log_level_sim_responder_info) << "Message responder sending AC prime completed message." << endmsg;   
	ACPrimeCompletedMsg msg(MessageBase::SEND_LOCAL);
	msg.send(1);
}

void SimResponder::gotBeginRunMsg()
{
	DataLog (log_level_sim_responder_info) << "Message responder recieved begin run message. Transition to DONOR_CONNECTED state" << endmsg;   
	SimUtilities::setSystemState(DONOR_CONNECTED);
}

void SimResponder::gotDonorConnectedMsg()
{
	DataLog (log_level_sim_responder_info) << "Message responder recieved donor connected message. Transition to BLOOD_PRIME state." << endmsg;   
	SimUtilities::setSystemState(BLOOD_PRIME);
}

void SimResponder::gotRequestRunStatsMsg()
{
	DataLog (log_level_sim_responder_info) << "Message responder recieved run stats request message. Sending the stats." << endmsg;   
	SimUtilities::sendRunStats();
}

void SimResponder::gotDonorDisconnectedMsg()
{	
	DataLog (log_level_sim_responder_info) << "Message responder recieved donor disconnected message. Transitioning to METERED_STORAGE state." << endmsg;
	SimUtilities::setSystemState(METERED_STORAGE);
	
	DataLog (log_level_sim_responder_info) << "Message responder donor disconnect notification.  Tell the machine the AC Closure is verified." << endmsg;
	VerifyAcClosureCompleteMsg acClosureVerified(MessageBase::SEND_LOCAL);
	acClosureVerified.send();
}

void SimResponder::gotClampChannelCompleteMsg()
{
	DataLog (log_level_sim_responder_info) << "Message responder recieved clamp channel complete message. Waiting 10 seconds..." << endmsg;

	// Pause for Ten Seconds
	SimDelay::seconds(10);

	DataLog (log_level_sim_responder_info) << "Message responder sending the setup complete message." << endmsg;
    
	MSSSetupCompleteMsg msg(MessageBase::SEND_LOCAL);
	msg.send();
}

void SimResponder::gotAasConnectionsCompleteMsg()
{
   DataLog (log_level_sim_responder_info) << "Responder sees that AAS connections are complete." << endmsg;

	SimDelay::seconds(10);
	DataLog (log_level_sim_responder_info) << "Message responder recieved aas connections complete message. Sending addition message" << endmsg;

    MSSConnectedMsg msg(MessageBase::SEND_LOCAL);
	msg.send(0);
}

void SimResponder::recieveCassetteFuncBits()
{
	DataLog (log_level_sim_responder_info) << "Message responder recieved new function bits." << endmsg;
	_ProcRunCDS->CassetteFunctionBits.Set(((CASSETTE_MESSAGE)_msgPtrCassetteFuncBits->getData()).functBits);
	DataLog (log_level_sim_responder_info) << "Function bits " << _ProcRunCDS->CassetteFunctionBits.Get() << " written." << endmsg;
}

void SimResponder::gotRemainingPasVolRequestedMsg()
{
	#define PAS_VOL_REMAINING_RESPONSE 43
	DataLog (log_level_sim_responder_info) << "Message responder recieved pas vol remaining request. Responding with " << PAS_VOL_REMAINING_RESPONSE << endmsg;
	Proc2GuiSendRemainingPasVolDataMsg responseMsg(MessageBase::SEND_LOCAL);
	responseMsg.send(PAS_VOL_REMAINING_RESPONSE);
}

int SimResponder::responderLoop()
{
	DataLog (log_level_sim_responder_debug) << "Starting message responder loop." << endmsg;

	SimResponder responderObj;

    while (true)
		if (responderObj.dispatchMessages() == 0)
			nanosleep(&SimUtilities::delay,0);

	DataLog (log_level_sim_responder_debug) << "Leaving message responder loop." << endmsg;

	return 0;
}




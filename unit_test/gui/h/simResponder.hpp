/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simResponder.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: The sim responder listens for incoming messages from the GUI
 *	and responds to keep things moving along.  This will work its magic
 *	regardless of whether or not screenshots are being taken.
 *
 ******************************************************************************/

#ifndef __SIM_MSG_RESPONDER_HPP__
#define __SIM_MSG_RESPONDER_HPP__


#include "trimamessages.h"
#include "states.h"
#include "guistring.h"
#include "gui.h"
#include "cfg_ids.h"
#include "runinfo.h"
#include "simMsgSys.hpp"
#include "procrun_cds.h"
#include "guiUnitTestCds.h"
#include "guiproc.h"
#include "safproc.h"
#include "trimamessages.h"

class SimResponder: public SimMsgSys
{
private:
	ProcRun_CDS          *_ProcRunCDS;

	// The messages it's waiting for
	ConfirmDisposableLoadedMsg	*_msgPtrDispLoaded;
	ClampedForTestingMsg		*_msgPtrClamped4Testing;
	ACConnectedMsg				*_msgPtrAcConnected;
	GUIRequestBeginRunMsg		*_msgPtrBeginRun;
	DonorConnectedMsg			*_msgPtrDonorConnected;
	RequestEndRunStatsMsg		*_msgPtrRunStatsRequested;
	DonorDisconnectedMsg		*_msgPtrDonorDisco;
	MSSSetupMsg					*_msgPtrClampChannelComplete;
	MSSConnectedMsg				*_msgPtrAasConnectionsComplete;
	CrossoverClampedMsg			*_msgPtrCrossoverClampComplete;
	DVitalsFromProc 			*_msgPtrDonVitalsFromProc;
	QuerySafetyAliveMsg			*_msgPtrSafetyAlive;
	DVitalsFromGUI				*_msgPtrDonVitalsFromGui;

	void gotDonVitalsFromProc();
	void gotDonVitalsFromGui();
	void gotSafetyAliveRequest();

	Message<CASSETTE_MESSAGE>	*_msgPtrCassetteFuncBits;
	Gui2ProcRequestRemainingPasVolMsg *_msgPtrRemainingPasVolRequested;

	// Utility Functions
    void init();
    void sendAasData(int aasLevel);
	static char* getSuffix(CONFIG_DATA_LISTBOX_STRINGS lang);
    
	// Callback Functions
	void gotConfirmDisposableLoadedMsg();
	void gotClamped4TestingMsg();
	void gotAcConnectedMsg();
	void gotBeginRunMsg();
	void gotDonorConnectedMsg();
	void gotRequestRunStatsMsg();
	void gotDonorDisconnectedMsg();
	void gotClampChannelCompleteMsg();
	void gotAasConnectionsCompleteMsg();
	void gotReady4AasAdditionMsg();
	void finishSysTest();
	void recieveCassetteFuncBits();
	void gotRemainingPasVolRequestedMsg();

	static int responderLoop();

public:
	SimResponder();
};


#endif // #ifndef __SIM_MSG_RESPONDER_HPP__


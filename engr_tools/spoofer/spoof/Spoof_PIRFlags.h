/*****************************************************************************\
**
** Spoof_PIRFlags.h
**
** Spoof for testing PIR flags.
**
\*****************************************************************************/

#ifndef SPOOF_PIRFLAGS_H
#define SPOOF_PIRFLAGS_H

/* Includes ******************************************************************/

#include "recoverytypes.h"
#include "timermessage.h"

/* Constants *****************************************************************/

static const int kBaselineValues1[] = { 
	625, 725, 625, 725, 625,
	725, 625, 725, 625, 725,
	625, 725, 625, 725, 625,
	725, 625, 725, 625, 725
};
static const int kBaselineValues2[] = { 
	425, 525, 425, 525, 425,
	525, 425, 525, 425, 525,
	425, 525, 425, 525, 425,
	525, 425, 525, 425, 525
};
static const int kBaselineValues3[] = { 
	590, 810, 590, 810, 590,
	810, 590, 810, 590, 810,
	590, 810, 590, 810, 590,
	810, 590, 810, 590, 810
};
static const int kTestCaseValues1[] = {
	1300, 1301, 1302, 1303, 1304,
	1305, 1306, 1307, 1308, 1309,
	1310, 1311, 1312, 1313, 1314,
	1315, 1316, 1317, 1318, 1319
};
static const int kTestCaseValues2[] = {
	1300, 1301, 1302, 1303, 1304,
	1305, 1306, 1307, 1308, 1309,
	1310, 1311, 1312, 1313, 800,
	801, 802, 803, 804, 805
};
static const int kTestCaseValues3[] = {
	1300, 1301, 1302, 1303, 1304,
	1305, 1306, 1307, 800, 801,
	802, 803, 804, 805, 500,
	1400, 501, 1401, 502, 1402
};
static const int kTestCaseValues4[] = {
	1300, 1301, 1302, 1303, 1304,
	1305, 1306, 1307, 1308, 1309,
	1310, 1311, 500, 1400, 501,
	1401, 502, 1402, 503, 1403
};

static const int kCount = 20;

/* Classes *******************************************************************/

class Spoof_PIRFlagsBase : public SpoofBase {
public:
    Spoof_PIRFlagsBase(void);
	virtual ~Spoof_PIRFlagsBase(void);

    virtual void run();
    virtual void init();
    virtual void help();

protected:
	void TimerMessageNotify(void);
	void RecoveryStatusNotify(void);

	TimerMessage *mTimerMessageP;
	Message<RecoveryStatus> *mRecoveryStatusP;
	RecoveryTypes::Type mRecoverySubstate;
	RecoveryTypes::InRecovery mRecoveryStatus;
	int mBaselineRedPL;
	int mBaselineValues[kCount];
	int mTestCaseValues[kCount];
	int mValueIndex;
};

class Spoof_PIRFlagsCase1 : public Spoof_PIRFlagsBase {
public:
	Spoof_PIRFlagsCase1(void);
	virtual ~Spoof_PIRFlagsCase1(void) {}
};

class Spoof_PIRFlagsCase2 : public Spoof_PIRFlagsBase {
public:
	Spoof_PIRFlagsCase2(void);
	virtual ~Spoof_PIRFlagsCase2(void) {}
};

class Spoof_PIRFlagsCase3 : public Spoof_PIRFlagsBase {
public:
	Spoof_PIRFlagsCase3(void);
	virtual ~Spoof_PIRFlagsCase3(void) {}
};

class Spoof_PIRFlagsCase4 : public Spoof_PIRFlagsBase {
public:
	Spoof_PIRFlagsCase4(void);
	virtual ~Spoof_PIRFlagsCase4(void) {}
};

class Spoof_PIRFlagsCase5 : public Spoof_PIRFlagsBase {
public:
	Spoof_PIRFlagsCase5(void);
	virtual ~Spoof_PIRFlagsCase5(void) {}
};

class Spoof_PIRFlagsCase6 : public Spoof_PIRFlagsBase {
public:
	Spoof_PIRFlagsCase6(void);
	virtual ~Spoof_PIRFlagsCase6(void) {}
};

/* Functions *****************************************************************/

Spoof_PIRFlagsBase::Spoof_PIRFlagsBase(void) 
	: mTimerMessageP(NULL),
	mRecoveryStatusP(NULL), 
    mRecoverySubstate(RecoveryTypes::None),
	mRecoveryStatus(RecoveryTypes::NoRecovery),
	mBaselineRedPL(0),
	mValueIndex(0)
{
	mTimerMessageP = new TimerMessage(500, Callback<Spoof_PIRFlagsBase>(
			this, &Spoof_PIRFlagsBase::TimerMessageNotify), TimerMessage::DISARMED);
	mRecoveryStatusP = new Message<RecoveryStatus>(Callback<Spoof_PIRFlagsBase>(
			this, &Spoof_PIRFlagsBase::RecoveryStatusNotify), MessageBase::SNDRCV_RECEIVE_ONLY);
}

Spoof_PIRFlagsBase::~Spoof_PIRFlagsBase(void)
{
	mTimerMessageP->armTimer(TimerMessage::DISARMED);
}

void Spoof_PIRFlagsBase::run()
{
	LOG_INFO( "Spoofing baseline values at %d", mBaselineValues[0]);
    api().spoofData("CHwStates", "red", 0, mBaselineValues[0]);
	api().spoofData("CHwStates", "green", 0, mBaselineValues[0]);

    LOG_INFO( "Spoofing ProcRun RedPl at %d.", mBaselineRedPL );
	api().spoofData( "ProcRun_RedPl", "data", 0, mBaselineRedPL );
    api().spoofActivate();

    LOG_INFO( "Starting Timer Message." );
	mTimerMessageP->armTimer(TimerMessage::ARMED);
}

void Spoof_PIRFlagsBase::init()
{
}

void Spoof_PIRFlagsBase::help()
{
	LOG_INFO( "- Spoofs red values in order to trigger PIR flag conditions." );
	LOG_INFO( "- Run the spoof after midrun." );
	LOG_INFO( "- Do a purge during run." );
}

void Spoof_PIRFlagsBase::TimerMessageNotify(void)
{
	if (mValueIndex >= kCount) {
		mValueIndex = 0;
	}

	if (mRecoverySubstate == RecoveryTypes::None) {
		LOG_INFO( "Spoofing baseline values at %d", mBaselineValues[mValueIndex]);

        api().spoofData("CHwStates", "red", 0, mBaselineValues[mValueIndex]);
		api().spoofData("CHwStates", "green", 0, mBaselineValues[mValueIndex]);
	}
	else if (mRecoverySubstate == RecoveryTypes::WBCChamberPurge
			 || mRecoverySubstate == RecoveryTypes::ClearLine
			 || mRecoverySubstate == RecoveryTypes::SettleChannel) {
		LOG_INFO( "Spoofing testcase values at %d", mTestCaseValues[mValueIndex]);

		api().spoofData("CHwStates", "red", 0, mTestCaseValues[mValueIndex]);
		api().spoofData("CHwStates", "green", 0, mTestCaseValues[mValueIndex]);
	}

	++mValueIndex;
}

void Spoof_PIRFlagsBase::RecoveryStatusNotify(void)
{
   mRecoverySubstate = mRecoveryStatusP->getData()._Type;
   mRecoveryStatus = mRecoveryStatusP->getData()._InRecovery;
}

Spoof_PIRFlagsCase1::Spoof_PIRFlagsCase1(void)
{
    _name = "PIRFlagsCase1";
    _comment = "Spoofs red values in order to trigger PIR flag conditions.";
    _timeout = 60 * 60;

	mBaselineRedPL = 1300;
	std::memcpy(mBaselineValues, kBaselineValues1, sizeof(mBaselineValues));
	std::memcpy(mTestCaseValues, kTestCaseValues1, sizeof(mTestCaseValues));
}

Spoof_PIRFlagsCase2::Spoof_PIRFlagsCase2(void)
{
    _name = "PIRFlagsCase2";
    _comment = "Spoofs red values in order to trigger PIR flag conditions.";
    _timeout = 60 * 60;

	mBaselineRedPL = 1300;
	std::memcpy(mBaselineValues, kBaselineValues1, sizeof(mBaselineValues));
	std::memcpy(mTestCaseValues, kTestCaseValues2, sizeof(mTestCaseValues));
}

Spoof_PIRFlagsCase3::Spoof_PIRFlagsCase3(void)
{
    _name = "PIRFlagsCase3";
    _comment = "Spoofs red values in order to trigger PIR flag conditions.";
    _timeout = 60 * 60;

	mBaselineRedPL = 1300;
	std::memcpy(mBaselineValues, kBaselineValues1, sizeof(mBaselineValues));
	std::memcpy(mTestCaseValues, kTestCaseValues3, sizeof(mTestCaseValues));
}

Spoof_PIRFlagsCase4::Spoof_PIRFlagsCase4(void)
{
    _name = "PIRFlagsCase4";
    _comment = "Spoofs red values in order to trigger PIR flag conditions.";
    _timeout = 60 * 60;

	mBaselineRedPL = 1300;
	std::memcpy(mBaselineValues, kBaselineValues1, sizeof(mBaselineValues));
	std::memcpy(mTestCaseValues, kTestCaseValues4, sizeof(mTestCaseValues));
}

Spoof_PIRFlagsCase5::Spoof_PIRFlagsCase5(void)
{
    _name = "PIRFlagsCase5";
    _comment = "Spoofs red values in order to trigger PIR flag conditions.";
    _timeout = 60 * 60;

	mBaselineRedPL = 500;
	std::memcpy(mBaselineValues, kBaselineValues2, sizeof(mBaselineValues));
	std::memcpy(mTestCaseValues, kTestCaseValues3, sizeof(mTestCaseValues));
}

Spoof_PIRFlagsCase6::Spoof_PIRFlagsCase6(void)
{
    _name = "PIRFlagsCase6";
    _comment = "Spoofs red values in order to trigger PIR flag conditions.";
    _timeout = 60 * 60;

	mBaselineRedPL = 1300;
	std::memcpy(mBaselineValues, kBaselineValues3, sizeof(mBaselineValues));
	std::memcpy(mTestCaseValues, kTestCaseValues3, sizeof(mTestCaseValues));
}

#endif SPOOF_PIRFLAGS_H

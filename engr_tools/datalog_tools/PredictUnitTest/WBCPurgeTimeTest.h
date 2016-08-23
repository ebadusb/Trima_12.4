#ifndef _WBC_PURGE_TIME_TEST_H_
#define _WBC_PURGE_TIME_TEST_H_

#include "TestBase.h"

using namespace UnitTest;

class WBCPurgeTimeTest :
	public TestBase<WBCPurgeTimeTest>
{
protected:
	void setName();
public:
	WBCPurgeTimeTest(void);
	virtual ~WBCPurgeTimeTest(void);

	virtual void Initialize()
	{
		this->AddTestMethod( &WBCPurgeTimeTest::CentrifugeSpeedZeroTest, "CentrifugeSpeedZeroTest");
		this->AddTestMethod( &WBCPurgeTimeTest::CentrifugeSpeedAbove2500Test, "CentrifugeSpeedAbove2500Test");
		this->AddTestMethod( &WBCPurgeTimeTest::CentrifugeSpeedBelow2500Test, "CentrifugeSpeedBelow2500Test");
	}

	/*
	* test methods
	*/
	void CentrifugeSpeedZeroTest();
	void CentrifugeSpeedAbove2500Test();
	void CentrifugeSpeedBelow2500Test();
};

#endif
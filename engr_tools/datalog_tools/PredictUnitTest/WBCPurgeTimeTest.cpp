#include "WBCPurgeTimeTest.h"
#include "procdetpltplsrbc.h"
using namespace UnitTest;

WBCPurgeTimeTest::WBCPurgeTimeTest() : TestBase<WBCPurgeTimeTest>("WBCPurgeTimeTest")
{
}

WBCPurgeTimeTest::~WBCPurgeTimeTest(void)
{
}

void WBCPurgeTimeTest::CentrifugeSpeedZeroTest()
{
	CobeConfig::initialize("pir_cobe_config.dat",1);
	CobeConfig::CobeConfigStruct stru = CobeConfig::data();

	float time = procedureDetailPltPlsRBC::getWBCRmpUpRmpDnPurgeTime(0, stru);
	AssertAreEqual<const float>(0.0f, time, "time is not zero");
}

void WBCPurgeTimeTest::CentrifugeSpeedAbove2500Test()
{
	CobeConfig::initialize("pir_cobe_config.dat",1);
	CobeConfig::CobeConfigStruct stru = CobeConfig::data();

	float time = procedureDetailPltPlsRBC::getWBCRmpUpRmpDnPurgeTime(2800, stru);
	AssertAreEqual<const float>(14.0f, time, "time is not zero");
}

void WBCPurgeTimeTest::CentrifugeSpeedBelow2500Test()
{
	CobeConfig::initialize("pir_cobe_config.dat",1);
	CobeConfig::CobeConfigStruct stru = CobeConfig::data();

	float time = procedureDetailPltPlsRBC::getWBCRmpUpRmpDnPurgeTime(2200, stru);
	AssertAreEqual<const float>(44.0f, time, "time is not zero");
}

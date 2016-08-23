#ifndef __SUBSTATE_TEST_H__
#define __SUBSTATE_TEST_H__
#include "testbase.h"

using namespace UnitTest;

class SubstateTest :
    public TestBase<SubstateTest>
{
private:
    int m_proc;
public:
    SubstateTest(void);
    virtual ~SubstateTest(void);

    virtual void CleanUp();
    void SetupPrediction();

	virtual void Initialize();

	/*
	* test methods
	*/

	void ExecuteTest();
};

#endif //#ifndef __SUBSTATE_TEST_H__
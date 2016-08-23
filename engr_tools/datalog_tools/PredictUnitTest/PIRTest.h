#ifndef __PIR_TEST__
#define __PIR_TEST__

#include "TestBase.h"

using namespace UnitTest;

class PIRTest :
	public TestBase<PIRTest>
{
private:
    int m_proc;
public:
    PIRTest(int proc);
    ~PIRTest(void);

    virtual void CleanUp();
    void SetupPrediction();

	virtual void Initialize();

	/*
	* test methods
	*/

	void ExecuteTest();
	void ExecuteOneTest();
};


class PIRTestParameter
{
public:
    string donor;
    string expected;
    //expected result
    int id;
    int procNumber;
    float tbv;
    float precount;
    float hct;
    int gender;
    float height;
    float weight;
    float yield;
    float predVp;
    float predVc;
    float vinTarget;
    float predVac;
    float predTp;
    float ssPlateletPlasmaQininstd;
    float ssMidRunQininstd;
    float ssMidRunQch;
    float ssMidRunT;
    float ssPcaT;
    float ssExPcaT;
    float ssPirPlasmaT;
    float ssPirNoPlasmaT;
    float ssMidRunRpm;
    float ssPlateletPlasmaRpm;
    int status;

    void Extract();
};

#endif  

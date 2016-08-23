#ifndef __PREDICT_TEST_PARAMETER__
#define __PREDICT_TEST_PARAMETER__
#include <string>
using namespace std;

class PredictTestParameter
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

#endif //#ifndef __PREDICT_TEST_PARAMETER__
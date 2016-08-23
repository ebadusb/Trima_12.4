#ifndef _OLD_VERSION_COMPARE_TEST_H_
#define _OLD_VERSION_COMPARE_TEST_H_

#include "TestBase.h"
#include "OfflineOptimizer.h"
using namespace UnitTest;

namespace SS
{
enum State
{
	PREPLATELET_PLASMA,
	PREPLATELET_NOPLASMA,
	PLATELET_PLASMA,
	PCA,
	MIDRUN,
	PIR_PLASMA,
	PIR_NOPLASMA,
	EXTENDED_PCA,
	EXTENDED_PLASMA,
	LAST
};
}
class OldVersionParameter;
struct StateData;
class OldVersionCompareTest :
	public TestBase<OldVersionCompareTest>
{
private:
    int m_proc;
public:
    OldVersionCompareTest(int proc);
    ~OldVersionCompareTest(void);

    virtual void CleanUp();
    void SetupPrediction();

	virtual void Initialize();

	/*
	* test methods
	*/

	void OldVsMainTest();

private:
	void ComparePLTPLSRBC(PREDICT::Conditions ic, procedureDetail* proc, OldVersionParameter& parameter);
	void CompareSubstate(IntermediateResults& actual, StateData& expected, string sstate);

	string sstateName[SS::LAST];
};



struct StateData
{
   string Name;
   float Qcc;     // collect pump flow rate, flowing into the platelet collect bag
   float Qpc;     // plasma pump flow rate, flowing to plasma collect bag
   float T;    // Time
   float Qnret;   // flow through needle during SN return phase
   float Vsvn;    // SN stroke volume for nth cycle
   float Tcn;     // duration of SN cycle
   float Td;      // duration of SN cycle draw phase
   float Tr;      // duration of SN cycle return phase
   float D;    // SN duty cycle
   float Qrinst;  // instantaneous return pump flow
   float EndVin;  // The ending Vin value for this stage.
   float Vp;      // plasma collect volume
   float Vacp;    // ac in collected plasma
   float Vc;      // platelet collect volume;
   float Vacc;    // ac in collected platelet product
   float Vr;      // rbc collect volume
   float Vacr;    // ac in collected rbc product
   float AccumVp; // accumulated collected plasma (up to and including this substate)
   float AccumVacp; // accumulated ac in plasma (up to and including this substate)
   float AccumVc; // accumulated collected platelets (up to and including this substate)
   float AccumVacc; // accumulated ac in platelets (up to and including this substate)
   float AccumTime;  // accumulate the time storage
   float Vac;
   float AccumVac;

   //
   // new stuff for RBC-plasma protocols (Nov 2nd 2000)
   //
   float Vrep;    // Volume of replacement fluid
   float AccumVrep; // Vrep accumulated per substate
   float Qrep;    // Flow of replacement fluid
   float Hin;     // Inlet pump hematocrit
   float Facb;    // vol fraction of AC ref to pure plasma + AC
   float Facp;    // vol fraction of AC ref to pure plasma
   float AvgHt;   // average hematocrit
   float Hend;    // ending donor hematocrit for a substate

   //
   //  Instantaneous flows
   //
   float Qininstd;
   float Qininstr;
   float Qacinst;
   float Qcinstd;
   float Qcinstr;
   float Qpinstd;
   float Qpinstr;

   //
   // new stuff added August 10th for Bob L.'s new algorithms
   //
   float Qch;  // chamber flow ml/min

   //
   //  Margin for hypovolemia per substate
   //
   float HypovolemiaDistance;
   float HypovolemiaMargin;
   float Yield;
};

class OldVersionParameter
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
	string cassette;
    float yield;
	float plasmaVolume;
	float plateletVolume;
	float rbcVolume;
	float vinTarget;
	float Vac;
	float Time;
	float postCount;
	float hypoD;
	float postCrit;
    int status;
	StateData ssData[SS::LAST];

    void Extract();

	int ExtractState(int& offset, StateData& data);

	template <class T>
	int Extract( int count, T* data, char* format);
};



#endif
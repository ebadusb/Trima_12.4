#include "SubstateTest.h"
#include "OfflineOptimizer.h"
#include "PredictAnalyzer.h"
#include "PredictTestParameter.h"
#include "procdetpltplsrbc.h"
#include "procdetrbcpls.h"

extern PREDICT::PredictAnalyzer* analyzer;

void PlateleteTargetTest( const IntermediateResults& result, string substate, SubstateTest& test )
{
    if( result.Vc == 0 )
        test.AssertAreEqual<const float>(0.0f, result.T, substate +": (plt) Vc is zero, ");
    if( result.Vc > 0 )
    {
        //test.AssertIsTrue( result.T > 0, substate + " is used, time is not greater than zero, Platelet Target Test: ");
        //test.AssertIsTrue( result.Yield > 0, substate + " is used, yield is not greater than zero, Platelet Target Test: ");
    }
}

void PlasmaTargetTest( const IntermediateResults& result, string substate, SubstateTest& test )
{
    if( result.Vp == 0 )
        test.AssertAreEqual<const float>(0.0f, result.T, substate +" is not used, Plasma Target Test: ");
    //if( result.Vp > 0 )
    //    test.AssertIsTrue( result.T > 0, substate + " is used, time is not greater than zero, Plasma Target Test: ");
}

void RbcTargetTest( const IntermediateResults& result, string substate, SubstateTest& test )
{
    if( result.Vr == 0 )
        test.AssertAreEqual<const float>(0.0f, result.T, substate +" is not used, Rbc Target Test: ");
    if( result.Vr > 0 )
        test.AssertIsTrue( result.T > 0, substate + " is used, time is not greater than zero, Rbc Target Test: ");
}

void TargetTest( const IntermediateResults& result, string substate, SubstateTest& test )
{
    PlateleteTargetTest(result, substate, test);
    PlasmaTargetTest(result, substate, test);
    RbcTargetTest(result, substate, test);
}

SubstateTest::SubstateTest(void): TestBase<SubstateTest>("SubstateTest")
{
}


SubstateTest::~SubstateTest(void)
{
}

void SubstateTest::CleanUp()
{
    PREDICT::PredictAnalyzer::DestroyAnalyzer(analyzer);
}

void SubstateTest::Initialize()
{
    SetupPrediction();

    std::ifstream result;
    result.open( "pir_baseresult.csv", std::ios::in);

    int count = 0;
    char testName[20];

    char resultBuff[2000];
    result.getline(resultBuff, 2000);
    while( !result.eof() )
    {
        PredictTestParameter* param = new PredictTestParameter();
        result.getline(resultBuff, 2000);
        param->expected = string(resultBuff);
        param->Extract();

        //if( param->procNumber != m_proc )
        //    continue;

        if( result.eof() )
            break;

        sprintf(testName, "_%d_%d", param->id,param->procNumber);
        AddTestMethod( &SubstateTest::ExecuteTest, string("SubstateTest_Case") + string(testName), param);
    }
    result.close();
}

void SubstateTest::SetupPrediction()
{
    std::fstream output;
    output.open( "pir_predictunitest.csv", std::ios::out);
    PREDICT::PredictAnalyzer::CreateAnalyzerFromConfig("pir_config.dat", "pir_cobe_config.dat", "features.bin", &analyzer, output);
}

void SubstateTest::ExecuteTest()
{
    PREDICT::Conditions ic = { 0 };
    PREDICT::PredictAnalyzer::Function function = &PREDICT::PredictAnalyzer::PredictAllMod;

    PredictTestParameter* param = (PredictTestParameter*)GetParameter();

    ic.donor.id = param->id;
    ic.donor.female = param->gender;
    ic.donor.height = param->height;
    ic.donor.weight = param->weight;
    ic.donor.tbv = 0.0f;
    ic.donor.precount = param->precount;
    ic.donor.hct = param->hct;

    ic.adjust.qin = 60.0f;

    ic.donor.female--;

	if (!(ic.donor.hct < 1.0f && ic.donor.hct > 0.0f)) {
		ic.donor.hct /= 100.0f;
	}
	
    analyzer->SetDonor(ic.donor);

	analyzer->GetOptimizer().UpdateConditions(ic);

	std::list<procedureDetail*> listProc = (analyzer->*function)();
    std::list<procedureDetail*>::const_iterator procIter = listProc.begin();
    int count = 0;
   
    for( ; procIter != listProc.end() ; procIter++)
    {
        procedureDetailPltPlsRBC* proc = dynamic_cast<procedureDetailPltPlsRBC*>(*procIter);
        if( proc->getProcedureNumber() != param->procNumber )
            continue;

        int status = proc->getStatus();
        //preplatelet plasma, check for plasma target  :S
        const IntermediateResults& prePlateletPlasma = proc->getSubstate(Config::__PREPLATELET_PLASMA);
        PlasmaTargetTest(prePlateletPlasma, "__PREPLATELET_PLASMA", *this);

        //platelet plasma :S
        const IntermediateResults& plateletPlasma = proc->getSubstate(Config::__PLATELET_PLASMA);
        PlateleteTargetTest(plateletPlasma, "__PLATELET_PLASMA", *this);
        PlasmaTargetTest(plateletPlasma, "__PLATELET_PLASMA", *this);

        //pca :S
        const IntermediateResults& pCA = proc->getSubstate(Config::__PCA);
        PlasmaTargetTest(prePlateletPlasma, "__PCA", *this);

        //not sure
        //const IntermediateResults& prePlateletNoPlasma = proc->getSubstate(Config::__PREPLATELET_NOPLASMA);
        //PlasmaTargetTest(prePlateletNoPlasma, "__PREPLATELET_NOPLASMA", *this);

        //checking only platelets on midrun :S
        const IntermediateResults& midRun = proc->getSubstate(Config::__MIDRUN);
        PlateleteTargetTest(midRun, "__MIDRUN", *this);

        //:S
        const IntermediateResults& pirPlasma = proc->getSubstate(Config::__PIR_PLASMA);
        PlateleteTargetTest(pirPlasma, "__PIR_PLASMA", *this);
        //?
        PlasmaTargetTest(pirPlasma, "__PIR_PLASMA", *this);

        //:S
        const IntermediateResults& pirNoPlasma = proc->getSubstate(Config::__PIR_NOPLASMA);
        PlateleteTargetTest(pirNoPlasma, "__PIR_NOPLASMA", *this);

        //:S
        const IntermediateResults& extPCA = proc->getSubstate(Config::__EXTENDED_PCA);
        PlateleteTargetTest(extPCA, "__EXTENDED_PCA", *this);

        //:S
        const IntermediateResults& extPlasma = proc->getSubstate(Config::__EXTENDED_PLASMA);
        PlasmaTargetTest(extPlasma, "__EXTENDED_PLASMA", *this);

        //:S
        const IntermediateResults& rbc = proc->getSubstate(Config::__RBC);
        TargetTest(rbc, "__RBC", *this);
    
    }
}
        //const IntermediateResults& rbcSetup1 = proc->getSubstate(Config::__RBC_SETUP_1);
        //TargetTest(rbcSetup1, "__RBC_SETUP_1", *this);

        //const IntermediateResults& rbcSetup2 = proc->getSubstate(Config::__RBC_SETUP_2);
        //TargetTest(rbcSetup2, "__RBC_SETUP_2", *this);

        //const IntermediateResults& rbcPrime = proc->getSubstate(Config::__RBC_PRIME);
        //TargetTest(rbcPrime, "__RBC_PRIME", *this);

        //const IntermediateResults& channelSetup = proc->getSubstate(Config::__CHANNEL_SETUP);
        //TargetTest(channelSetup, "__CHANNEL_SETUP", *this);


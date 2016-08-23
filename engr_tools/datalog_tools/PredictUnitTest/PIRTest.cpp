#include "PIRTest.h"
#include "OfflineOptimizer.h"
#include "PredictAnalyzer.h"
#include "procdetpltplsrbc.h"
#include "procdetrbcpls.h"

PREDICT::PredictAnalyzer* analyzer;
void Compare(PREDICT::Conditions ic, procedureDetailPltPlsRBC* proc, PIRTest& test, PIRTestParameter& parameter);

PIRTest::PIRTest(int proc) : TestBase<PIRTest>("PIRTest")
{
    m_proc = proc;
}

PIRTest::~PIRTest(void)
{
}

void PIRTest::CleanUp()
{
    PREDICT::PredictAnalyzer::DestroyAnalyzer(analyzer);
}

void PIRTest::Initialize()
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
        PIRTestParameter* param = new PIRTestParameter();
        result.getline(resultBuff, 2000);
        param->expected = string(resultBuff);
        param->Extract();

        if( param->procNumber != m_proc )
            continue;

        if( result.eof() )
            break;

        sprintf(testName, "_%d_%d", param->id,param->procNumber);
        AddTestMethod( &PIRTest::ExecuteOneTest, string("ExecuteOneTest_Case") + string(testName), param);
    }
    result.close();
}

void PIRTest::SetupPrediction()
{
    std::fstream output;
    output.open( "pir_predictunitest.csv", std::ios::out);
    PREDICT::PredictAnalyzer::CreateAnalyzerFromConfig("pir_config.dat", "pir_cobe_config.dat", "features.bin", &analyzer, output);
}

void PIRTest::ExecuteOneTest()
{
    PREDICT::Conditions ic = { 0 };
    PREDICT::PredictAnalyzer::Function function = &PREDICT::PredictAnalyzer::PredictAllMod;

    PIRTestParameter* param = (PIRTestParameter*)GetParameter();

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
	procedureDetail* proc = analyzer->PredictProcedure(param->procNumber);
    procedureDetailPltPlsRBC* pltProc = dynamic_cast<procedureDetailPltPlsRBC*>(proc);

	if( pltProc != NULL )
		::Compare(ic, pltProc, *this, *param);
}

void Compare(PREDICT::Conditions ic, procedureDetailPltPlsRBC* proc, PIRTest& test, PIRTestParameter& parameter)
{
    test.AssertAreEqual<const int>(parameter.procNumber, proc->getProcedureNumber(), "Procedure number not equal");
    test.AssertAreEqual<const float>(parameter.yield, proc->getDesiredYield(), "Yield not equal");
    test.AssertAreEqual<const float>(parameter.predVp, proc->getPredVp(), "Plasma volume not equal");
    test.AssertAreEqual<const float>(parameter.predVc, proc->getPredVc(), "Platelet volume not equal");
    test.AssertAreEqual<const float>(parameter.vinTarget, proc->VinTarget(), "Vintarget not equal");
    test.AssertAreEqual<const float>(parameter.predVac, proc->getPredVac(), "AC volume not equal");
    test.AssertAreEqual<const float>(parameter.predTp, proc->getPredTp(), "Procedure Time not equal");
    test.AssertAreEqual<const float>(parameter.ssPlateletPlasmaQininstd, proc->getSubstate(Config::__PLATELET_PLASMA).Qininstd, "SS PltPls Qininstd not equal");
    test.AssertAreEqual<const float>(parameter.ssMidRunQininstd, proc->getSubstate(Config::__MIDRUN).Qininstd, "SS MR Qininstd not equal");
    test.AssertAreEqual<const float>(parameter.ssMidRunQch, proc->getSubstate(Config::__MIDRUN).Qch, "SS MR Qch not equal");
    test.AssertAreEqual<const float>(parameter.ssMidRunT, proc->getSubstate(Config::__MIDRUN).T, "SS MR time not equal");
    test.AssertAreEqual<const float>(parameter.ssPcaT, proc->getSubstate(Config::__PCA).T, "SS PCA time not equal");
    test.AssertAreEqual<const float>(parameter.ssExPcaT, proc->getSubstate(Config::__EXTENDED_PCA).T, "SS Ext PCA time not equal");
    test.AssertAreEqual<const float>(parameter.ssPirPlasmaT, proc->getSubstate(Config::__PIR_PLASMA).T, "SS PIR plasma time not equal");
    test.AssertAreEqual<const float>(parameter.ssPirNoPlasmaT, proc->getSubstate(Config::__PIR_NOPLASMA).T, "SS PIR No Plasma time not equal");
    test.AssertAreEqual<const float>(parameter.ssMidRunRpm, proc->getSubstate(Config::__MIDRUN).Rpm, "SS MR RPM not equal");
    test.AssertAreEqual<const float>(parameter.ssPlateletPlasmaRpm, proc->getSubstate(Config::__PLATELET_PLASMA).Rpm, "SS MR PltPls RPM not equal");
    test.AssertAreEqual<const int>(parameter.status, proc->getStatus(), "status not equal");
}

void PIRTestParameter::Extract()
{
	sscanf(expected.c_str(),"%d,%d,%f,%f,%f,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d", 
        &this->id,
        &this->procNumber,
        &this->tbv,
        &this->precount,
        &this->hct,
        &this->gender,
        &this->height,
        &this->weight,
        &this->yield,
        &this->predVp,
        &this->predVc,
        &this->vinTarget,
        &this->predVac,
        &this->predTp,
        &this->ssPlateletPlasmaQininstd,
        &this->ssMidRunQininstd,
        &this->ssMidRunQch,
        &this->ssMidRunT,
        &this->ssPcaT,
        &this->ssExPcaT,
        &this->ssPirPlasmaT,
        &this->ssPirNoPlasmaT,
        &this->ssMidRunRpm,
        &this->ssPlateletPlasmaRpm,
        &this->status);
}

void PIRTest::ExecuteTest()
{
    std::fstream output;
    output.open( "pir_predictunitest.csv", std::ios::out);
    PREDICT::PredictAnalyzer::CreateAnalyzerFromConfig("pir_config.dat", "pir_cobe_config.dat","pir_sw.dat", &analyzer, output);

    analyzer->AnalyzeDonorListMod("pir_donor.csv", &PREDICT::PredictAnalyzer::PredictAllMod);
    output.close();

    std::ifstream result;
    result.open( "pir_predictunitest.csv", std::ios::in);
    

    std::ifstream base_result;
    base_result.open( "pir_baseresult.csv", std::ios::in);

    char resultBuff[2000];
    char baseresultBuff[2000];
    while( !result.eof() && !base_result.eof() )
    {
        result.getline(resultBuff, 2000);
        base_result.getline(baseresultBuff, 2000);

        this->AssertAreEqual(string(baseresultBuff), string(resultBuff), "PIR Case Not Equal: ");
    }
}


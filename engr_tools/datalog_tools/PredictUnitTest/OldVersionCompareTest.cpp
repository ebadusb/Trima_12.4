#include "OldVersionCompareTest.h"
#include "OfflineOptimizer.h"
#include "PredictAnalyzer.h"
#include "procdetpltplsrbc.h"
#include "procdetrbcpls.h"

extern PREDICT::PredictAnalyzer* analyzer;
//void Compare(PREDICT::Conditions ic, procedureDetailPltPlsRBC* proc, OldVersionCompareTest& test, OldVersionParameter& parameter);

OldVersionCompareTest::OldVersionCompareTest(int proc) : TestBase<OldVersionCompareTest>("OldVersionCompareTest")
{
    m_proc = proc;
	sstateName[SS::PREPLATELET_PLASMA] = "PREPLATELATE_PLASMA";
	sstateName[SS::PREPLATELET_NOPLASMA] = "PREPLATELET_NOPLASMA";
	sstateName[SS::PLATELET_PLASMA] = "PLATELET_PLASMA";
	sstateName[SS::PCA] = "PCA";
	sstateName[SS::MIDRUN] = "MIDRUN";
	sstateName[SS::PIR_PLASMA] = "PIR_PLASMA";
	sstateName[SS::PIR_NOPLASMA] = "PIR_NOPLASMA";
	sstateName[SS::EXTENDED_PCA] = "EXTENDED_PCA";
	sstateName[SS::EXTENDED_PLASMA] = "EXTENDED_PLASMA";
}

OldVersionCompareTest::~OldVersionCompareTest(void)
{
}

void OldVersionCompareTest::CleanUp()
{
    PREDICT::PredictAnalyzer::DestroyAnalyzer(analyzer);
}

void OldVersionCompareTest::Initialize()
{
    SetupPrediction();

    std::ifstream result;
    result.open( "62_baseresult.csv", std::ios::in);

    int count = 0;
    char testName[100];

    char resultBuff[10000];
    result.getline(resultBuff, 10000);
    while( !result.eof() )
    {
        OldVersionParameter* param = new OldVersionParameter();
        result.getline(resultBuff, 10000);
        param->expected = string(resultBuff);
        param->Extract();

        if( result.eof() )
            break;

        sprintf(testName, "h:%1.0f w:%1.0f tbv:%1.0f pre:%1.0f hct%1.2f y:%1.1f pls:%1.0f r:%1.0f", param->height,param->weight,param->tbv, param->precount, param->hct, param->yield, param->plasmaVolume, param->rbcVolume);
		AddTestMethod( &OldVersionCompareTest::OldVsMainTest, string("vs62 ") + string(testName), param);
    }
    result.close();
}

void OldVersionCompareTest::SetupPrediction()
{
    std::fstream output;
    output.open( "pir_predictunitest.csv", std::ios::out);
    PREDICT::PredictAnalyzer::CreateAnalyzerFromConfig("62_config.dat", "pir_cobe_config.dat", "features.bin", &analyzer, output);
}

void OldVersionCompareTest::OldVsMainTest()
{
    PREDICT::Conditions ic = { 0 };
    PREDICT::PredictAnalyzer::Function function = &PREDICT::PredictAnalyzer::PredictAllMod;

    OldVersionParameter* param = (OldVersionParameter*)GetParameter();

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
	ComparePLTPLSRBC(ic, proc, *param);
}



void OldVersionCompareTest::ComparePLTPLSRBC(PREDICT::Conditions ic, procedureDetail* proc, OldVersionParameter& parameter)
{
    AssertAreEqual<const int>(parameter.procNumber, proc->getProcedureNumber(), "Procedure number not equal");

    AssertAreEqual<const float>(parameter.yield, proc->getDesiredYield(), "Yield not equal");
    AssertAreEqual<const float>(parameter.plasmaVolume, proc->getPredVp(), "Plasma volume not equal");
    AssertAreEqual<const float>(parameter.plateletVolume, proc->getPredVc(), "Platelet volume not equal");
    AssertAreEqual<const float>(parameter.rbcVolume, proc->getPredRBCVolume(), "rbc volume not equal");
	//AssertAreEqual<const float>(parameter.vinTarget, proc->getVintarg(), "Vin target not equal");
    AssertAreEqual<const float>(parameter.Vac, proc->getPredVac(), "Vac not equal");
	AssertAreEqual<const float>(parameter.Time, proc->getPredTp(), "Time not equal");
	AssertAreEqual<const int>(parameter.status, proc->getStatus(), "status not equal");

    procedureDetailPltPlsRBC* pltProc = dynamic_cast<procedureDetailPltPlsRBC*>(proc);
	if( pltProc == NULL )
		return;

	IntermediateResults substates[SS::LAST];
	substates[SS::PREPLATELET_PLASMA] = proc->getSubstate(Config::__PREPLATELET_PLASMA);
	substates[SS::PREPLATELET_NOPLASMA] = proc->getSubstate(Config::__PREPLATELET_NOPLASMA);
	substates[SS::PLATELET_PLASMA] = proc->getSubstate(Config::__PLATELET_PLASMA);
	substates[SS::PCA] = proc->getSubstate(Config::__PLATELET_PLASMA);
	substates[SS::MIDRUN] = proc->getSubstate(Config::__MIDRUN);
	substates[SS::PIR_PLASMA] = proc->getSubstate(Config::__PIR_PLASMA);
	substates[SS::PIR_NOPLASMA] = proc->getSubstate(Config::__PIR_PLASMA);
	substates[SS::EXTENDED_PCA] = proc->getSubstate(Config::__EXTENDED_PCA);
	substates[SS::EXTENDED_PLASMA] = proc->getSubstate(Config::__EXTENDED_PLASMA);

	for( int i = SS::PREPLATELET_PLASMA ; i < SS::LAST ; i++)
	{
		CompareSubstate(substates[i], parameter.ssData[i], sstateName[i]);
	}
}

void OldVersionCompareTest::CompareSubstate(IntermediateResults& actual, StateData& expected, string sstate)
{
	AssertAreEqual(expected.AccumTime, actual.AccumTime , sstate + " AccumTime not equal");
}

char* intFormat = "%d";
char* floatFormat = "%f";
char* stringFormat = "%s";

void OldVersionParameter::Extract()
{
	int data;
	int count = 0, nth = 0;

	//Extract<int>(count, nth, data, format);
	int offset = 0;
	offset += Extract<int>(offset, &this->id, intFormat);
	offset += Extract<int>(offset, &this->procNumber, intFormat);
	offset += Extract<float>(offset, &this->tbv, floatFormat);
	offset += Extract<float>(offset, &this->precount, floatFormat);
	offset += Extract<float>(offset, &this->hct, floatFormat);
	offset += Extract<int>(offset, &this->gender, intFormat);
	offset += Extract<float>(offset, &this->height, floatFormat);
	offset += Extract<float>(offset, &this->weight, floatFormat);
	offset += Extract<string>(offset, &this->cassette, stringFormat);
	offset += Extract<float>(offset, &this->yield, floatFormat);
	offset += Extract<float>(offset, &this->plasmaVolume, floatFormat);
	offset += Extract<float>(offset, &this->plateletVolume, floatFormat);
	offset += Extract<float>(offset, &this->rbcVolume, floatFormat);
	offset += Extract<float>(offset, &this->vinTarget, floatFormat);
	offset += Extract<float>(offset, &this->Vac, floatFormat);
	offset += Extract<float>(offset, &this->Time, floatFormat);
	offset += Extract<float>(offset, &this->postCount, floatFormat);
	offset += Extract<float>(offset, &this->hypoD, floatFormat);
	offset += Extract<float>(offset, &this->postCrit, floatFormat);
	offset += Extract<int>(offset, &this->status, intFormat);

	if( strcmp(cassette.c_str(), "WHITE") == 0)
	{
		for( int i = 0; i < SS::LAST ; i++ )
			ExtractState(offset, ssData[i]);
	}
}

int OldVersionParameter::ExtractState(int& offset, StateData& data)
{
	offset += Extract<string>(offset, &data.Name, stringFormat);
	offset += Extract<float>(offset, &data.AccumTime, floatFormat);
	offset += Extract<float>(offset, &data.AccumVac, floatFormat);
	offset += Extract<float>(offset, &data.AccumVacc, floatFormat);
	offset += Extract<float>(offset, &data.AccumVacp, floatFormat);
	offset += Extract<float>(offset, &data.AccumVc, floatFormat);
	offset += Extract<float>(offset, &data.AccumVp, floatFormat);
	offset += Extract<float>(offset, &data.AccumVrep, floatFormat);
	offset += Extract<float>(offset, &data.AvgHt, floatFormat);
	offset += Extract<float>(offset, &data.D, floatFormat);
	offset += Extract<float>(offset, &data.EndVin, floatFormat);
	offset += Extract<float>(offset, &data.Facb, floatFormat);
	offset += Extract<float>(offset, &data.Facp, floatFormat);
	offset += Extract<float>(offset, &data.Hend, floatFormat);
	offset += Extract<float>(offset, &data.Hin, floatFormat);
	offset += Extract<float>(offset, &data.HypovolemiaDistance, floatFormat);
	offset += Extract<float>(offset, &data.HypovolemiaMargin, floatFormat);
	offset += Extract<float>(offset, &data.Qacinst, floatFormat);
	offset += Extract<float>(offset, &data.Qcc, floatFormat);
	offset += Extract<float>(offset, &data.Qch, floatFormat);
	offset += Extract<float>(offset, &data.Qcinstd, floatFormat);
	offset += Extract<float>(offset, &data.Qcinstr, floatFormat);
	offset += Extract<float>(offset, &data.Qininstd, floatFormat);
	offset += Extract<float>(offset, &data.Qininstr, floatFormat);
	offset += Extract<float>(offset, &data.Qnret, floatFormat);
	offset += Extract<float>(offset, &data.Qpc, floatFormat);
	offset += Extract<float>(offset, &data.Qpinstd, floatFormat);
	offset += Extract<float>(offset, &data.Qpinstr, floatFormat);
	offset += Extract<float>(offset, &data.Qrep, floatFormat);
	offset += Extract<float>(offset, &data.Qrinst, floatFormat);
	offset += Extract<float>(offset, &data.T, floatFormat);
	offset += Extract<float>(offset, &data.Tcn, floatFormat);
	offset += Extract<float>(offset, &data.Td, floatFormat);
	offset += Extract<float>(offset, &data.Tr, floatFormat);
	offset += Extract<float>(offset, &data.Vac, floatFormat);
	offset += Extract<float>(offset, &data.Vacc, floatFormat);
	offset += Extract<float>(offset, &data.Vacp, floatFormat);
	offset += Extract<float>(offset, &data.Vacr, floatFormat);
	offset += Extract<float>(offset, &data.Vc, floatFormat);
	offset += Extract<float>(offset, &data.Vp, floatFormat);
	offset += Extract<float>(offset, &data.Vr, floatFormat);
	offset += Extract<float>(offset, &data.Vrep, floatFormat);
	offset += Extract<float>(offset, &data.Vsvn, floatFormat);
	offset += Extract<float>(offset, &data.Yield, floatFormat);
	return offset;
}

template <class T>
int OldVersionParameter::Extract( int count, T* data, char* format)
{
	int loc = expected.find( ',', count);
	string sub = expected.substr(count, loc - count);
	sscanf(sub.c_str(), format, data);
	return loc - count + 1;
}

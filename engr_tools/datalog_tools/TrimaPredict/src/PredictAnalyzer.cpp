/*****************************************************************************\
**
** PredictAnalyzer.cpp
**
** Predict analyzer implementation.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <iomanip>

#include "PredictAnalyzer.h"
#include "sharedstruct.h"
#include "predict_strings.h"
#include "posthct.hpp"
#include "postcount.hpp"
#include "bloodtype.h"
#include "LogExtractor.h"
#include "LogExecute.h"
#include "ExtractConfig.h"
#include "procdetpltplsrbc.h"
#include "procdetrbcpls.h"

namespace PREDICT {

/* Functions *****************************************************************/

/**
 *
 * CreateAnalyzerFromLog
 *
 * Creates an analyzer and populates the configurations
 * using the given datalog and cobe config files.
 *
 * @param dataLogPath The datalog file
 * @param cobeConfigPath The cobe config file.
 * @param analyzer The new analyzer.
 *
 * @return True if the analyzer was created.
 *
 */
bool PredictAnalyzer::CreateAnalyzerFromLog(
		const char* dataLogPath,
		const char* cobeConfigPath, 
		PredictAnalyzer **analyzer)
{
	OfflineOptimizer *offlineOptimizerP = NULL;
	PredictRecordMrg *predictRecordMrgP = new PredictRecordMrg();

	if (!OfflineOptimizer::CreateOptimizerFromLog(
			dataLogPath, cobeConfigPath, 
			*predictRecordMrgP, &offlineOptimizerP)) {
		delete predictRecordMrgP;
		std::cerr << "Error creating offline optimizer." << std::endl;
		return false;
	}

	*analyzer = new PredictAnalyzer();
	(*analyzer)->mOfflineOptimizerP = offlineOptimizerP;
	(*analyzer)->mPredictRecordMrgP = predictRecordMrgP;

	return true;
}

/**
 *
 * CreateAnalyzerFromConfig
 *
 * Creates an analyzer and populates the configurations
 * using the given user and cobe config files.
 *
 * @param userConfigPath The user config file
 * @param cobeConfigPath The cobe config file.
 * @param analyzer The new analyzer.
 *
 * @return True if the analyzer was created.
 *
 */
bool PredictAnalyzer::CreateAnalyzerFromConfig(
		const char* userConfigPath,
		const char* cobeConfigPath, 
		PredictAnalyzer **analyzer,
		std::ostream& output)
{

	OfflineOptimizer *offlineOptimizerP = NULL;

	if (!OfflineOptimizer::CreateOptimizerFromConfig(userConfigPath, cobeConfigPath, &offlineOptimizerP))
    {
		std::cerr << "Error creating offline optimizer." << std::endl;
		return false;
	}

	*analyzer = new PredictAnalyzer(output);

	(*analyzer)->mOfflineOptimizerP = offlineOptimizerP;

	(*analyzer)->mPredictRecordMrgP = new PredictRecordMrg();

	return true;
}

bool PredictAnalyzer::CreateAnalyzerFromConfig(
		const char* userConfigPath,
		const char* cobeConfigPath, 
        const char* swConfigPath,
		PredictAnalyzer **analyzer,
		std::ostream& output)
{

	OfflineOptimizer *offlineOptimizerP = NULL;

	if (!OfflineOptimizer::CreateOptimizerFromConfig(userConfigPath, cobeConfigPath, swConfigPath, &offlineOptimizerP))
    {
		std::cerr << "Error creating offline optimizer." << std::endl;
		return false;
	}

	*analyzer = new PredictAnalyzer(output);

	(*analyzer)->mOfflineOptimizerP = offlineOptimizerP;

	(*analyzer)->mPredictRecordMrgP = new PredictRecordMrg();

	return true;
}

/**
 *
 * DestroyOptimizer
 *
 * Destroys the given analyzer.
 *
 * @param analyzer The analyzer to delete.
 *
 * @return True if the analyzer was destroyed.
 *
 */
bool PredictAnalyzer::DestroyAnalyzer(PredictAnalyzer *analyzer)
{
	bool result = false;

	if (analyzer != NULL) {
		OfflineOptimizer::DestroyOptimizer(analyzer->mOfflineOptimizerP);
		delete analyzer->mPredictRecordMrgP;
		delete analyzer;
		result = true;
	}

	return result;
}

/**
 *
 * PredictAnalyzer
 *
 * Constructor
 *
 */
PredictAnalyzer::PredictAnalyzer(void) : 
mOfflineOptimizerP(NULL), mPredictRecordMrgP(NULL), mStream(std::cout), mCurDonor()
{
}

PredictAnalyzer::PredictAnalyzer(std::ostream& outfile) : 
mOfflineOptimizerP(NULL), mPredictRecordMrgP(NULL), mStream(outfile), mCurDonor()
{
}
/**
 *
 * ~PredictAnalyzer
 *
 * Destructor
 *
 */
PredictAnalyzer::~PredictAnalyzer()
{
}

void PredictAnalyzer::AnalyzeSingleRun(int procedureNum, std::string minsIntoRun)
{	
	mOfflineOptimizerP->UpdateConditions(mPredictRecordMrgP->GetRecord(minsIntoRun));

	procedureDetail* proc = mOfflineOptimizerP->GetOptimizeProcedure(procedureNum);

	if (proc->getStatus() == GOOD_PRODUCT) {
		this->mStream << procedureNum << ": "
				<< proc->getYtarg()  << " "
				<< proc->getPredVc()  << " "
				<< proc->getPredVp()  << " "
				<< proc->getPredRBCVolume()  << " "
				<< proc->getPredTp()  << " "
				<< std::endl;
	} 
	else {
		this->mStream << procedureNum << ": "
				<< ConvertResultToDescription((INTERNAL_PREDICT_RESULTS)proc->getStatus())
				<< std::endl;
	}
}

void PredictAnalyzer::AnalyzeSingleRun(int procedureNum, std::string minsIntoRun, const char* features)
{	
	Conditions& condition  = mPredictRecordMrgP->GetRecord(minsIntoRun);

	if(Software_CDS::Read(features))
	{
		mOfflineOptimizerP->UpdateConditions(condition);

		procedureDetail* proc = mOfflineOptimizerP->GetOptimizeProcedure(procedureNum);

		if (proc->getStatus() == GOOD_PRODUCT) {
			this->mStream << procedureNum << ": "
					<< proc->getYtarg()  << " "
					<< proc->getPredVc()  << " "
					<< proc->getPredVp()  << " "
					<< proc->getPredRBCVolume()  << " "
					<< proc->getPredTp()  << " "
					<< std::endl;
		} 
		else {
			this->mStream << procedureNum << ": "
					<< ConvertResultToDescription((INTERNAL_PREDICT_RESULTS)proc->getStatus())
					<< std::endl;
		}
	}
}

std::list<procedureDetail*> PredictAnalyzer::AnalyzeAllPredictions(float minsIntoRun)
{                                                
	mOfflineOptimizerP->UpdateConditions(mPredictRecordMrgP->GetRecord(minsIntoRun));
	std::list<procedureDetail*> listProc;
	for (int product = 1; product <= mOfflineOptimizerP->GetNumProcedures(); ++product) {
		procedureDetail* proc = mOfflineOptimizerP->GetOptimizeProcedure(product);

		if (proc->getStatus() == GOOD_PRODUCT) {
			this->mStream << product << ": "
					<< proc->getYtarg()  << " "
					<< proc->getPredVc()  << " "
					<< proc->getPredVp()  << " "
					<< proc->getPredRBCVolume()  << " "
					<< proc->getPredTp()  << " "
					<< std::endl;
			listProc.push_back(proc);
		} 
		else { 
			this->mStream << product << ": "
					<< ConvertResultToDescription((INTERNAL_PREDICT_RESULTS)proc->getStatus())
					<< std::endl;
		}
	}
	return listProc;
}

std::list<procedureDetail*> PredictAnalyzer::PredictAll()
{
	std::list<procedureDetail*> listProc;
	for (int product = 1; product <= mOfflineOptimizerP->GetNumProcedures(); ++product) {
		procedureDetail* proc = mOfflineOptimizerP->GetOptimizeProcedure(product);
		if (proc->getStatus() == GOOD_PRODUCT) {
			listProc.push_back(proc);
		} 
	}
	return listProc;
}

//void PredictAnalyzer::AnalyzeAmapPlatelets(Stream &this->mStream, const char *donorListPath)
//{
//	this->AnalyzeDonorList(this->mStream, donorListPath, &PredictAnalyzer::AmapPlatelets);
//}

void PredictAnalyzer::AnalyzeDonorList(const char *donorListPath, PredictAnalyzer::Function function)
{


	std::ifstream ifs(donorListPath);
	PREDICT::Conditions ic = { 0 };
	std::string linebuffer;

    this->mStream << "Run No., TBV, Cpre, H(%), Sex , Height(in) , Weight(lbs), ProcNum"
			"Yield, Vp, Vc, Vrbc, Vintarg, Vac, Time(min), Postcount, HypoD(ml), Postcrit, "
			"Qin-pp, Qin-pca, Qin-mr, Qin-pir, Qr-pp, Qr-pca, Qr-mr, Qr-pir, Status"
			<< std::endl;

	while (getline(ifs, linebuffer)) {
		if (linebuffer.find("ENDDATA") != std::string::npos ) {
			break;
		}

		sscanf(linebuffer.c_str(), "%d, %d,%f,%f,%f,%f,%f", 
				&ic.donor.id, &ic.donor.female, &ic.donor.height, 
				&ic.donor.weight, &ic.donor.tbv, &ic.donor.precount, 
				&ic.donor.hct);
		
		ic.donor.female--;

		if (!(ic.donor.hct < 1.0f && ic.donor.hct > 0.0f)) {
			ic.donor.hct /= 100.0f;
		}
		this->SetDonor(ic.donor);

		mOfflineOptimizerP->UpdateConditions(ic);

		std::list<procedureDetail*> listProc = (this->*function)();
		for(std::list<procedureDetail*>::const_iterator procIter = listProc.begin(); 
			procIter != listProc.end(); procIter++ )
		{
			OutputData(*procIter, ic.donor);
		}
   }

   ifs.close();
}

void PredictAnalyzer::AnalyzeDonorListMod(const char *donorListPath, PredictAnalyzer::Function function)
{
   char gender[2] = {' ', '\0'};
	std::ifstream ifs(donorListPath);
	PREDICT::Conditions ic = { 0 };
	std::string linebuffer;
    
    std::string ht("Height(in)");
    std::string wt("Weight(lbs)");
    if( mOfflineOptimizerP->GetConfigs().LangUnit.Get().key_weight != WEIGHT_UNITS_LB )
        wt = "Weight(kg)";
    if( mOfflineOptimizerP->GetConfigs().LangUnit.Get().key_height != HEIGHT_UNITS_IN )
        ht = "Height(cm)";
    std::string tbv("TBV");
	if(Software_CDS::GetInstance().getFeature(JapanFeatures)  == true)
        tbv = "TBV(Ogawa)";

   OutputCommonHeader(this->mStream, tbv, ht, wt);
   OutputSubstateHeader(this->mStream, "CHL");
   OutputSubstateHeader(this->mStream, "prePLTPLS");
   OutputSubstateHeader(this->mStream, "prePLTnPLS");
   OutputSubstateHeader(this->mStream, "PLTPLS");
   OutputSubstateHeader(this->mStream, "PCA");
   OutputSubstateHeader(this->mStream, "MR");
   OutputSubstateHeader(this->mStream, "PIR_PLS");
   OutputSubstateHeader(this->mStream, "PIR_NPLS");
   OutputSubstateHeader(this->mStream, "EPCA");
   OutputSubstateHeader(this->mStream, "EPLS");
   this->mStream << std::endl;

	while (getline(ifs, linebuffer))
     {

		if (linebuffer.find("ENDDATA") != std::string::npos ) 
        {
            std::cout << " breaking input read" << std::endl;
			break;
		}


      sscanf(linebuffer.c_str(), "%d, %c,%f,%f,%f,%f,%f,%f,%f,%f,%f",
             &ic.donor.id, gender, &ic.donor.height,
             &ic.donor.weight, &ic.donor.tbv, &ic.donor.precount,
             &ic.donor.hct,
             // parameterized adjustments
             &ic.adjust.qin, &ic.adjust.qrp, &ic.adjust.ratio, &ic.adjust.ir);
		
      ic.donor.female = (gender[0] == 'M') ? 0 : 1;

		if (!(ic.donor.hct < 1.0f && ic.donor.hct > 0.0f)) {
			ic.donor.hct /= 100.0f;
		}
		this->SetDonor(ic.donor);

		mOfflineOptimizerP->UpdateConditions(ic);

		this->mStream.setf(std::ios::fixed);
		this->mStream.precision(2);
		std::list<procedureDetail*> listProc = (this->*function)();
		for(std::list<procedureDetail*>::const_iterator procIter = listProc.begin(); 
			procIter != listProc.end(); procIter++ )
		{
         procedureDetailPltPlsRBC* whiteStamp = dynamic_cast<procedureDetailPltPlsRBC*>(*procIter);
         procedureDetailRBCPls*    blackStamp = dynamic_cast<procedureDetailRBCPls*>(*procIter);
         if ( whiteStamp )
            OutputCommonData(this->mStream, *whiteStamp, (*procIter)->getProcedureNumber(), ic.donor, gender);
         else if ( blackStamp )
            OutputCommonData(this->mStream, *blackStamp, (*procIter)->getProcedureNumber(), ic.donor, gender);

         OutputSubstateHeader(this->mStream, "", (*procIter)->getSubstate(Config::__CHANNEL_SETUP));
         OutputSubstateHeader(this->mStream, "", (*procIter)->getSubstate(Config::__PREPLATELET_PLASMA));
         OutputSubstateHeader(this->mStream, "", (*procIter)->getSubstate(Config::__PREPLATELET_NOPLASMA));
         OutputSubstateHeader(this->mStream, "", (*procIter)->getSubstate(Config::__PLATELET_PLASMA));
         OutputSubstateHeader(this->mStream, "", (*procIter)->getSubstate(Config::__PCA));
         OutputSubstateHeader(this->mStream, "", (*procIter)->getSubstate(Config::__MIDRUN));
         OutputSubstateHeader(this->mStream, "", (*procIter)->getSubstate(Config::__PIR_PLASMA));
         OutputSubstateHeader(this->mStream, "", (*procIter)->getSubstate(Config::__PIR_NOPLASMA));
         OutputSubstateHeader(this->mStream, "", (*procIter)->getSubstate(Config::__EXTENDED_PCA));
         OutputSubstateHeader(this->mStream, "", (*procIter)->getSubstate(Config::__EXTENDED_PLASMA));

         this->mStream << std::endl;
		}
   }

   ifs.close();
}

std::list<procedureDetail*> PredictAnalyzer::FirstValid()
{
	std::list<procedureDetail*> listProc;
	for (int product = 1; product <= mOfflineOptimizerP->GetNumProcedures(); ++product) { 
		procedureDetail* proc = mOfflineOptimizerP->GetOptimizeProcedure(product);
		if(proc->getStatus() == GOOD_PRODUCT){
			listProc.push_back(proc);
			break;
		}
	}
	return listProc;
}

std::list<procedureDetail*> PredictAnalyzer::PredictAllMod()
{
	std::list<procedureDetail*> listProc;
	for (int product = 1; product <= mOfflineOptimizerP->GetNumProcedures(); ++product) {
		procedureDetail* proc = mOfflineOptimizerP->GetOptimizeProcedure(product);
		if( proc->getStatus() != 12)
			listProc.push_back(proc);
	}
	return listProc;
}

procedureDetail* PredictAnalyzer::PredictProcedure(int index)
{
	procedureDetail* proc = mOfflineOptimizerP->GetOptimizeProcedure(index);
	return proc;
}

void PredictAnalyzer::OutputData(procedureDetail *pd, DECODER::UTIL::Donor& donor)
{
	PostHct hct; 

	// regurgitate data
	this->mStream << donor.id << ","  
			<< donor.tbv << ","  
			<< donor.precount << "," 
			<< donor.hct << "," 
			<< donor.female + 1 << "," 
			<< donor.height << "," 
			<< donor.weight ;


	if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {
		procedureDetailPltPlsRBC* proc = static_cast<procedureDetailPltPlsRBC*>(pd);
   
		this->mStream  << "," << proc->getDesiredYield()
				<< "," << proc->getPredVp()
				<< "," << proc->getPredVc()
				<< "," << proc->getPredRBCVolume()
				<< "," << proc->VinTarget()
				<< "," << proc->getPredVac()
				<<  "," << ((proc->getPredTp() > 0) ? (proc->getPredTp()): 0)
				<< "," << Postcount(donor.precount, 
						donor.tbv, 
						proc->getDesiredYield()*1E11,        // precount, tbv, yield, minpostcount 
						Config::Instance().getMinPostCount()).postcount() 
				<< "," << ((fabs(proc->hypovolemiaDistance()) > donor.tbv /* cuts out random unitialized vals. */ ) ? 0 : proc->hypovolemiaDistance() )
				<< "," << hct.postHct(donor.hct, 
						donor.tbv,
						proc->getProcRBCDose(),
						proc->getResidualRBCVolume(),
						20.0f /*sample volume*/)
				<< "," << proc->getSubstate(Config::__PLATELET_PLASMA).Qininstd
				<< "," << proc->getSubstate(Config::__PCA).Qininstd
				<< "," << proc->getSubstate(Config::__MIDRUN).Qininstd
				<< "," << proc->getSubstate(Config::__PIR_PLASMA).Qininstd
				<< "," << proc->getSubstate(Config::__PLATELET_PLASMA).Qrinst
				<< "," << proc->getSubstate(Config::__PCA).Qrinst
				<< "," << proc->getSubstate(Config::__MIDRUN).Qrinst
                << "," << proc->getSubstate(Config::__PIR_PLASMA).Qrinst
				<< "," << proc->getStatus() ;
	}
	else{
		procedureDetailRBCPls* proc = static_cast<procedureDetailRBCPls*>(pd);

		this->mStream  << "," << 0 
				<< "," << proc->getPredVp()
				<< "," << 0 
				<< "," << proc->getPredRBCVolume()
				<< "," << 0
				<< "," << proc->getPredVac()
				<<  "," << ((proc->getPredTp() > 0) ? (proc->getPredTp()): 0)
				<< "," << Postcount(donor.precount, 
						donor.tbv, 
						proc->getDesiredYield()*1E11,        // precount, tbv, yield, minpostcount 
						Config::Instance().getMinPostCount()).postcount() 
				<< "," << ((fabs(proc->hypovolemiaDistance()) > donor.tbv /* cuts out random unitialized vals. */ ) ? 0 : proc->hypovolemiaDistance() )
				<< "," << hct.postHct(donor.hct, 
						donor.tbv,
						proc->getProcRBCDose(),
						proc->getResidualRBCVolume(),
						20.0f /*sample volume*/)
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << proc->getStatus() ;

		//RBC? 
	}

	this->mStream << std::endl;
}

void PredictAnalyzer::OutputDataMod(procedureDetail *pd, DECODER::UTIL::Donor& donor, const int& procedureNum)
{
	PostHct hct; 

	// regurgitate data
	this->mStream << donor.id << "," << procedureNum << ","
		<< donor.tbv << ","  
			<< donor.precount << "," 
			<< donor.hct << "," 
			<< donor.female + 1 << "," 
			<< donor.height << "," 
			<< donor.weight;

	if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {
		procedureDetailPltPlsRBC* proc = static_cast<procedureDetailPltPlsRBC*>(pd);
		this->mStream  << ", " << proc->getDesiredYield()
				<< "," << proc->getPredVp()
				<< "," << proc->getPredVc()
				//<< "," << proc->_PredRBCVolume
				<< "," << proc->VinTarget()
				<< "," << proc->getPredVac()
				<<  "," << ((proc->getPredTp() > 0) ? (proc->getPredTp()): 0)
				//<< "," << Postcount(donor.precount, 
				//		donor.tbv, 
				//		proc->getDesiredYield()*1E11,        // precount, tbv, yield, minpostcount 
				//		Config::Instance().getMinPostCount()).postcount() 
				//<< "," << ((fabs(proc->hypovolemiaDistance()) > donor.tbv /* cuts out random unitialized vals. */ ) ? 0 : proc->hypovolemiaDistance() )
				//<< "," << hct.postHct(donor.hct, 
				//		donor.tbv,
				//		proc->getProcRBCDose(),
				//		proc->getResidualRBCVolume(),
				//		20.0f /*sample volume*/)
				<< "," << proc->getSubstate(Config::__PLATELET_PLASMA).Qininstd
				//<< "," << proc->substate[Config::__PCA].Qininstd
				<< "," << proc->getSubstate(Config::__MIDRUN).Qininstd
				//<< "," << proc->substate[Config::__PIR_PLASMA].Qininstd
				//<< "," << proc->substate[Config::__PLATELET_PLASMA].Qrinst
				//<< "," << proc->substate[Config::__PCA].Qrinst
				//<< "," << proc->substate[Config::__MIDRUN].Qrinst3
				//<< "," << proc->substate[Config::__PIR_PLASMA].Qrinst
				<< ", " << proc->getSubstate(Config::__MIDRUN).Qch
				<< ", " << proc->getSubstate(Config::__MIDRUN).T
				<< ", " << proc->getSubstate(Config::__PCA).T
				<< ", " << proc->getSubstate(Config::__EXTENDED_PCA).T
				<< ", " << proc->getSubstate(Config::__PIR_PLASMA).T
				<< ", " << proc->getSubstate(Config::__PIR_NOPLASMA).T
				<< ", " << proc->getSubstate(Config::__MIDRUN).Rpm
				<< ", " << proc->getSubstate(Config::__PLATELET_PLASMA).Rpm
				<< "," << proc->getStatus() ;
	}
	else{
		procedureDetailRBCPls* proc = static_cast<procedureDetailRBCPls*>(pd);

		this->mStream  << ", " << 0 
				<< "," << proc->getPredVp()
				<< "," << 0 
				//<< "," << proc->_PredRBCVolume
				<< "," << 0
				<< "," << proc->getPredVac()
				<<  "," << ((proc->getPredTp() > 0) ? (proc->getPredTp()): 0)
				//<< "," << Postcount(donor.precount, 
				//		donor.tbv, 
				//		proc->getDesiredYield()*1E11,        // precount, tbv, yield, minpostcount 
				//		Config::Instance().getMinPostCount()).postcount() 
				//<< "," << ((fabs(proc->hypovolemiaDistance()) > donor.tbv /* cuts out random unitialized vals. */ ) ? 0 : proc->hypovolemiaDistance() )
				//<< "," << hct.postHct(donor.hct, 
				//		donor.tbv,
				//		proc->getProcRBCDose(),
				//		proc->getResidualRBCVolume(),
				//		20.0f /*sample volume*/)
				<< "," << 0 
				//<< "," << 0 
				<< "," << 0 
				//<< "," << 0 
				//<< "," << 0 
				//<< "," << 0 
				//<< "," << 0 
				//<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << 0 
				<< "," << proc->getStatus() ;

		//RBC? 
	}

	this->mStream << std::endl;
}

} // namespace PREDICT
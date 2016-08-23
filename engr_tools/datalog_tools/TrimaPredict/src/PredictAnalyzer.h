/*****************************************************************************\
**
** PredictAnalyzer.h
**
** Predict analyzer implementation.
**
\*****************************************************************************/

#ifndef PREDICTANALYZER_H
#define PREDICTANALYZER_H

/* Includes ******************************************************************/

#include "OfflineOptimizer.h"
#include "procdetpltplsrbc.h"
#include "procdetrbcpls.h"

namespace PREDICT {

/* Classes *******************************************************************/

class PredictAnalyzer {
public:

	typedef std::ostream Stream;
	typedef std::list<procedureDetail*> (PredictAnalyzer::*Function)();//Update all conditions before invoking this function
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
	static bool CreateAnalyzerFromLog(
			const char* dataLogPath,
			const char* cobeConfigPath, 
			PredictAnalyzer **analyzer);

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
	static bool CreateAnalyzerFromConfig(
			const char* userConfigPath,
			const char* cobeConfigPath,
			PredictAnalyzer **analyzer,
			std::ostream& ostream = std::cout);

	static bool CreateAnalyzerFromConfig(
			const char* userConfigPath,
			const char* cobeConfigPath,
            const char* swConfigPath,
			PredictAnalyzer **analyzer,
			std::ostream& ostream = std::cout);
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
	static bool DestroyAnalyzer(
			PredictAnalyzer *analyzer);

	void AnalyzeSingleRun(int procedureNum, std::string minsIntoRun );
	void AnalyzeSingleRun(int procedureNum, std::string minsIntoRun, const char* features );
	std::list<procedureDetail*> AnalyzeAllPredictions(float minsIntoRun = 0.0f);
	//void AnalyzeAmapPlatelets(Stream &stream, const char *donorListPath);
	void SetDonor(DECODER::UTIL::Donor& donor) {this->mCurDonor = donor;}
	void AnalyzeDonorList(const char *donorListPath, PredictAnalyzer::Function function);
	void AnalyzeDonorListMod(const char *donorListPath, PredictAnalyzer::Function function);
	std::list<procedureDetail*> FirstValid();
	std::list<procedureDetail*> AmapPlatelets();
	std::list<procedureDetail*> PredictAll();
	std::list<procedureDetail*> PredictAllMod();
    OfflineOptimizer& GetOptimizer() { return *mOfflineOptimizerP;}
    std::ostream& GetOutputStream() { return mStream; }

    void OutputData(procedureDetail *procedure, DECODER::UTIL::Donor& donor);
	void OutputDataMod(procedureDetail *procedure, DECODER::UTIL::Donor& donor, const int& procedureNum);
	
   void OutputCommonHeader (std::ostream& stream, std::string tbv, std::string ht, std::string wt);
   void OutputSubstateHeader (std::ostream& stream, std::string substate);
   void OutputCommonData (std::ostream& stream, procedureDetailPltPlsRBC& procedure, int procNum, DECODER::UTIL::Donor& donor, const std::string& gender);
   void OutputSubstateHeader (std::ostream& stream, std::string substate, IntermediateResults result);
   void OutputCommonData (std::ostream& stream, procedureDetailRBCPls& procedure, int procNum, DECODER::UTIL::Donor& donor, const std::string& gender);

   procedureDetail* PredictProcedure(int index);
private:
	

	PredictAnalyzer(void);
	PredictAnalyzer(std::ostream& outfile);
	~PredictAnalyzer(void);

	OfflineOptimizer *mOfflineOptimizerP;
	PredictRecordMrg *mPredictRecordMrgP;
	DECODER::UTIL::Donor mCurDonor;
	std::ostream& mStream;
};

} // namespace PREDICT

#endif // PREDICTANALYZER_H 

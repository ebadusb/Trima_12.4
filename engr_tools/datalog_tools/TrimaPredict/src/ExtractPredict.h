/*****************************************************************************\
**
** ExtractPredict.h
**
** Stack trace implementation.
**
\*****************************************************************************/

#ifndef EXTRACTPREDICT_H
#define EXTRACTPREDICT_H

/* Includes ******************************************************************/

#include <string>
#include <sstream>
#include <list>

#include "LogReader.h"
#include "LogExtractor.h"
#include "LogReaderUtil.h"
#include "PredictDataUtil.h"
#include "config_cds.h"

namespace PREDICT {

/* Classes *******************************************************************/

class ExtractPredict : public DECODER::LogExtractor {
public:
	typedef DECODER::UTIL::AdjustmentsMap AdjustmentsMap;
	typedef DECODER::UTIL::ProcDataMap ProcDataMap;
	typedef DECODER::UTIL::PredictDataArr PredictDataArr;
    typedef DECODER::UTIL::PredictDataMap PredictDataMap;
	typedef DECODER::UTIL::DonorStatsMap DonorStatsMap;
    typedef DECODER::UTIL::FeatureMap FeatureMap;
    typedef DECODER::RecordLogHeader LogHeader;
	enum {
		kResultDLogUnknownResult =  DECODER::LogReader::kResultLast 
	};

	typedef std::string String;

	/**
	 *
	 * ExtractPredict
	 *
	 * Constructor
	 *
	 */
	ExtractPredict(AdjustmentsMap &adjustments, 
			ProcDataMap &procData, PredictDataArr &predictDataArr, PredictDataMap &predictDataMap ,DonorStatsMap &donorStats, FeatureMap &featureMap, unsigned int& secondsStarted);

	/**
	 *
	 * Register
	 *
	 * Registers the necessary handlers with the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Register(DECODER::LogReader &reader);

	/**
	 *
	 * Unregister
	 *
	 * Unregisters the handlers from the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Unregister(DECODER::LogReader &reader);

	/**
	 *
	 * PreProcess
	 *
	 * Perform the procedures before the import process.
	 *
	 */
	virtual void PreProcess(void);

	/**
	 *
	 * PostProcess
	 *
	 * Perform the procedures after the import process.
	 *
	 */
	virtual void PostProcess(void);

	/**
	 *
	 * Extract
	 *
	 * Extracts the config data from the log file.
	 *
	 * @param logFile The log file to analyze.
	 * @param configFile The config file to write.
	 *
	 * @return The results.
	 *
	 */
	static DECODER::Result Extract(const String &logFile, AdjustmentsMap &adjustments, 
			ProcDataMap &procData, PredictDataArr &predictData, PredictDataMap &predictDataMap, DonorStatsMap &donorStats, 
            FeatureMap& featureMap, unsigned int& secondsStarted);

private:
	typedef DECODER::ProcessFilterLog<DECODER::RecordStreamOutput, DECODER::UTIL::ProcessPredictAdjustments> FilterPredictAdjustments;
	typedef DECODER::ProcessFilterLog<DECODER::RecordStreamOutput, DECODER::UTIL::ProcessPredictProcData> FilterPredictProcData;
	typedef DECODER::ProcessFilterLog<DECODER::RecordStreamOutput, DECODER::UTIL::ProcessPredictData> FilterPredictData;
	typedef DECODER::ProcessFilterLog<DECODER::RecordStreamOutput, DECODER::UTIL::ProcessPredictDonorStats> FilterPredictDonorStats;
	typedef DECODER::ProcessFilterLog<DECODER::RecordStreamOutput, DECODER::UTIL::ProcessSoftwareFeature> FilterSoftwareFeatures;
	typedef DECODER::ProcessFilterLog<DECODER::RecordStreamOutput, DECODER::UTIL::DatalogInitialization> FilterDlogInitialization;

	AdjustmentsMap &mAdjustmentsMap;
	ProcDataMap &mProcDataMap;
	PredictDataArr &mPredictDataArr;
    PredictDataMap &mPredictDataMap;
	DonorStatsMap &mDonorStatsMap;
    FeatureMap &mFeatureMap;

	DECODER::RecordLogLevel mPredictLogLevel;
	DECODER::ProcessGetLevelID mProcessGetPredictLevel;

	DECODER::RecordLogLevel mLogTrapLogLevel;
	DECODER::ProcessGetLevelID mProcessLogTrapLogLevel;

	DECODER::RecordLogLevel mstartupDetailLogLevel;
	DECODER::ProcessGetLevelID mProcessStartupDetailLogLevel;

	DECODER::UTIL::ProcessPredictAdjustments mProcessPredictAdjustments;
	DECODER::UTIL::ProcessPredictProcData mProcessPredictProcData;
	DECODER::UTIL::ProcessPredictData mProcessPredictData;
	DECODER::UTIL::ProcessPredictDonorStats mProcessPredictDonorStats;
    DECODER::UTIL::ProcessSoftwareFeature mProcessSoftwareFeatures;
    DECODER::UTIL::DatalogInitialization mDatalogInitialization;

	FilterPredictAdjustments mFilterPredictAdjustments;
	FilterPredictProcData mFilterPredictProcData;
	FilterPredictData mFilterPredictData;
	FilterPredictDonorStats mFilterPredictDonorStats;
    FilterSoftwareFeatures mFilterSoftwareFeatures;
    FilterDlogInitialization mFilterInitialization;
public:
    static LogHeader mLastFileProcessedHeader;
};

} // namespace PREDICT

#endif // EXTRACTCONFIG_H


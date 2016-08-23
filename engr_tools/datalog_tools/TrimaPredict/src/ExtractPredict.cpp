/*****************************************************************************\
**
** ExtractConfig.h
**
** Stack trace implementation.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <algorithm>
#include <iostream>

#include "ExtractPredict.h"
#include "LogExecute.h"

///this is the single instance of the recordlogheader
DECODER::RecordLogHeader DECODER::RecordLogHeader::LastRecordLogHeader;

namespace PREDICT {

/* Functions *****************************************************************/

/**
 *
 * ExtractPredict
 *
 * Constructor
 *
 */
ExtractPredict::ExtractPredict(AdjustmentsMap &adjustments, ProcDataMap &procData, 
        PredictDataArr &predictData, PredictDataMap &predictDataMap, DonorStatsMap &donorStats, FeatureMap &featureMap, unsigned int& secondsStarted) :
		mAdjustmentsMap(adjustments),
		mProcDataMap(procData),
		mPredictDataArr(predictData),
        mPredictDataMap(predictDataMap),
		mDonorStatsMap(donorStats),
        mFeatureMap(featureMap),
        mDatalogInitialization(secondsStarted),
		mProcessGetPredictLevel("predict_debug", mPredictLogLevel),
        mProcessLogTrapLogLevel("vxworks_info", mLogTrapLogLevel),
        mProcessStartupDetailLogLevel("startup_detail", mstartupDetailLogLevel),
		mProcessPredictAdjustments(mAdjustmentsMap),
		mProcessPredictProcData(mProcDataMap),
		mProcessPredictData(&predictDataMap, mPredictDataArr),
		mProcessPredictDonorStats(mDonorStatsMap),
        mProcessSoftwareFeatures(mFeatureMap),
		mFilterPredictAdjustments(mPredictLogLevel, mProcessPredictAdjustments),
		mFilterPredictProcData(mPredictLogLevel, mProcessPredictProcData),
		mFilterPredictData(mPredictLogLevel, mProcessPredictData),
		mFilterPredictDonorStats(mPredictLogLevel, mProcessPredictDonorStats),
        mFilterSoftwareFeatures(mLogTrapLogLevel, mProcessSoftwareFeatures),
        mFilterInitialization(mstartupDetailLogLevel, mDatalogInitialization)
{
}

/**
 *
 * Register
 *
 * Registers the necessary handlers with the reader.
 *
 * @param reader The file reader.
 *
 */
void ExtractPredict::Register(DECODER::LogReader &reader)
{
	reader.RegisterLogLevel(mProcessGetPredictLevel);
	reader.RegisterLogLevel(mProcessStartupDetailLogLevel);
    reader.RegisterStreamOutput(mFilterInitialization);
	reader.RegisterStreamOutput(mFilterPredictAdjustments);
	reader.RegisterStreamOutput(mFilterPredictProcData);
	reader.RegisterStreamOutput(mFilterPredictData);
	reader.RegisterStreamOutput(mFilterPredictDonorStats);
    reader.RegisterStreamOutput(mFilterPredictDonorStats);
   	reader.RegisterLogLevel(mProcessLogTrapLogLevel);
    reader.RegisterStreamOutput(mFilterSoftwareFeatures);
}

/**
 *
 * Unregister
 *
 * Unregisters the handlers from the reader.
 *
 * @param reader The file reader.
 *
 */
void ExtractPredict::Unregister(DECODER::LogReader &reader)
{
	reader.UnregisterLogLevel(mProcessGetPredictLevel);
	reader.UnregisterLogLevel(mProcessStartupDetailLogLevel);
	reader.UnregisterStreamOutput(mFilterPredictAdjustments);
	reader.UnregisterStreamOutput(mFilterPredictProcData);
	reader.UnregisterStreamOutput(mFilterPredictData);
	reader.UnregisterStreamOutput(mFilterPredictDonorStats);
    reader.UnregisterLogLevel(mProcessLogTrapLogLevel);
    reader.UnregisterStreamOutput(mFilterSoftwareFeatures);
    reader.UnregisterStreamOutput(mFilterInitialization);
}

/**
 *
 * PreProcess
 *
 * Perform the procedures before the import process.
 *
 */
void ExtractPredict::PreProcess(void)
{
	mAdjustmentsMap.clear();
	mProcDataMap.clear();
	mPredictDataArr.clear();
	mDonorStatsMap.clear();
    mFeatureMap.clear();
}

/**
 *
 * PostProcess
 *
 * Perform the procedures after the import process.
 *
 */
void ExtractPredict::PostProcess(void)
{
	mPredictLogLevel.mTimeSeconds = 0;
	mPredictLogLevel.mTimeNanosec = 0;
	mPredictLogLevel.mNodeID = 0;
	mPredictLogLevel.mLevelID = 0;
	mPredictLogLevel.mLevelName = "";
}

/**
 *
 * Extract
 *
 * Extracts the config data from the log file.
 *
 * @param logFile The log file to analyze.
 * @param config The config CDS to update.
 *
 * @return The results.
 *
 */
DECODER::Result ExtractPredict::Extract(const String &logFile, AdjustmentsMap &adjustments, 
			ProcDataMap &procData, PredictDataArr &predictData, PredictDataMap &predictDataMap, DonorStatsMap &donorStats, FeatureMap& featureMap, unsigned int& secondsStarted)
{
	ExtractPredict extractor(adjustments, procData, predictData, predictDataMap, donorStats, featureMap, secondsStarted );

	return DECODER::ProcessLogData(logFile.c_str(), extractor);
}

} // namespace PREDICT


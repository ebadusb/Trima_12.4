/*****************************************************************************\
**
** TrimaAnalysis.h
**
** Extracts the analysis data from a datalog file.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <algorithm>

#include "TrimaAnalysis.h"
#include "LogExecute.h"

namespace ANALYSIS {

/* Functions *****************************************************************/

/**
 *
 * Register
 *
 * Registers the necessary handlers with the reader.
 *
 * @param reader The file reader.
 *
 */
void TrimaAnalysis::Register(DECODER::LogReader &reader)
{
	reader.RegisterLogLevel(mProcessGetSafeExecInfo);
	reader.RegisterStreamOutput(mFilterLogData);
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
void TrimaAnalysis::Unregister(DECODER::LogReader &reader)
{
	reader.UnregisterLogLevel(mProcessGetSafeExecInfo);
	reader.UnregisterStreamOutput(mFilterLogData);
}

/**
 *
 * PreProcess
 *
 * Perform the procedures before the import process.
 *
 */
void TrimaAnalysis::PreProcess(void)
{
	mTimeDataList.clear();
}

/**
 *
 * PostProcess
 *
 * Perform the procedures after the import process.
 *
 */
void TrimaAnalysis::PostProcess(void)
{
	if (mTimeDataList.size() > 100) {
		TimeDataList::const_iterator iter = mTimeDataList.begin();
		TimeDataList::const_reverse_iterator last = mTimeDataList.rbegin();
		TimeDataList::const_iterator end = mTimeDataList.end();

		double reference = iter->safeTime;
		double beginTimeDiff = iter->safeTime - iter->procTime;
		double endTimeDiff = last->safeTime - last->procTime;
		double totalTimeSpan = last->safeTime - iter->safeTime;
		double driftRate = (endTimeDiff - beginTimeDiff) / (totalTimeSpan);
		double timeOffset = beginTimeDiff > 0 ? 0 : beginTimeDiff * -2;

		for (; iter != end; ++iter) {
			mStream << iter->procTime << "," 
					<< iter->safeTime << ","
					<< iter->safeTime - reference << ","
					<< iter->safeTime - iter->procTime << ","
					<< driftRate * (iter->safeTime - reference) << ","
					<< timeOffset + (iter->safeTime - iter->procTime) - (driftRate * (iter->safeTime - reference))
					<< std::endl;
		}
	}
}

/**
 *
 * ExtractLogData
 *
 * Utility function that extracts the message data to a file.
 *
 * @param logFile The filename of the log file.
 * @param datFile The filename of the data file.
 *
 * @return The results. 
 *
 */
DECODER::Result TrimaAnalysis::ExtractLogData(const DECODER::String &logFile, const DECODER::String &datFile)
{
	DECODER::Result results = DECODER::LogReader::kResultNone;

	std::ofstream file;
	file.open(datFile.c_str());
	results = DECODER::ProcessLogData(logFile.c_str(), TrimaAnalysis(file));
	file.flush();
	file.close();

	return results;
}

} // namespace ANALYSIS


/*****************************************************************************\
**
** DatalogAnalysis.h
**
** Extracts the analysis data from a datalog file.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <algorithm>

#include "DatalogAnalysis.h"
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
void DatalogAnalysis::Register(DECODER::LogReader &reader)
{
	reader.RegisterStreamOutput(mProcessLogData);
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
void DatalogAnalysis::Unregister(DECODER::LogReader &reader)
{
	reader.UnregisterStreamOutput(mProcessLogData);
}

/**
 *
 * PreProcess
 *
 * Perform the procedures before the import process.
 *
 */
void DatalogAnalysis::PreProcess(void)
{
}

/**
 *
 * PostProcess
 *
 * Perform the procedures after the import process.
 *
 */
void DatalogAnalysis::PostProcess(void)
{
}

/**
 *
 * ExtractLogData
 *
 * Utility function that extracts the message data to a file.
 *
 * @param logFile The filename of the log file.
 * @param scanList The list of logs to scan.
 * @param datFile The filename of the data file.
 *
 * @return The results. 
 *
 */
DECODER::Result DatalogAnalysis::ExtractLogData(
		const DECODER::String &logFile, 
		const Scanlist &scanList,
		const DECODER::String &datFile)
{
	DECODER::Result results = DECODER::LogReader::kResultNone;

	std::ofstream file;
	file.open(datFile.c_str());
	results = DECODER::ProcessLogData(logFile.c_str(), DatalogAnalysis(file, scanList));
	file.flush();
	file.close();

	return results;
}

} // namespace ANALYSIS


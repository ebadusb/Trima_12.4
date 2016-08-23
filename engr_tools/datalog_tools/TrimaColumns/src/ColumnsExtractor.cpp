/*****************************************************************************\
**
** ColumnsExtractor.h
**
** Extracts the column names from a datalog file.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <algorithm>

#include "ColumnsExtractor.h"
#include "LogExecute.h"

namespace COLUMNS {

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
void ColumnsExtractor::Register(LogReader &reader)
{
	reader.RegisterPeriodicSet(mProcessGetPeriodicSets);
	reader.RegisterPeriodicSetItem(mProcessGetPeriodicItems);
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
void ColumnsExtractor::Unregister(LogReader &reader)
{
	reader.UnregisterPeriodicSet(mProcessGetPeriodicSets);
	reader.UnregisterPeriodicSetItem(mProcessGetPeriodicItems);
}

/**
 *
 * PreProcess
 *
 * Perform the procedures before the import process.
 *
 */
void ColumnsExtractor::PreProcess(void)
{
	mPeriodicSets.clear();
	mPeriodicItems.clear();
}

/**
 *
 * PostProcess
 *
 * Perform the procedures after the import process.
 *
 */
void ColumnsExtractor::PostProcess(void)
{
	PeriodicItems::iterator iter = mPeriodicItems.begin();
	PeriodicItems::iterator end = mPeriodicItems.end();

	for (; iter != end; ++iter) {
		(*iter).set = mPeriodicSets[(*iter).node];
	}
}

/**
 *
 * ImportLogData
 *
 * Inports the data from the data log file.
 *
 * @param filename The filename of the data log file.
 *
 */
ColumnsExtractor::Result ColumnsExtractor::ImportLogData(const char *filename, PeriodicItems &set)
{
	return DECODER::ProcessLogData(filename, ColumnsExtractor(set));
}

} // namespace COLUMNS


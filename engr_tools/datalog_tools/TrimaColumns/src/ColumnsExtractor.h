/*****************************************************************************\
**
** ColumnsExtractor.h
**
** Extracts the column names from a datalog file.
**
\*****************************************************************************/

#ifndef COLUMNSEXTRACTOR_H
#define COLUMNSEXTRACTOR_H

/* Includes ******************************************************************/

#include <string>
#include <set>

#include "LogReader.h"
#include "LogExtractor.h"
#include "ColumnDataUtil.h"

namespace COLUMNS {

/* Classes *******************************************************************/

class ColumnsExtractor : public DECODER::LogExtractor {
public:
	typedef DECODER::LogReader LogReader;
	typedef DECODER::Result Result;
	typedef DECODER::UTIL::PeriodicItems PeriodicItems;

	ColumnsExtractor(PeriodicItems &set) :
			mPeriodicItems(set),
			mProcessGetPeriodicSets(mPeriodicSets),
			mProcessGetPeriodicItems(mPeriodicItems) {}

	/**
	 *
	 * Register
	 *
	 * Registers the necessary handlers with the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Register(LogReader &reader);

	/**
	 *
	 * Unregister
	 *
	 * Unregisters the handlers from the reader.
	 *
	 * @param reader The file reader.
	 *
	 */
	virtual void Unregister(LogReader &reader);

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
	 * ImportLogData
	 *
	 * Utility function that extracts the columns from a datalog file.
	 *
	 * @param filename The filename of the data log file.
	 * @param set The column name set.
	 *
	 * @return The results. 
	 *
	 */
	static Result ImportLogData(const char *filename, PeriodicItems &set);

private:
	DECODER::UTIL::PeriodicSets mPeriodicSets;
	DECODER::UTIL::PeriodicItems &mPeriodicItems;
	DECODER::UTIL::ProcessGetPeriodicSets mProcessGetPeriodicSets;
	DECODER::UTIL::ProcessGetPeriodicItems mProcessGetPeriodicItems;
};

} // namespace COLUMNS

#endif // COLUMNSEXTRACTOR_H


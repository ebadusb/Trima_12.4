/*****************************************************************************\
**
** DatalogAnalysis.h
**
** Extracts the analysis data from a datalog file.
**
\*****************************************************************************/

#ifndef DATALOGANALYSIS_H
#define DATALOGANALYSIS_H

/* Includes ******************************************************************/

#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>

#include "LogReader.h"
#include "LogExtractor.h"
#include "ColumnDataUtil.h"

namespace ANALYSIS {

/* Types *********************************************************************/

typedef std::ostream Stream;
typedef std::string String;
typedef std::vector<std::string> Scanlist;
typedef std::vector<std::string> Findlist;

/* Classes *******************************************************************/

class ProcessLogData {
public:
	ProcessLogData(Stream &stream, const Scanlist &scanList) 
			: mStream(stream), mScanList(scanList), mIndex(0)
	{
		Scanlist::const_iterator iter = mScanList.begin();
		Scanlist::const_iterator end = mScanList.end();

		for (; iter != end; ++iter) {
			mFindlist.push_back((*iter).substr(0, (*iter).find('%')));
		}
	}

	void operator()(const DECODER::RecordStreamOutput &record)
	{
		if (record.mMessage.find(mFindlist[mIndex]) != std::string::npos) {
			const char *text = record.mMessage.c_str();
			const char *scan = mScanList[mIndex++].c_str();
			bool addComma = false;

			if (mIndex == 1) {
				mStream << float(record.mTimeSeconds + record.mTimeNanosec / 1000000000.0F) << ", ";		
			}

			while (text != NULL && scan != NULL && *text != NULL && *scan != NULL) {
				if (*text == *scan) {
					++text;
					++scan;
				}
				else if (*scan == '%') {
					++scan;

					switch (*scan) {
					case 'c':
					case 'i':
					case 'f':
					case 's':
						if (addComma) {
							mStream << ", ";
						}

						while (*text != NULL && !std::isspace(*text)) {
							mStream << *(text++);
						}

						addComma = true;
						break;
					case 'x':
						while (*text != NULL && !std::isspace(*text)) {
							text++;
						}
					}

					++scan;
				}
				else {
					text = NULL;
					scan = NULL;
				}
			}

			if (mIndex < mScanList.size()) {
				mStream << ", ";
			}
			else if (mIndex == mScanList.size()) {
				mStream << std::endl;
				mIndex = 0;
			}
		}
	}

private:
	ProcessLogData(const ProcessLogData &o);

	Stream &mStream;
	Scanlist mScanList;
	Findlist mFindlist;

	int mIndex;
};

class DatalogAnalysis : public DECODER::LogExtractor {
public:
	DatalogAnalysis(Stream &stream, const Scanlist &scanList) : mProcessLogData(stream, scanList)
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
	static DECODER::Result ExtractLogData(
			const DECODER::String &logFile, 
			const Scanlist &scanList, 
			const DECODER::String &datFile);

private:
	ProcessLogData mProcessLogData;
};

} // namespace ANALYSIS

#endif // DATALOGANALYSIS_H


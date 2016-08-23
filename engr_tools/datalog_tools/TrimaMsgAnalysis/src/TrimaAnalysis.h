/*****************************************************************************\
**
** TrimaAnalysis.h
**
** Extracts the analysis data from a datalog file.
**
\*****************************************************************************/

#ifndef TRIMAANALYSIS_H
#define TRIMAANALYSIS_H

/* Includes ******************************************************************/

#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <vector>

#include "LogReader.h"
#include "LogExtractor.h"
#include "ColumnDataUtil.h"

namespace ANALYSIS {

/* Constants *****************************************************************/

static const char *kMsgText = "MSG ";
static const char *kProcText = "Proc:";
static const char *kSafeText = "Safe:";
static const int kProcLength = 5;
static const int kSafeLength = 5;

/* Types *********************************************************************/

struct TimeData {
	double procTime;
	double safeTime;
};

typedef std::vector<TimeData> TimeDataList;

/* Classes *******************************************************************/

class ProcessLogData {
public:
	ProcessLogData(TimeDataList &list) : mTimeDataList(list) {}

	void operator()(const DECODER::RecordStreamOutput &record)
	{
		if (record.mMessage.find(kMsgText) == 0) {
			TimeData data = { 0 };

			int procIndex = record.mMessage.find(kProcText) + kProcLength + 1;
			int safeIndex = record.mMessage.find(kSafeText) + kSafeLength + 1;
			int procLength = safeIndex - procIndex - kSafeLength - 2;
			int safeLength = record.mMessage.length() - safeIndex;	

			data.procTime = this->GetTime(record.mMessage.substr(procIndex, procLength));
			data.safeTime = this->GetTime(record.mMessage.substr(safeIndex, safeLength));

			mTimeDataList.push_back(data);
		}
	}

	double GetTime(const DECODER::String &text)
	{
		int index = text.find(':');

		long sec = std::atol(text.substr(0, index).c_str());
		long nano = std::atol(text.substr(index + 1).c_str());

		return sec + (nano / 1000000000.0);
	}

private:
	TimeDataList &mTimeDataList;

private:
	ProcessLogData(const ProcessLogData &o);
};

class TrimaAnalysis : public DECODER::LogExtractor {
public:
	typedef std::ostream Stream;

	TrimaAnalysis(Stream &stream) :
			mStream(stream),
			mProcessLogData(mTimeDataList),
			mProcessGetSafeExecInfo("safe_exec_info", mLevelSafeExecInfo),
			mFilterLogData(mLevelSafeExecInfo, mProcessLogData) 
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
	 * @param datFile The filename of the data file.
	 *
	 * @return The results. 
	 *
	 */
	static DECODER::Result ExtractLogData(const DECODER::String &logFile, const DECODER::String &datFile);

private:
	typedef DECODER::ProcessFilterLog<DECODER::RecordStreamOutput, ProcessLogData> FilterLogData;

	Stream &mStream;

	TimeDataList mTimeDataList;

	DECODER::RecordLogLevel mLevelSafeExecInfo;
	DECODER::ProcessGetLevelID mProcessGetSafeExecInfo;

	FilterLogData mFilterLogData;
	ProcessLogData mProcessLogData;
};

} // namespace ANALYSIS

#endif // TRIMAANALYSIS_H


/*****************************************************************************\
**
** ExtractConfig.h
**
** Stack trace implementation.
**
\*****************************************************************************/

#ifndef EXTRACTCONFIG_H
#define EXTRACTCONFIG_H

/* Includes ******************************************************************/

#include <string>
#include <sstream>
#include <list>

#include "LogReader.h"
#include "LogExtractor.h"
#include "LogReaderUtil.h"
#include "columndatautil.h"

class Config_CDS;

namespace CONFIG {

/* Classes *******************************************************************/

class ExtractConfig : public DECODER::LogExtractor {
public:
	typedef DECODER::LogReader LogReader;
	typedef DECODER::Result Result;
	typedef std::string String;

	enum {
		kResultNone = DECODER::LogReader::kResultNone,
		kResultDLogFileAlreadyOpen = DECODER::LogReader::kResultFileAlreadyOpen,
		kResultDLogFileNotOpen = DECODER::LogReader::kResultFileNotOpen,
		kResultDLogFileOpenFailed = DECODER::LogReader::kResultFileOpenFailed,
		kResultDLogInvalidHeaderType = DECODER::LogReader::kResultInvalidHeaderType,
		kResultDLogInvalidVersion = DECODER::LogReader::kResultInvalidVersion,
		kResultDLogUnexpectedEOF = DECODER::LogReader::kResultUnexpectedEOF,
		kResultDLogUnknownRecordType = DECODER::LogReader::kResultUnknownRecordType,
		kResultDLogUnknownResult
	};

	/**
	 *
	 * ExtractConfig
	 *
	 * Constructor
	 *
	 * @param config The config CDS.
	 *
	 */
	ExtractConfig(Config_CDS &config);

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
	static Result Extract(const String &logFile, Config_CDS &config);

private:
	typedef DECODER::RecordPeriodicSet RecordPeriodicSet;
	typedef std::list<DECODER::RecordPeriodicSetItem> PeriodicSetItemList;
	typedef std::list<DECODER::RecordPeriodicLogOutput> PeriodicLogOutputList;

	typedef DECODER::ProcessSet<RecordPeriodicSet> ProcessPeriodicSet;
	typedef DECODER::ProcessInclude<DECODER::RecordPeriodicSetItem, PeriodicSetItemList> ProcessPeriodicSetItem;
	typedef DECODER::ProcessInclude<DECODER::RecordPeriodicLogOutput, PeriodicLogOutputList> ProcesseriodicLogOutput;
	typedef DECODER::UTIL::FilterPeriodicSetByName<ProcessPeriodicSet> FilterPeriodicSet;
	typedef DECODER::UTIL::FilterPeriodicSetItemByID<ProcessPeriodicSetItem> FilterPeriodicSetItem;
	typedef DECODER::UTIL::FilterPeriodicLogOutputByID<ProcesseriodicLogOutput> FilterPeriodicLogOutput;
	typedef DECODER::UTIL::FilterPeriodicLogOutputBySetID<FilterPeriodicLogOutput> FilterPeriodicLogOutputBySetID;

	struct ColumnData {
		RecordPeriodicSet configDataRegister;
		PeriodicSetItemList periodicSetItemList;
		PeriodicLogOutputList periodicLogOutputList;
	};

	/**
	 *
	 * Assign
	 *
	 * Assigns the value with hte given name to the address.
	 *
	 * @param data The data structure.
	 * @param name The data name.
	 * @param address The location to copy.
	 *
	 */
	static void Assign(const ColumnData &data, const String &name, void *address);

	Config_CDS &mConfigCDS;
	ColumnData mColumnData;

	ProcessPeriodicSet mProcessPeriodicSet;
	ProcessPeriodicSetItem mProcessPeriodicSetItem;
	ProcesseriodicLogOutput mProcesseriodicLogOutput;

	FilterPeriodicSet mFilterPeriodicSet;
	FilterPeriodicSetItem mFilterPeriodicSetItem;
	FilterPeriodicLogOutput mFilterPeriodicLogOutput;
	FilterPeriodicLogOutputBySetID mFilterPeriodicLogOutputBySetID;
};

} // namespace CONFIG

#endif // EXTRACTCONFIG_H


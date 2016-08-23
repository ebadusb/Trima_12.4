/*****************************************************************************\
**
** Main.cpp
**
** Extracts the analysis data from a datalog file.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <iostream>
#include <map>

#include "LogReader.h"
#include "DatalogAnalysis.h"
#include "AppParamUtil.h"

/* Constants *****************************************************************/

static const char *kStrErrorHelp = "DatalogAnalysis datalog -scan 'text' output";
static const char *kStrErrorFailed = "Error: Failed to open file.";
static const char *kStrErrorInvalid = "Error: Invalid header type.";
static const char *kStrErrorUnsupported = "Error: Log file version is unsupported.";
static const char *kStrErrorUnknown = "Error: Unknown record type.";
static const char *kStrErrorEndOfFile = "Error: Unexpected end of file.";

/* Functions *****************************************************************/

int main(int argc, char* argv[])
{
	ANALYSIS::Scanlist scanlist;
	DECODER::Result results = DECODER::LogReader::kResultNone;
	DECODER::UTIL::Arguments params;
	DECODER::UTIL::AppParamUtil::Parse(params, argv + 1, argc - 1);
	DECODER::UTIL::AppParamUtil::Extract(params, "-scan", scanlist);
	DECODER::String logfile = DECODER::UTIL::AppParamUtil::Get(params, "", 0);
	DECODER::String outfile = DECODER::UTIL::AppParamUtil::Get(params, "", 1);

	if (DECODER::UTIL::AppParamUtil::Count(params, "") != 2
			|| DECODER::UTIL::AppParamUtil::Count(params, "-scan") < 1) {
		std::cout << kStrErrorHelp << std::endl;
		exit(0);
	}

	results = ANALYSIS::DatalogAnalysis::ExtractLogData(logfile, scanlist, outfile);

	switch (results) {
	case DECODER::LogReader::kResultFileOpenFailed:
		std::cout << kStrErrorFailed << std::endl;
		break;
	case DECODER::LogReader::kResultUnexpectedEOF:
		std::cout << kStrErrorEndOfFile << std::endl;
		break;
	case DECODER::LogReader::kResultUnknownRecordType:
		std::cout << kStrErrorUnknown << std::endl;
		break;
	case DECODER::LogReader::kResultInvalidHeaderType:
		std::cout << kStrErrorInvalid << std::endl;
		break;
	case DECODER::LogReader::kResultInvalidVersion:
		std::cout << kStrErrorUnsupported << std::endl;
		break;
	}

	return results;
}

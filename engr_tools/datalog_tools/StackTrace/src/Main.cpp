/*****************************************************************************\
**
** Main.cpp
**
** Application entry point.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <iostream>

#include "LogReader.h"
#include "StackTrace.h"

/* Constants *****************************************************************/

static const char *kStrErrorHelp = 
		"StackTrace version 1.9\n"
		"Creates a crash report from the data found in a given datalog.\n"
		"StackTrace [-location buildpath] [-version (2.02 | 2.20)] [-osfile filename] [-ospath pathnames] [-pathalias old[:new]] [-address values] filename\n";
static const char *kStrErrorFailed = "Error: Failed to open file.";
static const char *kStrErrorInvalid = "Error: Invalid header type.";
static const char *kStrErrorUnsupported = "Error: Log file version is unsupported.";

/* Functions *****************************************************************/

int main(int argc, char* argv[])
{
	std::ostringstream output;
	DECODER::Result results = DECODER::LogReader::kResultNone;
	TRACE::StackTrace::Arguments arguments;

	if (argc < 2) {
		std::cerr << kStrErrorHelp << std::endl;
		exit(0);
	}

	for (int i = 1; i < argc - 1; ++i) {
		if (((i != argc - 2) && *argv[i] == '-') && (*argv[i + 1] != '-')) {
			arguments[argv[i]] = argv[++i];
		}
		else if (*argv[i] == '-') {
			arguments[argv[i]] = "";
		}
		else {
			std::cerr << kStrErrorHelp << std::endl;
			exit(0);
		}
	}

	results = TRACE::StackTrace::CreateCrashReport(argv[argc - 1], arguments, output);

	switch (results) {
	case DECODER::LogReader::kResultNone:
	case DECODER::LogReader::kResultUnexpectedEOF:
	case DECODER::LogReader::kResultUnknownRecordType:
		std::cout << output.str() << std::endl;
		break;
	case DECODER::LogReader::kResultFileOpenFailed:
		std::cerr << kStrErrorFailed << std::endl;
		break;
	case DECODER::LogReader::kResultInvalidHeaderType:
		std::cerr << kStrErrorInvalid << std::endl;
		break;
	case DECODER::LogReader::kResultInvalidVersion:
		std::cerr << kStrErrorUnsupported << std::endl;
		break;
	}

	return results;
}

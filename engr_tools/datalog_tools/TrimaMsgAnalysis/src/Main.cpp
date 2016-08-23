/*****************************************************************************\
**
** Main.cpp
**
** Extracts the analysis data from a datalog file.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <iostream>

#include "LogReader.h"
#include "TrimaAnalysis.h"

/* Constants *****************************************************************/

static const char *kStrErrorHelp = "MessageAnalysis datalog output";
static const char *kStrErrorFailed = "Error: Failed to open file.";
static const char *kStrErrorInvalid = "Error: Invalid header type.";
static const char *kStrErrorUnsupported = "Error: Log file version is unsupported.";
static const char *kStrErrorUnknown = "Error: Unknown record type.";
static const char *kStrErrorEndOfFile = "Error: Unexpected end of file.";
static const char *kStrInfo = "Reading datalog file: ";
static const char *kStrSuccess = "Writing config file: ";

/* Functions *****************************************************************/

int main(int argc, char* argv[])
{
	if ( argc != 3 ) {
		std::cout << kStrErrorHelp << std::endl;
		exit(0);
	}

	DECODER::Result results = DECODER::LogReader::kResultNone;

	results = ANALYSIS::TrimaAnalysis::ExtractLogData(argv[1], argv[2]);

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

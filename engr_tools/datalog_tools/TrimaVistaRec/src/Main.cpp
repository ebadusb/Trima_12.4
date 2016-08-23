/*****************************************************************************\
**
** Main.cpp
**
** Extracts the vista binary records from a datalog file.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <iostream>

#include "LogReader.h"
#include "VistaRecExtractor.h"

/* Constants *****************************************************************/

static const char *kStrErrorHelp = "VistaRecExtractor datalog";
static const char *kStrErrorFailed = "Error: Failed to open file.";
static const char *kStrErrorInvalid = "Error: Invalid header type.";
static const char *kStrErrorUnsupported = "Error: Log file version is unsupported.";
static const char *kStrErrorUnknown = "Error: Unknown record type.";
static const char *kStrErrorEndOfFile = "Error: Unexpected end of file.";

/* Functions *****************************************************************/

int main(int argc, char* argv[])
{
	if ( argc != 2 ) {
		std::cout << kStrErrorHelp << std::endl;
		exit(0);
	}

	DECODER::Result results =  VISTAREC::VistaRecExtractor::ImportLogData(argv[1], std::cout);

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

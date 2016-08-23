/*****************************************************************************\
**
** Main.cpp
**
** Extracts the datalog data into an database using ADO.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#import <c:\\Program Files\\Common Files\\system\\ado\\msadox.dll>

#include <sys/stat.h> 

#include "LogReader.h"
#include "DatabaseConverter.h"

/* Constants *****************************************************************/

static const char *kStrErrorHelp = "DatabaseConverter datalog database";
static const char *kStrErrorFailed = "Error: Failed to open file.";
static const char *kStrErrorInvalid = "Error: Invalid header type.";
static const char *kStrErrorUnsupported = "Error: Log file version is unsupported.";
static const char *kStrErrorUnknown = "Error: Unknown record type.";
static const char *kStrErrorEndOfFile = "Error: Unexpected end of file.";

/* Functions *****************************************************************/

int main(int argc, char* argv[])
{
	if (argc != 3) {
		std::cout << kStrErrorHelp << std::endl;
		exit(0);
	}

	CoInitialize(NULL);

	DECODER::Result results = CONVERT::DatabaseConverter::ExtractLogData(argv[1], argv[2]);

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

	CoUninitialize();

	return results;
}

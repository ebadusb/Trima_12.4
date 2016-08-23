/*****************************************************************************\
**
** Main.cpp
**
** Extracts the column names from a datalog file.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <iostream>

#include "LogReader.h"
#include "ColumnsExtractor.h"

/* Constants *****************************************************************/

static const char *kStrErrorHelp = "ColumnsExtractor datalog";
static const char *kStrErrorFailed = "Error: Failed to open file.";
static const char *kStrErrorInvalid = "Error: Invalid header type.";
static const char *kStrErrorUnsupported = "Error: Log file version is unsupported.";
static const char *kStrInfo = "Reading datalog file: ";
static const char *kStrSuccess = "Writing config file: ";

/* Functions *****************************************************************/

int main(int argc, char* argv[])
{
	if ( argc != 2 ) {
		std::cout << kStrErrorHelp << std::endl;
		exit(0);
	}

	DECODER::Result results = DECODER::LogReader::kResultNone;
	COLUMNS::ColumnsExtractor::PeriodicItems items;

	results = COLUMNS::ColumnsExtractor::ImportLogData(argv[1], items);

	switch (results) {
	case DECODER::LogReader::kResultFileOpenFailed:
	case DECODER::LogReader::kResultUnexpectedEOF:
	case DECODER::LogReader::kResultUnknownRecordType:
		std::cout << kStrErrorFailed << std::endl;
		break;
	case DECODER::LogReader::kResultInvalidHeaderType:
		std::cout << kStrErrorInvalid << std::endl;
		break;
	case DECODER::LogReader::kResultInvalidVersion:
		std::cout << kStrErrorUnsupported << std::endl;
		break;
	}

	COLUMNS::ColumnsExtractor::PeriodicItems::const_iterator iter = items.begin();
	COLUMNS::ColumnsExtractor::PeriodicItems::const_iterator end = items.end();

	for (; iter != end; ++iter) {
		std::cout.setf(std::ios::left);
		std::cout.width(20);
		std::cout << (*iter).set;
		std::cout << " ";
		std::cout.width(30);
		std::cout << (*iter).name;
		std::cout << " ";
		std::cout << (*iter).info;
		std::cout << std::endl;
	}

	return results;
}

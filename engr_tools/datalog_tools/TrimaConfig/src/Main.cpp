/*****************************************************************************\
**
** Main.cpp
**
** Test application for DLog reader.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <iostream>

#include "LogReader.h"
#include "ExtractConfig.h"
#include "config_cds.h"

/* Constants *****************************************************************/

static const char *kStrErrorHelp = "ExtractConfig datalog configfile";
static const char *kStrErrorFailed = "Error: Failed to open file.";
static const char *kStrErrorInvalid = "Error: Invalid header type.";
static const char *kStrErrorUnsupported = "Error: Log file version is unsupported.";
static const char *kStrInfo = "Reading datalog file: ";
static const char *kStrSuccess = "Writing config file: ";
static const char *kStrFailure = "Failed to write config file: ";

/* Functions *****************************************************************/

int main(int argc, char* argv[])
{
	if (argc != 3) {
		std::cerr << kStrErrorHelp << std::endl;
		exit(0);
	}

	std::cout << kStrInfo << argv[1] << std::endl;

	Config_CDS config(ROLE_RW);
	CONFIG::ExtractConfig::Result results = CONFIG::ExtractConfig::Extract(argv[1], config);

	switch (results) {
	case CONFIG::ExtractConfig::kResultNone:
	case CONFIG::ExtractConfig::kResultDLogUnexpectedEOF:
	case CONFIG::ExtractConfig::kResultDLogUnknownRecordType:
		if (config.WriteConfig(argv[2])) {
			std::cout << kStrSuccess << argv[2] << std::endl;
		}
		else {
			std::cout << kStrFailure << argv[2] << std::endl;
		}
		break;
	case CONFIG::ExtractConfig::kResultDLogFileOpenFailed:
		std::cerr << kStrErrorFailed << std::endl;
		break;
	case CONFIG::ExtractConfig::kResultDLogInvalidHeaderType:
		std::cerr << kStrErrorInvalid << std::endl;
		break;
	case CONFIG::ExtractConfig::kResultDLogInvalidVersion:
		std::cerr << kStrErrorUnsupported << std::endl;
		break;
	}

	return results;
}

#include <cstdlib>
#include <iostream>
#include <cstdio>
#include "offline_optimizer.h"
#include "toplevel.h"
#include "sharedstruct.h"
#include "datalog.h"
#include <GetOpt.h>
#include "analyze.h"

#include "configdat_utils.h"

extern void (*ProcessingFnPtr)(std::ofstream&, Offline_optimizer*, init::Donor&);
extern ProcessBatch(const char*, const char*, const char*, const char*,
					 void (*ProcessingFnPtr)(std::ofstream&, Offline_optimizer*, init::Donor&));
extern void FirstValid(std::ofstream&, Offline_optimizer*, init::Donor&);
extern void AmapPlatelets(std::ofstream&, Offline_optimizer*, init::Donor&); 
void printHelp(void);

const char* option_str = "a:s:q";

int main(int argc, char* argv[])
{  
	//DataLog_Level dl;

	//GetOpt getOpt(argc, argv, option_str);
	if(argc == 1) {
		printHelp();
		exit(0);
	}
	char option_char = argv[1][0];           
	cout << "Option Char: " << option_char << endl;
	Analyzer* analyzer;
	// Invokes member function `int operator ()(void);'       
	//while ((option_char = getopt ()) != EOF)
	switch (option_char)
	{  
		case 'a': /* Analyze */
		{
			if(argc != 6) {
			   cout << "usage: predict a dlog cobe_config procedureNum minsIntoRun" << endl;
			   exit(0);
			}
		   analyzer = new Analyzer(argv[2], argv[3]);
		   analyzer->AnalyzeRun(atoi(argv[4]), atof(argv[5]));
		}
		break;
	case 'm': /* Amap Platelets */ 
		{ 
			if(argc != 6) {
			   cout << "usage: predict b donor_pool config_dat cobe_config output_file" << endl;
			   exit(0);
			}
			ProcessBatch(argv[2], argv[3], argv[4], argv[5], &AmapPlatelets);
		}
		break;

	case 'c':
		{
			if (argc != 4) {
				cout << "usage: predict c dlog config_dat" << endl;
				exit(0);
			}
			ConfigDatUtils::CreateConfigFileFromDlog(argv[2], argv[3]);
		}
		break;
	case 'p':
		{
			if(argc != 4) {
				   cout << "usage: predict p dlog cobe_config" << endl;
				   exit(0);
			}

			analyzer = new Analyzer(argv[2], argv[3]);
			analyzer->RepredictAll();
		}
		break;
	case 's': 
		{ 
			if(argc != 6) {
			   cout << "usage: predict s donor_pool config_dat cobe_config output_file" << endl;
			   exit(0);
			}
			ProcessBatch(argv[2], argv[3], argv[4], argv[5], &FirstValid);

		}
		break;
	default:
		{
			printHelp();
			exit(2);
		}
    }
    return 0;
}

void printHelp(void)
{
	cout << "Use one of the following :" << endl;
	cout << "\tPredict a procedure:\n\tpredict a dlog cobe_config procedureNum minsIntoRun" << endl;
	cout << "\n\tExtract config from dlog:\n\tpredict c dlog config_dat" << endl;
	cout << "\n\tAmap Platelets:\n\tpredict m donor_pool config_dat cobe_config output_file" << endl;
	cout << "\n\tPredict all procedures:\n\tpredict p dlog cobe_config" << endl;
	cout << "\n\tRun a simulation:\n\tpredict s donor_pool config_dat cobe_config output_file" << endl;
}




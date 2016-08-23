#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <vxworks.h>

#include "OfflineOptimizer.h"
#include "sharedstruct.h"
#include "PredictAnalyzer.h"

void printHelp(void);

int main(int argc, char* argv[])
{  

    if(argc == 1) {
        printHelp();
        exit(0);
    }

    char option_char = argv[1][0];           
    PREDICT::PredictAnalyzer* analyzer;

    //Load sw.dat if present -> move to CreateOptimizer* functions in offline_optimizer
    //trimaSysLoadOptionFiles();
    //Software_CDS::Read("sw.dat");

    switch (option_char) {  
        case 'a':
        {
           
            
            if(argc != 6) {
               std::cout << "usage: predict a dlog cobe_config procedureNum <YYYY/MM/DD_HH:mm:SEC.xxx>" << std::endl;
               exit(0);
            }

            PREDICT::PredictAnalyzer::CreateAnalyzerFromLog(argv[2], argv[3], &analyzer);
            if (analyzer != NULL) {
               analyzer->AnalyzeSingleRun(atoi(argv[4]), std::string(argv[5]));
            }
            PREDICT::PredictAnalyzer::DestroyAnalyzer(analyzer);
        }
        break;
        case 'b':
        {
            if(argc != 7) {
               std::cout << "usage: predict b dlog cobe_config features.bin procedureNum <YYYY/MM/DD_HH:mm:SEC.xxx>" << std::endl;
               exit(0);
            }

            PREDICT::PredictAnalyzer::CreateAnalyzerFromLog(argv[2], argv[3], &analyzer);
            if (analyzer != NULL) {
               analyzer->AnalyzeSingleRun(atoi(argv[5]), std::string(argv[6]), argv[4]);
            }
            PREDICT::PredictAnalyzer::DestroyAnalyzer(analyzer);
        }
		break;
/*	case 'm':
        { 
            if(argc != 5) {
               std::cout << "usage: predict m donor_pool config_dat cobe_config" << std::endl;
               exit(0);
            }

            PREDICT::PredictAnalyzer::CreateAnalyzerFromConfig(argv[3], argv[4], &analyzer);
            if (analyzer != NULL) {
                analyzer->AnalyzeAmapPlatelets(std::cout, argv[2]);
            }
            PREDICT::PredictAnalyzer::DestroyAnalyzer(analyzer);
        }
        break;
*/
    case 'p':
        {
            if(argc != 4) {
                   std::cout << "usage: predict p dlog cobe_config" << std::endl;
                   exit(0);
            }

            PREDICT::PredictAnalyzer::CreateAnalyzerFromLog(argv[2], argv[3], &analyzer);
            if (analyzer != NULL) {
                analyzer->AnalyzeAllPredictions();
            }
            PREDICT::PredictAnalyzer::DestroyAnalyzer(analyzer);
        }
        break;
    case 's': 
        { 
            if(argc != 5) {
               std::cout << "usage: predict s donor_pool config_dat cobe_config" << std::endl;
               exit(0);
            }

            PREDICT::PredictAnalyzer::CreateAnalyzerFromConfig(argv[3], argv[4], &analyzer);
            if (analyzer != NULL) {
                analyzer->AnalyzeDonorList(argv[2], &PREDICT::PredictAnalyzer::FirstValid);
            }
            PREDICT::PredictAnalyzer::DestroyAnalyzer(analyzer);
        }
        break;
        case 'z': 
        { 
            if(argc != 5) {
               std::cout << "usage: predict z donor_pool config_dat cobe_config" << std::endl;
               exit(0);
            }

            PREDICT::PredictAnalyzer::CreateAnalyzerFromConfig(argv[3], argv[4], &analyzer);
            if (analyzer != NULL) {
                analyzer->AnalyzeDonorList(argv[2], &PREDICT::PredictAnalyzer::PredictAll);
            }
            PREDICT::PredictAnalyzer::DestroyAnalyzer(analyzer);
        }
        break;
        case 'g': 
        { 
            if(argc < 6 || argc > 7 ) {
               std::cout << "usage: predict a donor_pool config_dat cobe_config [features.bin] output.csv" << std::endl;
               exit(0);
            }

            std::fstream output;
            if( argc == 6 )
            {
                output.open( argv[5], std::ios::out);
                PREDICT::PredictAnalyzer::CreateAnalyzerFromConfig(argv[3], argv[4], &analyzer, output);
            }
            else if( argc == 7 )
            {
                output.open( argv[6], std::ios::out);
                PREDICT::PredictAnalyzer::CreateAnalyzerFromConfig(argv[3], argv[4], argv[5], &analyzer, output);
            }

            if (analyzer != NULL) {
                analyzer->AnalyzeDonorListMod(argv[2], &PREDICT::PredictAnalyzer::PredictAllMod);
            }
            PREDICT::PredictAnalyzer::DestroyAnalyzer(analyzer);

            output.flush();
            output.close();
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
	std::cout << "Use one of the following :" << std::endl;
	std::cout << "\tPredict a procedure:\n\tpredict a dlog cobe_config procedureNum minsIntoRun" << std::endl;
	std::cout << "\n\tExtract config from dlog:\n\tpredict c dlog config_dat" << std::endl;
	std::cout << "\n\tAmap Platelets:\n\tpredict m donor_pool config_dat cobe_config output_file" << std::endl;
	std::cout << "\n\tPredict all procedures:\n\tpredict p dlog cobe_config" << std::endl;
	std::cout << "\n\tRun a simulation:\n\tpredict s donor_pool config_dat cobe_config output_file" << std::endl;
}




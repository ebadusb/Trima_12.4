#include <fstream>
#include <string>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <math.h>

#include "offline_optimizer.h"
#include "toplevel.h"
#include "sharedstruct.h"
#include "datalog.h"
#include "posthct.hpp"
#include "postcount.hpp"
#include "CSVParser.h"
#include "bloodtype.h"

void (*ProcessingFnPtr)(std::ofstream&, Offline_optimizer*, init::Donor&) = NULL;

void CSVOutputData(procedureDetail* proc, std::ofstream &op, Offline_optimizer* o, init::Donor& d);

void FirstValid(std::ofstream &op, Offline_optimizer* o, init::Donor& d)
{
	o->optimize();
	  //    output the Procedure params to file
	 for (int product=1; 
		  product <= o->_userconfig->ProcedureList.numberOfProcedures.Get();
		  product++) 
	 { 
		 procedureDetail* proc = o->getProduct(product);
		 if(proc->getStatus() == 0) {
			CSVOutputData(proc, op, o, d);
			break;
		 }
	 }
}
																								   
void AmapPlatelets(std::ofstream &op, Offline_optimizer* o, init::Donor& d)
{
   //Move these to an amap_config file later and the config.dat file eventually :)

	procedureDetail* procStart = o->getProduct(1);
	procedureDetail* procEnd = o->getProduct(2);

	float start = procStart->getDesiredYield(), end_yield = procEnd->getDesiredYield(), step = 0.1f;
	float current_yield = start;
	float plt_vol;
	while (end_yield <= current_yield) {
		plt_vol = 100000.0f * current_yield / procStart->getProcPlateletProdCon(); 
		o->_predict.create_procedure(current_yield,
        plt_vol,
        procStart->getProcFixedPlasmaVol(), 	/* plasma vol */
        procStart->getProcRBCDose(), 	/* rbc dose */
        procStart->getProcRBCHct(), 	/* rbc crit */
        o->_userconfig->Predict.Get().key_rsf, 	/* RSF */    
        procStart->getProcTime(),/* procedure time */
        BT_ALL, 
        procStart->getDesignator(), // designator
        false, 0, 0,    // Metered Platelets o->_userconfig->Templates.Get().key_plt_mss[0]
        false, 0,     // Metered Redcells
        3); // procedure number 
		if(o->optimize(3) == GOOD_PRODUCT) {
			CSVOutputData(o->getProduct(3), op, o, d);
			break;
		}
		current_yield -= step;
	}
	 
}

// Expects input csv files in a certain format. Modify appropriately for desired output. 
void ProcessBatch(const char* donorListPath, 
				  const char* userConfigPath,
				  const char* cobeConfigPath, 
				  const char* outFilePath,
				  void (*ProcessingFnPtr)(std::ofstream&, Offline_optimizer*, init::Donor&))
{
    // Open  Input CSV file
    std::ifstream ifs(donorListPath);

    // Open  Output Data File
     std::ofstream ofs(outFilePath);

	 Offline_optimizer* optimizer = new Offline_optimizer();
	 init::InitialConditions ic;
	 memset(&ic, 0, sizeof(init::InitialConditions));
	 ic.donor.height = 60; ic.donor.weight = 175; ic.donor.hct = 44; ic.donor.precount = 250; //Dummy defaults
	 optimizer->initialize(userConfigPath, cobeConfigPath, ic);

    // Write Header to Output file
    ofs << "Run No., TBV, Cpre, H(%), Sex , Height(in) , Weight(lbs), "
		"Yield, Vp, Vc, Vrbc, Vintarg, Vac, Time(min), Postcount, HypoD(ml), Postcrit, "
		"Qin-pp, Qin-pca, Qin-mr, Qin-pir, Qr-pp, Qr-pca, Qr-mr, Qr-pir, Status"
		 << std::endl;
	//char* linebuffer = new char[512];
	std::string linebuffer;
    // While <lines to parse>
    while ( getline(ifs, linebuffer) )
    {
		 if (linebuffer.find("ENDDATA") != std::string::npos ) {
			 break;
		 }
		 sscanf(linebuffer.c_str(), "%d, %d,%f,%f,%f,%f,%f", &ic.donor.id, &ic.donor.female, &ic.donor.height, &ic.donor.weight, &ic.donor.tbv, &ic.donor.precount, &ic.donor.hct);
		 /* in the data file, female = 2 and male = 1. But in predict, female = 1 and male = 0*/
		ic.donor.female--;
		if (!(ic.donor.hct < 1.0f && ic.donor.hct > 0.0f)) {
			ic.donor.hct /= 100.0f;
		}

		optimizer->update(ic);

		//Call the appropriate callback
		(*ProcessingFnPtr)(ofs, optimizer, ic.donor);

   }

   // Close Output File
   ofs.close();

   // Close Input File
   ifs.close();

}

void CSVOutputData(procedureDetail *pd,std::ofstream &op, Offline_optimizer* o, init::Donor& d)
{
      
   PostHct hct; 
   // regurgitate data
	op << d.id << ","  
		<< d.tbv << ","  
		<< d.precount << "," 
		<< d.hct << "," 
		<< d.female + 1 << "," 
		<< d.height << "," 
		<< d.weight  ;

   if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {
	   procedureDetailPltPlsRBC* proc = static_cast<procedureDetailPltPlsRBC*>(pd);
   
	   op  << "," << proc->getDesiredYield()
		   << "," << proc->_PredVp
		   << "," << proc->_PredVc
		   << "," << proc->_PredRBCVolume
		   << "," << proc->VinTarget()
		   << "," << proc->_PredVac
		   <<  "," << ((proc->_PredTp > 0) ? (proc->_PredTp): 0)
		   << "," << Postcount(d.precount, 
							   d.tbv, 
							   proc->getDesiredYield()*1E11,        // precount, tbv, yield, minpostcount 
							   o->_predict.getMinPostCount()).postcount() 
		   << "," << ((fabs(proc->hypovolemiaDistance()) > d.tbv /* cuts out random unitialized vals. */ ) ? 0 : proc->hypovolemiaDistance() )
		   << "," << hct.postHct(d.hct, 
								 d.tbv,
								 proc->getProcRBCDose(),
								 proc->getResidualRBCVolume(),
								 20.0f /*sample volume*/)
	       << "," << proc->substate[Predict::__PLATELET_PLASMA].Qininstd
		   << "," << proc->substate[Predict::__PCA].Qininstd
		   << "," << proc->substate[Predict::__MIDRUN].Qininstd
		   << "," << proc->substate[Predict::__PIR_PLASMA].Qininstd
		   << "," << proc->substate[Predict::__PLATELET_PLASMA].Qrinst
		   << "," << proc->substate[Predict::__PCA].Qrinst
		   << "," << proc->substate[Predict::__MIDRUN].Qrinst
		   << "," << proc->substate[Predict::__PIR_PLASMA].Qrinst
		   << ", " << proc->getStatus() ;
   }
   else{
	   procedureDetailRBCPls* proc = static_cast<procedureDetailRBCPls*>(pd);

	   op  << "," << 0  /* desired yield*/
		   << "," << proc->_PredVp
		   << "," << 0 /* Vc */ 
		   << "," << proc->_PredRBCVolume
		   << "," << 0 /* vintarg */ 
		   << "," << proc->_PredVac
		   <<  "," << ((proc->_PredTp > 0) ? (proc->_PredTp): 0)
		   << "," << Postcount(d.precount, 
							   d.tbv, 
							   proc->getDesiredYield()*1E11,        // precount, tbv, yield, minpostcount 
							   o->_predict.getMinPostCount()).postcount() 
		   << "," << ((fabs(proc->hypovolemiaDistance()) > d.tbv /* cuts out random unitialized vals. */ ) ? 0 : proc->hypovolemiaDistance() )
		   << "," << hct.postHct(d.hct, 
								 d.tbv,
								 proc->getProcRBCDose(),
								 proc->getResidualRBCVolume(),
								 20.0f /*sample volume*/)
		   << "," << 0 
		   << "," << 0 
		   << "," << 0 
		   << "," << 0 
		   << "," << 0 
		   << "," << 0 
		   << "," << 0 
		   << "," << 0 
		   << ", " << proc->getStatus() ;

	   //RBC? 
   }
   op << endl;
}

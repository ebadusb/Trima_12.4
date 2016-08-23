#include <cstdlib>
#include <iostream>
#include <cstdio>
#include "offline_optimizer.h"
#include "toplevel.h"
#include "sharedstruct.h"
#include "datalog.h"
#include "analyze.h"

Analyzer::Analyzer(const char* dlogPath, const char* cobeConfigPath )
{
	_dlogPath = dlogPath;
	_cobeConfigPath = cobeConfigPath;
	_optimizer = new Offline_optimizer();
	_optimizer->initialize(_dlogPath, _cobeConfigPath);

}
int Analyzer::AnalyzeRun(int procedureNum, float minsIntoRun)
{	
	
    init::InitialConditions ic = _optimizer->_predictReader.GetRecord(minsIntoRun);
    _optimizer->update(ic);
    _optimizer->optimize(procedureNum);
   int product = procedureNum; 
	procedureDetail* proc = _optimizer->getProduct(product);
	if (proc->getStatus() == GOOD_PRODUCT) {
		std::printf("%d: %.1f in %.1f  %.1f  %.1f  %.1f %.2f\n", product, proc->toplevel.Ytarg, proc->_PredVc, proc->_PredVp, proc->getPredRBCVolume(), proc->_PredTp, ic.time.proctime);             
	 } else 
		std::printf("%d: %s\n", product, _optimizer->description((INTERNAL_PREDICT_RESULTS)proc->getStatus()));
   
	//donor file, userconfig, cobeconfig
	//DataLog_Level dl;
	//ProcessBatch(argv[1], argv[2], argv[3]);
    return 0;
}

int Analyzer::RepredictAll(float minsIntoRun)
{
	init::InitialConditions ic = _optimizer->_predictReader.GetRecord(minsIntoRun);
	_optimizer->update(ic);
	_optimizer->optimize();
	for (int product=1; product<=_optimizer->getNumProcedures(); product++) {
		procedureDetail* proc = _optimizer->getProduct(product);
		if (proc->getStatus() == GOOD_PRODUCT) {
			std::printf("%d: %.1f in %.1f  %.1f  %.1f  %.1f\n", product, proc->toplevel.Ytarg, proc->_PredVc, proc->_PredVp, proc->getPredRBCVolume(), proc->_PredTp);
	  } else 
			std::printf("%d: %s\n", product, _optimizer->description((INTERNAL_PREDICT_RESULTS)proc->getStatus()));
	}
	return 1;
   
}

int Analyzer::AnalyzeEntireRun(int procedureNum)
{
	float prevTime = 0.0f;
    init::InitialConditions ic = _optimizer->_predictReader.GetRecord(1.0f);
	while(ic.time.proctime != prevTime ) {
		AnalyzeRun(procedureNum, ic.time.proctime);
		prevTime = ic.time.proctime;
		ic = _optimizer->_predictReader.GetRecord(ic.time.proctime + 0.1f);
	}
	return 1;
	
}

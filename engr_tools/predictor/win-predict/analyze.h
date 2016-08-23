#ifndef __PRED_ANALYZE_H_
#define __PRED_ANALYZE_H_
#include "offline_optimizer.h"

class Analyzer
{
	const char* _dlogPath;
	const char* _cobeConfigPath;
	Offline_optimizer* _optimizer;
public:
	Analyzer(const char* dlogPath, const char* cobeConfigPath );
	~Analyzer();

	int AnalyzeRun(int procedureNum = 1, float minsIntoRun = 0.0f );
	int RepredictAll(float minsIntoRun = 0.0f);
	int AnalyzeEntireRun(int procedureNum = 1);
};

#endif /* __PRED_ANALYZE_H_ */

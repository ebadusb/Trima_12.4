#ifndef __PREDICT_READER_H__
#define __PREDICT_READER_H__
#include "log_format.h"
#include "predict_init_structs.h"

class DLOG_PredictReader : public DLOG_Reader
{
	float current_minutes;
public:
	DLOG_PredictReader();
	~DLOG_PredictReader();
	bool initialize(const char* dlogName);     
	init::InitialConditions GetRecord(float minutes = 0);
	init::InitialConditions GetNext(); /* can be called repeatedly after invoking GetRecord once*/
};
#endif /* __PREDICT_READER_H__ */

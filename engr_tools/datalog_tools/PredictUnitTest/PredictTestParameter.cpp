#include "PredictTestParameter.h"


void PredictTestParameter::Extract()
{
	sscanf(expected.c_str(),"%d,%d,%f,%f,%f,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d", 
        &this->id,
        &this->procNumber,
        &this->tbv,
        &this->precount,
        &this->hct,
        &this->gender,
        &this->height,
        &this->weight,
        &this->yield,
        &this->predVp,
        &this->predVc,
        &this->vinTarget,
        &this->predVac,
        &this->predTp,
        &this->ssPlateletPlasmaQininstd,
        &this->ssMidRunQininstd,
        &this->ssMidRunQch,
        &this->ssMidRunT,
        &this->ssPcaT,
        &this->ssExPcaT,
        &this->ssPirPlasmaT,
        &this->ssPirNoPlasmaT,
        &this->ssMidRunRpm,
        &this->ssPlateletPlasmaRpm,
        &this->status);
}

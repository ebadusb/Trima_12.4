/*****************************************************************************\
**
** PredictRecordMrg.h
**
** Predict record manager.
**
\*****************************************************************************/

#ifndef PREDICTRECORDMGR_H
#define PREDICTRECORDMGR_H

/* Includes ******************************************************************/

#include "ExtractPredict.h"

namespace PREDICT {

/* Types *********************************************************************/

struct Conditions {
	DECODER::UTIL::Donor donor;
	DECODER::UTIL::Volumes volumes;
	DECODER::UTIL::Run run;
	DECODER::UTIL::Time time;
	DECODER::UTIL::Cassette cassette;
	DECODER::UTIL::Adjustments adjust;
	DECODER::UTIL::PredictData predict;
};

/* Classes *******************************************************************/

class PredictRecordMrg {
public:
	typedef std::map<DECODER::Uint32, DECODER::UTIL::Adjustments> AdjustmentsMap;
	typedef std::map<DECODER::Float, DECODER::UTIL::ProcData> ProcDataMap;
	typedef std::map<DECODER::Uint32, DECODER::UTIL::Donor> DonorStatsMap;
	typedef std::vector<DECODER::UTIL::PredictData> PredictDataArr;
    typedef std::map<DECODER::Uint32, DECODER::UTIL::PredictData> PredictDataMap;
    typedef std::map<int, DECODER::UTIL::SoftwareFeature> FeatureMap;

    Conditions GetRecord(std::string& dateTime)
    {
        return GetRecord((DECODER::Float)GetSecondsSinceStarted(dateTime));
    }

	Conditions GetRecord(DECODER::Uint32 seconds = 0){
	
		Conditions conditions = { 0 }; 
		
		AdjustmentsMap::const_iterator adjustmentsIter = mAdjustmentsMap.lower_bound(static_cast<int>(seconds));
 		ProcDataMap::const_iterator procDataIter = mProcDataMap.lower_bound(static_cast<int>(seconds));
		DonorStatsMap::const_iterator donorStatsIter = mDonorStatsMap.lower_bound(static_cast<int>(seconds));
		
		
		

#if FORREFERENCE		
		if (adjustmentsIter != mAdjustmentsMap.begin()) {
			--adjustmentsIter;
		}

		if (procDataIter != mProcDataMap.begin()) {
			--procDataIter;
		}

		if (donorStatsIter != mDonorStatsMap.begin()) {
			--donorStatsIter;
		}
#endif
        AdjustmentsMap::const_reverse_iterator revadjustment = mAdjustmentsMap.rbegin();
        for( ;revadjustment != mAdjustmentsMap.rend(); revadjustment++)
        {
            if(revadjustment->first > seconds )
                continue;
            conditions.adjust = revadjustment->second;
            break;
        }

        ProcDataMap::const_reverse_iterator revprocIter = mProcDataMap.rbegin();
        for(; revprocIter != mProcDataMap.rend(); revprocIter ++)
        {
            if( revprocIter->first > seconds )
                continue;
            conditions.volumes = revprocIter->second.volumes;
            conditions.run = revprocIter->second.run;
            conditions.time = revprocIter->second.time;
            conditions.cassette = revprocIter->second.cassette;
            break;
        }

        PredictDataMap::const_reverse_iterator revpred = mPredictDataMap.rbegin();
        for( ;revpred != mPredictDataMap.rend(); revpred++)
        {
            if(revpred->first > seconds )
                continue;
            conditions.predict = revpred->second;
            break;
        }
       

		if (donorStatsIter != mDonorStatsMap.end()) {
			conditions.donor = donorStatsIter->second;
		}
		else if (!mDonorStatsMap.empty())  {

			conditions.donor = mDonorStatsMap.rbegin()->second;
		}

		return conditions; 
	}

    unsigned int GetSecondsSinceStarted( std::string& dateTime)
    {
        time_t begin = DECODER::RecordLogHeader::LastRecordLogHeader.NodeTime();
        struct tm ref;
        sscanf(dateTime.c_str(),"%d/%d/%d_%d:%d:%d",&ref.tm_year, &ref.tm_mon, &ref.tm_mday, &ref.tm_hour, &ref.tm_min, &ref.tm_sec);
        ref.tm_year -= 1900;
        ref.tm_mon -=1;
        time_t end = mktime(&ref);
        double diff = difftime(end, begin);
        return (unsigned int)diff;
    }

    void SetFeatures()
    {
        Software_CDS& sw = Software_CDS::getRWCds();
        FeatureMap::iterator featureIter = mFeatureMap.begin();
        if( featureIter == mFeatureMap.end())
            return;
        for( ; featureIter != mFeatureMap.end(); featureIter++)
        {
            DECODER::UTIL::SoftwareFeature& feature = featureIter->second;
            sw.SetFeature(feature.feature, feature.enabled, AUTH_R_N_D, false);
        }
    }

	AdjustmentsMap mAdjustmentsMap;
	ProcDataMap mProcDataMap;
	DonorStatsMap mDonorStatsMap;
	PredictDataArr mPredictDataArr;
    PredictDataMap mPredictDataMap;
    FeatureMap mFeatureMap;
    unsigned int mSecondsStarted;
};

class ExtractPredictMgr : public ExtractPredict {
public:
	ExtractPredictMgr(PredictRecordMrg &mgr) 
		: ExtractPredict(mgr.mAdjustmentsMap, mgr.mProcDataMap, mgr.mPredictDataArr, mgr.mPredictDataMap, mgr.mDonorStatsMap, mgr.mFeatureMap, mgr.mSecondsStarted) {}
};

} // namespace PREDICT

#endif // PREDICTRECORDMGR_H

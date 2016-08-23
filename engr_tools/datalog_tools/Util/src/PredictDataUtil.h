/*****************************************************************************\
**
** PredictDataUtil.h
**
** Utility classes for extracting predict data from datalog files.
**
\*****************************************************************************/

#ifndef PREDICTDATAUTIL_H
#define PREDICTDATAUTIL_H

/* Includes ******************************************************************/

#include <Windows.h>
#include <string>
#include <sstream>
#include <set>

namespace DECODER {
namespace UTIL {

/* Constants *****************************************************************/

static const char *kTagDlogInitialization = "data log initialization completed";
static const char *kTagPredictAdjustment = "Adjustment caps:";
static const char *kTagPredictProcData = "Predict: ProcData";
static const char *kTagPredictDonorStats = "Donor Stats.";
static const char *kTagPredictData = "PredictData:";
static const char *kTagSoftwareFeatureData = "Feature:";
/* Types *********************************************************************/

struct Donor {
	int id;
	float height;
	float weight;
	float tbv;
	float precount;
	float hct;
	float bmi;
	int female; /* 1-female 0-male*/
	int blood_type;
};

struct Volumes {
	float vin;
	float vac;
	float vplasma_bag;
	float vplasma_bag_ac;
	float vcollect_bag;
	float vcollect_bag_ac;
	float vrbc_bag;
	float vrbc_bag_ac;
	float dose_rbc_bag;
	float vrep;
	float vsaline_bolus;
	float platelet_yield;
	float time_of_vincv; /* Total Collect volume */
	float vincv;
	float vin_epca_start;

	float rbc_s1;
	float rbc_s2;
	float rbc_prime;

	float rbc_s1_double;
	float rbc_s2_double;
	float rbc_double;
};

struct Run {
	int substate;
	float qch_min;
	float qch_cur;
	int npurges;
	int	nScheduledPurges;
	float adjusted_hct;
	int current_recovery;
	float vin_clearline;
	float wbc_purge_start_time;
	int wbc_purge_type;
	float substate_start_time;
	int midrun_entered;
	float last_proc_purge;
};

struct Time {
	float proctime;
};

struct Cassette  {
	int type;
	bool rbc_detector_calibrated; 
}; 

struct Adjustments {
	float qin;
	float qrp;
	float ir;
	float ratio;
};

struct ProcData {
	Volumes volumes;
	Run run;
	Time time;
	Cassette cassette;
};

struct PredictData
{
	float optimizing_sel_proc;
	float pir_qin_limit;
	float pir_csf;
	float accum_ydpur;
	float accum_purges;
	float last_calc_qchmax;
	float pca_entered;
	float pir_lockin;
	float qcmr;
	float platelet_plasma_qch;
	float pre_pir_qch;
};

struct SoftwareFeature
{
    int feature;
    Uint32 time;
    bool enabled;
    std::string name;
};

typedef std::map<Uint32, Adjustments> AdjustmentsMap;
typedef std::map<Float, ProcData> ProcDataMap;
typedef std::vector<PredictData> PredictDataArr;
typedef std::map<Uint32, PredictData> PredictDataMap;
typedef std::map<Uint32, Donor> DonorStatsMap;
typedef std::map<int, SoftwareFeature> FeatureMap;
typedef std::map<int, SoftwareFeature>::iterator FeatureMapIterator;
/* Classes *******************************************************************/

class DatalogInitialization
{
private:
    bool found;
    DECODER::Uint32& timeStarted;
public:
    DatalogInitialization( DECODER::Uint32& started) : timeStarted(started)
    {
        found = false;
    }

	void operator()(const DECODER::RecordStreamOutput &data) 
	{ 	
        if( ! found && data.mMessage.find(kTagDlogInitialization) != std::string::npos)
        {
            timeStarted = data.mTimeSeconds;
            found = true;
		}
	}

};

class ProcessPredictAdjustments {
public:
	ProcessPredictAdjustments(AdjustmentsMap &map) : mAdjustmentsMap(map) {}

	void operator()(const DECODER::RecordStreamOutput &data) 
	{ 	
		if(data.mMessage.find(kTagPredictAdjustment) != std::string::npos){
			Adjustments adjustments = { 0 };

			// Adjustment caps:Qin Qrp IR R: 142.000 302.500 1.200 13.700
			sscanf(data.mMessage.c_str(),
					"Adjustment caps:Qin Qrp IR R: %f %f %f %f", 
					&(adjustments.qin),
					&(adjustments.qrp),
					&(adjustments.ir),
					&(adjustments.ratio));

			mAdjustmentsMap[data.mTimeSeconds] = adjustments;
		}
	}
private:
	AdjustmentsMap &mAdjustmentsMap;
};

class ProcessPredictProcData {
public:
	ProcessPredictProcData(ProcDataMap &map) : mProcDataMap(map) {}

	void operator()(const DECODER::RecordStreamOutput &data) 
	{ 	
		if(data.mMessage.find(kTagPredictProcData) != std::string::npos){
			ProcData procData = { 0 };
			float maxQch = 0;
			float EPCAstart = 0;
            float substateStartTime = 0, wbcPurgeStartTime =0;
			
//			                               Predict: ProcData: subst 10006 yld 6.974 Vc 179.971 - 30.419 Vp 33.605 - 5.327 Vrep 0.000 Vrbc 0.000 Dose 0.000 Vin 5082.830 Vac 463.181 Qch 3.431 minQch 2.839 maxQch 4.000 time 76.892 PIRSpill false Purges 1 Vincv 454.507 VincvTm 12.442 EPCAstart 0.000 VrbcS1 0.000 VrbcS2 0.000 VrbcS1-double 0.000 VrbcS2-double 0.000 VrbcPr 0.000 Vrbc-double 0.000 WBCPurgeStartTime 62.533 WBCPurgeType 2 SubstateStartTime 12.425 MidrunEntered 1 Scheduled Purges 1 PredictRequest UNKNOWN(0) LastProcPurge 4604.114
            sscanf(data.mMessage.c_str(), "Predict: ProcData: subst %d yld %f Vc %f - %f Vp %f - %f Vrep %f Vrbc %f Dose %f Vin %f Vac %f Qch %f minQch %f maxQch %f time %f PIRSpill %*s Purges %d Vincv %f VincvTm %f EPCAstart %f VrbcS1 %f VrbcS2 %f VrbcS1-double %f VrbcS2-double %f VrbcPr %f Vrbc-double %f WBCPurgeStartTime %f WBCPurgeType %d SubstateStartTime %f MidrunEntered %d Scheduled Purges %d PredictRequest %*s LastProcPurge %f",
					&procData.run.substate, 
					&procData.volumes.platelet_yield, 
					&procData.volumes.vcollect_bag,
					&procData.volumes.vcollect_bag_ac, 
					&procData.volumes.vplasma_bag, 
					&procData.volumes.vplasma_bag_ac,
					&procData.volumes.vrep, 
					&procData.volumes.vrbc_bag, 
					&procData.volumes.dose_rbc_bag,
					&procData.volumes.vin, 
					&procData.volumes.vac, 
					&procData.run.qch_cur, 
					&procData.run.qch_min, 
					&maxQch, 
					&procData.time.proctime, 
					/*PIRSpill-ignored notice the %*s */ 
					&procData.run.npurges,
					&procData.volumes.vincv, 
					&procData.volumes.time_of_vincv, 
					&procData.volumes.vin_epca_start, 
					&procData.volumes.rbc_s1, 
					&procData.volumes.rbc_s2,
					&procData.volumes.rbc_s1_double, 
					&procData.volumes.rbc_s2_double, 
					&procData.volumes.rbc_prime, 
					&procData.volumes.rbc_double,
					&procData.run.wbc_purge_start_time, 
					&procData.run.wbc_purge_type,
					&procData.run.substate_start_time,
					&procData.run.midrun_entered,
					&procData.run.nScheduledPurges,
					/* predict request ignored */
					&procData.run.last_proc_purge
					);
            procData.volumes.platelet_yield *= (float)1.0e11;
			mProcDataMap[data.mTimeSeconds] = procData;
		}
	}
private:
	ProcDataMap &mProcDataMap;
};

class ProcessPredictDonorStats {
public:
	ProcessPredictDonorStats(DonorStatsMap &map) : mDonorStatsMap(map) {}

	void operator()(const DECODER::RecordStreamOutput &data) 
	{ 	
		if(data.mMessage.find(kTagPredictDonorStats) != std::string::npos){
			Donor donor = { 0 };
			float sampleVol = 0;

			// Donor Stats.  TBV: 5498.99600 PreCnt: 198 Hct: 0.40000 Wt: 200.00000 BT: 4 SampleVol: 20.00000
			sscanf(data.mMessage.c_str(),
					"Donor Stats.  TBV: %f PreCnt: %f Hct: %f Wt: %f BT: %d SampleVol: %f",
					&donor.tbv, 
					&donor.precount, 
					&donor.hct, 
					&donor.weight, 
					&donor.blood_type, 
					&sampleVol);

			mDonorStatsMap[data.mTimeSeconds] = donor;
		}
	}
private:
	DonorStatsMap &mDonorStatsMap;
};
class ProcessPredictData {
public:
	ProcessPredictData(PredictDataArr &arr) : mPredictDataArr(arr) 
    {
        pPredictDataMap = NULL;
    }
    ProcessPredictData(PredictDataMap *map, PredictDataArr &arr) : mPredictDataArr(arr) 
    {
        pPredictDataMap = NULL;
        PredictDataMap* mapPtr = dynamic_cast<PredictDataMap*>(map);
        if( mapPtr != NULL )
            pPredictDataMap = mapPtr;
    }
	void operator()(const DECODER::RecordStreamOutput &data) 
	{ 	
		if(data.mMessage.find(kTagPredictData) != std::string::npos){
			PredictData predictData = { 0 };
			float maxQch = 0;
			float EPCAstart = 0;
            float plateletPlasmaQch = 0;
            float prePirQch = 0;
			
			// PredictData: AccumPurges 0.000 AccumYdPur 0.000 LastCalcQchmax 3.000 LastCsf 0.934 OptimizingSelProc 1.000 
			//	PCAEntered 0.000 PIRLockin 0.000 QinMidrun 65.366 OpAdjQinmax 0.000 Qcmr 0.000
			//sscanf(data.mMessage.c_str(), "PredictData: AccumPurges %f AccumYdPur %f LastCalcQchmax %f LastCsf %f OptimizingSelProc %f PCAEntered %f PIRLockin %f PirQinLimit %f OpAdjQinmax %f Qcmr %f", 
            if (strstr(data.mMessage.c_str(),"PCAEntered")) { // make backwards compatible for either logs with/without PCAEntered logging
            sscanf(data.mMessage.c_str(),   "PredictData: AccumPurges %f AccumYdPur %f LastCalcQchmax %f PirCsf %f OptimizingSelProc %f PCAEntered %f PIRLockin %f PirQinLimit %f Qcmr %f PlateletPlasmaQch %f PrePirQch %f", 
					&predictData.accum_purges, 
					&predictData.accum_ydpur, 
					&predictData.last_calc_qchmax,
					&predictData.pir_csf,
					&predictData.optimizing_sel_proc,
					&predictData.pca_entered,
					&predictData.pir_lockin,
					&predictData.pir_qin_limit,
                    &predictData.qcmr,
					&predictData.platelet_plasma_qch,
					&predictData.pre_pir_qch);
            } else {
            sscanf(data.mMessage.c_str(),   "PredictData: AccumPurges %f AccumYdPur %f LastCalcQchmax %f PirCsf %f OptimizingSelProc %f PIRLockin %f PirQinLimit %f Qcmr %f PlateletPlasmaQch %f PrePirQch %f", 
					&predictData.accum_purges, 
					&predictData.accum_ydpur, 
					&predictData.last_calc_qchmax,
					&predictData.pir_csf,
					&predictData.optimizing_sel_proc,
					&predictData.pir_lockin,
					&predictData.pir_qin_limit,
                    &predictData.qcmr,
					&predictData.platelet_plasma_qch,
					&predictData.pre_pir_qch);
            }
			
			mPredictDataArr.push_back(predictData);
            if( pPredictDataMap != NULL )
                (*pPredictDataMap)[data.mTimeSeconds] = predictData;
		}
	}
private:
	PredictDataArr &mPredictDataArr;
    PredictDataMap *pPredictDataMap;
};

class ProcessSoftwareFeature {
public:
	ProcessSoftwareFeature(FeatureMap &map) : mFeatureMap(map), num(0), namebuffer(new char[50]), onbuffer(new char[50]),onRef("on"){}
    ~ProcessSoftwareFeature(){ delete namebuffer; delete onbuffer;}
	void operator()(const DECODER::RecordStreamOutput &data) 
	{ 	
		if(data.mMessage.find(kTagSoftwareFeatureData) != std::string::npos){
            memset(onbuffer,NULL, 50);
			SoftwareFeature feature = { 0 };
            sscanf(data.mMessage.c_str(),  "Feature: %d %s = %s",&num, namebuffer, onbuffer);
            feature.enabled = onRef.compare(onbuffer) == 0 ? true : false;
            std::string tmp = std::string(namebuffer);
            feature.feature = num;
            feature.time = data.mTimeSeconds;
            feature.name = tmp.substr(1, tmp.length() - 1).substr(0, tmp.length() - 2);
            FeatureMapIterator iter = mFeatureMap.find(num);
            

            if( iter == mFeatureMap.end() )
                
    			mFeatureMap[num] = feature;
            else
            {
                SoftwareFeature soft = iter->second;
                if( (*iter).second.time < data.mTimeSeconds || ( (*iter).second.time <= data.mTimeSeconds && feature.enabled) )
                    mFeatureMap[num] = feature;
            }
		}
	}
private:
	FeatureMap &mFeatureMap;
    int num;
    char* namebuffer;
    char* onbuffer;
    std::string onRef;
};
} // namespace UTIL
} // namespace TRACE

#endif // PREDICTDATAUTIL_H


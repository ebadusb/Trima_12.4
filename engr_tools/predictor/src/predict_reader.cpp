#include "predict_reader.h"
#include "log_format.h"
#include <cstring>
#include <cstdio>
DLOG_PredictReader::DLOG_PredictReader() : DLOG_Reader()
{ 
}
DLOG_PredictReader::~DLOG_PredictReader() 
{ 
}

bool DLOG_PredictReader::initialize(const char* dlogname)
{
	_dlogName = dlogname;
	//read in only stream records. 
	//filter.includeRecordTypes.push_back(DLOG_Record::DLOG_StreamOutputId);
	_dlogFilter.includeStrings.push_back("Predict: ProcData");
	_dlogFilter.includeStrings.push_back("Donor Stats.");
	_dlogFilter.includeStrings.push_back("Adjustment caps:");
	if(!_initialized) {
		if (DLOG_Reader::initialize() == 0) 
				return true;
			else
				return false;
	}
	//initialized, hopefully well
	return true;
};


init::InitialConditions DLOG_PredictReader::GetRecord(float minutes)
{
	init::InitialConditions ic;
	memset(&ic, 0, sizeof(ic)); 
	vector<DLOG_Record*>::iterator it;
	float dummyFloat = 0.0; //for unused floats in the log.
	bool bDonorFound = false, bProcDataFound = false, bAdjCapsFound = false;
   /* An adjustment caps log can occur any time */ 
	for(it =_dlogRecords.begin(); it!= _dlogRecords.end(); it++) {
		if( bAdjCapsFound && bDonorFound && bProcDataFound) {
				break;
		}
		DLOG_StreamOutput* streamRecord = (DLOG_StreamOutput*) *it;
		string& contents = streamRecord->streamContents;

		if(contents.find("Adjustment caps:") != contents.npos){
			/*Adjustment caps:Qin Qrp IR R: 142.000 302.500 1.200 13.700 */
			sscanf(contents.c_str(), "Adjustment caps:Qin Qrp IR R: %f %f %f %f", 
				   &(ic.adjust.qin), &(ic.adjust.qrp), &(ic.adjust.ir), &(ic.adjust.ratio));
			bAdjCapsFound = true;
			continue;
		}  
		if(!bProcDataFound && contents.find("Predict: ProcData") != contents.npos) {
	
			/*Predict: ProcData: subst 10031 yld 0.00000 Vc 0.00000 - 0.00000 Vp 0.00000 - 0.00000 Vrep 0.00000 Vrbc 0.00000 Dose 0.00000 Vin 0.00000 Vac 18.28890 Qch 0.00000 minQch 0.00000 maxQch 0.00000 time 0.00000 PIRSpill false Purges 0 Vincv 0.00000 VincvTm 0.00000 EPCAstart 0.00000 VrbcS1 0.000 VrbcS2 0.000 VrbcS1-double 0.000 VrbcS2-double 0.000 VrbcPr 0.000 Vrbc-double 0.000 CurrentRecovery -1 VinClearLine 0.00000 WBCPurgeStartTime 0 */
				sscanf(contents.c_str(), "Predict: ProcData: subst %d yld %f Vc %f - %f Vp %f - %f Vrep %f Vrbc %f Dose %f Vin %f Vac %f Qch %f minQch %f maxQch %f time %f PIRSpill %*s Purges %d Vincv %f VincvTm %f EPCAstart %f VrbcS1 %f VrbcS2 %f VrbcS1-double %f VrbcS2-double %f VrbcPr %f Vrbc-double %f CurrentRecovery %d VinClearLine %f WBCPurgeStartTime %lu WBCPurgeType %d", 
					  &ic.run.substate, &ic.volumes.platelet_yield, &ic.volumes.vcollect_bag,
					   &ic.volumes.vcollect_bag_ac, &ic.volumes.vplasma_bag, &ic.volumes.vplasma_bag_ac,
					   &ic.volumes.vrep, &ic.volumes.vrbc_bag, &ic.volumes.vrbc_bag_ac,
					   &ic.volumes.vin, &ic.volumes.vac, &ic.run.qch_cur, &ic.run.qch_min, &dummyFloat/*maxQch*/, 
					   &ic.time.proctime/*time*/, /*PIRSpill-ignored notice the %*s */ &ic.run.npurges, &ic.volumes.vincv, 
					   &ic.volumes.time_of_vincv, &dummyFloat/*EPCAstart*/, &ic.volumes.rbc_s1, &ic.volumes.rbc_s2,
					   &ic.volumes.rbc_s1_double, &ic.volumes.rbc_s2_double, &ic.volumes.rbc_prime, &ic.volumes.rbc_double,
					   &ic.run.current_recovery, &ic.run.vin_clearline, &ic.run.wbc_purge_start_time, &ic.run.wbc_purge_type);
				
			if(ic.time.proctime >= minutes){ /* tricky */
				bProcDataFound = true;
			}
			continue;
		}
		if(contents.find("Donor Stats.") != contents.npos ){
			/*Donor Stats.  TBV: 5498.99600 PreCnt: 198 Hct: 0.40000 Wt: 200.00000 BT: 4 SampleVol: 20.00000*/
		   sscanf(contents.c_str(), "Donor Stats.  TBV: %f PreCnt: %f Hct: %f Wt: %f BT: %d SampleVol: %f",
				  &ic.donor.tbv, &ic.donor.precount, &ic.donor.hct, &ic.donor.weight, &ic.donor.blood_type, &dummyFloat);
		   bDonorFound = true;
		   continue;
		}
	}
	current_minutes = ic.time.proctime;
	return ic;
}

init::InitialConditions DLOG_PredictReader::GetNext()
{
	float minutes = current_minutes;
	init::InitialConditions ic = GetRecord(minutes + 0.1);
	if(minutes == current_minutes)
		ic.time.proctime = -1.0f;
	return ic;
}

/*****************************************************************************\
**
** OfflineOptimizer.h
**
** Predict offline optimizer.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <cmath>
#include <string>

#include "OfflineOptimizer.h"
#include "substatemap.h"
#include "units.h"
#include "states.h"
#include "alarms.h"
#include "recoverytypes.h"
#include "ExtractConfig.h"
#include "LogExtractor.h"
#include "LogExecute.h"

namespace PREDICT {

/* Functions *****************************************************************/

/**
 *
 * Delete
 *
 * Utility function for deleting pointers.
 *
 * @param ptr The pointer to delete.
 *
 */
static void Delete(DECODER::LogExtractor *ptr) 
{ 
	delete ptr; 
}

/**
 *
 * CreateOptimizerFromLog
 *
 * Creates an offline optimizer and populates the configurations
 * using the given datalog and cobe config files.
 *
 * @param dataLogPath The datalog file
 * @param cobeConfigPath The cobe config file.
 * @param optimizer The new optimizer.
 *
 * @return True if the optimizer was created.
 *
 */
bool OfflineOptimizer::CreateOptimizerFromLog(
		const char* dataLogPath,
		const char* cobeConfigPath, 
		OfflineOptimizer **optimizer)
{
	ALARM_VALUES status = NULL_ALARM;
	OfflineOptimizer *offlineOptimizerP = new OfflineOptimizer();
	Conditions conditions = { 1, 60, 175, 0, 250, 44, 0 }; 

	if((status = CobeConfig::initialize(cobeConfigPath, 1)) != NULL_ALARM) {
		delete offlineOptimizerP;
		std::cerr << "Status: "<< status << " : Error reading cobe config file..." << std::endl;
		return false;
	}  

	if (!DECODER::ProcessLogData(dataLogPath, CONFIG::ExtractConfig(offlineOptimizerP->mConfig))) {
		delete offlineOptimizerP;
		std::cerr << "Error reading user datalog file..." << std::endl;
		return false;
	}

	offlineOptimizerP->mPredict = new Predict;

	offlineOptimizerP->UpdateConditions(conditions);
	offlineOptimizerP->mPredict->Init();
	offlineOptimizerP->mPredict->FillInConfig();
	offlineOptimizerP->mPredict->FillInProcedureConfig();

	*optimizer = offlineOptimizerP;

	return true;
}

/**
 *
 * CreateOptimizerFromLog
 *
 * Creates an offline optimizer and populates the configurations
 * using the given datalog and cobe config files. This version 
 * also extracts the predict initial conditions from the datalog. 
 *
 * @param dataLogPath The datalog file
 * @param cobeConfigPath The cobe config file.
 * @param predictRecordMrg The predict record manager.
 * @param optimizer The new optimizer.
 *
 * @return True if the optimizer was created.
 *
 */
bool OfflineOptimizer::CreateOptimizerFromLog(
		const char* dataLogPath,
		const char* cobeConfigPath, 
		PredictRecordMrg &manager,
		OfflineOptimizer **optimizer)
{
	ALARM_VALUES status = NULL_ALARM;
	OfflineOptimizer *offlineOptimizerP = new OfflineOptimizer();
	DECODER::LogProcessList logProcessList;
	Conditions conditions = { 1, 60, 175, 0, 250, 44, 0 }; 

	manager.mAdjustmentsMap.clear();
	manager.mProcDataMap.clear();
	manager.mDonorStatsMap.clear();

	logProcessList.push_back(new PREDICT::ExtractPredictMgr(manager));
	logProcessList.push_back(new CONFIG::ExtractConfig(offlineOptimizerP->mConfig));

	if((status = CobeConfig::initialize(cobeConfigPath, 1)) != NULL_ALARM) {
		for_each(logProcessList.begin(), logProcessList.end(), Delete);
		delete offlineOptimizerP;		
		std::cerr << "Status: "<< status << " : Error reading cobe config file..." << std::endl;
		return false;
	}  


	DECODER::Result result = DECODER::ProcessLogData(dataLogPath, logProcessList);
	if ( result && (result != DECODER::LogReader::kResultUnexpectedEOF))
	{
		for_each(logProcessList.begin(), logProcessList.end(), Delete);
		delete offlineOptimizerP;
		std::cerr << "Error reading user datalog file..." << std::endl;
		return false;
	}

	for_each(logProcessList.begin(), logProcessList.end(), Delete);
    manager.SetFeatures();
	offlineOptimizerP->mPredict = new Predict;
	offlineOptimizerP->UpdateConditions(conditions);
	offlineOptimizerP->mPredict->Init();
	offlineOptimizerP->mPredict->FillInConfig();
	offlineOptimizerP->mPredict->FillInProcedureConfig();

	*optimizer = offlineOptimizerP;

	return true;
}

/**
 *
 * CreateOptimizerFromConfig
 *
 * Creates an offline optimizer and populates the configurations
 * using the given user and cobe config files.
 *
 * @param userConfigPath The user config file
 * @param cobeConfigPath The cobe config file.
 * @param optimizer The new optimizer.
 *
 * @return True if the optimizer was created.
 *
 */
bool OfflineOptimizer::CreateOptimizerFromConfig(
		const char* userConfigPath,
		const char* cobeConfigPath, 
		OfflineOptimizer **optimizer)
{
	ALARM_VALUES status = NULL_ALARM;
	OfflineOptimizer *offlineOptimizerP = new OfflineOptimizer();
	Conditions conditions = { 1, 60, 175, 0, 250, 44, 0 }; 


	if((status = CobeConfig::initialize(cobeConfigPath,1)) != NULL_ALARM) {
		delete offlineOptimizerP;
		std::cerr << "Status: "<< status << " : Error reading cobe config file..." << std::endl;
		return false;
	}	

	if(!offlineOptimizerP->mConfig.ReadConfig(userConfigPath)) {
		delete offlineOptimizerP;
		std::cerr << "Error reading user config file..." << std::endl;
		return false;
	}


	offlineOptimizerP->mPredict = new Predict;
	offlineOptimizerP->UpdateConditions(conditions);
	offlineOptimizerP->mPredict->Init();
	offlineOptimizerP->mPredict->FillInConfig();
	offlineOptimizerP->mPredict->FillInProcedureConfig();

	*optimizer = offlineOptimizerP;

   	return true;
}

/**
 *
 * CreateOptimizerFromConfig
 *
 * Creates an offline optimizer and populates the configurations
 * using the given user and cobe config files.
 *
 * @param userConfigPath The user config file
 * @param cobeConfigPath The cobe config file.
 * @param softwareConfigPath The software config file.
 * @param optimizer The new optimizer.
 *
 * @return True if the optimizer was created.
 *
 */
bool OfflineOptimizer::CreateOptimizerFromConfig(
		const char* userConfigPath,
		const char* cobeConfigPath, 
		const char* softwareConfigPath, 
		OfflineOptimizer **optimizer)
{
	if (Software_CDS::Read(softwareConfigPath)) 
    {
		return OfflineOptimizer::CreateOptimizerFromConfig(
				userConfigPath, cobeConfigPath, optimizer);
	}

	return NULL;
}

/**
 *
 * DestroyOptimizer
 *
 * Destroys the given offline optimizer.
 *
 * @param optimizer The optimizer to delete.
 *
 * @return True if the optimizer was destroyed.
 *
 */
bool OfflineOptimizer::DestroyOptimizer(OfflineOptimizer *optimizer)
{
	bool result = false;

	if (optimizer != NULL) {
		delete optimizer;
		result = true;
	}

	return result;
}

/**
 *
 * OfflineOptimizer
 *
 * Constructor
 *
 */
OfflineOptimizer::OfflineOptimizer(void) : mConfig(ROLE_RW)
{
}

/**
 *
 * ~OfflineOptimizer
 *
 * Destructor
 *
 */
OfflineOptimizer::~OfflineOptimizer()
{
}

void OfflineOptimizer::UpdateConditions(Conditions& ic)
{

	mPredict->SetAllUnoptimized();

	DECODER::UTIL::Donor& donor = ic.donor;
	DECODER::UTIL::Volumes& volumes = ic.volumes;
	DECODER::UTIL::Run& run = ic.run;
	DECODER::UTIL::Cassette& set = ic.cassette;
	DECODER::UTIL::Time& time = ic.time;
	DECODER::UTIL::Adjustments& adjust = ic.adjust;
	DECODER::UTIL::PredictData& predictData = ic.predict;
                
	ProcRun_CDS runcds(ROLE_RW);
	runcds.AdjustedHct.Set(run.adjusted_hct);
	/* LR additions */
	runcds.CurrentRecovery.Set(static_cast<RecoveryTypes::Type> (run.current_recovery));
	runcds.WBCPurgeStartTime.Set(run.wbc_purge_start_time);
	/* might have to modify manually */
	runcds.WBCPurgeType.Set(static_cast<WBCPURGE_TYPE>(run.wbc_purge_type));

	ProcDonor_CDS donorcds(ROLE_RW);
	if(donor.tbv == 0.0f) 
		donor.tbv = calculateTBV(donor.height, donor.weight, donor.female);
	donorcds.TBV.Set(donor.tbv);
	if(donor.height == 0.0f) {
		donor.height = calculateHeight(donor.tbv, donor.weight, donor.female);
	}
    if( Software_CDS::GetInstance().getFeature(JapanFeatures) == true )
    {
        CDonor aDonor;
        float weight = donor.weight;
        float height = donor.height;
       // If metric units, must convert to English units
       if ( mConfig.LangUnit.Get().key_height == HEIGHT_UNITS_IN)
         height *= CM_PER_IN;  // convert to centimeters
       if (mConfig.LangUnit.Get().key_weight == WEIGHT_UNITS_LB)
          weight *= KG_PER_LB;    // convert to kg to

        float tbv = aDonor.cCalTBVOgawa(!donor.female, weight, height/100.f);
        donorcds.TBVOgawa.Set( tbv );
    }
	donorcds.PreCnt.Set(donor.precount);
	donorcds.Hct.Set(donor.hct);
	donorcds.Weight.Set(donor.weight);
	donorcds.BloodType.Set(donor.blood_type);
	donorcds.SampleVolume.Set(20.0f);
	donorcds.Female.Set(donor.female);	
	donor.bmi = calculateBMI(donor.height, donor.weight);
	donorcds.BMI.Set(donor.bmi);
	
	ProcVolumes_CDS volumescds(ROLE_RW);
	volumescds.Vin.Set(volumes.vin);
	volumescds.Vac.Set(volumes.vac);
	volumescds.PlasmaVolume.Set(volumes.vplasma_bag);
	volumescds.VacPlasmaBag.Set(volumes.vplasma_bag_ac);
	volumescds.PlateletVolume.Set(volumes.vcollect_bag);
	volumescds.VacPlateletBag.Set(volumes.vcollect_bag_ac);
	volumescds.RBCVolume.Set(volumes.vrbc_bag);
	volumescds.VacRBCBag.Set(volumes.vrbc_bag_ac);
	volumescds.RBCDose.Set(volumes.dose_rbc_bag);
	volumescds.VReplacement.Set(volumes.vrep);
	volumescds.VSalineBolus.Set(volumes.vsaline_bolus);
	volumescds.PlateletYield.Set(volumes.platelet_yield);///1.0e11f);
	volumescds.TimeOfVincv.Set(volumes.time_of_vincv);
	volumescds.Vincv.Set(volumes.vincv);
	volumescds.VinEPCAStart.Set(volumes.vin_epca_start);
	
	ProcTime_CDS timecds(ROLE_RW);
	timespec proctime;
	float f_runtime = time.proctime*60.0f;

	//crazy there is a roundoff error of 1.0 when converting
	//from float to long.
	int temp = (int) (f_runtime + 0.001f);
	proctime.tv_sec = temp;//static_cast<>(f_runtime);
	proctime.tv_nsec = 0;
	timecds.tmProcRun.Set(proctime);
	
	ProcTrimaSet_CDS setcds(ROLE_RW);
	set.rbc_detector_calibrated = 1;

	//NOT_LOADED=0, UNKNOWN, PLTPLSRBC, RBCPLS, END_OF_LIST
	set.type = 0;
	setcds.Cassette.Set(cassette::cassetteType(set.type));
	setcds.RBCDetectorCalibrated.Set(set.rbc_detector_calibrated);
	
	// RKM
	//setcdsj.RBCPTFSet.Set(conditions.m_ptf);
	if(run.substate == 0 || (run.substate == 10045 && ic.time.proctime <= 10.0f)) {
		run.substate = INVALID_SUBSTATE;
	}
	runcds.Substate.Set(static_cast<State_names>(run.substate));
	runcds.Qchmin.Set(run.qch_min);
	runcds.Qchcurrent.Set(run.qch_cur);
	runcds.NumberWBCPurges.Set(run.npurges);
	runcds.NScheduledWBCPurges.Set(run.nScheduledPurges);
	runcds.SubstateStartTime.Set(run.substate_start_time);
	runcds.MidrunEntered.Set(run.midrun_entered);
	runcds.LastProcPurge.Set(run.last_proc_purge);

	// RKM
	//runcds.PTFQrbc.Set(30.0f);
	//runcds.PTFQrbc.Set(conditions.m_ptf_qrbc);
	Predict_CDS predictcds(ROLE_RW);
	predictcds.PirQinLimit.Set(predictData.pir_qin_limit);
	predictcds.accumulatedPurges.Set(predictData.accum_purges);
	predictcds.accumulatedYdpur.Set(predictData.accum_ydpur);
	predictcds.lastCalcQchMax.Set(predictData.last_calc_qchmax);
	predictcds.PirCsf.Set(predictData.pir_csf);
	//predictcds.OpAdjQinmax.Set(predictData.op_adj_qinmax);
	predictcds.OptimizingSelProc.Set(predictData.optimizing_sel_proc);
	// predictcds.PCAEntered.Set(predictData.pca_entered);
	predictcds.PIRLockin.Set(predictData.pir_lockin);
	predictcds.PlateletPlasmaQch.Set(predictData.platelet_plasma_qch);
	predictcds.PrePirQch.Set(predictData.pre_pir_qch);
	predictcds.Qcmr.Set(predictData.qcmr);
	
	mPredict->UpdateProcData();
	
	ProcAdjust_CDS adjustcds(ROLE_RW);
	if(adjust.qin == 0.0f) 
		adjust.qin = 142.0f;
	if (adjust.qrp == 0.0f)
		adjust.qrp = 302.5f;
	if(adjust.ratio == 0.0f)
		adjust.ratio = 13.7f;
	if(adjust.ir == 0.0f)
		adjust.ir = 1.2f;
	adjustcds.Draw.CurrentCap.Set(adjust.qin);
	adjustcds.Return.CurrentCap.Set(adjust.qrp);
	adjustcds.Tingling.CurrentCap.Set(adjust.ir);
	adjustcds.Clumping.CurrentCap.Set(adjust.ratio);


	mPredict->doRepredictForAdjustments();

	//mPredict.FillInDonor();
	
}

procedureDetail *OfflineOptimizer::GetOptimizeProcedure(const int index)
{
   procedureDetail *pd = mPredict->getProcedure(index);

   if (pd && (pd->getStatus() == UNOPTIMIZED )) {
      mPredict->predictProcedure(*pd);
   }
   return pd;
}

procedureDetail *OfflineOptimizer::GetProcedure(int index)
{
   return mPredict->getProcedure(index);
}

int OfflineOptimizer::GetNumProcedures()
{
	return mConfig.ProcedureList.numberOfProcedures.Get();
}

const float OfflineOptimizer::calculateTBV(float height, float weight, int female)
{
   float TBV = 0.0f;

   // If metric units, must convert to English units
   if ( mConfig.LangUnit.Get().key_height == HEIGHT_UNITS_CM)
     height /= CM_PER_IN;  // convert centimeters to inches
   if (mConfig.LangUnit.Get().key_weight != WEIGHT_UNITS_LB)
      weight *= LB_PER_KG;    // convert kg to lbs
   if (female)
      TBV = 183.0f + 0.005835f*(float)pow(height,3) + 15.0f*weight;
   else
      TBV = 604.0f + 0.006012f*(float)pow(height,3) + 14.6f*weight;
   return TBV;
}

const float OfflineOptimizer::calculateBMI(float height,float weight)
{
   // check for height in inches, convert to meters
   if ( mConfig.LangUnit.Get().key_height == HEIGHT_UNITS_IN )
      height /= 39.370079f;

   // check for height in cm, convert to meters
   if ( mConfig.LangUnit.Get().key_height == HEIGHT_UNITS_CM )
      height /= 100.0f;

   // check for weight in lbs, convert to kg
   if ( mConfig.LangUnit.Get().key_weight == WEIGHT_UNITS_LB )
      weight /= 2.2046226f;
   return weight/(height * height);
}

const float OfflineOptimizer::calculateHeight(float tbv, float weight, int female)
{
   float height = 0.0f;

   if (mConfig.LangUnit.Get().key_weight != WEIGHT_UNITS_LB)
      weight *= LB_PER_KG;    // convert kg to lbs
   if (female)
	   height = pow((tbv - 15.0f*weight - 183.0f)/0.005835f, 1.0f/3.0f);
   else
	   height = pow((tbv - 14.6f*weight - 604.0f)/0.006012f, 1.0f/3.0f);
   // If metric units, must convert to English units
   if ( mConfig.LangUnit.Get().key_height == HEIGHT_UNITS_CM)
	 height *= CM_PER_IN;  // convert centimeters to inches

   return height;
}

} // namespace PREDICT

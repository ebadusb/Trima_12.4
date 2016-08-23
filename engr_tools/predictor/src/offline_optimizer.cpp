#include "offline_optimizer.h"
#include "substatemap.h"
#include "units.h"
#include "states.h"
#include "predict_strings.h"
#include "predict_init_structs.h"
#include "alarms.h"
#include "recoverytypes.h"
#include "configdat_reader.h"

#include <cmath>
#include <string>
//-----------------------------------------------------------------------------
//                 Constructor, destructor
//
Offline_optimizer::Offline_optimizer() {}

Offline_optimizer::~Offline_optimizer()
{
	delete this->_userconfig;
}



//-----------------------------------------------------------------------------
//                         Initialization
//
int Offline_optimizer::initialize(const char* dlogPath, const char* cobeConfigPath)
{

	// Locate dlog in the quad. 
	//Read in config from DLOG
	this->_userconfig = new Config_CDS(ROLE_RW);
	ConfigDatReader datReader;
	datReader.initialize(dlogPath);
	datReader.ReadConfig(this->_userconfig);
	ALARM_VALUES status = NULL_ALARM;
	if( (status = CobeConfig::initialize(cobeConfigPath,1)) != NULL_ALARM) {
		cerr << "Status: "<< status << " :Error reading cobe config file..." << endl;
		return status;
	}  

	_predictReader.initialize(dlogPath);

	this->_ic = _predictReader.GetRecord();
	this->setInitialConditions(this->_ic);

	_predict.FillInDonor();
	_predict.Init();
	_predict.FillInConfig();
	_predict.FillInProcedureConfig();

	return status;
}

ALARM_VALUES Offline_optimizer::initialize(const char* userConfigPath, const char* cobeConfigPath, const char* dlogPath)
{
	this->_userconfig = new Config_CDS(ROLE_RW);
	const bool success = _userconfig->ReadConfig(userConfigPath); 
	if(!success) {
		cerr << "Error reading user config file..." << endl;
		return CONFIGDAT_BAD_FILE;
	}

	ALARM_VALUES status = NULL_ALARM;
	if( (status = CobeConfig::initialize(cobeConfigPath,1)) != NULL_ALARM) {
		cerr << "Status: "<< status << " :Error reading cobe config file..." << endl;
		return status;
	}	

	DLOG_PredictReader reader;
	reader.initialize(dlogPath);
	this->_ic = reader.GetRecord(0);
	this->setInitialConditions(this->_ic);

	_predict.FillInDonor();
	_predict.Init();
	_predict.FillInConfig();
	_predict.FillInProcedureConfig();

   	return status;
}

ALARM_VALUES Offline_optimizer::initialize(const char* userConfigPath, const char* cobeConfigPath, init::InitialConditions ic)
{
	this->_userconfig = new Config_CDS(ROLE_RW);
	const bool success = _userconfig->ReadConfig(userConfigPath); 
	if(!success) {
		cerr << "Error reading user config file..." << endl;
		return CONFIGDAT_BAD_FILE;
	}

	ALARM_VALUES status = NULL_ALARM;
	if( (status = CobeConfig::initialize(cobeConfigPath,1)) != NULL_ALARM) {
		cerr << "Status: "<< status << " :Error reading cobe config file..." << endl;
		return status;
	}	

	this->_ic = ic; 
	this->setInitialConditions(this->_ic);

	_predict.FillInDonor();
	_predict.Init();
	_predict.FillInConfig();
	_predict.FillInProcedureConfig();

   	return status;
}

void Offline_optimizer::setInitialConditions(init::InitialConditions& ic)
{
	init::Donor& donor = ic.donor;
	init::Volumes& volumes = ic.volumes;
	init::Run& run = ic.run;
	init::Cassette& set = ic.cassette;
	init::Time& time = ic.time;
	init::Adjustments& adjust = ic.adjust;

	ProcRun_CDS runcds(ROLE_RW);
	runcds.AdjustedHct.Set(run.adjusted_hct);
	/* LR additions */
	//runcds.CurrentRecovery.Set(static_cast<RecoveryTypes::Type> (run.current_recovery));
	//runcds.WBCPurgeStartTime.Set(run.wbc_purge_start_time);
	/* might have to modify manually */
	//runcds.WBCPurgeType.Set(WBCPURGE_SCHEDULED);

	ProcDonor_CDS donorcds(ROLE_RW);
	if(donor.tbv == 0.0f) 
		donor.tbv = calculateTBV(donor.height, donor.weight, donor.female);
	donorcds.TBV.Set(donor.tbv);
	donorcds.PreCnt.Set(donor.precount);
	donorcds.Hct.Set(donor.hct);
	donorcds.Weight.Set(donor.weight);
	donorcds.BloodType.Set(donor.blood_type);
	donorcds.SampleVolume.Set(20.0f);
	donorcds.Female.Set(donor.female);	
	if(donor.height == 0.0f) {
		donor.height = calculateHeight(donor.tbv, donor.weight, donor.female);
	}
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
	volumescds.PlateletYield.Set(volumes.platelet_yield);
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
	if(run.substate == 0) {
		run.substate = SubstateMap::alg(SS_CHANNEL_SETUP, set.type); 
	}
	runcds.Substate.Set(static_cast<State_names>(run.substate));
	runcds.Qchmin.Set(run.qch_min);
	runcds.Qchcurrent.Set(run.qch_cur);
	runcds.NumberWBCPurges.Set(run.npurges);
	
	// RKM
	//runcds.PTFQrbc.Set(30.0f);
	//runcds.PTFQrbc.Set(conditions.m_ptf_qrbc);
	
	_predict.UpdateProcData();
	
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
	_predict.doRepredictForAdjustments();
}

void Offline_optimizer::update(init::InitialConditions& ic)
{
	this->_ic = ic;
	setInitialConditions(this->_ic);
	_predict.FillInDonor();
}

void Offline_optimizer::optimize()
{
   _predict.SetUnoptimized();
   int productindex = 1;
   procedureDetail *pd = getProduct(productindex);
   while (pd) {
	  //pd->ResetProcDet();
      _predict.predictProcedure(*pd);
      pd = getProduct(++productindex);
   }
}

INTERNAL_PREDICT_RESULTS Offline_optimizer::optimize(const int productindex)
{
   _predict.SetUnoptimized();
   procedureDetail *pd = getProduct(productindex);
   if (pd) {
	   //pd->ResetProcDet();
      _predict.predictProcedure(*pd);
      return pd->getStatus();
   }
   return UNOPTIMIZED;
}

procedureDetail *Offline_optimizer::getProduct(int procindex)
{
   procedureDetail *detail = _predict.getProcedure(procindex);
   return detail;
}

int Offline_optimizer::getNumProcedures()
{
	if (this->_userconfig != NULL) {
		return this->_userconfig->ProcedureList.numberOfProcedures.Get();
	}
	return -1;
}

const char* Offline_optimizer::description(const INTERNAL_PREDICT_RESULTS result)
{
   static const int MAXRESULT = 32;

   int result_as_int = (int)result;
   if ((result_as_int >= 0) && (result_as_int <= MAXRESULT)) {

      //  ----- WARNING ----- This code assumes that the predict_strings array contains
      //  enumerations in the order specified in the predict_strings.h enumeration!
     return PredictStrings[result]; 
   } 
   return NULL;
}

const float Offline_optimizer::calculateTBV(float height, float weight, 
                                            int female)
{
   float TBV = 0.0f;

   // If metric units, must convert to English units
   if ( _userconfig->LangUnit.Get().key_height == HEIGHT_UNITS_CM)
     height /= CM_PER_IN;  // convert centimeters to inches
   if (_userconfig->LangUnit.Get().key_weight != WEIGHT_UNITS_LB)
      weight *= LB_PER_KG;    // convert kg to lbs
   if (female)
      TBV = 183.0f + 0.005835f*(float)pow(height,3) + 15.0f*weight;
   else
      TBV = 604.0f + 0.006012f*(float)pow(height,3) + 14.6f*weight;
   return TBV;
}

const float Offline_optimizer::calculateBMI(float height,float weight)
{
   // check for height in inches, convert to meters
   if ( _userconfig->LangUnit.Get().key_height == HEIGHT_UNITS_IN )
      height /= 39.370079;

   // check for height in cm, convert to meters
   if ( _userconfig->LangUnit.Get().key_height == HEIGHT_UNITS_CM )
      height /= 100.0;

   // check for weight in lbs, convert to kg
   if ( _userconfig->LangUnit.Get().key_weight == WEIGHT_UNITS_LB )
      weight /= 2.2046226;
   return weight/(height * height);
}

const float Offline_optimizer::calculateHeight(float tbv, float weight, 
                                            int female)
{
   float height = 0.0f;

   if (_userconfig->LangUnit.Get().key_weight != WEIGHT_UNITS_LB)
      weight *= LB_PER_KG;    // convert kg to lbs
   if (female)
	   height = pow((tbv - 15.0f*weight - 183.0f)/0.005835f, 1.0f/3.0f);
   else
	   height = pow((tbv - 14.6f*weight - 604.0f)/0.006012f, 1.0f/3.0f);
   // If metric units, must convert to English units
   if ( _userconfig->LangUnit.Get().key_height == HEIGHT_UNITS_CM)
	 height /= CM_PER_IN;  // convert centimeters to inches

   return height;
}


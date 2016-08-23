#include <vxworks.h>
#include <math.h>
#include "procdonor_cds.h"
#include "config_cds.h"
#include "procadjust_cds.h"
#include "procrun_cds.h"
#include "procruntargets_cds.h"
#include "procvolumes_cds.h"
#include "proctrimaset_cds.h"
#include "proctime_cds.h"
#include "predict_cds.h"
#include "bloodtype.h"
#include "trimamessages.h"
#include "predict_msgs.h"
#include "messagesystem.h"

static MessageSystem predict_test_ms;
static int predict_test_ms_initialized = 0;

static ProcDonor_CDS *donorcds = 0; // get around multiple writer bug
static Config_CDS    *configcds = 0;

static const int NUMPROC=12;
void repredict()
{

   if (!predict_test_ms_initialized) { predict_test_ms.initBlocking(); predict_test_ms_initialized=1;}
   DoPredictionMsg msg(MessageBase::SEND_LOCAL);
   msg.send((int)DONOR_INFO_PRECONNECT);
}

void next(int count)
{
   if (!predict_test_ms_initialized) { predict_test_ms.initBlocking(); predict_test_ms_initialized=1;}
   if (count <=1) count = 1;
   GuiToProcPredictRequestMsg downbtn(MessageBase::SEND_LOCAL);
   for (int i=0; i<count; i++) downbtn.send(SELECT_NEXT);
}

void fill_in_cds()
{
   if (!donorcds) donorcds = new ProcDonor_CDS(ROLE_RW);
   donorcds->Female.Set(0);
   donorcds->Height.Set(72.0f);
   donorcds->Weight.Set(150.0f);
   donorcds->TBV.Set(5038.0f);
   donorcds->PreCnt.Set(270.0f);
   donorcds->Hct.Set(0.43f);
   donorcds->BloodType.Set(0);
   donorcds->SampleVolume.Set(20.0f);


   if (!configcds) configcds = new Config_CDS(ROLE_RW);
   LangUnitCfg lang;
   lang.key_lang = 0;
   lang.key_height = 0;
   lang.key_weight = 0;
   configcds->LangUnit.Set(lang);

   ProcedureConfig proc;
   proc.key_proc_time = 150;
   proc.key_custom_ratio = 11.0f;
   proc.key_ac_rate = 4;
   proc.key_post_crit = 0.32f;
   proc.key_tbv_vol_setting = 1; // TBV
   proc.key_tbv_percent = 15.0f;
#if 1
   proc.key_weight_setting = 175.0f;
   proc.key_weight_less_than_vol = 500.0f;
   proc.key_weight_greater_than_vol = 600.0f;
#else
   proc.key_weight_3_setting = 175.0f;
   proc.key_weight_3_less_than_vol = 500.0f;
   proc.key_weight_3_greater_than_vol = 600.0f;
#endif
   proc.key_ml_per_kg = 10.5f;
   proc.key_post_plat = 80.0f;
   proc.key_max_draw_flow = 2;
   proc.key_min_replacement_volume = 250;
   proc.key_drbc_alert = 0;
   configcds->Procedure.Set(proc);

   PredictionConfig pred;
   pred.key_rbc_ratio = 0;
   pred.key_rbc_replace_fluid = 1;
   pred.key_rbc_fluid_percent = 100.0f;
   pred.key_pls_amap_min = 150;
   pred.key_pls_amap_max = 1000;
   pred.key_ysf = 1.0f;
   pred.key_guaranteed_yield = 0;
   pred.key_inlet_management = 5;
   pred.key_inlet_flow_ramp = 1;
   pred.key_return_management = 3;
   pred.key_rsf = 1.0f;
   configcds->Predict.Set(pred);

   ProductListArray procedure;
   procedure[0].yield = 3.0f;
   procedure[0].platelet_volume = 214.3f;
   procedure[0].plasma_volume = 0.0f;
   procedure[0].rbc_dose = 0.0f;
   procedure[0].rbc_crit = 80.0f;
   procedure[0].max_procedure_time = 120.0f;
   procedure[0].blood_type = BT_ALL;

   procedure[1].yield = 3.0f;
   procedure[1].platelet_volume = 214.3f;
   procedure[1].plasma_volume = 200.0f;
   procedure[1].rbc_dose = 0.0f;
   procedure[1].rbc_crit = 80.0f;
   procedure[1].max_procedure_time = 120.0f;
   procedure[1].blood_type = BT_ALL;

   procedure[2].yield = 3.0f;
   procedure[2].platelet_volume = 214.3f;
   procedure[2].plasma_volume = -1.0f;
   procedure[2].rbc_dose = 0.0f;
   procedure[2].rbc_crit = 80.0f;
   procedure[2].max_procedure_time = 120.0f;
   procedure[2].blood_type = BT_ALL;

   procedure[3].yield = 3.0f;
   procedure[3].platelet_volume = 214.3f;
   procedure[3].plasma_volume = -2.0f;
   procedure[3].rbc_dose = 0.0f;
   procedure[3].rbc_crit = 80.0f;
   procedure[3].max_procedure_time = 120.0f;
   procedure[3].blood_type = BT_ALL;

   procedure[4].yield = 3.5f;
   procedure[4].platelet_volume = 87.5f;
   procedure[4].plasma_volume = 0.0f;
   procedure[4].rbc_dose = 0.0f;
   procedure[4].rbc_crit = 80.0f;
   procedure[4].max_procedure_time = 120.0f;
   procedure[4].blood_type = BT_ALL;

   procedure[5].yield = 3.0f;
   procedure[5].platelet_volume = 214.3f;
   procedure[5].plasma_volume = -1.0f;
   procedure[5].rbc_dose = 200.0f;
   procedure[5].rbc_crit = 80.0f;
   procedure[5].max_procedure_time = 120.0f;
   procedure[5].blood_type = BT_ALL;

   procedure[6].yield = 0.0f;
   procedure[6].platelet_volume = 0.0f;
   procedure[6].plasma_volume = 0.0f;
   procedure[6].rbc_dose = 200.0f;
   procedure[6].rbc_crit = 80.0f;
   procedure[6].max_procedure_time = 120.0f;
   procedure[6].blood_type = BT_ALL;

   procedure[7].yield = 0.0f;
   procedure[7].platelet_volume = 0.0f;
   procedure[7].plasma_volume = -1.0f;
   procedure[7].rbc_dose = 200.0f;
   procedure[7].rbc_crit = 80.0f;
   procedure[7].max_procedure_time = 120.0f;
   procedure[7].blood_type = BT_ALL;

   //
   //  These products were noted in Japan trials as nonoptimal (see IT5749)
   //
   procedure[8].yield = 4.2f; // 1T00159_20020801_001 donor 4528 45 251
   procedure[8].platelet_volume = 270.0f;
   procedure[8].plasma_volume = 200.0f;
   procedure[8].rbc_dose = 0.0f;
   procedure[8].rbc_crit = 80.0f;
   procedure[8].max_procedure_time = 120.0f;
   procedure[8].blood_type = BT_ALL;

   procedure[9].yield = 3.2f; // 1T00755_20020730_000 donor 4430 42 220
   procedure[9].platelet_volume = 250.0f;
   procedure[9].plasma_volume = 220.0f;
   procedure[9].rbc_dose = 0.0f;
   procedure[9].rbc_crit = 80.0f;
   procedure[9].max_procedure_time = 120.0f;
   procedure[9].blood_type = BT_ALL;

   procedure[10].yield = 4.2f; // 1T00755_20020806_001 donor 3834 44 275
   procedure[10].platelet_volume = 270.0f;
   procedure[10].plasma_volume = 200.0f;
   procedure[10].rbc_dose = 0.0f;
   procedure[10].rbc_crit = 80.0f;
   procedure[10].max_procedure_time = 120.0f;
   procedure[10].blood_type = BT_ALL;

   procedure[11].yield = 2.3f; // 1T00732_20020805_000 donor 3303 40 311
   procedure[11].platelet_volume = 220.0f;
   procedure[11].plasma_volume = 250.0f;
   procedure[11].rbc_dose = 0.0f;
   procedure[11].rbc_crit = 80.0f;
   procedure[11].max_procedure_time = 120.0f;
   procedure[11].blood_type = BT_ALL;


   configcds->ProcedureList.numberOfProcedures.Set(NUMPROC);
   configcds->ProcedureList.Procedure.Set(procedure);

   ProcAdjust_CDS adjustcds(ROLE_RW); // set initial values
   ProcRun_CDS runcds(ROLE_RW); // set initial values
   ProcVolumes_CDS volumescds(ROLE_RW); // set initial values
   ProcTrimaSet_CDS setcds(ROLE_RW); // set initial values
   ProcTime_CDS timecds(ROLE_RW); // set initial values
   setcds.Cassette.Set(cassette::NOT_LOADED);
}

//
//
//   These numbers are from V5 results.
//
static float expTime[20][11] = {
	0.291f,   0.0f, 3.656f,   0.0f,   0.0f, 22.49f, 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,
	0.291f, 3.253f,   0.0f, 7.931f,   0.0f, 13.79f, 0.0f, 0.0f, 0.372f,   0.0f,   0.0f,
	0.291f, 3.273f,   0.0f, 21.16f,  1.07f, 0.279f, 0.0f, 0.0f,   3.2f, 0.198f,   0.0f,
	0.291f, 3.217f,   0.0f, 21.31f, 0.941f, 0.480f, 0.0f, 0.0f, 3.098f,   0.0f,   0.0f,
	0.291f,   0.0f, 6.780f,   0.0f,   0.0f,  26.5f, 0.0f, 0.0f, 0.257f,   0.0f,   0.0f,
	0.291f, 3.258f,   0.0f, 11.54f,   0.0f, 9.796f, 0.0f, 0.0f, 0.478f,   0.0f, 9.256f,
	0.562f,   0.0f, 9.948f,   0.0f,   0.0f,   0.0f, 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,
	0.562f, 27.07f, 9.926f,   0.0f,   0.0f,   0.0f, 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,
   0.291f, 3.343f,   0.0f, 5.045f,   0.0f, 38.77f, 0.0f, 0.0f, 3.571f, 3.853f,   0.0f,
   0.291f, 3.463f,   0.0f, 6.298f,   0.0f, 31.53f, 0.0f, 0.0f, 3.571f, 2.273f,   0.0f,
   0.314f, 3.749f,   0.0f, 8.785f,   0.0f, 37.55f, 0.0f, 0.0f, 2.993f,   0.0f,   0.0f,
   0.345f,   0.0f, 3.827f,   0.0f, 2.353f,  22.4f, 0.0f, 0.0f,   0.0f, 15.90f,   0.0f,

	  0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,
	  0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,
	  0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,
	  0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,
	  0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,
	  0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,
	  0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,
	  0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f, 0.0f, 0.0f,   0.0f,   0.0f,   0.0f
};

static int expVin[20][11] = {
	200,  200,  402,  402,  402, 2241, 2241, 2241, 2241, 2241, 2241,
	200,  404,  404, 1038, 1038, 2217, 2217, 2217, 2243, 2243, 2243,
	200,  401,  401, 2107, 2207, 2230, 2230, 2230, 2450, 2450, 2450,
	200,  401,  401, 2106, 2193, 2237, 2237, 2237, 2454, 2454, 2454,
	200,  200,  572,  572,  572, 2438, 2438, 2438, 2463, 2463, 2463,
	200,  404,  404, 1327, 1327, 2209, 2209, 2209, 2242, 2242, 2772,
	215,  215,  775,    0,    0,    0,    0,    0,    0,    0,    0,
	215, 1463, 2023,    0,    0,    0,    0,    0,    0,    0,    0,
   200,  409,  409,  762,  762, 3091, 3091, 3091, 3341, 3650, 3650,
   200,  416,  416,  857,  857, 2741, 2741, 2741, 2991, 3173, 3173,
   200,  407,  407, 1022, 1022, 2935, 2935, 2935, 3145, 3145, 3145,
   200,  200,  392,  392,  542, 1680, 1680, 1680, 1680, 2653, 2653,

	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
	  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0

};

static int procedure_expected = 0;

class PredictTestCallback
{
public:
   void GotProcedure() {
      datalog_SetTaskOutputOptions(DATALOG_CURRENT_TASK, DataLog_LogEnabled, DataLog_ConsoleEnabled);
      /*
      response.getData();

      //
      //
      //  Note: due to handshake (eg predict reponds with this message each time
      //  pinged by proc, even if no procedure or invalid data), check to ensure
      //  valid procedure number response.  The first response is always invalid
      //  since it is the response from the preconnect reopt, before any product
      //  is selected.
      //
      if (response.procedureNumber() != procedure_expected) return;
 
      Predict_CDS cds(ROLE_RO);
      const Predict_CDS::substate_info_t &data = cds.SubstateInfo.Get();
      int valid = 1;
      int subst;
      for (subst=1; subst<12; subst++) {
         if (fabs(expTime[response.procedureNumber()-1][subst-1] - data[subst].T) > 0.2) valid = 0;
         if (fabs((double)expVin[response.procedureNumber()-1][subst-1] - (double)data[subst].EndVin) > 10.0) valid = 0;
      }
      if (!valid) {
         DataLog( log_level_predict_info ) << "procedure number " << response.procedureNumber() << " ";
         for (subst=1; subst<12; subst++) {
            DataLog( log_level_predict_info ) << "[" << expVin[response.procedureNumber()-1][subst-1] << "," << data[subst].EndVin << "]";
         }
         DataLog( log_level_predict_info ) << endmsg;
         DataLog( log_level_predict_info ) << "procedure number " << response.procedureNumber() << " ";
         for (subst=1; subst<12; subst++) {
            DataLog( log_level_predict_info ) << "[" << expTime[response.procedureNumber()-1][subst-1] << "," << data[subst].T << "]";
         }
         DataLog( log_level_predict_info ) << endmsg;
      } else {
         DataLog( log_level_predict_info ) << "procedure number " << response.procedureNumber() << " predicted OK" << endmsg;
      }

      if (response.procedureNumber() < NUMPROC) {
         Message<ModifyProductStruct> modifyProductMsg(MessageBase::SEND_GLOBAL);
         ModifyProductStruct request;
         request.product_number = procedure_expected = response.procedureNumber()+1;
         request.action = REOPTIMIZE_PRODUCT;

         //
         //
         //   Set specific donor cases
         //
         DoPredictionMsg msg(MessageBase::SEND_LOCAL);
         ConfigChangedMsg configchanged(MessageBase::SEND_LOCAL);
         if (!donorcds) donorcds = new ProcDonor_CDS(ROLE_RW);
         
         if (!configcds) configcds = new Config_CDS(ROLE_RW);
         ProcedureConfig proc = configcds->Procedure.Get();
         
         switch (request.product_number) {
         case 9:
            donorcds->TBV.Set(4528.0f);
            donorcds->PreCnt.Set(251.0f);
            donorcds->Hct.Set(0.45f);
            proc.key_ac_rate = 2;
            configcds->Procedure.Set(proc);
            configchanged.send(0);
            msg.send((int)DONOR_INFO_CHANGED);
            break;
         case 10:
            donorcds->TBV.Set(4430.0f);
            donorcds->PreCnt.Set(220.0f);
            donorcds->Hct.Set(0.42f);
            msg.send((int)DONOR_INFO_CHANGED);
            break;
         case 11:
            donorcds->TBV.Set(3834.0f);
            donorcds->PreCnt.Set(275.0f);
            donorcds->Hct.Set(0.44f);
            msg.send((int)DONOR_INFO_CHANGED);
            break;
         case 12:
            donorcds->TBV.Set(3303.0f);
            donorcds->PreCnt.Set(311.0f);
            donorcds->Hct.Set(0.40f);
            msg.send((int)DONOR_INFO_CHANGED);
            break;
         }

         modifyProductMsg.send(request);
         request.action = SET_PRODUCT_AS_CURRENT;
         modifyProductMsg.send(request);
      }
      */

   };

};

void predict_test()
{
   datalog_SetTaskOutputOptions(DATALOG_CURRENT_TASK, DataLog_LogEnabled, DataLog_ConsoleEnabled);

   if (!predict_test_ms_initialized) { predict_test_ms.initBlocking(); predict_test_ms_initialized=1;}
   PredictTestCallback testcallback;
   // testcallback.response.init(Callback<PredictTestCallback>(&testcallback,&PredictTestCallback::GotProcedure));

   fill_in_cds();
   repredict();

   Message<ModifyProductStruct> modifyProductMsg(MessageBase::SEND_GLOBAL);
   ModifyProductStruct request;
   request.product_number = procedure_expected = 1;
   request.action = REOPTIMIZE_PRODUCT;
   modifyProductMsg.send(request);
   request.action = SET_PRODUCT_AS_CURRENT;
   modifyProductMsg.send(request);

   predict_test_ms.dispatchMessages();
   cout << "Exiting dispatch loop" << endl;

}

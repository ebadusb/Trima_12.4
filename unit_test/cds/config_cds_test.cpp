#include "config_cds.h"
#include <iostream.h>

void test_config_cds()
{
   //DataStore::turn_on_logging();

   Config_CDS _cds(ROLE_RW);

   _cds.Read();

/*
   LangUnitCfg _lang = _cds_w.LangUnit.Get();
   _lang.key_lang = 5;
   _cds_w.LangUnit.Set(_lang);
   _cds_w.Write();
*/

   cout << "Language Unit Values:" << endl;
   cout << "\tlanguage              " << (_cds.LangUnit.Get()).key_lang << endl;
   cout << "\theight                " << (_cds.LangUnit.Get()).key_height << endl;
   cout << "\tweight                " << (_cds.LangUnit.Get()).key_weight << endl;
   cout << "\tkey_date_format       " << (_cds.LangUnit.Get()).key_date_format << endl;
   cout << "\tkey_time_format       " << (_cds.LangUnit.Get()).key_time_format << endl;
   cout << "\tkey_decimal_delimiter " << (_cds.LangUnit.Get()).key_decimal_delimiter << endl;
   cout << endl;

   cout << "Basic Machine Values:" << endl;
   cout << "\ttime     " << (_cds.Machine.Get()).key_set_time << endl;
   cout << "\tdate     " << (_cds.Machine.Get()).key_set_date << endl;
   cout << "\taudio    " << (_cds.Machine.Get()).key_set_audio << endl;
   cout << "\tpassword " << (_cds.Machine.Get()).key_pword << endl;
   cout << endl;

   cout << "Procedure Values:    " << endl;
   cout << "\tProc Time          " << (_cds.Procedure.Get()).key_proc_time << endl;
   cout << "\tReturn Pressure    " << (_cds.Procedure.Get()).key_return_press << endl;
   cout << "\tDraw Pressure      " << (_cds.Procedure.Get()).key_draw_press << endl;
   cout << "\tCustom Ratio       " << (_cds.Procedure.Get()).key_custom_ratio << endl;
   cout << "\tAC Rate            " << (_cds.Procedure.Get()).key_ac_rate << endl;
   cout << "\tPost Crit          " << (_cds.Procedure.Get()).key_post_crit << endl;
   cout << "\tTBV Vol            " << (_cds.Procedure.Get()).key_tbv_vol_setting << endl;
   cout << "\tTBV Percent        " << (_cds.Procedure.Get()).key_tbv_percent << endl;
#if 1
   cout << "\tWeight Setting     " << (_cds.Procedure.Get()).key_weight_setting << endl;
   cout << "\tLess Than Vol      " << (_cds.Procedure.Get()).key_weight_less_than_vol << endl;
   cout << "\tGreater Than Vol   " << (_cds.Procedure.Get()).key_weight_greater_than_vol << endl;
#else
   cout << "\tWeight Setting     " << (_cds.Procedure.Get()).key_weight_3_setting << endl;
   cout << "\tLess Than Vol      " << (_cds.Procedure.Get()).key_weight_3_less_than_vol << endl;
   cout << "\tGreater Than Vol   " << (_cds.Procedure.Get()).key_weight_3_greater_than_vol << endl;
#endif
   cout << "\tML Per Kg          " << (_cds.Procedure.Get()).key_ml_per_kg << endl;
   cout << "\tPost Platelet      " << (_cds.Procedure.Get()).key_post_plat << endl;
   cout << "\tMax Draw Flow      " << (_cds.Procedure.Get()).key_max_draw_flow << endl;
   cout << "\tMin Replace Vol    " << (_cds.Procedure.Get()).key_min_replacement_volume << endl;
   cout << "\tDRBC Alert         " << (_cds.Procedure.Get()).key_drbc_alert << endl;
   cout << "\tRinseback Protocol " << (_cds.Procedure.Get()).key_plasma_rinseback << endl;
   cout << endl;

   cout << "Predict Values:      " << endl;
   cout << "\tRBC Ratio          " << (_cds.Predict.Get()).key_rbc_ratio << endl;
   cout << "\tRBC Replace Fluid  " << (_cds.Predict.Get()).key_rbc_replace_fluid << endl;
   cout << "\tRBC Fluid Percent  " << (_cds.Predict.Get()).key_rbc_fluid_percent << endl;
   cout << "\tPlasma AMAP Min    " << (_cds.Predict.Get()).key_pls_amap_min << endl;
   cout << "\tPlasma AMAP Max    " << (_cds.Predict.Get()).key_pls_amap_min << endl;
   cout << "\tYSF                " << (_cds.Predict.Get()).key_ysf << endl;
   cout << "\tGuaranteed Yield   " << (_cds.Predict.Get()).key_guaranteed_yield << endl;
   cout << "\tConfidence Level   " << (_cds.Predict.Get()).key_confidence_level << endl;
   cout << "\tCoeff Of Variation " << (_cds.Predict.Get()).key_coefficient_of_variation << endl;
   cout << "\tInlet Management   " << (_cds.Predict.Get()).key_inlet_management << endl;
   cout << "\tInlet Flow Ramp    " << (_cds.Predict.Get()).key_inlet_flow_ramp << endl;
   cout << "\tReturn Mgt         " << (_cds.Predict.Get()).key_return_management << endl;
   cout << "\tRSF                " << (_cds.Predict.Get()).key_rsf << endl;
   cout << endl;

   cout << "Template Values:      " << endl;
   cout << "\tRBC Crit[0]         " << (_cds.Templates.Get()).key_rbc_crit[0] << endl;
   cout << "\tRBC Crit[1]         " << (_cds.Templates.Get()).key_rbc_crit[1] << endl;
   cout << "\tRBC Crit[2]         " << (_cds.Templates.Get()).key_rbc_crit[2] << endl;
   cout << "\tRBC Dose[0]         " << (_cds.Templates.Get()).key_rbc_dose[0] << endl;
   cout << "\tRBC Dose[1]         " << (_cds.Templates.Get()).key_rbc_dose[1] << endl;
   cout << "\tRBC Dose[2]         " << (_cds.Templates.Get()).key_rbc_dose[2] << endl;
   cout << "\tPlatelet Yield[0]   " << (_cds.Templates.Get()).key_plt_yield[0] << endl;
   cout << "\tPlatelet Yield[1]   " << (_cds.Templates.Get()).key_plt_yield[1] << endl;
   cout << "\tPlatelet Yield[2]   " << (_cds.Templates.Get()).key_plt_yield[2] << endl;
   cout << "\tPlatelet Yield[3]   " << (_cds.Templates.Get()).key_plt_yield[3] << endl;
   cout << "\tPlatelet Yield[4]   " << (_cds.Templates.Get()).key_plt_yield[4] << endl;
   cout << "\tPlatelet Yield[5]   " << (_cds.Templates.Get()).key_plt_yield[5] << endl;
   cout << "\tPlatelet Volume[0]  " << (_cds.Templates.Get()).key_plt_volume[0] << endl;
   cout << "\tPlatelet Volume[1]  " << (_cds.Templates.Get()).key_plt_volume[1] << endl;
   cout << "\tPlatelet Volume[2]  " << (_cds.Templates.Get()).key_plt_volume[2] << endl;
   cout << "\tPlatelet Volume[3]  " << (_cds.Templates.Get()).key_plt_volume[3] << endl;
   cout << "\tPlatelet Volume[4]  " << (_cds.Templates.Get()).key_plt_volume[4] << endl;
   cout << "\tPlatelet Volume[5]  " << (_cds.Templates.Get()).key_plt_volume[5] << endl;
   cout << "\tPlasma Volume[0]    " << (_cds.Templates.Get()).key_pls_volume[0] << endl;
   cout << "\tPlasma Volume[1]    " << (_cds.Templates.Get()).key_pls_volume[1] << endl;
   cout << "\tPlasma Volume[2]    " << (_cds.Templates.Get()).key_pls_volume[2] << endl;
   cout << "\tPlasma Volume[3]    " << (_cds.Templates.Get()).key_pls_volume[3] << endl;
   cout << "\tPlasma Volume[4]    " << (_cds.Templates.Get()).key_pls_volume[4] << endl;
   cout << "\tPlasma Volume[5]    " << (_cds.Templates.Get()).key_pls_volume[5] << endl;
   cout << endl;

   cout << "Template Values:      " << endl;

   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      cout << "\tPlatelet[" << i << "]   " << (_cds.Definition.Get())[i].key_platelet << endl;
   }
   cout << endl;
   
   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      cout << "\tPlasma[" << i << "]     " << (_cds.Definition.Get())[i].key_plasma << endl;
   }
   cout << endl;
   
   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      cout << "\tRBC[" << i << "]        " << (_cds.Definition.Get())[i].key_rbc << endl;
   }
   cout << endl;
   
   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      cout << "\tBlood Type[" << i << "] " << (_cds.Definition.Get())[i].key_blood_type << endl;
   }
   cout << endl;
   
   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      cout << "\tTime[" << i << "]       " << (_cds.Definition.Get())[i].key_time << endl;
   }
   cout << endl;
   
   cout << endl;


   for (int i = 0; i < _cds.ProcedureList.numberOfProcedures.Get(); i++)
   {
      cout << "Template Values:      " << endl;
      cout << "\tYield[" << i << "]      " << (_cds.ProcedureList.Procedure.Get())[i].yield << endl;
      cout << "\tPlatelet[" << i << "]   " << (_cds.ProcedureList.Procedure.Get())[i].platelet_volume << endl;
      cout << "\tPlasma[" << i << "]     " << (_cds.ProcedureList.Procedure.Get())[i].plasma_volume << endl;
      cout << "\tRBC Crit[" << i << "]   " << (_cds.ProcedureList.Procedure.Get())[i].rbc_crit << endl;
      cout << "\tRBC Dose[" << i << "]   " << (_cds.ProcedureList.Procedure.Get())[i].rbc_dose << endl;
      cout << "\tTime[" << i << "]       " << (_cds.ProcedureList.Procedure.Get())[i].max_procedure_time << endl;
      cout << "\tBlood Type[" << i << "] " << (_cds.ProcedureList.Procedure.Get())[i].blood_type << endl;
   }
}







void print_cds()
{
   //DataStore::turn_on_logging();

   Config_CDS _cds(ROLE_RW);

   _cds.Read();

   cout << "Language Unit Values:" << endl;
   cout << "\tlanguage              " << (_cds.LangUnit.Get()).key_lang << endl;
   cout << "\theight                " << (_cds.LangUnit.Get()).key_height << endl;
   cout << "\tweight                " << (_cds.LangUnit.Get()).key_weight << endl;
   cout << "\tkey_date_format       " << (_cds.LangUnit.Get()).key_date_format << endl;
   cout << "\tkey_time_format       " << (_cds.LangUnit.Get()).key_time_format << endl;
   cout << "\tkey_decimal_delimiter " << (_cds.LangUnit.Get()).key_decimal_delimiter << endl;
   cout << endl;

   cout << "Basic Machine Values:" << endl;
   cout << "\ttime     " << (_cds.Machine.Get()).key_set_time << endl;
   cout << "\tdate     " << (_cds.Machine.Get()).key_set_date << endl;
   cout << "\taudio    " << (_cds.Machine.Get()).key_set_audio << endl;
   cout << "\tpassword " << (_cds.Machine.Get()).key_pword << endl;
   cout << endl;

   cout << "Procedure Values:    " << endl;
   cout << "\tProc Time          " << (_cds.Procedure.Get()).key_proc_time << endl;
   cout << "\tReturn Pressure    " << (_cds.Procedure.Get()).key_return_press << endl;
   cout << "\tDraw Pressure      " << (_cds.Procedure.Get()).key_draw_press << endl;
   cout << "\tCustom Ratio       " << (_cds.Procedure.Get()).key_custom_ratio << endl;
   cout << "\tAC Rate            " << (_cds.Procedure.Get()).key_ac_rate << endl;
   cout << "\tPost Crit          " << (_cds.Procedure.Get()).key_post_crit << endl;
   cout << "\tTBV Vol            " << (_cds.Procedure.Get()).key_tbv_vol_setting << endl;
   cout << "\tTBV Percent        " << (_cds.Procedure.Get()).key_tbv_percent << endl;
#if 1
   cout << "\tWeight Setting     " << (_cds.Procedure.Get()).key_weight_setting << endl;
   cout << "\tLess Than Vol      " << (_cds.Procedure.Get()).key_weight_less_than_vol << endl;
   cout << "\tGreater Than Vol   " << (_cds.Procedure.Get()).key_weight_greater_than_vol << endl;
#else
   cout << "\tWeight Setting     " << (_cds.Procedure.Get()).key_weight_3_setting << endl;
   cout << "\tLess Than Vol      " << (_cds.Procedure.Get()).key_weight_3_less_than_vol << endl;
   cout << "\tGreater Than Vol   " << (_cds.Procedure.Get()).key_weight_3_greater_than_vol << endl;
#endif
   cout << "\tML Per Kg          " << (_cds.Procedure.Get()).key_ml_per_kg << endl;
   cout << "\tPost Platelet      " << (_cds.Procedure.Get()).key_post_plat << endl;
   cout << "\tMax Draw Flow      " << (_cds.Procedure.Get()).key_max_draw_flow << endl;
   cout << "\tMin Replace Vol    " << (_cds.Procedure.Get()).key_min_replacement_volume << endl;
   cout << "\tDRBC Alert         " << (_cds.Procedure.Get()).key_drbc_alert << endl;
   cout << "\tRinseback Protocol " << (_cds.Procedure.Get()).key_plasma_rinseback << endl;
   cout << endl;

   cout << "Predict Values:      " << endl;
   cout << "\tRBC Ratio          " << (_cds.Predict.Get()).key_rbc_ratio << endl;
   cout << "\tRBC Replace Fluid  " << (_cds.Predict.Get()).key_rbc_replace_fluid << endl;
   cout << "\tRBC Fluid Percent  " << (_cds.Predict.Get()).key_rbc_fluid_percent << endl;
   cout << "\tPlasma AMAP Min    " << (_cds.Predict.Get()).key_pls_amap_min << endl;
   cout << "\tPlasma AMAP Max    " << (_cds.Predict.Get()).key_pls_amap_max << endl;
   cout << "\tYSF                " << (_cds.Predict.Get()).key_ysf << endl;
   cout << "\tGuaranteed Yield   " << (_cds.Predict.Get()).key_guaranteed_yield << endl;
   cout << "\tConfidence Level   " << (_cds.Predict.Get()).key_confidence_level << endl;
   cout << "\tCoeff Of Variation " << (_cds.Predict.Get()).key_coefficient_of_variation << endl;
   cout << "\tInlet Management   " << (_cds.Predict.Get()).key_inlet_management << endl;
   cout << "\tInlet Flow Ramp    " << (_cds.Predict.Get()).key_inlet_flow_ramp << endl;
   cout << "\tReturn Mgt         " << (_cds.Predict.Get()).key_return_management << endl;
   cout << "\tRSF                " << (_cds.Predict.Get()).key_rsf << endl;
   cout << endl;

   cout << "Template Values:      " << endl;
   cout << "\tRBC Crit[0]         " << (_cds.Templates.Get()).key_rbc_crit[0] << endl;
   cout << "\tRBC Crit[1]         " << (_cds.Templates.Get()).key_rbc_crit[1] << endl;
   cout << "\tRBC Crit[2]         " << (_cds.Templates.Get()).key_rbc_crit[2] << endl;
   cout << "\tRBC Dose[0]         " << (_cds.Templates.Get()).key_rbc_dose[0] << endl;
   cout << "\tRBC Dose[1]         " << (_cds.Templates.Get()).key_rbc_dose[1] << endl;
   cout << "\tRBC Dose[2]         " << (_cds.Templates.Get()).key_rbc_dose[2] << endl;
   cout << "\tPlatelet Yield[0]   " << (_cds.Templates.Get()).key_plt_yield[0] << endl;
   cout << "\tPlatelet Yield[1]   " << (_cds.Templates.Get()).key_plt_yield[1] << endl;
   cout << "\tPlatelet Yield[2]   " << (_cds.Templates.Get()).key_plt_yield[2] << endl;
   cout << "\tPlatelet Yield[3]   " << (_cds.Templates.Get()).key_plt_yield[3] << endl;
   cout << "\tPlatelet Yield[4]   " << (_cds.Templates.Get()).key_plt_yield[4] << endl;
   cout << "\tPlatelet Yield[5]   " << (_cds.Templates.Get()).key_plt_yield[5] << endl;
   cout << "\tPlatelet Volume[0]  " << (_cds.Templates.Get()).key_plt_volume[0] << endl;
   cout << "\tPlatelet Volume[1]  " << (_cds.Templates.Get()).key_plt_volume[1] << endl;
   cout << "\tPlatelet Volume[2]  " << (_cds.Templates.Get()).key_plt_volume[2] << endl;
   cout << "\tPlatelet Volume[3]  " << (_cds.Templates.Get()).key_plt_volume[3] << endl;
   cout << "\tPlatelet Volume[4]  " << (_cds.Templates.Get()).key_plt_volume[4] << endl;
   cout << "\tPlatelet Volume[5]  " << (_cds.Templates.Get()).key_plt_volume[5] << endl;
   cout << "\tPlasma Volume[0]    " << (_cds.Templates.Get()).key_pls_volume[0] << endl;
   cout << "\tPlasma Volume[1]    " << (_cds.Templates.Get()).key_pls_volume[1] << endl;
   cout << "\tPlasma Volume[2]    " << (_cds.Templates.Get()).key_pls_volume[2] << endl;
   cout << "\tPlasma Volume[3]    " << (_cds.Templates.Get()).key_pls_volume[3] << endl;
   cout << "\tPlasma Volume[4]    " << (_cds.Templates.Get()).key_pls_volume[4] << endl;
   cout << "\tPlasma Volume[5]    " << (_cds.Templates.Get()).key_pls_volume[5] << endl;
   cout << endl;

   cout << "Definition Values:      " << endl;

   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      cout << "\tPlatelet[" << i << "]   " << (_cds.Definition.Get())[i].key_platelet << endl;
   }
   cout << endl;
   
   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      cout << "\tPlasma[" << i << "]     " << (_cds.Definition.Get())[i].key_plasma << endl;
   }
   cout << endl;
   
   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      cout << "\tRBC[" << i << "]        " << (_cds.Definition.Get())[i].key_rbc << endl;
   }
   cout << endl;
   
   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      cout << "\tBlood Type[" << i << "] " << (_cds.Definition.Get())[i].key_blood_type << endl;
   }
   cout << endl;
   
   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      cout << "\tTime[" << i << "]       " << (_cds.Definition.Get())[i].key_time << endl;
   }
   cout << endl;
   
   cout << endl;
   
   for (int i = 0; i < _cds.ProcedureList.numberOfProcedures.Get(); i++)
   {
      cout << "Procedure List Values:      " << endl;
      cout << "\tYield[" << i << "]      " << (_cds.ProcedureList.Procedure.Get())[i].yield << endl;
      cout << "\tPlatelet[" << i << "]   " << (_cds.ProcedureList.Procedure.Get())[i].platelet_volume << endl;
      cout << "\tPlasma[" << i << "]     " << (_cds.ProcedureList.Procedure.Get())[i].plasma_volume << endl;
      cout << "\tRBC Crit[" << i << "]   " << (_cds.ProcedureList.Procedure.Get())[i].rbc_crit << endl;
      cout << "\tRBC Dose[" << i << "]   " << (_cds.ProcedureList.Procedure.Get())[i].rbc_dose << endl;
      cout << "\tTime[" << i << "]       " << (_cds.ProcedureList.Procedure.Get())[i].max_procedure_time << endl;
      cout << "\tBlood Type[" << i << "] " << (_cds.ProcedureList.Procedure.Get())[i].blood_type << endl;
   }
}


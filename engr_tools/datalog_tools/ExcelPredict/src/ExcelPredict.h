/*****************************************************************************\
**
** ExcelPredict.h
**
** Provides the interface to the Trima Predict.
**
\*****************************************************************************/

#ifndef EXCELPREDICT_H
#define EXCELPREDICT_H

/* Includes ******************************************************************/

#include "OfflineOptimizer.h"
#include "predict_strings.h"

/* Macros ********************************************************************/

#define PROCEDURE_FUNCTIONS_BEGIN
#define PROCEDURE_FUNCTIONS_END
#define CONFIG_FUNCTIONS_BEGIN
#define CONFIG_FUNCTIONS_END
#define CONFIGARRAY_FUNCTIONS_BEGIN
#define CONFIGARRAY_FUNCTIONS_END
#define SUBSTATE_FUNCTIONS_BEGIN
#define SUBSTATE_FUNCTIONS_END
#define CONDITION_FUNCTIONS_BEGIN
#define CONDITION_FUNCTIONS_END
#define SOFTWARE_OPTIONS_BEGIN
#define SOFTWARE_OPTIONS_END
/* Classes *******************************************************************/

namespace PREDICT {

template<class _OPJECTTYPE, typename _VALUETYPE>
class Accessor {
public:
	typedef _OPJECTTYPE Object;
	typedef _VALUETYPE Value;

   virtual Value Get(Object &object) const = 0;
};

template<class _OPJECTTYPE, typename _VALUETYPE>
class FunctionAccessor : Accessor<_OPJECTTYPE, _VALUETYPE> {
public:
	FunctionAccessor( Value (procedureDetail::*function)(void) ) : mFunction(function) {}

   virtual Value Get(Object &object) const
	{
		return (object.*mFunction)();
	}

private:
	Value (procedureDetail::*mFunction)(void);
};

template<class _OPJECTTYPE, typename _VALUETYPE>
class ConstFunctionConstAccessor : Accessor<_OPJECTTYPE, _VALUETYPE> {
public:
	ConstFunctionConstAccessor(const Value (procedureDetail::*function)(void) const) : mFunction(function) {}

   virtual Value Get(Object &object) const
	{
		return (object.*mFunction)();
	}

private:
	const Value (procedureDetail::*mFunction)(void) const;
};

template<class _OPJECTTYPE, typename _VALUETYPE>
class ConstFunctionAccessor : Accessor<_OPJECTTYPE, _VALUETYPE> {
public:
	ConstFunctionAccessor(const Value (procedureDetail::*function)(void)) : mFunction(function) {}

   virtual Value Get(Object &object) const
	{
		return (object.*mFunction)();
	}

private:
	const Value (procedureDetail::*mFunction)(void);
};

template<class _OPJECTTYPE, typename _VALUETYPE>
class FunctionConstAccessor : Accessor<_OPJECTTYPE, _VALUETYPE> {
public:
	FunctionConstAccessor(Value (procedureDetail::*function)(void) const) : mFunction(function) {}

   virtual Value Get(Object &object) const
	{
		return (object.*mFunction)();
	}

private:
	Value (procedureDetail::*mFunction)(void) const;
};

template<class _OPJECTTYPE, typename _VALUETYPE>
class MemberAccessor : Accessor<_OPJECTTYPE, _VALUETYPE> {
public:
	MemberAccessor(Value Object::*member) : mMember(member) {}

   virtual Value Get(Object &object) const
	{
		return object.*mMember;
	}

private:
	Value Object::*mMember;
};

template<typename _ACCESSORTYPE, typename _VALUETYPE>
class ProcedureAccess {
public:
	typedef _ACCESSORTYPE Accessor;
	typedef _VALUETYPE Type;

	ProcedureAccess(Accessor accessor) : mAccessor(accessor) {}

   virtual Type Get(PREDICT::OfflineOptimizer *optimizer, int procedure) const
	{
		Type value = 0;

		if (optimizer != NULL) {
			procedureDetail *details = optimizer->GetProcedure(procedure);
			
			if (details != NULL) {
				value = mAccessor.Get(*details);
			}
		}

		return value;
	}

private:
	Accessor mAccessor;
};

template<typename _ACCESSORTYPE, typename _VALUETYPE>
class ProcedureToplevelAccess {
public:
	typedef _ACCESSORTYPE Accessor;
	typedef _VALUETYPE Type;

	ProcedureToplevelAccess(Accessor accessor) : mAccessor(accessor) {}

   virtual Type Get(PREDICT::OfflineOptimizer *optimizer, int procedure) const
	{
		Type value = 0;

		if (optimizer != NULL) {
			procedureDetail *details = optimizer->GetProcedure(procedure);
			
			if (details != NULL) {
				value = mAccessor.Get(details->toplevel);
			}
		}

		return value;
	}

private:
	Accessor mAccessor;
};

template<typename _ACCESSORTYPE, typename _VALUETYPE>
class SubstateAccess {
public:
	typedef _ACCESSORTYPE Accessor;
	typedef _VALUETYPE Type;

	SubstateAccess(Accessor accessor) : mAccessor(accessor) {}

   virtual Type Get(PREDICT::OfflineOptimizer *optimizer, int procedure, int substate) const
	{
		Type value = 0;

		if (optimizer != NULL && substate >= 0 && substate < ALLOCATED_SUBSTATES) {
			procedureDetail *details = optimizer->GetProcedure(procedure);
			
			if (details != NULL) {
                IntermediateResults& ss = const_cast<IntermediateResults&>(details->getSubstate(substate));
				value = mAccessor.Get(ss);
				//value = (Type)ss.AccumTime;
			}
		}
		return value;
	}

private:
	Accessor mAccessor;
};

template<class _CONTAINERTYPE, typename _VALUETYPE>
class ConfigAccess {
public:
	typedef _CONTAINERTYPE Container;
	typedef _VALUETYPE Type;

	ConfigAccess(Container Config_CDS::*container, Type Container::DataType::*member)
			: mContainer(container), mMember(member) {}

	Type Get(PREDICT::OfflineOptimizer *optimizer) const
	{
		return optimizer != NULL ? (optimizer->GetConfigs().*mContainer).Get().*mMember : 0;
	}

	Type Set(PREDICT::OfflineOptimizer *optimizer, Type value)
	{
		if (optimizer != NULL) {
			Container::DataType data = (optimizer->GetConfigs().*mContainer).Get();
			data.*mMember = value;
			(optimizer->GetConfigs().*mContainer).Set(data);
		}

		return this->Get(optimizer);
	}

private:
	Container Config_CDS::*mContainer;
	Type Container::DataType::*mMember;
};

template<class _CONTAINERTYPE, int _SIZE, typename _VALUETYPE>
class ConfigAccessArray {
public:
	typedef _CONTAINERTYPE Container;
	typedef _VALUETYPE Type;

	ConfigAccessArray(Container Config_CDS::*container, Type (Container::DataType::*member)[_SIZE])
			: mContainer(container), mMember(member) {}

	Type Get(PREDICT::OfflineOptimizer *optimizer, int index) const
	{
		return optimizer != NULL && index >= 0 && index < _SIZE
				? ((optimizer->GetConfigs().*mContainer).Get().*mMember)[index]
				: 0;
	}

	Type Set(PREDICT::OfflineOptimizer *optimizer, int index, Type value)
	{
		if (optimizer != NULL && index >= 0 && index < _SIZE) {
			Container::DataType data = (optimizer->GetConfigs().*mContainer).Get();
			(data.*mMember)[index] = value;
			(optimizer->GetConfigs().*mContainer).Set(data);
		}

		return this->Get(optimizer, index);
	}

private:
	Container Config_CDS::*mContainer;
	Type (Container::DataType::*mMember)[_SIZE];
};

template<class _CONTAINERTYPE, int _SIZE, typename _VALUETYPE>
class ConfigAccessCArray {
public:
	typedef _CONTAINERTYPE Container;
	typedef _VALUETYPE Type;

	ConfigAccessCArray(Container Config_CDS::*container, Type Container::DataType::value_type::*member)
			: mContainer(container), mMember(member) {}

	Type Get(PREDICT::OfflineOptimizer *optimizer, int index) const
	{
		return optimizer != NULL && index >= 0 && index < _SIZE
				? (optimizer->GetConfigs().*mContainer).Get()[index].*mMember
				: 0;
	}

	Type Set(PREDICT::OfflineOptimizer *optimizer, int index, Type value)
	{
		if (optimizer != NULL && index >= 0 && index < _SIZE) {
			Container::DataType data = (optimizer->GetConfigs().*mContainer).Get();
			data[index].*mMember = value;
			(optimizer->GetConfigs().*mContainer).Set(data);
		}

		return this->Get(optimizer, index);
	}

private:
	Container Config_CDS::*mContainer;
	Type Container::DataType::value_type::*mMember;
};

template<class _CONTAINERTYPE, typename _VALUETYPE>
class ConditionAccess {
public:
	typedef _CONTAINERTYPE Container;
	typedef _VALUETYPE Type;

	ConditionAccess(Container PREDICT::Conditions::*parent, Type Container::*member)
			: mParent(parent), mMember(member) {}

	Type Get(PREDICT::Conditions &conditions) const
	{
		return conditions.*mParent.*mMember;
	}

	Type Set(PREDICT::Conditions &conditions, Type value)
	{
		conditions.*mParent.*mMember = value;
		return value;
	}

private:
	Container PREDICT::Conditions::*mParent;
	Type Container::*mMember;
};

template<class _OPJECTTYPE, typename _VALUETYPE>
class SoftwareOptionFunctionAccessor : Accessor<_OPJECTTYPE, _VALUETYPE> {
public:
	SoftwareOptionFunctionAccessor(const Value (procedureDetail::*function)(void) const) : mFunction(function) {}

   virtual Value Get(Object &object) const
	{
		return (object.*mFunction)();
	}

private:
	Value (procedureDetail::*mFunction)(void);
};
class PredictAccess {
public:
	PredictAccess(void) :
			RBCState(&procedureDetail::RBCState),
			RBCDoubleState(&procedureDetail::RBCDoubleState),
			VinTarget(&procedureDetail::VinTarget),
			VinPIRStart(&procedureDetail::VinPIRStart),
			YdchPIR(&procedureDetail::YdchPIR),
			PIRLockin(&procedureDetail::getPIRLockin),
			PIRRequired(&procedureDetail::GetPIRRequired),
			doingPTFDouble(&procedureDetail::doingPTFDouble),
			secondPartofPTFDouble(&procedureDetail::secondPartofPTFDouble),
			NumOfSubstates(&procedureDetail::getNumOfSubstates),
			ExtracorporealVolume(&procedureDetail::getExtracorporealVolume),
			ResidualRBCVolume(&procedureDetail::getResidualRBCVolume),
			PrimeDuration(&procedureDetail::getPrimeDuration),
			VinPrime(&procedureDetail::getVinPrime),
			numOfTimes(&procedureDetail::getnumOfTimes),
			convergenceCtr(&procedureDetail::getconvergenceCtr),
			ProcedureNumber(&procedureDetail::getProcedureNumber),
			Designator(&procedureDetail::getDesignator),
			ProcAMAPorMC(&procedureDetail::getProcAMAPorMC),
			ProcBloodType(&procedureDetail::getProcBloodType),
			ProcCollectPlasma(&procedureDetail::getProcCollectPlasma),
			ProcCollectVolFixed(&procedureDetail::getProcCollectVolFixed),
			ProcFixedCollectVol(&procedureDetail::getProcFixedCollectVol),
			ProcFixedPlasmaVol(&procedureDetail::getProcFixedPlasmaVol),
			ProcPlasmaVolFixed(&procedureDetail::getProcPlasmaVolFixed),
			ProcPlateletProdCon(&procedureDetail::getProcPlateletProdCon),
			ProcRBCDose(&procedureDetail::getProcRBCDose),
			ProcRBCHct(&procedureDetail::getProcRBCHct),
			ProcRBCVolume(&procedureDetail::getProcRBCVolume),
			ProcPtfIsSplit(&procedureDetail::getProcPtfIsSplit),
			ProcTime(&procedureDetail::getProcTime),
			DesiredYield(&procedureDetail::getDesiredYield),
			MSSPlt(&procedureDetail::getMSSPlt),
			MSSPltVolume(&procedureDetail::getMSSPltVolume),
			MSSPltPctCarryover(&procedureDetail::getMSSPltPctCarryover),
			OriginalPltVolume(&procedureDetail::getOriginalPltVolume),
			OriginalPltYeild(&procedureDetail::getOriginalPltYeild),
			MSSRbc(&procedureDetail::getMSSRbc),
			MSSRbcVolume(&procedureDetail::getMSSRbcVolume),
			MaxPlasma(&procedureDetail::getMaxPlasma),
			hypovolemic(&procedureDetail::hypovolemic),
			hypovolemiaDistance(&procedureDetail::hypovolemiaDistance),
			HypoMargin(&procedureDetail::getHypoMargin),
			PlasmaTargetCannotChange(&procedureDetail::getPlasmaTargetCannotChange),
			MaxACRate(&procedureDetail::getMaxACRate),
			PredRBCDose(&procedureDetail::getPredRBCDose),
			PredRBCVolume(&procedureDetail::getPredRBCVolume),
			PredRBCCrit(&procedureDetail::getPredRBCCrit),
			RBCCalculatedCrit(&procedureDetail::getRBCCalculatedCrit),
			Status(&procedureDetail::getStatus),
			Optimized(&procedureDetail::getOptimized),
            MaxQin(&procedureDetail::getMaxQin),//This is a change
			PredVc(&procedureDetail::getPredVc),
			PredVp(&procedureDetail::getPredVp),
			PredVac(&procedureDetail::getPredVac),
            PredVrep(&procedureDetail::getPredVrep),
			PredTp(&procedureDetail::getPredTp),

			InfusionRate(&procedureDetail::getInfusionRate),
			Tc(&procedureDetail::getTc),
			Ytarg(&procedureDetail::getYtarg),
			Ec(&procedureDetail::getEc),
			Qinc(&procedureDetail::getQinc),
			Ydpur(&procedureDetail::getYdpur),
			Vintarg(&procedureDetail::getVintarg),
			Vptarg(&procedureDetail::getVptarg),
			Vrtarg(&procedureDetail::getVrtarg),
			Qcmr(&procedureDetail::getQcmr),
			Vpmin(&procedureDetail::getVpmin),
			Vinpur(&procedureDetail::getVinpur),
			Ydch(&procedureDetail::getYdch),


			key_lang(&Config_CDS::LangUnit, &LangUnitCfg::key_lang),
			key_height(&Config_CDS::LangUnit, &LangUnitCfg::key_height),
			key_weight(&Config_CDS::LangUnit, &LangUnitCfg::key_weight),
			key_date_format(&Config_CDS::LangUnit, &LangUnitCfg::key_date_format),
			key_time_format(&Config_CDS::LangUnit, &LangUnitCfg::key_time_format),
			key_decimal_delimiter(&Config_CDS::LangUnit, &LangUnitCfg::key_decimal_delimiter),
			key_crit_or_glob(&Config_CDS::LangUnit, &LangUnitCfg::key_crit_or_glob),

			key_set_time(&Config_CDS::Machine, &BasicMachineCfg::key_set_time),
			key_set_date(&Config_CDS::Machine, &BasicMachineCfg::key_set_date),
			key_set_audio(&Config_CDS::Machine, &BasicMachineCfg::key_set_audio),
			key_pword(&Config_CDS::Machine, &BasicMachineCfg::key_pword),

			key_proc_time(&Config_CDS::Procedure, &ProcedureConfig::key_proc_time),
			key_return_press(&Config_CDS::Procedure, &ProcedureConfig::key_return_press),
			key_draw_press(&Config_CDS::Procedure, &ProcedureConfig::key_draw_press),
			key_custom_ratio(&Config_CDS::Procedure, &ProcedureConfig::key_custom_ratio),
			key_plasma_ratio(&Config_CDS::Procedure, &ProcedureConfig::key_plasma_ratio),
			key_ac_rate(&Config_CDS::Procedure, &ProcedureConfig::key_ac_rate),
			key_post_crit(&Config_CDS::Procedure, &ProcedureConfig::key_post_crit),
			key_tbv_vol_setting(&Config_CDS::Procedure, &ProcedureConfig::key_tbv_vol_setting),
			key_tbv_percent(&Config_CDS::Procedure, &ProcedureConfig::key_tbv_percent),
			key_weight_setting(&Config_CDS::Procedure, &ProcedureConfig::key_weight_setting),
			key_weight_less_than_vol(&Config_CDS::Procedure, &ProcedureConfig::key_weight_less_than_vol),
			key_weight_greater_than_vol(&Config_CDS::Procedure, &ProcedureConfig::key_weight_greater_than_vol),
			key_ml_per_kg(&Config_CDS::Procedure, &ProcedureConfig::key_ml_per_kg),
			key_drbc_body_vol(&Config_CDS::Procedure, &ProcedureConfig::key_drbc_body_vol),
			key_max_plasma_vol_during_platelet_collection(&Config_CDS::Procedure, &ProcedureConfig::key_max_plasma_vol_during_plasma_collection),
			key_max_plasma_vol_during_plasma_collection(&Config_CDS::Procedure, &ProcedureConfig::key_max_plasma_vol_during_platelet_collection),
			key_post_plat(&Config_CDS::Procedure, &ProcedureConfig::key_post_plat),
			key_max_draw_flow(&Config_CDS::Procedure, &ProcedureConfig::key_max_draw_flow),
			key_min_replacement_volume(&Config_CDS::Procedure, &ProcedureConfig::key_min_replacement_volume),
			key_plt_mss_split_notif(&Config_CDS::Procedure, &ProcedureConfig::key_plt_mss_split_notif),
			key_override_pas_bag_vol(&Config_CDS::Procedure, &ProcedureConfig::key_override_pas_bag_vol),
			key_blood_diversion(&Config_CDS::Procedure, &ProcedureConfig::key_blood_diversion),
			key_drbc_alert(&Config_CDS::Procedure, &ProcedureConfig::key_drbc_alert),
			key_drbc_threshold(&Config_CDS::Procedure, &ProcedureConfig::key_drbc_threshold),
			key_plasma_rinseback(&Config_CDS::Procedure, &ProcedureConfig::key_plasma_rinseback),
			key_saline_rinseback(&Config_CDS::Procedure, &ProcedureConfig::key_saline_rinseback),
			key_audit_tracking(&Config_CDS::Procedure, &ProcedureConfig::key_audit_tracking),
			key_air_removal(&Config_CDS::Procedure, &ProcedureConfig::key_air_removal),
			key_mss_plt_on(&Config_CDS::Procedure, &ProcedureConfig::key_mss_plt_on),
			key_mss_rbc_on(&Config_CDS::Procedure, &ProcedureConfig::key_mss_rbc_on),
			key_show_pre_aas_flags(&Config_CDS::Procedure, &ProcedureConfig::key_show_pre_aas_flags),
			key_plt_def_bag_vol(&Config_CDS::Procedure, &ProcedureConfig::key_plt_def_bag_vol),
			key_rbc_def_bag_vol(&Config_CDS::Procedure, &ProcedureConfig::key_rbc_def_bag_vol),

			key_rbc_ratio(&Config_CDS::Predict, &PredictionConfig::key_rbc_ratio),
			key_rbc_replace_fluid(&Config_CDS::Predict, &PredictionConfig::key_rbc_replace_fluid),
			key_rbc_fluid_percent(&Config_CDS::Predict, &PredictionConfig::key_rbc_fluid_percent),
			key_pls_amap_min(&Config_CDS::Predict, &PredictionConfig::key_pls_amap_min),
			key_pls_amap_max(&Config_CDS::Predict, &PredictionConfig::key_pls_amap_max),
			key_ysf(&Config_CDS::Predict, &PredictionConfig::key_ysf),
			key_guaranteed_yield(&Config_CDS::Predict, &PredictionConfig::key_guaranteed_yield),
			key_confidence_level(&Config_CDS::Predict, &PredictionConfig::key_confidence_level),
			key_coefficient_of_variation(&Config_CDS::Predict, &PredictionConfig::key_coefficient_of_variation),
			key_inlet_management(&Config_CDS::Predict, &PredictionConfig::key_inlet_management),
			key_inlet_flow_ramp(&Config_CDS::Predict, &PredictionConfig::key_inlet_flow_ramp),
			key_return_management(&Config_CDS::Predict, &PredictionConfig::key_return_management),
			key_rsf(&Config_CDS::Predict, &PredictionConfig::key_rsf),
			key_male_only_plt(&Config_CDS::Predict, &PredictionConfig::key_male_only_plt),
			key_male_only_plasma(&Config_CDS::Predict, &PredictionConfig::key_male_only_plasma),
            key_ffp_volume(&Config_CDS::Predict, &PredictionConfig::key_ffp_volume),
			key_complementary_plasma(&Config_CDS::Predict, &PredictionConfig::key_complementary_plasma),

            key_rbc_crit(&Config_CDS::Templates, &ProductTemplates::key_rbc_crit),
			key_rbc_dose(&Config_CDS::Templates, &ProductTemplates::key_rbc_dose),
			key_rbc_mss_volume(&Config_CDS::Templates, &ProductTemplates::key_rbc_mss_volume),
			key_rbc_mss(&Config_CDS::Templates, &ProductTemplates::key_rbc_mss),

			key_plt_amap_min_yield(&Config_CDS::Templates, &ProductTemplates::key_plt_amap_min_yield),
			key_plt_amap_max_yield(&Config_CDS::Templates, &ProductTemplates::key_plt_amap_max_yield),
			key_plt_amap_conc(&Config_CDS::Templates, &ProductTemplates::key_plt_amap_conc),


			key_plt_yield(&Config_CDS::Templates, &ProductTemplates::key_plt_yield),
			key_plt_volume(&Config_CDS::Templates, &ProductTemplates::key_plt_volume),
			key_plt_mss(&Config_CDS::Templates, &ProductTemplates::key_plt_mss),
			key_plt_pct_carryover(&Config_CDS::Templates, &ProductTemplates::key_plt_pct_carryover),

			key_pls_volume(&Config_CDS::Templates, &ProductTemplates::key_pls_volume),

			key_platelet(&Config_CDS::Definition, &ProductDefinition::key_platelet),
			key_plasma(&Config_CDS::Definition, &ProductDefinition::key_plasma),
			key_rbc(&Config_CDS::Definition, &ProductDefinition::key_rbc),
			key_blood_type(&Config_CDS::Definition, &ProductDefinition::key_blood_type),
			key_time(&Config_CDS::Definition, &ProductDefinition::key_time),

			Qcc(&IntermediateResults::Qcc),
			Qpc(&IntermediateResults::Qpc),
			T(&IntermediateResults::T),
			Qnret(&IntermediateResults::Qnret),
			Vsvn(&IntermediateResults::Vsvn),
			Tcn(&IntermediateResults::Tcn),
			Td(&IntermediateResults::Td),
			Tr(&IntermediateResults::Tr),
			D(&IntermediateResults::D),
			Qrinst(&IntermediateResults::Qrinst),
			EndVin(&IntermediateResults::EndVin),
			Vp(&IntermediateResults::Vp),
			Vacp(&IntermediateResults::Vacp),
			Vc(&IntermediateResults::Vc),
			Vacc(&IntermediateResults::Vacc),
			Vr(&IntermediateResults::Vr),
			Vacr(&IntermediateResults::Vacr),
			AccumVp(&IntermediateResults::AccumVp),
			AccumVacp(&IntermediateResults::AccumVacp),
			AccumVc(&IntermediateResults::AccumVc),
			AccumVacc(&IntermediateResults::AccumVacc),
			AccumTime(&IntermediateResults::AccumTime),
			Vac(&IntermediateResults::Vac),
			AccumVac(&IntermediateResults::AccumVac),
			Vrep(&IntermediateResults::Vrep),
			AccumVrep(&IntermediateResults::AccumVrep),
			Qrep(&IntermediateResults::Qrep),
			Hin(&IntermediateResults::Hin),
			Facb(&IntermediateResults::Facb),
			Facp(&IntermediateResults::Facp),
			AvgHt(&IntermediateResults::AvgHt),
			Hend(&IntermediateResults::Hend),
			Qininstd(&IntermediateResults::Qininstd),
			Qininstr(&IntermediateResults::Qininstr),
			Qacinst(&IntermediateResults::Qacinst),
			Qcinstd(&IntermediateResults::Qcinstd),
			Qcinstr(&IntermediateResults::Qcinstr),
			Qpinstd(&IntermediateResults::Qpinstd),
			Qpinstr(&IntermediateResults::Qpinstr),
			Qch(&IntermediateResults::Qch),
			HypovolemiaDistance(&IntermediateResults::HypovolemiaDistance),
			HypovolemiaMargin(&IntermediateResults::HypovolemiaMargin),
			Yield(&IntermediateResults::Yield),
			Csf(&IntermediateResults::Csf),
			Rpm(&IntermediateResults::Rpm),

			id(&PREDICT::Conditions::donor, &DECODER::UTIL::Donor::id),
			height(&PREDICT::Conditions::donor, &DECODER::UTIL::Donor::height),
			weight(&PREDICT::Conditions::donor, &DECODER::UTIL::Donor::weight),
			tbv(&PREDICT::Conditions::donor, &DECODER::UTIL::Donor::tbv),
			precount(&PREDICT::Conditions::donor, &DECODER::UTIL::Donor::precount),
			hct(&PREDICT::Conditions::donor, &DECODER::UTIL::Donor::hct),
			bmi(&PREDICT::Conditions::donor, &DECODER::UTIL::Donor::bmi),
			female(&PREDICT::Conditions::donor, &DECODER::UTIL::Donor::female),
			blood_type(&PREDICT::Conditions::donor, &DECODER::UTIL::Donor::blood_type),
			vin(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vin),
			vac(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vac),
			vplasma_bag(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vplasma_bag),
			vplasma_bag_ac(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vplasma_bag_ac),
			vcollect_bag(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vcollect_bag),
			vcollect_bag_ac(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vcollect_bag_ac),
			vrbc_bag(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vrbc_bag),
			vrbc_bag_ac(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vrbc_bag_ac),
			dose_rbc_bag(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::dose_rbc_bag),
			vrep(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vrep),
			vsaline_bolus(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vsaline_bolus),
			platelet_yield(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::platelet_yield),
			time_of_vincv(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::time_of_vincv),
			vincv(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vincv),
			vin_epca_start(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::vin_epca_start),
			rbc_s1(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::rbc_s1),
			rbc_s2(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::rbc_s2),
			rbc_prime(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::rbc_prime),
			rbc_s1_double(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::rbc_s1_double),
			rbc_s2_double(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::rbc_s2_double),
			rbc_double(&PREDICT::Conditions::volumes, &DECODER::UTIL::Volumes::rbc_double),
			substate(&PREDICT::Conditions::run, &DECODER::UTIL::Run::substate),
			qch_min(&PREDICT::Conditions::run, &DECODER::UTIL::Run::qch_min),
			qch_cur(&PREDICT::Conditions::run, &DECODER::UTIL::Run::qch_cur),
			npurges(&PREDICT::Conditions::run, &DECODER::UTIL::Run::npurges),
			adjusted_hct(&PREDICT::Conditions::run, &DECODER::UTIL::Run::adjusted_hct),
			current_recovery(&PREDICT::Conditions::run, &DECODER::UTIL::Run::current_recovery),
			vin_clearline(&PREDICT::Conditions::run, &DECODER::UTIL::Run::vin_clearline),
			wbc_purge_start_time(&PREDICT::Conditions::run, &DECODER::UTIL::Run::wbc_purge_start_time),
			wbc_purge_type(&PREDICT::Conditions::run, &DECODER::UTIL::Run::wbc_purge_type),

            substate_start_time(&PREDICT::Conditions::run, &DECODER::UTIL::Run::substate_start_time),
			midrun_entered(&PREDICT::Conditions::run, &DECODER::UTIL::Run::midrun_entered),

            proctime(&PREDICT::Conditions::time, &DECODER::UTIL::Time::proctime),
			type(&PREDICT::Conditions::cassette, &DECODER::UTIL::Cassette::type),
			rbc_detector_calibrated(&PREDICT::Conditions::cassette, &DECODER::UTIL::Cassette::rbc_detector_calibrated),
			qin(&PREDICT::Conditions::adjust, &DECODER::UTIL::Adjustments::qin),
			qrp(&PREDICT::Conditions::adjust, &DECODER::UTIL::Adjustments::qrp),
			ir(&PREDICT::Conditions::adjust, &DECODER::UTIL::Adjustments::ir),
			ratio(&PREDICT::Conditions::adjust, &DECODER::UTIL::Adjustments::ratio)
	{
	}

	PROCEDURE_FUNCTIONS_BEGIN
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> RBCState;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> RBCDoubleState;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> VinTarget;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> VinPIRStart;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> YdchPIR;
    //NOPIR
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> PIRLockin;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> PIRRequired;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, bool>, bool> doingPTFDouble;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, bool>, bool> secondPartofPTFDouble;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> NumOfSubstates;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> ExtracorporealVolume;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> ResidualRBCVolume;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> PrimeDuration;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> VinPrime;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> numOfTimes;
	ProcedureAccess<FunctionAccessor<procedureDetail, int>, int> convergenceCtr;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> ProcedureNumber;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> Designator;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> ProcAMAPorMC;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> ProcBloodType;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, bool>, bool> ProcCollectPlasma;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, bool>, bool> ProcCollectVolFixed;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> ProcFixedCollectVol;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> ProcFixedPlasmaVol;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> ProcPlasmaVolFixed;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> ProcPlateletProdCon;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> ProcRBCDose;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> ProcRBCHct;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> ProcRBCVolume;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, bool>, bool> ProcPtfIsSplit;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> ProcTime;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> DesiredYield;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, bool>, bool> MSSPlt;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> MSSPltVolume;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> MSSPltPctCarryover;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> OriginalPltVolume;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> OriginalPltYeild;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, bool>, bool> MSSRbc;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> MSSRbcVolume;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> MaxPlasma;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> hypovolemic;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> hypovolemiaDistance;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> HypoMargin;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, int>, int> PlasmaTargetCannotChange;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> MaxACRate;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> PredRBCDose;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> PredRBCVolume;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> PredRBCCrit;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> RBCCalculatedCrit;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, INTERNAL_PREDICT_RESULTS>, int> Status;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, bool>, bool> Optimized;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> MaxQin;
    ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> PredVc;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> PredVp;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> PredVac;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> PredVrep;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> PredTp;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> InfusionRate;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Tc;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Ytarg;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Ec;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Qinc;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Ydpur;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Vintarg;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Vptarg;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Vrtarg;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Qcmr;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Vpmin;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Vinpur;
	ProcedureAccess<FunctionConstAccessor<procedureDetail, float>, float> Ydch;
	PROCEDURE_FUNCTIONS_END

	CONFIG_FUNCTIONS_BEGIN
	ConfigAccess<LangConfigElement, int> key_lang;
	ConfigAccess<LangConfigElement, int> key_height;
	ConfigAccess<LangConfigElement, int> key_weight;
	ConfigAccess<LangConfigElement, int> key_date_format;
	ConfigAccess<LangConfigElement, int> key_time_format;
	ConfigAccess<LangConfigElement, int> key_decimal_delimiter;
	ConfigAccess<LangConfigElement, int> key_crit_or_glob;
	ConfigAccess<BasicMachineElement, int> key_set_time;
	ConfigAccess<BasicMachineElement, int> key_set_date;
	ConfigAccess<BasicMachineElement, int> key_set_audio;
	ConfigAccess<BasicMachineElement, int> key_pword;
	ConfigAccess<ProcedureElement, int> key_proc_time;
	ConfigAccess<ProcedureElement, float> key_return_press;
	ConfigAccess<ProcedureElement, float> key_draw_press;
	ConfigAccess<ProcedureElement, float> key_custom_ratio;
	ConfigAccess<ProcedureElement, float> key_plasma_ratio;
	ConfigAccess<ProcedureElement, int> key_ac_rate;
	ConfigAccess<ProcedureElement, float> key_post_crit;
	ConfigAccess<ProcedureElement, int> key_tbv_vol_setting;
	ConfigAccess<ProcedureElement, float> key_tbv_percent;
	ConfigAccess<ProcedureElement, float> key_weight_setting;
	ConfigAccess<ProcedureElement, float> key_weight_less_than_vol;
	ConfigAccess<ProcedureElement, float> key_weight_greater_than_vol;
	ConfigAccess<ProcedureElement, float> key_ml_per_kg;
	ConfigAccess<ProcedureElement, float> key_drbc_body_vol;
	ConfigAccess<ProcedureElement, float> key_max_plasma_vol_during_plasma_collection;
    ConfigAccess<ProcedureElement, float> key_max_plasma_vol_during_platelet_collection;
	ConfigAccess<ProcedureElement, float> key_post_plat;
	ConfigAccess<ProcedureElement, int> key_max_draw_flow;
	ConfigAccess<ProcedureElement, int> key_min_replacement_volume;
	ConfigAccess<ProcedureElement, int> key_plt_mss_split_notif;
	ConfigAccess<ProcedureElement, int> key_override_pas_bag_vol;
	ConfigAccess<ProcedureElement, int> key_drbc_alert;
	ConfigAccess<ProcedureElement, int> key_drbc_threshold;
	ConfigAccess<ProcedureElement, int> key_plasma_rinseback;
	ConfigAccess<ProcedureElement, int> key_saline_rinseback;
	ConfigAccess<ProcedureElement, int> key_audit_tracking;
	ConfigAccess<ProcedureElement, int> key_air_removal;
	ConfigAccess<ProcedureElement, int> key_blood_diversion;
	ConfigAccess<ProcedureElement, int> key_mss_plt_on;
	ConfigAccess<ProcedureElement, int> key_mss_rbc_on;
	ConfigAccess<ProcedureElement, int> key_show_pre_aas_flags;
	ConfigAccess<ProcedureElement, int> key_plt_def_bag_vol;
	ConfigAccess<ProcedureElement, int> key_rbc_def_bag_vol; 
	ConfigAccess<PredictionElement, int> key_rbc_ratio;
	ConfigAccess<PredictionElement, int> key_rbc_replace_fluid;
	ConfigAccess<PredictionElement, float> key_rbc_fluid_percent;
	ConfigAccess<PredictionElement, int> key_pls_amap_min;
	ConfigAccess<PredictionElement, int> key_pls_amap_max;
	ConfigAccess<PredictionElement, float> key_ysf;
	ConfigAccess<PredictionElement, int> key_guaranteed_yield;
	ConfigAccess<PredictionElement, int> key_confidence_level;
	ConfigAccess<PredictionElement, float> key_coefficient_of_variation;
	ConfigAccess<PredictionElement, int> key_inlet_flow_ramp;
	ConfigAccess<PredictionElement, int> key_inlet_management;
	ConfigAccess<PredictionElement, int> key_return_management;
	ConfigAccess<PredictionElement, float> key_rsf;
	ConfigAccess<PredictionElement, int> key_male_only_plt;
	ConfigAccess<PredictionElement, int> key_male_only_plasma;
	ConfigAccess<PredictionElement, int> key_complementary_plasma;
	ConfigAccess<PredictionElement, int> key_ffp_volume;
	CONFIG_FUNCTIONS_END

	CONFIGARRAY_FUNCTIONS_BEGIN
	ConfigAccessArray<ProductTemplatesElement, NUM_CRIT_DOSE, float> key_rbc_crit;
	ConfigAccessArray<ProductTemplatesElement, NUM_CRIT_DOSE, float> key_rbc_dose;
	ConfigAccessArray<ProductTemplatesElement, NUM_CRIT_DOSE, int> key_rbc_mss;
	ConfigAccessArray<ProductTemplatesElement, NUM_CRIT_DOSE, int> key_rbc_mss_volume;

	ConfigAccessArray<ProductTemplatesElement, NUM_PLT_AMAP, float> key_plt_amap_min_yield;
	ConfigAccessArray<ProductTemplatesElement, NUM_PLT_AMAP, float> key_plt_amap_max_yield;
	ConfigAccessArray<ProductTemplatesElement, NUM_PLT_AMAP, float> key_plt_amap_conc;

	ConfigAccessArray<ProductTemplatesElement, NUM_YIELD_VOLUME, float> key_plt_yield;
	ConfigAccessArray<ProductTemplatesElement, NUM_YIELD_VOLUME, float> key_plt_volume;
	ConfigAccessArray<ProductTemplatesElement, NUM_YIELD_VOLUME, int> key_plt_mss;
	ConfigAccessArray<ProductTemplatesElement, NUM_YIELD_VOLUME, int> key_plt_pct_carryover;
	ConfigAccessArray<ProductTemplatesElement, NUM_PLASMA_VOLUME, float> key_pls_volume;

	ConfigAccessCArray<ProductDefinitionElement, NUM_OFFLINE_PROCEDURES, int> key_platelet;
	ConfigAccessCArray<ProductDefinitionElement, NUM_OFFLINE_PROCEDURES, int> key_plasma;
	ConfigAccessCArray<ProductDefinitionElement, NUM_OFFLINE_PROCEDURES, int> key_rbc;
	ConfigAccessCArray<ProductDefinitionElement, NUM_OFFLINE_PROCEDURES, int> key_blood_type;
	ConfigAccessCArray<ProductDefinitionElement, NUM_OFFLINE_PROCEDURES, float> key_time;
	CONFIGARRAY_FUNCTIONS_END

	SUBSTATE_FUNCTIONS_BEGIN
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qcc;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qpc;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> T;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qnret;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Vsvn;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Tcn;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Td;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Tr;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> D;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qrinst;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> EndVin;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Vp;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Vacp;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Vc;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Vacc;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Vr;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Vacr;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> AccumVp;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> AccumVacp;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> AccumVc;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> AccumVacc;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> AccumTime;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Vac;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> AccumVac;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Vrep;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> AccumVrep;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qrep;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Hin;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Facb;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Facp;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> AvgHt;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Hend;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qininstd;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qininstr;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qacinst;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qcinstd;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qcinstr;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qpinstd;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qpinstr;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Qch;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> HypovolemiaDistance;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> HypovolemiaMargin;
    SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Yield;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Csf;
	SubstateAccess<MemberAccessor<IntermediateResults, float>, float> Rpm;
    SUBSTATE_FUNCTIONS_END

    CONDITION_FUNCTIONS_BEGIN
	ConditionAccess<DECODER::UTIL::Donor, int> id;
	ConditionAccess<DECODER::UTIL::Donor, float> height;
	ConditionAccess<DECODER::UTIL::Donor, float> weight;
	ConditionAccess<DECODER::UTIL::Donor, float> tbv;
	ConditionAccess<DECODER::UTIL::Donor, float> precount;
	ConditionAccess<DECODER::UTIL::Donor, float> hct;
	ConditionAccess<DECODER::UTIL::Donor, float> bmi;
	ConditionAccess<DECODER::UTIL::Donor, int> female;
	ConditionAccess<DECODER::UTIL::Donor, int> blood_type;
	ConditionAccess<DECODER::UTIL::Volumes, float> vin;
	ConditionAccess<DECODER::UTIL::Volumes, float> vac;
	ConditionAccess<DECODER::UTIL::Volumes, float> vplasma_bag;
	ConditionAccess<DECODER::UTIL::Volumes, float> vplasma_bag_ac;
	ConditionAccess<DECODER::UTIL::Volumes, float> vcollect_bag;
	ConditionAccess<DECODER::UTIL::Volumes, float> vcollect_bag_ac;
	ConditionAccess<DECODER::UTIL::Volumes, float> vrbc_bag;
	ConditionAccess<DECODER::UTIL::Volumes, float> vrbc_bag_ac;
	ConditionAccess<DECODER::UTIL::Volumes, float> dose_rbc_bag;
	ConditionAccess<DECODER::UTIL::Volumes, float> vrep;
	ConditionAccess<DECODER::UTIL::Volumes, float> vsaline_bolus;
	ConditionAccess<DECODER::UTIL::Volumes, float> platelet_yield;
	ConditionAccess<DECODER::UTIL::Volumes, float> time_of_vincv;
	ConditionAccess<DECODER::UTIL::Volumes, float> vincv;
	ConditionAccess<DECODER::UTIL::Volumes, float> vin_epca_start;
	ConditionAccess<DECODER::UTIL::Volumes, float> rbc_s1;
	ConditionAccess<DECODER::UTIL::Volumes, float> rbc_s2;
	ConditionAccess<DECODER::UTIL::Volumes, float> rbc_prime;
	ConditionAccess<DECODER::UTIL::Volumes, float> rbc_s1_double;
	ConditionAccess<DECODER::UTIL::Volumes, float> rbc_s2_double;
	ConditionAccess<DECODER::UTIL::Volumes, float> rbc_double;
	ConditionAccess<DECODER::UTIL::Run, int> substate;
	ConditionAccess<DECODER::UTIL::Run, float> qch_min;
	ConditionAccess<DECODER::UTIL::Run, float> qch_cur;
	ConditionAccess<DECODER::UTIL::Run, int> npurges;
	ConditionAccess<DECODER::UTIL::Run, float> adjusted_hct;
	ConditionAccess<DECODER::UTIL::Run, int> current_recovery;
	ConditionAccess<DECODER::UTIL::Run, float> vin_clearline;
	ConditionAccess<DECODER::UTIL::Run, float> wbc_purge_start_time;
	ConditionAccess<DECODER::UTIL::Run, int> wbc_purge_type;
	ConditionAccess<DECODER::UTIL::Run, float> substate_start_time;
	ConditionAccess<DECODER::UTIL::Run, int> midrun_entered;
	ConditionAccess<DECODER::UTIL::Time, float> proctime;
	ConditionAccess<DECODER::UTIL::Cassette, int> type;
	ConditionAccess<DECODER::UTIL::Cassette, bool> rbc_detector_calibrated;
	ConditionAccess<DECODER::UTIL::Adjustments, float> qin;
	ConditionAccess<DECODER::UTIL::Adjustments, float> qrp;
	ConditionAccess<DECODER::UTIL::Adjustments, float> ir;
	ConditionAccess<DECODER::UTIL::Adjustments, float> ratio;
	CONDITION_FUNCTIONS_END

	SOFTWARE_OPTIONS_BEGIN

	SOFTWARE_OPTIONS_END
};

} // namespace PREDICT

class PredictInstance {
public:
	static PredictInstance &GetInstance(void) 
	{
		static PredictInstance instance;

		return instance;
	}

	int LoadOptimizerFromLog(char* dataLogPath, char* cobeConfigPath)
	{
		PREDICT::PredictRecordMrg manager;

		PREDICT::OfflineOptimizer::DestroyOptimizer(mOfflineOptimizerP);
		mOfflineOptimizerP = NULL;
		PREDICT::OfflineOptimizer::CreateOptimizerFromLog(
				dataLogPath, cobeConfigPath, manager, &mOfflineOptimizerP);

		if (mOfflineOptimizerP != NULL) {
			mPredictConditions = manager.GetRecord(0);
			mOfflineOptimizerP->UpdateConditions(mPredictConditions);
		}

		return mOfflineOptimizerP != NULL;
	}

	int LoadOptimizerFromConfig(char* userConfigPath, char* cobeConfigPath)
	{
		PREDICT::OfflineOptimizer::DestroyOptimizer(mOfflineOptimizerP);
		mOfflineOptimizerP = NULL;
		PREDICT::OfflineOptimizer::CreateOptimizerFromConfig(
				userConfigPath, cobeConfigPath, &mOfflineOptimizerP);

		if (mOfflineOptimizerP != NULL) {
			mOfflineOptimizerP->UpdateConditions(mPredictConditions);
		}

		return mOfflineOptimizerP != NULL;
	}

	int LoadOptimizerFromConfig(char* userConfigPath, char* cobeConfigPath, char* softwareConfigPath)
	{
		PREDICT::OfflineOptimizer::DestroyOptimizer(mOfflineOptimizerP);
		mOfflineOptimizerP = NULL;
		PREDICT::OfflineOptimizer::CreateOptimizerFromConfig(
				userConfigPath, cobeConfigPath, softwareConfigPath, &mOfflineOptimizerP);

		if (mOfflineOptimizerP != NULL) {
			mOfflineOptimizerP->UpdateConditions(mPredictConditions);
		}

		return mOfflineOptimizerP != NULL;
	}

	int UpdateConditions(void)
	{
		if (mOfflineOptimizerP != NULL) {
			mOfflineOptimizerP->UpdateConditions(mPredictConditions); 
		}

		return mOfflineOptimizerP != NULL;
	}

	int UpdateConfigurations(void)
	{
		if (mOfflineOptimizerP != NULL) {
			mOfflineOptimizerP->GetConfigs().CopyOfflineProcedureList();
			mOfflineOptimizerP->GetPredict().FillInConfig();
			mOfflineOptimizerP->GetPredict().FillInProcedureConfig();
		}

		return mOfflineOptimizerP != NULL;
	}

	int OptimizeProduct(int product)
	{
		procedureDetail *details = NULL;

		if (mOfflineOptimizerP != NULL) {
			details = mOfflineOptimizerP->GetOptimizeProcedure(product); 
		}

		return mOfflineOptimizerP != NULL && details != NULL;
	}

	PREDICT::OfflineOptimizer *GetOptimizer(void) { return mOfflineOptimizerP; }
	PREDICT::Conditions &GetConditions(void) { return mPredictConditions; }
	PREDICT::PredictAccess &GetAccessor(void) { return mPredictAccessor; }
	Software_CDS& GetSoftwareCDS(void){ return mSoftwareCDS; }
private:
	PredictInstance(void) : mOfflineOptimizerP(NULL),mSoftwareCDS(Software_CDS::getRWCds()) 
	{
		std::memset(&mPredictConditions, 0, sizeof(mPredictConditions));
	}
		
	~PredictInstance(void) 
	{
		PREDICT::OfflineOptimizer::DestroyOptimizer(mOfflineOptimizerP);
	}

	PREDICT::OfflineOptimizer *mOfflineOptimizerP;
	PREDICT::Conditions mPredictConditions;
	PREDICT::PredictAccess mPredictAccessor;
	Software_CDS& mSoftwareCDS;
};

#endif
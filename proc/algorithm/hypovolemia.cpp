/*
 * Copyright (c) 2000 Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      hypovolemia.cpp
 *             Focussed system hypovolemia functions.
 *
 * AUTHOR:     R.E. Edwards
 *
 * ABSTRACT:   This file encapsulates the hypovolemia algorithm.
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 * EXAMPLE:
 *
 */

#include <vxworks.h>
#include "hypovolemia.h"
#include "cobeconfig.h"
#include "an_alarm.h"
#include "trima_datalog.h"
#include "states.h"
#include "procdata.h"
#include "software_cds.h"

#include "run_defs.h"
#include "cfg_ids.h"
#include "safproc.h"

// ----------------------------------------------------------------------------
// SPECIFICATION:    Hypovolemia::Hypovolemia
//                   Parameters: None
//                   Returns: None
//
//                   Description: Constructor for the Hypovolemia class.
//
//                   Error Handling: None
//

Hypovolemia::Hypovolemia()
   : _Initialized(0),
     _RemovalSetting(0),
     _WeightUnits(0),
     _MaxPercentRemoved(0.0f),
     _TBV(0.0f),
     _TBVNadler(0.0f),
     _DonorWeight(0.0f),
     _RemoveVolumeWeight(0.0f),
     _RemoveVolumeGreaterThan(0.0f),
     _RemoveVolumeLessThan(0.0f),
     _RemoveVolumePlatelet(0.0f),
     _RemoveVolumePlasma(0.0f),
     _MlPerKg(0.0f),
     _Cassette(cassette::UNKNOWN),
     _Config_CDS(ROLE_RO),
     _ProcDonor_CDS(ROLE_RO)
{}
// ----------------------------------------------------------------------------
// SPECIFICATION:    Hypovolemia::~Hypovolemia
//                   Parameters: None
//                   Returns: None
//                   Description: Destructor for Hypovolemia class.
//                   Error Handling: None
//
Hypovolemia::~Hypovolemia()
{}


// ----------------------------------------------------------------------------
// SPECIFICATION:    Hypovolemia::initialize
//                   Parameters: bunches
//                   Returns: None
//                   Description: reinitialization of member data
//
//                   Error Handling: None
//
void Hypovolemia::initialize (const cassette::cassetteType currentcassette)
{
   DataLog(log_level_proc_debug) << "In Hypovolemia::initialize "
                                 << " Cassette Type:" << currentcassette
                                 << endmsg;

   _WeightUnits             = _Config_CDS.LangUnit.Get().key_weight;

   _RemovalSetting          = _Config_CDS.Procedure.Get().key_tbv_vol_setting;
   _MaxPercentRemoved       = _Config_CDS.Procedure.Get().key_tbv_percent;

   _RemoveVolumeWeight      = _Config_CDS.Procedure.Get().key_weight_setting;
   _RemoveVolumeGreaterThan = _Config_CDS.Procedure.Get().key_weight_greater_than_vol;
   _RemoveVolumeLessThan    = _Config_CDS.Procedure.Get().key_weight_less_than_vol;
   _RemoveVolumePlatelet    = _Config_CDS.Procedure.Get().key_max_plasma_vol_during_platelet_collection;
   _RemoveVolumePlasma      = _Config_CDS.Procedure.Get().key_max_plasma_vol_during_plasma_collection;

   _MlPerKg                 = _Config_CDS.Procedure.Get().key_ml_per_kg;

   _Cassette                = currentcassette;

   //
   //
   //   Choose control-side remove volume limit depending on TBV calculation switch.
   //
   const bool ogawa = Software_CDS::GetInstance().getFeature(JapanFeatures);

   _TBV = _TBVNadler = _ProcDonor_CDS.TBV.Get();

   if (ogawa)
   {
      _TBV = _ProcDonor_CDS.TBVOgawa.Get();
      DataLog(log_level_proc_debug) << "Hypovolemia: overriding TBV from " << _TBVNadler
                                    << " to use Ogawa TBV of " << _TBV << endmsg;
   }

   _DonorWeight = _ProcDonor_CDS.Weight.Get();

   _Initialized = 1;

   // DataLog( log_level_proc_debug ) << "Out of Hypovolemia::initialize " << endmsg;

}

//
// ----------------------------------------------------------------------------
//
const float Hypovolemia::SafetyLimit () const
{
   if (!_Initialized)
   {
      anAlarmMsg alarm(HYPOVOLEMIA_NOT_INITIALIZED);
      alarm.setAlarm();
      return 0.0f;
   }

   float maxpercent = 0.15f; // default
   if (Software_CDS::GetInstance().getFeature(JapanFeatures))
   {
      maxpercent = SAFETY_EC_LIMIT;
   }

   return (maxpercent * _TBVNadler); // force Nadler in case Ogawa is in effect
}

// ----------------------------------------------------------------------------
const float Hypovolemia::ControlLimit (const float Vcol, const float Vp) const
{
   if (!_Initialized)
   {
      anAlarmMsg alarm(HYPOVOLEMIA_NOT_INITIALIZED);
      alarm.setAlarm();
      return 0.0f;
   }

   //
   //
   //   Convert donor weight into lbs if needed.
   //
   float donorweight = _DonorWeight;

   if (_WeightUnits == KG_WEIGHT)
      donorweight *= 2.2f;

   float remove_volume_limit = 0.0f;

   //
   //
   //   Check for TBV removal limits if specified. (available for both Japan and worldwide)
   //
   if ((_RemovalSetting & VOLUME_REMOVAL_TBV) != 0)
   {
      remove_volume_limit = (_MaxPercentRemoved / 100.0f) * _TBV;
   }

   //
   //
   //   Volume removal and ml/kg are not available in Japan
   //
   const bool japan = Software_CDS::GetInstance().getFeature(JapanFeatures);
   if (!japan)
   {
      //
      //
      //   Check for volume removal limits if specified.  Note that the
      //   remove volume weight is always in lbs.
      //
      if ( (_RemovalSetting & VOLUME_REMOVAL_WEIGHT) != 0)
      {
         float weight_limit;
         if (donorweight <= _RemoveVolumeWeight)
            weight_limit = _RemoveVolumeLessThan;
         else
            weight_limit = _RemoveVolumeGreaterThan;

         //
         //
         //  Choose the lesser of the limits
         //
         if (remove_volume_limit <= 0.0f)
            remove_volume_limit = weight_limit;

         if (weight_limit < remove_volume_limit)
            remove_volume_limit = weight_limit;
      }

      //
      //
      //   Check for ml/kg removal limits if specified.  Remember donorweight
      //   is in lbs so divide by 2.2 to get kg.
      //
      if ((_RemovalSetting & VOLUME_REMOVAL_ML_PER_KG) != 0)
      {
         float limit = _MlPerKg * (donorweight / 2.2f);

         //
         //
         //  Choose the lesser of the limits
         //
         if (remove_volume_limit <= 0.0f)
            remove_volume_limit = limit;

         if (limit < remove_volume_limit)
            remove_volume_limit = limit;
      }
   }
   else
   {

      //
      //
      //   Japan specific volume removal limits
      //
      if ((_RemovalSetting & VOLUME_REMOVAL_PLATELET_VOL) != 0)
      {
         if (Vcol > 0.0f)
         {
            float limit = _RemoveVolumePlatelet;

            if (remove_volume_limit <= 0.0f)
               remove_volume_limit = limit;

            if (limit < remove_volume_limit)
               remove_volume_limit = limit;
         }
      }

      if ((_RemovalSetting & VOLUME_REMOVAL_PLASMA_VOL) != 0)
      {
         if (Vp > 0.0f)
         {
            float limit = _RemoveVolumePlasma;

            if (remove_volume_limit <= 0.0f)
               remove_volume_limit = limit;

            if (limit < remove_volume_limit)
               remove_volume_limit = limit;
         }
      }
   } // if not japan ... else

   //
   //
   //   Regardless of user selection always enforce 15% TBV maximum removal limit
   //
   float limit = (0.15f * _TBVNadler);  // force Nadler in case Ogawa is in effect
   if (remove_volume_limit <= 0.0f)
      remove_volume_limit = limit;

   if (limit < remove_volume_limit)
      remove_volume_limit = limit;


   return remove_volume_limit;
}

//
//
// ----------------------------------------------------------------------------
const float Hypovolemia::SafetyVolumeRemaining (const int thisIsTheCurrentProcedure,
                                                const float overdrawVolume,
                                                const float Vcol, const float Vac_col,
                                                const float Vp,   const float Vac_p,
                                                const float Vrbc, const float Vac_rbc,
                                                const float Vrep, const float Vac) const
{
   if (!_Initialized)
   {
      anAlarmMsg alarm(HYPOVOLEMIA_NOT_INITIALIZED);
      alarm.setAlarm();
      return 0.0f;
   }

   const float safety_limit = SafetyLimit();

   //
   //
   //   Calculate extracorporeal volume.  Note safety includes first return volume for total
   //   set volume determination, so Cobe Config numbers need to be adjusted by reservoir volume.
   //
   float ECVolume = ( (_Cassette == cassette::PLTPLSRBC) ?
                      CobeConfig::data().HypovolemiaFCVwhite :
                      CobeConfig::data().HypovolemiaFCVblack) - CobeConfig::data().ReservoirVolume;


   float DrawMargin              = MAX_ALLOWABLE_SAFETY_DRAW_CYCLE_VOLUME - CobeConfig::data().ReservoirVolume;

   float safety_volume_remaining = safety_limit
                                   - ( Vcol
                                       + Vp
                                       + Vrbc
                                       - Vrep
                                       - Vac)
                                   - ECVolume
                                   - DrawMargin;
   return safety_volume_remaining;
}

//
//
// ----------------------------------------------------------------------------
const float Hypovolemia::ControlVolumeRemaining (const int thisIsTheCurrentProcedure,
                                                 const float overdrawVolume,
                                                 const float Vcol, const float Vac_col,
                                                 const float Vp,   const float Vac_p,
                                                 const float Vrbc, const float Vac_rbc,
                                                 const float Vrep, const float Vac) const
{
   if (!_Initialized)
   {
      anAlarmMsg alarm(HYPOVOLEMIA_NOT_INITIALIZED);
      alarm.setAlarm();
      return 0.0f;
   }

   const float control_limit  = ControlLimit(Vcol, Vp);
   float       removed_volume = (Vcol - Vac_col +
                                 Vp - Vac_p +
                                 Vrbc - Vac_rbc);

   return (control_limit - removed_volume);
}
// ----------------------------------------------------------------------------
// SPECIFICATION:    Hypovolemia::VolumeRemainingToHypovolemia
//                   Parameters:varies
//                   Returns: float
//                   Description: This method will return the volume remaining
//                                until hypovolemia is detected.
//                   Error Handling: Alarm if object not initialized
//
const float Hypovolemia::VolumeRemainingToHypovolemia (const int thisIsTheCurrentProcedure,
                                                       const float overdrawVolume,
                                                       const float Vcol, const float Vac_col,
                                                       const float Vp,   const float Vac_p,
                                                       const float Vrbc, const float Vac_rbc,
                                                       const float Vrep, const float Vac) const
{
   if (!_Initialized)
   {
      anAlarmMsg alarm(HYPOVOLEMIA_NOT_INITIALIZED);
      alarm.setAlarm();
      return 0.0f;
   }

   float control_volume_remaining = ControlVolumeRemaining(thisIsTheCurrentProcedure,
                                                           overdrawVolume,
                                                           Vcol, Vac_col,
                                                           Vp,   Vac_p,
                                                           Vrbc, Vac_rbc,
                                                           Vrep, Vac);
   float safety_volume_remaining = SafetyVolumeRemaining(thisIsTheCurrentProcedure,
                                                         overdrawVolume,
                                                         Vcol, Vac_col,
                                                         Vp,   Vac_p,
                                                         Vrbc, Vac_rbc,
                                                         Vrep, Vac);


   // Compare the control calculated hypovolemia volume left to the safety
   // hypovolemia volume left to determine which one will be hit first.
   float volume_remaining = ( control_volume_remaining < safety_volume_remaining ) ?
                            control_volume_remaining : safety_volume_remaining;

   // Fix for IT 7888:  During draw cycles, we now deduct reservior volume over 72 mL
   // from the volume buffer to hypovolemia calculated above
   volume_remaining -= overdrawVolume;

   if (overdrawVolume > 0)
      DataLog(log_level_proc_debug) << "Hypovolemia::VolumeRemainingToHypovolemia() sees overdraw; "
                                    << volume_remaining << " mL now remaining to hypovolemia." << endmsg;

   return volume_remaining;
}


// ----------------------------------------------------------------------------
// SPECIFICATION:    Hypovolemia::log
//                   Parameters:none
//                   Returns: void
//                   Description: This method will log the hypovolemia values
//                                to both the run_data and trace logs.
//                   Error Handling: None
//
void Hypovolemia::log () const
{

   const bool japan = Software_CDS::GetInstance().getFeature(JapanFeatures);
   DataLog(log_level_proc_info).flags(std::ios::showpoint | std::ios::fixed);
   DataLog(log_level_proc_info).precision(2);
   DataLog(log_level_proc_info) << "Hypovolemia: ";

   if ((_RemovalSetting & VOLUME_REMOVAL_TBV) != 0)
   {
      DataLog(log_level_proc_info) << "using " << (_MaxPercentRemoved) << "%TBV limits; ";
   }

   if (!japan && (_RemovalSetting & VOLUME_REMOVAL_WEIGHT) != 0)
   {
      DataLog (log_level_proc_info) << "using ("
                                    << _RemoveVolumeWeight << "/" << _RemoveVolumeLessThan << " "
                                    << _RemoveVolumeGreaterThan << ") weight limits; ";
   }

   if (!japan && (_RemovalSetting & VOLUME_REMOVAL_ML_PER_KG) != 0)
   {
      DataLog(log_level_proc_info) << "using " << _MlPerKg << " ml/kg limits; ";
   }

   if (japan && (_RemovalSetting & VOLUME_REMOVAL_PLATELET_VOL) != 0)
   {
      DataLog(log_level_proc_info) << "using " << _RemoveVolumePlatelet << " platelet volume limits; ";
   }

   if (japan && (_RemovalSetting & VOLUME_REMOVAL_PLASMA_VOL) != 0)
   {
      DataLog(log_level_proc_info) << "using " << _RemoveVolumePlasma << " plasma volume limits; ";
   }

   // Can no longer log max removal limits without knowing current procedure volumes
   // DataLog( log_level_proc_info ) << " max removal " << ControlLimit() << " ml / " << SafetyLimit() << " ml" << endmsg;
   DataLog (log_level_proc_info) << endmsg;

}



// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// copy constructor and op=
Hypovolemia::Hypovolemia(const Hypovolemia& orig)
   : _Initialized(orig._Initialized),
     _RemovalSetting(orig._RemovalSetting),
     _WeightUnits(orig._WeightUnits),
     _MaxPercentRemoved(orig._MaxPercentRemoved),
     _TBV(orig._TBV),
     _TBVNadler(orig._TBVNadler),
     _DonorWeight(orig._DonorWeight),
     _RemoveVolumeWeight(orig._RemoveVolumeWeight),
     _RemoveVolumeGreaterThan(orig._RemoveVolumeGreaterThan),
     _RemoveVolumeLessThan(orig._RemoveVolumeLessThan),
     _RemoveVolumePlatelet(orig._RemoveVolumePlatelet),
     _RemoveVolumePlasma(orig._RemoveVolumePlasma),
     _MlPerKg(orig._MlPerKg),
     _Cassette(orig._Cassette),
     _Config_CDS(ROLE_RO),   // no need to copy
     _ProcDonor_CDS(ROLE_RO) // no need to copy
{}



Hypovolemia& Hypovolemia::operator = (const Hypovolemia& orig)
{
   if (this != &orig)
   {
      _Initialized             = orig._Initialized;
      _RemovalSetting          = orig._RemovalSetting;
      _WeightUnits             = orig._WeightUnits;
      _MaxPercentRemoved       = orig._MaxPercentRemoved;
      _TBV                     = orig._TBV;
      _TBVNadler               = orig._TBVNadler;
      _DonorWeight             = orig._DonorWeight;
      _RemoveVolumeWeight      = orig._RemoveVolumeWeight;
      _RemoveVolumeGreaterThan = orig._RemoveVolumeGreaterThan;
      _RemoveVolumeLessThan    = orig._RemoveVolumeLessThan;
      _RemoveVolumePlatelet    = orig._RemoveVolumePlatelet;
      _RemoveVolumePlasma      = orig._RemoveVolumePlasma;
      _MlPerKg                 = orig._MlPerKg;
      _Cassette                = orig._Cassette;
      // _Config_CDS(ROLE_RO),  // no need to copy
      // _ProcDonor_CDS(ROLE_RO)  // no need to copy
   }

   return *this;
}

/* FORMAT HASH 6b06196dcaade2327fcec481f4c549de */

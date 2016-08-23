// Static Model

//
// Static Model

//
#include "pqcontrol.h"
#include "procdata.h"

PQControl::PQControl()
{
   Initialize();    // Set up the valid reasons for each flag.
}

PQControl::~PQControl()
{}

int PQControl::GetReasons (const ProductQualityData& data, ENUMERATED_FLAG theFlag) const
{
   if ( initialized )
   {
      switch ( theFlag )
      {
         ///////////////////////////////////////////////
         // Platelet Flags
         case VERIFY_PLATELET_WBCS :
         {
            return( data.verify_platelet_wbc_reasons.Get() );
         }
         case VERIFY_PLATELET_YIELD :
         {
            return( data.verify_platelet_yield_volume_reasons.Get() );
         }
         case CONC_OUT_OF_RANGE :
         {
            return( data.platelet_concentration_reasons.Get() );
         }

         ///////////////////////////////////////////////
         // Plasma Flags
         case VERIFY_PLASMA_VOL :
         {
            return( data.verify_plasma_volume_reasons.Get() );
         }
         case VERIFY_PLASMA_WBCS :
         {
            return( data.verify_plasma_wbc_reasons.Get() );
         }

         ///////////////////////////////////////////////
         // RBC Flags
         case VERIFY_RBC_VOL :
         {
            return( data.verify_rbc_volume_reasons.Get() );
         }
         case VERIFY_RBC_VOL_PROD2 :
         {
            return( data.verify_rbc_volume_reasons_prod2.Get() );
         }
         case VERIFY_RBC_WBCS :
         {
            return( data.verify_rbc_wbc_reasons.Get() );
         }
         case VERIFY_RBC_WBCS_PROD2 :
         {
            return( data.verify_rbc_wbc_reasons_prod2.Get() );
         }
         case VERIFY_RBC_QUALITY :
         {
            return( data.verify_rbc_quality_reasons.Get() );
         }
         case VERIFY_RBC_QUALITY_PROD2 :
         {
            return( data.verify_rbc_quality_reasons_prod2.Get() );
         }

         default :
         {
            return( 0 );
         }
      }
   }
   else
   {
      return( -1 );
   }
}


// Encapsulates the basic function of getting the reasons that are currently set  for a particular flag.  The method's function value is the count of reasons for the flag to be set.  If it is 0, then the flag is not set.  If negative, then the object has not been initialized.
int PQControl::GetReasons (ENUMERATED_FLAG theFlag) const
{
   ProcData           pd;

   ProductQualityData PQLocal = pd.PQI().PQdata;

   return GetReasons(PQLocal, theFlag);

}

void PQControl::Initialize ()
{
   if ( !initialized )
   {
      // Initialize the "Valid Reasons for Flagging" Objects
      valid.verify_platelet_wbc_reasons.Initialize(SPILL_DETECTION, SPILL_RECOVERY, STOPPED_CENTRIFUGE, RBC_DETECTOR_ERROR,
                                                   HIGH_PLATELET_CONCENTRATION, LOW_CHAMBER_FLOW, HIGH_DELTA_HCT, AIR_BLOCK_RECOVERY,
                                                   HIGH_HCT, LOW_DETECTED_PLATELET_CONCENTRATION, LONG_DRAW,
                                                   SHORT_DRAW, PLATELET_CONTAMINANT_DETECTION, RINSEBACK_NOT_COMPLETED, CHANNEL_CLAMP_ERROR,
                                                   TRIPLE_PPC_AS_NON_LEUKOREDUCED);

      valid.verify_platelet_yield_volume_reasons.Initialize(SPILL_DETECTION, SPILL_RECOVERY, STOPPED_CENTRIFUGE,
                                                            LOW_CHAMBER_FLOW, AIR_BLOCK_RECOVERY, RUN_ENDED_EARLY,
                                                            LOW_DETECTED_PLATELET_CONCENTRATION, HIGH_PLATELET_YIELD_DEVIATION, LONG_DRAW,
                                                            SHORT_DRAW, PLATELET_VOLUME_ERROR, METERED_SOLUTION_VOLUME_ERROR, METERED_SOLUTION_ENDED_BY_OPERATOR,
                                                            FINAL_CONC_NOT_ACHIEVED, METERED_SOLUTION_PRIME_ERROR, RINSEBACK_NOT_COMPLETED);

      valid.platelet_concentration_reasons.Initialize(PLATELET_CONCENTRATION_BELOW_STORAGE_RANGE, PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE, FINAL_CONC_NOT_ACHIEVED);

      valid.verify_plasma_wbc_reasons.Initialize(SPILL_DETECTION, SPILL_RECOVERY, STOPPED_CENTRIFUGE, RBC_DETECTOR_ERROR,
                                                 AIR_BLOCK_RECOVERY, LOW_DETECTED_PLATELET_CONCENTRATION);

      valid.verify_plasma_volume_reasons.Initialize(STOPPED_CENTRIFUGE, AIR_BLOCK_RECOVERY, SHORT_DRAW, LONG_DRAW,
                                                    PLASMA_VOLUME_ERROR, RUN_ENDED_EARLY);

      valid.verify_rbc_volume_reasons.Initialize(STOPPED_CENTRIFUGE, AIR_BLOCK_RECOVERY, LONG_DRAW, RBC_VOLUME_ERROR,
                                                 RUN_ENDED_EARLY, CPS_HIGH, APS_HIGH, METERED_SOLUTION_VOLUME_ERROR, METERED_SOLUTION_PRIME_ERROR,
                                                 METERED_SOLUTION_ENDED_BY_OPERATOR);

      valid.verify_rbc_wbc_reasons.Initialize(CPS_HIGH, APS_HIGH);

      valid.verify_rbc_quality_reasons.Initialize(CPS_HIGH, APS_HIGH);

      valid.verify_rbc_volume_reasons_prod2.Initialize(STOPPED_CENTRIFUGE, AIR_BLOCK_RECOVERY, LONG_DRAW, RBC_VOLUME_ERROR,
                                                       RUN_ENDED_EARLY, CPS_HIGH, APS_HIGH, METERED_SOLUTION_VOLUME_ERROR, METERED_SOLUTION_ENDED_BY_OPERATOR);

      valid.verify_rbc_wbc_reasons_prod2.Initialize(CPS_HIGH, APS_HIGH);

      valid.verify_rbc_quality_reasons_prod2.Initialize(CPS_HIGH, APS_HIGH);
   }

   // Flag this object as initialized.
   initialized = 1;
}

int PQControl::Initialized ()
{
   return initialized;
}



void PQControl::PutReasonRBCprod2 (ENUMERATED_REASON theReason)
{
   ProcData pd;

   pd.MakePQIWritable();
   ProductQualityData PQLocal = pd.PQI().PQdata;
   bool               SetIt   = false;

   if ( valid.verify_rbc_volume_reasons_prod2.isReason(theReason) )
   {
      PQLocal.verify_rbc_volume_reasons_prod2.Put(theReason);
      SetIt = true;
   }

   if ( valid.verify_rbc_wbc_reasons_prod2.isReason(theReason) )
   {
      PQLocal.verify_rbc_wbc_reasons_prod2.Put(theReason);
      SetIt = true;
   }

   if ( valid.verify_rbc_quality_reasons_prod2.isReason(theReason) )
   {
      PQLocal.verify_rbc_quality_reasons_prod2.Put(theReason);
      SetIt = true;
   }

   if ( SetIt )
   {
      pd.PQI().PQdata.Set(PQLocal);
   }
   else
   {
      DataLog(log_level_proc_pqi_error) << "Reason for RBC prod2" << theReason << " was not registered to be set." << endmsg;
   }

   pd.MakePQIReadable();

}





// Encapsulates the basic function of adding a reason to the flags for which it is valid.  If a flag is not already set, this operation sets it.
void PQControl::PutReason (ENUMERATED_REASON theReason)
{
   ProcData pd;

   pd.MakePQIWritable();
   ProductQualityData PQLocal = pd.PQI().PQdata;
   bool               SetIt   = false;

   if ( valid.verify_platelet_wbc_reasons.isReason(theReason) )
   {
      PQLocal.verify_platelet_wbc_reasons.Put(theReason);
      SetIt = true;
   }

   if ( valid.verify_platelet_yield_volume_reasons.isReason(theReason) )
   {
      PQLocal.verify_platelet_yield_volume_reasons.Put(theReason);
      SetIt = true;
   }

   if ( valid.platelet_concentration_reasons.isReason(theReason) )
   {
      PQLocal.platelet_concentration_reasons.Put(theReason);
      SetIt = true;
   }

   if ( valid.verify_plasma_wbc_reasons.isReason(theReason) )
   {
      PQLocal.verify_plasma_wbc_reasons.Put(theReason);
      SetIt = true;
   }

   if ( valid.verify_plasma_volume_reasons.isReason(theReason) )
   {
      PQLocal.verify_plasma_volume_reasons.Put(theReason);
      SetIt = true;
   }

   if ( valid.verify_rbc_volume_reasons.isReason(theReason) )
   {
      PQLocal.verify_rbc_volume_reasons.Put(theReason);
      SetIt = true;
   }

   if ( valid.verify_rbc_wbc_reasons.isReason(theReason) )
   {
      PQLocal.verify_rbc_wbc_reasons.Put(theReason);
      SetIt = true;
   }

   if ( valid.verify_rbc_quality_reasons.isReason(theReason) )
   {
      PQLocal.verify_rbc_quality_reasons.Put(theReason);
      SetIt = true;
   }


   if ( SetIt )
   {
      pd.PQI().PQdata.Set(PQLocal);
   }
   else
   {
      DataLog(log_level_proc_pqi_error) << "Reason " << theReason << " was not registered to be set." << endmsg;
   }

   pd.MakePQIReadable();

}


// Encapsulates the basic function of testing a reason for being set for any flag.
int PQControl::isReason (ENUMERATED_REASON theReason) const
{
   ProcData           pd;

   ProductQualityData PQLocal = pd.PQI().PQdata;

   if ( PQLocal.verify_platelet_wbc_reasons.isReason(theReason) )
   {
      return( 1 );
   }

   if ( PQLocal.verify_platelet_yield_volume_reasons.isReason(theReason) )
   {
      return( 1 );
   }

   if ( PQLocal.platelet_concentration_reasons.isReason(theReason) )
   {
      return( 1 );
   }

   if ( PQLocal.verify_plasma_wbc_reasons.isReason(theReason) )
   {
      return( 1 );
   }

   if ( PQLocal.verify_plasma_volume_reasons.isReason(theReason) )
   {
      return( 1 );
   }

   if ( PQLocal.verify_rbc_volume_reasons.isReason(theReason) )
   {
      return( 1 );
   }

   if ( PQLocal.verify_rbc_wbc_reasons.isReason(theReason) )
   {
      return( 1 );
   }

   if ( PQLocal.verify_rbc_quality_reasons.isReason(theReason) )
   {
      return( 1 );
   }

   if ( PQLocal.verify_rbc_volume_reasons_prod2.isReason(theReason) )
   {
      return( 1 );
   }

   if ( PQLocal.verify_rbc_wbc_reasons_prod2.isReason(theReason) )
   {
      return( 1 );
   }

   if ( PQLocal.verify_rbc_quality_reasons_prod2.isReason(theReason) )
   {
      return( 1 );
   }

   return( 0 );
}

// Encapsulates the basic function of testing a reason for being set for a particular flag.
int PQControl::isReason (ENUMERATED_REASON theReason,
                         ENUMERATED_FLAG theFlag) const
{

   if ( initialized )
   {
      ProcData           pd;

      ProductQualityData PQLocal = pd.PQI().PQdata;

      switch ( theFlag )
      {
         case VERIFY_PLATELET_WBCS :
         {
            return( PQLocal.verify_platelet_wbc_reasons.isReason(theReason) );
         }
         case VERIFY_PLATELET_YIELD :
         {
            return( PQLocal.verify_platelet_yield_volume_reasons.isReason(theReason) );
         }
         case CONC_OUT_OF_RANGE :
         {
            return( PQLocal.platelet_concentration_reasons.isReason(theReason) );
         }
         case VERIFY_PLASMA_VOL :
         {
            return( PQLocal.verify_plasma_volume_reasons.isReason(theReason) );
         }
         case VERIFY_PLASMA_WBCS :
         {
            return( PQLocal.verify_plasma_wbc_reasons.isReason(theReason) );
         }
         case VERIFY_RBC_VOL :
         {
            return( PQLocal.verify_rbc_volume_reasons.isReason(theReason) );
         }
         case VERIFY_RBC_WBCS :
         {
            return( PQLocal.verify_rbc_wbc_reasons.isReason(theReason) );
         }
         case VERIFY_RBC_QUALITY :
         {
            return (PQLocal.verify_rbc_quality_reasons.isReason(theReason) );
         }
         case VERIFY_RBC_VOL_PROD2 :
         {
            return( PQLocal.verify_rbc_volume_reasons_prod2.isReason(theReason) );
         }
         case VERIFY_RBC_WBCS_PROD2 :
         {
            return( PQLocal.verify_rbc_wbc_reasons_prod2.isReason(theReason) );
         }
         case VERIFY_RBC_QUALITY_PROD2 :
         {
            return (PQLocal.verify_rbc_quality_reasons.isReason(theReason) );
         }
         case NO_FLAG :
         {
            break;
         }
      }
   }

   return 0;
}

// Encapsulates the basic function of removing a reason from the flags for which it is set.
// The major side effect of this method is that removing the last (or only) reason for the verify flag also "clears" the flag, because the reason count is set to 0.
void PQControl::RemoveReason (ENUMERATED_REASON theReason)
{
   ProcData           pd;
   pd.MakePQIWritable();
   ProductQualityData PQLocal = pd.PQI().PQdata;

   PQLocal.verify_platelet_wbc_reasons.Remove(theReason);
   PQLocal.verify_platelet_yield_volume_reasons.Remove(theReason);
   PQLocal.platelet_concentration_reasons.Remove(theReason);
   PQLocal.verify_plasma_wbc_reasons.Remove(theReason);
   PQLocal.verify_plasma_volume_reasons.Remove(theReason);
   PQLocal.verify_rbc_volume_reasons.Remove(theReason);
   PQLocal.verify_rbc_wbc_reasons.Remove(theReason);
   PQLocal.verify_rbc_quality_reasons.Remove(theReason);
   PQLocal.verify_rbc_volume_reasons_prod2.Remove(theReason);
   PQLocal.verify_rbc_wbc_reasons_prod2.Remove(theReason);
   PQLocal.verify_rbc_quality_reasons_prod2.Remove(theReason);

   pd.PQI().PQdata.Set(PQLocal);
   pd.MakePQIReadable();
}

// Encapsulates the basic function of adding a reason to the designated flag, as long as it is valid to do so.  If a reason is not already set, this operation sets it.
void PQControl::AddReasonToFlag (ENUMERATED_REASON theReason, ENUMERATED_FLAG theFlag)
{

   if ( initialized )
   {
      ProcData           pd;
      pd.MakePQIWritable();
      ProductQualityData PQLocal = pd.PQI().PQdata;
      bool               SetIt   = false;

      switch ( theFlag )
      {
         case VERIFY_PLATELET_WBCS :
         {
            if ( valid.verify_platelet_wbc_reasons.isReason(theReason) )
            {
               PQLocal.verify_platelet_wbc_reasons.Put(theReason);
               SetIt = true;
            }
            break;
         }
         case VERIFY_PLATELET_YIELD :
         {
            if ( valid.verify_platelet_yield_volume_reasons.isReason(theReason) )
            {
               PQLocal.verify_platelet_yield_volume_reasons.Put(theReason);
               SetIt = true;
            }
            break;
         }
         case CONC_OUT_OF_RANGE :
         {
            if ( valid.platelet_concentration_reasons.isReason(theReason) )
            {
               PQLocal.platelet_concentration_reasons.Put(theReason);
               SetIt = true;
            }
            break;
         }
         case VERIFY_PLASMA_VOL :
         {
            if ( valid.verify_plasma_volume_reasons.isReason(theReason) )
            {
               PQLocal.verify_plasma_volume_reasons.Put(theReason);
               SetIt = true;
            }
            break;
         }
         case VERIFY_PLASMA_WBCS :
         {
            if ( valid.verify_plasma_wbc_reasons.isReason(theReason) )
            {
               PQLocal.verify_plasma_wbc_reasons.Put(theReason);
               SetIt = true;
            }
            break;
         }
         case VERIFY_RBC_VOL :
         {
            if ( valid.verify_rbc_volume_reasons.isReason(theReason) )
            {
               PQLocal.verify_rbc_volume_reasons.Put(theReason);
               SetIt = true;
            }
            break;
         }
         case VERIFY_RBC_WBCS :
         {
            if ( valid.verify_rbc_wbc_reasons.isReason(theReason) )
            {
               PQLocal.verify_rbc_wbc_reasons.Put(theReason);
               SetIt = true;
            }
            break;
         }
         case VERIFY_RBC_QUALITY :
         {
            if ( valid.verify_rbc_quality_reasons.isReason(theReason) )
            {
               PQLocal.verify_rbc_quality_reasons.Put(theReason);
               SetIt = true;
            }
            break;
         }
         case VERIFY_RBC_VOL_PROD2 :
         {
            if ( valid.verify_rbc_volume_reasons_prod2.isReason(theReason) )
            {
               PQLocal.verify_rbc_volume_reasons_prod2.Put(theReason);
               SetIt = true;
            }
            break;
         }
         case VERIFY_RBC_WBCS_PROD2 :
         {
            if ( valid.verify_rbc_wbc_reasons_prod2.isReason(theReason) )
            {
               PQLocal.verify_rbc_wbc_reasons_prod2.Put(theReason);
               SetIt = true;
            }
            break;
         }
         case VERIFY_RBC_QUALITY_PROD2 :
         {
            if ( valid.verify_rbc_quality_reasons_prod2.isReason(theReason) )
            {
               PQLocal.verify_rbc_quality_reasons_prod2.Put(theReason);
               SetIt = true;
            }
            break;
         }

         case NO_FLAG :
         default :
         {
            DataLog(log_level_proc_pqi_error) << "Flag NO_FLAG was added to reason " << theReason << endmsg;

            break;
         }
      }

      if ( SetIt )
      {
         pd.PQI().PQdata.Set(PQLocal);
      }
      else
      {
         DataLog(log_level_proc_pqi_error) << "Reason " << theReason << ", flag " << theFlag
                                           << " was not registered to be set." << endmsg;
      }

      pd.MakePQIReadable();
   }
   else
   {
      DataLog(log_level_proc_pqi_error) << "Reason " << theReason << ", flag " << theFlag
                                        << "was not set because PQControl class was not initialized." << endmsg;
   }
}

// Encapsulates the basic function of removing a reason from the designated flag.  If the reason is the only one for the flag, then the flag's _count is zeroed, indicating the verify flag is not set.
void PQControl::RemoveReasonFromFlag (ENUMERATED_REASON theReason,
                                      ENUMERATED_FLAG theFlag)
{

   if ( initialized )
   {
      ProcData           pd;
      pd.MakePQIWritable();
      ProductQualityData PQLocal = pd.PQI().PQdata;
      bool               SetIt   = true;

      switch ( theFlag )
      {
         case VERIFY_PLATELET_WBCS :
         {
            PQLocal.verify_platelet_wbc_reasons.Remove(theReason);
            break;
         }
         case VERIFY_PLATELET_YIELD :
         {
            PQLocal.verify_platelet_yield_volume_reasons.Remove(theReason);
            break;
         }
         case CONC_OUT_OF_RANGE :
         {
            PQLocal.platelet_concentration_reasons.Remove(theReason);
            break;
         }
         case VERIFY_PLASMA_VOL :
         {
            PQLocal.verify_plasma_volume_reasons.Remove(theReason);
            break;
         }
         case VERIFY_PLASMA_WBCS :
         {
            PQLocal.verify_plasma_wbc_reasons.Remove(theReason);
            break;
         }
         case VERIFY_RBC_VOL :
         {
            PQLocal.verify_rbc_volume_reasons.Remove(theReason);
            break;
         }
         case VERIFY_RBC_WBCS :
         {
            PQLocal.verify_rbc_wbc_reasons.Remove(theReason);
            break;
         }
         case VERIFY_RBC_QUALITY :
         {
            PQLocal.verify_rbc_quality_reasons.Remove(theReason);
            break;
         }
         case VERIFY_RBC_VOL_PROD2 :
         {
            PQLocal.verify_rbc_volume_reasons_prod2.Remove(theReason);
            break;
         }
         case VERIFY_RBC_WBCS_PROD2 :
         {
            PQLocal.verify_rbc_wbc_reasons_prod2.Remove(theReason);
            break;
         }
         case VERIFY_RBC_QUALITY_PROD2 :
         {
            PQLocal.verify_rbc_quality_reasons_prod2.Remove(theReason);
            break;
         }

         default :
         {
            SetIt = false;
            break;
         }
      }

      if ( SetIt )
      {
         pd.PQI().PQdata.Set(PQLocal);
      }
      pd.MakePQIReadable();
   }
}

/* FORMAT HASH 41376f5fee6aca4348db1d6a80f864bd */

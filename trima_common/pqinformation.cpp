#include <vxworks.h>

//
// Static Model

//
#include "pqinformation.h"
#include "trima_datalog.h"
#include "productqualitydata.h"


PQInformation::PQInformation()
   : _ProcPQICDS(ROLE_RO)
{}

PQInformation::~PQInformation()
{}

// Returns 0 if no reasons exist for any of the Verify Flags.
// Returns 1 if any reasons exist for one or more of the Flags.
bool PQInformation::AtLeastOneFlag () const
{
   return( orFlags(VERIFY_PLATELET_WBCS,
                   VERIFY_PLATELET_YIELD,
                   CONC_OUT_OF_RANGE,
                   VERIFY_PLASMA_VOL,
                   VERIFY_PLASMA_WBCS,
                   VERIFY_RBC_VOL,
                   VERIFY_RBC_WBCS,
                   VERIFY_RBC_QUALITY,
                   VERIFY_RBC_VOL_PROD2,
                   VERIFY_RBC_WBCS_PROD2,
                   VERIFY_RBC_QUALITY_PROD2,
                   NO_FLAG)
           );
}

// Returns 0 if no reasons exist for any of the given Flags.
// Returns 1 if any reasons exist for one or more of the Flags.
bool PQInformation::AtLeastOneFlag (ENUMERATED_FLAG Flag1,
                                    ENUMERATED_FLAG Flag2 /* = NO_FLAG*/,
                                    ENUMERATED_FLAG Flag3 /* = NO_FLAG*/,
                                    ENUMERATED_FLAG Flag4 /* = NO_FLAG*/,
                                    ENUMERATED_FLAG Flag5 /* = NO_FLAG*/,
                                    ENUMERATED_FLAG Flag6 /* = NO_FLAG*/) const
{
   return( orFlags(Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, NO_FLAG) );
}

// Returns 0 if no reasons exist for the given Verify Flag.
// Returns 1 if any reasons exist for the Flag.
bool PQInformation::AtLeastOneReason (ENUMERATED_FLAG theFlag) const
{
   return( NumberOfReasons(theFlag) > 0 );
}

// Returns 0 if none of the given reasons exist for the given Flag.
// Returns 1 if any of the given reasons exist for the Flag.
bool PQInformation::AtLeastOneReason (ENUMERATED_FLAG theFlag,
                                      ENUMERATED_REASON Reason1,
                                      ENUMERATED_REASON Reason2 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason3 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason4 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason5 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason6 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason7 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason8 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason9 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason10 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason11 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason12 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason13 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason14 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason15 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason16 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason17 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason18 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason19 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason20 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason21 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason22 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason23 /* = NULL_REASON*/,
                                      ENUMERATED_REASON Reason24 /* = NULL_REASON*/) const
{
   return( orReasons(theFlag,
                     Reason1,
                     Reason2,
                     Reason3,
                     Reason4,
                     Reason5,
                     Reason6,
                     Reason7,
                     Reason8,
                     Reason9,
                     Reason10,
                     Reason11,
                     Reason12,
                     Reason13,
                     Reason14,
                     Reason15,
                     Reason16,
                     Reason17,
                     Reason18,
                     Reason19,
                     Reason20,
                     Reason21,
                     Reason22,
                     Reason23,
                     LAST_REASON) );
}

// Returns 0 if no reasons exist for the given flag or the parameter value is not a valid
// ENUMERATED_FLAG.  Returns the number of reasons for the flag if reasons have
// been given.
int PQInformation::NumberOfReasons (ENUMERATED_FLAG theFlag) const
{
   const ProductQualityData& PQLocal(_ProcPQICDS.PQdata.Get());

   switch ( theFlag )
   {
      case VERIFY_PLATELET_WBCS :
      {
         return( PQLocal.verify_platelet_wbc_reasons.Get() );
      }
      case VERIFY_PLATELET_YIELD :
      {
         return( PQLocal.verify_platelet_yield_volume_reasons.Get() );
      }
      case CONC_OUT_OF_RANGE :
      {
         return( PQLocal.platelet_concentration_reasons.Get() );
      }
      case VERIFY_PLASMA_VOL :
      {
         return( PQLocal.verify_plasma_volume_reasons.Get() );
      }
      case VERIFY_PLASMA_WBCS :
      {
         return( PQLocal.verify_plasma_wbc_reasons.Get() );
      }
      case VERIFY_RBC_VOL :
      {
         return( PQLocal.verify_rbc_volume_reasons.Get() );
      }
      case VERIFY_RBC_WBCS :
      {
         return( PQLocal.verify_rbc_wbc_reasons.Get() );
      }
      case VERIFY_RBC_QUALITY :
      {
         return( PQLocal.verify_rbc_quality_reasons.Get() );
      }
      case VERIFY_RBC_VOL_PROD2 :
      {
         return( PQLocal.verify_rbc_volume_reasons_prod2.Get() );
      }
      case VERIFY_RBC_WBCS_PROD2 :
      {
         return( PQLocal.verify_rbc_wbc_reasons_prod2.Get() );
      }
      case VERIFY_RBC_QUALITY_PROD2 :
      {
         return( PQLocal.verify_rbc_quality_reasons_prod2.Get() );
      }
      default :
      {
         return( 0 );
      }
   }
}

// This is a reason query method.  It tests the given flag for the given reason
// enumeration value. If the reason exists, it returns true; otherwise false.
bool PQInformation::isReason (ENUMERATED_FLAG theFlag,
                              ENUMERATED_REASON theReason) const
{
   bool v = ( theReason != NULL_REASON && theReason != LAST_REASON );

   if (v)
   {
      const ProductQualityData& PQLocal(_ProcPQICDS.PQdata.Get());

      switch ( theFlag )
      {
         case VERIFY_PLATELET_WBCS :
         {
            v = ( PQLocal.verify_platelet_wbc_reasons.isReason(theReason) );
            break;
         }
         case VERIFY_PLATELET_YIELD :
         {
            v = ( PQLocal.verify_platelet_yield_volume_reasons.isReason(theReason) );
            break;
         }
         case CONC_OUT_OF_RANGE :
         {
            v = ( PQLocal.platelet_concentration_reasons.isReason(theReason) );
            break;
         }
         case VERIFY_PLASMA_VOL :
         {
            v = ( PQLocal.verify_plasma_volume_reasons.isReason(theReason) );
            break;
         }
         case VERIFY_PLASMA_WBCS :
         {
            v = ( PQLocal.verify_plasma_wbc_reasons.isReason(theReason) );
            break;
         }
         case VERIFY_RBC_VOL :
         {
            v = ( PQLocal.verify_rbc_volume_reasons.isReason(theReason) );
            break;
         }
         case VERIFY_RBC_WBCS :
         {
            v = ( PQLocal.verify_rbc_wbc_reasons.isReason(theReason) );
            break;
         }
         case VERIFY_RBC_QUALITY :
         {
            v = ( PQLocal.verify_rbc_quality_reasons.isReason(theReason) );
            break;
         }
         case VERIFY_RBC_VOL_PROD2 :
         {
            v = ( PQLocal.verify_rbc_volume_reasons_prod2.isReason(theReason) );
            break;
         }
         case VERIFY_RBC_WBCS_PROD2 :
         {
            v = ( PQLocal.verify_rbc_wbc_reasons_prod2.isReason(theReason) );
            break;
         }
         case VERIFY_RBC_QUALITY_PROD2 :
         {
            v = ( PQLocal.verify_rbc_quality_reasons_prod2.isReason(theReason) );
            break;
         }
         default :
         {
            v = false;
            break;
         }
      }
   }

   return v;
}

// This is the reason query method.  For each invocation it returns one reason
// enumeration value for the flag enumeration given to it.  The 'which' parameter
// is the ordinal of the reason to return.  If 'which' is greater than the number
// of reasons or <= 0, then NULL_REASON is returned.
ENUMERATED_REASON PQInformation::GetReason (ENUMERATED_FLAG theFlag,
                                            int which) const
{
   int r;
   // Find the which-th reason
   for (r = NULL_REASON; which > 0 && r < LAST_REASON; r++ )
   {
      if ( isReason(theFlag, (ENUMERATED_REASON)r) ) which--;
   }
   if ( which == 0 && r != LAST_REASON )
   {
      // Return the which-th reason, if we found it.
      return( (ENUMERATED_REASON)r );
   }
   else
   {
      // Otherwise, return NULL_REASON
      return( NULL_REASON );
   }
}

// Returns non-zero (true) if there are NO reasons (stemming from events during the run) that preclude labelling the Platelet collection as containing less than 10E6 WBC contaminants.  Returns 0 (false) if events DID occur that may have contaminated the product with WBC in excess of 10E6.
bool PQInformation::LabelPlatelets () const
{
   return _ProcPQICDS.PQdata.Get().label_platelets_leukoreduced;
}

// Returns non-zero (true) if there are NO reasons (stemming from events during the run) that preclude labelling the Plasma collection as containing less than 10E6 WBC contaminants.  Returns 0 (false) if events DID occur that may have contaminated the product with WBC in excess of 10E6.
bool PQInformation::LabelPlasma () const
{
   return _ProcPQICDS.PQdata.Get().label_plasma_leukoreduced;
}

// Returns non-zero (true) if there are NO reasons (stemming from events during the run) that preclude labelling the RBC collection as containing less than 10E6 WBC contaminants.  Returns 0 (false) if events DID occur that may have contaminated the product with WBC in excess of 10E6.
bool PQInformation::LabelRbcs () const
{
   return _ProcPQICDS.PQdata.Get().label_rbcs_leukoreduced;
}

// Returns non-zero (true) if there are NO reasons (stemming from events during the run) that preclude labelling the RBC collection as containing less than 10E6 WBC contaminants.  Returns 0 (false) if events DID occur that may have contaminated the product with WBC in excess of 10E6.
bool PQInformation::LabelRbcsProd2 () const
{
   return _ProcPQICDS.PQdata.Get().label_rbcs_leukoreduced_prod2;
}


// Returns the boolean inclusive OR of all the Flags whose enumerations are given in the parameter list.
bool PQInformation::orFlags (ENUMERATED_FLAG aFlag,
                             ... /*_Variable_*/) const
{
   va_list which;
   int     v = 0;

   // Scan the flags in the parameter list looking for a set one.
   va_start(which, aFlag);
   while ( aFlag != NO_FLAG && NumberOfReasons(aFlag) == 0 )
   {
      aFlag = (ENUMERATED_FLAG)va_arg(which, int);
   }
   v = (aFlag != NO_FLAG);
   va_end(which);

   return( v );
}

// For the given flag, returns the boolean inclusive OR of all the Reasons whose enumerations are given in the parameter list.
bool PQInformation::orReasons (ENUMERATED_FLAG theFlag,
                               ENUMERATED_REASON aReason,
                               ... /*_Variable_*/) const
{
   va_list which;
   int     v = 0;

   // Scan the Reasons in the parameter list looking for a set one for the given flag.
   va_start(which, aReason);
   while ( aReason != LAST_REASON && !isReason(theFlag, aReason) )
   {
      aReason = (ENUMERATED_REASON)va_arg(which, int);
   }
   v = (aReason != LAST_REASON);
   va_end(which);

   return( v );
}

// For the given flag, returns true if any of the detailed reason flags for the given composite reason are true.
bool PQInformation::ShouldShow (ENUMERATED_FLAG flag, ENUMERATED_REASON_TO_SHOW show) const
{
   for ( int j = 0; _reason_map[j].reason != NULL_REASON; j++ )
   {
      if ( (_reason_map[j].show == show) && isReason(flag, _reason_map[j].reason) ) return true;
   }
   return false;
}



//
// LogReasons
//
void PQInformation::LogReasons () const
{
   for (int i = VERIFY_PLATELET_WBCS; i < NO_FLAG; i++)
   {
      for (int j = REASON_TO_SHOW_FIRST_ITEM; j < REASON_TO_SHOW_END_OF_LIST; j++)
      {
         if (ShouldShow((ENUMERATED_FLAG)i, (ENUMERATED_REASON_TO_SHOW)j))
         {
            DataLog(log_level_proc_pqi_external) << "Show flag " << (ENUMERATED_FLAG)i << ", reason " << (ENUMERATED_REASON_TO_SHOW)j << "." << endmsg;
         }
      }
   }
}



//
// DataLog_Stream enum to string manipulator
//
DataLog_Stream& operator << (DataLog_Stream& os, ENUMERATED_FLAG flag)
{
   os << ENUMERATED_FLAG_NAME(flag);
   os << "(" << (int)flag << ")";
   return os;
}



//
// DataLog_Stream enum to string manipulator
//
DataLog_Stream& operator << (DataLog_Stream& os, ENUMERATED_REASON_TO_SHOW show)
{
   switch (show)
   {
      case SHOW_NOTHING :
         os << "NOTHING";
         break;

      case SHOW_CENTRIFUGE_STOPPED :
         os << "CENTRIFUGE_STOPPED";
         break;

      case SHOW_DETECTED_CONCENTRATION_TOO_LOW :
         os << "DETECTED_CONCENTRATION_TOO_LOW";
         break;

      case SHOW_DONOR_HEMATOCRIT_CHANGE_TOO_LARGE :
         os << "DONOR_HEMATOCRIT_CHANGE_TOO_LARGE";
         break;

      case SHOW_DONOR_HEMATOCRIT_TOO_HIGH :
         os << "DONOR_HEMATOCRIT_TOO_HIGH";
         break;

      case SHOW_TARGET_VOL_NOT_ACHIEVED :
         os << "TARGET_VOL_NOT_ACHIEVED";
         break;

      case SHOW_PLATELET_CONCENTRATION_TOO_HIGH :
         os << "PLATELET_CONCENTRATION_TOO_HIGH";
         break;

      case SHOW_POSSIBLE_AIR_BLOCK :
         os << "POSSIBLE_AIR_BLOCK";
         break;

      case SHOW_PREDICTED_CONCENTRATION_BELOW_STORAGE_RANGE :
         os << "PREDICTED_CONCENTRATION_BELOW_STORAGE_RANGE";
         break;

      case SHOW_PREDICTED_CONCENTRATION_ABOVE_STORAGE_RANGE :
         os << "PREDICTED_CONCENTRATION_ABOVE_STORAGE_RANGE";
         break;

      case SHOW_RBC_SPILLOVER :
         os << "RBC_SPILLOVER";
         break;

      case SHOW_RBC_DETECTOR_CALIBRATION_ERROR :
         os << "RBC_DETECTOR_CALIBRATION_ERROR";
         break;

      case SHOW_PLATELET_CONTAMINANT_DETECTION :
         os << "SHOW_PLATELET_CONTAMINANT_DETECTION";
         break;

      case SHOW_METERED_VOLUME_ERROR :
         os << "SHOW_METERED_VOLUME_ERROR";
         break;

      case SHOW_RINSEBACK_NOT_COMPLETED :
         os << "SHOW_RINSEBACK_NOT_COMPLETED";
         break;

      case SHOW_CPS_HIGH :
         os << "SHOW_CPS_HIGH";
         break;

      case SHOW_APS_HIGH :
         os << "SHOW_APS_HIGH";
         break;

      case SHOW_FINAL_CONC_NOT_ACHIEVED :
         os << "SHOW_FINAL_CONC_NOT_ACHIEVED";
         break;

      case SHOW_METERED_SOLUTION_PRIME_ERROR :
         os << "SHOW_METERED_SOLUTION_PRIME_ERROR";
         break;

      case SHOW_CHANNEL_CLAMP_ERROR :
         os << "SHOW_CHANNEL_CLAMP_ERROR";
         break;

      case SHOW_WBC_CONTENT_NOT_VALIDATED :
         os << "SHOW_WBC_CONTENT_NOT_VALIDATED";
         break;

      case REASON_TO_SHOW_END_OF_LIST :
         os << "END_OF_LIST";
         break;

      default :
         break;
   }

   os << "(" << (int)show << ")";

   return os;
}



//
// ReasonsShown
//
bool PQInformation::ReasonsShown (ENUMERATED_FLAG flag, string& codes) const
{
   codes.erase();

   if (ShouldShow(flag, SHOW_RBC_SPILLOVER))
   {
      codes += 'a';
   }

   if (ShouldShow(flag, SHOW_CENTRIFUGE_STOPPED))
   {
      codes += 'b';
   }

   if (ShouldShow(flag, SHOW_TARGET_VOL_NOT_ACHIEVED))
   {
      codes += 'c';
   }

   if (ShouldShow(flag, SHOW_RBC_DETECTOR_CALIBRATION_ERROR))
   {
      codes += 'd';
   }

   if (ShouldShow(flag, SHOW_PLATELET_CONCENTRATION_TOO_HIGH))
   {
      codes += 'e';
   }

   if (ShouldShow(flag, SHOW_DONOR_HEMATOCRIT_CHANGE_TOO_LARGE))
   {
      codes += 'f';
   }

   if (ShouldShow(flag, SHOW_DONOR_HEMATOCRIT_TOO_HIGH))
   {
      codes += 'g';
   }

   if (ShouldShow(flag, SHOW_POSSIBLE_AIR_BLOCK))
   {
      codes += 'h';
   }

   if (ShouldShow(flag, SHOW_PREDICTED_CONCENTRATION_BELOW_STORAGE_RANGE))
   {
      codes += 'j';
   }

   if (ShouldShow(flag, SHOW_PREDICTED_CONCENTRATION_ABOVE_STORAGE_RANGE))
   {
      codes += 'k';
   }

   if (ShouldShow(flag, SHOW_DETECTED_CONCENTRATION_TOO_LOW))
   {
      codes += 'l';
   }

   /* m is not used any more, because this reason was combined
      with reason 'c' to make SHOW_TARGET_VOL_NOT_ACHIEVED.
   if (ShouldShow(flag, SHOW_RUN_ENDED_EARLY))
   {
      codes += 'm';
   }
   */

   if (ShouldShow(flag, SHOW_PLATELET_CONTAMINANT_DETECTION))
   {
      codes += 'n';
   }

   if (ShouldShow(flag, SHOW_METERED_VOLUME_ERROR))
   {
      codes += 'p';
   }

   if (ShouldShow(flag, SHOW_RINSEBACK_NOT_COMPLETED))
   {
      codes += 'q';
   }

   if (ShouldShow(flag, SHOW_CPS_HIGH))
   {
      codes += 'r';
   }

   if (ShouldShow(flag, SHOW_APS_HIGH))
   {
      codes += 's';
   }

   if (ShouldShow(flag, SHOW_FINAL_CONC_NOT_ACHIEVED))
   {
      codes += 't';
   }

   if (ShouldShow(flag, SHOW_METERED_SOLUTION_PRIME_ERROR))
   {
      codes += 'u';
   }

   if (ShouldShow(flag, SHOW_CHANNEL_CLAMP_ERROR))
   {
      codes += 'v';
   }

   if (ShouldShow(flag, SHOW_WBC_CONTENT_NOT_VALIDATED))
   {
      codes += 'w';
   }

   return (codes.size() > 0);
}

/* FORMAT HASH 11ab232dc2ce215120f257e2eda9315e */

/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.48  2003/11/27 00:01:36Z  ms10234
 * 6611 - check for donor info before setting invalid Hct alarm.
 * Revision 1.47  2003/10/23 16:27:48Z  ms10234
 * 6528- changed donor checks to give proper alarms for invalid data
 * Revision 1.46  2003/09/09 22:45:19Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.45  2003/08/27 21:25:04Z  ms10234
 * 6154 - fix for loss of ECLimit after consecutive PFRs
 * Revision 1.44  2003/08/21 15:40:26Z  ms10234
 * 6245 - set safety's saved TBV in message during TBV lockout conditions.
 * Revision 1.43  2003/07/17 21:33:27Z  td07711
 * IT6217 - log donor vitals info received from proc
 * Revision 1.42  2003/07/01 22:34:38Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.41  2003/04/04 00:02:59Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.40  2003/01/06 21:07:14Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.39  2002/10/11 09:47:38  pn02526
 * Massive checkin of fixed modules for integration checking
 * Revision 1.38  2002/08/07 12:43:57  pn02526
 * Remove extraneous #includes of numerous system files, obsolete message system files, and an obsolete dispatcher file.
 * Remove "trace" external; use DataLog_Default instead.
 * Use the new buffered message base class definition to send SafetyToGuiDonorVitalsMsg, rather than deriving class aSafetyToGuiDonorVitalsMsg_sndr from it.
 * Add aProcToSafetyDonorVitalsMsg_rcvr :: _theProcToSafetyDonorVitalsMsg to use the new buffered message class definition, rather than deriving from the buffered message base class.
 * Revision 1.37  2002/07/23 16:52:25  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.36  2002/04/23 14:50:51  pn02526
 * evise logging to use DataLogLevel class.  Remove (comment out) QNX includes.
 * Revision 1.35  2001/07/06 15:27:59Z  ms10234
 * 5195 - Change to lock out changes to TBV after PFR.
 * Revision 1.34  2001/06/13 22:20:43  ms10234
 * 4922 - Remove redundant TBV verification after PFR
 * Revision 1.33  2000/06/08 21:35:10  bs04481
 * Simplify.  Also, change processing during TBV lockout to restore
 * safety's donor data to the message before sending it to GUI
 * Revision 1.32  2000/03/15 18:16:17  BS04481
 * Move s_donor methods from s_donor.hpp to s_donor.cpp.
 * Add default alarm for unknown problem in donor vital data
 * 3.3 non-essential code review issues.
 * Create SafetyToGuiDonorVital message where used instead
 * of in s_main
 * Revision 1.31  1999/08/31 18:22:07  BS04481
 * Remove unnecessary printfs
 * Revision 1.30  1999/07/27 21:26:49  BS04481
 * Use higher significance conversion equation to convert inches to
 * meters and lbs to kilograms while calculating TBV.  Allow an extra
 * 5ml of margin in maximum TBV to make up for further conversion
 * error.
 * Revision 1.29  1999/06/29 15:40:22  BS04481
 * Remove TBVRangeValid=DONOR_VITAL_NOT_VALID from
 * the clause the allows operator to re-enter data.  It was preventing
 * A2 alarms for error cases.
 * Revision 1.28  1999/06/25 16:58:59  BS04481
 * For donor vital errors, change all fatal errors to A2 and do not
 * send data on to GUI if Safety is issueing an A2
 * Revision 1.27  1999/06/23 19:36:07  BS04481
 * Change handling of corrupt/incomplete donor vital data as
 * described in IT 3492
 * Revision 1.26  1998/11/12 18:33:55  bs04481
 * Only do the TBV comparison if we are actually going to do a PFR
 * recovery.
 * Revision 1.25  1998/11/07 00:20:31  bs04481
 * If we are in TBVLockOut but it is during a PFR, stuff our TBV into
 * the sdonorvitals message so that it can be displayed.  If we don't
 * do this, it will display as 0 even though safety is using a good TBV.
 * Revision 1.24  1998/10/20 20:43:50  bs04481
 * Change power fail recovery to restore PF data and result all
 * active monitoring immediately after Safety determines the
 * procedure is recoverable.
 * Add all donor data to PFR data set instead of just TBV
 * Do an exact TBV comparison of donor vital data received during
 * PFR with previously restored PFR data even if TBVLockOut
 * is in place
 * Revision 1.23  1998/10/09 05:19:30  bs04481
 * Re-activate TBVLockOut which was inadvertently left out in
 * the recent safety re-architecture.
 * Revision 1.22  1998/07/02 13:53:34  bs04481
 * Remove references to obsolete elements of sDonorVitals structure
 * Revision 1.21  1997/06/20 16:36:31  SM02805
 * Modified to implement continuous hct range testing
 *  ( see PPQA Resolutions from 06/19/97 Safety Meeting
 * for details )
 * Revision 1.20  1996/11/28 00:08:02  SM02805
 * Revision 1.19  1996/11/25 19:43:51  SM02805
 * Found and fixed bug in weight conversion from lb units to kg units.
 * Revision 1.18  1996/10/25 21:25:16  SM02805
 * Modifications to TBV calculations to address mixed units and
 * independant calc of TBV by safety and control.
 * Increase TBV neighborhood to 25.0 ml from 10.0 ml to accomodate
 * differences in calcs between proc and safety by using
 * two different equations taken from Spectra manual for version
 * 4 pgs 1-47.
 * Revision 1.17  1996/10/24 23:09:46  SM02805
 * 1. Modified method for TBV calculation :
 * only use one routine for calculation of TBV using metric
 * units for height and weight.  The routine converts height in
 * inches to cm and weight in lbs to kg if required before calculating
 * TBV.
 * 2. The TBV_COMPARISON _ERROR code was modified to
 * generate the proper alarm if this condition ever occurs ( would
 * have generated a TBV range error if the condition had been detected
 * previous to this change. )
 * Revision 1.16  1996/09/23 12:12:18  SM02805
 * Fix donor vital bug weight and height entry detection based
 * on values > 0. NOT >= 0. so user may enter gender prior to
 * height or weight. Was failing on wegith too low on entry.
 * Revision 1.15  1996/09/22 18:08:06  SM02805
 * Respond to 09/19/96 code review action items.
 * Revision 1.14  1996/09/17 21:15:22  SM02805
 * Revision 1.13  1996/09/15 21:49:32  SM02805
 * Significantly reduce print instrumentation in source files.
 * Implement _LOG_ERROR processing for cases where
 * data is important for post-run safety analysis.
 * Revision 1.12  1996/09/15 21:49:32  SM02805
 * Pursuant to 09/06/96 review and xfunc resolution :
 * 1. Implement hypovolemia limit for all cases as
 * 15% of TBV - 68.0 ml to account for possible pump
 * tolerance errors during RBC collect ( fill volume
 * may be understated by as much as 17.0 for each
 * fill cycle during RBC coll assume four fill volumes
 * for rbc collect resulting in 68.0 ml worst-case
 * underreporting ).
 * Revision 1.11  1996/07/29 23:01:26  SM02805
 * Revision 1.10  1996/07/29 23:01:26  SM02805
 * Lint fixes per 07/23/96, 07/26/96 reviews.
 * Revision 1.9  1996/07/24 16:57:08  SM02805
 * Fixed compiler warnings about unreachable code : removed switch
 * constructs for TBV calcs went to if, else if , return value at bottom
 * off function only.
 * Revision 1.8  1996/07/24 13:56:53  SM02805
 * Mods to remove absolute paths for include files, includes source
 * code modifications and makefile ( safety.mak) modifications.
 *
 * safety.mak mods use CPPFLAGS , now getting warnings level 3.
 *
 * Sorce code mods to implement donor disconnect test, only partially tested
 * - works as expected when test is passed first time, ie access line
 * properly clamped on first test try...
 * Revision 1.7  1996/07/22 17:13:41  SM02805
 * Update documentation for code review.
 *
 * Corrections to pump monitoring : but for platelet pump due to two magnet rotor.
 *
 * stest.mak target changed to be  saf_exec.
 *
 * TITLE:      safdvit.cpp, FS Safety System Monitoring Task
 *
 * ABSTRACT:  Safety donor vitals implementation file.
 *
 * History
 * 1. Created 06/24/96
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <math.h>


#include "trima_datalog.h"

#include "s_donor.hpp"
#include "s_glob.hpp"
#include "s_pfr.hpp"


#define DONOR_VITAL_NOT_VALIDATED          0          // all local flag values are set to this upon entry -> not yet validated
#define DONOR_VITAL_NOMINAL                1          // donor vital of interest is nominal


#define TBV_NEIGHBORHOOD                  25.0f       // TBV neighborhood (ml) for comparison of proc and safety calculated values

// if cDonorSex is found to be != ( DONOR_SEX_UNKNOWN || DONOR_SEX_IS_MALE || DONOR_SEX_FEMALE )
// cDonorSex s/b set to DONOR_SEX_ERROR
#define DONOR_SEX_ERROR                  50      // Erroneous sex value detected

// if cDonorHeightUnits is found to be != ( HEIGHT_UNITS_UNKNOWN || HEIGHT_UNITS_IN || HEIGHT_UNITS_CM )
// cDonorHeightUnits s/b set to HEIGHT_UNITS_ERROR
#define HEIGHT_UNITS_ERROR               51         // erroneous height units detected

// if cDonorWeightUnits is found to be != ( WEIGHT_UNITS_UNKNOWN || WEIGHT_UNITS_IN || WEIGHT_UNITS_CM )
// cDonorWeightUnits s/b set to WEIGHT_UNITS_ERROR
#define WEIGHT_UNITS_ERROR               52         // erroneous weight units detected

// Value for fDonorWeight if weight range error detected.
#define DONOR_WEIGHT_ERROR               53     // donor weight range error

// Value for fDonorHeight if height range error detected.
#define DONOR_HEIGHT_ERROR               54     // donor height range error

// Value for fSafetyDonorTBV or fProcDonorTBV  if TBV range error detected.
// OR TBV comparison failed by safety.
#define DONOR_TBV_ERROR                   55     // donor TBV error



// Defines for TBV processing ERRORS.
// the starting value for DONOR_WEIGHT_LOW_ERROR is arbitrary.
//
#define DONOR_WEIGHT_LOW_ERROR             60          // donor weight passed from control/GUI < minimum allowable weight
#define DONOR_WEIGHT_HIGH_ERROR            61          // donor weight passed from control/GUI > maximum allowable weight
#define DONOR_HEIGHT_LOW_ERROR             62          // donor height passed from control/GUI < minimum allowable height
#define DONOR_HEIGHT_HIGH_ERROR            63          // donor height passed from control/GUI > maximum allowable height
#define DONOR_TBV_HIGH_ERROR_SAFETY        64          // safety calculated TBV > max allowable for donor sex
#define DONOR_TBV_LOW_ERROR_SAFETY         65          // safety calculated TBV < min allowable for donor sex
#define DONOR_TBV_COMPARISON_ERROR         66          // safety caluclated TBV disagrees with proc value

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;


/*******************************************************************************
 *
 *  aDonorD Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety Donor Data object constructor
//
// ERROR HANDLING:   none.

aDonor::aDonor()
   : _cDonorVitalStatus(DONOR_VITAL_NOT_VALIDATED),
     _cDonorConnectedState(DONOR_IS_CONNECTED),
     _bTBVLockOut(false),
     _fSafetyDonorTBV_liters(0.0f),
     _fECLimit_ml(85.0f),
     _fDonorHct_decimal(0.0f),
     _fDonorPltPrecount(0.0f),
     _fDonorPlasmaFraction(0.0f)
{
   memset(&_structSDonorVitals, 0, sizeof(SDonorVitals) );    // clear the Donor Vitals buffer

   // default the EC limit to 85.  this will cause us to monitor
   // for too much inlet volume during donor disconnect even
   // if we have no idea what donor is connected.

   // Save for PFR
   updateToPFR();
}

// SPECIFICATION:    Safety Donor Data method
//
// ERROR HANDLING:   none.

void aDonor::SetTBV_liters (float TBV_ml)
{

   _fSafetyDonorTBV_liters = TBV_ml / 1000.0f;

   DataLog(log_level_safe_exec_info) << "Set TBV : " << _fSafetyDonorTBV_liters << endmsg;

   // set the extracorporeal limit.  use 85 as a default if we
   // don't have a valid donor.
   if (TBV_ml > 1.0f) // fix for floating point error
      SetECLimit_ml(SAFETY_EC_LIMIT * TBV_ml);
   else
      SetECLimit_ml(85.0f);

   // Save for PFR
   updateToPFR();

}

// SPECIFICATION:    Safety Donor Data method
//                   Range tests HCT.
//
// ERROR HANDLING:   DONOR_HCT_RANGE_ERROR.

void aDonor::SetHCT_decimal (float HCT_pcnt)
{
   if ( ( HCT_pcnt > 0.01f )  // fix for floating point error
        ||( _cDonorVitalStatus == DONOR_VITAL_NOMINAL_COMPLETE && _cDonorConnectedState == DONOR_IS_CONNECTED ) )
   {
      // must range test
      if ( HCT_pcnt < MIN_ALLOWABLE_HCT || HCT_pcnt > MAX_ALLOWABLE_HCT )
      {
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DONOR_HCT_RANGE_ERROR);
         // NOMINAL, Non-Error Operation logging
         DataLog(log_level_safety_alarm_detail) << "Hct Range Error Hct : " << HCT_pcnt << endmsg;
      }
      else
      {
         _fDonorHct_decimal    = HCT_pcnt / 100.0f;

         _fDonorPlasmaFraction = 1.f - _fDonorHct_decimal;

         // NOMINAL, Non-Error Operation logging
         DataLog(log_level_safe_exec_info) << "Set Hct : " << _fDonorHct_decimal << ", Set Plasma Fraction : " << _fDonorPlasmaFraction << endmsg;
      }
   }
   else
   {
      // NOMINAL, Non-Error Operation logging
      DataLog(log_level_safe_exec_info) << "Hct NOT set Hct passed : " << HCT_pcnt
                                        << ", DonVital Status = " << (int)_cDonorVitalStatus
                                        << ", Don Conn = " << (int)_cDonorConnectedState << endmsg;
   }


   // Save for PFR
   updateToPFR();
}

// SPECIFICATION:    Safety Donor Data method
//                   Sets extra-corporeal limit
//
// ERROR HANDLING:   none.

void aDonor::SetECLimit_ml (float ECLimit_ml)
{
   _fECLimit_ml = ECLimit_ml;

   DataLog(log_level_safe_exec_info) << "Set EC Limit : " << _fECLimit_ml << endmsg;

   // Save for PFR
   updateToPFR();
}

// SPECIFICATION:    Safety Donor Data method
//                   No checks on platelet precount
//
// ERROR HANDLING:   none.

void aDonor::SetPltPrecount (float PltPre)
{
   _fDonorPltPrecount = PltPre;

   DataLog(log_level_safe_exec_info) << "Set Precount " << _fDonorPltPrecount << endmsg;

   // Save for PFR
   updateToPFR();
}

// SPECIFICATION:    Safety Donor Data method
//                   Disables hypovolemia monitoring after
//                   the donor disconenct test has passed
//
// ERROR HANDLING:   none.

void aDonor::SetDonorConnectedState (char cDonorConnectedState)
{
   _cDonorConnectedState = cDonorConnectedState;

   DataLog(log_level_safe_exec_info) << "Set Donor Connected State : " << (int)_cDonorConnectedState << endmsg;

   // Save for PFR
   updateToPFR();

}

void aDonor::SetDonorVitalStatus (char cDonorVitalStatus)
{
   _cDonorVitalStatus = cDonorVitalStatus;

   // Save for PFR
   updateToPFR();
}

void aDonor::SetDonorVitalData (SDonorVitals _donorVital)
{
   // don't keep anything that safety doesn't use
   _structSDonorVitals.cDonorVitalsStatus = _donorVital.cDonorVitalsStatus;
   _structSDonorVitals.cDonorSex          = _donorVital.cDonorSex;
   _structSDonorVitals.cDonorHeightUnits  = _donorVital.cDonorHeightUnits;
   _structSDonorVitals.cDonorWeightUnits  = _donorVital.cDonorWeightUnits;
   _structSDonorVitals.fDonorWeight       = _donorVital.fDonorWeight;
   _structSDonorVitals.fDonorHeight       = _donorVital.fDonorHeight;
   _structSDonorVitals.fProcDonorTBV      = _donorVital.fProcDonorTBV;
   _structSDonorVitals.fSafetyDonorTBV    = _donorVital.fSafetyDonorTBV;

   // Save for PFR
   updateToPFR();
}

void aDonor::GetDonorVitalData (SDonorVitals& donorVital)
{
   // don't return anything that safety doesn't use
   donorVital.cDonorVitalsStatus = _structSDonorVitals.cDonorVitalsStatus;
   donorVital.cDonorSex          = _structSDonorVitals.cDonorSex;
   donorVital.cDonorHeightUnits  = _structSDonorVitals.cDonorHeightUnits;
   donorVital.cDonorWeightUnits  = _structSDonorVitals.cDonorWeightUnits;
   donorVital.fDonorWeight       = _structSDonorVitals.fDonorWeight;
   donorVital.fDonorHeight       = _structSDonorVitals.fDonorHeight;
   donorVital.fProcDonorTBV      = _structSDonorVitals.fProcDonorTBV;
   donorVital.fSafetyDonorTBV    = _structSDonorVitals.fSafetyDonorTBV;

   DataLog(log_level_safe_exec_info) << "TBV Nominal Gender:" << (int)donorVital.cDonorSex
                                     << " Ht:" << donorVital.fDonorHeight
                                     << " Wt:" << donorVital.fDonorWeight
                                     << " Safety TBV:" << donorVital.fSafetyDonorTBV
                                     << " Control TBV:" << donorVital.fProcDonorTBV
                                     << " HCT:" << donorVital.fDonorHct
                                     << " Pre:" << donorVital.fDonorPltPrecount
                                     << " ht units=" << (int)_structSDonorVitals.cDonorHeightUnits
                                     << " wt units=" << (int)_structSDonorVitals.cDonorWeightUnits
                                     << " rbc units=" << (int)_structSDonorVitals.cDonorRbcCount
                                     << endmsg;

}

void aDonor::SetTBVLockOut (bool bTBVLockOut)
{
   _bTBVLockOut = bTBVLockOut;

   // Save for PFR
   updateToPFR();
}


// SPECIFICATION:    Safety Donor Data method
//                   Enables hypovolemia monitoring in
//                   the appropriate states
//
// ERROR HANDLING:   none.

void aDonor::initializeForState (long newState)
{
   switch (newState)
   {
      case DONOR_CONNECTED :
      case BLOOD_PRIME :
      case BLOOD_RUN :
      case BLOOD_RINSEBACK :
      case DONOR_DISCONNECT :
         // 09/09/96 move from donor connected message....
         SetDonorConnectedState(DONOR_IS_CONNECTED);
         SetTBVLockOut(true);
         break;
      case POST_RUN :
         SetTBVLockOut(false);
         break;
      default :
         break;
   }
}

// SPECIFICATION:    Safety Donor Data method
//
// ERROR HANDLING:   none.

void aDonor::updateToPFR ()
{
   SafetyPFRDataToSave.PFR_fSafetyDonorTBV_liters         = _fSafetyDonorTBV_liters;
   SafetyPFRDataToSave.PFR_fECLimit_ml                    = _fECLimit_ml;
   SafetyPFRDataToSave.PFR_fDonorHct_decimal              = _fDonorHct_decimal;
   SafetyPFRDataToSave.PFR_fDonorPltPrecount              = _fDonorPltPrecount;
   SafetyPFRDataToSave.PFR_fDonorPlasmaFraction           = _fDonorPlasmaFraction;
   SafetyPFRDataToSave.PFR_cDonorVitalStatus              = _cDonorVitalStatus;
   SafetyPFRDataToSave.PFR_cDonorConnectedState           = _cDonorConnectedState;
   SafetyPFRDataToSave.PFR_bTBVLockOut                    = _bTBVLockOut;

   SafetyPFRDataToSave.PFR_DonorVitals.cDonorVitalsStatus = _structSDonorVitals.cDonorVitalsStatus;
   SafetyPFRDataToSave.PFR_DonorVitals.cDonorSex          = _structSDonorVitals.cDonorSex;
   SafetyPFRDataToSave.PFR_DonorVitals.cDonorHeightUnits  = _structSDonorVitals.cDonorHeightUnits;
   SafetyPFRDataToSave.PFR_DonorVitals.cDonorWeightUnits  = _structSDonorVitals.cDonorWeightUnits;
   SafetyPFRDataToSave.PFR_DonorVitals.fDonorWeight       = _structSDonorVitals.fDonorWeight;
   SafetyPFRDataToSave.PFR_DonorVitals.fDonorHeight       = _structSDonorVitals.fDonorHeight;
   SafetyPFRDataToSave.PFR_DonorVitals.fDonorHct          = _structSDonorVitals.fDonorHct;
   SafetyPFRDataToSave.PFR_DonorVitals.fDonorPltPrecount  = _structSDonorVitals.fDonorPltPrecount;
   SafetyPFRDataToSave.PFR_DonorVitals.fProcDonorTBV      = _structSDonorVitals.fProcDonorTBV;
   SafetyPFRDataToSave.PFR_DonorVitals.fSafetyDonorTBV    = _structSDonorVitals.fSafetyDonorTBV;
}

// SPECIFICATION:    Safety Donor Data method
//
// ERROR HANDLING:   none.

void aDonor::restoreDonorDataFromPFR ()
{
   _fSafetyDonorTBV_liters                = SafetyPFRDataFromRestore.PFR_fSafetyDonorTBV_liters;
   _fECLimit_ml                           = SafetyPFRDataFromRestore.PFR_fECLimit_ml;
   _fDonorHct_decimal                     = SafetyPFRDataFromRestore.PFR_fDonorHct_decimal;
   _fDonorPltPrecount                     = SafetyPFRDataFromRestore.PFR_fDonorPltPrecount;
   _fDonorPlasmaFraction                  = SafetyPFRDataFromRestore.PFR_fDonorPlasmaFraction;
   _cDonorVitalStatus                     = SafetyPFRDataFromRestore.PFR_cDonorVitalStatus;
   _cDonorConnectedState                  = SafetyPFRDataFromRestore.PFR_cDonorConnectedState;
   _bTBVLockOut                           = SafetyPFRDataFromRestore.PFR_bTBVLockOut;

   _structSDonorVitals.cDonorVitalsStatus = SafetyPFRDataFromRestore.PFR_DonorVitals.cDonorVitalsStatus;
   _structSDonorVitals.cDonorSex          = SafetyPFRDataFromRestore.PFR_DonorVitals.cDonorSex;
   _structSDonorVitals.cDonorHeightUnits  = SafetyPFRDataFromRestore.PFR_DonorVitals.cDonorHeightUnits;
   _structSDonorVitals.cDonorWeightUnits  = SafetyPFRDataFromRestore.PFR_DonorVitals.cDonorWeightUnits;
   _structSDonorVitals.fDonorWeight       = SafetyPFRDataFromRestore.PFR_DonorVitals.fDonorWeight;
   _structSDonorVitals.fDonorHeight       = SafetyPFRDataFromRestore.PFR_DonorVitals.fDonorHeight;
   _structSDonorVitals.fDonorHct          = SafetyPFRDataFromRestore.PFR_DonorVitals.fDonorHct;
   _structSDonorVitals.fDonorPltPrecount  = SafetyPFRDataFromRestore.PFR_DonorVitals.fDonorPltPrecount;
   _structSDonorVitals.fProcDonorTBV      = SafetyPFRDataFromRestore.PFR_DonorVitals.fProcDonorTBV;
   _structSDonorVitals.fSafetyDonorTBV    = SafetyPFRDataFromRestore.PFR_DonorVitals.fSafetyDonorTBV;

   updateToPFR();

   DataLog(log_level_safe_exec_info) << "Restoring Donor TBV: " << _fSafetyDonorTBV_liters
                                     << " EClimit: " << _fECLimit_ml
                                     << " HCT: " << _fDonorHct_decimal
                                     << " Precount: " << _fDonorPltPrecount
                                     << endmsg;

}

// SPECIFICATION:    Safety Donor Data method
//                   TRUE=>donor vital data is received and OK
//
// ERROR HANDLING:   none.

bool aDonor::DonorVitalsOK ()
{
   bool result = false;

   if ( (GetDonorVitalStatus()) == DONOR_VITAL_NOMINAL_COMPLETE )
   {
      // range test hct
      float ftempHCT_decimal = 0.f;
      float ftempPltPrecount = 0.f;

      ftempHCT_decimal = GetHCT_decimal();

      if ( (ftempHCT_decimal >= (MIN_ALLOWABLE_HCT / 100.f) )
           &&(ftempHCT_decimal <= (MAX_ALLOWABLE_HCT / 100.f) ) )
         result = true;
      else
      {
         DataLog(log_level_safe_exec_error) << "Hct error at BLOOD_PRIME trans : hct = " << ftempHCT_decimal << endmsg;
      }
   }
   return(result);
}



aProcToSafetyDonorVitalsMsg_rcvr::aProcToSafetyDonorVitalsMsg_rcvr(aDonor* paDonor_init)
   : _theProcToSafetyDonorVitalsMsg(Callback<aProcToSafetyDonorVitalsMsg_rcvr>(this, &aProcToSafetyDonorVitalsMsg_rcvr::notify), MessageBase::SNDRCV_RECEIVE_ONLY),
     _safetyToGuiDvitalsMsg(MessageBase::SEND_GLOBAL),
     _cWeightRangeValid((char)DONOR_VITAL_NOT_VALIDATED),
     _cHeightRangeValid((char)DONOR_VITAL_NOT_VALIDATED),
     _cSexValid((char)DONOR_VITAL_NOT_VALIDATED),
     _cTBVRangeValid((char)DONOR_VITAL_NOT_VALIDATED),
     _cWeightUnitsValid((char)DONOR_VITAL_NOT_VALIDATED),
     _cHeightUnitsValid((char)DONOR_VITAL_NOT_VALIDATED),
     _fTempDonorTBV(0.0f),
     _paDonor(paDonor_init)
{
   trima_assert(paDonor_init);

   initDonorVitals(&_structSDonorVitals);
}



aProcToSafetyDonorVitalsMsg_rcvr::~aProcToSafetyDonorVitalsMsg_rcvr()
{
   _paDonor = NULL;
}



// SPECIFICATION:    calculate donor TBV in ml and return to caller.
//                   safety implementation converts all units to metric (
//
//
// Revision :
// 10/24/96 modify routine to use cm , kg for all calculations of TBV
// allowing for mixed units for height and weight units as well
// 10/25/96 Procedure converts all usint to "english" safety converts all units to
// metric therefore, safety and procedure use different ie independant methods for
// calulation of TBV.
// TBV equations from Spectra manual for s/w version 4, pg 1-47
// found an error in the maual ! eqns listed for TBV calc using metric units
// returns TBV in LITERS NOT MILLILITERS. Multiply result by 1000.0
// to get TBV in l to ml as required.
//
// ERROR HANDLING:   none.

float aProcToSafetyDonorVitalsMsg_rcvr::calcTBV_CM_KG (void)
{
   double fcalcTBV     = 0.0f;
   double fDonorHeight = (double)_structSDonorVitals.fDonorHeight;
   double fDonorWeight = (double)_structSDonorVitals.fDonorWeight;

   // check for height in inches, convert to meters
   if ( _structSDonorVitals.cDonorHeightUnits == (char)HEIGHT_UNITS_IN )
   {
      fDonorHeight = fDonorHeight / 39.370079;

   }

   // check for height in cm, convert to meters
   if ( _structSDonorVitals.cDonorHeightUnits == (char)HEIGHT_UNITS_CM )
   {
      fDonorHeight = fDonorHeight / 100.0;
   }
   // check for weight in lbs, convert to kg
   if ( _structSDonorVitals.cDonorWeightUnits == (char)WEIGHT_UNITS_LB )
   {
      fDonorWeight = fDonorWeight / 2.2046226;
   }

   if ( _structSDonorVitals.cDonorSex == DONOR_SEX_IS_FEMALE )
   {
      // calc TBV in ml for female
      fcalcTBV = 1000.0 * ( 0.1833
                            + 0.3561 * ( fDonorHeight * fDonorHeight * fDonorHeight )
                            + 0.03308 * fDonorWeight );
   }
   else if ( _structSDonorVitals.cDonorSex == DONOR_SEX_IS_MALE )
   {
      // calculate TBV in ml for male
      fcalcTBV = 1000.0 * ( 0.6041
                            + 0.3669 * ( fDonorHeight * fDonorHeight * fDonorHeight )
                            + 0.03219 * fDonorWeight );
   }

   return( (float)fcalcTBV );

} // END float aProcToSafetyDonorVitalsMsg_rcvr :: calcTBV_CM_KG( void )



// SPECIFICATION:    notify for donor vitals msgs from GUI/proc
//
//
// ERROR HANDLING:   none.

void aProcToSafetyDonorVitalsMsg_rcvr::notify ()
{
   _structSDonorVitals = _theProcToSafetyDonorVitalsMsg.getData();

   DataLog(log_level_safe_exec_info) << "Received donor vitals:"
                                     << " gender=" << (int)_structSDonorVitals.cDonorSex
                                     << " ht=" << _structSDonorVitals.fDonorHeight
                                     << " wt=" << _structSDonorVitals.fDonorWeight
                                     << " tbv_s=" << _structSDonorVitals.fSafetyDonorTBV
                                     << " tbv_c=" << _structSDonorVitals.fProcDonorTBV
                                     << " pct=" << _structSDonorVitals.fDonorPltPrecount
                                     << " hct=" << _structSDonorVitals.fDonorHct
                                     << " ht units=" << (int)_structSDonorVitals.cDonorHeightUnits
                                     << " wt units=" << (int)_structSDonorVitals.cDonorWeightUnits
                                     << " rbc units=" << (int)_structSDonorVitals.cDonorRbcCount
                                     << " cDonorDownloadStatus: " << _structSDonorVitals.cDonorDownloadStatus
                                     << endmsg;

   // check the donor vital data and calculate TBV.
   validate();

   // compare the TBV to Proc's in a normal run or to
   // the saved one in a PFR run
   compareTBV();

   // generate the appropriate alarm, in necessary
   checkAndAlarm();

   // allow for update of HCT and Plt Precount anytime.
   // may have new values for hct, plt pre-count
   _paDonor->SetHCT_decimal(_structSDonorVitals.fDonorHct);
   _paDonor->SetPltPrecount(_structSDonorVitals.fDonorPltPrecount);

   // send structure to gui/control
   // 24June99 don't send if it is in error as the invalid data may
   // won't be displayed anyway
   if (_structSDonorVitals.cDonorVitalsStatus != (char)DONOR_VITAL_ERROR)
   {
      DataLog(log_level_safe_exec_info) << "aProcToSafetyDonorVitalsMsg_rcvr :: notify() sending SafetyToGuiDonorVitalsMsg(_structSDonorVitals)" << endmsg;
      _safetyToGuiDvitalsMsg.send(_structSDonorVitals);
   }

} // END void aProcToSafetyDonorVitalsMsg_rcvr :: notify()



// SPECIFICATION:    validate the donor vital data and calculate TBV
//
//
// ERROR HANDLING:   none.

void aProcToSafetyDonorVitalsMsg_rcvr::validate ()
{
   // if we haven't passed Donor Connect or if this is a PFR run,
   // validate the donor vitals.
   if ( ( _paDonor->GetTBVLockOut() == FALSE )
        ||(SafetyPFRDataFromRestore.PFR_fSafetyDonorTBV_liters > 1.0f) ) // 1.0 to fix floating point error
   {
      _cWeightRangeValid = (char)DONOR_VITAL_NOT_VALIDATED;
      _cHeightRangeValid = (char)DONOR_VITAL_NOT_VALIDATED;
      _cSexValid         = (char)DONOR_VITAL_NOT_VALIDATED;
      _cTBVRangeValid    = (char)DONOR_VITAL_NOT_VALIDATED;
      _cWeightUnitsValid = (char)DONOR_VITAL_NOT_VALIDATED;
      _cHeightUnitsValid = (char)DONOR_VITAL_NOT_VALIDATED;


      // has weight been entered ? validate it.
      validateWeight();

      // has height been entered? validate it.
      validateHeight();

      // validate the donor gender and calculate the TBV
      validateGenderAndTBV();

      DataLog(log_level_safe_exec_info) << "Validate: weight units range: " << (int)_cWeightUnitsValid
                                        << " weight range: " << (int)_cWeightRangeValid
                                        << " height units range: " << (int)_cHeightUnitsValid
                                        << " height range: " << (int)_cHeightRangeValid
                                        << " sex: " << (int)_cSexValid
                                        << " TBV range: " << (int)_cTBVRangeValid << endmsg;
   } // endif TBVLockOut == FALSE
   else
   {
      DataLog(log_level_safe_exec_error) << "TBV Alterations NOT ALLOWED : Lock Out Active" << endmsg;
      // get the last donor data because that is what we are using.
      // we'll send this one on to gui
      _paDonor->GetDonorVitalData(_structSDonorVitals);
   }
}


// SPECIFICATION:    validate the weight values in donor vital data
//
//
// ERROR HANDLING:   none.

void aProcToSafetyDonorVitalsMsg_rcvr::validateWeight ()
{
   if ( _structSDonorVitals.fDonorWeight > 1.0f ) // fix for floating point error
   {
      // check weight units.
      switch ( _structSDonorVitals.cDonorWeightUnits )
      {
         case (char)WEIGHT_UNITS_LB :
            _cWeightUnitsValid = (char)DONOR_VITAL_NOMINAL;

            // range test donor weight
            if ( _structSDonorVitals.fDonorWeight < MIN_ALLOWABLE_WEIGHT_LB )
               _cWeightRangeValid =  (char)DONOR_WEIGHT_LOW_ERROR;
            else if ( _structSDonorVitals.fDonorWeight > MAX_ALLOWABLE_WEIGHT_LB )
               _cWeightRangeValid =  (char)DONOR_WEIGHT_HIGH_ERROR;
            else
               _cWeightRangeValid = (char)DONOR_VITAL_NOMINAL;
            break;

         case (char)WEIGHT_UNITS_KG :
            _cWeightUnitsValid = (char)DONOR_VITAL_NOMINAL;

            // range test donor weight
            if ( _structSDonorVitals.fDonorWeight < MIN_ALLOWABLE_WEIGHT_KG )
               _cWeightRangeValid =  (char)DONOR_WEIGHT_LOW_ERROR;
            else if ( _structSDonorVitals.fDonorWeight > MAX_ALLOWABLE_WEIGHT_KG )
               _cWeightRangeValid =  (char)DONOR_WEIGHT_HIGH_ERROR;
            else
               _cWeightRangeValid = (char)DONOR_VITAL_NOMINAL;
            break;

         default :
            // 24June99
            // change from fatal to A2 for corrupt weight units
            _cWeightUnitsValid = (char)WEIGHT_UNITS_ERROR;
            _cWeightRangeValid = (char)DONOR_WEIGHT_LOW_ERROR;
            break;
      } // end switch weight units
   }
   else if ( (_paDonor->GetDonorVitalStatus() == (char)DONOR_VITAL_NOMINAL_COMPLETE)
             &&(_structSDonorVitals.fDonorWeight < 1.0f) ) // fix for floating point error
   {
      // we once had a valid donor but seem to have lost it
      _cWeightRangeValid = (char)DONOR_VITAL_NOT_VALIDATED;
   }
   else if (_structSDonorVitals.fDonorWeight < 1.0f) // fix for floating point error
   {
      // 24June99
      // change from ignored to A2 for negative weight
      _cWeightRangeValid =  (char)DONOR_WEIGHT_LOW_ERROR;
   } // end donor weight range testing != 0

}

// SPECIFICATION:    validate the height values in donor vital data
//
//
// ERROR HANDLING:   none.

void aProcToSafetyDonorVitalsMsg_rcvr::validateHeight ()
{
   if ( _structSDonorVitals.fDonorHeight > 1.f ) // fix for floating point error
   {
      // check height units
      switch ( _structSDonorVitals.cDonorHeightUnits )
      {
         case (char)HEIGHT_UNITS_IN :
            _cHeightUnitsValid = (char)DONOR_VITAL_NOMINAL;

            // range test donor height
            if ( _structSDonorVitals.fDonorHeight < MIN_ALLOWABLE_HEIGHT_IN )
               _cHeightRangeValid = (char)DONOR_HEIGHT_LOW_ERROR;
            else if ( _structSDonorVitals.fDonorHeight > MAX_ALLOWABLE_HEIGHT_IN )
               _cHeightRangeValid = (char)DONOR_HEIGHT_HIGH_ERROR;
            else
               _cHeightRangeValid = (char)DONOR_VITAL_NOMINAL;
            break;

         case (char)HEIGHT_UNITS_CM :
            _cHeightUnitsValid = (char)DONOR_VITAL_NOMINAL;

            // range test donor height
            if ( _structSDonorVitals.fDonorHeight < MIN_ALLOWABLE_HEIGHT_CM )
               _cHeightRangeValid = (char)DONOR_HEIGHT_LOW_ERROR;
            else if ( _structSDonorVitals.fDonorHeight > MAX_ALLOWABLE_HEIGHT_CM )
               _cHeightRangeValid = (char)DONOR_HEIGHT_HIGH_ERROR;
            else
               _cHeightRangeValid = (char)DONOR_VITAL_NOMINAL;
            break;

         default :
            // 24June99
            // change from fatal to A2 for corrupt height units
            _cHeightUnitsValid = (char)HEIGHT_UNITS_ERROR;
            _cHeightRangeValid = (char)DONOR_HEIGHT_LOW_ERROR;
            break;

      } // end switch height range testing
   }
   else if ( (_paDonor->GetDonorVitalStatus() == (char)DONOR_VITAL_NOMINAL_COMPLETE)
             &&(_structSDonorVitals.fDonorHeight < 1.0f) ) // fix for floating point error
   {
      // we once had a valid donor but seem to have lost it
      _cHeightRangeValid = (char)DONOR_VITAL_NOT_VALIDATED;
   }
   else if (_structSDonorVitals.fDonorHeight < 1.0f) // fix for floating point error
   {
      // 24June99
      // change from ignored to A2 for corrupt height
      _cHeightRangeValid = (char)DONOR_HEIGHT_LOW_ERROR;

   } // end donor height != 0


}

// SPECIFICATION:    validate the gender values in donor vital data
//                   and calculate the TBV if we have sufficient data
//
//
// ERROR HANDLING:   none.

void aProcToSafetyDonorVitalsMsg_rcvr::validateGenderAndTBV ()
{
   _fTempDonorTBV = 0.f;


   switch ( _structSDonorVitals.cDonorSex )
   {
      case (char)DONOR_SEX_UNKNOWN :
         if (_paDonor->GetDonorVitalStatus() == (char)DONOR_VITAL_NOMINAL_COMPLETE)
            // we had a valid donor but have lost it
            _cSexValid = (char)DONOR_SEX_ERROR;
         // if we haven't had a donor yet, no additional processing
         // as requried by the around-the-horn design, send
         // the data back to control/GUI but set the TBV to
         // 0 in case this is a PFR and we have to do a compare.
         _fTempDonorTBV  = 0.0f;
         _cTBVRangeValid = (char)DONOR_VITAL_NOT_VALIDATED;
         break;

      case (char)DONOR_SEX_IS_MALE :
         _cSexValid = (char)DONOR_VITAL_NOMINAL;

         if ( (_cWeightRangeValid == (char)DONOR_VITAL_NOMINAL)
              &&(_cHeightRangeValid == (char)DONOR_VITAL_NOMINAL) )
         {
            _fTempDonorTBV = calcTBV_CM_KG();

            // range test TBV for male
            if ( _fTempDonorTBV < MIN_ALLOWABLE_TBV_MALE )
               _cTBVRangeValid = (char)DONOR_TBV_LOW_ERROR_SAFETY;
            // precision of the english to metric conversion makes
            // for a slight error at the upper end of the range, so
            // allow 5ml over the maximum TBV allowed.
            else if ( _fTempDonorTBV > (MAX_ALLOWABLE_TBV_MALE + 5.0f) )
               _cTBVRangeValid = (char)DONOR_TBV_HIGH_ERROR_SAFETY;
            else
               _cTBVRangeValid = (char)DONOR_VITAL_NOMINAL;
         }
         break;

      case (char)DONOR_SEX_IS_FEMALE :
         _cSexValid = (char)DONOR_VITAL_NOMINAL;

         if ( (_cWeightRangeValid == (char)DONOR_VITAL_NOMINAL)
              &&(_cHeightRangeValid == (char)DONOR_VITAL_NOMINAL) )
         {
            _fTempDonorTBV = calcTBV_CM_KG();

            // range test TBV for female
            if ( _fTempDonorTBV < MIN_ALLOWABLE_TBV_FEMALE )
               _cTBVRangeValid = (char)DONOR_TBV_LOW_ERROR_SAFETY;
            // precision of the english to metric conversion makes
            // for a slight error at the upper end of the range, so
            // allow 5ml over the maximum TBV allowed.
            else if ( _fTempDonorTBV > (MAX_ALLOWABLE_TBV_FEMALE + 5.0f) )
               _cTBVRangeValid = (char)DONOR_TBV_HIGH_ERROR_SAFETY;
            else
               _cTBVRangeValid = (char)DONOR_VITAL_NOMINAL;
         }
         break;

      default :
         // 24June99
         // change from fatal error to A2 for corrupt sex
         _cSexValid      = (char)DONOR_SEX_ERROR;
         _fTempDonorTBV  = 0.0f;
         _cTBVRangeValid = (char)DONOR_VITAL_NOT_VALIDATED;
         break;
   } // end switch donor sex

}


// SPECIFICATION:    compare TBV to Proc's value.  also, if this is a PFR,
//                   make sure it compare's exactly to the saved one
//
//
// ERROR HANDLING:   none.

void aProcToSafetyDonorVitalsMsg_rcvr::compareTBV ()
{
   // 01/30/96 Dave S. : For TBV neighborhoos" not a safety issue so use what you want... "
   //  using 25ml
   //
   // safety won't change the donor TBV after the run begins
   if ( _paDonor->GetTBVLockOut() == FALSE )
   {
      // did we have enough data to calculate a TBV above?
      if (_cTBVRangeValid == (char)DONOR_VITAL_NOMINAL)
      {
         // safety and procedure calculation of TBV need to match within 25ml
         if ( (fabs(_fTempDonorTBV - _structSDonorVitals.fProcDonorTBV)) < TBV_NEIGHBORHOOD )
         {
            _structSDonorVitals.fSafetyDonorTBV = _fTempDonorTBV;

            _paDonor->SetTBV_liters(_structSDonorVitals.fSafetyDonorTBV);

            // NOMINAL, Non-Error Operation logging
            DataLog(log_level_safe_exec_info) << "TBV Nominal : Gender:" << (int)_structSDonorVitals.cDonorSex
                                              << " Ht:" << _structSDonorVitals.fDonorHeight
                                              << " Wt:" << _structSDonorVitals.fDonorWeight
                                              << " Safety TBV:" << _structSDonorVitals.fSafetyDonorTBV
                                              << " Control TBV:" << _structSDonorVitals.fProcDonorTBV
                                              << endmsg;
         }
         else
         {
            _cTBVRangeValid = (char)DONOR_TBV_COMPARISON_ERROR;

            // Comparison ERROR!
            DataLog(log_level_safe_exec_error) << "TBV ERROR ! Gender:" << (int)_structSDonorVitals.cDonorSex
                                               << " Ht:" << _structSDonorVitals.fDonorHeight
                                               << " Wt:" << _structSDonorVitals.fDonorWeight
                                               << " Safety TBV:" << _fTempDonorTBV
                                               << " Control TBV:" << _structSDonorVitals.fProcDonorTBV
                                               << endmsg;
         }
      }
   }
   else
   {
      _structSDonorVitals.fSafetyDonorTBV = _paDonor->GetTBV_liters() * 1000.0f;

      // NOMINAL, Non-Error Operation logging
      DataLog(log_level_safe_exec_info) << "TBV Alterations NOT ALLOWED : Lock Out Active" << endmsg;

      // If PFR, compare the Saftey's restored TBV with that received from Proc
      if (SafetyPFRDataFromRestore.PFR_fSafetyDonorTBV_liters > 1.0f)
      {
         // Safety's restored TBV and procedure's calculation of TBV need to match within 25ml
         const float deltaTBV = fabs(_structSDonorVitals.fSafetyDonorTBV - _structSDonorVitals.fProcDonorTBV);
         if (deltaTBV < TBV_NEIGHBORHOOD)
         {
            // Nominal
            DataLog(log_level_safe_exec_info) << "PFR TBV Comparison Nominal :"
                                              << " SafetyTBV=" << _structSDonorVitals.fSafetyDonorTBV
                                              << " ProcTBV=" << _structSDonorVitals.fProcDonorTBV
                                              << endmsg;
         }
         else
         {
            // Comparison ERROR
            _cTBVRangeValid = (char)DONOR_TBV_COMPARISON_ERROR;
            DataLog(log_level_safe_exec_error) << "PFR TBV Comparison ERROR !"
                                               << " SafetyTBV=" << _structSDonorVitals.fSafetyDonorTBV
                                               << " ProcTBV=" << _structSDonorVitals.fProcDonorTBV
                                               << endmsg;
         }
      }
   }

}

// SPECIFICATION:    check to see if we need to alarm for any
//                   donor vital information
//
//
// ERROR HANDLING:   none.

void aProcToSafetyDonorVitalsMsg_rcvr::checkAndAlarm ()
{
   // check for alarm conditions
   if ( (_cSexValid         == (char)DONOR_VITAL_NOMINAL)
        &&(_cWeightRangeValid == (char)DONOR_VITAL_NOMINAL)
        &&(_cHeightRangeValid == (char)DONOR_VITAL_NOMINAL)
        &&(_cTBVRangeValid    == (char)DONOR_VITAL_NOMINAL)
        &&(_cWeightUnitsValid == (char)DONOR_VITAL_NOMINAL)
        &&(_cHeightUnitsValid == (char)DONOR_VITAL_NOMINAL) )
   {
      _structSDonorVitals.cDonorVitalsStatus = (char)DONOR_VITAL_NOMINAL_COMPLETE;
      _paDonor->SetDonorVitalStatus( (char)DONOR_VITAL_NOMINAL_COMPLETE);
      _paDonor->SetDonorVitalData(_structSDonorVitals);
   }
   else if ( (_cSexValid       == (char)DONOR_VITAL_NOT_VALIDATED)
             ||(_cWeightRangeValid == (char)DONOR_VITAL_NOT_VALIDATED)
             ||(_cHeightRangeValid == (char)DONOR_VITAL_NOT_VALIDATED)
             ||(_cWeightUnitsValid == (char)DONOR_VITAL_NOT_VALIDATED)
             ||(_cHeightUnitsValid == (char)DONOR_VITAL_NOT_VALIDATED) )
   {
      // check for conditions where the data is still invalid
      // and set TBV to 0
      _structSDonorVitals.fSafetyDonorTBV    = 0.0f;
      _paDonor->SetTBV_liters(_structSDonorVitals.fSafetyDonorTBV);
      _structSDonorVitals.cDonorVitalsStatus = (char)DONOR_VITAL_INCOMPLETE;
      _paDonor->SetDonorVitalStatus( (char)DONOR_VITAL_INCOMPLETE);
   }
   else  // find out what is in error and post alarm(s)
   {
      _structSDonorVitals.fSafetyDonorTBV    = 0.0f;
      _paDonor->SetTBV_liters(_structSDonorVitals.fSafetyDonorTBV);
      _structSDonorVitals.cDonorVitalsStatus = (char)DONOR_VITAL_ERROR;
      _paDonor->SetDonorVitalStatus( (char)DONOR_VITAL_ERROR);

      if ( _cSexValid == (char)DONOR_SEX_ERROR )
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DONOR_SEX_INVALID);
      else if ( _cWeightUnitsValid == (char)WEIGHT_UNITS_ERROR || _cHeightUnitsValid == (char)HEIGHT_UNITS_ERROR )
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, TBV_UNITS_UNKNOWN);
      else if ( _cWeightRangeValid == (char)DONOR_WEIGHT_LOW_ERROR )
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DONOR_WEIGHT_TOO_LOW);
      else if ( _cWeightRangeValid == (char)DONOR_WEIGHT_HIGH_ERROR )
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DONOR_WEIGHT_TOO_HIGH);
      else if ( _cHeightRangeValid == (char)DONOR_HEIGHT_LOW_ERROR )
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DONOR_HEIGHT_TOO_LOW);
      else if ( _cHeightRangeValid == (char)DONOR_HEIGHT_HIGH_ERROR )
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DONOR_HEIGHT_TOO_HIGH);
      else if ( _cTBVRangeValid == (char)DONOR_TBV_LOW_ERROR_SAFETY )
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DONOR_TBV_TOO_LOW);
      else if ( _cTBVRangeValid == (char)DONOR_TBV_HIGH_ERROR_SAFETY )
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DONOR_TBV_TOO_HIGH);
      else if ( _cTBVRangeValid == (char)DONOR_TBV_COMPARISON_ERROR )
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, TBV_COMPARISON_FAILED);
      else
         // something is wrong but we seem to have lost track of what
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);
   } // end if / else for donor TBV errors
}


// SPECIFICATION:    initializer for donor vitals structure
//
//
// ERROR HANDLING:   none.

void aProcToSafetyDonorVitalsMsg_rcvr::initDonorVitals (SDonorVitals* pstructDonorVitals)
{
   memset(pstructDonorVitals, 0, sizeof(SDonorVitals) );   // clear the Donor Vitals buffer
   pstructDonorVitals->cDonorVitalsStatus   = DONOR_VITAL_INCOMPLETE;
   pstructDonorVitals->cDonorDownloadStatus = DONOR_DOWNLOAD_DEFAULT;
   pstructDonorVitals->cDonorInfoStatus     = DONOR_INFO_DEFAULT;
   pstructDonorVitals->cDonorSex            = DONOR_SEX_UNKNOWN;
   pstructDonorVitals->cDonorHeightUnits    = HEIGHT_UNITS_UNKNOWN;
   pstructDonorVitals->cDonorWeightUnits    = WEIGHT_UNITS_UNKNOWN;
   pstructDonorVitals->cDonorRbcCount       = RBC_COUNT_UNKNOWN;
   pstructDonorVitals->fDonorWeight         = 0.f;
   pstructDonorVitals->fDonorHeight         = 0.f;
   pstructDonorVitals->fDonorHct            = 0.f;
   pstructDonorVitals->fDonorPltPrecount    = 0.f;
   pstructDonorVitals->fProcDonorTBV        = 0.f;
   pstructDonorVitals->fSafetyDonorTBV      = 0.f;
}

/* FORMAT HASH a18654f33ac00abf63c99eb9f660ab95 */

/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cassetteid.cpp
 *
 */

#include "caldat.h"
#include "cassette.h"
#include "cassetteid.h"
#include "rbcdat.h"
#include "procdata.h"
#include "CatalogChecker.h"

DEFINE_STATE(CassetteID);

// Local constants
const unsigned char MIN_DRIVE_VALUE = 0;     // Lowest possible drive value

CassetteID::CassetteID()
   : StateAbs(),
     _DetectionWaitTimer(0, Callback<CassetteID>(this, &CassetteID::initRBCDetector), TimerMessage::DISARMED),
     _IdentifySetMsg(Callback<CassetteID>(this, &CassetteID::operatorSelectedCassette) ),
     _SetIdentificationAlarm(SET_IDENTIFICATION_ALARM),
     _CassetteType(0),
     _RedDriveValue(0),
     _GreenDriveValue(0),
     _RetryAttempted(0),
     _DisposableSelected(0),
     _LoadError(CASSETTE_OK),
     _CatalogNumberIsWhite(false),
     _CatalogNumberIsBlack(false),
     _CatalogNumberIsPLTMSS(false),
     _CatalogNumberIsRBCMSS(false),
     _CatalogNumberDoesNotSupportSaline(false)
{}

CassetteID::CassetteID(const CassetteID& state)
   : StateAbs(state),
     _DetectionWaitTimer(0, Callback<CassetteID>(this, &CassetteID::initRBCDetector), TimerMessage::DISARMED),
     _IdentifySetMsg(Callback<CassetteID>(this, &CassetteID::operatorSelectedCassette) ),
     _SetIdentificationAlarm(SET_IDENTIFICATION_ALARM),
     _CassetteType(state._CassetteType),
     _RedDriveValue(state._RedDriveValue),
     _GreenDriveValue(state._GreenDriveValue),
     _RetryAttempted(state._RetryAttempted),
     _DisposableSelected(state._DisposableSelected),
     _LoadError(state._LoadError),
     _CatalogNumberIsWhite(false),
     _CatalogNumberIsBlack(false),
     _CatalogNumberIsPLTMSS(false),
     _CatalogNumberIsRBCMSS(false),
     _CatalogNumberDoesNotSupportSaline(false)
{
   CassetteID::copyOver(state);
}

CassetteID::~CassetteID()
{
   CassetteID::cleanup();
}

int CassetteID::transitionStatus ()
{
   ProcData pd;

   if ( _DisposableSelected )
   {
      //
      // cassette::cassetteType { NOT_LOADED=0, UNKNOWN, PLTPLSRBC, RBCPLS, END_OF_LIST };
      //
      pd.MakeTrimaSetWritable();
      DataLog(log_level_proc_load_cassette_info) << "Cassette identified as -> " << _CassetteType << endmsg;
      pd.Orders().CassetteId( (cassette::cassetteType)_CassetteType);
      pd.TrimaSet().Cassette.Set((cassette::cassetteType)_CassetteType);

      DataLog(log_level_proc_load_cassette_info) << "Disposable Selected " << pd.Orders().CassetteId() << endmsg;
      return 1;
   }

   if ( _LoadError == CASSETTE_OK && !pd.Status().CassIsDown() )
   {
      DataLog(log_level_proc_load_cassette_error) << "Cassette is not down" << endmsg;
      _LoadError = OPERATOR_REQUESTED_UNLOAD;
   }

   if ( _LoadError != CASSETTE_OK )
   {
      DataLog(log_level_proc_load_cassette_error) << "Cassette load error " << _LoadError << endmsg;
      //
      //
      //  Since an error was set to get into this code, one of the
      //  following alarms must be active.  For cassette stamp alarms
      //  (not recognized or invalid for procedure) we transition back
      //  to the ss1-6 state.
      //
      if (_LoadError == CASSETTE_UNRECOGNIZED)
      {
         PendingCassetteAlarmMsg pendingAlarmMsg(MessageBase::SEND_LOCAL);
         pendingAlarmMsg.send((long)CASSETTE_UNRECOGNIZED_ALARM);
         DataLog(log_level_proc_load_cassette_error) << "Set cassette unrecognized alarm " << CASSETTE_UNRECOGNIZED_ALARM << endmsg;
      }
      else if (_LoadError == CASSETTE_INVALID)
      {
         PendingCassetteAlarmMsg pendingAlarmMsg(MessageBase::SEND_LOCAL);
         pendingAlarmMsg.send((long)CASSETTE_INVALID_ALARM);
         DataLog(log_level_proc_load_cassette_error) << "Set cassette invalid alarm " << CASSETTE_INVALID_ALARM << endmsg;
      }
      return 2;
   }

   return NO_TRANSITION;
}

void CassetteID::operatorSelectedCassette ()
{
   _CassetteType = (cassette::cassetteType)_IdentifySetMsg.getData();

   DataLog(log_level_proc_load_cassette_info) << "Operator selected cassette type: " << _CassetteType << endmsg;

   if ( _CassetteType == cassette::NOT_LOADED)
   {
      _LoadError = OPERATOR_REQUESTED_UNLOAD;
   }
   else
   {
      checkCompatibility();
   }
}

int CassetteID::preProcess ()
{
   ProcData pd;

   //
   //  Reset the drive values.  In general this does nothing since the lower
   //  level software will examine the values and note that they are unchanged.
   //  However this is important in the case where the retry for set ID is
   //  attempted.
   //
   if (_RetryAttempted == 1)
   {
      pd.Orders().RBCRed(MIN_DRIVE_VALUE);
      pd.Orders().RBCGreen(MIN_DRIVE_VALUE);
      _RetryAttempted = 2;
   }
   else
   {
      pd.Orders().RBCRed(_RedDriveValue);
      pd.Orders().RBCGreen(_GreenDriveValue);
   }

   if (!_driveValuesLogged)
   {
      DataLog(log_level_proc_info) << "Initial drive values are: Red " << (int)_RedDriveValue
                                   << " Green " << (int)_GreenDriveValue << endmsg;
      _driveValuesLogged = true;
   }

   //
   // If any error has occurred while lowering the cassette or
   // initializing the RBC detector then reverse the command to UP
   // and wait for it to reach position.
   //
   if ( _LoadError != CASSETTE_OK )
   {
      pd.Orders().RBCRed(MIN_DRIVE_VALUE);
      pd.Orders().RBCGreen(MIN_DRIVE_VALUE);
   }

   return NORMAL;
}

int CassetteID::postProcess ()
{
   return NORMAL;
}

int CassetteID::preEnter ()
{
   ProcData pd;


   DataLog(log_level_proc_info) <<    "Cassette Function codes : "  << pd.Run().CassetteFunctionBits.Get() << endmsg;
   // the stamp bit is set to 1 if white or 0 if black
   if (CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::STAMP))
   {

      DataLog(log_level_proc_info) << "Catalog Number says WHITE" << endmsg;
      _CatalogNumberIsWhite = true;
      _CatalogNumberIsBlack = false;
   }
   else
   {

      DataLog(log_level_proc_info) << "Catalog Number says BLACK" << endmsg;
      _CatalogNumberIsWhite = false;
      _CatalogNumberIsBlack = true;
   }

   if (CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::MSS_PLT))
   {
      DataLog(log_level_proc_info) << "Catalog Number says MSS PLT enabled" << endmsg;
      _CatalogNumberIsPLTMSS = true;
   }

   if ( CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::MSS_RBC) )
   {
      DataLog(log_level_proc_info) << "Catalog Number says MSS RBC/PTF enabled" << endmsg;
      _CatalogNumberIsRBCMSS = true;
   }

   if ( CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::NO_SALINE_BOLUS) )
   {
      DataLog(log_level_proc_info) << "Catalog Number says saline is not supported" << endmsg;
      _CatalogNumberDoesNotSupportSaline = true;
   }

   //
   // Set up my timer ...
   //
   _DetectionWaitTimer.interval(5000);   // 5 seconds (5000 msecs)
   DataLog(log_level_proc_debug) << "Started the detection wait timer" << endmsg;

   //
   // Default the rbc calibration to allow calibration
   //
   //  RBCDetectorForcedUncalibrated( 0 );

   // Set up the starting drive values for the red an green leds.
   _RedDriveValue   = (unsigned char)CalDat::data().RBCInitRedDrive;
   _GreenDriveValue = (unsigned char)CalDat::data().RBCInitGreenDrive;

   // Drive the leds @ those values.
   pd.Orders().RBCRed(_RedDriveValue);
   pd.Orders().RBCGreen(_GreenDriveValue);

   _driveValuesLogged = false;
   //
   //
   //   Log RBC calibration data at this point (IT3118)
   //
   logCalData();

   return NORMAL;
}

int CassetteID::postExit ()
{


   // clear the cassette MSS flags
   _CatalogNumberIsWhite              = false;
   _CatalogNumberIsBlack              = false;
   _CatalogNumberIsPLTMSS             = false;
   _CatalogNumberIsRBCMSS             = false;
   _CatalogNumberDoesNotSupportSaline = false;

   if ( _DisposableSelected )
   {
      //
      // Create the set object ...
      //
      if ( _CassetteType == cassette::PLTPLSRBC )
      {
         DataLog(log_level_proc_load_cassette_info) << "Standard Trima set type loaded " << _CassetteType << endmsg;
      }
      else
      {
         DataLog(log_level_proc_load_cassette_info) << "RBC/Plasma Trima set type loaded " << _CassetteType << endmsg;
      }

      //
      // Ask for the RBC Detector to be initialized ...
      //
      RBCCalibrationRequestMsg initReq(MessageBase::SEND_LOCAL);
      initReq.send(1);
   }

   //
   // Stop my timer ...
   //
   _DetectionWaitTimer.interval(0);

   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void CassetteID::initRBCDetector ()
{
   ProcData pd;
   float    red   = (float)pd.Status().RedValue();
   float    green = (float)pd.Status().GreenValue();

   DataLog(log_level_proc_load_cassette_info) << "Cassette detection red=" << red << " green=" << green << endmsg;

   // PltPlsRBC (4-bag) Set
   if ( ( red > RbcDat::data().RedLowPltPlsRBCStamp ) &&
        ( red < RbcDat::data().RedHighPltPlsRBCStamp ) )
   {
      _CassetteType = cassette::PLTPLSRBC;

      DataLog(log_level_proc_load_cassette_info) << "PltPlsRBC stamp detected." << endmsg;

      checkCompatibility();
   }
   // RBCPls (DRBC) Set
   else if ( ( red > RbcDat::data().RedLowRBCPlsStamp ) &&
             ( red < RbcDat::data().RedHighRBCPlsStamp ) )
   {
      _CassetteType = cassette::RBCPLS;

      DataLog(log_level_proc_load_cassette_info) << "RBCPls stamp detected." << endmsg;

      checkCompatibility();
   }
   else
   {
      //
      //   If we have already attempted a retry and we are still not
      //   recognized have the operator decide.  If we're outside the
      //   acceptable ranges then it's still an alarm.
      //
      if ( _RetryAttempted > 0)
      {
         if ( ( red < RbcDat::data().MinimumCassetteReflection ) ||
              ( red > RbcDat::data().MaximumCassetteReflection ) )
         {
            _LoadError    = CASSETTE_UNRECOGNIZED;

            _CassetteType = cassette::UNKNOWN;

            DataLog(log_level_proc_load_cassette_error) << "Bad stamp detected - unable to identify set" << endmsg;
         }
         else
         {
            //
            // Stop the checking, the operator will have to identify it ...
            //
            _DetectionWaitTimer.interval(0);
            _SetIdentificationAlarm.setAlarm();
            DataLog(log_level_proc_load_cassette_error) << "Set the set identification alarm " << SET_IDENTIFICATION_ALARM << endmsg;
         }

         //
         //  The cassette was not identified the first time.  Set the drive
         //  values to zero this frame; they will be reset next frame.
         //
      }
      else
      {
         _RetryAttempted = 1;

         _CassetteType   = cassette::NOT_LOADED; // force compatibility

         DataLog(log_level_proc_load_cassette_error) << "Bad stamp detected - retry attempted." << endmsg;
      }
   }
}


void CassetteID::checkCompatibility ()
{
   ProcData pd;
   int      incompatibleSpecial = 0;

   if ( (_CassetteType == cassette::PLTPLSRBC)
        && _CatalogNumberIsBlack)
   {

      pd.MakeRunWritable();
      pd.Run().CassetteFunctionBits.Set(0);    // reset the funtion bits because all bets are off now
      pd.MakeRunReadable();

      DataLog(log_level_proc_info) << "ERROR: Catalog Number vs RBC mismatch (black,white)" << endmsg;
      incompatibleSpecial = 1;
   }

   if ( (_CassetteType == cassette::RBCPLS)
        && _CatalogNumberIsWhite )
   {
      pd.MakeRunWritable();
      pd.Run().CassetteFunctionBits.Set(0);    // reset the funtion bits because all bets are off now
      pd.MakeRunReadable();

      DataLog(log_level_proc_info) << "ERROR: Catalog Number vs RBC mismatch (white,black)" << endmsg;
      incompatibleSpecial = 1;
   }

   // if a procedure has been selected we will check it vs the catalog numbers fuctions...
   if (pd.RunTargets().ProcNumber.Get() !=0)
   {
      if (pd.RunTargets().MeterPlatelets.Get() && !_CatalogNumberIsPLTMSS)
      {
         pd.MakeRunWritable();
         pd.Run().CassetteFunctionBits.Set(0);     // reset the funtion bits because all bets are off now
         pd.MakeRunReadable();

         DataLog(log_level_proc_info) << "ERROR: selected MSS PLT procedure and Catalog Number is not MSS enabled " << endmsg;
         incompatibleSpecial = 1;
      }
   }

   // if a procedure has been selected we will check it vs the catalog numbers fuctions...
   if (pd.RunTargets().ProcNumber.Get() !=0)
   {
      if (pd.RunTargets().MeterRbcs.Get() && !_CatalogNumberIsRBCMSS)
      {
         pd.MakeRunWritable();
         pd.Run().CassetteFunctionBits.Set(0);     // reset the funtion bits because all bets are off now
         pd.MakeRunReadable();

         DataLog(log_level_proc_info) << "ERROR: selected MSS RBC procedure and Catalog Number is not MSS enabled " << endmsg;
         incompatibleSpecial = 1;
      }
   }

   if (pd.RunTargets().ProcNumber.Get() !=0)
   {
      if (pd.RunTargets().RequiresRepFluid.Get() && _CatalogNumberDoesNotSupportSaline)
      {
         pd.MakeRunWritable();
         pd.Run().CassetteFunctionBits.Set(0);  // reset the funtion bits because all bets are off now
         pd.MakeRunReadable();

         DataLog(log_level_proc_info) << "ERROR: Procedure requires saline but Catalog Number does not support saline " << endmsg;
         incompatibleSpecial = 1;
      }
   }

   //
   //
   //  Special case: a DRBC procedure is only compatible
   //  with a DRBC set at load time (even though it may be
   //  compatible with a pltplsrbc set after loading).
   //

   if (_CassetteType == cassette::PLTPLSRBC)
   {
      if ((pd.RunTargets().PlateletYieldTarget.Get() == 0.0f) &&
          ((pd.RunTargets().PlasmaTarget.Get() > 0.0f) ||
           (pd.RunTargets().RbcDoseTarget.Get() > 0.0f)))
      {

         DataLog(log_level_proc_info) << "Special case: an "
                                      << cassette::RBCPLS << " procedure is incompatible with a(n) "
                                      << (cassette::cassetteType)_CassetteType << " set at load time.  Either load an "
                                      << cassette::RBCPLS << " cassette, or choose a procedure compatible with this one. " << endmsg;

         incompatibleSpecial = 1;
      }
   }

   //
   //
   //
   //  Check to see if the cassette is compatible with the
   //  current procedure.  If we ended up with UNKNOWN then
   //  the compatibility check will always be false.  Note
   //  that if a procedure has not yet been selected then
   //  the volumes below will be zero (and compatible with
   //  all valid cassette types).  We fudge the RBC volume by a skosh
   //  to avoid rounding effects.
   //
   cassette cs;

   // Fix for IT 7937: We're now providing for explicit logging information
   // for test and debugging
   const float pltTarg = pd.RunTargets().PlateletYieldTarget.Get(),
               plsTarg = pd.RunTargets().PlasmaTarget.Get(),
               rbcTarg = pd.RunTargets().RbcDoseTarget.Get() - 0.5f;

   int compatible      = cs.compatible( (cassette::cassetteType)_CassetteType, pltTarg, plsTarg, rbcTarg);

   if ( incompatibleSpecial || !compatible)
   {
      _LoadError = CASSETTE_INVALID;

      DataLog(log_level_proc_load_cassette_error) << "Cassette incompatible with selected procedure." << endmsg;

      if (!compatible)
      {
         cassette::cassetteType recommendType = cs.recommendedType(pltTarg, plsTarg, rbcTarg);

         DataLog(log_level_proc_load_cassette_info) << "Recommended cassette type for procedure "
                                                    << "(PLT: " << pltTarg
                                                    << ", PLS: " << plsTarg
                                                    << ", RBC: " << rbcTarg
                                                    << ") is " << recommendType << endmsg;
      }
   }
   else
   {
      DisposableIDMsg type(MessageBase::SEND_GLOBAL);
      type.send(_CassetteType);
      DataLog(log_level_proc_load_cassette_info) << "Sent the cassette type message " << endmsg;
      _DisposableSelected = 1;
   }





}

void CassetteID::logCalData ()
{
   DataLog(log_level_proc_load_cassette_info) << "rbc init red/green=" << (int)_RedDriveValue << "/" << (int)_GreenDriveValue << endmsg;

   DataLog(log_level_proc_load_cassette_info).flags(ios::showpoint | ios::fixed);
   DataLog(log_level_proc_load_cassette_info).precision(2);
   DataLog(log_level_proc_load_cassette_info) << "rbc cal:"
                                              << " return=" << RbcDat::data().InitRedReturn
                                              << " spillover trigger/samples=" << RbcDat::data().SpilloverTriggerRatio
                                              << "/" << RbcDat::data().SpilloverTriggerSamples
                                              << " cal within " << RbcDat::data().RBCRatioTolerance
                                              << " for " << RbcDat::data().RBCRatioFilter << " periods"
                                              << endmsg;


   DataLog(log_level_proc_load_cassette_info) << "pltplsrbc:"   << RbcDat::data().RedLowPltPlsRBCStamp << "-" << RbcDat::data().RedHighPltPlsRBCStamp << " "
                                              << RbcDat::data().GreenLowPltPlsRBCStamp << "-" << RbcDat::data().GreenHighPltPlsRBCStamp
                                              << "  rbcpls:" << RbcDat::data().RedLowRBCPlsStamp << "-" << RbcDat::data().RedHighRBCPlsStamp << " "
                                              << RbcDat::data().GreenLowRBCPlsStamp << "-" << RbcDat::data().GreenHighRBCPlsStamp
                                              << "  reflectance min/max:" << RbcDat::data().MinimumCassetteReflection << "/" << RbcDat::data().MaximumCassetteReflection
                                              << endmsg;

   DataLog(log_level_proc_load_cassette_info) << "reasonableness " << RbcDat::data().RBCTestMinRed << " " << RbcDat::data().RBCTestMaxRed << " "
                                              << RbcDat::data().RBCTestMinGreen << " " << RbcDat::data().RBCTestMaxGreen << " "
                                              << RbcDat::data().RBCTestFreq << " " << RbcDat::data().RBCTestSamples
                                              << endmsg;
}

void CassetteID::reset ()
{

   //
   // Reset my flags ...
   //
   _RetryAttempted     = 0;
   _RedDriveValue      = 0;
   _GreenDriveValue    = 0;
   _DisposableSelected = 0;
   _LoadError          = CASSETTE_OK;
   _CassetteType       = 0;

}

void CassetteID::copyOver (const CassetteID&)
{}

void CassetteID::cleanup ()
{}

/* FORMAT HASH b2547335b93e7ca8831c5473bd0ccaed */

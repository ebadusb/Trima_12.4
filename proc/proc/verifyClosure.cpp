/*
 * Copyright (c) 2010 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      VerifyClosure.cpp
 *
 */


#include "verifyClosure.h"
#include "states.h"
#include "cobeconfig.h"


DEFINE_STATE(VerifyClosure);

const float PASSDELTA      =     CobeConfig::data().MssVerifyClosureAPSDelta;
const float VOLUMELIMIT    =     CobeConfig::data().MssVerifyClosureMaxVol;
const float INLETTESTSPEED =     CobeConfig::data().MssVerifyClosureInletSpeed;
const float ACTESTSPEED    =     CobeConfig::data().MssVerifyClosureACSpeed;
const float ACPRESSSPEED   =     CobeConfig::data().MssVerifyClosureACPresureUpSpeed;
const float ALERTS_ALLOWED =     2;


VerifyClosure::VerifyClosure()
   : StateAbs(),
     _acClampNotClosedAlert(AC_CLOSURE_FAILED_ALERT),
     _acClampNotClosedAlarm(AC_CLOSURE_FAILED_ALARM),
     _acClampNotClosedAlarm_butContinuePAS(AC_CLOSURE_FAILED_CONTINUE_PAS_ALARM),
     _fluidDetectedAlarm(AC_NOT_REMOVED_ALERT),
     _fluidAtSensor(false),
     _numberOfClosureAttempts(0),
     _transitionNow(false),
     _alarmTriggered(false),
     _pressureUp(false),
     _inPumpDown(false),
     _initialApsPressure(0.0f),
     _initialVac(0.0f),
     _initialVin(0.0f),
     _pumpDownInletVolm(0.0f)
{}



VerifyClosure::VerifyClosure(const VerifyClosure& state)
   : StateAbs(),
     _acClampNotClosedAlert(AC_CLOSURE_FAILED_ALERT),
     _acClampNotClosedAlarm(AC_CLOSURE_FAILED_ALARM),
     _acClampNotClosedAlarm_butContinuePAS(AC_CLOSURE_FAILED_CONTINUE_PAS_ALARM),
     _fluidDetectedAlarm(AC_NOT_REMOVED_ALERT),
     _fluidAtSensor(false),
     _numberOfClosureAttempts(0),
     _transitionNow(false),
     _alarmTriggered(false),
     _pressureUp(false),
     _inPumpDown(false),
     _initialApsPressure(0.0f),
     _initialVac(0.0f),
     _initialVin(0.0f),
     _pumpDownInletVolm(0.0f)
{
   copyOver(state);
}

VerifyClosure::~VerifyClosure()
{
   cleanup();
}

//
//
//
void VerifyClosure::copyOver (const VerifyClosure& copyObj)
{}



void VerifyClosure::reset ()
{}



//
//
//
void VerifyClosure::cleanup ()
{}



//
// One-time setup done on initial transition
//
int VerifyClosure::preEnter ()
{
   // Capture Initial APS pressure
   _initialApsPressure = _pd.Status().APS();
   _initialVac         = _pd.Volumes().VacTotal;  // This one always accumulates
   _initialVin         = _pd.Volumes().VinTotal;  // This one always accumulates


   DataLog(log_level_proc_mss_debug) << "Initial APS is " << _initialApsPressure << "mmHg" << endmsg;

   /////////// PFR releated code /////////////////////////////////////
   if (!_pd.Run().mssInPFR)  // if we're NOT  in PFR
   {
      _pd.MakeRunWritable();
      _pd.Run().currentMSSSubState.Set(SS_VERIFYCLOSURE);
      _pd.MakeRunReadable();

   }
   else
   {
      if ( _pd.Run().currentMSSSubState.Get() == SS_VERIFYCLOSURE )
      {
         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR" << endmsg;
         _pd.MakeRunWritable();
         _pd.Run().mssInPFR.Set(false);
         _pd.MakeRunReadable();
      }
   }

   return NORMAL;
}



//
// One-time setup done on initial transition
//
int VerifyClosure::postExit ()
{
   _transitionNow = false;
   _pd.Orders().Qin(0.0f);
   _pd.Orders().Qac(0.0f);

   return NORMAL;
}



//
// Processing done before State Transition check
//
int VerifyClosure::preProcess ()
{
   _fluidAtSensor = false;

   if (!_inPumpDown)
   {
      if (_pd.Status().ACDetectFluid())
      {
         _fluidDetectedAlarm.setAlarm("Fluid seen during clamp closure test");
         _fluidAtSensor = true;
         _pd.Orders().Qac(0.0f);
         _pd.Orders().Qin(0.0f);
      }
      else if (!_pressureUp)
      {
         _pd.Orders().Qac(ACPRESSSPEED);
         _pd.Orders().Qin(0.0f);

      }
      else
      {
         _pd.Orders().Qin(INLETTESTSPEED);
         _pd.Orders().Qac(ACTESTSPEED);
      }

   }
   else     // pump down the pressure
   {
      _pd.Orders().Qin(30.0f);
   }

   return NORMAL;
}



//
// Processing done after State transition check (still in state)
//
int VerifyClosure::postProcess ()
{
   const float currentAps  =  _pd.Status().APS();
   const float apsDelta    =  currentAps - _initialApsPressure;
   const float acPumpDelta =  _pd.Volumes().VacTotal - _initialVac;
   const float inPumpDelta =  _pd.Volumes().VinTotal - _initialVin;

   // if the fluid is seen at sensor in Pre Porc... do nothing in post proc
   if (_fluidAtSensor)
   {
      return NORMAL;
   }


   if (_inPumpDown)
   {
      _pumpDownInletVolm += _pd.Status().ACPump.dV();


      if (   currentAps <= 0.0f
             || _pumpDownInletVolm > 45.0f )
      {

         DataLog(log_level_proc_mss_info) << "Pressure pump down finished. aps= " << currentAps << " mmHg. " <<  _pumpDownInletVolm << "mL of Vin pump."  << endmsg;
         _pumpDownInletVolm = 0.0f;
         _inPumpDown        = false;

         // reset the testing criteria.
         _initialVin =  _pd.Volumes().VinTotal;
         _initialVac =  _pd.Volumes().VacTotal;

         AlertOrAlarm();

      }
      return NORMAL;
   }

   // Baseleine Pressure to zero............
   if (!_pressureUp)
   {
      if (currentAps > 0.0f || acPumpDelta > 10.0f)    // acPumpDelta check is to keep from running forever cases....
      {
         _pressureUp = true;

         // stop pumps to get the reset readings
         _pd.Orders().Qac(0.0f);
         _pd.Orders().Qin(0.0f);

         _initialApsPressure = _pd.Status().APS();
         _initialVin         =  _pd.Volumes().VinTotal;
         _initialVac         =  _pd.Volumes().VacTotal;

         DataLog(log_level_proc_mss_info) << "raised APS for the test. aps= " << currentAps << " mmHg. " <<  acPumpDelta << "mL of AC pump."  << endmsg;
      }

      // Run until the pressure delta is seen......   (or not)
   }
   else
   {


      // failure..........
      if (currentAps > 400.0f)
      {

         DataLog(log_level_proc_mss_info) << "Failed, Closure NOT Verified. (pressure too high)" << endmsg;
         DataLog(log_level_proc_mss_info) << "Current APS " << currentAps << "mmHg." << endmsg;
         DataLog(log_level_proc_mss_info) << "APS delta   " << apsDelta << "mmHg." << endmsg;
         DataLog(log_level_proc_info) << "AC    Pump total -> " << acPumpDelta  << "ml" << endmsg;
         DataLog(log_level_proc_info) << "Inlet Pump total -> " << inPumpDelta  << "ml" << endmsg;

         _pd.Orders().Qin(0.0f);
         _pd.Orders().Qac(0.0f);

         _inPumpDown = true;


         // pass............
      }
      else if (apsDelta <= PASSDELTA)
      {

         DataLog(log_level_proc_mss_info) << "Passed, Closure Verified." << endmsg;
         DataLog(log_level_proc_mss_info) << "Current APS " << currentAps << "mmHg." << endmsg;
         DataLog(log_level_proc_mss_info) << "APS delta   " << apsDelta << "mmHg reached.  Stopping  pumps." << endmsg;
         DataLog(log_level_proc_info) << "AC    Pump total -> " << acPumpDelta  << "ml" << endmsg;
         DataLog(log_level_proc_info) << "Inlet Pump total -> " << inPumpDelta  << "ml" << endmsg;

         _pd.Orders().Qin(0.0f);
         _pd.Orders().Qac(0.0f);

         // Test completed
         _transitionNow = true;



         // pending.........
      }
      else
      {

         // failure..........
         if (inPumpDelta >= VOLUMELIMIT)
         {

            DataLog(log_level_proc_mss_info) << "Failed, Closure NOT Verified. (test volume exceeded)" << endmsg;
            DataLog(log_level_proc_mss_info) << "Current APS " << currentAps << "mmHg." << endmsg;
            DataLog(log_level_proc_mss_info) << "APS delta   " << apsDelta << "mmHg reached." << endmsg;
            DataLog(log_level_proc_info) << "AC    Pump total -> " << acPumpDelta  << "ml" << endmsg;
            DataLog(log_level_proc_info) << "Inlet Pump total -> " << inPumpDelta  << "ml" << endmsg;

            _pd.Orders().Qin(0.0f);
            _pd.Orders().Qac(0.0f);

            _inPumpDown = true;

         }
         //  pending.........
         // else
         // running pumps and waiting for pressure or volume to build....



      }



   }

   return NORMAL;



}



void VerifyClosure::AlertOrAlarm ()
{


   // alert or alarm here....
   if (_numberOfClosureAttempts < ALERTS_ALLOWED)
   {

      if  (_acClampNotClosedAlert.getState() == CLEARED)
      {

         DataLog(log_level_proc_info) << "AC Closure Test ALERT attempt #" << _numberOfClosureAttempts << endmsg;
         _acClampNotClosedAlert.setAlarm();
         _numberOfClosureAttempts++;

         // reset so we do another  "MssAcClosureMaxVol"  ml
         _initialVac = _pd.Volumes().VacTotal;
         _initialVin = _pd.Volumes().VinTotal;        // This one always accumulates
      }

   }
   else
   {


      if  (_acClampNotClosedAlarm.getState() == CLEARED & !_alarmTriggered)
      {
         _alarmTriggered = true;

         // special processing to allow continue if RBC fails and PLT passes
         if (_pd.RunTargets().MeterRbcs && _pd.RunTargets().MeterPlatelets)
         {
            _acClampNotClosedAlarm_butContinuePAS.setAlarm("AC Closure Test failed final attempt -- continue PAS");

            DataLog(log_level_proc_info) << "Disabling RBC because of AC Closure Test failure" << endmsg;

            // set flag indicating RBC metering is OFF due to errors
            _pd.MakeRunWritable();
            _pd.Run().MeterRbcs_OK = false;
            _pd.MakeRunReadable();
         }
         else
         {

            _acClampNotClosedAlarm.setAlarm("AC Closure Test failed final attempt");

         }

      }

   }

}



int VerifyClosure::transitionStatus ()
{
   bool timeToTransition = false;

   // If not metering RBC's(either through setup or by erroring out) or RBC's have not been collected, exit
   if (!(_pd.RunTargets().MeterRbcs && _pd.Run().MeterRbcs_OK) /*|| (_pd.Volumes().RBCVolume == 0.0f)*/ )
   {
      DataLog(log_level_proc_mss_info) << "Not metering RBC's or no RBC volume collected.  AC Closure test will not be performed" << endmsg;
      timeToTransition = true;
   }

   if (_pd.Run().mssInPFR)  // if we're in PFR
   {
      if ( _pd.Run().currentMSSSubState > SS_VERIFYCLOSURE  )
         timeToTransition = true;
   }

   // Normal exit.
   if (_transitionNow || timeToTransition)
   {
      DataLog (log_level_proc_mss_info) << "Verify closure complete." << endmsg;
      VerifyAcClosureCompleteMsg acClosureVerified(MessageBase::SEND_LOCAL);
      acClosureVerified.send();
      return 1;
   }


   return NO_TRANSITION;
}

/* FORMAT HASH 32a706d2c1002fd4d8e4cecf061285dc */

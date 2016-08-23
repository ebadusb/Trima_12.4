/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      verifyClosure3.cpp
 *
 * Return and Inlet pump @ 50 mls/min until Reservoir Empty.
 * Returned volume should be less than 6 mls.
 *
 * Log:
 *    $Log: verifyClosure3.cpp $
 *    Revision 1.18  2008/10/01 21:34:35Z  jheiusb
 *    8007 -- redo flagging for clampclosure and mssPltPrime errors.
 *    Revision 1.17  2008/04/08 21:43:42Z  jheiusb
 *    8115 -- fix prime and General PFR stuff
 *    Revision 1.16  2007/04/02 20:20:25Z  jheiusb
 *    7256 -- do the pressure balanced inlet/return pump running
 *    Revision 1.15  2006/12/04 16:17:08Z  jheiusb
 *    remerge from original rbc+plt mss 5.2
 *    Revision 1.14  2006/11/07 23:24:21Z  jheiusb
 *    7792 -- runt pull and test in parallel for time savings
 *    Revision 1.13  2006/10/11 18:10:12Z  jheiusb
 *    7780 add back in purge at end
 *    Revision 1.12  2006/09/28 20:22:35Z  jheiusb
 *    7707 -- add rinseback monitor for MSS
 *    Revision 1.11  2006/08/22 22:24:29Z  jheiusb
 *    7673 -- add reason for salavge case wbc- rinseback failed
 *    Revision 1.10  2006/07/26 22:38:16Z  jheiusb
 *    7672 -- add MSS rinseback started flag
 *    Revision 1.9  2006/07/21 16:22:11Z  jheiusb
 *    7601 -- salvage case added
 *    Revision 1.8  2006/06/09 16:44:39Z  jheiusb
 *    7583 -- fix transition on second failure
 *    Revision 1.7  2006/04/19 22:09:06Z  jheiusb
 *    7491 -- PFR hangs
 *    Revision 1.6  2006/04/19 20:04:56Z  jheiusb
 *    7491 -- PFR hangs
 *    Revision 1.5  2006/04/07 19:34:14Z  jheiusb
 *    7476 -- Pumps not saved in CDS
 *    Revision 1.4  2006/03/30 20:49:13Z  jheiusb
 *    7350 -- Metered storage PFR
 *    Revision 1.3  2006/03/27 17:53:27Z  jheiusb
 *    7350 -- PFR for MSS
 *    Revision 1.2  2006/03/03 17:33:54Z  jheiusb
 *    initial merge
 *    Revision 1.1  2006/03/03 17:16:03Z  jheiusb
 *    Initial revision
 *    Revision 1.9  2006/03/02 18:16:58Z  jheiusb
 *    add many MSS values to CobeConfig
 *    Revision 1.8  2006/02/23 17:36:08Z  jheiusb
 *    7344 -- add independent stroke volume alg for MSS, so change volm here
 *    Revision 1.7  2006/01/19 23:09:09Z  jheiusb
 *    7355 -- rework clamp closure tests
 *    Revision 1.6  2006/01/18 23:42:39Z  jheiusb
 *    7355 -- fix contamination
 *    Revision 1.5  2006/01/12 16:29:16Z  jheiusb
 *    7342 -- change spec'ed pump speed
 *    Revision 1.4  2005/12/09 19:57:06Z  jheiusb
 *    7290 -- fix problem with pumps running forever if res -> Empty
 *    Revision 1.3  2005/12/02 17:12:01Z  jheiusb
 *    7267 -- stop pumps on exiting the state (postExit)
 *    Revision 1.2  2005/11/30 19:13:46Z  jheiusb
 *    7266 -- inlet alarm fixed, along with inlet pump volm accrual
 *    Revision 1.1  2005/10/21 21:50:49Z  rm70006
 *    Initial revision
 *
 */


#include "verifyClosure3.h"
#include "states.h"
#include "cobeconfig.h"


DEFINE_STATE(VerifyClosure3);


const float collectSpeedPhase1    = CobeConfig::data().MssChannelCheck1CollectSpeed;
const float collectSpeedPhase3    = CobeConfig::data().MssChannelCheck2CollectSpeed;
const float plasmaSpeedPhase3     = CobeConfig::data().MssChannelCheck2PlasmaSpeed;
const float collectVolPhase1      = CobeConfig::data().MssChannelCheck1CollectVol;
const float plasmaVolPhase3       = CobeConfig::data().MssChannelCheck2PlasmaVol;

const float returnInletMatchSpeed = CobeConfig::data().MssReturnInletMatchSpeed;

VerifyClosure3::VerifyClosure3()
   : StateAbs(),
     _channelClampAlert(CHANNEL_CLAMP_NOT_CLOSED_ALERT),
     _transitionNow(false),
     _testPassed(false),
     _testFailed(false),
     _testPumpingDone(false),
     _endPurgeDone(false),
     _doingSalvageCase(false),
     _testAttempts(0),
     _initialVin(0.0f),
     _initialVrp(0.0f),
     _initialVp(0.0f),
     _initialVc(0.0f)
{}



VerifyClosure3::VerifyClosure3(const VerifyClosure3& state)
   : StateAbs(),
     _channelClampAlert(CHANNEL_CLAMP_NOT_CLOSED_ALERT),
     _transitionNow(false),
     _testPassed(false),
     _testFailed(false),
     _testPumpingDone(false),
     _endPurgeDone(false),
     _doingSalvageCase(false),
     _testAttempts(0),
     _initialVin(0.0f),
     _initialVrp(0.0f),
     _initialVp(0.0f),
     _initialVc(0.0f)
{
   copyOver(state);
}



VerifyClosure3::~VerifyClosure3()
{
   cleanup();
}



//
//
//
void VerifyClosure3::copyOver (const VerifyClosure3&)
{}



//
//
//
void VerifyClosure3::cleanup ()
{}



//
// One-time setup done on initial transition
//
int VerifyClosure3::preEnter ()
{

   // Capture Initial Volumes
   _initialVin = _pd.Volumes().VinTotal.Get();
   _initialVrp = _pd.Volumes().Vreturn.Get()  ;   // always get the saved init volm in case of PFR (just for Return pump )
   _initialVp  =  _pd.Volumes().Vp.Get();
   _initialVc  =  _pd.Volumes().Vc.Get();




   if (!_pd.Run().mssInPFR)   // if we're NOT  in PFR
   {
      _pd.MakeRunWritable();
      // setup this for PFR
      _pd.Run().currentMSSSubState.Set(SS_VERIFYCLOSURE3);
      _pd.Run().clampCloseRetVolm.Set(_pd.Volumes().Vreturn.Get());
      _pd.MakeRunReadable();

   }
   else
   {
      // in PFR get saved volm
      _initialVrp = _pd.Run().clampCloseRetVolm.Get();


      if (_pd.Run().currentMSSSubState.Get() == SS_VERIFYCLOSURE3)
      {
         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR" << endmsg;
         // back to where we PFR'ed reset flag and carry on
         _pd.MakeRunWritable();
         _pd.Run().mssInPFR.Set(false);
         _pd.MakeRunReadable();
      }

   }

   // if we did'nt do rinseback skip this is salvage case
   // oh, and flag it too!
   if ( !_pd.Run().rinsebackVolumeSuccessMss.Get()  )
   {

      DataLog(log_level_proc_info) << "shorting VerifyClosure3 because rinseback not entered!"   << endmsg;
      _doingSalvageCase = true;

   }

   DataLog(log_level_proc_info) << " initial Return Volume is => " << _initialVrp << endmsg;

   return NORMAL;
}



//
// One-time setup done on initial transition
//
int VerifyClosure3::postExit ()
{

   _transitionNow = false;
   // shut the pumps off when this state exits
   _pd.Orders().Qrp(0.0f);
   _pd.Orders().Qin(0.0f);
   _pd.Orders().Qcollect(0.0f);
   _pd.Orders().Qplasma(0.0f);


   return NORMAL;
}



//
// Processing done before State Transition check
//
int VerifyClosure3::preProcess ()
{

   const float aps = _pd.Status().APS();

   // if we're still testing
   if (!_testPassed || !_testFailed)
   {

      if (!_testPumpingDone)
      {
         _pd.Orders().Qcollect(collectSpeedPhase1);
      }
      if (!_doingSalvageCase)
      {
         if (!_pd.Status().ReservoirIsEmpty())
         {

            if (aps < CobeConfig::data().MssApsBackpressure)
            {
               _pd.Orders().Qrp(returnInletMatchSpeed);
               _pd.Orders().Qin(0.0f);

            }
            else
            {
               _pd.Orders().Qrp(returnInletMatchSpeed);
               _pd.Orders().Qin(returnInletMatchSpeed);
            }

         }
         else
         {

            _pd.Orders().Qrp(0.0);
            _pd.Orders().Qin(0.0f);

         }
      }
   }

   if ( _testPassed && !_endPurgeDone )
   {
      _pd.Orders().Qrp(0.0f);
      _pd.Orders().Qin(0.0f);
      _pd.Orders().Qcollect(collectSpeedPhase3);
      _pd.Orders().Qplasma(plasmaSpeedPhase3);
   }



   return NORMAL;


}
int VerifyClosure3::postProcess ()
{

   float Vpdelta  = _pd.Volumes().Vp.Get() - _initialVp;
   float Vcdelta  = _pd.Volumes().Vc.Get() - _initialVc;
   float Vindelta = _pd.Volumes().VinTotal.Get() - _initialVin;
   float Vrpdelta = _pd.Volumes().Vreturn.Get() - _initialVrp;


   const float aps = _pd.Status().APS();


   if (Vcdelta >= collectVolPhase1)
   {
      _testPumpingDone = true;
   }


   if ((_testAttempts < 2) && !_testPassed)
   {
      if (_doingSalvageCase)
      {
         if (Vcdelta >= collectVolPhase1)
         {
            _testPassed = true;
            _testFailed = false;
            DataLog(log_level_proc_mss_info) << "Salvage case auto pass"   << " attempts -> " << _testAttempts <<  endmsg;
         }

      }
      else
      {
         _testPassed = false;
         _testFailed = false;
      }

   }
   else if  ( (_testAttempts >= 2) && !_testPassed)
   {

      _testPassed = true;
      _testFailed = true;

      DataLog(log_level_proc_mss_info) << "Test Passed"   << " attempts -> " << _testAttempts << endmsg;

   }
   else
   {


      _testPassed = true;
      _testFailed = true;


   }

   // if we're still testing
   if (!_testPassed || !_testFailed)
   {

      if (Vrpdelta > CobeConfig::data().MssCassetteVolMax )
      {

         DataLog(log_level_proc_mss_info) << "Failure  Vrpdelta"  << Vrpdelta << "ml ;Vcdelta " << Vcdelta << "ml" << endmsg;

         _testFailed = true;
         _testPassed = false;
         _testAttempts++;

         _pd.MakePQIWritable();
         _pd.PQI().ClampClosureFailedPartFailure.Set(true);
         _pd.MakePQIReadable();

         DataLog(log_level_proc_mss_info) << "ClampClosureFailedPartFailure Set"  << endmsg;

         // zero the accruals and reset  for another try
         _initialVin = _pd.Volumes().VinTotal;
         _initialVrp = _pd.Volumes().Vreturn;
         _initialVp  = _pd.Volumes().Vp;
         _initialVc  = _pd.Volumes().Vc;


         if ( ( _channelClampAlert.getState() == CLEARED) )
         {

            DataLog(log_level_proc_mss_info) << "ALERT: Return Volume indicates channel NOT clamped. volm-> "  << Vrpdelta << "ml" << endmsg;
            _channelClampAlert.setAlarm("Channel Clamp closure test failed.");
         }


      }

      if (  Vrpdelta <= CobeConfig::data().MssCassetteVolMax
            && _testPumpingDone)
      {
         _testFailed = false;
         _testPassed = true;

         DataLog(log_level_proc_mss_info) << "PASS  Vrpdelta"  << Vrpdelta << "ml ;Vcdelta " <<  Vcdelta << "ml" << endmsg;
      }


   }


   if ( _testPassed && !_endPurgeDone )
   {

      if (Vpdelta >= plasmaVolPhase3)
      {

         DataLog(log_level_proc_mss_info) << "_endPurgeDone -- Vpdelta"  << Vpdelta << "ml ; Vcdelta " << Vcdelta << "ml" << endmsg;
         _endPurgeDone  = true;
         _transitionNow = true;
      }
   }


   return NORMAL;

}




int VerifyClosure3::transitionStatus ()
{


   // if we're not doing MSS for PLT then get the heck out of here
   if (!(_pd.RunTargets().MeterPlatelets && _pd.Run().MeterPlatelets_OK))
   {

      DataLog(log_level_proc_mss_info) << "Not metering PLT's or no PLT volume collected.  Clamp Closure test will not be performed" << endmsg;
      return 1;
   }


   if (_pd.TrimaSet().Cassette == cassette::RBCPLS)
   {
      DataLog(log_level_proc_mss_info) << "Black stamp set detected.  Clamp Closure test will not be performed." << endmsg;
      return 1;
   }

   if (_pd.Run().mssInPFR)  // if we're in PFR
   {

      if ( _pd.Run().currentMSSSubState > SS_VERIFYCLOSURE3  )
         return 1;
   }

   if (_transitionNow)
   {
      return 1;
   }



   return NO_TRANSITION;
}

/* FORMAT HASH 5efa40d236b97b5d9b3aeaf8bb368761 */

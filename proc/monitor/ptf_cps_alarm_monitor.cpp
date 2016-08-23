/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centpres_alarm.cpp
 *
 * Revision 1.3  2009/03/31 21:37:23Z  jheiusb
 * 8988 -- add logging for OR
 * Revision 1.2  2009/02/13 22:59:55Z  jheiusb
 * 9002 -- cps mods
 * Revision 1.1  2009/02/09 23:54:13Z  jheiusb
 * Initial revision
 * Revision 1.20  2008/09/08 16:28:31Z  dslausb
 * IT 8306 - Split should always be based on configured RBC dose with no scaling or adjustments.
 * Revision 1.19  2008/02/21 20:42:53Z  jheiusb
 *  -- removed PTFRbcProduct flag as redundant to MSSRbc flag
 * Revision 1.18  2007/12/21 21:23:26Z  rm70006
 * IT 8418.  Remove excessive logging.
 * Revision 1.17  2007/12/12 22:24:44Z  jheiusb
 * 8241 -- alarm counter reset
 * Revision 1.16  2007/12/11 15:31:00Z  jheiusb
 * 8385 -- roll over to product 2 on CPS alarm
 * 8384 -- cancle metering is 2 CPS on a product
 * Revision 1.15  2007/11/30 17:15:37Z  jheiusb
 * just added logging for QA testing.
 * Revision 1.14  2007/11/08 20:51:23Z  jheiusb
 * 8200  -  end srbc on CPS if black stamp.
 * Revision 1.13  2007/11/07 22:58:04Z  jheiusb
 * 8294 -- dont allow second alarm as continuable on single RAS
 * Revision 1.12  2007/11/06 23:16:59Z  jheiusb
 * 8200 -- seperate product handling for RBC
 * 8294 -- seperate product handling for RBC
 * Revision 1.11  2007/10/26 17:03:50Z  jheiusb
 * 8082 -- fix the alarm count on Prod 2.
 * Revision 1.10  2007/10/26 16:49:56Z  jheiusb
 * 8082 -- PTF CPS alarms end product based
 * Revision 1.9  2007/10/18 20:03:42Z  jheiusb
 * 8254
 * Revision 1.8  2007/09/19 16:09:30Z  jheiusb
 * 8055 -- remove annoying logging statement
 * Revision 1.7  2007/08/30 22:52:49Z  jheiusb
 * 8057 -- auto split black stamp
 * Revision 1.6  2007/07/11 23:23:33Z  jheiusb
 * 7180 -- move the CPS alarm to after first draw to allow Rinseback
 * Revision 1.5  2007/06/08 17:14:55Z  jheiusb
 * 8060, 7180 -- fix the PTF alarms for CPS and the baselining algorithm for doubles
 * Revision 1.4  2007/06/05 17:17:05Z  jheiusb
 * 8060 -- clean up CPS PTF monitor
 * Revision 1.3  2007/03/20 21:26:25Z  jheiusb
 * 7215 -- port PTF FMEA mitigations to 5.R
 * Revision 1.2  2007/03/15 18:20:45Z  jheiusb
 * 7215 - temporary fix
 * Revision 1.1  2007/03/15 17:03:28Z  jheiusb
 * Initial revision
 * Revision 1.6  2005/12/22 20:45:41Z  rm70006
 * IT 7197.  Fix baseline pressure calculation.
 *
 */

#include "ptf_cps_alarm_monitor.h"
#include "trima_datalog.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "MssCommon.h"   // common MSS functions
#include "predict_msgs.h"

DEFINE_OBJ(PTFCPS_Alarm_Monitor);

PTFCPS_Alarm_Monitor::PTFCPS_Alarm_Monitor()
   : _PTFCPSBaselineTestAlarm(CPS_PTF_BASELINE_TEST_ALARM),      // first 50ml
     _PTFCPSBaselineTestAlert(CPS_PTF_BASELINE_TEST_ALERT),
     _PTFCPSAlert(CPS_HIGH_DURING_RBC_COLLECT_ALERT),              // after the baseline established
     _PTFCPSAlert2(CPS_HIGH_DURING_RBC_COLLECT_PROD2_OK_ALERT),    // Disables product 1 for further collect or metering
     _PTFCPSAlarm(CPS_HIGH_DURING_RBC_COLLECT_ALARM),              // Disables product 2 for further collect or metering
     _averageCPSPressure(0.0),
     _accumCPSPressure(0.0),
     _highCPSPressure(0.0),
     _numberOfSamples(0),
     _baselineDetermined(false),
     _HighPTFCPSBaselinePressureAlarmPoint(0),
     _numberOfAlerts(0),
     _resetForDouble(false),
     _AllowableFailures(1) // number of alerts before alarming R2
{
   _lowCPSPressure         = CobeConfig::data().CentPresLimitLow;    // the alarm pressure
   _baselineVolumeEndPoint = CobeConfig::data().PTFVrbcCollectPoint; // the volume to create the baseline
   _blackout_volume        =  CobeConfig::data().PTFVrbcCollectCPSNocount;
}


PTFCPS_Alarm_Monitor::~PTFCPS_Alarm_Monitor()
{}

void PTFCPS_Alarm_Monitor::Monitor ()
{
   ProcData pd;

   if (!pd.RunTargets().MeterRbcs.Get())
   {
      return; // only for MSS PTF ie. MSSRBC
   }

   float CPSPressure     = pd.Status().CPS() - pd.TrimaSet().CPSZero;
   float RBCVolume       = pd.Volumes().RBCVolume;

   bool  isRBCCollecting = false;

   if (pd.Status().RBCValve.IsCollecting())
   {
      isRBCCollecting = true;

   }
   else
   {
      isRBCCollecting = false;
   }


   // PTF Specific alarms.  Only check with PTF set and while collecting
   if (   pd.RunTargets().MeterRbcs.Get()
          &&  (isRBCCollecting)
          &&  (pd.Status().InletPump.CmdRPM() > 0)
          )
   {

      if (_baselineDetermined)
      {
         checkPressure(CPSPressure);
      }
      else
      {

         if ( RBCVolume < _baselineVolumeEndPoint)
         {
            findBaseline(CPSPressure, RBCVolume);
         }
         else
         {
            setBaseline();
         }
      }

   } // endif  pd.RunTargets().PTFRbcProduct && .....

   ///////////////////////////////////////////////////////////////
   // reset the calculation if we start a double
   if (!_resetForDouble)
   {
      string currState        =  pd.SubstateName();
      string RBCCollectDouble = "RBCCollectionDouble";

      if (RBCCollectDouble  == currState  )
      /* if we have a Black stamp prime include it here as an or */
      {
         static bool reset = false;
         if (!reset)   // baseline was known ... now we reset, makes this oneshot code
         {
            reset               = true;
            _baselineDetermined = false;

            // 50mL from where we are now (begining RBCDoublecollect)
            _baselineVolumeEndPoint = CobeConfig::data().PTFVrbcCollectPoint + RBCVolume;

            // put the blackout period on the second product too!
            _blackout_volume = _blackout_volume + RBCVolume;


            DataLog(log_level_proc_alarm_monitor_info) << " RESET THE CPS BASELINE FOR A DOUBLE! ::  "
                                                       << " reset the baseline Volume from  " << RBCVolume
                                                       << " to " << _baselineVolumeEndPoint
                                                       << " blackout volume to " << _blackout_volume
                                                       << endmsg;

            _resetForDouble  = true;
            _numberOfSamples = 0;
            _accumCPSPressure                     = 0.0f;
            _lowCPSPressure                       = CobeConfig::data().CentPresLimitLow;
            _highCPSPressure                      = 0.0f;
            _HighPTFCPSBaselinePressureAlarmPoint = 0.0f;
            _numberOfAlerts                       = 0;

         }
      }
   }
   ///////////////////////////////////////////////////////////////

} // end monitor


void PTFCPS_Alarm_Monitor::findBaseline (float thePressure, float currentRBCVolume)
{

   ProcData pd;


   bool isBlackStamp = (pd.TrimaSet().Cassette == cassette::RBCPLS);

   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   bool doubleTargeted =  pd.Config().isPromptedDrbcSplit(pd.RunTargets().ProcNumber.Get());
   //
   // Determine baseline CPS pressure during first 50 (CC) ml of RBC Collection
   //
   _numberOfSamples++;
   _accumCPSPressure += thePressure;

   // remove this logging debug use only:
   // DataLog(log_level_proc_alarm_monitor_info)  << "Sample taken=" << thePressure
   //    << "/@" << currentRBCVolume << "mL" << endmsg;

   if (thePressure > _highCPSPressure)
   {
      _highCPSPressure = thePressure;
   }
   else if (thePressure < _lowCPSPressure)
   {
      _lowCPSPressure = thePressure;
   }

   //
   // PTF CPS Alarm during first 50 ml (CC) of RBC collection is defaulted to 1000 (CC) mmHg.
   // Note:  There after the point is based on the baseline average
   //
   if ( thePressure > CobeConfig::data().PTFRBCInitialCPSLimit )
   {

      if (!pd.AlarmActive())
      {
         // Set the target point to 50 mls from where we are now and reset the CPS accumulators
         _baselineVolumeEndPoint = CobeConfig::data().PTFVrbcCollectPoint + currentRBCVolume;
         _averageCPSPressure     = 0.0f;
         _numberOfSamples        = 0;


         if (_numberOfAlerts < _AllowableFailures)
         {

            // dont count alerts in the first 25mL agains out limit
            if (currentRBCVolume > _blackout_volume)
            {


               _numberOfAlerts++;
               DataLog(log_level_proc_alarm_monitor_info) << "Alert counts against total " << _numberOfAlerts << endmsg;

            }
            else
            {

               DataLog(log_level_proc_alarm_monitor_info) << "Alert within blackout so it doesnt count against total " << endmsg;
            }

            flagRBCVolume();

            _PTFCPSBaselineTestAlert.setAlarm("PTF CPS too high during initial phase of RBC Collect");

            DataLog(log_level_proc_alarm_monitor_info)
               << "PTF CPS is " << thePressure
               << ".  Alarm Limit is "  << CobeConfig::data().PTFRBCInitialCPSLimit
               << ", new RBC Vol collection point is " << _baselineVolumeEndPoint
               << endmsg;

            DataLog(log_level_proc_alarm_monitor_info) << "PTF CPS is " << thePressure << ".  Upper alarm limit is "
                                                       << _HighPTFCPSBaselinePressureAlarmPoint << endmsg;

         }
         else
         {

            flagRBCVolume();
            flagRBCQuality();
            flagWBCContamination();

            if (  !_resetForDouble
                  && isBlackStamp
                  && doubleTargeted )
            {

               // End Run of product ONE
               DataLog(log_level_proc_alarm_monitor_info) << "PTF Alarm during initial phase of RBC collection.  First RBC Product Ending." << endmsg;

               DataLog(log_level_proc_alarm_monitor_info)
                  << "PTF CPS is " << thePressure
                  << ".  Alarm Limit is "  << CobeConfig::data().PTFRBCInitialCPSLimit
                  << ".  RBC volm so far -> " << pd.Volumes().RBCVolume.Get() << "mL"
                  << endmsg;

               pd.SetPluggedFilter();

               pd.MakeRunWritable();
               pd.Run().PTFRBCprod1Disabled.Set(true);
               pd.MakeRunReadable();

               _PTFCPSAlert2.setAlarm("PTF CPS too high during initial phase of RBC Collect For Product 1");

               pd.MakeRunWritable();
               pd.Run().PredictRequest.Set(pd.Run().PredictRequest.Get() | PTF_FILTER_RECALC);
               pd.MakeRunReadable();

               // reset for prod two's adventures
               _numberOfAlerts = 0;


            }
            else
            {

               // Flows already reduced.  End Run.
               DataLog(log_level_proc_alarm_monitor_info) << "PTF Alarm during initial phase of RBC collection.  Ending run." << endmsg;
               DataLog(log_level_proc_alarm_monitor_info)
                  << "PTF CPS is " << thePressure
                  << ".  Upper alarm limit is " << _HighPTFCPSBaselinePressureAlarmPoint
                  << ".  RBC volm so far -> " << currentRBCVolume << "mL"
                  << endmsg;

               pd.SetPluggedFilter();

               // JPH this should only alarm now A1 allow rinseback
               _PTFCPSBaselineTestAlarm.setAlarm("PTF CPS too high during initial phase of RBC Collect.");

               pd.MakeRunWritable();

               pd.Run().PTFRBCprod2Disabled.Set(true);

               if (!isBlackStamp || !doubleTargeted)
               {
                  pd.Run().PTFRBCprod1Disabled.Set(true);
                  pd.Run().MeterRbcs_OK.Set(false);
                  _PTFCPSAlarm.setAlarm("PTF CPS too high during initial phase of RBC Collect.");

               }
               else
               {
                  _PTFCPSAlarm.setAlarm("PTF CPS too high during initial phase of RBC Collect for Product 2.");
               }
               pd.MakeRunReadable();

            }




         }           // endif _numberOfAlerts ....
      }
   }

}


void PTFCPS_Alarm_Monitor::setBaseline ()
{

   ProcData pd;
   // If first time after collection point, determine baseline values.
   if (!_baselineDetermined)
   {
      _averageCPSPressure                   = _accumCPSPressure / _numberOfSamples;

      _HighPTFCPSBaselinePressureAlarmPoint = _averageCPSPressure + CobeConfig::data().PTFCPSRBCCollectDelta;

      DataLog(log_level_proc_alarm_monitor_info) << pd.SubstateName()
                                                 << " PTF CPS Baseline pressure set to " << _averageCPSPressure
                                                 << "["   << _lowCPSPressure
                                                 << " - " << _highCPSPressure
                                                 << "]. High Limit = " << _HighPTFCPSBaselinePressureAlarmPoint
                                                 << "; number of samples collected ->" << _numberOfSamples
                                                 << "; accumulated pressures ->" << _accumCPSPressure
                                                 << endmsg;

      _baselineDetermined = true;
   }

}

void PTFCPS_Alarm_Monitor::checkPressure (float thePressure)
{

   ProcData pd;

   bool     isBlackStamp = (pd.TrimaSet().Cassette == cassette::RBCPLS);

   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   bool doubleTargeted = pd.Config().isPromptedDrbcSplit(pd.RunTargets().ProcNumber.Get());

   if ( (thePressure > _HighPTFCPSBaselinePressureAlarmPoint) ||                            // 455 + baseline (cpsZero'ed)
        (thePressure > CobeConfig::data().PTFCPSRBCCollectLimit) ||                         // 1100 (cpsZero'ed)
        ( pd.Status().CPS() > (pd.TrimaSet().CentPresLimit.Get() - pd.TrimaSet().CPSZero) ) // 1350 old alarm
        )
   {

      DataLog(log_level_proc_alarm_monitor_info) << "Pressure High " << thePressure << endmsg;

      if (_PTFCPSAlert.getState() != ACTIVE)
      {

         DataLog(log_level_proc_alarm_monitor_info) << "Pressure High alaerts so far " << _numberOfAlerts << endmsg;
         if (_numberOfAlerts < _AllowableFailures)
         {
            _numberOfAlerts++;

            if (thePressure > _HighPTFCPSBaselinePressureAlarmPoint)
            {

               DataLog(log_level_proc_alarm_monitor_info) << "Pressure past the Baseline+ offset " << _HighPTFCPSBaselinePressureAlarmPoint  << endmsg;
               flagRBCVolume();
               // flagRBCQuality(); // removed by IT 9188
            }
            else
            {

               DataLog(log_level_proc_alarm_monitor_info) << "Pressure NOT past the Baseline+ offset " << _HighPTFCPSBaselinePressureAlarmPoint  << endmsg;
               flagRBCVolume();
            }


            if ( (thePressure > _HighPTFCPSBaselinePressureAlarmPoint) ||          // 455 + baseline (cpsZero'ed)
                 (thePressure > CobeConfig::data().PTFCPSRBCCollectLimit)          // 1100 (cpsZero'ed)
                 )
            {

               _PTFCPSAlert.setAlarm("Alert::PTF CPS too high during RBC Collect");

               pd.MakeRunWritable();
               pd.Run().DoCPTPTFRecovery.Set(true);         // invoke recovery

               DataLog(log_level_proc_alarm_monitor_info)
                  << "PTF CPS(zeroed) is " << thePressure
                  << "CPS raw is "    << pd.Status().CPS()
                  << ".  Upper alarm limit is " << _HighPTFCPSBaselinePressureAlarmPoint
                  << " OR the raw 1350mmHg "
                  << ".  RBC volm so far -> " << pd.Volumes().RBCVolume.Get() << "mL"
                  << endmsg;

            }
            else
            {


               DataLog(log_level_proc_alarm_monitor_info)
                  << "CPS(zeroed) is below the baseline and limit" << thePressure
                  << "CPS raw is "    << pd.Status().CPS()
                  << ".  Upper alarm limit is " << _HighPTFCPSBaselinePressureAlarmPoint
                  << ".  PTFCPSRBCCollectLimit  is " << CobeConfig::data().PTFCPSRBCCollectLimit
                  << " no alarm thrown just flagging and counting here -- alarm thrown from CentPresAlarm"
                  << ".  RBC volm so far -> " << pd.Volumes().RBCVolume.Get() << "mL"
                  << endmsg;

            }



         }
         else
         {
            flagRBCVolume();
            flagRBCQuality();
            flagWBCContamination();

            // First product on a black stamp may fail if we planned on a double
            if (   !_resetForDouble
                   && isBlackStamp
                   && doubleTargeted)
            {
               // End Run.
               DataLog(log_level_proc_alarm_monitor_info) << "PTF Alarm during RBC collection.  First RBC Product Ending." << endmsg;

               DataLog(log_level_proc_alarm_monitor_info)
                  << "PTF CPS is " << thePressure
                  << ".  Upper alarm limit is " << _HighPTFCPSBaselinePressureAlarmPoint
                  << ".  RBC volm so far -> " << pd.Volumes().RBCVolume.Get() << "mL"
                  << endmsg;

               pd.SetPluggedFilter();

               pd.MakeRunWritable();
               pd.Run().PTFRBCprod1Disabled.Set(true);
               pd.MakeRunReadable();

               _PTFCPSAlert2.setAlarm("PTF CPS too high during RBC Collect For Product 1");

               // DataLog(log_level_proc_alarm_monitor_info) << "!!Sending  Message to re-predict" << endmsg;
               pd.MakeRunWritable();
               pd.Run().PredictRequest.Set(pd.Run().PredictRequest.Get() | PTF_FILTER_RECALC);
               pd.MakeRunReadable();

               // reset for prod two's adventures
               _numberOfAlerts = 0;

               DataLog(log_level_proc_alarm_monitor_info) << "we should switch to RBC2 now!!!" << endmsg;

               // this is the first product failing... still a chance for two unless white stamp
            }
            else
            {

               DataLog(log_level_proc_alarm_monitor_info) << "PTF Alarm during RBC collection.  Ending run." << endmsg;

               DataLog(log_level_proc_alarm_monitor_info)
                  << "PTF CPS is " << thePressure
                  << ".  Upper alarm limit is " << _HighPTFCPSBaselinePressureAlarmPoint
                  << ".  RBC volm so far -> " << pd.Volumes().RBCVolume.Get() << "mL"
                  << endmsg;

               pd.SetPluggedFilter();

               pd.MakeRunWritable();
               pd.Run().PTFRBCprod2Disabled.Set(true);

               if (!isBlackStamp || !doubleTargeted)
               {
                  pd.Run().PTFRBCprod1Disabled.Set(true);
                  pd.Run().MeterRbcs_OK.Set(false);
                  _PTFCPSAlarm.setAlarm("PTF CPS too high during RBC Collect.");

               }
               else
               {

                  _PTFCPSAlarm.setAlarm("PTF CPS too high during RBC Collect for Product 2.");

               }
               pd.MakeRunReadable();


            }

         } // endif _numberOfAlerts ....
      }    // endif no alarms active ....
   }       // endif  if CPSPressure > ....
}




void PTFCPS_Alarm_Monitor::flagRBCVolume ()
{
   if ( !_resetForDouble )  // product 1
   {
      DataLog(_pqLog) << (int)CPS_HIGH << " VERIFY RBC VOLUME because of CPS HIGH alarm during RBC "
                      << "collection of product 1" << endmsg;
      LogIt2(CPS_HIGH, VERIFY_RBC_VOL);
      _pqi.AddReasonToFlag(CPS_HIGH, VERIFY_RBC_VOL);
   }
   else // product 2
   {
      DataLog(_pqLog) << (int)CPS_HIGH << " VERIFY RBC VOLUME because of CPS HIGH alarm during RBC "
                      << "collection of product 2" << endmsg;
      LogIt2(CPS_HIGH, VERIFY_RBC_VOL_PROD2);
      _pqi.AddReasonToFlag(CPS_HIGH, VERIFY_RBC_VOL_PROD2);
   }
}


void PTFCPS_Alarm_Monitor::flagRBCQuality ()
{
   if ( !_resetForDouble )  // product 1
   {
      DataLog(_pqLog) << (int)CPS_HIGH << " VERIFY RBC QUALITY because of CPS HIGH alarm during RBC "
                      << "collection of product 1" << endmsg;
      LogIt2(CPS_HIGH, VERIFY_RBC_QUALITY);
      _pqi.AddReasonToFlag(CPS_HIGH, VERIFY_RBC_QUALITY);
   }
   else // product 2
   {
      DataLog(_pqLog) << (int)CPS_HIGH << " VERIFY RBC QUALITY because of CPS HIGH alarm during RBC "
                      << "collection of product 2" << endmsg;
      LogIt2(CPS_HIGH, VERIFY_RBC_QUALITY_PROD2);
      _pqi.AddReasonToFlag(CPS_HIGH, VERIFY_RBC_QUALITY_PROD2);
   }

}


void PTFCPS_Alarm_Monitor::flagWBCContamination ()
{
   if ( !_resetForDouble )  // product 1
   {
      DataLog(_pqLog) << (int)CPS_HIGH << " COUNT RBC WBC because of CPS HIGH alarm during RBC "
                      << "collection of product 1" << endmsg;
      LogIt2(CPS_HIGH, VERIFY_RBC_WBCS);
      _pqi.AddReasonToFlag(CPS_HIGH, VERIFY_RBC_WBCS);
   }
   else // product 2
   {
      DataLog(_pqLog) << (int)CPS_HIGH << " COUNT RBC WBC because of CPS HIGH alarm during RBC "
                      << "collection of product 2" << endmsg;
      LogIt2(CPS_HIGH, VERIFY_RBC_WBCS_PROD2);
      _pqi.AddReasonToFlag(CPS_HIGH, VERIFY_RBC_WBCS_PROD2);
   }

}

/* FORMAT HASH cb235bce6c4abf63680a4e013cba7c93 */

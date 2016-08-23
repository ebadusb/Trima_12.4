/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssRBCAddition.cpp
 *
 */


#include "mssRBCAddition.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"    // for all the cobe config values
#include "MssCommon.h"

#include "CatalogChecker.h"
#include "guiproc.h"


DEFINE_STATE(MSSRBCAddition);

MSSRBCAddition::MSSRBCAddition()
   : StateAbs(),
     transitionNow(false),
     solutionToBeMeteredOut(0.0),
     initialACVolume(0.0),
     aps2hiPTFalarm(APS_TOO_HIGH_ALARM_MSS),
     aps2hiPTFalarmWithPAS(APS_TOO_HIGH_ALARM_MSS_WITH_PAS),
     aps2hiPTFalarmProd1Ok(APS_HIGH_MSS_P1_OK_ALERT),
     aps2hiPTFalert(APS_TOO_HIGH_PTF_ALERT),
     drbcSplit(DRBC_SPLIT_ALERT),
     _acClampNotClosedAlarm(AC_CLOSURE_FAILED_ALARM),
     _acClampNotClosedAlarm_butContinuePAS(AC_CLOSURE_FAILED_CONTINUE_PAS_ALARM),
     apsInstanceCount(0),
     apsInstanceCount_double(0),
     apsHigh(false),
     thisIsDRBC(false),
     thisIsPartialDRBC(false),
     // shortSecondRBCBagDose(false),
     recoveryFromProductTwo(false),
     doubleMultiplier(2.0)
{}

MSSRBCAddition::MSSRBCAddition(const MSSRBCAddition& state)
   : StateAbs(state),
     transitionNow(false),
     solutionToBeMeteredOut(0.0),
     initialACVolume(0.0),
     aps2hiPTFalarm(APS_TOO_HIGH_ALARM_MSS),
     aps2hiPTFalarmWithPAS(APS_TOO_HIGH_ALARM_MSS_WITH_PAS),
     aps2hiPTFalarmProd1Ok(APS_HIGH_MSS_P1_OK_ALERT),
     aps2hiPTFalert(APS_TOO_HIGH_PTF_ALERT),
     drbcSplit(DRBC_SPLIT_ALERT),
     _acClampNotClosedAlarm(AC_CLOSURE_FAILED_ALARM),
     _acClampNotClosedAlarm_butContinuePAS(AC_CLOSURE_FAILED_CONTINUE_PAS_ALARM),
     apsInstanceCount(0),
     apsInstanceCount_double(0),
     apsHigh(false),
     thisIsPartialDRBC(false),
     thisIsDRBC(false),
     // shortSecondRBCBagDose(false),
     recoveryFromProductTwo(false),
     doubleMultiplier(2.0)
{}

const float APS_RECOVERY_PUMP_SPEED  = CobeConfig::data().MssApsAlertRecoveryQin;
const int   MAX_NUMBER_OF_APS_ALERTS = (int)CobeConfig::data().PTFAPSAlertsPerProductMetering;

MSSRBCAddition::~MSSRBCAddition()
{}

int MSSRBCAddition::preProcess ()
{

   ProcData pd;
   float    CurAPS = pd.Status().APS();

   //// RUN THE PUMPS ////////////////////
   if (apsHigh || recoveryFromProductTwo)
   {
      pd.Orders().Qac (0.0f);
      pd.Orders().Qplasma (0.0f);
      pd.Orders().Qcollect(0.0f);
      pd.Orders().Qrp(0.0);
   }
   else
   {
      pd.Orders().Qin (0.0);

      // IT 9156 some alarms can cause the valve to close... we must re-assert it open after the alarm
      pd.Orders().ValveCmd(HAL_VRBC,  HAL_VALVE_COLLECT);

      if (pd.Run().mssDrbcSplitDone.Get() || !pd.Run().drbcSplitDone.Get() )
         pd.Orders().Qac (calculatePumpSpeed(pd.Volumes().RBCProduct_A_FinalVss.Get()) );
      else
         pd.Orders().Qac (calculatePumpSpeed(pd.Volumes().RBCProduct_B_FinalVss.Get()) );

   }
   ///////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   /////////////////// MANAGE APS STUFF ////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   /////////////////// MANAGE APS STUFF /////////////////////////////////////
   if (thisIsDRBC)
   {

      //  Second half of double metering:
      // This is product 1 usually
      ////////  PRODUCT 1 APS CHECKING //////////////////////////////////////////////////////////////////
      if ( pd.Run().mssDrbcSplitDone.Get() )
      {

         // this if we need to recover from a product 2 APS abort,
         //  we need to bleed off pressure without alarming on product 1
         if (recoveryFromProductTwo)
         {
            if (CurAPS <= CobeConfig::data().MssApsBackpressure)
            {
               DataLog(log_level_proc_info) << "Ending reducing APS From Product2s failure; APS = " <<  CurAPS << endmsg;
               recoveryFromProductTwo = false;
            }
            else
            {
               static bool logit = false;
               if (logit)
               {
                  logit = true;
                  DataLog(log_level_proc_info) << "Beginning reducing APS From Product2s failure; APS = " << CurAPS << endmsg;
               }

               pd.Orders().Qin (APS_RECOVERY_PUMP_SPEED);
               pd.Orders().Qac (0.0);
            }

         }
         else     // this is not a  recoveryFromProductTwo   (Usual path)
         {


            ///////  PRODUCT 1 APS CHECKING //////////////////////////////////////////////////////////////////
            if (  CurAPS >= CobeConfig::data().MssApsHigh || apsHigh  )
            {

               if ( apsInstanceCount_double < MAX_NUMBER_OF_APS_ALERTS )
               {

                  DataLog(log_level_proc_info) << "Double APS alerts so far ->" <<  apsInstanceCount_double << endmsg;
                  if (!apsHigh)
                  {

                     DataLog(log_level_proc_info) << "P1 Found APS too High ->" << CurAPS << endmsg;
                     apsHigh = true;
                     pd.Orders().Qac (0.0);
                     pd.Orders().Qplasma (0.0f);
                     pd.Orders().Qcollect(0.0f);
                     pd.Orders().Qin(0.0f);
                     pd.Orders().Qrp(0.0);
                  }
                  else
                  {

                     DataLog(log_level_proc_info) << "P1 trying to reduce APS too High ->" << CurAPS << endmsg;
                     pd.Orders().Qin (APS_RECOVERY_PUMP_SPEED);
                     pd.Orders().Qac (0.0);

                  }

                  //////  PRODUCT 1 APS ALARM //////////////////////////////////////////////////////////////////
               }
               else
               {         // apsInstanceCount >= numberOfAPSAlerts

                  if (aps2hiPTFalarm.getState() == CLEARED)
                  {
                     DataLog(log_level_proc_info) << "END P1 Alarm, its over for all   #" << apsInstanceCount_double << endmsg;
                     pd.SetPluggedFilter();

                     aps2hiPTFalarm.setAlarm();

                     pd.MakeVolumesWritable();
                     // for end run sum... here is prod B's SS
                     pd.Volumes().RBCProduct_A_FinalVss.Set(pd.Volumes().VstorageRBCBag.Get() - pd.Volumes().RBCProduct_B_FinalVss.Get());
                     pd.MakeVolumesReadable();


                     pd.MakeRunWritable();
                     pd.Run().MssApsTooHighRBCProduct1.Set(MAX_NUMBER_OF_APS_ALERTS + 1);
                     pd.Run().PTFRBCprod1Disabled.Set(true);
                     pd.MakeRunReadable();

                     transitionNow = true;          // done
                  }

               }
               //////  END PRODUCT 1 APS ALARM //////////////////////////////////////////////////////////////////

            }
            /////// END  PRODUCT 1 APS CHECKING //////////////////////////////////////////////////////////////////

            //////  PRODUCT 1 APS ALERT  //////////////////////////////////////////////////////////////////
            if ( (CurAPS <= CobeConfig::data().MssApsBackpressure)
                 && (apsInstanceCount_double < MAX_NUMBER_OF_APS_ALERTS)
                 && apsHigh )
            {


               DataLog(log_level_proc_info) << "P1 APS to backpressure ->" << CurAPS << endmsg;
               pd.Orders().Qin (0.0);

               pd.Orders().Qac (calculatePumpSpeed(pd.Volumes().RBCProduct_A_FinalVss.Get()) );

               apsHigh = false;
               DataLog(log_level_proc_info) << "P1  Alert Op #" << apsInstanceCount_double << endmsg;

               aps2hiPTFalert.setAlarm();
               apsInstanceCount_double++;

               pd.MakeRunWritable();
               pd.Run().RASAdditionSpeedReductionFactor.Set(apsInstanceCount_double);
               pd.Run().MssApsTooHighRBCProduct1.Set(apsInstanceCount_double);
               pd.MakeRunReadable();


               DataLog(log_level_proc_info) << "P1  Speed Reduction Factor" << pd.Run().RASAdditionSpeedReductionFactor.Get() << endmsg;

            }
            ////// END  PRODUCT 1 APS ALERT  //////////////////////////////////////////////////////////////////

         }    // if not recoveryFromProductTwo

         ///////  PRODUCT 2 APS CHECKING //////////////////////////////////////////////////////////////////
         /////////// First half of the double
         // This is product 2 usually
      }
      else
      {

         if (  CurAPS >= CobeConfig::data().MssApsHigh  || apsHigh )
         {

            //// APS CORRECTION PRODUCT 2 ////////////////////////////////////////////////////////////////////
            if ( apsInstanceCount < MAX_NUMBER_OF_APS_ALERTS )
            {

               if (!apsHigh)
               {

                  DataLog(log_level_proc_info) << "P2 Found APS too High ->" << CurAPS << endmsg;
                  apsHigh = true;
                  pd.Orders().Qac (0.0);
                  pd.Orders().Qplasma (0.0f);
                  pd.Orders().Qcollect(0.0f);
                  pd.Orders().Qin(0.0f);
                  pd.Orders().Qrp(0.0);
               }
               else
               {

                  DataLog(log_level_proc_info) << "P2 trying to reduce APS too High ->" << CurAPS << endmsg;
                  pd.Orders().Qin (APS_RECOVERY_PUMP_SPEED);
                  pd.Orders().Qac (0.0);

               }

               //// APS ALARM PRODUCT 2 ////////////////////////////////////////////////////////////////////
            }
            else
            {

               if (aps2hiPTFalarmProd1Ok.getState() == CLEARED)
               {
                  pd.SetPluggedFilter();

                  DataLog(log_level_proc_info) << "END P2 Alarm, its over for product 2   #" << apsInstanceCount << endmsg;
                  aps2hiPTFalarmProd1Ok.setAlarm();


                  if (!pd.Run().mssDrbcSplitDone.Get())
                  {
                     drbcSplit.setAlarm();

                     pd.MakeRunWritable();
                     pd.Run().mssDrbcSplitDone.Set(true);
                     pd.MakeRunReadable();
                  }

                  pd.MakeVolumesWritable();
                  pd.Volumes().RBCProduct_B_FinalVss.Set(pd.Volumes().VstorageRBCBag.Get());            // for end run sum... here is prod A's SS
                  pd.MakeVolumesReadable();

                  pd.MakeRunWritable();
                  pd.Run().PTFRBCprod2Disabled.Set(true);
                  pd.Run().RASAdditionSpeedReductionFactor.Set(0);
                  pd.Run().MssApsTooHighRBCProduct2.Set(MAX_NUMBER_OF_APS_ALERTS + 1);
                  pd.MakeRunReadable();


                  transitionNow          = false;  /// dont quit yet
                  recoveryFromProductTwo = true;   // use the second product path to reduce pressure without alarming.

                  DataLog(log_level_proc_info) << "Prod 2 recieved " << pd.Volumes().RBCProduct_B_FinalVss.Get()  << "mL before alarming out" << endmsg;
               }

            }

         }

         //// APS ALERTS PRODUCT 2 ////////////////////////////////////////////////////////////////////
         if ( (CurAPS <= CobeConfig::data().MssApsBackpressure)
              && (apsInstanceCount < MAX_NUMBER_OF_APS_ALERTS)
              && apsHigh )
         {

            DataLog(log_level_proc_info) << "P2 APS to backpressure ->" << CurAPS << endmsg;
            pd.Orders().Qin (0.0);
            pd.Orders().Qac (calculatePumpSpeed(pd.Volumes().RBCProduct_B_FinalVss.Get()) );

            apsHigh = false;
            DataLog(log_level_proc_info) << "P2  Alert Op #" << apsInstanceCount << endmsg;

            aps2hiPTFalert.setAlarm();
            apsInstanceCount++;

            pd.MakeRunWritable();
            pd.Run().RASAdditionSpeedReductionFactor.Set(apsInstanceCount);
            pd.Run().MssApsTooHighRBCProduct2.Set(apsInstanceCount);
            pd.MakeRunReadable();

            DataLog(log_level_proc_info) << "P2  Speed Reduction Factor" << pd.Run().RASAdditionSpeedReductionFactor.Get() << endmsg;

         }

      }  // end first half of double////////////////////////////


      /// SINGLE APS CHECKING //////////////////////////////////////////////////////////////
      // non doubles due to chance or design ////////////////////////////////////////////
   }
   else
   {


      if (  CurAPS >= CobeConfig::data().MssApsHigh  || apsHigh  )
      {
         //// APS CORRECTION ////////////////////////////////////////////////////////////////////
         if ( apsInstanceCount < MAX_NUMBER_OF_APS_ALERTS )
         {

            if (!apsHigh)
            {

               DataLog(log_level_proc_info) << "S Found APS too High ->" << CurAPS << endmsg;
               apsHigh = true;
               pd.Orders().Qac (0.0);
               // pd.Orders().Qplasma ( 0.0f );
               // pd.Orders().Qcollect( 0.0f );
               pd.Orders().Qin(0.0f);
               pd.Orders().Qrp(0.0);

            }
            else
            {

               DataLog(log_level_proc_info) << "S trying to reduce APS too High ->" << CurAPS << endmsg;
               pd.Orders().Qin (APS_RECOVERY_PUMP_SPEED);
               pd.Orders().Qac (0.0);

            }

            //// APS ALARM ////////////////////////////////////////////////////////////////////
         }
         else
         {

            if (aps2hiPTFalarm.getState() == CLEARED && aps2hiPTFalarmWithPAS.getState() == CLEARED )
            {
               DataLog(log_level_proc_info) << "S Alarm, its over #" << apsInstanceCount << endmsg;
               pd.SetPluggedFilter();

               if (pd.Run().MeterPlatelets_OK.Get())
               {
                  aps2hiPTFalarmWithPAS.setAlarm();
               }
               else
               {
                  aps2hiPTFalarm.setAlarm();
               }

               pd.MakeRunWritable();
               // pd.Run().MeterRbcs_OK.Set(false);
               pd.Run().RBCMeteringDone.Set(true);


               if (pd.Run().mssDrbcSplitDone.Get())
               {
                  // this is product one


                  pd.MakeRunWritable();
                  pd.Run().MssApsTooHighRBCProduct1.Set(MAX_NUMBER_OF_APS_ALERTS + 1);
                  pd.MakeRunReadable();

                  pd.MakeVolumesWritable();
                  pd.Volumes().RBCProduct_A_FinalVss = pd.Volumes().VstorageRBCBag.Get();         // for end run sum... here is prod A's SS
                  pd.MakeVolumesReadable();
               }
               else
               {
                  // this is product two

                  pd.MakeRunWritable();
                  pd.Run().MssApsTooHighRBCProduct2.Set(MAX_NUMBER_OF_APS_ALERTS + 1);
                  pd.MakeRunReadable();

                  pd.MakeVolumesWritable();
                  pd.Volumes().RBCProduct_B_FinalVss = pd.Volumes().VstorageRBCBag.Get();         // for end run sum... here is prod B's SS
                  pd.MakeVolumesReadable();
               }

               DataLog(log_level_proc_info) << "S  Process ended due to APS too High alarm: VstorageRBCBag = "
                                            << pd.Volumes().VstorageRBCBag
                                            << "solutionToBeMeteredOut was suppose to be = " << solutionToBeMeteredOut << endmsg;
               transitionNow = true;

            }
         }
      }

      //// APS ALERTS ////////////////////////////////////////////////////////////////////
      if ( (CurAPS <= CobeConfig::data().MssApsBackpressure)
           && (apsInstanceCount < MAX_NUMBER_OF_APS_ALERTS)
           && apsHigh )
      {


         apsInstanceCount++;
         DataLog(log_level_proc_info) << "S APS to backpressure ->" << CurAPS << endmsg;
         pd.Orders().Qin (0.0);

         if (pd.Run().mssDrbcSplitDone.Get())
         {

            pd.MakeRunWritable();
            pd.Run().MssApsTooHighRBCProduct1.Set(apsInstanceCount);
            pd.MakeRunReadable();
            pd.Orders().Qac (calculatePumpSpeed(pd.Volumes().RBCProduct_A_FinalVss) );
         }
         else
         {

            pd.MakeRunWritable();
            pd.Run().MssApsTooHighRBCProduct2.Set(apsInstanceCount);
            pd.MakeRunReadable();
            pd.Orders().Qac (calculatePumpSpeed(pd.Volumes().RBCProduct_B_FinalVss) );
         }

         apsHigh = false;
         DataLog(log_level_proc_info) << "S Alert Op #" << apsInstanceCount << endmsg;

         aps2hiPTFalert.setAlarm();

         pd.MakeRunWritable();
         pd.Run().RASAdditionSpeedReductionFactor.Set(apsInstanceCount);
         pd.MakeRunReadable();

         DataLog(log_level_proc_info) << "S  Speed Reduction Factor" << pd.Run().RASAdditionSpeedReductionFactor.Get() << endmsg;

      }


   }

   //  END OF APS MANAGEMENT STUFF /////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   return NORMAL;
}

int MSSRBCAddition::postProcess ()
{

   ProcData pd;

   if (!apsHigh)
   {

      pd.MakeVolumesWritable();
      pd.Volumes().VstorageRBCBag = pd.Volumes().VstorageRBCBag + pd.Status().ACPump.dV();
      pd.MakeVolumesReadable();

      // DataLog(log_level_proc_info) << "total Volume to both RAS bags so far ->" << pd.Volumes().VstorageRBCBag << "mL  " << endmsg;

      //////////////// DRBC  /////////////////////////////////////////////////////////////
      if (thisIsDRBC)
      {

         // 111/////// METERING to PRODUCT 1 /////////////////////////////////////////////
         //  Second half of double metering:
         if ( pd.Run().mssDrbcSplitDone.Get()  )
         {
            // ending condition met......
            if ( pd.Volumes().VstorageRBCBag.Get() >= solutionToBeMeteredOut + pd.Volumes().RBCProduct_B_FinalVss )
            {
               DataLog(log_level_proc_info) << "This looks like the end of the second metering product..."  << endmsg;
               DataLog(log_level_proc_info) << "I've metered to BAG in both phases =" << pd.Volumes().VstorageRBCBag << "mL out" << endmsg;
               DataLog(log_level_proc_info) << "I've metered this phase = " << solutionToBeMeteredOut  << "mL out" << endmsg;
               DataLog(log_level_proc_info) << "first Half volume = " << pd.Volumes().RBCProduct_B_FinalVss  << "mL out" << endmsg;

               pd.MakeVolumesWritable();
               pd.Volumes().RBCProduct_A_FinalVss = pd.Volumes().VstorageRBCBag.Get() - pd.Volumes().RBCProduct_B_FinalVss.Get();
               pd.MakeVolumesReadable();

               DataLog(log_level_proc_info) << "Finished: I've metered " << pd.Volumes().RBCProduct_A_FinalVss  << "mL to Product 1!" << endmsg;
               transitionNow = true;        // done

               // else still metering......
            }
            else
            {


               // disable the replace bag alarm if we are close to comleteing
               if (  !pd.Run().stopCheckingRbcASBagEmpty.Get()
                     &&  pd.Volumes().VstorageRBCBag.Get() >= (solutionToBeMeteredOut * 2.0f ) - 5.0f /*mL*/)
               {

                  DataLog(log_level_proc_info) << "Disabling PAS bag alarm " << pd.Volumes().RBCProduct_A_FinalVss  << "mL to Product 1!" << endmsg;
                  pd.MakeRunWritable();
                  pd.Run().stopCheckingRbcASBagEmpty.Set(true);
                  pd.MakeRunReadable();
               }

               // and keep going
               pd.MakeVolumesWritable();
               pd.Volumes().RBCProduct_A_FinalVss = pd.Volumes().VstorageRBCBag.Get() - pd.Volumes().RBCProduct_B_FinalVss.Get();
               pd.MakeVolumesReadable();
               // DataLog(log_level_proc_info) << "I've metered " << pd.Volumes().RBCProduct_A_FinalVss  << "mL so far to Product 1!" << endmsg;
            }

            ///222////// METERING to PRODUCT 2 /////////////////////////////////////////////
         }
         else
         {

            // ending condition met......
            if (  pd.Volumes().VstorageRBCBag.Get() >= solutionToBeMeteredOut )
            {

               DataLog(log_level_proc_info) << "This looks like the end of the first metering..."  << endmsg;
               DataLog(log_level_proc_info) << "I've metered To BAGs in total=" << pd.Volumes().VstorageRBCBag << "mL out" << endmsg;
               DataLog(log_level_proc_info) << "solutionToBeMeteredOut  = " << solutionToBeMeteredOut  << "mL out" << endmsg;
               DataLog(log_level_proc_info) << "Metered in first half of double (prod 2) = " <<  pd.Volumes().RBCProduct_B_FinalVss << endmsg;

               if (!pd.Run().mssDrbcSplitDone.Get())
               {
                  drbcSplit.setAlarm();
                  pd.MakeRunWritable();
                  pd.Run().mssDrbcSplitDone.Set(true);
                  pd.MakeRunReadable();
               }

               pd.MakeVolumesWritable();
               pd.Volumes().RBCProduct_B_FinalVss.Set(pd.Volumes().VstorageRBCBag.Get() );          // for end run sum... here is prod A's SS
               pd.MakeVolumesReadable();

               DataLog(log_level_proc_info) << "Finished: I've metered " << pd.Volumes().RBCProduct_B_FinalVss  << "mL to Product 2!" << endmsg;
               transitionNow = false;          /// dont quit yet

               // clear as we move to product 1
               pd.MakeRunWritable();
               pd.Run().RASAdditionSpeedReductionFactor.Set(0);
               pd.MakeRunReadable();

               // else still going to product 2
            }
            else
            {

               pd.MakeVolumesWritable();
               pd.Volumes().RBCProduct_B_FinalVss = pd.Volumes().VstorageRBCBag.Get();         // for end run sum... here is prod A's SS
               pd.MakeVolumesReadable();
               // else please keep going

               // DataLog(log_level_proc_info) << "I've metered " << pd.Volumes().RBCProduct_B_FinalVss  << "mL so far to Product 2!" << endmsg;
            }

         }

         ///sss///////////// SINGLE  /////////////////////////////////////////////////////////////
         // non doubles due to chance or design ////////////////////////////////////////////
      }
      else          // all non-double runs:




      {      // ending condition met......
         if (    ( pd.Volumes().RBCProduct_A_FinalVss  >= solutionToBeMeteredOut )
                 || ( pd.Volumes().RBCProduct_B_FinalVss  >= solutionToBeMeteredOut ) )
         {
            DataLog(log_level_proc_info) << "This looks like single metering product run..."  << endmsg;

            // stop the metering!
            pd.Orders().Qac (0.0);

            if (pd.Run().mssDrbcSplitDone.Get() || !pd.Run().drbcSplitDone.Get() )
            {
               pd.MakeVolumesWritable();
               pd.Volumes().RBCProduct_A_FinalVss = pd.Volumes().VstorageRBCBag.Get() - pd.Volumes().RBCProduct_B_FinalVss;          // for end run sum... here is prod B's SS
               DataLog(log_level_proc_info) << "FINISHED: single prod I've metered " << pd.Volumes().RBCProduct_A_FinalVss  << "mL  to Product 1! -- with total to bags = " <<  pd.Volumes().VstorageRBCBag.Get() << endmsg;
               pd.MakeVolumesReadable();
            }
            else
            {
               pd.MakeVolumesWritable();
               pd.Volumes().RBCProduct_B_FinalVss = pd.Volumes().VstorageRBCBag.Get() - pd.Volumes().RBCProduct_A_FinalVss;          // for end run sum... here is prod A's SS
               DataLog(log_level_proc_info) << "FINISHED: single prod I've metered " << pd.Volumes().RBCProduct_B_FinalVss  << "mL  to Product 2! -- with total to bags = " <<  pd.Volumes().VstorageRBCBag.Get() << endmsg;
               pd.MakeVolumesReadable();
            }

            // set this when SS has been completely added
            transitionNow = true;

         }
         else
         {

            // disable the replace bag alarm if we are close to completeing
            if (!pd.Run().stopCheckingRbcASBagEmpty.Get()
                && pd.Volumes().VstorageRBCBag.Get() >= solutionToBeMeteredOut - 5.0f /*mL*/)
            {

               DataLog(log_level_proc_info) << "Disabling PAS bag alarm " << pd.Volumes().VstorageRBCBag.Get()  << "mL to Product!" << endmsg;
               pd.MakeRunWritable();
               pd.Run().stopCheckingRbcASBagEmpty.Set(true);
               pd.MakeRunReadable();
            }

            // else keep going
            if (pd.Run().mssDrbcSplitDone.Get() || !pd.Run().drbcSplitDone.Get() )
            {
               pd.MakeVolumesWritable();
               pd.Volumes().RBCProduct_A_FinalVss = pd.Volumes().VstorageRBCBag.Get() - pd.Volumes().RBCProduct_B_FinalVss;        // for end run sum... here is prod B's SS
               pd.MakeVolumesReadable();
               // DataLog(log_level_proc_info) << "single prod I've metered " << pd.Volumes().RBCProduct_A_FinalVss  << "mL so far to Product 1! -- with total to bags = " <<  pd.Volumes().VstorageRBCBag.Get() << endmsg;
            }
            else
            {
               pd.MakeVolumesWritable();
               pd.Volumes().RBCProduct_B_FinalVss = pd.Volumes().VstorageRBCBag.Get() - pd.Volumes().RBCProduct_A_FinalVss;        // for end run sum... here is prod A's SS
               pd.MakeVolumesReadable();
               // DataLog(log_level_proc_info) << "single prod I've metered " << pd.Volumes().RBCProduct_B_FinalVss  << "mL so far to Product 2! -- with total to bags = " <<  pd.Volumes().VstorageRBCBag.Get() << endmsg;
            }

         }

      }

      // Tell GUI where we're at so we can update the status bar.
      GuiAasUpdater::sendUpdate();

   }  // end if !apsHigh

   return NORMAL;
}

int MSSRBCAddition::transitionStatus ()
{

   ProcData pd;

   // check if manifold is sealed (IT10074)
   if (pd.Status().APS() <= 0.0f)
   {

      if (pd.RunTargets().MeterRbcs && pd.RunTargets().MeterPlatelets)
      {
         _acClampNotClosedAlarm_butContinuePAS.setAlarm("APS blind (possible manifold sealed off) -- continue PAS");
         DataLog(log_level_proc_info) << "Disabling RASAddition because negative pressure (possible manifold sealed off). aps=" << pd.Status().APS() << endmsg;
      }
      else if ( !(pd.RunTargets().MeterRbcs) && pd.RunTargets().MeterPlatelets)
      {
         DataLog(log_level_proc_info) << "Detecting possible manifold sealed off. PAS will continue aps=" << pd.Status().APS() << endmsg;
      }
      else
      {
         _acClampNotClosedAlarm.setAlarm("APS blind (possible manifold sealed off)");
         DataLog(log_level_proc_info) << "Disabling RASAddition because negative pressure (possible manifold sealed off). aps=" << pd.Status().APS() << endmsg;
      }



      // set flag indicating RBC metering is OFF due to errors
      pd.MakeRunWritable();
      pd.Run().MeterRbcs_OK = false;
      pd.MakeRunReadable();

      return 1;
   }


   // if we're not doing MSS for RBC then get the heck out of here
   if (!(pd.RunTargets().MeterRbcs && pd.Run().MeterRbcs_OK))
   {
      return 1;
   }

   if (pd.Run().mssInPFR)  // if we're in PFR
   {
      // it this were we left off?
      if ((pd.Run().currentMSSSubState ==  SS_MSSPARALLELPROC )
          && ( pd.Run().currentRBCMSSSubState > SS_MSSRBCADDITION  ) )
         return 1;
   }


   if (transitionNow)
   {

      // if we arnt doing a PLT mss then we can send the completed message
      // else it will come out of the mssPLTAdittion's transitionStstaus()
      if ( !(pd.RunTargets().MeterPlatelets && pd.Run().MeterPlatelets_OK)
           || pd.Run().PltMeteringDone )
      {
         // Send a message to GUI to tell it that we have completed SS addition.
         MSSCompletionMsg _completeMeteredStorage(MessageBase::SEND_LOCAL);
         _completeMeteredStorage.send(END_MSS_MESSAGE_SENDER_RAS);
      }

      DataLog(log_level_proc_info) << "Storage Solution Delivered (filter+prod) =>"  << pd.Volumes().VstorageRBCBag  << endmsg;
      if (pd.Run().mssDrbcSplitDone.Get())
      {

         if (thisIsPartialDRBC)
         {
            DataLog(log_level_proc_info) << "Storage Solution to RBC Product Bag =>"  << pd.Volumes().VstorageRBCBag - CobeConfig::data().MssPtfVolume * 1.0f + CobeConfig::data().mssPtfFilterLoss << endmsg;
         }
         else
         {
            DataLog(log_level_proc_info) << "Storage Solution to RBC Product Bag =>"  << pd.Volumes().VstorageRBCBag - CobeConfig::data().MssPtfVolume * 2.0f + CobeConfig::data().mssPtfFilterLoss * 2.0f << endmsg;
         }

      }
      else
      {
         DataLog(log_level_proc_info) << "Storage Solution to RBC Product Bag =>"  << pd.Volumes().VstorageRBCBag - CobeConfig::data().MssPtfVolume + CobeConfig::data().mssPtfFilterLoss  << endmsg;
      }

      DataLog(log_level_proc_info) << "Storage Solution Remaining in Bag =>"  << pd.Volumes().VrbcStorageRemaining << endmsg;
      DataLog(log_level_proc_info) << "SS Volm Pumped(ac pump) =>"  << pd.Volumes().VacTotal - initialACVolume << endmsg;
      DataLog(log_level_proc_info) << "Completed all MSSRBCAddition"  << endmsg;

      // signal that we've passed the gate
      pd.MakeRunWritable();
      pd.Run().RBCMeteringDone = true;
      pd.MakeRunReadable();

      return 1;
   }

   return NO_TRANSITION;

}

int MSSRBCAddition::preEnter ()
{
   ProcData pd;

   if ( (pd.RunTargets().MeterRbcs && pd.Run().MeterRbcs_OK) )
   {
      // Send msg to GUI to let tell it we have entered SS Addition.
      MSSRASAdditionMsg _addMeteredStorage(MessageBase::SEND_GLOBAL);
      _addMeteredStorage.send(0);

      pd.Orders().ValveCmd(HAL_VRBC,  HAL_VALVE_COLLECT);

   }

   // if we're in a salvage case we dont allow any continuable APS alerts
   // so... max out the alert instances to make the next on an alarm.
   if (!pd.Run().rinsebackVolumeSuccessMss.Get())
   {
      apsInstanceCount        = MAX_NUMBER_OF_APS_ALERTS;
      apsInstanceCount_double = MAX_NUMBER_OF_APS_ALERTS;
   }

/////////////////////////////////////////////////////////////////////////////////////
// OK Figure out a plan to meter to this RBC product
   float collectedDose               =  pd.Volumes().RBCDose.Get();
   bool  isBlackStamp                =  (pd.TrimaSet().Cassette == cassette::RBCPLS);
   bool  isWhiteStamp                =  (pd.TrimaSet().Cassette == cassette::PLTPLSRBC);
   int   totalsolutionToBeMeteredOut = (int)pd.RunTargets().RBCSolutionTarget.Get();
   float prod_1_Volume               =  pd.Volumes().RBCProduct_A_FinalVolume.Get();
   float prod_2_Volume               =  pd.Volumes().RBCProduct_B_FinalVolume.Get();
   bool  prod_1_invalid              =  pd.Run().PTFRBCprod1Disabled.Get();
   bool  prod_2_invalid              =  pd.Run().PTFRBCprod2Disabled.Get();

   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   bool doubleTargeted = pd.Config().isPromptedDrbcSplit(pd.RunTargets().ProcNumber.Get());

   DataLog(log_level_proc_info) << "collectedDose -> " << collectedDose << endmsg;
   DataLog(log_level_proc_info) << "isBlackStamp ->" << isBlackStamp << endmsg;
   DataLog(log_level_proc_info) << "totalsolutionToBeMeteredOut -> " << totalsolutionToBeMeteredOut << endmsg;
   DataLog(log_level_proc_info) << "prod_1_Volume -> " << prod_1_Volume << endmsg;
   DataLog(log_level_proc_info) << "prod_2_Volume -> " << prod_2_Volume << endmsg;
   DataLog(log_level_proc_info) << "doubleTargeted -> " << doubleTargeted << endmsg;
   DataLog(log_level_proc_info) << "RBC Split Done -> " << pd.Run().drbcSplitDone.Get() << endmsg;
   DataLog(log_level_proc_info) << "MSS RBC Split Done -> " << pd.Run().mssDrbcSplitDone.Get() << endmsg;
   DataLog(log_level_proc_info) << "Metered to product A so far is -> " << pd.Volumes().RBCProduct_A_FinalVss.Get() << endmsg;
   DataLog(log_level_proc_info) << "Metered to product B so far is -> " << pd.Volumes().RBCProduct_B_FinalVss.Get() << endmsg;
   DataLog(log_level_proc_info) << "Metered total so far is -> " << pd.Volumes().VstorageRBCBag.Get() << endmsg;


   if (isBlackStamp)
   {
      DataLog(log_level_proc_info) << "A black stamp set" << endmsg;

//////// **** BLACK DOUBLE RBC ***** ///////////////////////////////////////////////////
      if ( doubleTargeted )
      {
         DataLog(log_level_proc_info) << "targets indicates a planned double"
                                      << endmsg;

         // assume a standard double metering
         thisIsPartialDRBC = false;
         thisIsDRBC        = true;

         // check if problems with product 2
         if ((prod_2_Volume <= CobeConfig::data().MssPtfVolume) || prod_2_invalid)
         {

            if (prod_2_Volume <= CobeConfig::data().MssPtfVolume)
               DataLog(log_level_proc_info) << "Product two is disabled for metering; because its volume is less than the filter volume" << endmsg;
            else
               DataLog(log_level_proc_info) << "Product two is disabled for metering; because of CPS alarms. Prod 2's volm ->" << pd.Volumes().RBCProduct_B_FinalVolume << endmsg;

            // if we did a split then undo it
            if (pd.Run().drbcSplitDone.Get()
                && !(pd.Run().mssDrbcSplitDone.Get()))
            {
               DataLog(log_level_proc_info) << "Notify op. of split due to Prod 2 being CPS alarmed out or zero volume. Prod 2's volm ->" << pd.Volumes().RBCProduct_B_FinalVolume << endmsg;
               drbcSplit.setAlarm();

               pd.MakeRunWritable();
               pd.Run().mssDrbcSplitDone.Set(true);
               pd.MakeRunReadable();
            }

            thisIsDRBC        = false;
            thisIsPartialDRBC = true;

         }

         // check if problems with product 1
         if ( (prod_1_Volume <= CobeConfig::data().MssPtfVolume) || prod_1_invalid)
         {
            if (prod_1_Volume <= CobeConfig::data().MssPtfVolume)
               DataLog(log_level_proc_info) << "Product one is disabled for metering; because its volume is less than the filter volume" << endmsg;
            else
               DataLog(log_level_proc_info) << "Product one is disabled for metering; because of CPS alarms. Prod 1's volm ->" << pd.Volumes().RBCProduct_A_FinalVolume << endmsg;


            thisIsDRBC        = false;
            thisIsPartialDRBC = true;

         }

//////// **** BLACK SPECIAL CORRUPTED DOUBLE RBC ***** ///////////////////////////////////////////////////
         // special case where it was a double and re-predicted to a single like product due to CPS repredict
         // on the first product
      }
      else if ( ( (prod_1_Volume> 0.0f) && prod_1_invalid )
                && (prod_2_Volume > 0.0f) )
      {


         DataLog(log_level_proc_info) << "This was a planned double but CPS alarms repredicted to smaller (treat as partial double)"
                                      << endmsg;

         DataLog(log_level_proc_info) << "Product one is disabled for metering; Prod 1's volm ->" << pd.Volumes().RBCProduct_A_FinalVolume << endmsg;
         thisIsDRBC        = false;
         thisIsPartialDRBC = true;




//////// **** BLACK SINGLE RBC ***** ///////////////////////////////////////////////////
      }
      else       /* only a single */

      {
         DataLog(log_level_proc_info) << "targets indicates a planned single"
                                      << endmsg;


         thisIsDRBC        = false;
         thisIsPartialDRBC = false;

         // considered Split for natural single processing
         pd.MakeRunWritable();
         pd.Run().mssDrbcSplitDone.Set(true);
         pd.MakeRunReadable();
      }

//////// **** WHITE STAMP SINGLE RBC ***** ///////////////////////////////////////////////////
   }
   else       /* isWhiteStamp */

   {
      DataLog(log_level_proc_info) << "A White stamp set" << endmsg;

      DataLog(log_level_proc_info) << "targets indicates a planned single"
                                   << endmsg;

      thisIsDRBC        = false;
      thisIsPartialDRBC = false;

      // considered Split for natural single processing
      pd.MakeRunWritable();
      pd.Run().mssDrbcSplitDone.Set(true);
      pd.MakeRunReadable();
   }
/////////////////////////////////////////////////////////////////////////////////////////////////

   // If DRBC then we only send 1/2 of the requested solution...
   //  next time through this state we do the other 1/2
   if (thisIsDRBC)
   {
      solutionToBeMeteredOut = totalsolutionToBeMeteredOut * 0.5;
      DataLog(log_level_proc_info) << "DRBC. SS Solution this run (half ss to each product) -> " << solutionToBeMeteredOut << endmsg;

   }
   else if (thisIsPartialDRBC)
   {

      solutionToBeMeteredOut = totalsolutionToBeMeteredOut * 0.5;
      DataLog(log_level_proc_info) << "Partial DRBC. SS Solution this run (half ss to surviving product) -> " << solutionToBeMeteredOut << endmsg;

   }
   else
   {
      solutionToBeMeteredOut = totalsolutionToBeMeteredOut;
      DataLog(log_level_proc_info) << "Single RBC. SS Solution this run (all ss to product)-> " << solutionToBeMeteredOut << endmsg;
   }

   // add in the PTF volume lost....
   solutionToBeMeteredOut = solutionToBeMeteredOut + CobeConfig::data().MssPtfVolume - CobeConfig::data().mssPtfFilterLoss;
   DataLog(log_level_proc_info) << "SSolution this run + PTF loss " << solutionToBeMeteredOut << endmsg;

   // get the starting point  of the collect pumps volume
   initialACVolume = pd.Volumes().VacTotal;

   ///////////////////// PFR /////////////////////////////////////////////////////////
   if (!pd.Run().mssInPFR)   // if we're NOT  in PFR
   {
      pd.MakeRunWritable();
      // setup this for PFR
      pd.Run().currentRBCMSSSubState.Set(SS_MSSRBCADDITION);
      pd.MakeRunReadable();

      pd.MakeVolumesWritable();
      pd.Volumes().RBCProduct_B_FinalVss = 0.0f;     // for end run sum... here is prod A's SS
      pd.Volumes().RBCProduct_A_FinalVss = 0.0f;     // for end run sum... here is prod B's SS
      pd.MakeVolumesReadable();

   }
   else
   {
      DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR" << endmsg;

      // i can reuse the speed reduction number as the number of aps alarms too!
      if (pd.Run().mssDrbcSplitDone.Get())
      {
         apsInstanceCount_double = pd.Run().RASAdditionSpeedReductionFactor.Get();
         DataLog(log_level_proc_info) << "Back to PFR point we had " << apsInstanceCount_double << " APS alarms" << endmsg;
      }
      else
      {
         apsInstanceCount = pd.Run().RASAdditionSpeedReductionFactor.Get();
         DataLog(log_level_proc_info) << "Back to PFR point we had " << apsInstanceCount << " APS alarms" << endmsg;
      }

      pd.MakeRunWritable();
      pd.Run().mssInPFR.Set(false);
      pd.MakeRunReadable();
   }
   ////////////////////////////////////////////////////////////////////////////////////

   return NORMAL;
}

int MSSRBCAddition::postExit ()
{

   ProcData pd;

   // set to false to
   transitionNow = false;

   pd.Orders().Qac (0.0f);
   pd.Orders().Qin (0.0f);

   // close the RBC valve
   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);

   DataLog(log_level_proc_info) << "Total Storage Solution used (filter+to_product) =>" << pd.Volumes().VstorageRBCBag << endmsg;

   return NORMAL;
}

void MSSRBCAddition::copyOver (const MSSRBCAddition&)
{}

void MSSRBCAddition::cleanup ()
{}


float MSSRBCAddition::calculatePumpSpeed (float v)
{

   ProcData pd;

   // double check constraint
   if (v < 0.0)
   {
      v = 0.0;
   }

   // this is the speed if no PTF... if we ever go back to that.
   // CobeConfig::data().MssRbcSubstateAddAcSpeed
   float speedReductionFactor = pd.Run().RASAdditionSpeedReductionFactor.Get();

   float speed                = 0.00004 * v * v * v * v
                                - 0.0036 * v * v * v
                                + 0.0969 * v * v
                                + 0.7617 * v
                                + 10.0;


   if (speed > 70.0)
   {
      speed = 70.0;
   }


   // reduce speed by 10% each APS alarm.
   if ( speedReductionFactor > 0 )
   {
      speed = speed - (speed * (float)speedReductionFactor * 0.1f);
      // DataLog(log_level_proc_info) << "Reduced RBC MSS addition speed to => "  << speed << " because of " << speedReductionFactor << " APS alert"  << endmsg;
   }


   return speed;
}

/* FORMAT HASH 4ccc065079210702f45ea95986455b02 */

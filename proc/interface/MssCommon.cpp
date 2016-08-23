#include <vxworks.h>
#include "procdata.h"
#include "MssCommon.h"
#include "CatalogChecker.h"
#include "cobeconfig.h"



bool readyForPLTMSS (bool ignoreAlarms)
{

   ProcData    pd;
   static bool logged = false;


   bool check =  (
      (CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::STAMP) == 1)
      && (pd.RunTargets().MeterPlatelets.Get())
      && (pd.RunTargets().PltSolutionTarget.Get() >= 1.0f)
      && (pd.Volumes().PlateletVolume.Get() >= CobeConfig::data().MssMinPltVolume )
      && (pd.Run().FirstCycleComplete.Get())
      && (pd.Volumes().PlateletVolume.Get() + pd.RunTargets().PltSolutionTarget.Get() <= CobeConfig::data().MaxPlateletBagVolume)
      && (((!pd.Run().cancelMSS.Get()) && (!pd.Run().cancelJustPas.Get())) || ignoreAlarms)
      );



   // if our concetration blows up our metering... then cancle PAS
   float pasNeeded = getAdjustedPAS(true /*just a peek dont set*/);
   if ( pasNeeded < 0.0f )
   {
      static bool logIt = true;
      if (logIt)
      {
         logIt = false;
         DataLog(log_level_proc_mss_info) << "Check Fails For PLT MSS because Adjustment calculation could not be made." << endmsg;
      }
      pd.MakeRunWritable();
      pd.Run().flagMSSPASConcetrationError.Set(true);
      pd.Run().flagMSSPASVolumeError.Set(true);
      check = false;
   }


   if ( !check )
   {
      if (!logged)
      {
         logged = true;

         // show the laundry list of reasons......
         DataLog(log_level_proc_mss_info) << "Check Fails For PLT MSS because:" << endmsg;

         if (CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::STAMP) == 0)
            DataLog(log_level_proc_mss_info) << "PLT Metered Storage skipped this is a BLACK stamp set" << endmsg;

         if (!pd.RunTargets().MeterPlatelets)
            DataLog(log_level_proc_mss_info) << "PLT Metered Storage skipped because it's turned off for this proc" << endmsg;

         if (pd.RunTargets().PltSolutionTarget.Get() < 1.0f)
            DataLog(log_level_proc_mss_info) << "PLT Metered Storage skipped because PltSolutionTarget is < 1.0mL" << endmsg;

         if (pd.Volumes().PlateletVolume.Get() < CobeConfig::data().MssMinPltVolume)
            DataLog(log_level_proc_mss_info) << "PLT Metered Storage skipped because PlateletVolume is not at least " << CobeConfig::data().MssMinPltVolume << "mL (" << pd.Volumes().PlateletVolume.Get() << "ml)" << endmsg;

         if (!pd.Run().FirstCycleComplete.Get())
            DataLog(log_level_proc_mss_info) << "PLT Metered Storage skipped because First cycle never completed" << endmsg;

         if (pd.Run().cancelMSS.Get() && !ignoreAlarms )
            DataLog(log_level_proc_mss_info) << "Metered Storage set to be cancelled during run (usually an A2)" << endmsg;

         if (pd.Volumes().PlateletVolume.Get() + pd.RunTargets().PltSolutionTarget.Get() > CobeConfig::data().MaxPlateletBagVolume )
            DataLog(log_level_proc_mss_info) << "Metered Storage Volume plus Plt Volume exceed bag capacity!" << endmsg;

         if (pd.Run().cancelJustPas.Get() && !ignoreAlarms )
            DataLog(log_level_proc_mss_info) << "PLT Metered Storage cancelled by operator" << endmsg;

         if (pasNeeded < 0.0f)
            DataLog(log_level_proc_mss_info) << "Check Fails For PLT MSS because:  PAS concetration setting PAS volm/process error flags " << endmsg;
      }

   }

   return check;
}


bool readyForRBCMSS (bool ignoreAlarms)
{

   ProcData    pd;
   static bool logged = false;

   bool        check  =  (
      ( pd.RunTargets().MeterRbcs.Get() )
      && ( pd.RunTargets().RBCSolutionTarget.Get() > 0.0 )
      && (
         ( pd.Volumes().RBCProduct_A_FinalVolume.Get() > CobeConfig::data().MssPtfVolume )
         ||
         ( pd.Volumes().RBCProduct_B_FinalVolume.Get() > CobeConfig::data().MssPtfVolume )
         )
      && ( pd.Run().FirstCycleComplete.Get()  )
      && (((!pd.Run().cancelMSS.Get()) && (!pd.Run().cancelJustRas.Get())) || ignoreAlarms)
      );

   if ( !check )
   {


      if (!logged)
      {
         logged = true;
         // show the laundry list of reasons...
         DataLog(log_level_proc_mss_info) << "Check Fails For RBC MSS because:" << endmsg;

         if (!pd.RunTargets().MeterRbcs)
            DataLog(log_level_proc_mss_info) << "RBC Metered Storage skipped because it's turned off for this proc" << endmsg;

         if (pd.RunTargets().RBCSolutionTarget.Get() <= 0.0f)
            DataLog(log_level_proc_mss_info) << "RBC Metered Storage skipped because RBCSolutionTarget is Zero or less" << endmsg;

         if (     ( pd.Volumes().RBCProduct_A_FinalVolume.Get() <= CobeConfig::data().MssPtfVolume)
                  &&  ( pd.Volumes().RBCProduct_B_FinalVolume.Get() <= CobeConfig::data().MssPtfVolume)
                  )
            DataLog(log_level_proc_mss_info) << "RBC Metered Storage skipped because RBC Volume(s) is less than the PTF volume" << endmsg;

         if (!pd.Run().FirstCycleComplete.Get())
            DataLog(log_level_proc_mss_info) << "RBC Metered Storage skipped because First cycle never completed" << endmsg;

         if (pd.Run().cancelMSS.Get() && !ignoreAlarms )
            DataLog(log_level_proc_mss_info) << "Metered Storage set to be cancelled during run (usually an A2)" << endmsg;

         if (pd.Run().cancelJustRas.Get() && !ignoreAlarms )
            DataLog(log_level_proc_mss_info) << "RBC Metered Storage cancelled by operator" << endmsg;
      }
   }

   return check;
}


float getAdjustedPAS (bool peekOnly)
{

   ProcData    pd;

   const float ppco             = pd.RunTargets().PltPctCarryover.Get();
   const float TargetYield      = pd.RunTargets().PlateletYieldTarget.Get();
   const float ActualYield      = (pd.Volumes().PlateletYield.Get() / 100000000000.0);
   const float TargetConc       = pd.RunTargets().PlateletConcenTarget.Get();
   const float TargetVolm       = pd.RunTargets().PlateletVolumeTarget.Get();
   const float ActualVolm       = pd.Volumes().PlateletVolume.Get();
   const float originalSS       = pd.RunTargets().PltSolutionTarget.Get();
   const float originalPltVolm  = pd.RunTargets().OriginalVolume.Get();
   const float originalPltYeild = pd.RunTargets().OriginalYeild.Get();



   float AdjustedPAS = -1.0;

   // no PLT no PAS    ( and dont clutter the log with stuff that we wont do)
   if (ActualVolm == 0.0f)
      return 0.0f;

   if (!peekOnly)
   {
      DataLog(log_level_proc_mss_info) << " .... Adjusting PAS to maintain concentration ...." << endmsg;
      DataLog(log_level_proc_mss_info) << "Plasma % Carryover  ->" << ppco << endmsg;
      DataLog(log_level_proc_mss_info) << "current Targeted Yield      ->" << TargetYield << endmsg;
      DataLog(log_level_proc_mss_info) << "current Target Concentration->" << TargetConc << endmsg;
      DataLog(log_level_proc_mss_info) << "current Target Collect Volm ->" << TargetVolm << endmsg;
      DataLog(log_level_proc_mss_info) << "Actual Collect volm ->" <<  ActualVolm << endmsg;
      DataLog(log_level_proc_mss_info) << "Actual Yield        ->" <<  ActualYield << endmsg;
      DataLog(log_level_proc_mss_info) << "Original SS        ->" <<  originalSS << endmsg;
      DataLog(log_level_proc_mss_info) << "Original Plt Volm  ->" <<  originalPltVolm << endmsg;
      DataLog(log_level_proc_mss_info) << "Original Plt Yeild ->" <<  originalPltYeild << endmsg;
   }


   ///// 1) check for pre-condition error
   float oldSSVol = pd.RunTargets().PltSolutionTarget.Get();
   if (oldSSVol <= 1.0f)
   {
      oldSSVol = 1.0f;       // protect  agains divide by zeros
   }

   if (TargetConc < 1.0)
   {

      if (!peekOnly) DataLog(log_level_proc_info) << "Using original product values to calc. Concentration " << TargetConc << endmsg;

      // this in if you switch to a non-created product. i.e you select a product and end collection

      ///// 2) what Did we originally target for SS
      if (!peekOnly) DataLog(log_level_proc_info) << "Original Targeted SS -> " << originalSS  << endmsg;

      // check for overflow
      if ( (originalPltVolm + originalSS) == 0.0f || originalPltYeild == 0.0f)
      {
         // this is an error
         DataLog(log_level_proc_error) << "PAS adjustment calc data contains zeros ERROR.  Original Plt Volm ->" << originalPltVolm
                                       <<     "Original Plt Yeild ->" <<  originalPltYeild
                                       <<     "Original SS ->" <<  originalSS << endmsg;

         pd.MakeRunWritable();
         pd.Run().PltSSVolumeFactor.Set( ( -1.0f ) );     // put the recalc factor here
         if (!peekOnly) DataLog(log_level_proc_error) << "Error, the Adjstment Factor  -> " <<  -1.0f  << endmsg;
         return -1.0f;

      }
      else         // all the data is present and accounted for:


      {     ///// 3) deternmine Original Final Concentration
            // using the last valid product
         float OriginalFinalConcentration =  ( originalPltYeild / (originalPltVolm + originalSS) ) * 100000;
         if (!peekOnly) DataLog(log_level_proc_info) << "Original Final Concentration -> " << OriginalFinalConcentration  << endmsg;

         ///// 4) Determine new SS to keep the concetration stable
         AdjustedPAS =  (( ActualYield / (OriginalFinalConcentration) ) * 100000) - ActualVolm;
         if (!peekOnly) DataLog(log_level_proc_info) << "Adjusted PAS volm -> " << AdjustedPAS  << endmsg;

         ///// 5) set the volume factor that effects all SS volm
         if (!peekOnly)
         {
            pd.MakeRunWritable();
            // just get a multiplier of the ratio of collected/targeted
            pd.Run().PltSSVolumeFactor.Set( ( AdjustedPAS / oldSSVol ) );      // put the recalc factor here

            if (!peekOnly) DataLog(log_level_proc_info) << "the Factor  -> " <<  pd.Run().PltSSVolumeFactor.Get()  << endmsg;
         }

      }




   }
   else
   {

      // this is for switching to or keeping an offered product.
      if (!peekOnly) DataLog(log_level_proc_info) << "Using current target product values to calc. Concentration " << TargetConc << endmsg;

      ///// 2) what Did we originally target for SS
      float plannedSSvolume =  ( TargetVolm * ((100.0 - ppco) / 100.0) ) / (ppco / 100.0);
      if (!peekOnly) DataLog(log_level_proc_info) << "Original Targeted SS -> " << plannedSSvolume  << endmsg;



      // check for overflow
      if ( (TargetVolm + plannedSSvolume) == 0.0f || TargetYield == 0.0f)
      {
         // this is an error
         DataLog(log_level_proc_error) << "PAS adjustment calc data contains zeros ERROR.  TargetVolm ->" << TargetVolm
                                       <<     "TargetYield ->" <<  TargetYield
                                       <<     "plannedSSvolume ->" <<  plannedSSvolume << endmsg;

         pd.MakeRunWritable();
         pd.Run().PltSSVolumeFactor.Set(-1.0f);       // put the recalc factor here
         if (!peekOnly) DataLog(log_level_proc_error) << "Error, the Adjstment Factor  -> " <<  -1.0f  << endmsg;
         return -1.0f;

      }
      else         // all the data is present and accounted for:


      {     ///// 3) deternmine Original Final Concentration
            // using the current products targets
         float OriginalFinalConcentration =  ( TargetYield / (TargetVolm + plannedSSvolume) ) * 100000;
         if (!peekOnly) DataLog(log_level_proc_info) << "Original Final Concentration -> " << OriginalFinalConcentration  << endmsg;

         ///// 4) Determine new SS to keep the concetration stable
         AdjustedPAS =  (( ActualYield / (OriginalFinalConcentration) ) * 100000) - ActualVolm;
         if (!peekOnly) DataLog(log_level_proc_info) << "Adjusted PAS volm -> " << AdjustedPAS  << endmsg;


         if (!peekOnly)
         {
            ///// 5) set the volume factor that effects all SS volm
            pd.MakeRunWritable();
            // just get a multiplier of the ratio of collected/targeted
            pd.Run().PltSSVolumeFactor.Set( ( AdjustedPAS / oldSSVol ) );      // put the recalc factor here

            if (!peekOnly) DataLog(log_level_proc_info) << "the Factor  -> " <<  pd.Run().PltSSVolumeFactor.Get()  << endmsg;
         }

      }

   }

   return AdjustedPAS;


}

/////////////////////////////////////////////////////////////////////////////////////////////

bool RASBlockedDueToCPSalarms ()
{

   ProcData pd;

   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   bool doubleTargeted =
      pd.Config().isPromptedDrbcSplit(pd.RunTargets().ProcNumber.Get()) ||
      pd.Run().drbcSplitDone.Get();

   float prod_1_Volume  =  pd.Volumes().RBCProduct_A_FinalVolume.Get();
   float prod_2_Volume  =  pd.Volumes().RBCProduct_B_FinalVolume.Get();
   bool  prod_1_invalid =  pd.Run().PTFRBCprod1Disabled.Get();
   bool  prod_2_invalid =  pd.Run().PTFRBCprod2Disabled.Get();

   bool  Return_status  = false;

   if (pd.TrimaSet().Cassette == cassette::RBCPLS)
   {
      if (doubleTargeted)
      {

         if   ( prod_1_invalid && prod_2_invalid )
         {

            Return_status =   true;
         }


         if (    ( (prod_1_Volume > 0.0f) && prod_1_invalid  )
                 &&( (prod_2_Volume > 0.0f) && !prod_2_invalid ) )
         {

            Return_status =   false;
         }

         if ( ( (prod_1_Volume > 0.0f) && prod_1_invalid )
              && (prod_2_Volume < 1.0f) )
         {

            Return_status =   true;
         }


      }
      else
      {

         if (    ( (prod_1_Volume > 0.0f) && prod_1_invalid  )
                 &&( (prod_2_Volume > 0.0f) && !prod_2_invalid ) )
         {

            Return_status =   false;
         }

         if ( ( (prod_1_Volume > 0.0f) && prod_1_invalid )
              && (prod_2_Volume < 1.0f) )
         {

            Return_status =  true;
         }

         if ( prod_1_invalid  )
         {

            Return_status = true;
         }
      }

   }
   else        // white stamp....


   {
      Return_status =  prod_1_invalid ;
   }

   if (Return_status)
   {

      if (pd.TrimaSet().Cassette == cassette::RBCPLS)
      {

         DataLog(log_level_proc_info) << "RASBlockedDueToCPSalarms :: "
                                      << " prod_1_Volume = "     <<  prod_1_Volume   << " "
                                      << " prod_2_Volume = "     <<  prod_2_Volume   << " "
                                      << " prod_1_invalid = "     << prod_1_invalid    << " "
                                      << " prod_2_invalid = "     << prod_2_invalid    << " "
                                      << " doubleTargeted = "     << doubleTargeted    << " "
                                      << " Return_status = "     <<  Return_status
                                      << endmsg;
      }
      else        // white stamp...

      {
         DataLog(log_level_proc_info) << "RASBlockedDueToCPSalarms :: "
                                      << " prod_1_Volume = "     <<  prod_1_Volume   << " "
                                      << " prod_1_invalid = "     << prod_1_invalid    << " "
                                      << " Return_status = "     <<  Return_status
                                      << endmsg;

      }
   }



   return Return_status;  // if you made it thru the gauntlet i guess your ok....
}


/////////////////////////////////////////////////////////////////////////////////////////////

int GuiAasUpdater::_lastAasUpdateValue = 0;
int GuiAasUpdater::_loggingCount       = 0;

void GuiAasUpdater::sendUpdate ()
{
   ProcData pd;
   int      pasVol      = (int)pd.Volumes().VstoragePltBag;
   int      rasVol      = (int)pd.Volumes().VstorageRBCBag;
   int      totalAasVol = pasVol + rasVol;

   if (_lastAasUpdateValue != totalAasVol)
   {
      if (!(_loggingCount % 10)) DataLog(log_level_proc_info) << "Sending AasAdditionVolumeUpdate from Proc to Gui to update the bar. Total Aas Vol = " << totalAasVol << endmsg;
      _loggingCount++;

      _lastAasUpdateValue = totalAasVol;

      AasAdditionVolumeUpdate updateMsg(MessageBase::SEND_GLOBAL);
      updateMsg.send(totalAasVol);
   }
}

/* FORMAT HASH 9e3b683f99dbd53467f58308e5401083 */

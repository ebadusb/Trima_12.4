/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:       meteredStorage.cpp
 *
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/meteredStorage.cpp 1.39 2009/04/20 20:16:58Z dslausb Exp jheiusb $
$Log: meteredStorage.cpp $
Revision 1.39  2009/04/20 20:16:58Z  dslausb
IT 9171 - Change disconnect test functionality, text, and graphics
Revision 1.38  2009/04/13 22:39:17Z  dslausb
IT 9155 - Add "operator abort" reason code for MSS
Revision 1.37  2008/12/18 23:22:29Z  jheiusb
8988 -- add column log for OR PAS and RAS total
Revision 1.36  2008/07/29 15:03:42Z  dslausb
Make metering more robust to PFR
Revision 1.35  2008/07/21 18:42:39Z  jheiusb
8140 -- alarm  column  and time off data for MSS
Revision 1.34  2008/05/28 20:54:56Z  jheiusb
8077 -- update GUI on the Seperate endruns also.
Revision 1.33  2008/05/13 21:11:21Z  jheiusb
8077 -- implement separate endruns for PAS/RAS
Revision 1.32  2008/03/26 18:05:00Z  jheiusb
8521 -- clean up logging
Revision 1.31  2008/02/21 20:44:58Z  jheiusb
 -- removed PTFRbcProduct flag as redundant to MSSRbc flag
Revision 1.30  2007/11/29 23:43:15Z  jheiusb
8360 -- fix metering summary TotalAC value reported
Revision 1.29  2007/11/27 18:31:24Z  jheiusb
8257 -- connect RBC bags in relation to MSS volume needed
Revision 1.28  2007/10/26 16:54:12Z  jheiusb
8082 -- PTF CPS alarms end product based
Revision 1.27  2007/10/08 16:17:31Z  jheiusb
7346 -- accound for two bags spiked on back stamp
Revision 1.26  2007/08/15 16:52:43Z  jheiusb
7930 -- disable RAS if rinseback failes to complete
Revision 1.25  2007/04/02 20:21:03Z  jheiusb
7579 -- patch up PFR for 5.R
Revision 1.24  2007/03/06 21:56:05Z  jheiusb
7892 -- keep door locked after alarm
Revision 1.23  2007/01/22 22:42:21Z  jheiusb
7843 -- add metering time column to dlogs
Revision 1.22  2007/01/17 21:09:17Z  jheiusb
7291 -- add RBC metering setups
Revision 1.21  2006/12/04 21:42:31Z  jheiusb
Revision 1.20  2006/12/04 16:16:17Z  jheiusb
remerge from original rbc+plt mss 5.2
Revision 1.19  2006/11/09 06:09:49Z  jheiusb
7670 -- rbc collection in salvage case is wbc flag
Revision 1.18  2006/09/28 20:20:28Z  jheiusb
7707 -- add rinseback monitor for MSS
Revision 1.17  2006/07/26 22:37:47Z  jheiusb
7672 -- add MSS rinseback started flag
Revision 1.16  2006/07/21 16:23:04Z  jheiusb
7662 -- remove alarm
Revision 1.15  2006/06/09 16:55:26Z  dslausb
Removed fixed volume stuff and remnants of cassette options from config
Revision 1.14  2006/06/05 14:20:05Z  rm70006
IT 7510.  Remove CassetteOptions struct from config and move MSS fields to Procedure.
Revision 1.13  2006/05/19 19:02:24Z  jheiusb
7549 -- fix leak detecter alarm
Revision 1.12  2006/05/15 22:34:38Z  jheiusb
7272 -- add catalog number chech
Revision 1.11  2006/05/08 21:56:41Z  jheiusb
7514 -- fix addition during PFR;
Revision 1.10  2006/04/27 20:31:41Z  jheiusb
7468 -- change % carryover to product based
Revision 1.9  2006/04/19 22:08:08Z  jheiusb
7491 -- PFR hangs
Revision 1.8  2006/04/12 19:50:05Z  jheiusb
cleanup logging
Revision 1.7  2006/04/11 22:30:04Z  jheiusb
7424 -- fix default bag volm override
Revision 1.6  2006/04/10 21:17:34Z  jheiusb
7424 - fix the bag empty alarm
Revision 1.5  2006/03/27 17:51:50Z  jheiusb
7350 -- PFR for MSS
Revision 1.4  2006/03/15 17:03:17Z  jheiusb
Revision 1.3  2006/03/03 21:22:01Z  jheiusb
Revision 1.2  2006/03/03 17:31:13Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:16:11Z  jheiusb
Initial revision
Revision 1.12  2006/01/04 21:06:56Z  jheiusb
7330 -- fix ACTotal to not include the SS used during mss
Revision 1.11  2005/12/29 18:23:25Z  jheiusb
7328 -- fix bad master switch combo.
Revision 1.10  2005/12/19 23:25:24Z  dslausb
IT 7301 - Fixed override default bag volume so that it actually writes the override value to CDS like it's supposed to.
Revision 1.9  2005/12/13 18:12:38Z  jheiusb
7265 -- make metered storage a full state
Revision 1.8  2005/11/16 19:47:20Z  jheiusb
7230 -- Alarm Processing to split RBC and PLT runs after one fatal errors
Revision 1.7  2005/10/25 20:57:27Z  jheiusb
7202 -- initial PLT mss path
Revision 1.6  2005/10/21 18:46:48Z  rm70006
IT 7202.  Add proc update to state.
Revision 1.5  2005/10/11 21:17:50Z  jheiusb
7202 -- channel check code
Revision 1.4  2005/10/07 22:06:10Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.3  2005/10/05 20:34:54Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#include "meteredStorage.h"
#include "states.h"
#include "guiproc.h"
#include "procdata.h"


DEFINE_STATE(MeteredStorage);

MeteredStorage::MeteredStorage()
   : StateAbs(),
     initialACVolmTotal(0.0)
{}
///////////////////////////////////////////////////////////////////////////
MeteredStorage::MeteredStorage(const MeteredStorage& state)
   : StateAbs(),
     callbackSetUp(false),
     initialACVolmTotal(0.0)
{
   copyOver(state);
}
///////////////////////////////////////////////////////////////////////////
MeteredStorage::~MeteredStorage()
{
   cleanup();
}
///////////////////////////////////////////////////////////////////////////
int MeteredStorage::preProcess ()
{
   ProcData pd;

   if (!callbackSetUp)
   {
      callbackSetUp        = true;

      ptr_writeBagVolRBC   = new  MSSWriteBagVolRBC(Callback<MeteredStorage>(this, &MeteredStorage::overwriteSSBagVolRBC), MessageBase::SNDRCV_GLOBAL);
      ptr_writeBagVolPLT   = new  MSSWriteBagVolPLT(Callback<MeteredStorage>(this, &MeteredStorage::overwriteSSBagVolPLT), MessageBase::SNDRCV_GLOBAL);

      ptr_partialCancelMSS = new PartialCancellationMsg(Callback<MeteredStorage>(this, &MeteredStorage::partialMeteringCancel), MessageBase::SNDRCV_GLOBAL);

      DataLog(log_level_proc_mss_info) << "Set up callbacks for writing SS bag volumes to CDS." << endmsg;
   }

   pd.Update(ProcData::SOLUTION);

   return NORMAL;
}
///////////////////////////////////////////////////////////////////////////
int MeteredStorage::postProcess ()
{


   ProcData pd;

   if ( !pd.AlarmActive() )
   {

      pd.MSSRunTime().activate();  // start the metering run timer
      pd.Orders().LockDoor();

   }
   else     // ALARM IS ON

   {                                //
      pd.MSSRunTime().inactivate(); // stop the metering run timer

   }


   pd.MakeVolumesWritable();
   // keep running total on the total RAS & PAS used
   if ( pd.Run().currentMSSSubState.Get() >  SS_MSSCONNECT )
   {
      // tot up all used PAS
      pd.Volumes().TotalPASVolumePumped.Set(pd.Volumes().TotalPASVolumePumped.Get() + pd.Status().PlasmaPump.dV() );
      // decrement the PAS bag volm as it is used
      pd.Volumes().VpltStorageRemaining.Set(pd.Volumes().VpltStorageRemaining.Get() - pd.Status().PlasmaPump.dV() );


      // tot up all used RAS
      pd.Volumes().TotalRASVolumePumped.Set(pd.Volumes().TotalRASVolumePumped.Get() + pd.Status().ACPump.dV() );
      // decrement the RAS bag volm as it is used
      pd.Volumes().VrbcStorageRemaining.Set(pd.Volumes().VrbcStorageRemaining - pd.Status().ACPump.dV() );
   }



   return NORMAL;
}
///////////////////////////////////////////////////////////////////////////
// no transitionStatus function needed because this state runs its substates.

// int MeteredStorage :: transitionStatus()
// {
//     DataLog(log_level_proc_info) << "MeteredStorage Enter transitionStatus" << endmsg;
//     return NORMAL;  //
// }

///////////////////////////////////////////////////////////////////////////
int MeteredStorage::preEnter ()
{
   //
   // Set the system state to AC Prime
   //
   ProcData pd;
   pd.SystemState(METERED_STORAGE);
   pd.MSSRunTime().activate(); // start the metering run timer


   if (    (pd.Run().rinsebackEnteredMss.Get() == false)
           && (pd.Run().rinsebackVolumeSuccessMss.Get() == false) )

   {
      if (pd.PQI().RbcWereCollected.Get() == true)

      {
         // Special salvage case
         DataLog(log_level_proc_info) << "Rinseback NOT started & RBC were collected!" << endmsg;

         if (pd.RunTargets().MeterPlatelets.Get())
            DataLog(log_level_proc_info) << "PAS Salvage case Allowed with WBC Flagging" << endmsg;
         // flagging done in PostRun_Monitor

      }
      else
      {

         DataLog(log_level_proc_info) << "Rinseback NOT started & NOT returned enough volume, no RBCs collected" << endmsg;

         if (pd.RunTargets().MeterPlatelets.Get())
            DataLog(log_level_proc_info) << "PAS Salvage case allowed with NO WBC Flagging" << endmsg;
      }

      // IT8248 RAS Salvage case!
      if ( pd.RunTargets().MeterRbcs.Get())
         DataLog(log_level_proc_info) << "RAS Salvage case allowed" << endmsg;

      // disable MSS for RBC
      //  pd.MakeRunWritable();
      //  pd.Run().MeterRbcs_OK = false;
      //  pd.MakeRunReadable();


   }
   else if ( (pd.Run().rinsebackEnteredMss.Get() == true)
             && (pd.Run().rinsebackVolumeSuccessMss.Get() == false) )

   {
      DataLog(log_level_proc_info) << "Rinseback started & NOT returned enough volume" << endmsg;

      if (pd.RunTargets().MeterPlatelets.Get())
         DataLog(log_level_proc_info) << "PAS Salvage case allowed with WBC Flagging" << endmsg;
      // flagging done in PostRun_Monitor


      // IT8248 RAS Salvage case!
      if ( pd.RunTargets().MeterRbcs.Get())
         DataLog(log_level_proc_info) << "RAS Salvage case allowed" << endmsg;

      // disable MSS for RBC
      //  pd.MakeRunWritable();
      //  pd.Run().MeterRbcs_OK = false;
      //  pd.MakeRunReadable();


   }
   else if ( (pd.Run().rinsebackEnteredMss.Get() == true)
             && (pd.Run().rinsebackVolumeSuccessMss.Get() == true) )

   {
      // normal processing for MSS
      DataLog(log_level_proc_info) << "Rinseback started & returned enough volume" << endmsg;


      if (pd.RunTargets().MeterPlatelets.Get())
         DataLog(log_level_proc_info) << "PAS Nominal case Allowed" << endmsg;

      if ( pd.RunTargets().MeterRbcs.Get())
         DataLog(log_level_proc_info) << "RAS Nominal case Allowed" << endmsg;


   }

   if (!pd.Run().mssInPFR)  // if we're in PFR these should be set with the last known volumes.. so don't write over them
   {

      pd.MakeVolumesWritable();
      // save this at start of MSS so we can display the AC used (without the mss pumping)
      pd.Volumes().VacTotal_tmp.Set(pd.Volumes().VacTotal.Get() );

      bool isBlackStamp =  (pd.TrimaSet().Cassette == cassette::RBCPLS);
      ////////////////////////////////////////////////////////////////////
      // update the volumes_cds with known metered storage data....
      //

      // set the default storage solution bag volume
      // this may be updated again by operator during the MSSConnect GUI
      // but these are the default bag volm set in the config screens
      pd.Volumes().VpltBagCapacity.Set(pd.Config().Procedure.Get().key_plt_def_bag_vol);
      pd.Volumes().VrbcBagCapacity.Set(pd.Config().Procedure.Get().key_rbc_def_bag_vol);
      // zero out the volumes that need to be accrued during this procedure:

      // MOVED VrbcStorageRemaining setup to MssConnect for IT8257 but init to zero here:
      pd.Volumes().VpltStorageRemaining.Set(0.0f);
      pd.Volumes().VrbcStorageRemaining.Set(0.0f);

      // this is the SS that has been sent to the product bag
      pd.Volumes().VstoragePltBag.Set(0.0);
      pd.Volumes().VstorageRBCBag.Set(0.0);

      // if your looking for the amount that needs to be metered to the products
      // its in procruntarget_cds: RBCSolutionTarget & PltSolutionTarget
      // should be getting set in predict..... good luck
      pd.MakeVolumesReadable();


      pd.MakeRunWritable();
      // setup this for PFR
      pd.Run().currentMSSSubState = SS_MSSSETUP;
      pd.MakeRunReadable();
   }
   // mark the cassette as being used for MSS
   //  leak deteecter needs this set to storage to work...
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::STORAGE);
   pd.MakeTrimaSetReadable();


   // Dump some info
   DataLog(log_level_proc_info) <<    "Metered Storage parameters: " << endmsg;
   DataLog(log_level_proc_info) <<    "  Cassette Function code "    << pd.Run().CassetteFunctionBits.Get() << endmsg;

   if (pd.RunTargets().MeterPlatelets.Get())
   {
      DataLog(log_level_proc_info) <<    "  PLT Default SS Bag Volm        -> " << pd.Config().Procedure.Get().key_plt_def_bag_vol << endmsg;
      DataLog(log_level_proc_info) <<    "  PLT Solution to be Metered out -> " << pd.RunTargets().PltSolutionTarget << endmsg;
      DataLog(log_level_proc_info) <<    "  PLT Percent carryover ->" << pd.RunTargets().PltPctCarryover.Get() << endmsg;
      DataLog(log_level_proc_info) <<    "  PLT SS multiplier is currently -> " << pd.Run().PltSSVolumeFactor << endmsg;
      DataLog(log_level_proc_info) <<    "  PLT target  volm ->"  << pd.RunTargets().PlateletVolumeTarget << endmsg;
      DataLog(log_level_proc_info) <<    "  PLT collect volm ->"  << pd.Volumes().PlateletVolume << endmsg;
   }

   if ( pd.RunTargets().MeterRbcs.Get())
   {

      if ( !pd.Run().MeterRbcs_OK.Get() )
      {
         DataLog(log_level_proc_info) << "RAS Procedure is Forbidden, but the parameters were...." << endmsg;
      }
      if (pd.Run().PTFRBCprod1Disabled.Get())
      {
         DataLog(log_level_proc_info) << "RAS Procedure Blocked due to CPS alarms on Product 1, metering to 2 only" << endmsg;
      }
      if (pd.Run().PTFRBCprod2Disabled.Get())
      {
         DataLog(log_level_proc_info) << "RAS Procedure Blocked due to CPS alarms on Product 2, metering to 1 only" << endmsg;
      }

      DataLog(log_level_proc_info) <<    "  RBC target  volm ->"  << pd.RunTargets().RbcVolumeTarget << endmsg;
      DataLog(log_level_proc_info) <<    "  RBC collect volm ->"  << pd.Volumes().RBCVolume << endmsg;

      DataLog(log_level_proc_rinseback_info) << " --Prod A RBC Dose    ->" << pd.Volumes().RBCProduct_A_FinalDose.Get() << endmsg;
      DataLog(log_level_proc_rinseback_info) << " --Prod A RBC Volume  ->" << pd.Volumes().RBCProduct_A_FinalVolume.Get() << endmsg;
      DataLog(log_level_proc_rinseback_info) << " --Prod A RBC AC Volm ->" << pd.Volumes().RBCProduct_A_FinalVac.Get() << endmsg;
      DataLog(log_level_proc_rinseback_info) << " --Prod B RBC Dose    ->" << pd.Volumes().RBCProduct_B_FinalDose.Get() << endmsg;
      DataLog(log_level_proc_rinseback_info) << " --Prod B RBC Volume  ->" << pd.Volumes().RBCProduct_B_FinalVolume.Get() << endmsg;
      DataLog(log_level_proc_rinseback_info) << " --Prod B RBC AC Volm ->" << pd.Volumes().RBCProduct_B_FinalVac.Get() << endmsg;


      DataLog(log_level_proc_info) <<    "  RBC Default SS Bag Volm        -> " << pd.Config().Procedure.Get().key_rbc_def_bag_vol << endmsg;
      DataLog(log_level_proc_info) <<    "  RBC Solution to be Metered out -> " << pd.RunTargets().RBCSolutionTarget << endmsg;

   }





   return NORMAL;
}
///////////////////////////////////////////////////////////////////////////
int MeteredStorage::postExit ()
{
   ProcData pd;

   // restore the AC volume prior to MSS
   pd.MakeVolumesWritable();
   pd.Volumes().VacTotal.Set(pd.Volumes().VacTotal_tmp.Get() );
   pd.MakeVolumesReadable();


   return NORMAL;
}
///////////////////////////////////////////////////////////////////////////

void MeteredStorage::copyOver (const MeteredStorage&) {}

///////////////////////////////////////////////////////////////////////////

void MeteredStorage::cleanup () {}

///////////////////////////////////////////////////////////////////////////

void MeteredStorage::overwriteSSBagVolRBC (void* data)
{
   DataLog(log_level_proc_mss_info) << "Proc got the message to override the default RBC bag volume." << endmsg;
   ProcData pd;
   long     long_data = (long)ptr_writeBagVolRBC->getData();
   pd.MakeVolumesWritable();
   pd.Volumes().VrbcBagCapacity.Set(long_data);
   pd.MakeVolumesReadable();
   DataLog(log_level_proc_mss_info) << "Set default RBC MSS bag volume to " << long_data << endmsg;
}

void MeteredStorage::overwriteSSBagVolPLT (void* data)
{
   ProcData pd;
   long     long_data = (long)ptr_writeBagVolPLT->getData();
   pd.MakeVolumesWritable();
   pd.Volumes().VpltBagCapacity.Set(long_data);
   pd.Volumes().VpltStorageRemaining.Set(long_data);
   pd.MakeVolumesReadable();
   DataLog(log_level_proc_mss_info) << "Re-Set default PLT MSS bag volume to "  << pd.Volumes().VpltStorageRemaining << endmsg;
}


void MeteredStorage::partialMeteringCancel (void* data)
{
   ProcData             pd;
   PARTIAL_END_RUN_TYPE endType = (PARTIAL_END_RUN_TYPE)ptr_partialCancelMSS->getData();

   DataLog(log_level_proc_mss_info) << "partialMeteringCancel(): endType=" << endType << endmsg;

   switch (endType)
   {
      case  END_RAS_ADDITION :
      {
         MSS_USER_ABORT_TYPE newAbortType = MSS_USER_ABORT_TYPE_RAS;

         if (pd.Run().MssUserAbortType.Get() == MSS_USER_ABORT_TYPE_ALL ||
             pd.Run().MssUserAbortType.Get() == MSS_USER_ABORT_TYPE_PAS)
         {
            newAbortType = MSS_USER_ABORT_TYPE_ALL;
         }

         pd.MakeRunWritable();
         pd.Run().MeterRbcs_OK.Set(false);
         pd.Run().cancelJustRas.Set(true);
         pd.Run().MssUserAbortType.Set(newAbortType);
         DataLog(log_level_proc_mss_info) << "End Run of RAS ordered by operator" << endmsg;
      }
      break;

      case  END_PAS_ADDITION :
      {
         MSS_USER_ABORT_TYPE newAbortType = MSS_USER_ABORT_TYPE_PAS;

         if (pd.Run().MssUserAbortType.Get() == MSS_USER_ABORT_TYPE_ALL ||
             pd.Run().MssUserAbortType.Get() == MSS_USER_ABORT_TYPE_RAS)
         {
            newAbortType = MSS_USER_ABORT_TYPE_ALL;
         }

         pd.MakeRunWritable();
         pd.Run().MeterPlatelets_OK.Set(false);
         pd.Run().cancelJustPas.Set(true);
         pd.Run().MssUserAbortType.Set(newAbortType);
         DataLog(log_level_proc_mss_info) << "End Run of PAS ordered by operator" << endmsg;
      }
      break;

      default :
         break;
   }

}
/*
typedef enum
{
    NORMAL_END_RUN,
    UNLOAD_ONLY,
    END_RAS_ADDITION,
    END_PAS_ADDITION
} PARTIAL_END_RUN_TYPE;
*/

/* FORMAT HASH 4ffda79f1f46fdb4b91d4baa568e150d */

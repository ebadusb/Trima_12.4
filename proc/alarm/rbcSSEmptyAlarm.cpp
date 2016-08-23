/***************************************
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:
 *
 *************************************************
 *
 * $Log: rbcSSEmptyAlarm.cpp $
 * Revision 1.7  2008/06/10 21:11:20Z  dslausb
 * IT 8672 - Change text on RAS empty alarm for DRBC sets
 * Revision 1.6  2007/11/28 20:58:47Z  jheiusb
 * 8364 -- need to activate alarms after connection only
 * Revision 1.5  2007/11/27 18:35:43Z  jheiusb
 * 8257 -- fix RBC bag assumtions on product size
 * Revision 1.4  2007/10/08 16:05:44Z  jheiusb
 * 8124 -- cut out alarm to replace SS bag just before complete
 * Revision 1.3  2007/02/20 23:56:37Z  dslausb
 * - IT 7105 - Changed text of safety failure startup alarm
 * - Added change RAS bag alarm
 * Revision 1.2  2007/02/16 22:12:46Z  jheiusb
 * add RBC ss bag alarm
 * Revision 1.1  2007/01/31 23:26:48Z  jheiusb
 * Initial revision
 *
 *
 *************************************************/

#include "rbcSSEmptyAlarm.h"
#include "procdata.h"
#include "cobeconfig.h"

const float PercentLeft = CobeConfig::data().MssRbcSSPercent / 100.0f; // TODO : ADD NEW cc VARIABLE

DEFINE_OBJ(RbcSSEmpty);

RbcSSEmpty::RbcSSEmpty()
   : _emptySrbc(MSS_RBC_SS_EMPTY),
     _emptyDrbc(MSS_DRBC_SS_EMPTY),
     _cnt(0)
{}

RbcSSEmpty::~RbcSSEmpty()
{}

void RbcSSEmpty::Monitor ()
{
   ProcData pd;

   if (!(pd.RunTargets().MeterRbcs.Get() && pd.Run().MeterRbcs_OK.Get() ))
   {
      return;
   }


   if (pd.Run().currentMSSSubState.Get() < SS_MSSPRIME)
   {
      return;
   }

   if (pd.Run().stopCheckingRbcASBagEmpty.Get())
   {
      return;
   }


   float SS_Volume_Trigger = pd.Volumes().VrbcBagCapacity.Get() * PercentLeft;
   bool  isBlackStamp      =  (pd.TrimaSet().Cassette == cassette::RBCPLS);

   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   bool targetedAsSplit = (pd.RunTargets().RbcDoseTarget.Get() > 299.0) ||
                          (pd.Config().isPromptedDrbcSplit(pd.RunTargets().ProcNumber.Get()));

   float estSolutionUsed = ( pd.RunTargets().RBCSolutionTarget
                             + CobeConfig::data().MssRbcSubstateMaxPrimeVol
                             + CobeConfig::data().MssPtfVolume);

   if (targetedAsSplit)
   {
      estSolutionUsed =  estSolutionUsed + CobeConfig::data().MssPtfVolume;
   }

   if (isBlackStamp &&
       ((estSolutionUsed * (1.0f + CobeConfig::data().MssRbcSSPercent / 100.0f)) >= pd.Config().Procedure.Get().key_rbc_def_bag_vol))
   {
      SS_Volume_Trigger = pd.Volumes().VrbcBagCapacity.Get() * PercentLeft * 2.0f;
   }

   if (pd.Volumes().VrbcStorageRemaining.Get() <= SS_Volume_Trigger)
   {
      // warning for Operator to pause pumps and switch SS bags
      if (isBlackStamp) _emptyDrbc.setAlarm();
      else _emptySrbc.setAlarm();

      _cnt++;
      DataLog(log_level_proc_alarm_monitor_info) << "Alarm: RBC BAG IS ALMOST EMPTY:: volm=>" << pd.Volumes().VrbcStorageRemaining.Get() << "mL" << endmsg;

      // record the values in procvolume_cds

      float tmp_volm =  pd.Volumes().VrbcBagCapacity.Get();

      pd.MakeVolumesWritable();
      // assume fresh SS bag
      // IT8257 just connect one at a time after the first two
      //  if (isBlackStamp)
      //  {
      //      pd.Volumes().VrbcStorageRemaining.Set(2.0f * tmp_volm);
      //  } else {
      pd.Volumes().VrbcStorageRemaining.Set(tmp_volm);
      //  }

      pd.MakeVolumesReadable();

      DataLog(log_level_proc_alarm_monitor_info) << "ReSet RBC SS bag vol remaining to =>" <<  pd.Volumes().VrbcStorageRemaining.Get() << "mL" <<  endmsg;

   }





}

/* FORMAT HASH dacfdc32f664eec08fff2dc843c612e7 */

/***************************************
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ss_prime2soon_alarm.cpp
 *
 *************************************************
 *
 * $Log: pltSSEmptyAlarm.cpp $
 * Revision 1.7  2007/11/28 20:58:40Z  jheiusb
 * 8364 -- need to activate alarms after connection only
 * Revision 1.6  2007/10/08 16:05:36Z  jheiusb
 * 8124 -- cut out alarm to replace SS bag just before complete
 * Revision 1.5  2007/04/02 20:26:35Z  jheiusb
 * 7879 -- detect second SS bag.
 * Revision 1.4  2006/07/21 16:19:43Z  jheiusb
 * 7643 -- add sound to this alarm
 * Revision 1.3  2006/04/11 22:30:19Z  jheiusb
 * 7424 -- fix default bag volm override
 * Revision 1.2  2006/03/03 17:28:07Z  jheiusb
 * initial merge
 * Revision 1.1  2006/03/03 17:13:22Z  jheiusb
 * 7428
 * Revision 1.2  2006/01/21 21:36:06Z  jheiusb
 * 7058 -- add plt bag running empty alarm
 * Revision 1.1  2006/01/20 22:36:49Z  jheiusb
 * Initial revision
 * Revision 1.2  2005/10/06 14:56:33Z  dslausb
 * IT 7202 - Metered Storage
 * - Fixed MKS Revision History Tag
 * - Implemented Monitor() Function
 * - Improved Logging
 *
 *
 *************************************************/

#include "pltSSEmptyAlarm.h"
#include "procdata.h"
#include "guiproc.h"
#include "cobeconfig.h"

const float PercentLeft = CobeConfig::data().MssPltSSPercent / 100.0f; // 10%

DEFINE_OBJ(PltSSEmpty);

PltSSEmpty::PltSSEmpty()
   : _empty(MSS_PLT_SS_EMPTY),
     _cnt(0),
     _requestPASVolmMsg(Callback<PltSSEmpty>(this, &PltSSEmpty::remainingPASneeded)  )
{}

PltSSEmpty::~PltSSEmpty()
{}
void PltSSEmpty::Init ()
{}

void PltSSEmpty::Monitor ()
{
   ProcData pd;

   if (!(pd.RunTargets().MeterPlatelets.Get() && pd.Run().MeterPlatelets_OK.Get() ))
   {
      return;
   }

   if (pd.Run().currentMSSSubState.Get() < SS_MSSPRIME)
   {
      return;
   }

   if (pd.Run().stopCheckingPltASBagEmpty.Get())
   {
      return;
   }

   const float SS_Volume_Trigger = pd.Volumes().VpltBagCapacity.Get() * PercentLeft;

   if (pd.Volumes().VpltStorageRemaining.Get() <= SS_Volume_Trigger)
   {

      // warning for Operator to pause pumps and switch SS bags
      _empty.setAlarm() ;
      _cnt++;
      DataLog(log_level_proc_alarm_monitor_info) << "Alarm: PLT BAG IS ALMOST EMPTY"  << endmsg;

      // record the values in procvolume_cds
      pd.MakeVolumesWritable();
      float tmp_volm =  pd.Volumes().VpltBagCapacity.Get();
      // assume fresh SS bag
      pd.Volumes().VpltStorageRemaining.Set(tmp_volm);

      pd.MakeVolumesReadable();

      DataLog(log_level_proc_alarm_monitor_info) << "ReSet SS bag vol remaining to =>" <<  pd.Volumes().VpltStorageRemaining.Get() << endmsg;

      pd.MakeRunWritable();
      pd.Run().SecondPLTSSBag.Set(true);
      pd.MakeRunReadable();

   }



}


void PltSSEmpty::remainingPASneeded ()
{
   ProcData pd;

   Proc2GuiSendRemainingPasVolDataMsg volumeNeeded(MessageBase::SEND_LOCAL);

   float solutionToBeMeteredOut = pd.RunTargets().PltSolutionTarget  *  pd.Run().PltSSVolumeFactor;
   float volm                   = (solutionToBeMeteredOut - pd.Volumes().VstoragePltBag);
   if (volm < 0.0f)
      volm = 0.0f;


   DataLog(log_level_proc_alarm_monitor_info) << "Sending PAS volume remaining to meter to GUI => "  << volm << endmsg;
   volumeNeeded.send(volm);

}

/* FORMAT HASH 0d4746a8e8c53e9016f6920387df2678 */

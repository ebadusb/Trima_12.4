/************************************************************
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cpsHighAlarm.cpp
 *
 ************************************************************
 *
 *  $Log: cpsHighAlarm.cpp $
 *  Revision 1.7  2007/12/04 23:13:25Z  jheiusb
 *  8317 -- make CPS endrun for MSS
 *  Revision 1.6  2007/10/29 21:36:09Z  jheiusb
 *  8237 add logging
 *  Revision 1.5  2007/10/29 21:05:51Z  jheiusb
 *  8237 add some more to the logs
 *  Revision 1.4  2007/10/26 16:49:20Z  jheiusb
 *  8082 -- PTF CPS alarms end product based
 *  Revision 1.3  2007/10/18 20:00:33Z  jheiusb
 *  8254 -- remove the  Autosplit
 *  Revision 1.2  2007/09/19 16:10:41Z  jheiusb
 *  8055 -- CPS alarm for metering is based on the Zero'ed CPS value
 *  Revision 1.1  2006/03/03 23:06:23Z  jheiusb
 *  Initial revision
 *  Revision 1.1  2005/10/06 19:07:50Z  dslausb
 *  Initial revision
 *  Revision 1.4  2005/10/05 20:10:15Z  dslausb
 *  IT 7202 - Metered Storage
 *  - Fixed MKS Revision History Tag
 *  - Improved Logging
 *
 *
 ************************************************************/

#include "cpsHighAlarm.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_OBJ(cpsHighAlarm);

cpsHighAlarm::cpsHighAlarm()
   : CPShighAlarm(CPS_TOO_HIGH_ALARM)
{}

cpsHighAlarm::~cpsHighAlarm()
{}

void cpsHighAlarm::Monitor ()
{
   ProcData    pd;
   const float CPSPressure = pd.Status().CPS() - pd.TrimaSet().CPSZero;

   if (  (CPSPressure > CobeConfig::data().MssCpsHigh)
         && (!pd.AlarmActive())  )
   {
      CPShighAlarm.setAlarm();

      pd.SetPluggedFilter();

      DataLog(log_level_proc_alarm_monitor_info)
         << "CPS value too high during metered storage. "
         << "CPS Pressure(zeroed): "
         << CPSPressure
         << " ( limit-> "
         << CobeConfig::data().MssCpsHigh
         << " )"
         << " (  volume of SS metered to Product 1 ->" <<  pd.Volumes().RBCProduct_A_FinalVss
         << ",  volume of SS metered to Product 2 ->" <<  pd.Volumes().RBCProduct_B_FinalVss
         << " )"
         << endmsg;
   }
}

/* FORMAT HASH 1e3cbff394e9df529ca00758d003c500 */

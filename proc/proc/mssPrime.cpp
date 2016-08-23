/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssPrime.cpp
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssPrime.cpp 1.14 2009/02/09 21:22:07Z jheiusb Exp jheiusb $
$Log: mssPrime.cpp $
Revision 1.14  2009/02/09 21:22:07Z  jheiusb
9002 -- CPS changes
Revision 1.13  2008/02/26 23:47:15Z  jheiusb
8424 -- fix aps alarm in PLT mss
Revision 1.12  2007/11/20 23:27:56Z  jheiusb
8317 -- fix number of Pressure alarms
Revision 1.11  2007/10/30 21:08:01Z  jheiusb
7402 -- MSS Prime was running in RBC addition
Revision 1.10  2007/04/17 15:47:05Z  jheiusb
7579 -- make parallel prime PFR
Revision 1.9  2006/12/04 16:16:49Z  jheiusb
remerge from original rbc+plt mss 5.2
Revision 1.8  2006/06/09 19:07:55Z  jheiusb
Revision 1.7  2006/05/09 16:45:00Z  jheiusb
just add some comments for posterity
Revision 1.6  2006/04/26 17:28:29Z  jheiusb
7405 -- fix aps if it triggers and then falls below threshold
Revision 1.5  2006/04/19 22:08:41Z  jheiusb
7491 -- PFR hangs
Revision 1.4  2006/04/13 17:23:02Z  jheiusb
7484 -- APS checking for Prime and addition enabled
Revision 1.3  2006/03/27 17:52:52Z  jheiusb
7350 -- PFR for MSS
Revision 1.2  2006/03/03 17:33:01Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:17:31Z  jheiusb
Initial revision
Revision 1.6  2005/12/02 17:11:49Z  jheiusb
7267 -- stop pumps on exiting the state (postExit)
Revision 1.5  2005/11/03 18:03:19Z  jheiusb
7202 -- state file cleanup
Revision 1.4  2005/11/03 17:04:27Z  jheiusb
7230 -- DRBC split alarm
Revision 1.3  2005/10/07 22:06:37Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:23Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#include "mssPrime.h"
#include "states.h"
#include "procdata.h"

#include "cobeconfig.h"

DEFINE_STATE(MSSPrime);


const float pressureRecoverySpeed = CobeConfig::data().MssApsAlertRecoveryQin;

MSSPrime::MSSPrime()
   : StateAbs(),
     apsHigh(false)
{}

MSSPrime::MSSPrime(const MSSPrime& state)
   : StateAbs(state),
     apsHigh(false)
{}

MSSPrime::~MSSPrime()
{}

int MSSPrime::preProcess ()
{

   ProcData pd;

   float    CurAPS = pd.Status().APS();

   if (
      ( pd.Run().currentMSSSubState != SS_MSSRBCPRIME1 )
      || ( pd.Run().currentMSSSubState != SS_MSSRBCPRIMESALVAGE )
      )
   {


      // if your APS is too high OR if it was too high last time through (see IT 7504)
      if (  CurAPS >= CobeConfig::data().MssApsHigh || apsHigh)
      {

         if (!apsHigh)
         {

            DataLog(log_level_proc_info) << "Found APS too High ->" << CurAPS << endmsg;
            apsHigh = true;
            pd.Orders().Qac (0.0f);
            pd.Orders().Qplasma (0.0f);
            pd.Orders().Qcollect(0.0f);
            pd.Orders().Qin(0.0f);
            pd.Orders().Qrp(0.0);

         }
         else
         {

            DataLog(log_level_proc_info) << "trying to reduce APS too High ->" << CurAPS << endmsg;
            pd.Orders().Qin (pressureRecoverySpeed);
            pd.Orders().Qac (0.0f);
            pd.Orders().Qplasma (0.0f);
            pd.Orders().Qcollect(0.0f);
            pd.Orders().Qrp(0.0);

         }
      }

      if (apsHigh)
      {

         pd.Orders().Qac (0.0f);
         pd.Orders().Qplasma (0.0f);
         pd.Orders().Qcollect(0.0f);
         pd.Orders().Qrp(0.0);
      }

      if ( (CurAPS <= CobeConfig::data().MssApsBackpressure)
           && apsHigh )
      {

         DataLog(log_level_proc_info) << "APS to backpressure ->" << CurAPS << endmsg;
         pd.Orders().Qin (0.0);
         apsHigh = false;

      }

   }
   return NORMAL;
}

int MSSPrime::postProcess ()
{
   ProcData pd;

   if (
      apsHigh &&
      (
         ( pd.Run().currentMSSSubState != SS_MSSRBCPRIME1 ) || ( pd.Run().currentMSSSubState != SS_MSSRBCPRIMESALVAGE )
      )
      )
   {
      pd.Orders().Qac (0.0f);
      pd.Orders().Qplasma (0.0f);
      pd.Orders().Qcollect(0.0f);
      pd.Orders().Qrp(0.0);
   }
   // set this when you are complete with priming
   return NORMAL;
}


int MSSPrime::preEnter ()
{
   ProcData pd;
   DataLog(log_level_proc_info) << "MSS Prime Started!!! "  << endmsg;
   if (!pd.Run().mssInPFR)   // if we're in PFR
   {
      pd.MakeRunWritable();
      pd.Run().currentMSSSubState = SS_MSSPRIME;
      pd.MakeRunReadable();
   }


   return NORMAL;
}

int MSSPrime::postExit ()
{

   ProcData pd;

   // make sure all pumps that do MSS addition are off!
   pd.Orders().Qac (0.0f);
   pd.Orders().Qplasma (0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qrp(0.0);


   DataLog(log_level_proc_info) << "MSS Prime ended!!! "  << endmsg;
   return NORMAL;
}

void MSSPrime::copyOver (const MSSPrime&)
{}

void MSSPrime::cleanup ()
{}

/* FORMAT HASH 441219029b718498fadb380c66e9b4b1 */

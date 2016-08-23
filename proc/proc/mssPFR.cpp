/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssPFR.cpp
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssPFR.cpp 1.19 2008/08/04 17:04:40Z dslausb Exp jheiusb $
$Log: mssPFR.cpp $
Revision 1.19  2008/08/04 17:04:40Z  dslausb
More PFR robustication
Revision 1.18  2008/07/30 21:57:49Z  dslausb
Fixed some PFR issues
Revision 1.17  2008/07/29 15:03:46Z  dslausb
Make metering more robust to PFR
Revision 1.16  2008/04/08 21:41:45Z  jheiusb
8115 -- fix prime and General PFR stuff
Revision 1.15  2007/08/14 18:22:56Z  dslausb
IT7986: Combined cassette bit for PTF and RAS into one bit
Revision 1.14  2007/06/11 22:37:13Z  dslausb
IT 8084 - PFR button returning to Two-Button screen
Revision 1.13  2007/05/01 22:29:36Z  jheiusb
7579 -- complete PFR for the parallel states
Revision 1.12  2007/04/02 20:21:13Z  jheiusb
7579 -- patch up PFR for 5.R
Revision 1.11  2007/03/06 23:52:52Z  dslausb
- Changed MSS progress bar to a one-bar system.
- Changed the order of the connect screens
- Changed the way proc updates GUI of the MSS volumes
Revision 1.10  2007/01/24 21:13:01Z  dslausb
Fixed MSS PFR gui messages
Revision 1.9  2006/12/04 16:16:31Z  jheiusb
remerge from original rbc+plt mss 5.2
Revision 1.8  2006/09/11 21:55:27Z  jheiusb
7730 -- add second ressetup to insure empty at start of MSSPrime.
Revision 1.7  2006/09/11 19:28:57Z  dslausb
Revision 1.6  2006/07/21 16:21:33Z  jheiusb
7601 -- salvage case added
Revision 1.5  2006/05/08 21:56:45Z  jheiusb
7514 -- fix addition during PFR;
Revision 1.4  2006/05/03 17:24:34Z  jheiusb
7515 -- PFR for MSS Disconnect
Revision 1.3  2006/03/30 20:49:04Z  jheiusb
7350 -- Metered storage PFR
Revision 1.2  2006/03/27 17:52:09Z  jheiusb
7350 -- PFR for MSS
*/

#include "mssPFR.h"
#include "states.h"
#include "procdata.h"
#include "MssCommon.h"


DEFINE_STATE(MSSPFR);

MSSPFR::MSSPFR()
   : StateAbs(),
     transitionNow(false),
     transitionTo(TransitionToVerifyClosure)
{}

MSSPFR::MSSPFR(const MSSPFR& state)
   : StateAbs(state),
     transitionNow(false),
     transitionTo(TransitionToVerifyClosure)

{}

MSSPFR::~MSSPFR()
{}

void MSSPFR::sendGuiTransitionMsg (AAS_PFR_SCREEN scrnTran)
{
   DataLog (log_level_proc_mss_info)
      << "Sending PFR AAS gui screen transition message with value of "
      << scrnTran << endmsg;

   ProcToGuiAasPfrMsg transitionMsg(MessageBase::SEND_LOCAL);
   transitionMsg.send(scrnTran);
}

int MSSPFR::preProcess ()
{
   ProcData pd;

   setGuiMssStatus();

   State_names mssSubstate = pd.Run().currentMSSSubState.Get();
   State_names rasSubstate = pd.Run().currentRBCMSSSubState.Get();
   State_names pasSubstate = pd.Run().currentPLTMSSSubState.Get();

   DataLog(log_level_proc_mss_info) << "MSS PFR state info: (MSS="
                                    << mssSubstate << "; RAS=" << rasSubstate << "; PAS="
                                    << pasSubstate << ")" << endmsg;

   ProcToGuiAasPfrMsg transitionMsg(MessageBase::SEND_GLOBAL);

   switch (mssSubstate)
   {
      case  METERED_STORAGE :    // STATE #12
      case  SS_MSSPFR :
      case  SS_VERIFYCLOSURE :
      case  SS_VERIFYCLOSURE3 :
      case  SS_RESSETUP :
      {
         DataLog(log_level_proc_mss_info) << "MSSPFR to MSS Testing states." << endmsg;
         transitionTo = TransitionToVerifyClosure;
         sendGuiTransitionMsg(AAS_PFR_CHANNELTEST);
         break;
      }

      case  SS_MSSSETUP :
      {
         DataLog(log_level_proc_mss_info) << "MSSPFR to clamp channel state." << endmsg;
         transitionTo = TransitionToVerifyClosure;
         sendGuiTransitionMsg(AAS_PFR_CLAMPCHANNEL);
         break;
      }

      case  SS_MSSCONNECT :
      {
         transitionTo = TransitionToMSSConnect;

         if ( readyForRBCMSS() )
         {
            DataLog(log_level_proc_mss_info) << "MSSPFR to RasConnect." << endmsg;
            sendGuiTransitionMsg(AAS_PFR_RASCONNECT);
         }
         else
         {
            DataLog(log_level_proc_mss_info) << "MSSPFR to PasConnect." << endmsg;
            sendGuiTransitionMsg(AAS_PFR_PASCONNECT);
         }
         break;
      }
      case  SS_MSSPRIME :
      case  SS_MSSRBCPRIME1 :
      case  SS_MSSRBCPRIMESALVAGE :
      case  SS_RESSETUP2 :
      case  SS_RESSETUP3 :
      case  SS_MSSPLTPRIMESALVAGE :
      case  SS_MSSPLTPRIME1 :
      case  SS_MSSPLTPRIME2 :
      {
         DataLog(log_level_proc_mss_info) << "MSSPFR to Main State MSSPrime." << endmsg;
         transitionTo = TransitionToMSSPrime;
         sendGuiTransitionMsg(AAS_PFR_PRIME);
         break;
      }
      case  SS_MSSPARALLELPROC :
      {
         DataLog(log_level_proc_mss_info) << "MSSPFR to MSS Parallel state." << endmsg;
         transitionTo = TransitionToMSSParallel;

         if (pasSubstate == SS_MSSPLTADDITIONPAUSE)
         {
            sendGuiTransitionMsg(AAS_PFR_REDIRECT_PAS);
         }
         else if (rasSubstate >= SS_MSSPLTADDITIONPAUSE && pasSubstate > SS_MSSPLTADDITIONPAUSE)
         {
            sendGuiTransitionMsg(AAS_PFR_ADDITION);
         }
         else
         {
            // There's still work to be done in prime.
            sendGuiTransitionMsg(AAS_PFR_PRIME);
         }

         break;
      }
      case  METERED_STORAGE_DISCONNECT :     // STATE #13
      {
         DataLog(log_level_proc_mss_info) << "MSSPFR to MSSDisconnect." << endmsg;
         transitionTo = TransitionToMSSDisconnect;
         // sendGuiTransitionMsg(AAS_PFR_UNLOAD); //No transition message necessary.  This is the default screen
         // for this state.
         break;
      }
      default :
      {
         DataLog(log_level_proc_mss_info) << "MSSPFR cannot find its place ... Unable to do MSS PFR, ending run" << endmsg;
         transitionTo = TransitionToMSSDisconnect;
         break;
      }
   }
   return NORMAL;
}

int MSSPFR::postProcess ()
{
   transitionNow = true;
   return NORMAL;
}

int MSSPFR::transitionStatus ()
{
   ProcData pd;

   if (!pd.Run().mssInPFR)     // if we're in PFR then we dont have to do this
   {
      return 1  ;
   }

   if (transitionNow)
   {
      return transitionTo;
   }

   return NO_TRANSITION;

}

int MSSPFR::preEnter ()
{
   ProcData pd;

   if (!pd.Run().mssInPFR)    // if we're in PFR these should be set with the last known volumes.. so don't write over them
   {
      pd.MakeRunWritable();
      pd.Run().currentMSSSubState = SS_MSSPFR;
      pd.MakeRunReadable();
   }

   return NORMAL;
}

int MSSPFR::postExit ()
{
   return NORMAL;
}

void MSSPFR::copyOver (const MSSPFR&)
{}

void MSSPFR::cleanup ()
{}

void MSSPFR::setGuiMssStatus ()
{
   ProcData pd;

   bool     ready4pas = readyForPLTMSS();
   bool     ready4ras = readyForRBCMSS();

   // if its ready for MSS
   pd.MakeRunWritable();
   pd.Run().MeterRbcs_OK      = ready4ras;
   pd.Run().MeterPlatelets_OK = ready4pas;
   pd.MakeRunReadable();
}

/* FORMAT HASH 998fe7ea45c7613a4a24d2dd496c6aac */

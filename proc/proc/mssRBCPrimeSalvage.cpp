#include "mssRBCPrimeSalvage.h"
#include "states.h"
#include "procdata.h"

#include "cobeconfig.h"

DEFINE_STATE(MSSRBCPrimeSalvage);


const float acPumpCmdSpeed        = CobeConfig::data().MssRBCPrimeSpeed;
const float rasSalvagePrimeVolume = CobeConfig::data().MssRbcSubstateSalvageVol;

MSSRBCPrimeSalvage::MSSRBCPrimeSalvage()
   : StateAbs(),
     transitionNow(false),
     rasPumpVol(0.0f),
     apsPres2HighAlarm(APS_TOO_HIGH_ALARM_MSS)
{}

MSSRBCPrimeSalvage::MSSRBCPrimeSalvage(const MSSRBCPrimeSalvage& state)
   : StateAbs(state),
     transitionNow(false),
     rasPumpVol(0.0f),
     apsPres2HighAlarm(APS_TOO_HIGH_ALARM_MSS)
{}

MSSRBCPrimeSalvage::~MSSRBCPrimeSalvage()
{}

int MSSRBCPrimeSalvage::transitionStatus ()
{
   ProcData pd;

   // if we're not doing MSS for RBC then get the heck out of here
   if (!(pd.RunTargets().MeterRbcs && pd.Run().MeterRbcs_OK))
   {
      return 1;
   }

   if (pd.Run().mssInPFR)
   {
      if ( pd.Run().currentMSSSubState > SS_MSSRBCPRIMESALVAGE  )
         return 1;
   }

   // if we DID do enough rinseback skip this salvage case
   if (pd.Run().rinsebackVolumeSuccessMss.Get() )
   {
      DataLog(log_level_proc_info) << "Skipping out because Not a salvage case MSS!"   << endmsg;
      return 1;
   }

   if ( transitionNow )
   {
      return 1;
   }
   return NO_TRANSITION;
}

int MSSRBCPrimeSalvage::preProcess ()
{
   ProcData pd;

   pd.Orders().Qac (acPumpCmdSpeed);



   if (  pd.Status().APS() >= CobeConfig::data().MssApsHigh  )
   {
      DataLog(log_level_proc_info) << "APS Alarm ends RAS prime salvage; APS=" <<  pd.Status().APS() << endmsg;

      pd.SetPluggedFilter();

      // disable MSS for RBC
      pd.MakeRunWritable();
      pd.Run().MeterRbcs_OK = false;
      pd.MakeRunReadable();

      apsPres2HighAlarm.setAlarm();
   }

   return NORMAL;
}



int MSSRBCPrimeSalvage::postProcess ()
{
   ProcData pd;
   rasPumpVol += pd.Status().ACPump.dV();

   if (rasPumpVol >= rasSalvagePrimeVolume)
   {
      transitionNow = true;
   }



   return NORMAL;
}


int MSSRBCPrimeSalvage::preEnter ()
{
   ProcData pd;


   if (!pd.Run().mssInPFR)   // if we're NOT  in PFR
   {
      pd.MakeRunWritable();

      // setup this for PFR
      pd.Run().currentMSSSubState.Set(SS_MSSRBCPRIMESALVAGE);
      pd.MakeRunReadable();

   }
   else
   {

      if ( (pd.Run().currentMSSSubState.Get() == SS_MSSRBCPRIMESALVAGE)  )
      {
         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR; with AC pumped => " << pd.Run().hadPrimedVolm.Get() << "; Return was at =>" << pd.Run().hadPrimedReturnVolm.Get() << endmsg;

         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();
      }


   }

   return NORMAL;
}

int MSSRBCPrimeSalvage::postExit ()
{
   ProcData pd;


   transitionNow = false;

   // kill the pumps
   pd.Orders().Qac(0.0);
   pd.Orders().Qin(0.0);
   pd.Orders().Qrp(0.0);


   return NORMAL;
}

void MSSRBCPrimeSalvage::copyOver (const MSSRBCPrimeSalvage&)
{}

void MSSRBCPrimeSalvage::cleanup ()
{}

/* FORMAT HASH a7377283fa24f9b68f21f2765ee05d5b */

#include "ptfcpshigh_recovery.h"
#include "cobeconfig.h"
#include "procdata.h"

const float CPSClearInletLimit = 70.0f;
const float BAGLINE            = 50.0f;

DEFINE_OBJ(PtfCpsHighRecovery);

PtfCpsHighRecovery::PtfCpsHighRecovery()
   : RecoveryAbs(),
     _clearInletVolume(0.0f),
     _bagLineInletVolume(0.0f),
     _centrifugeAtAltSpeed(false)
{}

PtfCpsHighRecovery::PtfCpsHighRecovery(const PtfCpsHighRecovery& rec)
   : RecoveryAbs(rec),
     _clearInletVolume(0.0f),
     _bagLineInletVolume(0.0f),
     _centrifugeAtAltSpeed(false)
{
   copyOver(rec);
}

PtfCpsHighRecovery::~PtfCpsHighRecovery()
{
   cleanup();
}

int PtfCpsHighRecovery::init ()
{
   return 1;
}

int PtfCpsHighRecovery::doRecovery ()
{
   ProcData pd;

   if (_centrifugeAtAltSpeed)
   {

      float returnSpeedNC =    pd.Predict().Qrp(pd.Run().DrawCycle.Get(), SubstateMap::alg(NOCOLLECT_STATE) );
      float currentQin    =    pd.Predict().Qin(pd.Run().DrawCycle.Get(), SubstateMap::alg(pd.Run().Substate.Get() ) );
      float currentQc     =    pd.Predict().Qc(pd.Run().DrawCycle.Get(), SubstateMap::alg(pd.Run().Substate.Get() ) );
      float currentQp     =    pd.Predict().Qp(pd.Run().DrawCycle.Get(), SubstateMap::alg(pd.Run().Substate.Get() ) );
      float currentQac    =    pd.Predict().Qac(pd.Run().DrawCycle.Get(), SubstateMap::alg(pd.Run().Substate.Get() ) );

      if ( _clearInletVolume < CPSClearInletLimit )
      {
         _clearInletVolume += pd.Status().InletPump.dV();

         pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
         pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
         pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);


         if ((_clearInletVolume > 35.0f) )
         {
            static int logged1 = 0;
            if (!logged1)
            {
               logged1 = 1;
               DataLog (log_level_proc_info) << "Start second half of clear"   << endmsg;
            }

            pd.Orders().Qplasma(0.0f);
            pd.Orders().Qac(currentQac);
            pd.Orders().Qcollect(currentQc);
            pd.Orders().Qin(currentQin);

            if (returnSpeedNC > 110.0f)
               returnSpeedNC = 110.0f;

            pd.Orders().Qrp(returnSpeedNC);

            pd.Orders().RPM(3000.0f);

         }
         else
         {

            static int logged2 = 0;
            if (!logged2)
            {
               logged2 = 1;
               DataLog (log_level_proc_info) << "Start first half of clear"   << endmsg;
            }

            command_AC_Inlet(currentQac, currentQin);

            if (returnSpeedNC > 110.0f)
               returnSpeedNC = 110.0f;
            pd.Orders().Qrp(returnSpeedNC);

            pd.Orders().Qplasma(0.0f);
            pd.Orders().Qcollect(currentQc / 2.0f);

            pd.Orders().RPM(1800.0f);
         }

      }
      else if (  ( _clearInletVolume   >= CPSClearInletLimit)
                 &&( _bagLineInletVolume < BAGLINE))
      {

         static int logged3 = 0;
         if (!logged3)
         {
            logged3 = 1;
            DataLog (log_level_proc_info) << "Enter clearing bag line"   << endmsg;
         }

         // ok, now we open the valve so listen for the damn CPS alarms... that ends the circus
         _AlarmMsg.init(Callback<PtfCpsHighRecovery>(this, &PtfCpsHighRecovery::alarmListener), MessageBase::SNDRCV_RECEIVE_ONLY);

         _bagLineInletVolume += pd.Status().InletPump.dV();

         pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
         pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_COLLECT);
         pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);


         pd.Orders().RPM(3000.0f);

         command_AC_Inlet(currentQac, currentQin);

         pd.Orders().Qplasma(currentQp / 2.0f);
         pd.Orders().Qcollect(currentQc / 2.0f);

         if (returnSpeedNC > 110.0f)
            returnSpeedNC = 110.0f;
         pd.Orders().Qrp(returnSpeedNC);

      }
      else
      {

         static int logged4 = 0;
         if (!logged4)
         {
            logged4 = 1;
            DataLog (log_level_proc_info) << "Done with recovery"   << endmsg;
         }
         //
         // We are done the line is primed ...
         //
         pd.MakeRunWritable();
         pd.Run().DoCPTPTFRecovery.Set(false);     // invoke recovery
         pd.MakeRunReadable();

      }

   }
   else
   {


      static int logged5 = 0;
      if (!logged5)
      {
         logged5 = 1;
         DataLog (log_level_proc_info) << "Enter centrifuge spin down"   << endmsg;
      }
      pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
      pd.Orders().RPM(1800.0f);

      pd.Orders().Qplasma(0.0);
      pd.Orders().Qac(0.0);
      pd.Orders().Qcollect(0.0);
      pd.Orders().Qin(0.0);
      pd.Orders().Qrp(0.0);


      if (  pd.Status().CentrifugeRPM() < 1850.0f)
      {

         static int logged6 = 0;
         if (!logged6)
         {
            logged6 = 1;
            DataLog (log_level_proc_info) << "Finished centrifuge spin down. rpm="  << pd.Status().CentrifugeRPM()  << endmsg;
         }

         _centrifugeAtAltSpeed = true;
      }
   }

   return 1;
}

int PtfCpsHighRecovery::mayEnter () const
{
   // ProcData pd;

   return 1;
}

int PtfCpsHighRecovery::shouldRecover ()
{
   ProcData pd;

   //
   // If we are using a set with replacement fluid and the replacement
   //  fluid is trying to be pumped without the line being primed, or
   //  we are not finished with the recovery ...
   //
   if ( pd.Run().DoCPTPTFRecovery.Get() && !pd.AlarmActive() )
   {

      return 1;
   }
   return 0;
}

void PtfCpsHighRecovery::reset ()
{
   _clearInletVolume     = 0.0f;
   _bagLineInletVolume   = 0.0f;
   _centrifugeAtAltSpeed = false;
}



void PtfCpsHighRecovery::copyOver (const PtfCpsHighRecovery&)
{}

void PtfCpsHighRecovery::cleanup ()
{}

void PtfCpsHighRecovery::command_AC_Inlet (float Qac, float Qin)
{

   ProcData pd;
   // Run the Qin and Qac at half their commanded, but remember Qin floor is 20
   // and Qac must be scaled to maintain proper AC ratio.
   if (pd.Run().DrawCycle.Get())
   {
      const float MIN_Q_IN =   CobeConfig::data().QinLimitMin;
      float       newQin   =   Qin / 2.0f;
      float       acRatio  =   0.0;

      if (   newQin <= MIN_Q_IN
             && Qin > 0.0f)
      {
         pd.Orders().Qin(MIN_Q_IN);
         acRatio = MIN_Q_IN / Qin;
      }
      else
      {
         pd.Orders().Qin(newQin);
      }
      // this AcRatio stuff is to maintain the acRatio when you approach the Qin floor of 20mL/min
      if (acRatio > 0.0f )
         pd.Orders().Qac(acRatio * Qac);
      else
         pd.Orders().Qac(Qac / 2.0f);

   }
   else       // return dont do nothin

   {
      pd.Orders().Qin(0.0f);
      pd.Orders().Qac(0.0f);
   }

}



void PtfCpsHighRecovery::alarmListener ()
{

   ProcData        pd;
   AlarmQueueEntry Alarm = _AlarmMsg.getData();

   switch ( Alarm.name() )

   {   // only this alarm makes sense here, otherwise keep going.....
      case CPS_HIGH_DURING_RBC_COLLECT_PROD2_OK_ALERT :
      {
         DataLog (log_level_proc_info) << "Failing out of Recovery in Clearing Line because CPS still to high"    << endmsg;
         pd.MakeRunWritable();
         pd.Run().DoCPTPTFRecovery.Set(false);       // invoke recovery
         pd.MakeRunReadable();
         break;
      }
      default :
         break;
   }

}

/* FORMAT HASH e52cdfb966a9943cd27fe9bdc2529e93 */

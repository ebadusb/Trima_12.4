/*******************************************************************************
 *
 *
 *
 * TITLE:      s_pmsg.cpp
 *
 *
 * ABSTRACT:   Several one-time messages received from procedure.
 *                aDisposableIDMsg
 *                aMidCycleSwitchMsg
 *
 */

#include "s_pmsg.hpp"
#include "s_exec.hpp"


// SPECIFICATION:    "rcvr" for DisposableID
//
// NOTES :
//
// ERROR HANDLING:   none.

//
//
//   ----------------- Disposable ID message
//
aDisposableIDMsg_rcvr::aDisposableIDMsg_rcvr(aReservoir_Safety* paReservoir)
   : _theDisposableIDMsg(Callback<aDisposableIDMsg_rcvr>(this, &aDisposableIDMsg_rcvr::notify),
                         MessageBase::SNDRCV_RECEIVE_ONLY)
{
   trima_assert(paReservoir);
   _paReservoir = paReservoir;
}

aDisposableIDMsg_rcvr::~aDisposableIDMsg_rcvr()
{
   _paReservoir = NULL;
}

void aDisposableIDMsg_rcvr::notify ()
{
   int temp;
   temp = _theDisposableIDMsg.getData();
   if ( (SET_TYPE)temp > SET_NOT_LOADED )
   {
      _paReservoir->SetSetType((SET_TYPE)temp);
   }
}


//
//
//   ----------------- Mid-cycle switch message
//
aMidCycleSwitchMsg_rcvr::aMidCycleSwitchMsg_rcvr(aReservoir_Safety* paReservoir)
   : _theMidCycleSwitchMsg(Callback<aMidCycleSwitchMsg_rcvr>(this, &aMidCycleSwitchMsg_rcvr::notify),
                           MessageBase::SNDRCV_RECEIVE_ONLY)
{
   trima_assert(paReservoir);
   _paReservoir = paReservoir;
}

aMidCycleSwitchMsg_rcvr::~aMidCycleSwitchMsg_rcvr()
{
   _paReservoir = NULL;
}

void aMidCycleSwitchMsg_rcvr::notify ()
{

   int  cycle;
   cycle = _theMidCycleSwitchMsg.getData();
   long currentState = SafetyExec::instance()->SystemState();

   if (    currentState == BLOOD_RUN
           || currentState == BLOOD_RINSEBACK )
   {
      DataLog(log_level_safe_exec_info) << "Cycle value: " << cycle << "  Mid-cycle switch ordered" << endmsg;

      if ((NEW_CYCLE)cycle == SWITCH_TO_DRAW)
      {
         _paReservoir->UpdateInfusionTimers(CHW_RESERVOIR_EMPTY);
         _paReservoir->update_res_mon();
         _paReservoir->switch_to_draw_cycle();
      }
      else if ((NEW_CYCLE)cycle == SWITCH_TO_RETURN)
      {
         _paReservoir->UpdateInfusionTimers(CHW_RESERVOIR_HIGH);
         _paReservoir->update_res_mon();
         _paReservoir->switch_to_return_cycle();
      }
   }
   else
   {
      DataLog(log_level_safe_exec_info) << "State: " << currentState << "  Mid-cycle switch ignored." << endmsg;
   }
}


// SPECIFICATION:    "rcvr" for NotifySafetyOfA2
//
// NOTES :
//
// ERROR HANDLING:   none.

aNotifySafetyOfA2Msg_rcvr::aNotifySafetyOfA2Msg_rcvr()
   : _theNotifySafetyOfA2Msg(Callback<aNotifySafetyOfA2Msg_rcvr>(this, &aNotifySafetyOfA2Msg_rcvr::notify),
                             MessageBase::SNDRCV_RECEIVE_ONLY)
{
   DataLog(log_level_safe_exec_debug) << "NotifySafetyOfA2Msg_rcvr() constructed." << endmsg;
   trima_assert(paSafetyHardwareCommandsExec);
}

aNotifySafetyOfA2Msg_rcvr::~aNotifySafetyOfA2Msg_rcvr()
{}

void aNotifySafetyOfA2Msg_rcvr::notify ()
{
   DataLog(log_level_safe_exec_info) << "Safety notified of an A2 Alarm by Control." << endmsg;
   // block pump & centrifuge power.
   // we'll re-enable it only for donor disconnect.
   paSafetyHardwareCommandsExec->BlockPumpPower(TRUE);
}

/* FORMAT HASH 5b1e07faf5d411a099bdf0ae24527953 */

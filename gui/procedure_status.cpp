/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/procedure_status.cpp 1.51 2009/06/18 22:14:06Z rm70006 Exp dslausb $
 * $Log: procedure_status.cpp $
 * Revision 1.51  2009/06/18 22:14:06Z  rm70006
 * IT 9239.  Make common code out of the PTF RBC volume display code.
 * Revision 1.50  2009/06/18 21:22:33Z  dslausb
 * Fix rinseback button in screenshots
 * Revision 1.49  2009/04/21 21:18:22Z  dslausb
 * IT 9186 - Fix rinseback button on PFR screens.
 * Revision 1.48  2009/04/15 15:01:57Z  dslausb
 * IT 9072 - Don't seal icon for special rinsebacks.
 * Revision 1.47  2008/12/08 19:09:05Z  dslausb
 * IT 7021 - Get the correct stat line to appear.
 * Revision 1.46  2008/12/02 17:12:05Z  dslausb
 * IT 7021 - Fix the status line for an RBC spillover
 * Revision 1.45  2008/10/01 16:05:50Z  dslausb
 * Change CDS objects so they're not pointers
 * Revision 1.44  2008/09/30 22:26:58Z  dslausb
 * IT 7021 - Chamber saturation displays a different status line.
 * Revision 1.44  2008/09/30 21:54:50Z  dslausb
 * IT 7021 - Make sure unscheduled purge gets its new text.
 * Revision 1.43  2008/09/09 18:36:16Z  dslausb
 * IT 8481 - One last tweak
 * Revision 1.42  2008/09/09 17:00:24Z  dslausb
 * IT 8481 - Fixed a couple bugs with set load/unload screen transitions
 * Revision 1.41  2008/09/08 16:31:54Z  dslausb
 * IT 8306 - Split should always be based on configured RBC dose with no scaling or adjustments.
 * Revision 1.40  2008/08/11 19:10:01Z  dslausb
 * Improve PFR robustness of AAS procedure type and cassette type
 * Revision 1.39  2008/08/04 17:03:10Z  dslausb
 * More GUI PFR robustication
 * Revision 1.38  2008/07/10 20:30:10Z  dslausb
 * Hopefully last bit of stringId standardization stuff
 * Revision 1.37  2008/07/07 18:27:02Z  dslausb
 * More localization string id standardization
 * Revision 1.36  2008/07/03 20:17:55Z  dslausb
 * Further standardization of string IDs for localization
 * Revision 1.35  2008/07/02 23:24:27Z  dslausb
 * Apply new string conventions requested by Marita
 * Revision 1.34  2008/06/02 23:29:54Z  dslausb
 * IT 8556 - Allow flow adjustments on the procedure selection screen
 * Revision 1.33  2008/04/23 21:04:07Z  dslausb
 * IT 8590 - Crit vs Hemoglobin
 * Revision 1.32  2008/04/21 17:30:24Z  dslausb
 * IT 7497 - Update copyright info and splash screen, and add code to snap a screenshot of that screen.
 * Revision 1.31  2008/02/19 17:17:24Z  dslausb
 * Add reboot confirm dialog
 * Revision 1.30  2008/02/18 20:20:19Z  dslausb
 * IT 8456 - Changes for string.info files
 * Revision 1.29  2007/12/10 20:37:21Z  dslausb
 * IT 8241 - Make GUI keep a different set of books for PTF RBC volumes
 * Revision 1.28  2007/12/07 23:46:13Z  dslausb
 * It 8241 - Subtract filter loss from target RBC vol.
 * Revision 1.27  2007/12/07 22:34:04Z  dslausb
 * IT 8241 - Subtract filterloss when necessary.
 * Revision 1.26  2007/12/07 21:02:54Z  jheiusb
 * 8241 -- fix the RSF like overcollection for PTF
 * Revision 1.25  2007/12/07 20:48:22Z  jheiusb
 * 8241 -- Redo RSF for overcollection on PTF
 * Revision 1.24  2007/11/27 18:42:04Z  dslausb
 * IT 8257 - Allow only for one bag to be connected at RAS connection.
 * Revision 1.23  2007/10/31 16:20:47Z  dslausb
 * IT 8245 - Change RBC config screen so that it shows the PTF RSF offset, just like the procedure selection screen.
 * Revision 1.22  2007/04/26 17:09:36Z  dslausb
 * - Fixed PFR cassette func bits message
 * - Moved some initialization stuff from guistart to guiinit (service mode doesn't care about the cassette manager or the aas info)
 * Revision 1.21  2007/03/28 18:34:09Z  dslausb
 * Make sure GUI loads cassette from CDS on PFR
 * Revision 1.20  2007/03/07 23:46:39Z  dslausb
 * IT 7893 - Change cassette alarm button setup
 * Revision 1.19  2006/10/31 21:53:49Z  dslausb
 * Disabled access pressure warning for simulator.  It was jumping in and messing up the screenshot scripts.
 * Revision 1.18  2004/04/07 16:14:25Z  ms10234
 * 6902 - fix rinseback status to always be yellow
 * Revision 1.17  2004/04/05 19:56:28Z  ms10234
 * 6902 - fixed color on status line
 * Revision 1.16  2004/04/02 14:45:44Z  ms10234
 * 6902 - Make rinseback status bar appear during rinseback states.
 * Revision 1.15  2004/02/04 23:16:09Z  jl11312
 * - corrected problem with unecessary reprediction request (IT 6736)
 * Revision 1.14  2004/01/21 17:50:03Z  ms10234
 * 4297 - remove optimization in progress status line when the predict task responds.
 * Revision 1.13  2003/12/18 22:31:12Z  ms10234
 * 6654 - remove the optimization in progress status line when the predict response comes in.
 * Revision 1.12  2003/12/02 18:53:26Z  jl11312
 * - corrected problem with sending re-predict message (IT 6648)
 * Revision 1.11  2003/12/02 16:05:26Z  rm70006
 * IT 6054.  Put hook in to refresh status bar.
 * Revision 1.10  2003/11/21 20:00:16Z  jl11312
 * - updates to insure predict screen is displayed when necessary (IT 6555)
 * Revision 1.9  2003/11/14 20:13:10Z  ms10234
 * 6600 - disallow recovery status line when machine is paused
 * Revision 1.8  2003/11/10 17:39:28Z  rm70006
 * IT 6588.  Fixed truncation problem for both plasma and RBC's.
 * Revision 1.7  2003/10/31 20:42:36Z  ms10234
 * 6565 - Changed to not put status up during plama valve motion recovery.
 * Revision 1.6  2003/10/23 15:53:47Z  ms10234
 * 4653 - added recovery status lines
 * 3338 - added decoupling status line
 * Revision 1.5  2003/10/16 16:59:03Z  jl11312
 * - rounding changes for platelet yield values
 * - transition back to run proc screen at end of rinseback
 * Revision 1.4  2003/08/19 13:59:55Z  jl11312
 * - added common handler for predict messages (IT 5909)
 * Revision 1.3  2003/04/04 17:58:58Z  rm10919
 * Datalog changes. IT 5818
 * Revision 1.2  2003/03/28 22:30:58Z  jl11312
 * - fixed end time display (IT 5934)
 * Revision 1.1  2003/02/12 18:35:28Z  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>
#include <math.h>

#include "gui.h"
#include "guiglobs.hpp"
#include "procedure_status.h"
#include "sounds.hpp"
#include "statline.hpp"
#include "display_formatter.h"



ProcedureStatus::ProcedureStatus()
   : _run_complete_msg        (NULL),
     _proc_status_msg         (NULL),
     _system_state_msg        (NULL),
     _proc_substate_msg       (NULL),
     _recovery_status_msg     (NULL),
     _aps_decoupled_pause_msg (NULL),
     _statLineForSilentAlarms_msg (NULL),
//        _alarm_msg               (NULL),
     _ptrScrnEventDispRaised  (NULL),
     _msgPtrCobeConfigStruct  (NULL),
     _rinseback_request_msg   (NULL),
     _cancel_procedure_msg    (NULL),
     _attached_obj            (NULL),
     _min_proc_status_interval_msec(0),
     _force_proc_status_update(false),
     _system_state_callback(),
     _system_substate_callback(),
     _proc_status_callback(),
     _run_complete_callback(),
     _pressure_callback(),
     _access_pressure_low_warning_enabled(false),
     _access_pressure_low (false),
     _aps_decoupled_alert (false),
     _bitmap_squeeze      (NULL),
     _rinseback_sounded   (false),
     _last_status_line_text(textEmpty),
     _last_status_line_type(DEFAULT_STATUS_LINE),
     _last_system_state    ((State_names) - 1),
     _last_system_substate ((State_names) - 1),
     _notified_run_complete(false),
//        _procedure_information_request_msg (NULL),
     _msgPredictStatus                  (NULL),
//        _procedure_information_available(false),
//        _predict_screen_waiting(false),
//        _repredict_in_progress(false),
//        _predict_pause_active(false),
     _predict_screen_type(0),
//        _procedure_information_callback(),
     _rinseback_or_cancel(false)
//        _periodic_timer (NULL)
{
   memset(&_current_data, 0, sizeof(_current_data));
   _current_data.recovery_substate = RecoveryTypes::None;
   _current_data.recovery_status   = RecoveryTypes::NoRecovery;
}

ProcedureStatus::~ProcedureStatus()
{
   // In real life, the dtor never gets called; the machine just gets reset.  Nevertheless, we clean
   // up after ourselves.

   delete _run_complete_msg;
   delete _proc_status_msg;
   delete _system_state_msg;
   delete _proc_substate_msg;
   delete _recovery_status_msg;
   delete _aps_decoupled_pause_msg;
//   delete _alarm_msg;
   delete _ptrScrnEventDispRaised;
   delete _msgPtrCobeConfigStruct;
   delete _rinseback_request_msg;
   delete _cancel_procedure_msg;
   delete _statLineForSilentAlarms_msg;

//   delete _procedure_information_request_msg;
   delete _msgPredictStatus;
//   delete _periodic_timer;
}

void ProcedureStatus::initialize ()
{
//   _procedure_information_request_msg = new GuiToProcPredictRequestMsg(Callback<ProcedureStatus>(this, &ProcedureStatus::procedure_information_request_notify), MessageBase::SNDRCV_RECEIVE_ONLY);
//   _alarm_msg = new Message<Alarm_struct>(Callback<ProcedureStatus>(this, &ProcedureStatus::procedure_alarm_monitor), MessageBase::SNDRCV_RECEIVE_ONLY);
   _rinseback_request_msg       = new RinsebackRequestMsg(Callback<ProcedureStatus>(this, &ProcedureStatus::procedure_rinseback_or_cancel), MessageBase::SNDRCV_RECEIVE_ONLY);
   _cancel_procedure_msg        = new CancelProcedureMsg(Callback<ProcedureStatus>(this, &ProcedureStatus::procedure_rinseback_or_cancel), MessageBase::SNDRCV_RECEIVE_ONLY);
   _statLineForSilentAlarms_msg = new StatLineRequestMsg(Callback<ProcedureStatus>(this, &ProcedureStatus::procedure_show_stat_line), MessageBase::SNDRCV_RECEIVE_ONLY);
   _ptrScrnEventDispRaised      = new DisposableRaisedMsg(Callback<ProcedureStatus>(this, &ProcedureStatus::disposable_raised), MessageBase::SNDRCV_RECEIVE_ONLY);
   _msgPtrCobeConfigStruct      = new Message<GUI_COBE_CONFIG_STRUCT>(Callback<ProcedureStatus>(this, &ProcedureStatus::cobeConfigStructNotify), MessageBase::SNDRCV_RECEIVE_ONLY);

//   _periodic_timer = new TimerMessage(500, Callback<ProcedureStatus>(this, &ProcedureStatus::periodic_update));
}

void ProcedureStatus::attach (void* obj, unsigned int min_proc_status_interval_msec)
{
   if ( _attached_obj )
   {
      detach(_attached_obj);
   }

   _attached_obj                  = obj;
   _min_proc_status_interval_msec = min_proc_status_interval_msec;

   if ( !_run_complete_msg )
   {
      _run_complete_msg = new RunCompleteMsg(Callback<ProcedureStatus>(this, &ProcedureStatus::run_complete_notify), MessageBase::SNDRCV_RECEIVE_ONLY);
   }

   if ( !_proc_status_msg )
   {
      _proc_status_msg = new Message<CURRENT_PROCEDURE_STATUS_STRUCT>(Callback<ProcedureStatus>(this, &ProcedureStatus::proc_status_notify), MessageBase::SNDRCV_RECEIVE_ONLY);
   }

   if (!_msgPredictStatus)
   {
      _msgPredictStatus = new  ProcToGuiPredictStatusMsg(Callback<ProcedureStatus>(this, &ProcedureStatus::predictRequestProcess), MessageBase::SNDRCV_RECEIVE_ONLY);
   }

   if ( !_system_state_msg )
   {
      _system_state_msg = new SystemStateChangeMsg(Callback<ProcedureStatus>(this, &ProcedureStatus::system_state_notify), MessageBase::SNDRCV_RECEIVE_ONLY);
   }

   if ( !_proc_substate_msg )
   {
      _proc_substate_msg = new ProcSubstateChangeMsg(Callback<ProcedureStatus>(this, &ProcedureStatus::system_substate_notify), MessageBase::SNDRCV_RECEIVE_ONLY);
   }

   if ( !_recovery_status_msg )
   {
      _recovery_status_msg = new Message<RecoveryStatus>(Callback<ProcedureStatus>(this, &ProcedureStatus::recovery_status_notify), MessageBase::SNDRCV_RECEIVE_ONLY);
   }

   if ( !_msgPtrCobeConfigStruct )
   {
      _msgPtrCobeConfigStruct = new Message<GUI_COBE_CONFIG_STRUCT>(Callback<ProcedureStatus>(this, &ProcedureStatus::cobeConfigStructNotify), MessageBase::SNDRCV_RECEIVE_ONLY);
   }

   if ( !_aps_decoupled_pause_msg )
   {
      _aps_decoupled_pause_msg = new APSDecoupledPauseMsg(Callback<ProcedureStatus>(this, &ProcedureStatus::aps_decoupled_pause_notify), MessageBase::SNDRCV_RECEIVE_ONLY);
   }

   _current_data.system_state    = guiglobs::screen_status.get_current_system_state();
   _current_data.system_substate = guiglobs::screen_status.get_current_substate();

   _last_system_state            = (State_names) - 1;
   _last_system_substate         = (State_names) - 1;
   _notified_run_complete        = false;
}

void ProcedureStatus::detach (void* obj)
{
   if ( obj == _attached_obj )
   {
      CallbackBase null_callback;

      _system_state_callback    = null_callback;
      _system_substate_callback = null_callback;
      _proc_status_callback     = null_callback;
      _run_complete_callback    = null_callback;
      _pressure_callback        = null_callback;
//      _procedure_information_callback = null_callback;
//      _predict_screen_waiting = false;

      _access_pressure_low_warning_enabled = false;
      _bitmap_squeeze = NULL;

      _attached_obj   = NULL;
   }
}

void ProcedureStatus::set_system_state_callback (const CallbackBase& callback)
{
   _system_state_callback = callback;
   _last_system_state     = (State_names) - 1;
}

void ProcedureStatus::set_system_substate_callback (const CallbackBase& callback)
{
   _system_substate_callback = callback;
   _last_system_substate     = (State_names) - 1;
}

void ProcedureStatus::set_proc_status_callback (const CallbackBase& callback)
{
   _proc_status_callback     = callback;
   _force_proc_status_update = true;
}

void ProcedureStatus::set_run_complete_callback (const CallbackBase& callback)
{
   _run_complete_callback = callback;
   _notified_run_complete = false;
}

void ProcedureStatus::set_pressure_callback (const CallbackBase& callback)
{
   _pressure_callback = callback;
}

void ProcedureStatus::enable_access_pressure_low_warning (bool state)
{
   #if (CPU != SIMNT)
   _access_pressure_low_warning_enabled = state;
   #endif
}

void ProcedureStatus::set_access_pressure_low_bitmap (Bitmap& bitmap_squeeze)
{
   _bitmap_squeeze = &bitmap_squeeze;
}

void ProcedureStatus::system_state_notify ()
{
   _current_data.system_state = (State_names)_system_state_msg->getData();
   _system_state_callback();
   _last_system_state         = _current_data.system_state;

   if ( !_rinseback_sounded &&
        _current_data.system_state == BLOOD_RINSEBACK )
   {
      // Display rinseback in progress message
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatRinsebackInProgress);

      // call sound method to create "beep" to signify end of run
      guiglobs::button_audio_feedback.create_feedback_sound(RINSE_BACK_SOUND, SOUND_IMMEDIATE);
      _rinseback_sounded = true;
   }

   check_status_line();
   if ( _access_pressure_low_warning_enabled ) check_access_pressure();
}

void ProcedureStatus::system_substate_notify ()
{
   _current_data.system_substate = (State_names)_proc_substate_msg->getData();
   _system_substate_callback();
   _last_system_substate         = _current_data.system_substate;
}

void ProcedureStatus::proc_status_notify ()
{
   if ( _attached_obj )
   {
      const CURRENT_PROCEDURE_STATUS_STRUCT& status = _proc_status_msg->getData();

      if ( _access_pressure_low_warning_enabled ||
           _pressure_callback.assigned() )
      {
         _current_data.pressure           = (int)status.current_inlet_pressure;
         _current_data.inlet_pressure_max = (int)status.current_inlet_pressure_max;
         _current_data.inlet_pressure_min = (int)status.current_inlet_pressure_min;
         _current_data.use_stick_meter    = (status.use_startup_pressure_meter == 1);

         _pressure_callback();
         if ( _access_pressure_low_warning_enabled ) check_access_pressure();
      }

      _current_data.in_alarm_condition = status.alarm_condition;
      _current_data.return_pump        = status.return_pump_cycle;
      check_status_line();

      if ( _last_system_state != _current_data.system_state )
      {
         _system_state_callback();
         _last_system_state = _current_data.system_state;
      }

      if ( _last_system_substate != _current_data.system_substate )
      {
         _system_substate_callback();
         _last_system_substate = _current_data.system_substate;
      }

      if ( !_notified_run_complete &&
           _current_data.run_complete )
      {
         _run_complete_callback();
         _notified_run_complete = true;
      }

      if ( _proc_status_callback.assigned()
           #if CPU!=SIMNT
           && ( _force_proc_status_update ||
                osTime::howLongMilliSec(_last_proc_status_update) >= _min_proc_status_interval_msec )
           #endif // #if CPU!=SIMNT
           )
      {
         osTime::snapshotRawTime(_last_proc_status_update);
         _force_proc_status_update = false;

         // Round them all!
         _current_data.estimated_platelets = (int)(status.estimated_platelets * 10 + 0.5);
         _current_data.estimated_plasma    = (int)(status.estimated_plasma + 0.5);
         _current_data.collected_platelets = (int)(status.current_platelets_collected * 10 + 0.5);
         _current_data.collected_plasma    = (int)(status.current_plasma_collected + 0.5);

         DisplayFormatter::CalculateRbcs(guiglobs::cassette_mgr->currentSetIsRas(),
                                         guiglobs::cassette_mgr->currentSetIsDrbc(),
                                         guiglobs::cdsData.config.isPromptedDrbcSplit(guiglobs::cdsData.targets.ProcNumber.Get()),
                                         status.current_rbc_collected,
                                         status.estimated_rbcs,
                                         status.current_rbcs_prod1,
                                         status.current_rbcs_prod2,
                                         _current_data.collected_rbcs,
                                         _current_data.estimated_rbcs);

         _proc_status_callback();
      }
   }

//   With the PredictManager taking some of the responsibilities away from
//   Procedurestatus, we don't need to log this any more...
//   else DataLog (log_level_gui_error) << "Error: no attached objects." << endmsg;
}

// IT 10985
void ProcedureStatus::predictRequestProcess ()
{
   const ProcToGuiPredictStatus_t predict_status = (ProcToGuiPredictStatus_t)(_msgPredictStatus->getData());

   DataLog (log_level_gui_info) << "Recieved predict status " << predict_status << endmsg;

   if ((predict_status == REQUEST_SELECTION_SCREEN) && (_current_data.system_state == BLOOD_RUN))
   {
//		_procedure_information_available = false;
//		_repredict_in_progress = true;
   }
}


void ProcedureStatus::procedure_show_stat_line (void* data)
{


   int msgNum = (int)_statLineForSilentAlarms_msg->getData();
   DataLog(log_level_gui_info) << "Msg for stat line for silent alarms :" << msgNum << endmsg;

   if (msgNum==1)
   {
      DataLog(log_level_gui_info) << "Procedure show stat line for silent alarms :" << msgNum << endmsg;
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatRecoveryProcedure);
   }
   else
   {
      DataLog(log_level_gui_info) << "Procedure hide stat line for silent alarms :" << msgNum << endmsg;
      guiglobs::apheresis_status_line->deactivate_status_line_text_type(textStatRecoveryProcedure, ATTENTION_STATUS_LINE);
   }


}

void ProcedureStatus::procedure_rinseback_or_cancel ()
{
   DataLog(log_level_gui_info) << "Procedure rinseback/cancel - disabling reprediction" << endmsg;

   _rinseback_or_cancel = true;
//   _predict_pause_active = false;
}

// void ProcedureStatus::periodic_update()
// {
//    if ( _predict_screen_waiting &&
//         !_rinseback_or_cancel )
//       {
//          int delay_ms = osTime::howLongMilliSec(_predict_screen_wait_time);

//          if ( delay_ms > 10000 ||
//               (!_repredict_in_progress && delay_ms > 600) )
//             {
//                GuiToProcPredictRequestMsg msg(MessageBase::SEND_GLOBAL);

//                if (guiglobs::amapPlatelets())
//                   {

//                      // during run dont give optimal
//                      if (guiglobs::proc_status.get_data().system_state != BLOOD_RUN)
//                         {
//                            DataLog(log_level_gui_info) << "Sending REPREDICT_ALL_OPTIMAL from proc_status! " << endmsg;
//                            msg.send(REPREDICT_ALL_OPTIMAL);
//                         }
//                      else          {
//                         DataLog(log_level_gui_info) << "Sending REPREDICT_ALL from proc_status! " << endmsg;
//                         msg.send(REPREDICT_ALL);
//                      }
//                   }
//                else
//                   {
//                      DataLog(log_level_gui_info) << "Sending REPREDICT_ALL from proc_status! " << endmsg;
//                      msg.send(REPREDICT_ALL);
//                   }


//                //msg.send(REPREDICT_ALL);

//                DataLog(log_level_gui_info) << "Predict screen waiting for data - force reprediction after "
//                << delay_ms << " ms." << endmsg;
//                osTime::snapshotRawTime(_predict_screen_wait_time);
//             }
//       }
// }

void ProcedureStatus::run_complete_notify ()
{
   if ( !_current_data.run_complete )
   {
      // call sound method to create "beep" to signify end of run
      guiglobs::button_audio_feedback.create_feedback_sound (END_PROCEDURE_SOUND, SOUND_IMMEDIATE);

      // if we are current displaying run summary data, force system
      // back to runproc screen
      if ( guiglobs::run_summary_display_active )
      {
         Base_Apheresis_Window::goto_screen(GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
      }
   }

   _current_data.run_complete = true;
   _run_complete_callback();
   _notified_run_complete     = true;

   if ( _access_pressure_low_warning_enabled ) check_access_pressure();
}

void ProcedureStatus::check_access_pressure ()
{
   bool check_valid = false;
   if ( !_current_data.run_complete &&
        ( _current_data.system_state == BLOOD_PRIME || _current_data.system_state == BLOOD_RUN ))
   {
      check_valid = true;
   }

   if ( check_valid &&
        _current_data.pressure <= (_current_data.inlet_pressure_min * MINIMUM_PRESSURE_SCALING_FACTOR) )
   {
      osTime::snapshotRawTime(_access_pressure_low_time);

      if ( !_access_pressure_low )
      {
         _access_pressure_low = true;

         // Set status line to access pressure low
         guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ALERT_STATUS_LINE, textStatApsLow);

         // call sound method to create "beep" to signify squeeze
         guiglobs::button_audio_feedback.create_feedback_sound(SQUEEZE_SOUND, SOUND_IMMEDIATE);

         // set the squeeze bitmap
         if ( _bitmap_squeeze ) _bitmap_squeeze->set_visible(true);

         // log the access pressure low event
         DataLog(log_level_gui_info) << "RUNPROC LOG: access pressure low warning"
                                     << " pressure=" << _current_data.pressure
                                     << " min pressure=" << _current_data.inlet_pressure_min
                                     << endmsg;
      }
   }
   else if ( _access_pressure_low &&
             ( _current_data.run_complete ||
               osTime::howLongMilliSec(_access_pressure_low_time) >= 2000 ))
   {
      _access_pressure_low = false;

      // remove the squeeze bitmap
      if ( _bitmap_squeeze ) _bitmap_squeeze->set_visible(false);

      // log the access pressure low event
      DataLog(log_level_gui_info) << "RUNPROC LOG: access pressure low warning cleared" << endmsg;
   }

   if ( _aps_decoupled_alert )
   {
      if ( guiglobs::apheresis_status_line->get_current_status_line_text_by_type(ALERT_STATUS_LINE) != textStatAlarmApsDecoupling )
      {
         // Set status line to access pressure low
         guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ALERT_STATUS_LINE, textStatAlarmApsDecoupling);
      }
   }
   else if ( !_access_pressure_low )
   {
      // Set the status line to reflect no alarm condition
      if (    guiglobs::apheresis_status_line->get_current_status_line_text_by_type(ALERT_STATUS_LINE) == textStatApsLow
              || guiglobs::apheresis_status_line->get_current_status_line_text_by_type(ALERT_STATUS_LINE) == textStatAlarmApsDecoupling )
      {
         guiglobs::apheresis_status_line->deactivate_status_line_type (ALERT_STATUS_LINE);
      }
   }

}

void ProcedureStatus::check_status_line ()
{
   TEXT_STRING_ID   new_status_line_text = _last_status_line_text;
   STATUS_LINE_TYPE new_status_line_type = _last_status_line_type;

   switch ( _current_data.system_state )
   {
      case BLOOD_PRIME :
         new_status_line_text = textStatBloodPrimeInProgress;
         new_status_line_type = DEFAULT_STATUS_LINE;
         break;

      case BLOOD_RUN :
         if ((_current_data.recovery_substate == RecoveryTypes::WBCChamberPurge ||
              _current_data.recovery_substate == RecoveryTypes::RBCSpillover) &&
             guiglobs::cdsData.run.PurgeDueToChamberSat.Get())
         {
            // Stat Line: "Recovery in progress, WBCs detected"
            new_status_line_text = textStatUnscheduledPurge;
         }
         else if ((_current_data.recovery_substate == RecoveryTypes::WBCChamberPurge ||
                   _current_data.recovery_substate == RecoveryTypes::ClearLine ||
                   _current_data.recovery_substate == RecoveryTypes::SettleChannel) &&
                  (_current_data.recovery_status == RecoveryTypes::CountTime ||
                   _current_data.recovery_status == RecoveryTypes::CountVolume ||
                   _current_data.recovery_status == RecoveryTypes::CountAll))
         {
            // Stat Line: "Clearing LRS chamber."
            new_status_line_text = textStatLrsPurge;
         }
         else if (_current_data.recovery_substate == RecoveryTypes::SalinePrime)
         {
            // Stat Line: "Priming replacement solution line"
            new_status_line_text = textStatSalinePrime;
         }
         else if (    _current_data.recovery_substate != RecoveryTypes::None
                      && _current_data.recovery_substate != RecoveryTypes::PlasmaValveMotion )
         {
            // Stat Line: "Recovery procedure in progress"
            new_status_line_text = textStatRecoveryProcedure;
         }
         else if ( _current_data.system_substate == SS_EXTENDED_PCA )
         {
            // Stat Line: "Adjusting platelet concentration"
            new_status_line_text = textStatAdjustingPltConc;
         }
         else if ( _current_data.return_pump == RETURN_PUMP_CYCLE_DRAW )
         {
            // Stat Line: "Draw in progress"
            new_status_line_text = textStatReturnPumpStatusDraw;
         }
         else
         {
            // Stat Line: "Return in progress"
            new_status_line_text = textStatReturnPumpStatusReturn;
         }
         new_status_line_type = DEFAULT_STATUS_LINE;

         break;

      case BLOOD_RINSEBACK :
         // clear the default status line of draw/return text since we're done with blood run
         if ( _last_status_line_text == textStatReturnPumpStatusDraw ||
              _last_status_line_text == textStatReturnPumpStatusReturn )
         {
            guiglobs::apheresis_status_line->deactivate_status_line_type(DEFAULT_STATUS_LINE);
         }

         if (!_current_data.run_complete)
         {
            new_status_line_text = textStatRinsebackInProgress;
            new_status_line_type = ATTENTION_STATUS_LINE;
         }
         break;

      default :
         new_status_line_text = textEmpty;
         new_status_line_type = DEFAULT_STATUS_LINE;
         break;
   }

   if ( _current_data.in_alarm_condition )
   {
      new_status_line_text = textEmpty;
   }

   if ( _last_status_line_text != new_status_line_text ||
        _last_status_line_type != new_status_line_type )
   {
      if ( new_status_line_text == textEmpty )
      {
         guiglobs::apheresis_status_line->deactivate_status_line_type(_last_status_line_type);
      }
      else if ( _last_status_line_text != new_status_line_text &&
                new_status_line_type == DEFAULT_STATUS_LINE)
      {
         guiglobs::apheresis_status_line->set_current_text(new_status_line_text);
      }
      else
      {
         guiglobs::apheresis_status_line->set_current_status_line_text_by_type(new_status_line_type, new_status_line_text);
      }
      _last_status_line_text = new_status_line_text;
      _last_status_line_type = new_status_line_type;
   }

}

// void ProcedureStatus::procedure_information_notify()
// {
//    _procedure_information_available = true;
//    _predict_screen_waiting = false;
//    _repredict_in_progress = false;

//    // if ( _procedure_information_callback.assigned() &&
//    //          !_rinseback_or_cancel )
//    // {
//    //    _procedure_information_callback();
//    // }

//    if (    guiglobs::apheresis_status_line->get_current_status_line_type() != ATTENTION_STATUS_LINE
//         || guiglobs::apheresis_status_line->get_current_status_line_text_by_type(ATTENTION_STATUS_LINE) == textStatOptInProgress )
//    {
//       // Remove optimization in progress message
//       guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
//    }
// }

void ProcedureStatus::recovery_status_notify ()
{
   if ( _recovery_status_msg )
   {
      _current_data.recovery_substate = _recovery_status_msg->getData()._Type;
      _current_data.recovery_status   = _recovery_status_msg->getData()._InRecovery;
   }
}

void ProcedureStatus::cobeConfigStructNotify ()
{
   const GUI_COBE_CONFIG_STRUCT& cobeConfigStruct = _msgPtrCobeConfigStruct->getData();

   guiglobs::concWbcFlagThreshold  = cobeConfigStruct.pltWbcThresholdConc;
   guiglobs::PtfFilterLoss         = cobeConfigStruct.PtfFilterLoss;
   guiglobs::ptfFilterVol          = cobeConfigStruct.ptfFilterVol;

   guiglobs::rasBagEmptyPercentage = cobeConfigStruct.rasBagEmptyPercent;
   guiglobs::pasBagEmptyPercentage = cobeConfigStruct.pasBagEmptyPercent;

   guiglobs::flowAdjMin            = cobeConfigStruct.flowRateMin;
   guiglobs::flowAdjMax            = cobeConfigStruct.flowRateMax;

   guiglobs::pasPrimeVol           = cobeConfigStruct.pasPrimeVol;

   guiglobs::pasVolBiasPercent     = cobeConfigStruct.pasVolBiasPercent;
   guiglobs::pasNoRinsebackPenalty = cobeConfigStruct.pasNoRinsebackPenalty;

   guiglobs::minPlsRinseVol        = cobeConfigStruct.minPlsRinseVol;

   DataLog (log_level_gui_info) << "GUI recieved cobe config struct (concWbcFlagThreshold="
                                << guiglobs::concWbcFlagThreshold << "; PtfFilterLoss=" << guiglobs::PtfFilterLoss
                                << "; ptfFilterVol=" << guiglobs::ptfFilterVol
                                << "; rasBagEmptyPercent=" << guiglobs::rasBagEmptyPercentage
                                << "; pasBagEmptyPercent=" << guiglobs::pasBagEmptyPercentage
                                << "; flowAdjMin=" << guiglobs::flowAdjMin
                                << "; flowAdjMax=" << guiglobs::flowAdjMax
                                << "; pasPrimeVol=" << guiglobs::pasPrimeVol
                                << "; pasVolBiasPercent=" << guiglobs::pasVolBiasPercent
                                << "; pasNoRinsebackPenalty=" << guiglobs::pasNoRinsebackPenalty
                                << ");" << endmsg;
}

void ProcedureStatus::aps_decoupled_pause_notify ()
{
   if ( _aps_decoupled_pause_msg )
   {
      _aps_decoupled_alert = (bool)_aps_decoupled_pause_msg->getData();
   }
}

// void ProcedureStatus::procedure_information_request_notify()
// {
//    DataLog(log_level_gui_info) << "procedure_information_request_notify: " << (int)_procedure_information_request_msg->getData() << endmsg;
//    if ( _procedure_information_request_msg->getData() == REPREDICT_ALL ||
//              _procedure_information_request_msg->getData() == REPREDICT_ALL_OPTIMAL)
//    {
//       // if we see a re-predict request message, set info available to false so
//       // we'll be sure to notify only when new data comes in
//       _procedure_information_available = false;
//       _repredict_in_progress = true;
//    }
//    // else if ( _procedure_information_request_msg->getData() == SELECT_CURRENT )
//    // {
//    //    // if we see a select message, we can assume that the prediction pause
//    //    // is about to be cleared
//    //    _predict_pause_active = false;
//    // }
// }

// void ProcedureStatus::procedure_alarm_monitor()
// {
//    Alarm_struct data = _alarm_msg->getData();
//    if ( data.alarm_name == PREDICTION_PAUSE)
//    {
// //      _predict_pause_active = ( !_rinseback_or_cancel && (data.alarm_state != CLEARED) );
//    }
// }

// void ProcedureStatus::set_procedure_information_callback(const CallbackBase & callback)
// {
//    _procedure_information_callback = callback;

//    if ( _procedure_information_callback.assigned() )
//    {
// //      osTime::snapshotRawTime(_predict_screen_wait_time);
//       _predict_screen_waiting = true;

//       if ( _procedure_information_available )
//       {
//                      if ( !_rinseback_or_cancel )
//                      {
//                              _procedure_information_callback();
//                      }
//       }
//              else if ( !_repredict_in_progress )
//              {
//          DataLog(log_level_gui_info) << "predict screen displayed - waiting for request to predict" << endmsg;
//              }
//       else
//       {
//          DataLog(log_level_gui_info) << "predict screen displayed - waiting for response from predict" << endmsg;
//       }
//    }
// }


void ProcedureStatus::refresh_status_line ()
{
   // Only change the status line if the attached screen is the owning screen for the status line.
   if (guiglobs::apheresis_status_line->get_owning_screen() == ((Base_Apheresis_Window*)_attached_obj)->get_screen_id())
   {
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type(_last_status_line_type, _last_status_line_text);
   }
}

////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER

void ProcedureStatus::disposable_raised ()
{
   // Reset the status line message to null.
   guiglobs::apheresis_status_line->set_current_text (textEmpty);
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   guiglobs::cassette_mgr->clearCurrent();

   // Acceptable screens after raising disposable set are cassette load screen
   // and two-button screen.  If you're not on one of those two screens, then
   // we should make a transition.
   GUI_SCREEN_ID currentScreen = guiglobs::screen_status.get_current_screen();


   if (currentScreen == GUI_SCREEN_SYSCASSETTE ||
       currentScreen == GUI_SCREEN_SYSDONOR)
   {
      DataLog (log_level_gui_info) << "Screen " << currentScreen
                                   << " acceptable for raised set." << endmsg;

      if (guiglobs::airToDonorMitigationEnabled())
      {
         Base_Apheresis_Window::goto_screen (GUI_SCREEN_SYSCASSETTE, "GUI_SCREEN_SYSCASSETTE");
      }

   }
   else
   {
      if (   currentScreen == GUI_SCREEN_SYSCLAMP
             || currentScreen == GUI_SCREEN_RBC_SPLIT_INIT
             || currentScreen == GUI_SCREEN_PAS_INIT_SPLIT
             || currentScreen == GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG
             )
      {
         DataLog (log_level_gui_info) << "Screen " << currentScreen
                                      << " unacceptable for raised set. Go to " << GUI_SCREEN_SYSCASSETTE << endmsg;

         Base_Apheresis_Window::goto_screen (GUI_SCREEN_SYSCASSETTE, "GUI_SCREEN_SYSCASSETTE");
      }
      else
      {
         DataLog (log_level_gui_info) << "Screen " << currentScreen
                                      << " unacceptable for raised set. Go to " << GUI_SCREEN_SYSDONOR << endmsg;

         Base_Apheresis_Window::goto_screen (GUI_SCREEN_SYSDONOR, "GUI_SCREEN_SYSDONOR");
      }
   }
}     // END of Screen_SYSCLAMP SCREEN_NOTIFY_HANDLER


/////////////////////////////////////////////////////////////////////////////
// RINSEBACK_IS_AN_OPTION
bool ProcedureStatus::rinsebackIsAnOption (ALARM_VALUES alrmVal = NULL_ALARM)
{
   bool firstDrawComplete =
          #if CPU==SIMNT
      true;     // In the simulator, make sure this is always true so that rinseback buttons show up, etc.
          #else
      guiglobs::cdsData.run.FirstDrawComplete.Get();
          #endif

   // Power failure special case...
   if (((alrmVal == POWER_FAIL_CONTINUE) || (alrmVal == POWER_FAIL_RINSEBACK)) &&
       ((guiglobs::cdsData.set.CassetteState.Get() == ProcTrimaSet_CDS::PRIMED) &&
        firstDrawComplete))
   {
      DataLog (log_level_gui_info) << "Display rinseback button on PFR screen. (alrmVal="
                                   << alrmVal << "; firstDraw=" << firstDrawComplete << ")";
      return true;
   }

   // Normal case...
   return firstDrawComplete &&
          (guiglobs::screen_status.get_current_system_state() == BLOOD_RUN);
}

/* FORMAT HASH 8996dfd68e1c490bd9eeab5b83a98355 */

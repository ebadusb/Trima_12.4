/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef _PROCEDURE_STATUS_INCLUDE
#define _PROCEDURE_STATUS_INCLUDE

#include "bitmap.h"
#include "callback.h"
#include "guiproc.h"
#include "ostime.hpp"
#include "predict_msgs.h"
#include "recoverytypes.h"
#include "timermessage.h"
#include "trimamessages.h"

//
// The procedure status manager handles the RunCompleteMsg and the CURRENT_PROCEDURE_STATUS_STRUCT
// messages from proc.  Callbacks are provided so that screens can link to the manager to be informed
// of message reception.  An option is also provided to limit the frequency of notification for the
// CURRENT_PROCEDURE_STATUS_STRUCT message.
//
class ProcedureStatus
{
public:
   struct ProcSnapshot
   {
      State_names               system_state;      // system state (e.g. BLOOD_RUN)
      State_names               system_substate;   // system substate (e.g. SS_EXTENDED_PCA)
      RecoveryTypes::Type       recovery_substate; // Current recovery and status to
      RecoveryTypes::InRecovery recovery_status;   // be able to determine the proper line
      bool in_alarm_condition;                     // proc's alarm status
      int  return_pump;                            // return pump cycle (e.g. RETURN_PUMP_CYCLE_DRAW)

      int  estimated_platelets;     // estimated platelet target (x 1E10)
      int  estimated_plasma;        // estimated plasma target (ml)
      int  estimated_rbcs;          // estimated rbc target (ml)

      int  collected_platelets;     // collected platelets (x 1E10)
      int  collected_plasma;        // collected plasma (ml)
      int  collected_rbcs;          // collected rbcs (ml)

      int  pressure;                // access pressure (mm Hg)
      int  inlet_pressure_min;      // meter limits for access pressure (mm Hg)
      int  inlet_pressure_max;
      bool use_stick_meter;         // true if stick (startup) pressure meter should be used

      bool run_complete;
   };

private:
   RunCompleteMsg* _run_complete_msg;
   Message<CURRENT_PROCEDURE_STATUS_STRUCT>* _proc_status_msg;
   SystemStateChangeMsg*                     _system_state_msg;
   ProcSubstateChangeMsg*                    _proc_substate_msg;
   Message<RecoveryStatus>*                  _recovery_status_msg;
   APSDecoupledPauseMsg*                     _aps_decoupled_pause_msg;
//   Message<Alarm_struct>   * _alarm_msg;
   DisposableRaisedMsg*                      _ptrScrnEventDispRaised ;
   Message<GUI_COBE_CONFIG_STRUCT>*          _msgPtrCobeConfigStruct;

   RinsebackRequestMsg*                      _rinseback_request_msg;
   CancelProcedureMsg*                       _cancel_procedure_msg;
   StatLineRequestMsg*                       _statLineForSilentAlarms_msg;

   ProcSnapshot     _current_data;

   void*            _attached_obj;
   int              _min_proc_status_interval_msec;
   rawTime          _last_proc_status_update;
   bool             _force_proc_status_update;

   CallbackBase     _system_state_callback;
   CallbackBase     _system_substate_callback;
   CallbackBase     _proc_status_callback;
   CallbackBase     _run_complete_callback;
   CallbackBase     _pressure_callback;

   bool             _access_pressure_low_warning_enabled;
   bool             _access_pressure_low;
   bool             _aps_decoupled_alert;
   rawTime          _access_pressure_low_time;
   Bitmap*          _bitmap_squeeze;

   bool             _rinseback_sounded;

   TEXT_STRING_ID   _last_status_line_text;
   STATUS_LINE_TYPE _last_status_line_type;
   State_names      _last_system_state;
   State_names      _last_system_substate;
   bool             _notified_run_complete;


//   GuiToProcPredictRequestMsg * _procedure_information_request_msg;
   ProcToGuiPredictStatusMsg* _msgPredictStatus;
//   bool  _procedure_information_available;
//   bool  _predict_screen_waiting;
//   bool  _repredict_in_progress;
//   rawTime _predict_screen_wait_time;
//   bool  _predict_pause_active;
   int  _predict_screen_type;
   // CallbackBase _procedure_information_callback;
   bool _rinseback_or_cancel;

//   TimerMessage * _periodic_timer;

private:

   ProcedureStatus            (const ProcedureStatus&);  // not defined -- noncopyable
   ProcedureStatus& operator = (const ProcedureStatus&); // not defined -- nonassignable

   void proc_status_notify ();
   void predictRequestProcess ();
//   void periodic_update();
   void disposable_raised ();

   //
   // If a screen registers for run complete notifications, and the run is complete
   // at the time of registration, the callback will be performed when the first
   // procedure status is received.
   //
   void run_complete_notify ();

   //
   // If a screen registers for the following notifications, it is guaranteed to
   // get at least one callback for the current state.  Further callbacks will be
   // made only if a change is detected.
   //
   void system_state_notify ();
   void system_substate_notify ();

//   void procedure_information_notify();
//   void procedure_information_request_notify();
//   void procedure_alarm_monitor();

   void procedure_rinseback_or_cancel ();
   void procedure_show_stat_line (void* data);

   void check_access_pressure ();
   void check_status_line ();

   void recovery_status_notify ();
   void aps_decoupled_pause_notify ();
   void cobeConfigStructNotify ();

public:
   ProcedureStatus();
   virtual ~ProcedureStatus();

   void initialize ();

   void                       attach (void* obj, unsigned int min_proc_status_interval_msec);
   void                       detach (void* obj);
   inline const ProcSnapshot& get_data () { return _current_data; }

   void set_system_state_callback (const CallbackBase& callback);
   void set_system_substate_callback (const CallbackBase& callback);
   void set_proc_status_callback (const CallbackBase& callback);
   void set_run_complete_callback (const CallbackBase& callback);
   void set_pressure_callback (const CallbackBase& callback);
//   void set_procedure_information_callback(const CallbackBase & callback);

   void enable_access_pressure_low_warning (bool state);
   void set_access_pressure_low_bitmap (Bitmap& bitmap_squeeze);

   void enable_status_line (bool state);

   // const ProcedureInformationStruct & get_procedure_information();

//   inline bool predict_pause_active() { return _predict_pause_active; }
   inline void set_last_predict_screen_type (int type) { _predict_screen_type = type; }
   inline int  get_last_predict_screen_type () { return _predict_screen_type; }

   void refresh_status_line ();

   bool rinsebackIsAnOption (ALARM_VALUES alrmVal = NULL_ALARM);
};

#endif /* ifndef _PROCEDURE_STATUS_INCLUDE */

/* FORMAT HASH f5af73d0f30ef1b4576f7291e53ca886 */

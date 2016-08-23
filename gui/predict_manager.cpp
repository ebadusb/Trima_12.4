#include "predict_manager.hpp"
#include "predict_wait_state.hpp"

#include "an_alarm.h"
#include "aphwind.hpp"
#include "states.h"
#include "trimamessages.h"

//////////////////////////////////////////////////////////////////////
//
//  Predict Manager Implementation:
//
//  We stash everything into the detail namespace.  The free functions declared in the header access
//  this stuff directly.

namespace PredictManager
{

namespace detail
{

using namespace manager_detail;

using namespace PredictWaitStates;


// wait states.  These could probably be cleaned up (e.g. use a union, and just
// switch what's actually in it).

SelectCurrent       curr_wait       ("SelectCurr");
StatusAndProcedures flow_wait       ("FlowAdjustment", FLOW_ADJUSTMENT_ACK);
StatusAndProcedures donor_time_wait ("DonorTime", DONOR_TIME_ACK);
StatusOrProcedures  valid_wait      ("ProcedureVALID", IS_PROCEDURE_VALID_ACK);
StatusAndProcedures all_wait        ("RepredictAll", REPREDICT_ALL_ACK);
ProcedureUpdated    next_wait       ("SelectNext", SELECT_NEXT_ACK);
ProcedureUpdated    prev_wait       ("SelectPrev", SELECT_PREVIOUS_ACK);
ProcedureUpdated    optimal_wait    ("RepredictOptimal", REPREDICT_ALL_OPTIMAL_ACK);
DonorDataStatus     dvitals_wait    ;      // no parens(), C++ most vexing parse!


class PredictManager_Impl
{

public:

   // here's the singleton accessor
   friend PredictManager_Impl& singleton ();


   //////////////////////////////////////////////////////////////////////////////////
   // the free functions in namespace PredictManager forward directly to these member
   // functions

   void attach (Base_Apheresis_Window*);
   void detach (Base_Apheresis_Window*);


   void set_predict_status_callback (const CallbackBase&);
   void set_procedure_info_callback (const CallbackBase&);
   void set_donor_data_status_callback (const CallbackBase&);

   void set_request_complete_callback (const CallbackBase&);

   void sendPredictRequest (GuiToProcPredictRequest_t type,
                            int line, const char* fn, const char* file);
   void sendDonorTimeUpdate (int minutes);
   void sendFlowAdjustment (int flow);
   void sendDonorVitals (const SDonorVitals&, int line, const char* fn, const char* file);

   bool prediction_screen_requested () { return _prediction_screen_requested; }
   void prediction_screen_requested (bool r) { _prediction_screen_requested = r; }

   bool& no_product () { return _no_product; }

   bool procedures_are_available () { return _procedures_are_available; }
   void procedures_are_available (bool p) { _procedures_are_available = p; }

   // Returns payload of the donor status message that (generally) ends a
   // transaction; needed for the confirmDonInfo screen.  [It might be cleaner
   // to pass this value to the relevent callback, but that means adding data to
   // several callbacks... this seemed a little cleaner. --DGS]
   int donor_data_source () { return _donor_data_source; }

   bool waiting () { return _waiting; }

private:

   PredictManager_Impl();                                        // private, singleton
   PredictManager_Impl (const PredictManager_Impl&);             // noncopyable, not implemented
   PredictManager_Impl& operator = (const PredictManager_Impl&); // nonassignable, not implemented


   // callbacks
   void predict_status_notify ();      // called on ProcToGuiPredictStatusMsg
   void procedure_info_notify ();      // called on GuiProceduredUpdated
   void procedure_select_notify ();    // called on ProceudreSelectedMsg
   void donor_data_status_notify ();
   void predict_responded_notify ();   // called on ProceudreRespondedMsg
   void system_state_change_notify (); // on system state change

   void fire_procedure_information_callback (); // if nobody is listening to this
                                                // callback, we se the
                                                // _procedures_are_available flag.

   void fire_predict_status_callback (); // ditto

   void transaction_complete (const char* trans_name);

   void deactivate_callbacks ();

   string flags () const;

   void log_status (const char*) const;           // call this to log the current wait state status

private: // data

   // The screen currently talking to us
   Base_Apheresis_Window* _screen;

   // callback objects, to be set by attached screens
   CallbackBase _procedure_information_callback;
   CallbackBase _predict_status_callback;
   CallbackBase _donor_data_status_callback;
   CallbackBase _request_complete_callback;

   // Messages we subscribe to
   GuiProceduresUpdated      _procedure_information_msg; // goes to procedure_information_notify
   ProcToGuiPredictStatusMsg _predict_status_msg;        // goes to predict_status_notify
   ProcedureSelectedMsg      _procedure_selected_msg;    // goes to procedure_select_notify
   DonorDataStatusMsg        _donor_data_status_msg;     // goes to donor_data_status_notify
   PredictRespondedMsg       _predict_responded_msg;
   SystemStateChangeMsg      _system_state_change_msg;   // so we know when we transition to run

   // Outgoing messages
   GuiToProcPredictRequestMsg _guiToProcPredictRequestMsg ;
   GuiToProcDonorTimeMsg      _guiToProcDonorTimeMsg      ;
   GuiToProcFlowAdjustmentMsg _guiToProcFlowAdjustmentMsg ;
   DVitalsFromGUI             _dVitalsFromGUI             ;

   // State variables
   bool        _prediction_screen_requested;
   bool        _procedures_are_available;
   bool        _no_product;

   State_names _system_state; // current system state
   bool        _in_run;       // true when system state >= blood prime

   long        _donor_data_source;

   // And the actual wait state machine
   WaitState _waiting;

   // helper variables
   mutable stringstream buf;            // for formatting log messages

};



////////////////////////////////////////////////////////////////////////////////
//
//  Implementation
//
////////////////////////////////////////////////////////////////////////////////


// Singleton accessor function
inline PredictManager_Impl& singleton ()
{
   static PredictManager_Impl monitor;
   return monitor;
}


PredictManager_Impl::PredictManager_Impl()
   : _screen(NULL),

     _guiToProcPredictRequestMsg (MessageBase::SEND_LOCAL),
     _guiToProcDonorTimeMsg      (MessageBase::SEND_GLOBAL),
     _guiToProcFlowAdjustmentMsg (MessageBase::SEND_GLOBAL),
     _dVitalsFromGUI             (MessageBase::SEND_GLOBAL),

     _prediction_screen_requested (false),
     _procedures_are_available    (false),
     _no_product                  (false),
     _system_state                (FIRST_STATE),
     _in_run                      (false),
     _donor_data_source           (0),
     _waiting ()

{

   _procedure_information_msg.init(
      Callback<PredictManager_Impl>(this, &PredictManager_Impl::procedure_info_notify),
      MessageBase::SNDRCV_RECEIVE_ONLY);

   _predict_status_msg.init(
      Callback<PredictManager_Impl>(this, &PredictManager_Impl::predict_status_notify),
      MessageBase::SNDRCV_RECEIVE_ONLY);

   _procedure_selected_msg.init(
      Callback<PredictManager_Impl>(this, &PredictManager_Impl::procedure_select_notify),
      MessageBase::SNDRCV_RECEIVE_ONLY);

   _donor_data_status_msg.init(
      Callback<PredictManager_Impl>(this, &PredictManager_Impl::donor_data_status_notify),
      MessageBase::SNDRCV_RECEIVE_ONLY);

   _predict_responded_msg.init(
      Callback<PredictManager_Impl>(this, &PredictManager_Impl::predict_responded_notify),
      MessageBase::SNDRCV_RECEIVE_ONLY);

   _system_state_change_msg.init(
      Callback<PredictManager_Impl>(this, &PredictManager_Impl::system_state_change_notify),
      MessageBase::SNDRCV_RECEIVE_ONLY);


}


void PredictManager_Impl::attach (Base_Apheresis_Window* obj)
{

   DataLog(log_level_gui_info) << "PREDICT MANAGER, attach: new " << obj
                               << ", old " << _screen
                               << endmsg;


   if ( _screen )
   {
      if (_screen == obj)       // ... and if this is a duplicate attach from
                                // the same guy, we just log it and carry on
      {
         DataLog(log_level_gui_info) << "PREDICT MANAGER, re-attach from screen " << obj->get_screen_id ()
                                     << '[' << obj
                                     << ']' << endmsg;

         return;
      }

      DataLog(log_level_gui_info) << "PREDICT MANAGER, attach from screen " << obj->get_screen_id ()
                                  << '[' << obj
                                  << "], replacing screen "   // << _screen->get_screen_id()
                                  << '[' << _screen
                                  << "]; " << _waiting <<  endmsg;
      deactivate_callbacks();
   }
   else
      DataLog(log_level_gui_info) << "PREDICT MANAGER attach from screen " << obj->get_screen_id ()
                                  << '[' << obj
                                  << "]; " << _waiting << endmsg;

   _screen = obj;
   // log_status(__FUNCTION__);
}

void PredictManager_Impl::detach (Base_Apheresis_Window* obj)
{
   if (_screen == NULL)
   {
      // duplicate detach, perfectly fine
      return;
   }

   DataLog(log_level_gui_info) << "PREDICT MANAGER detach from screen " << obj->get_screen_id ()
                               << "; " << _waiting << flags() << endmsg;
   if ( obj != _screen)
   {
      // This is not an error state, but it's something to clean up : screen
      // B attached before A detached.
      // DataLog(log_level_gui_info) << "PREDICT MANAGER attach/detach cross" << endmsg;
      return;
   }

   deactivate_callbacks();
//   log_status(__FUNCTION__);

}

void PredictManager_Impl::deactivate_callbacks ()
{

   CallbackBase null_callback;

   _screen = NULL;
   _procedure_information_callback = null_callback;
   _predict_status_callback        = null_callback;
   _donor_data_status_callback     = null_callback;
   _request_complete_callback      = null_callback;

}

void PredictManager_Impl::set_predict_status_callback (const CallbackBase& callback)
{
   // DataLog(log_level_gui_info) << "PREDICT MANAGER predict status callback set." << endmsg;
   // log_status(__FUNCTION__);
   _predict_status_callback = callback;
}

void PredictManager_Impl::set_procedure_info_callback (const CallbackBase& callback)
{
   // DataLog(log_level_gui_info) << "PREDICT MANAGER procedure info callback set." << endmsg;
   // log_status(__FUNCTION__);
   _procedure_information_callback = callback;
}

void PredictManager_Impl::set_donor_data_status_callback (const CallbackBase& callback)
{
   // DataLog(log_level_gui_info) << "PREDICT MANAGER donor data status callback set."<< endmsg;
   // log_status(__FUNCTION__)
   _donor_data_status_callback = callback;
}


void PredictManager_Impl::set_request_complete_callback (const CallbackBase& callback)
{
   // DataLog(log_level_gui_info) << "PREDICT MANAGER request complete callback set."<< endmsg;
   // log_status(__FUNCTION__);
   _request_complete_callback = callback;
}


////////////////////////////////////////////////////////////////////////////////
// SEND MESSAGES


void PredictManager_Impl::sendPredictRequest (GuiToProcPredictRequest_t type,
                                              int line, const char* fn, const char* file)
{
   // store call site info for logging...
   buf.str("");
   buf <<  "From " << file << ":" << line << '[' << fn
       << ']';

   // TODO: if, at some point, we want to allow clients to start multiple
   // transactions, we'll need to queue them up; currently, this isn't allowed,
   // and this should never happen:

   if (_waiting)
   {
      FatalAlarm(buf, "PREDICT MANAGER ERROR: CROSSING MESSAGES");
      return;
   }

   // set up wait state.
   switch (type)
   {
      case IS_PROCEDURE_VALID    : _waiting.set(valid_wait); break;
      case SELECT_NEXT           : _waiting.set(next_wait); break;
      case SELECT_PREVIOUS       : _waiting.set(prev_wait); break;
      case SELECT_CURRENT        : _waiting.set(curr_wait); break;
      case REPREDICT_ALL         : _waiting.set(all_wait); break;
      case REPREDICT_ALL_OPTIMAL : _waiting.set(optimal_wait); break;
      default :
         FatalAlarm(buf, "unknown predict request");
         return;
   }


   DataLog(log_level_gui_info) << "PREDICT MANAGER SENDING predict request: " << type
                               << " from " << file << ":" << line << '[' << fn
                               << "] , " << _waiting << flags() << endmsg;

   _guiToProcPredictRequestMsg.send(type);
   update_status_line(_waiting);
}


void PredictManager_Impl::sendDonorTimeUpdate (int minutes)
{
   _waiting.set(donor_time_wait);

   DataLog(log_level_gui_info) <<  "PREDICT MANAGER SENDING Donor Time update (" << minutes
                               << " minutes), " << _waiting << flags() << endmsg;

   _guiToProcDonorTimeMsg.send(minutes);

   update_status_line(_waiting);
}

void PredictManager_Impl::sendFlowAdjustment (int flow)
{

   _waiting.set(flow_wait);

   DataLog(log_level_gui_info) << "PREDICT MANAGER SENDING Flow adjustment: " << flow
                               << ", " << _waiting << flags() << endmsg;

   _guiToProcFlowAdjustmentMsg.send(flow);
   update_status_line(_waiting);
}


void PredictManager_Impl::sendDonorVitals (const SDonorVitals& vitals,
                                           int line, const char* fn, const char* file)
{
   _waiting.set(dvitals_wait);


   DataLog(log_level_gui_info) << "PREDICT MANAGER SENDING Donor Vitals adjustment, "
                               << _waiting
                               << " from " << file << ":" << line << '[' << fn << "] "
                               << ", sending data: " << vitals << endmsg;

   _dVitalsFromGUI.send(vitals);
   update_status_line(_waiting);
}


// CALLBACKS

// reset
void PredictManager_Impl::transaction_complete (const char* transaction_name)
{
   DataLog(log_level_gui_info) << "PREDICT MANAGER: " << transaction_name
                               << " transaction complete"
                               << flags()
                               << endmsg;
   _waiting.reset();

   if (_request_complete_callback.assigned() )
      _request_complete_callback();
   else
      DataLog(log_level_gui_info) << "PREDICT MANAGER: ... but no callback set." << endmsg;

}


void PredictManager_Impl::fire_procedure_information_callback ()
{
   // We want to fire the procedure_information callback, but if it isn't
   // assigned, we need to set the flag
   if (_procedure_information_callback.assigned())
   {
      // DataLog(log_level_gui_info) << "PREDICT MANAGER: firing GuiProceduresUpdated callback."
      //                             << endmsg;
      _procedure_information_callback();
   }
   else
   {
      // DataLog(log_level_gui_info) << "PREDICT MANAGER: NO CALLBACK for GuiProceduresUpdated,"
      //                             << " setting 'procedures_are_available'" << endmsg;
      _procedures_are_available = true;
   }
}

void PredictManager_Impl::procedure_info_notify ()
{
   int  payload = _procedure_information_msg.getData();

   bool used    = _waiting.event(GUI_PROCEDURE_UPDATED, payload);

   DataLog(interesting(used)) << "PREDICT MANAGER GOT GuiProceduresUpdated with payload "
                              << GuiProceduresUpdated_t(payload)
                              << ", " << _waiting << endmsg;


   update_status_line(_waiting);
   // log_status(__FUNCTION__);

   if (!used)  // if we didn't use this event in a state transition
   {
      // then we need to fire the specific allback
      DataLog(log_level_gui_debug) << "PREDICT MANAGER: message not used, so firing callback directly" << endmsg;
      fire_procedure_information_callback();
      return;
   }


   if (!_request_complete_callback.assigned())  // whether we used it or not, if
                                                // we don't have a
                                                // request_complete callback,
   {
      // as above, with different logging, and we don't exit
      DataLog(log_level_gui_debug) << "PREDICT MANAGER: nobody's listening for a completion, "
                                   << "so firing callback directly" << endmsg;
      fire_procedure_information_callback();
   }



   // we used the message; if we're no longer waiting, fire that callback
   if (!_waiting)
      transaction_complete("");
}


void PredictManager_Impl::predict_status_notify ()
{

   ProcToGuiPredictStatus_t predict_status = static_cast<ProcToGuiPredictStatus_t>(_predict_status_msg.getData());

   bool                     used           = _waiting.event(PREDICT_STATUS, predict_status);
   DataLog (interesting(used)) << "PREDICT MANAGER GOT predict status (" << predict_status
                               << "), " << _waiting
                               << endmsg;

   // if someone out there wants the prediction screen, and we're in blood run,
   // set the requested flag.  (If we're not in blood run, nobody cares if the
   // flag is set because nobody checks it.)
   if (predict_status == REQUEST_SELECTION_SCREEN &&  _in_run)
      _prediction_screen_requested = true;

//   log_status(__FUNCTION__);
   update_status_line(_waiting);


   if (!used)  // if we didn't use this event in a state transition, notify any interested parties of the event
   {
      _predict_status_callback();
      return;
   }


   if (!_waiting)  // if the current request is complete
      transaction_complete("");

}

void PredictManager_Impl::procedure_select_notify ()
{
   int  procnum = _procedure_selected_msg.getData();

   bool used    = _waiting.event(PROCEDURE_SELECTED, 0);

   DataLog(interesting(used)) << "PREDICT MANAGER GOT procedure select message; Current Proc Number = "
                              << procnum << ", " << _waiting << endmsg;




//   log_status(__FUNCTION__);
   update_status_line(_waiting);


   if (!_waiting)  // if the current request is complete
      transaction_complete("procedure select");

}


void PredictManager_Impl::donor_data_status_notify ()
{
   bool used = _waiting.event(DONOR_DATA_STATUS, 0);
   DataLog(interesting(used)) << "PREDICT MANAGER GOT DonorDataStatusMsg, "  << _waiting << endmsg;

//    log_status(__FUNCTION__);
   update_status_line(_waiting);

   _donor_data_source = _donor_data_status_msg.getData();

   if ( !used || !_request_complete_callback.assigned())
   {
      DataLog(interesting(used)) << "PREDICT MANAGER: ... not used, firing donor_data_status callback"  << endmsg;
      _donor_data_status_callback();

      return;
   }

   if (!_waiting)   // if the current request is complete
      transaction_complete("donor vitals");
}


void PredictManager_Impl::predict_responded_notify ()
{
   unsigned seq = (unsigned)_predict_responded_msg.getData();


   bool used = _waiting.event(PREDICT_RESPONDED, seq);
   DataLog(interesting(used)) << "PREDICT MANAGER GOT PredictResponded(" << seq
                              << "), " << _waiting << endmsg;

//   log_status(__FUNCTION__);
   update_status_line(_waiting);

   // we don't have any specific callback for this; we only care if the
   // transaction is complete.


   if (used && !_waiting) // if the current request is complete (and it wasn't // complete before)
      transaction_complete("donor vitals");
}

void PredictManager_Impl::system_state_change_notify ()
{
   State_names newstate = static_cast<State_names>(_system_state_change_msg.getData());

   // If we just transitioned into run, clear the prediction screen request flag
   if (_system_state < BLOOD_PRIME)
      _prediction_screen_requested = false;

   DataLog(log_level_gui_debug) << "PREDICT MANAGER, system state transition, "
                                << state_name_table[_system_state].state_text
                                << " --> "
                                << state_name_table[newstate].state_text
                                << flags()
                                << endmsg;

   _system_state = newstate;

   if (_system_state >= BLOOD_PRIME)
      _in_run = true;

}





void PredictManager_Impl::log_status (const char* fn) const
{
   DataLog(log_level_gui_debug) << "PREDICT MANAGER, from " << fn << ", WaitState:" << _waiting
                                << ( _prediction_screen_requested ? "; prediction screen requested" : "")
                                << ( _procedures_are_available    ? "; procedures are available"    : "")
                                << endmsg;
}


string PredictManager_Impl::flags () const
{
   buf.str("");
   buf << ( _prediction_screen_requested ? "; prediction screen requested" : "")
       << ( _procedures_are_available    ? "; procedures are available"    : "");

   return buf.str();
}

}  // namespace detail



////////////////////////////////////////////////////////////////////////////////
//
// Finally, the public interface functions (all just forward)

using detail::singleton;

void attach (Base_Apheresis_Window* a) { singleton().attach(a); }
void detach (Base_Apheresis_Window* a) { singleton().detach(a); }

void set_predict_status_callback  (const CallbackBase& callback) { singleton().set_predict_status_callback(callback); }
void set_procedure_info_callback  (const CallbackBase& callback) { singleton().set_procedure_info_callback(callback); }
void set_donor_data_status_callback (const CallbackBase& callback) { singleton().set_donor_data_status_callback(callback); }
void set_request_complete_callback  (const CallbackBase& callback) { singleton().set_request_complete_callback(callback); }

void sendPredictRequest (GuiToProcPredictRequest_t type, int line, const char* fn, const char* file)
{
   singleton().sendPredictRequest(type, line, fn, file);
}

void sendDonorTimeUpdate (int minutes) { singleton().sendDonorTimeUpdate(minutes); }
void sendFlowAdjustment (int flow) { singleton().sendFlowAdjustment (flow)   ; }
void sendDonorVitals (const SDonorVitals& vitals, int line, const char* fn, const char* file)
{
   singleton().sendDonorVitals(vitals, line, fn, file);
}

bool prediction_screen_requested () { return singleton().prediction_screen_requested() ; }
void clear_prediction_screen_requested () { singleton().prediction_screen_requested(false) ; }
void set_prediction_screen_requested ()   { singleton().prediction_screen_requested(true) ; }

bool  procedures_are_available () { return singleton().procedures_are_available() ; }
void  clear_procedures_are_available ()  { singleton().procedures_are_available(false) ; }
bool& no_product () { return singleton().no_product(); }


int donor_data_source () { return singleton().donor_data_source(); }

bool waiting () { return singleton().waiting(); }


}    // namespace PredictManager

/* FORMAT HASH 5a524795560513d5a0ecfc1e6a7fe58e */

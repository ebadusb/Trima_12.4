#include "adjust_manager.hpp"
#include "adjust.h"

#include "an_alarm.h"
#include "aphwind.hpp"
#include "datalog.h"
#include "predict_msgs.h"
#include "trimamessages.h"

#include "trouble.hpp"          // really, we only work with trouble screens at the moment.

using namespace procedure_adjustment_status;
using manager_detail::update_status_line;


namespace AdjustManager_detail
{

using GuiPredictSequenceValues::ADJUST_START;

struct AdjWaitState
{
public:

   AdjWaitState() { set_not_waiting(); }


   void set_not_waiting ()   { w_ack = false; w_pred = false; }
   void set_wait_for_ack ()  { w_ack = true ; w_pred = false; }
   void set_wait_for_pred () { w_ack = false; w_pred = true ; }


   bool w_ack;                  // waiting for ack
   bool w_pred;                 // waiting for prediction response

   operator bool() { return w_ack || w_pred; }

   void print (DataLog_Stream& os) const
   {
      if (w_ack)
         os << "waiting for ack";
      else
      {
         if (w_pred)
            os << "waiting for pred response";
         else
            os << "not waiting";
      }
   }

};

inline
DataLog_Stream& operator << (DataLog_Stream& os, const AdjWaitState& wait)
{
   wait.print(os);
   return os;
}

class AdjustManager_Impl
{
public:

   // Singleton; this is the only way to get one.
   static AdjustManager_Impl& singleton ();

   void attach (Screen_TROUBLE*);
   void detach (Screen_TROUBLE*);

   void sendAdjustment (troubleshooting_adjustments type, int line, const char* fn, const char* file);

   void reset ();
   bool waiting ();

private: // methods

   AdjustManager_Impl();
   ~AdjustManager_Impl(){}
   AdjustManager_Impl            (const AdjustManager_Impl&);  // noncopyable, not implemented
   AdjustManager_Impl& operator = (const AdjustManager_Impl&); // nonassignable, not implemented


   // void deactivate_callbacks();

private: // Messages and Callbacks

   // Incoming messages, and callbacks
   ProcedureAdjustmentStatusMsg adjustmentStatusMsg;
   PredictRespondedMsg          predictRespondedMsg;

   void adjustmentStatusCallback ();
   void predictRespondedCallback ();

   // Registered callbacks
   CallbackBase request_completed_callback;
   CallbackBase adjustment_status_callback;

   // Outgoing messages
   ProcedureAdjustmentMsg procedureAdjustmentMsg;

   void transaction_complete (unsigned);

private: // member data

   Screen_TROUBLE* screen; // The trouble screen currently talking to us.

   unsigned        current_adjustment;

   AdjWaitState    wait;

   stringstream    buf;

};

inline
AdjustManager_Impl& AdjustManager_Impl::singleton ()
{
   static AdjustManager_Impl a;
   return a;
}

AdjustManager_Impl::AdjustManager_Impl()
   : screen(NULL),
     procedureAdjustmentMsg (MessageBase::SEND_LOCAL),
     current_adjustment (ADJUST_START),
     wait()
{
   // We attach to these messages upon construction: the AdjustManager listens
   // for incoming messages for the entirety of its existence.
   adjustmentStatusMsg.init(Callback<AdjustManager_Impl>(this, &AdjustManager_Impl::adjustmentStatusCallback),
                            MessageBase::SNDRCV_RECEIVE_ONLY);

   predictRespondedMsg.init(Callback<AdjustManager_Impl>(this, &AdjustManager_Impl::predictRespondedCallback),
                            MessageBase::SNDRCV_RECEIVE_ONLY);
}


// Here, a Trouble_SCREEN attaches to us.  This is more complicated that it
// might appear, since there can be many Trouble_SCREENS in existence, and the
// GUI can switch from one to another without warning (or, at least, without
// telling the Trouble_SCREEN that it's been switched away from -- see IT
// 13040).  So, we detect the phenomenon of "Trouble_SCREEN stomping" here and
// notify the relevent screen when it happens.



void AdjustManager_Impl::attach (Screen_TROUBLE* obj)
{
   DataLog(log_level_gui_info) << "ADJUST MANAGER, attach: new " << obj
                               << ", old " << screen
                               << endmsg;

   if ( screen )               // if we already had an attached screen...
   {
      if (screen == obj)       // ... and if this is a duplicate attach from
                               // the same guy, we just log it and carry on
      {
         DataLog(log_level_gui_info) << "ADJUST MANAGER, re-attach from screen " << obj->get_screen_id ()
                                     << '[' << obj
                                     << ']' << endmsg;

         return;
      }

      // .. otherwise, the new screen is preempting the old one.  Log that
      DataLog(log_level_gui_info) << "ADJUST MANAGER, attach from screen " << obj->get_screen_id ()
                                  << '[' << obj
                                  << "], replacing screen  "    // << _screen->get_screen_id()
                                  << '[' << screen
                                  << "]; " << wait <<  endmsg;

      // the old screen's callbacks will be overwritten with the new screen's
      // callbacks below.
   }
   else
   {
      DataLog(log_level_gui_info) << "ADJUST MANAGER attach from screen " << obj->get_screen_id ()
                                  << '[' << obj
                                  << "]; " << wait << endmsg;
   }
   screen = obj;

   // set callbacks
   request_completed_callback = Callback<Screen_TROUBLE>(screen,
                                                         &Screen_TROUBLE::adjustment_complete_notify);

   adjustment_status_callback = Callback<Screen_TROUBLE>(screen,
                                                         &Screen_TROUBLE::updateScreenData);



}

void AdjustManager_Impl::detach (Screen_TROUBLE* obj)
{
   if (screen == NULL)
   {
      // duplicate detach, perfectly fine
      DataLog(log_level_gui_info) << "ADJUST MANAGER, duplicate detach" << endmsg;
      return;
   }

   DataLog(log_level_gui_info) << "ADJUST MANAGER detach from screen " << obj->get_screen_id ()
                               << '[' << screen
                               << "]; " << wait << endmsg;
   if ( obj != screen)
   {
      DataLog(log_level_gui_info) << "ADJUST MANAGER attach/detach cross: currently attached=" << screen
                                  << ", detach from " << obj << endmsg;
      // but don't detach here -- whoever we're attached to right now needs
      // to stay that way, whoever's trying to detach already has been (when
      // the current guy attached, see above).
      return;
   }

   CallbackBase null_callback;

   screen                     = NULL;
   request_completed_callback = null_callback;
   adjustment_status_callback = null_callback;
}



void AdjustManager_Impl::sendAdjustment (troubleshooting_adjustments type,
                                         int line, const char* fn, const char* file)
{
   ++current_adjustment;

   buf.str("");
   buf <<  "ADJUST MANAGER SENDING adjustment " << type << " #" << current_adjustment;
   if (fn   != "")
      buf << " from " << file << ":" << line << '[' << fn << "] ";

   switch (type)
   {
      case ACCESS_PRESSURE_UP        :
      case ACCESS_PRESSURE_DOWN      :
      case RETURN_PRESSURE_UP        :
      case RETURN_PRESSURE_DOWN      :
      case TINGLING_UP               :
      case TINGLING_DOWN             :
      case CLUMPING_UP               :
      case CLUMPING_DOWN             : wait.set_wait_for_ack();

      case SPILLOVER                 :
      case AIR_IN_PLASMA_LINE        :
      case SALINE_BOLUS              :
      case ADJUSTMENT_SCREEN_CLOSED  : break; // we don't wait for responses to these.
      case ADJUSTMENT_MAX_ENUM_VALUE : break; // should never be used

      // these shouldnt do anything:
      case AUTOFLOW_QIN_INCREASE : break;  // should never be used
      case AUTOFLOW_QIN_DECREASE : break;  // should never be used
   }

   AdjustmentPayload payload(type, current_adjustment);

   DataLog(log_level_gui_info) << buf.str()
                               << " : " << payload
                               << ", " << wait
                               << endmsg;

   // Static here, to avoid repeated calls to message ctors
   procedureAdjustmentMsg.send(payload);
   update_status_line(wait);
}

bool AdjustManager_Impl::waiting () { return wait; }


void AdjustManager_Impl::transaction_complete (unsigned sequence)
{

   if (request_completed_callback.assigned())
   {
      DataLog(log_level_gui_info) << "ADJUST MANAGER: completed transaction # " << sequence << endmsg;
      request_completed_callback();
   }
   else
   {
      DataLog(log_level_gui_info) << "ADJUST MANAGER: completed transaction # " << sequence
                                  << ", but no callback set." << endmsg;
   }
}

void AdjustManager_Impl::adjustmentStatusCallback ()
{
   AdjustmentStatusPayload payload (adjustmentStatusMsg.getData());

   unsigned                sequence = payload._value;

   if (!screen)
   {
      // DataLog(log_level_gui_debug) << "ADJUST MANAGER: received StatusAck " << sequence
      //                              << " but no screen attached, so ignoring it" << endmsg;
      return;
   }

   // sequence < ADJUST_START means that an adjustment occurred that we didn't
   // request; don't fiddle with the wait state machine, but rather transmit the
   // event straight through.
   if (sequence < ADJUST_START)
   {
      if (adjustment_status_callback.assigned())
      {
         DataLog(log_level_gui_debug) << "ADJUST MANAGER: received StatusAck " << sequence
                                      << ", firing adjustment_status callback"
                                      << endmsg;
         adjustment_status_callback();
      }
      else
      {
         DataLog(log_level_gui_debug) << "ADJUST MANAGER: received StatusAck " << sequence
                                      << ", but no callback set " << endmsg;
      }
      return;
   }

   // Note that we do *not* fire adjustment_status_callback under any other
   // circumstances; if sequence >= 0, we were waiting for something, and we fire
   // the other callback (see below).

   if (sequence < current_adjustment)
   {
      DataLog(log_level_gui_debug) << "ADJUST MANAGER: received StatusAck # "
                                   << sequence << " < current adjustment " << current_adjustment
                                   << ", ignoring"
                                   << endmsg;
      return;
   }

   // Update waiting status appropriately
   switch (payload._enum)
   {
      case ADJUSTMENT_ACK    : wait.set_not_waiting()   ; break;
      case ADJUSTMENT_UPDATE : wait.set_wait_for_pred() ; break;
      default                :
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm("Bad Adjust Payload Type");
         break;
   }




   DataLog(log_level_gui_debug) << "ADJUST MANAGER: received StatusAck # " << sequence
                                << "(" << payload << ")"
                                << ", " << wait
                                << endmsg;

   if (!wait)
   {
      transaction_complete(sequence);
   }

   update_status_line(wait);
}

void AdjustManager_Impl::predictRespondedCallback ()
{
   unsigned seq = (unsigned)predictRespondedMsg.getData();

   if (!screen)
   {
      // DataLog(log_level_gui_debug) << "ADJUST MANAGER: received PredictResponse # " << seq
      //                              << " but no screen attached, so ignoring" << endmsg;
      return;
   }


   // these are generated by other processes, and we always ignore them.  Since
   // we start current_adjustment at ADJUST_START, this is actually handled by the logic
   // below... but it's good to call out this specific situation.  Also, since
   // it has nothing to do with us, we don't do any logging at all.
   if (seq < ADJUST_START) return;

   if (seq < current_adjustment)
   {
      DataLog(log_level_gui_debug) << "ADJUST MANAGER: received PredictResponse # " << seq
                                   << " < current adjustment " << current_adjustment
                                   << ", ignoring"
                                   << endmsg;
      return;
   }
   wait.set_not_waiting();

   transaction_complete(seq);

   update_status_line(wait);

}


void AdjustManager_Impl::reset ()
{
   if (wait)
   {
      // DataLog(log_level_gui_debug) << "ADJUST MANAGER: reset, current status " << wait << endmsg;
   }

   wait.set_not_waiting();
   update_status_line(wait);

}

} // namespace AdjustManager detail

//////////////////////////////////////////////////////////////////////
//
//  Now, provide the implementation of the user interface


namespace AdjustManager
{


#define ADJ_MGR  AdjustManager_detail::AdjustManager_Impl::singleton()


void sendAdjustment (troubleshooting_adjustments type, int line, const char* fn, const char* file)
{
   ADJ_MGR.sendAdjustment(type, line, fn, file);
}



void attach (Screen_TROUBLE* a) { ADJ_MGR.attach(a); }
void detach (Screen_TROUBLE* a) { ADJ_MGR.detach(a); }


bool waiting () { return ADJ_MGR.waiting(); }

void reset () { ADJ_MGR.reset(); }

#undef ADJ_MGR

} // namespace AdjustManager

/* FORMAT HASH 9d4135755d187949c846986f6000209b */

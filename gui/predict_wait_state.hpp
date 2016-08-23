#ifndef PREDICT_WAIT_STATES
#define PREDICT_WAIT_STATES


#include "trimamessages.h"
#include "datalog.h"

namespace PredictWaitStates
{

using GuiPredictSequenceValues::DVITALS_ACK; // sent to predict to identify
                                             // responses to DVitalsFromGui events


//////////////////////////////////////////////////////////////////////
//
//  Waiting State objects: these represent different "sets of events we're waiting for", each of
//  which corresponds to a message we've sent to Predict.  We can wait on ProcToGuiPredictStatusMsg
//  (with customization for the payload), GuiProceduresUpdated, or ProcedureSelectedMsg messages; we
//  wait for different combinations, with specific payloads in the Status msg, depending on what
//  messages we've sent.
//
//  If this seems overly complicated -- it is, or rather, the overly-complicated thing is the of
//  messages that the Predict process sends out after an operation, and we just have to deal.

// Start by defining the events

enum WaitStateEvent
{
   GUI_PROCEDURE_UPDATED,
   PREDICT_STATUS,
   PROCEDURE_SELECTED,
   DONOR_DATA_STATUS,
   PREDICT_RESPONDED
};


// we start with a very general abstract base struct
struct WaitStateBase
{
public:

   WaitStateBase(const char* n = "")
      : name(n) {}

   virtual void reset () = 0;
   virtual bool waiting () const                 = 0;

   virtual bool event(WaitStateEvent, int)       = 0; // returns true if the event was used

   virtual void print (DataLog_Stream& os) const = 0;

   virtual ~WaitStateBase() {}

   char tf (bool b) const { return b ? 'T' : 'F'; }


   const char* name;

};



//////////  Now, specific implementations

// Null wait state -- not waiting, no state transitions
struct NullWaitState
   : public WaitStateBase
{
   NullWaitState()
      : WaitStateBase("nothing") {}
   virtual void reset () {}
   virtual bool waiting () const { return false; }
   virtual bool event (WaitStateEvent, int) { return false; }
   virtual void print (DataLog_Stream& os) const {   }
};


///  Generic thing to wait on a GUI_PROCEDURE_UPDATED event
struct ProceduresWait
   : public WaitStateBase
{
   bool                   wait;
   GuiProceduresUpdated_t request;

   ProceduresWait(GuiProceduresUpdated_t req)
      : WaitStateBase(), wait(true), request(req) {}
   virtual void reset () { wait = true; }
   virtual bool waiting () const { return wait; }
   virtual bool event (WaitStateEvent e, int r)
   {
      // We only care about GUI_PROCEDURE_UPDATED events:
      if (e != GUI_PROCEDURE_UPDATED)
         return false;

      // If we're set to 'any', any GUI_PROCEDURE_UPDATED event will do
      if ( ( request == NON_REQUEST ) )
      {
         wait = false;
         return true;
      }

      // if the incoming request matches
      if ( request == r )
      {
         wait = false;
         return true;
      }

      // otherwise, we're not interested
      return false;
   }

   // Shorthand for setting the state; useful in other WaitStates that aggregate this.
   void operator = (bool b) { wait = b; }

   virtual void print (DataLog_Stream& os) const
   {
      if (wait)
      {
         if (request == -1)
            os << "GuiProcedures(any) ";
         else
            os << "GuiProcedures(" << request << ") ";
      }
   }
};


// Wait for a GuiProceduresUpdated message.  (Just wraps the ProceduresWait in
// something that has a name.)
struct ProcedureUpdated
   : public WaitStateBase
{
   ProceduresWait waiter;

   ProcedureUpdated(const char* n, GuiProceduresUpdated_t req)
      : WaitStateBase(n), waiter(req) {}
   virtual void reset () { waiter.reset(); }
   virtual bool waiting () const { return waiter.waiting(); }
   virtual bool event (WaitStateEvent e, int r)
   {
      return waiter.event(e, r);
   }
   virtual void print (DataLog_Stream& os) const { waiter.print(os); }
};


/////// Wait state for a selection message -- we need a procedure selected
/////// message.
struct SelectCurrent
   : public WaitStateBase
{
   bool wait;

   SelectCurrent(const char* n)
      : WaitStateBase(n), wait(true) {}
   virtual void reset () { wait = true; }
   virtual bool waiting () const { return wait;  }
   virtual bool event (WaitStateEvent e, int)
   {
      if (e == PROCEDURE_SELECTED)
      {
         wait = false;
         return true;
      }
      return false;
   }

   virtual void print (DataLog_Stream& os) const
   {
      if (wait) os << " ProcedureSelected ";
   }
};


// This case looks for a pair of messages: a ProcToGuiPredictStatusMsg
// (REQUEST_SELECTION_SCREEN) and a GuiProceduresUpdated.  (This is what we get
// when Predict actually decides that something has changed; this wait state is
// used for messages that are guaranteed to produce a change.)
struct StatusAndProcedures
   : public WaitStateBase
{
   ProceduresWait procedure;
   bool           status;

   StatusAndProcedures(const char* n,
                       GuiProceduresUpdated_t req = NON_REQUEST)
      : WaitStateBase(n),
        procedure(req),
        status(true)
   {}


   virtual void reset () { status = true; procedure.reset(); }
   virtual bool waiting () const { return status || procedure.waiting(); }
   virtual bool event (WaitStateEvent e, int t = -1)
   {
      if ( procedure.event(e, t) )
         return true;

      if ( (e == PREDICT_STATUS) &&
           (t == REQUEST_SELECTION_SCREEN) )
      {
         status = false;
         return true;
      }
      return false;
   }

   virtual void print (DataLog_Stream& os) const
   {
      procedure.print(os);
      os << ( status ? " PredictStatus(REQUEST) " : "" );
   }
};


// Here, we're either looking for a Status(REQUEST_SELECTION_SCREEN) and
// ProceduresUpdated pair (this is what we get when Predict actually decides
// that something has changed), or a single Status(PROCEDURE_VALID) message.
struct StatusOrProcedures
   : public WaitStateBase
{
   ProceduresWait procedure;
   bool           status;

   StatusOrProcedures(const char* n,
                      GuiProceduresUpdated_t req = NON_REQUEST)
      : WaitStateBase(n),
        procedure(req),
        status(true) {}
   virtual void reset () { status = true; procedure.reset(); }
   virtual bool waiting () const { return status || procedure.waiting(); }
   virtual bool event (WaitStateEvent e, int t)
   {
      if ( procedure.event(e, t) )
         return true;

      if ( e == PREDICT_STATUS )
      {
         switch  (t)
         {
            case PROCEDURE_VALID          : procedure = false ;       // fall through
            case REQUEST_SELECTION_SCREEN : status    = false    ; return true;
            default                       : return false      ;          // technically an error
         }
      }
      return false;
   }

   virtual void print (DataLog_Stream& os) const
   {
      procedure.print(os);
      os << ( status ? " PredictStatus " : "" );
   }
};


struct DonorDataStatus
   : public WaitStateBase
{
   bool status;                 // donor info status
   bool respond;                // predict responded

   DonorDataStatus()
      : WaitStateBase("DonorVitals"), status(true), respond(true) {}
   virtual void reset () { status = respond = true; }
   virtual bool waiting () const { return status || respond; }
   virtual bool event (WaitStateEvent e, int t)
   {
      if ( e == DONOR_DATA_STATUS)
      {
         status = false;
         return true;
      }
      if ( (e == PREDICT_RESPONDED) &&
           (t == DVITALS_ACK) )    // we're specifically looking for this ACK
      {
         respond = false;
         return true;
      }

      return false;
   }

   virtual void print (DataLog_Stream& os) const
   {
      if (status)
         os << " DonorDataStatus ";
      if (respond)
         os << " PredictResponded ";
   }
};


//////////////////////////////
//
//  Finally: a WaitState that wraps other wait states.  Basically a decorator,
//  but it's the thing the predict manager actually uses

class WaitState
   : public WaitStateBase
{
public:

   WaitState()
      : WaitStateBase(), wait(&null_wait), null_wait() {}

   virtual void reset () { wait = &null_wait; }
   virtual bool waiting () const { return wait->waiting(); }
   virtual bool event (WaitStateEvent e, int t) { return wait->event(e, t); }
   virtual void print (DataLog_Stream& os) const { wait->print(os); }

   virtual void set (WaitStateBase& newstate) { wait = &newstate; wait -> reset(); }

   operator bool() const { return wait->waiting(); }

   const char* getname () const { return wait->name; }

private:

   WaitStateBase* wait;

   NullWaitState  null_wait;

};

inline
DataLog_Stream& operator << (DataLog_Stream& os, const WaitState& wait)
{
   os << "Waiting for: " << wait.getname() << " < ";
   wait.print(os);
   os << " > ";
   return os;
}




}
#endif

/* FORMAT HASH 1f574fb20610f9890e92affd6cbb69a3 */

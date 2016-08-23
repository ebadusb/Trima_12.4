/*
  Predict Manager : GUI's communication path to and from the Predict task.

  The idea is to

  A) serialize GUI's communication with the Predict task: only one messages goes at a time;
  B) to properly recognize when predict has responded;
  C) to track important status changes.

  A) is to make sure that we don't send multiple requests to Predict, which is confusing and can
  lead to confusion when messages start coming back.

  B) is so that we know when Predict is done.

  C) is so that if a message comes in while a screen is inactive, or during a screen switch,
  screens can find out what happened.


  Under the hood, PredictManager has a singleton object that keeps a single, global state.  This
  object is hidden from clients; instead, we just have (apparently) a bunch of functions, gathered
  into a namespace for clarity.

*/

#ifndef PREDICT_MONITOR_INCLUDE
#define PREDICT_MONITOR_INCLUDE

#include "predict_msgs.h"
#include "callback.h"
#include "manager_base.hpp"
#include "safproc.h"

class Base_Apheresis_Window;

// We group


namespace PredictManager
{

void attach (Base_Apheresis_Window*);
void detach (Base_Apheresis_Window*);


// callbacks for specific messages, if you know what you're looking for:
void set_predict_status_callback (const CallbackBase&);     // ProcToGuiPredictStatusMsg
void set_procedure_info_callback (const CallbackBase&);     // GuiProceduresUpdated
void set_donor_data_status_callback  (const CallbackBase&); // DonorDataStatusmsg

// If you want to be notified when the current transaction is complete, do this one.
void set_request_complete_callback (const CallbackBase&);

void sendPredictRequest (GuiToProcPredictRequest_t type,
                         int line, const char* fn, const char* file);
void sendDonorTimeUpdate (int minutes);
void sendFlowAdjustment (int flow);
void sendDonorVitals (const SDonorVitals&,
                      int line, const char* fn, const char* file);

bool  prediction_screen_requested ();
void  clear_prediction_screen_requested ();
void  set_prediction_screen_requested ();
bool& no_product ();              // true when there are no valid procedures

bool procedures_are_available ();
void clear_procedures_are_available ();

int donor_data_source ();       // returns payload of donor data status messages
                                // (which end DonorVitals tranactions)

bool waiting ();


}

#endif // ifndef PREDICT_MONITOR_INCLUDE

/* FORMAT HASH 38d976214d9230370268cabc6d2a74ed */

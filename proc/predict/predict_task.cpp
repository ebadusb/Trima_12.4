/****************************************************************************
 *
 ***************************************************************************/

#include "predict_task.h"

// CDS
#include "proctrimaset_cds.h"
#include "software_cds.h"

#include <cmath>

// Make queue size big since on PFR, lots and lots of messages are sent.  See IT 5345
#define QUEUE_SIZE 104

// Macro to define the main function
MAIN2(PredictTask, QUEUE_SIZE);



PredictTask::PredictTask()
   : _predictRequestMsg   (Callback<PredictTask>(this, &PredictTask::ProcDataReceived)),
     _modifyProductMsg    (Callback<PredictTask>(this, &PredictTask::ModifyProduct)),
     _configMsg           (Callback<PredictTask>(this, &PredictTask::ConfigurationReceived)),
     _featureChangedMsg   (Callback<PredictTask>(this, &PredictTask::FeatureChangedReceived)),
     _donorTimeMsg        (Callback<PredictTask>(this, &PredictTask::DonorTimeReceived)),
     _flowAdjMsg          (Callback<PredictTask>(this, &PredictTask::FlowAdjReceived)),
     _guiRequest          (Callback<PredictTask>(this, &PredictTask::gui_predict_request)),
     _donorRejectedMsg    (Callback<PredictTask>(this, &PredictTask::donor_rejected)),
     _rinsebackRequestMsg (Callback<PredictTask>(this, &PredictTask::rinseback_request)),

// outgoing messages
     _guiProceduresUpdatedMsg   (MessageBase::SEND_LOCAL),
     _predictRespondedMsg       (MessageBase::SEND_LOCAL),
     _procedureSelected         (MessageBase::SEND_LOCAL),
     _procToGuiPredictStatusMsg (MessageBase::SEND_LOCAL),
     _updateAFTimeMsg           (MessageBase::SEND_LOCAL),

     // Alarm messages
     _AF_TimeAlert (AUTOFLOW_TIME_ALERT),
     _AF_TimeAlarm (AUTOFLOW_TIME_ALARM),
     _AF_DQAlarm   (AUTOFLOW_TIME_DQ),

// Singletons
     _procedures    (ProcedureList::Instance()),
     _selectedProc  (_procedures.Selected()),
     _curProc       (_procedures.Current()),
     _maxDesignator (_procedures.MaxDesignator()),
     _cobeConfig    (CobeConfig::data()),
     _config        (Config::Instance()),

     _AMAP_PLATELETS_ON (false),

     _offPrevProc (NULL),
     _prevProc    (NULL),
     _nextProc    (NULL),
     _offNextProc (NULL),

     _currentDesignator(1),

     _configurationReceived(false),

// CDS
     _procState (ProcDataState::Instance()),
     _adjustCDS (ROLE_RW),
     _guiProcedures(ROLE_RW),
     _procRunCDS    (ROLE_RW),
     _configCDS     (ROLE_RO),

     _current_response_type(NON_REQUEST),
     _adjustmentNumber (0),
     _currentPlasmaVol(0),

     _predict()
{
   DataLog(log_level_predict_debug) << "Predict Task Constructed" << endmsg;
}



PredictTask::~PredictTask() {}



//////////////////////////////////////////////////////////////////////////////
// Initialization routines called by Standard Main
ALARM_VALUES PredictTask::Init ()
{
   DataLog(log_level_predict_debug) << " PredictTask::Init()" << endmsg;

   // Init local variables:
   _prevProc          = _nextProc = _offPrevProc = _offNextProc = NULL;

   _AMAP_PLATELETS_ON = Software_CDS::GetInstance().getFeature(AmapPlateletsAvailable);

   return _predict.Init();
}



void PredictTask::Terminate ()
{
   DataLog(log_level_predict_debug) << "PredictTask::Terminated" << endmsg;

   KILL_ME(0);                  // calls CStdAppl::Terminate.
}



//////////////////////////////////////////////////////////////////////
//
//  Message callbacks
void PredictTask::ProcDataReceived ()
{
   // DataLog(log_level_predict_debug) << "PredictTask::ProcDataReceived1" << endmsg;

   DoPredictionPayload payload(_predictRequestMsg.getData() );

   // Ensure config.dat processed at least once before starting
   //
   if (!_configurationReceived) ConfigurationReceived(__LINE__);

   _adjustmentNumber = payload._value;

   ProcDataReceived(__LINE__, payload._enum);
}



void PredictTask::ProcDataReceived (int line, DoPrediction_t request)
{

   // DataLog(log_level_predict_debug) << "Predict::ProcDataReceived2" << endmsg;

   DataLog(log_level_predict_debug) << "ProcDataReceived cmd=" << request
                                    << " (called from line " << line << ")" << endmsg;

   _procState.setPredictRequest(request);
   _predict.UpdateProcData(); // refresh members from CDS

   float old_time = 0.0f;
   // time before prediction called
   if (_selectedProc)
   {
      old_time = _selectedProc->getPredTp();
   }
   int predictEvent = 0;  // used for the autoflow / non-autoflow disqualification alarms

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   if (request & ADJUSTMENT)
   {
      predictEvent = 0;
      _predict.process_adjustment();

      // we just made a manual adjustment, so reset the autoflow timer
      if (_config.isAutoFlowOn() == true)
      {
         // update the autoflow 10 min alarm
         _updateAFTimeMsg.send(3);
         DataLog(log_level_predict_debug) << "Predict: ADJUSTMENT (manual) sending timer reset msg" << endmsg;
      }

      request = (DoPrediction_t)(request & (~REPREDICT_ONLY)); // we've already done repredict if requestedq
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   if (request & AUTO_FLOW_ADJUSTMENT)
   {
      predictEvent = 2;
      DataLog(log_level_predict_debug) << "AUTO_FLOW ADJUSTMENT" << endmsg;
      _predict.process_adjustment(true);

      request = (DoPrediction_t)(request & (~REPREDICT_ONLY)); // we've already done repredict if requested
   }

   // ///////////////////////////////////////////////////////////////////////////////////////////////////////////
   if (request & REPREDICT_ONLY)
   {
      predictEvent = 3;
      _predict.ProcData_Repredict_Only();
   }


   // ///////////////////////////////////////////////////////////////////////////////////////////////////////////
   if (request & DONOR_INFO_UNCHANGED)
   {
      predictEvent = 0;
      DataLog(log_level_predict_debug) << " DONOR_INFO_UNCHANGED" << endmsg;
      validate_selected_procedure(__LINE__);
   }

   // ///////////////////////////////////////////////////////////////////////////////////////////////////////////
   if (request & DONOR_INFO_PRECONNECT)
   {
      //   If the 'confirm' button was pressed on the donor info screen, and
      //   the donor is not yet connected, we need to force reprediction and
      //   transfer to the selection screen.
      predictEvent = 0;
      DataLog(log_level_predict_debug) << " DONOR_INFO_PRECONNECT" << endmsg;
      _predict.reset_PIR_flags();

      _predict.SetAllUnoptimized();

      // (Note: both cases below send the same set of messages to GUI and the
      // rest of the system.)
      if (_AMAP_PLATELETS_ON)
      {
         DataLog(log_level_predict_debug) << "AMAPP: DONOR_INFO_PRECONNECT Show Optimal" << endmsg;
         show_optimal();

      }
      else
      {
         DataLog(log_level_predict_debug) << " DONOR_INFO_PRECONNECT, show initial screen" << endmsg;
         ShowInitialScreen();

      }
   }

   // ///////////////////////////////////////////////////////////////////////////////////////////////////////////
   if (request & DONOR_INFO_CHANGED)
   {
      predictEvent = 0;
      _predict.ProcData_Donor_Info_Changed();

      if (displayListHasChanged())
      {
         ShowInitialScreen();
      }
      else
      {
         validate_selected_procedure(__LINE__);
      }
   }



   // ///////////////////////////////////////////////////////////////////////////////////////////////////////////
   if (request & PTF_FILTER_RECALC)
   {
      predictEvent = 0;
      // repredict the selected proc, if it exists, and we've started the run
      if (_selectedProc && (_selectedProc->AlgSubstate(_procState.SubState()) > 0))
      {
         _selectedProc->setOptimized(0);
         _selectedProc->setStatus(UNOPTIMIZED);
         _predict.predictProcedure(*_selectedProc);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ///  AUTOFLOW Time checking stuff....

   if (_config.isAutoFlowOn() && _selectedProc)
   {
      float            new_time = _selectedProc->getPredTp();

      if (predictEvent==2)  // an AF adjust
      {
         float dt = new_time - old_time;

         float last_dt = _procRunCDS.AutoflowDeltaTime.Get();   // from before
         _procRunCDS.AutoflowDeltaTime.Set(last_dt + dt);
         _procRunCDS.AutoflowTotalNetTimeChanges.Set(_procRunCDS.AutoflowTotalNetTimeChanges.Get() + dt);

         DataLog (log_level_proc_info) << " Time Delta Set:  Current Proc Time=" << new_time
                                       << ",  Prior Proc Time=" << old_time
                                       << ",  This Changes Proc DeltaT=" << dt
                                       << ",  Prior Total DeltaT=" << last_dt
                                       << ",  New Total DeltaT=" <<   _procRunCDS.AutoflowDeltaTime.Get()
                                       << ",  Net Time Delta this run =" << _procRunCDS.AutoflowTotalNetTimeChanges.Get()
                                       << endmsg;



         // And now, check for resulting issues.  First, check for 10 minute increase
         if ( _procRunCDS.AutoflowDeltaTime.Get() >= 10.0f )
         {

            DataLog(log_level_proc_alarm_monitor_info) << "AutoFlowTimeAlarm set; time delta = "
                                                       <<  _procRunCDS.AutoflowDeltaTime.Get()
                                                       <<  endmsg;

            _procRunCDS.AutoflowDeltaTime.Set(0.0f);   // reset the delta time upon an alrm
            _procRunCDS.AutoflowTotal10MinAlarms.Set(_procRunCDS.AutoflowTotal10MinAlarms.Get() + 1);

            if (_AF_TimeAlert.getState() == ACTIVE)
            {

               DataLog(log_level_proc_alarm_monitor_info) << "AutoFlowTimeAlarm ALARM set, system stop. " <<  endmsg;
               // _AF_TimeAlert.clearAlarm();
               _AF_TimeAlarm.setAlarm();

            }
            else
            {

               DataLog(log_level_proc_alarm_monitor_info) << "AutoFlowTimeAlarm advisory set, no system stop. " <<  endmsg;
               _AF_TimeAlert.setAlarm();

            }
         }
      }

      if (predictEvent > 0 )
      {
         // check for procedure time disqualification
         if (new_time > _configCDS.Procedure.Get().key_proc_time)
         {
            DataLog(log_level_qa_external)
               << "Procedure Time Exceeds Max Proc Time,"
               << _configCDS.Procedure.Get().key_proc_time
               << " minutes.  Predict screen coming up as alert.  "
               << endmsg;

            if (predictEvent == 2)
            {
               DataLog(log_level_proc_alarm_monitor_info) << "set AutoFlow Disqualification Alarm" << endmsg;
               _AF_DQAlarm.setAlarm();
            }
         }
      }
      predictEvent = 0;
   }
   ////////////////////////////////////////////////////////////////////////////////////
   _predict.setPredictTargets();

   // Handshake with proc (regardless of results)
   SendPredictResponseMsgToProc(__LINE__);
}


void PredictTask::DonorTimeReceived ()
{
   int time = _donorTimeMsg.getData();

   DataLog(log_level_predict_debug) << "PredictTask: DonorTimeMsg Received, " << time
                                    << " minutes." << endmsg;

   _current_response_type = DONOR_TIME_ACK;
   // Ensure config.dat processed at least once before starting
   //
   if (!_configurationReceived) ConfigurationReceived(__LINE__);

   // Set the CDS
   _procState._predictCDS.maxDonorTime.Set(time);
   _procState.setRcvDonorTimeMsg(true);

   // Call the Predict Engine
   if ( _procState.CassetteState() < ProcTrimaSet_CDS::BLOOD )
      ProcDataReceived(__LINE__, DONOR_INFO_PRECONNECT);
   else
   {
      _predict.ProcData_Donor_Info_Changed();

      ShowInitialScreen();
   }
}


void PredictTask::FlowAdjReceived ()
{
   // this is a flow adjustment request from GUI.  Deal with it accordingly.
   int flowData = _flowAdjMsg.getData();

   DataLog (log_level_predict_debug) << "PredictTask: recieved flow adjustment message GUI: flow set to "
                                     << flowData << endmsg;

   _current_response_type = FLOW_ADJUSTMENT_ACK;

   //  Set current cap in adjustment CDS as if this was an in-run adjustment
   _adjustCDS.Draw.CurrentCap.Set(flowData);

   // Ensure config.dat processed at least once before starting
   if (!_configurationReceived) ConfigurationReceived(__LINE__);

   //   Reset internal caps and repredict.
   ProcDataReceived(__LINE__, ADJUSTMENT);

   ShowInitialScreen();
}

void PredictTask::ConfigurationReceived ()
{
   // DataLog(log_level_predict_debug) << " PredictTask::ConfigurationReceived()" << endmsg;
   ConfigurationReceived(__LINE__);
}

void PredictTask::ConfigurationReceived (int line)
{
   // DataLog(log_level_predict_debug) << " Predict::ConfigurationReceived()2" << endmsg;
   _configurationReceived = true;
   DataLog(log_level_predict_debug) << "PredictTask: Configuration received (called from line "
                                    << line << ")" << endmsg;
   _predict.FillInConfig();
   _predict.FillInProcedureConfig(); // since config.dat includes procedures
}



void PredictTask::FeatureChangedReceived ()
{
   _AMAP_PLATELETS_ON = Software_CDS::GetInstance().getFeature(AmapPlateletsAvailable);

   _predict.FeatureChangedReceived();
}



void PredictTask::gui_predict_request ()
{
   //  DataLog(log_level_predict_debug) << " PredictTask::gui_predict_request()" << endmsg;
   long int request = _guiRequest.getData();

   // remember what the type of the request was; we'll return this value on any
   // GuiProceduresUpdated messages going back as an ACK
   _current_response_type = static_cast<GuiProceduresUpdated_t>(request);

   DataLog (log_level_predict_debug) << "Recieved GuiToProcPredictRequestMsg(" << request << ")" << endmsg;

   switch (request)
   {
      case IS_PROCEDURE_VALID : // only sent after adjustment

         if (                                               // This if statement changed for IT 7682.
            (                                               // If one of the following is true:
               (_predict._blockTargets) ||                  // Adjustment has been made
               (!_selectedProc) ||                          // Or no product is selected
               (_selectedProc->getStatus() != GOOD_PRODUCT) // Or the selected product is not good
            )                                               // And the display list has changed...
            && displayListHasChanged()                      // (This gets evaluated only if one of the above is true)
            )
         {
            DataLog(log_level_predict_debug) << "Repredict is necessary." << endmsg;
            ShowInitialScreen();
         }
         else
         {
            DataLog(log_level_predict_debug) << "Repredict is NOT necessary." << endmsg;
            validate_selected_procedure(__LINE__);
         }
         break;

      case SELECT_NEXT :
         DataLog(log_level_predict_debug) << "Received from GUI SELECT_NEXT (" << request << ")" << endmsg;
         show_next();
         break;

      case SELECT_PREVIOUS :
         DataLog(log_level_predict_debug) << "Received from GUI  SELECT_PREVIOUS (" << request << ")" << endmsg;
         show_previous();
         break;

      case SELECT_CURRENT :
         DataLog(log_level_predict_debug) << "Received from GUI  SELECT_CURRENT (" << request << ")" << endmsg;
         // We don't send a GuiProceduresUpdated message in this case, so clear out
         // the response type.  (Otherwise, the next 'spontanous' GuiProcUpdated
         // message will say 'SELECT_CURRENT_ACK'.)
         _current_response_type = NON_REQUEST;
         _procRunCDS.AutoflowDeltaTime.Set(0.0f);

         select();
         break;

      case REPREDICT_ALL_OPTIMAL :
         DataLog(log_level_predict_debug) << "Received from GUI  REPREDICT_ALL_OPTIMAL (" << request << ")" << endmsg;
         _predict.UpdateProcData();
         // SetUnoptimized();
         //
         _predict.SetAllUnoptimized();
         show_optimal();
         break;


      case REPREDICT_ALL :
         DataLog(log_level_predict_debug) << "Received from GUI  REPREDICT_ALL (" << request << ")" << endmsg;
         _predict.UpdateProcData();
         _predict.SetAllUnoptimized();
         ShowInitialScreen();
         break;


      default :
         DataLog(log_level_predict_error) << "Unknown message " << request << " received" << endmsg;
         anAlarmMsg badmsg(PREDICT_INVALID_GUI_REQUEST);
         badmsg.setAlarm();
         break;
   }
}



void PredictTask::ModifyProduct ()
{
   ModifyProductStruct modification = _modifyProductMsg.getData();
   _predict.ModifyProduct(modification);
}



void PredictTask::donor_rejected ()
{
   DataLog(log_level_predict_debug) << " PredictTask::donor rejected()" << endmsg;
   // clear predict's list of procedures
   _predict.cleanup();

   //   Also need to reset targets to avoid cassette ID errors
   _predict.reset_targets();

   SendPredictResponseMsgToProc(__LINE__);
}



void PredictTask::rinseback_request ()
{
//  Historical note, no longer relevent, since the predict pause alert thing has
//  been fixed.  I wonder if handling a rinseback_request in Predict is still
//  necessary at all?
//
// -----------------------------------------------------------------------------
//
//   IT6736 - fix to ensure predict pause alert is cleared when rinseback request
//   received from GUI
//


   // Log this, 'cause the logging is gonna look weird in Predict when the call
   // comes from another file.  (This will eventually go away when the
   // Predict/PredictTask refactoring is complete.)
   DataLog(log_level_predict_debug) << " PredictTask::rinseback_request()" << endmsg;

   //
   //
   //  Also at this point the procedure status monitor in proc is waiting for a reply.
   //  Send out procedures to keep proc from reasserting the predict screen.  This is
   //  a fallout of implementation for IT6555.
   //
   send_to_gui_procedures(__LINE__);
}




//////////////////////////////////////////////////////////////////////
//
//  Processing routines



void PredictTask::SendPredictResponseMsgToProc (int line)
{
   //  DataLog(log_level_predict_debug) << " Predict::SendPredictResponseMsgToProc()" << endmsg;
   DataLog(log_level_predict_debug) << "PredictTask: SendPredictResponseMsgToProc: (called from line "
                                    << line << "), # " << _adjustmentNumber << endmsg;

   _predictRespondedMsg.send(_adjustmentNumber);

   // If we're acking a DVitals update, our adjustment number was (rather
   // artificially) set to that particular value.  Set it to 'default' so we
   // don't send anything else that looks like an ack.
   if (_adjustmentNumber == GuiPredictSequenceValues::DVITALS_ACK)
      _adjustmentNumber = GuiPredictSequenceValues::DEFAULT;
}



void PredictTask::SendStatusToGui (int line, ProcToGuiPredictStatus_t status)
{
   //   DataLog(log_level_predict_debug) << " Predict::SendStatusToGui()" << endmsg;
   DataLog(log_level_predict_debug) << "PredictTask: SendStatusToGui, " << status
                                    << " (called from line " << line << ")" << endmsg;
   _procToGuiPredictStatusMsg.send(status);
}



void PredictTask::cleanup ()
{
   _prevProc = _nextProc = _offPrevProc = _offNextProc = NULL;
   _predict.cleanup();
}



void PredictTask::validate_selected_procedure (int line)
{
   DataLog(log_level_predict_debug) << "Predict::validate_selected_procedure called from line " << line << endmsg;

   if (_selectedProc && _selectedProc->getStatus() == GOOD_PRODUCT)
   {
      _curProc = _selectedProc;    // IT6657
      if (_predict._blockTargets)
      {
         _predict._blockTargets = false;
         _predict.setPredictTargets();
      }
      SendStatusToGui(__LINE__, PROCEDURE_VALID);
      DataLog(log_level_predict_debug) << "Predict (Awaiting validation): sent to gui PROCEDURE_VALID" << endmsg;
      // We're not going to send procedures, so null out the procedure ack value
      _current_response_type = NON_REQUEST;
   }
   else
      ShowInitialScreen();

}

void PredictTask::send_to_gui_procedures (int line)
{
   //  DataLog(log_level_predict_info) << " Predict::send_to_gui_procedures " << endmsg;

   ProcedureInformationStruct reply;
   ProductOptionStruct        blank;

   reply[PREV] = _prevProc ? _predict.FillProcInfo(*_prevProc) : blank;
   reply[NEXT] = _nextProc ? _predict.FillProcInfo(*_nextProc) : blank;

   reply[CURR] =
      ( _curProc  && _curProc->getStatus() == GOOD_PRODUCT )
      ? _predict.FillProcInfo(*_curProc)
      : blank;

   if (_curProc) _predict._currentDisplayedValues = reply[CURR];



   DataLog(log_level_predict_debug) << "send_to_gui_procedures: "
                                    << " " << reply[PREV].procedure_index
                                    << " " << reply[CURR].procedure_index
                                    << " " << reply[NEXT].procedure_index
                                    << " (called from line " << line << ")" << endmsg;

   _guiProcedures.ProcedureInfo.Set(reply);


   // We return the type of request that produced this message.  If we return
   // NON_REQUEST, that means this was a 'gratuitous' response -- that is, the
   // outgoing message is not in response to a message from GUI.
   DataLog (log_level_predict_debug) << "PredictTask: sending GuiProceduresUpdated in response to "
                                     << _current_response_type
                                     << endmsg;

   _guiProceduresUpdatedMsg.send(_current_response_type);
   _current_response_type = NON_REQUEST;

}



void PredictTask::ShowInitialScreen ()
{
   //   DataLog(log_level_predict_debug) << " Predict::ShowInitialScreen()" << endmsg;

   DataLog(log_level_predict_debug) << "Predict (awaiting validation): sent to gui a request for selection screen"
                                    << endmsg;

   iterator         iter;
   procedureDetail* starting_product = _curProc;

   if (_curProc)
   {
      if (!_curProc->getOptimized())
         _predict.predictProcedure(*_curProc);
      if (_curProc->getStatus() != GOOD_PRODUCT)
      {
         starting_product = 0;
      }
      DataLog(log_level_predict_debug) << "Predict: _curProc set, starting product = "
                                       << ( starting_product ? starting_product->getProcedureNumber() : 0 )
                                       << ", current = " << _curProc->getProcedureNumber()
                                       << endmsg;

   }
   else
      DataLog (log_level_predict_error) << "Current procedure is NULL" << endmsg;

   _currentDesignator = 1;
   while (_currentDesignator <= _maxDesignator)
   {
      //
      //
      //   This clause is entered if the current product is known to be good.
      //   For this case we need to search for both a previous and a next product.
      //   We force the "previous" and "next" products to the current product
      //   and use the repopulate function to fill our request.
      //
      if (starting_product)
      {
         _nextProc = _prevProc = _curProc;
         repopulate_offscreen();
         _nextProc = _offNextProc;
         _prevProc = _offPrevProc;
      }
      else
      {
         //
         //
         //  If there isn't a current product, or the current product is invalid,
         //  search through the optimizers for the first two good products.  These
         //  will become the "current" and "next" priorities.
         //
         procedureDetail* current = 0;
         procedureDetail* next    = 0;

         for (iter = _procedures.begin(); iter != _procedures.end(); iter++)
         {
            if (!(*iter)->getOptimized())
            {
               _predict.predictProcedure(*(*iter));
            }
            if ((*iter)->getStatus() == GOOD_PRODUCT)
            {
               if (current)   // this is the second one found
               {
                  next = (*iter);
                  break;
               }
               else     // first good product found
               {
                  current = (*iter);
               }
            }
         }
         //
         //
         //   If we found a product (or maybe two) then we can send them to GUI.
         //
         _prevProc = 0;
         if (current)            // null if no products are good, in which case
            _curProc = current;  // don't change the current procedure ptr.
         _nextProc = next;       // may be null if only one product was good
      }
      if (_nextProc || _prevProc) break;
      if (_curProc &&                                          // we have a current product
          (_curProc->getDesignator() == _currentDesignator) && // and it's the right designation level
          (_curProc->getStatus() == GOOD_PRODUCT)) break;
      ++_currentDesignator;
   }
   SendStatusToGui(__LINE__, REQUEST_SELECTION_SCREEN);

   repopulate_offscreen();
   send_to_gui_procedures(__LINE__);
}



void PredictTask::show_previous ()
{
   // DataLog(log_level_predict_debug) << " Predict::show_previous()" << endmsg;
   if (_prevProc)
   {
      _nextProc = _curProc;
      _curProc  = _prevProc;
      _prevProc = _offPrevProc;
   }
   repopulate_offscreen();
   send_to_gui_procedures(__LINE__);
}



void PredictTask::show_next ()
{
   //  DataLog(log_level_predict_debug) << " Predict::show_next()" << endmsg;
   if (_nextProc)
   {
      _prevProc = _curProc;
      _curProc  = _nextProc;
      _nextProc = _offNextProc;
   }
   repopulate_offscreen();
   send_to_gui_procedures(__LINE__);
}



void PredictTask::show_optimal ()
{
   //  DataLog(log_level_predict_debug) << " Predict::show_optimal()" << endmsg;

   // Note: both cases below send the same set of messages; this is important
   // for synchronization with GUI.

   if (_selectedProc)
   {
      DataLog(log_level_predict_debug) << "amapp: show_optimal called but procedure is already selected...." << endmsg;
      ShowInitialScreen();

   }
   else
   {
      // tell GUI that we're gonna want to go to the prediction screen
      SendStatusToGui(__LINE__, REQUEST_SELECTION_SCREEN);


      // Compute current, prev, and next procedures
      DataLog(log_level_predict_debug) << "amapp: show_optimal, presenting optimal" << endmsg;
      iterator         iter;
      procedureDetail* optimal = 0;


      for (iter = _procedures.begin(); iter != _procedures.end(); iter++)
      {

         if (!(*iter)->getOptimized())
         {
            _predict.predictProcedure(*(*iter));
         }

         if ((*iter)->getStatus() == GOOD_PRODUCT)
         {
            optimal = (*iter);
            break;

         }
      }

      _prevProc = 0;
      _curProc  = optimal;
      _nextProc = 0;

      repopulate_offscreen();

      // And finally, tell GUI about the new procedures
      send_to_gui_procedures(__LINE__);
   }
}



void PredictTask::select ()
{
   _predict.select();


   // For AMAP Plasma, remember current predicted vol; we need to request the
   // selection screen if this changes too much (IT 6918).  [Note: this should
   // really only be done for adjustments and procedure selections, not for
   // repredicts and cycle switches --DGS]
   _currentPlasmaVol = _selectedProc->getPredVp();

   // Send the New procedure number to GUI (CFR11 Tracker)

   int procNum = _selectedProc->getProcedureNumber();
   DataLog(log_level_predict_debug) << "PredictTask: Procedure " << procNum
                                    << " selected, send to GUI" << endmsg;

   _procedureSelected.send(procNum);

   // Log the procedures that are in the current display check list  (Note
   // two loops so the repredict logging doesn't get interleaved with the
   // logging we're actually trying to produce..
   //
   iterator iter;
   for (iter = _procedures.begin(); (*iter) != _selectedProc; iter++)
   {
      if (!(*iter)->getOptimized())
         _predict.predictProcedure(*(*iter));
   }

   DataLog(log_level_predict_info) << "Predict: display check list:";
   for (iter = _procedures.begin(); (*iter) != _selectedProc; iter++)
   {
      if ((*iter)->getStatus() == GOOD_PRODUCT)
      {
         (*iter)->setInDisplayList(1);
         DataLog(log_level_predict_info) << " " << (*iter)->getProcedureNumber();
      }
      else
      {
         (*iter)->setInDisplayList(0);
      }
   }

   DataLog(log_level_predict_info) << endmsg ;
}



void PredictTask::repopulate_offscreen ()
{
   // DataLog(log_level_predict_debug) << " Predict::repopulate_offscreen()" << endmsg;

   int              curr    = _curProc ? _curProc->getProcedureNumber() : 0;
   int              next    = _nextProc ? _nextProc->getProcedureNumber() : 0;
   int              prev    = _prevProc ? _prevProc->getProcedureNumber() : 0;

   procedureDetail* proc    = NULL;

   int              offprev = 0;

   //   To ensure consistency with duplicate products force reoptimization
   //   of higher priority products first.
   //
   //  Remember that operator[] on ProcedureList is an associative lookup by procedure number, not
   //  an array index!
   for (int i = 1; i <= curr; ++i)
   {
      proc = _procedures[i];
      if (!proc->getOptimized())
         _predict.predictProcedure(*proc);
      // We're scanning through the list anyway, we might as well look for the last good product
      // before prev (== offPrev) while we're at it.
      if (proc->getStatus() == GOOD_PRODUCT && i < prev)
         offprev = i;
   }

   //
   //   Repopulate the offscreen next procedure
   //
   _offNextProc = 0;
   if (_nextProc)
   {
      for (int i = next + 1; i <= _procedures.size(); ++i)
      {
         proc = _procedures[i];
         if (!proc->getOptimized())
            _predict.predictProcedure(*proc);
         if (proc->getStatus() == GOOD_PRODUCT)
         {
            _offNextProc = proc;
            break;
         }
      }
   }


   //
   //  Now populate the offscreen previous procedure.
   //
   _offPrevProc = 0;

   if (_prevProc && prev > 1 && offprev) // && reviter != _procedures.begin())
   {
      _offPrevProc = _procedures[offprev];
   }
}


bool PredictTask::displayListHasChanged ()
{
   //  DataLog(log_level_predict_debug) << " Predict::displayListHasChanged()" << endmsg;

   //
   //
   //   The current product may have already been optimized; but it could already be
   //   out of date so we reoptimize anyway.
   //
   if (!_selectedProc)
   {
      DataLog(log_level_predict_debug) << "Predict: displayListHasChanged reports no selected procedure" << endmsg;
      return true;
   }

   //
   //
   //   Selected procedure exists so ensure reoptimization
   //
   _predict.SetAllUnoptimized();
   _predict.predictProcedure(*_selectedProc);
   if (_selectedProc->getStatus() != GOOD_PRODUCT)
   {
      DataLog(log_level_predict_debug) << "Predict: displayListHasChanged reports selected procedure no longer valid"
                                       << endmsg;
      return true;
   }


   //
   // IT 6918
   // Need to check to see if the AMAP target changed by more than 10 mls (CC) since the last time the user confirmed
   // the procedure selection (see Predict::select() ).  If it did, GUI needs to put up the select screen.
   //

   if ( !_selectedProc->getProcPlasmaVolFixed() &&     // AMAP or MC
        (fabs(_currentPlasmaVol - _selectedProc->getPredVp()) > _cobeConfig.Vplasma1) )
   {
      DataLog(log_level_predict_info) << "Plasma volume change will generate flag.  Show predict screen"
                                      << endmsg;
      return true;
   }

   // IT 11860:  For amap plasma runs, we need to signal GUI to refresh the
   // screen if the predicted plasma volume changes enough to make a difference
   // on the screen.
   if ( fabs(_currentPlasmaVol - _selectedProc->getPredVp()) > .5 )
   {
      DataLog(log_level_predict_info) << "Plasma volume has changed from "
                                      << _currentPlasmaVol
                                      << " to "
                                      << _selectedProc->getPredVp()
                                      << "; tell GUI to refresh the predict screen"
                                      << endmsg;
      return true;
   }

   //
   //   Go through and generate the "compare list"
   //
   _currentDesignator = 1;
   int foundone = 0;

   while (_currentDesignator <= _maxDesignator)
   {
      for ( const_iterator iter = _procedures.begin();
            (*iter) != _selectedProc;
            iter++ )
      {
         if (!((*iter)->getOptimized()))
            _predict.predictProcedure(*(*iter));
         INTERNAL_PREDICT_RESULTS status = (*iter)->getStatus();
         if (status != INCORRECT_DESIGNATOR) // can check status
         {                                   //
            //
            //   The display list has changed if this one is good (and not in
            //   the previous display list).  Note (IT 13095) that as of our
            //   design review in April 2013, we no longer consider the display
            //   list to have changed in a meaningful way if a previously
            //   available higher-priority procedure becomes unavailable.
            //
            if (status == GOOD_PRODUCT)
            {
               foundone = 1;
               if (!((*iter)->getInDisplayList()))
               {
                  DataLog(log_level_predict_debug) << "Predict: displayListHasChanged reports procedure "
                                                   << (*iter)->getProcedureNumber() << " now valid"
                                                   << endmsg;
                  return true;
               }
            }
         }
      }
      if (foundone || (_selectedProc->getDesignator() == _currentDesignator)) break;
      ++_currentDesignator;
   }
   return false;
}

/* FORMAT HASH d76b327c207e1b3a0af80c5ed120f651 */

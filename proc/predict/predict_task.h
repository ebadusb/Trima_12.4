/*******************************************************************************
 *
 *              Interface between Procedure/GUI and optimizer code.
 *
 ******************************************************************************/

#ifndef PREDICT_TASK_H
#define PREDICT_TASK_H

#include "stmain.h"
#include "trimamessages.h"

// singletons
#include "procDataState.h"
#include "procedure_list.hpp"
#include "cobeconfig.h"
#include "predictConfig.h"

// CDS
#include "procruntargets_cds.h"
#include "guiProcedures_cds.h"

// Predict Engine
#include "predict.h"


class PredictTask
   : public CStdAppl
{
public: // types

   typedef ProcedureList::iterator iterator;
   typedef ProcedureList::const_iterator const_iterator;

public: // member functions

   PredictTask();
   virtual ~PredictTask();

   virtual ALARM_VALUES Init ();
   void                 Terminate ();

private:
   PredictTask(const PredictTask&);              // not implemented, noncopyable
   PredictTask& operator = (const PredictTask&); // not implemented, nonassignable

   // Incoming message callbacks

   void ProcDataReceived ();
   void ProcDataReceived (int line, DoPrediction_t value);
   void ModifyProduct ();
   void ConfigurationReceived ();
   void FeatureChangedReceived ();
   void DonorTimeReceived ();
   void FlowAdjReceived ();
   void gui_predict_request ();
   void donor_rejected ();
   void rinseback_request ();

   void ConfigurationReceived (int line);


public: // for now, until we pull the rest of this stuff out of predict

   // Message Reply Methods
   void SendPredictResponseMsgToProc (int line);
   void SendStatusToGui(int line, ProcToGuiPredictStatus_t);

private:

   void cleanup ();

   // Message receiver support functions for GUI messages
   void validate_selected_procedure (int line);
   void send_to_gui_procedures (int line);

   void ShowInitialScreen     ();
   void show_next             ();
   void show_previous         ();
   void show_optimal          ();
   void select                ();
   void repopulate_offscreen  ();
   bool displayListHasChanged ();

private:                        // member data

   ////// Incoming messages

   DoPredictionMsg              _predictRequestMsg;
   Message<ModifyProductStruct> _modifyProductMsg;
   ConfigChangedMsg             _configMsg;
   FeatureChanged               _featureChangedMsg;
   GuiToProcDonorTimeMsg        _donorTimeMsg;
   GuiToProcFlowAdjustmentMsg   _flowAdjMsg;
   GuiToProcPredictRequestMsg   _guiRequest;
   VistaDonorRejectedMsg        _donorRejectedMsg;
   RinsebackRequestMsg          _rinsebackRequestMsg;

   //////  Outgoing messages
   GuiProceduresUpdated      _guiProceduresUpdatedMsg;
   PredictRespondedMsg       _predictRespondedMsg;
   ProcedureSelectedMsg      _procedureSelected;
   ProcToGuiPredictStatusMsg _procToGuiPredictStatusMsg;
   UpdateAutoFlowTimerMsg    _updateAFTimeMsg; // msg to reset autoflow timer

   //////  Alarms
   anAlarmMsg                _AF_TimeAlert;             // autoflow, time has increased > 10 min first time
   anAlarmMsg                _AF_TimeAlarm;             // autoflow, time has increased > 10 min having _AF_TimeAlert

   ////// Singletons
   ProcedureList& _procedures;
   procedureDetail*&                   _selectedProc;
   procedureDetail*&                   _curProc;
   const int&                          _maxDesignator; // maximum designator

   const CobeConfig::CobeConfigStruct& _cobeConfig; // only used once, can we
                                                    // get rid of this somehow? --DGS
   Config& _config;

   ///// SETUP

   bool _AMAP_PLATELETS_ON;         // feature-driven


   ////// State

   procedureDetail* _offPrevProc;
   procedureDetail* _prevProc;
   procedureDetail* _nextProc;
   procedureDetail* _offNextProc;

   bool             _configurationReceived;
   int              _currentDesignator;   // current designator level to optimize



   // CDS, etc
   ProcDataState&    _procState;
   ProcAdjust_CDS    _adjustCDS;
   GuiProcedures_CDS _guiProcedures;
   ProcRun_CDS       _procRunCDS;
   Config_CDS        _configCDS;

   // synchronization
   int                    _adjustmentNumber;
   GuiProceduresUpdated_t _current_response_type; // type of the currently requested predict
   float                  _currentPlasmaVol;      // Part of the state for deciding when to request a selection screen

   // Predict Engine
   Predict _predict;
};

#endif

/* FORMAT HASH ec300b98ab86a630a22caf5d0e31dc8e */

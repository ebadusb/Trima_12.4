/*******************************************************************************
 *
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      predict.h
 *             Predictor/Optimizer manager definitions.
 *
 * ABSTRACT:   This file contains the global data object definitions for the
 *                Prediction/Optimizer.
 *
 ******************************************************************************/

#ifndef PREDICT_H
#define PREDICT_H

#include "procedure_list.hpp"

#include "cobeconfig.h"
#include "predict_cds.h"
#include "procruntargets_cds.h"


class procedureDetail;

class Predict
{
public:

   enum AMAP_PLT_STATUS
   {
      AMAP_PLT_UNK      = -1,
      AMAP_PLT_GOOD     = 0,
      AMAP_PLT_BAD      = 1,
      AMAP_PLT_FINISHED = 2
   };


public:
   Predict();
   ~Predict();

protected:
   typedef ProcedureList::iterator iterator;
   typedef ProcedureList::const_iterator const_iterator;
   ProcedureList& _procedures;

   bool           _concentrationWarningGiven;
   int            _productGivenWarning;

   const int&     _maxDesignator;          // maximum designator

public: // for Predict_Task interface
   bool _blockTargets;
protected:
   bool _currentPltMss;

public: // ditto -- these to be cleaned up in Trima 7 development
   ProductOptionStruct _currentDisplayedValues;
protected:

   Config&        _config;
   ProcDataState& _procState;

private:
   Predict(const Predict&);              // not implemented
   Predict& operator = (const Predict&); // not implemented

protected:

   //   Datastore access
   ProcRunTargets_CDS _TargetsCDS;
   //
   // Flag to alert code if we are optimizing the SELECTED/run time
   // procedure....
   procedureDetail*&                   _selectedProc;
   procedureDetail*&                   _curProc;

   const CobeConfig::CobeConfigStruct& cobeConfig;



   // convergence logging
   void logConvergenceTrail (const procedureDetail& pd) const;

   ///////////////////////////////////////
   // AMAP PLATELET GOODIES:
   // procedures:
   void predictPltAmap (procedureDetail& pd);
   void logPredictData (procedureDetail& pd) const;
   void calcMaxAMAPplateletYield ();

   AMAP_PLT_STATUS checkAMAPplt (procedureDetail& pd, float yield);

   // member data:
   bool _AMAP_PLATELETS_ON;         // feature-driven
   bool _AMAP_PLATELETS_PROCESSING; // looping and stepping over plt sizes

   bool _COMP_PLS_ON;        // feature-driven


   float _stepSize;             // AMAP platelets only (I think?)

   float _maxAMAPPyield;

   bool  _amapPltItermediatePredict;
   void LogProcedure (procedureDetail& pd);
   void LogProcedureIntermediateAmapPlt (procedureDetail& pd);
   //////////////////////////////////////



public: // for all interfaces
   ALARM_VALUES Init ();

   void UpdateProcData ();


public: // for predict_task interface



//////////////////////////////////////////////////////////////////////
//
//  PUBLIC INTERFACE?
   bool             checkIfDuplicate (const procedureDetail&);
   void             setPredictTargets ();
   void             FillInConfig (); // VISTA INTERFACE
   void             FillInProcedureConfig ();
   procedureDetail* create_procedure (const ProcedureDescription& def,
                                      int procedure_number) const;

   void FillInRegionConfig (int procindex);         // new 6.3

   int  doRepredictForAdjustments (bool isAfAdjustment = false); // VISTA INTERFACE
   void SetAllUnoptimized ();
   bool MeteringStatusLocked (const procedureDetail& pd);

   void reset_PIR_flags ();

   procedureDetail* getProcedure (int procindex) {return _procedures[procindex]; } // VISTA INTERFACE
   void             checkCassetteTypes (); // VISTA INTERFACE

   ProductOptionStruct      FillProcInfo (const procedureDetail& pd) const;
   INTERNAL_PREDICT_RESULTS PreCheck (const procedureDetail& pd);
   INTERNAL_PREDICT_RESULTS PostCheck (const procedureDetail& pd);

   void predictProcedure (procedureDetail& pd); // VISTA INTERFACE

   void cleanup ();             // Cleanup the application memory

//////////////////////////////////////////////////////////////////////

#ifndef __WIN32__
public:

   void process_adjustment (bool isAfAdjustment = false);

   void ProcData_Repredict_Only ();
   void ProcData_Donor_Info_Changed ();

   void ModifyProduct (const ModifyProductStruct&);
   void FeatureChangedReceived ();

   // the following replaces donor_rejected, it sets all targets to 0
   void reset_targets ();

   // Message receiver support functions for GUI messages
   void select ();

protected:

#endif
   // endif not WIN32
};


#endif

/* FORMAT HASH c95b8a09443030f8d88f858413f51b21 */

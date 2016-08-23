/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      disposabletest.cpp
 *
 */

#include "disposabletest.h"
#include "states.h"
#include "procdata.h"
#include "volumeacc.h"
#include "filenames.h"
#include "filereader.h"
#include "trima_datalog.h"



DEFINE_STATE(DisposableTest);

DisposableTest::DisposableTest()
   : StateAbs(),
     _centSpin(),
     _centSpinStatus(NOT_INITIALIZED)
{}

DisposableTest::~DisposableTest()
{}

int DisposableTest::preProcess ()
{
   ProcData pd;
   //
   // Accumulate volume during the disposable tests ...
   //
   pd.Update(ProcData::TEST);


   if (Software_CDS::GetInstance().getFeature(JapanFeatures) )
   {
      _centSpinStatus = _centSpin.process();
   }

   return NORMAL;
}

int DisposableTest::postProcess ()
{
   return NORMAL;
}

int DisposableTest::preEnter ()
{
   ProcData pd;

   //
   // Set the system state to DISPOSABLE_TEST state ...
   //
   pd.SystemState(DISPOSABLE_TEST);

   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::TESTED);


   pd.Orders().LockDoor();


   if (Software_CDS::GetInstance().getFeature(JapanFeatures) )
   {
      DataLog(log_level_proc_info) << "Entering parallel states...." << endmsg;
      //
      // Enter my parallel state machine ...
      //
      _centSpin.enter();
   }


   return NORMAL;
}

int DisposableTest::postExit ()
{

   ProcData pd;
   if (  !(pd.Status().CentrifugeRPM() > 0.0f)  )
   {
      pd.Orders().UnlockDoor();
   }


   if (Software_CDS::GetInstance().getFeature(JapanFeatures) )
   {
      DataLog(log_level_proc_info) << "Exiting parallel states...." << endmsg;
      //
      // Exit my parallel state machine ...
      //
      _centSpin.exit();
   }


   pd.Orders().RPM(0.0f);
   //
   // Clear the presently accumulated volumes ...
   //
   VolumeAcc::Clear();

   return NORMAL;
}

int DisposableTest::init ()
{
   //
   //
   //  Note centrifuge spin test is always initialized even if we decide not to use it.
   //  This is to avoid problems for the case where the Japan features flag is changed
   //  in config but this state has already been initialized.
   //
   DataLog(log_level_proc_info) << "Centrifuge Spin Tests states initialization...." << endmsg;

   // Since I manage this parallel state machine, I must also name it
   _centSpin.stateName("CentrifugeSpinTest");

   //
   // Object which opens and reads the state
   //  machine initialization file.
   //
   FileReader fr;
   if ( fr.init(CENT_SPIN_TEST_STATES_FILE, &_centSpin) == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_error) << "Centrifuge Spin Tests states reader error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_debug) << "Created the Centrifuge Spin Tests reader" << endmsg;

   //
   // Read and create the state list
   //
   if ( fr.readFile() == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_error) << "Centrifuge Spin Tests state machine initialization error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_debug) << "Read in the Centrifuge Spin Tests file" << endmsg;

   //
   // Initialize the centrifuge tests state
   //
   if ( _centSpin.init() == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_error) << "Centrifuge Spin Tests state initialization error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_debug) << "Initialized the Centrifuge Spin Tests state" << endmsg;

   return StateAbs::init();
}

const char* DisposableTest::currentState () const
{
   static string parallelStateName;

   parallelStateName = StateAbs::currentState();

   if (Software_CDS::GetInstance().getFeature(JapanFeatures) )
   {
      if ( _centSpinStatus == NO_TRANSITION
           && _centSpin.currentState() )
      {
         parallelStateName += " | ";
         parallelStateName += _centSpin.currentState();
      }
   }

   return parallelStateName.c_str();
}

/* FORMAT HASH bfdb92c55fb871249a64968ba0b946e5 */

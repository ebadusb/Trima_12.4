/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      loadcassette.cpp
 *
 */

#include "loadcassette.h"
#include "filenames.h"
#include "filereader.h"
#include "guiproc.h"
#include "trimamessages.h"
#include "trima_datalog.h"
#include "procdata.h"
#include "cobeconfig.h"    // for all the cobe config values
#include "xif_cds.h"
#include "proctrimaset_cds.h"

DEFINE_STATE(LoadCassette);

LoadCassette::LoadCassette()
   : StateAbs(),
     _CentrifugeTests(),
     _CentrifugeTestsStatus(NO_TRANSITION)
{}

LoadCassette::~LoadCassette()
{}

int LoadCassette::init ()
{
   //
   // Object which opens and reads the state
   //  machine initialization file.
   //
   FileReader fr;
   if ( fr.init(CENT_TESTS_STATES_FILE, &_CentrifugeTests) == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_error) << "Centrifuge Tests states reader error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_debug) << "Created the CENT_TESTS_STATES_FILE reader" << endmsg;

   //
   // Read and create the state list
   //
   if ( fr.readFile() == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_error) << "Centrifuge Tests state machine initialization error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_debug) << "Read in the CENT_TESTS_STATES_FILE file" << endmsg;

   //
   // Initialize the centrifuge tests state
   //
   if ( _CentrifugeTests.init() == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_error) << "Centrifuge Tests state initialization error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_debug) << "Initialized the Centrifuge Tests state" << endmsg;

   return StateAbs::init();
}

const char* LoadCassette::currentState () const
{
   const char*   t = StateAbs::currentState();
   static string parallelStateName;
   parallelStateName = t;

   if (    _CentrifugeTestsStatus == NO_TRANSITION
           && _CentrifugeTests.currentState() )
   {
      parallelStateName += " | ";
      parallelStateName += _CentrifugeTests.currentState();
   }

   return parallelStateName.c_str();
}

int LoadCassette::preProcess ()
{
   //
   // Perform processing on the parallel state machine ...
   //
   _CentrifugeTestsStatus = _CentrifugeTests.process();

   return NORMAL;
}

int LoadCassette::postProcess ()
{
   return NORMAL;
}

int LoadCassette::preEnter ()
{
   ProcData pd;

   // Restrict the messaging to vista
   External_IF_CDS externalIFCDS(ROLE_RW);
   externalIFCDS.cassetteLowered.Set(true);

   //
   // Enter my parallel state machine ...
   //
   _CentrifugeTests.enter();

   CassettePositionStatusMsg posmsg(MessageBase::SEND_LOCAL);
   posmsg.send(CASSETTE_LOWERING);

   // only at startup
   if (pd.TrimaSet().CassetteState.Get() == ProcTrimaSet_CDS::DRY)
   {
      DataLog(log_level_proc_debug) << "Cook the LEDs!" << endmsg;

      // Warm up the detector.  Set the drive values to max output.
      pd.Orders().RBCRed(255);
      pd.Orders().RBCGreen(255);
   }

   return NORMAL;
}

int LoadCassette::postExit ()
{
   //
   // Exit my parallel state machine ...
   //
   _CentrifugeTests.exit();


   return NORMAL;
}

/* FORMAT HASH 21877d4a7ada80b6bceb308d28c1b044 */

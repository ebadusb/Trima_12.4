/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssParallelProcessing.cpp
 *
 */

#include "mssParallelProcessing.h"
#include "filenames.h"
#include "filereader.h"
#include "guiproc.h"
#include "trimamessages.h"
#include "trima_datalog.h"
#include "procdata.h"
#include "states.h"


DEFINE_STATE(MSSParallelProcessing);

MSSParallelProcessing::MSSParallelProcessing()
   : StateAbs(),
     _MSSPrimeState(),
     _MSSPrimeStatus(NO_TRANSITION),
     transitionNow(false),
     alreadySetAasStartTime(false),
     _MSSAdditionStatus(NO_TRANSITION)
{}

MSSParallelProcessing::~MSSParallelProcessing()
{}

int MSSParallelProcessing::init ()
{
   //
   // Object which opens and reads the state
   //  machine initialization file.
   //
   FileReader fr;
   if ( fr.init("/trima/proc/mssPltStates", &_MSSPrimeState) == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_info) << "mssPltStates  states reader error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_info) << "Created the /trima/proc/mssPltStates reader" << endmsg;


   FileReader fr2;
   if ( fr2.init("/trima/proc/mssRbcStates", &_MSSAdditionState) == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_info) << "_MSSAdditionState  states reader error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_info) << "Created the /trima/proc/mssRbcStates reader" << endmsg;


   //
   // Read and create the state list
   //
   if ( fr.readFile() == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_info) << "_MSSPrimeState state machine initialization error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_info) << "Read in the /trima/proc/mssPltStates file" << endmsg;


   //
   // Read and create the state list
   //
   if ( fr2.readFile() == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_info) << "_MSSAdditionState state machine initialization error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_info) << "Read in the /trima/proc/mssRbcStates file" << endmsg;


   //
   // Initialize the centrifuge tests state
   //
   if ( _MSSPrimeState.init() == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_info) << "_MSSPrimeState state initialization error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_info) << "Initialized the _MSSPrimeState" << endmsg;


   //
   // Initialize the centrifuge tests state
   //
   if ( _MSSAdditionState.init() == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_info) << "_MSSAdditionState state initialization error" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_info) << "Initialized the _MSSAdditionState" << endmsg;


   return StateAbs::init();
}

const char* MSSParallelProcessing::currentState () const
{
   const char*   t = ""; /*StateAbs::currentState()*/
   static string parallelStateName;
   parallelStateName = t;

   ProcData pd;

   if (pd.RunTargets().MeterPlatelets.Get())
   {

      if (    _MSSPrimeStatus == NO_TRANSITION
              && _MSSPrimeState.currentState() )
      {
         parallelStateName += _MSSPrimeState.currentState();
      }
   }

   if (pd.RunTargets().MeterRbcs.Get())
   {
      if (    _MSSAdditionStatus == NO_TRANSITION
              && _MSSAdditionState.currentState() )
      {
         // add a separator is doing both
         if (   pd.RunTargets().MeterPlatelets.Get()
                && pd.RunTargets().MeterRbcs.Get()
                && parallelStateName.length() > 0
                )
         {
            parallelStateName += " || ";
         }

         parallelStateName += _MSSAdditionState.currentState();
      }
   }

   return parallelStateName.c_str();
}



int MSSParallelProcessing::transitionStatus ()
{

   ProcData pd;
   // check if PFR and we need to do this yet!
   if (pd.Run().mssInPFR)    // if we're in PFR
   {
      if ( pd.Run().currentMSSSubState != SS_MSSPARALLELPROC  )
         return 1;

      DataLog(log_level_proc_info) << "MSSParallelProcessing recovering from PFR" << endmsg;
   }


   if (transitionNow)
   {
      pd.MSSRunTime().inactivate();     // stop timer Metering is done!
      return TRANSITION;
   }
   else
   {
      return NO_TRANSITION;
   }
}

int MSSParallelProcessing::preProcess ()
{
   //
   // Perform processing on the parallel state machine ...
   //
   _MSSPrimeStatus    = _MSSPrimeState.process();
   _MSSAdditionStatus = _MSSAdditionState.process();

   ProcData pd;

   if (
      ((_MSSPrimeStatus == TRANSITION_NOSTATE )   || !pd.Run().MeterPlatelets_OK)
      && ((_MSSAdditionStatus == TRANSITION_NOSTATE) || !pd.Run().MeterRbcs_OK)
      )
   {
      transitionNow = true;
   }

   return NORMAL;
}

int MSSParallelProcessing::postProcess ()
{
   return NORMAL;
}

int MSSParallelProcessing::preEnter ()
{


   ProcData pd;

   DataLog(log_level_proc_info) << "MSSParallelProcessing start" << endmsg;

   if (!alreadySetAasStartTime)
   {
      alreadySetAasStartTime = true;

      time_t aasStartTime;
      time(&aasStartTime);

      pd.MakeRunWritable();
      pd.Run().ProcStartAasTime.Set(aasStartTime);
      pd.Run().MeteringStarted.Set(true);
      pd.MakeRunReadable();

      // format the time for external product data mining.
      //  dont touch unless you want to loose some fingers...
      struct tm* timeinfo = 0;
      char       tbuffer [80];
      timeinfo = localtime (&aasStartTime);
      strftime (tbuffer, 80, "%Y/%m/%d_%H:%M:%S", timeinfo);
      DataLog(log_level_proc_info) << "AAS start time: " << tbuffer << endmsg;
   }

   if (!pd.Run().mssInPFR)   // if we're NOT  in PFR
   {
      pd.MakeRunWritable();
      // setup this for PFR
      pd.Run().currentMSSSubState = SS_MSSPARALLELPROC;
      pd.MakeRunReadable();
   }
   else
   {

      DataLog(log_level_proc_info) << "PFR back to MSSParallelProcessing, forking into seperate states" << endmsg;
      // back to where we PFR'ed reset flag and carry on
      // pd.MakeRunWritable();
      //    pd.Run().mssInPFR.Set(false);
      // pd.MakeRunReadable();

   }


   _MSSPrimeState.enter();
   _MSSAdditionState.enter();

   return NORMAL;
}

int MSSParallelProcessing::postExit ()
{


   DataLog(log_level_proc_info) << "MSSParallelProcessing exit" << endmsg;
   //
   // Exit my parallel state machine ...
   //
   _MSSPrimeState.exit();
   _MSSAdditionState.exit();

   return NORMAL;
}

/* FORMAT HASH 2a412fdc6c8c4bc9a895252429503073 */

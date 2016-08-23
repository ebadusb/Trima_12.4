/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      threeSpillovers.cpp
 *
 */

#include "threeSpillovers.h"
#include "alarms.h"
#include "procdata.h"
#include "recoverytypes.h"

#include "cobeconfig.h"

DEFINE_OBJ(ThreeSpillover);

ThreeSpillover::ThreeSpillover()
   : _threeSpilloversDetected(PERSISTENT_SPILLOVER_ALARM),
     _spilloverCounter(0),
     _inRBC(false),
     _inWBC(false)
{}

ThreeSpillover::~ThreeSpillover()
{}

void ThreeSpillover::Monitor ()
{
   ProcData pd;


   bool NONE_REC = (bool)(RecoveryTypes::RecoveryId(pd.RecoveryName().c_str() ) == RecoveryTypes::None);
   bool RBCSpill = (bool)(RecoveryTypes::RecoveryId(pd.RecoveryName().c_str() ) == RecoveryTypes::RBCSpillover);
   bool WBCPurge = (bool)(RecoveryTypes::RecoveryId(pd.RecoveryName().c_str() ) == RecoveryTypes::WBCChamberPurge);


   checkConsecutiveSpillover(NONE_REC, RBCSpill, WBCPurge);

   if ( _spilloverCounter >= CobeConfig::data().MaxConsecutiveSpillovers  )
   {
      _threeSpilloversDetected.setAlarm();
   }

}

void ThreeSpillover::checkConsecutiveSpillover (bool n, bool r, bool w)
{

   if (r)
   {
      if (!_inRBC)
      {
         DataLog(log_level_proc_info) << "in RBC Spillover. Watching for continuous RBC-WBC-WBC-WBC spillovers. " << endmsg;
      }

      _inRBC = true;
      _inWBC = false;

   }


   if (w)
   {
      if (!_inWBC)
      {
         _spilloverCounter++;
         DataLog(log_level_proc_info) << "WBC purge count: " <<  _spilloverCounter << endmsg;
      }
      _inWBC = true;

   }
   else
   {

      _inWBC = false;
   }



   if (n)
   {
      _inRBC = false;
      _inWBC = false;

      if (_spilloverCounter != 0)
         DataLog(log_level_proc_info) << "Recoveries ended WBC purge counter reset"  << endmsg;

      _spilloverCounter = 0;
   }

}

/* FORMAT HASH 5cfd6ce0582240b79cb43f2dfa615107 */

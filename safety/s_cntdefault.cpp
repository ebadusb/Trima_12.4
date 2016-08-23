/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 *
 * TITLE:      s_cntdefault.cpp, Default Container base class
 *
 */

#include "trima_datalog.h"
#include "hw.h"
#include "chw.hpp"
#include "shw.hpp"
#include "error.h"
#include "s_set.hpp"
#include "s_bag.hpp"

#include "s_cont.hpp"

/**
* Base Container Interface Implementation
**/

// calculate the volumes
void DefaultContainer::UpdateContainer (char cCycle)
{
   VOLUME_VALID result;

   result = _bag->UpdateVol(cCycle);

   if (result == VOLUME_FLOW_ERROR)
   {
      DataLog(log_level_safety_alarm_detail) << _containerName
                                             << " flow with unknown flowIO not allowed: "
                                             << _bag->_flowIO
                                             << endmsg;
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);
   }
   else if (result == VOLUME_MIN_EXCEEDED)
   {
      DataLog(log_level_safety_alarm_detail) << _containerName
                                             << " Minimum Volume Exceeded: "
                                             << _bag->_accumulatedVolume
                                             << endmsg;
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, _minimumAlarm);
   }
   else if (result == VOLUME_MAX_EXCEEDED)
   {
      DataLog(log_level_safety_alarm_detail) << _containerName
                                             << " Maximum Volume Exceeded: "
                                             << _bag->_accumulatedVolume
                                             << endmsg;
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, _maximumAlarm);
   }

   // Call Container-specific PFR routine
   SaveToPFR();
}

float DefaultContainer::GetVolume (VOL_TYPE type) const
{
   float volume = 0.0f;

   if (_bag != NULL)
   {
      switch (type)
      {
         case VOL_DELTA :
            volume = _bag->_deltaVolume;
            break;
         case VOL_ACCUM :
            volume = _bag->_accumulatedVolume;
            break;
         case VOL_DRAW_CYCLE :
            volume = _bag->_drawCycleVolume;
            break;
         case VOL_RETURN_CYCLE :
            volume = _bag->_returnCycleVolume;
            break;
         case VOL_DRAW_CYCLE_INFUSION :
            volume = _bag->_drawCycleVolumeInfusion;
            break;
         case VOL_RETURN_CYCLE_INFUSION :
            volume = _bag->_returnCycleVolumeInfusion;
            break;
         default :
         {
            _FATAL_ERROR(__FILE__, __LINE__, "Request for unknown VOL_TYPE");
            break;
         }
      }
   }
   else
   {
      DataLog(log_level_safe_exec_info) << "No bag" << endmsg;
      volume = 0.0f;
   }

   return volume;
}

void DefaultContainer::DumpContainer (DataLog_Level& log_level) const
{
   if (_bag != NULL)                         // a one pump bag (i.e.platelet)
      DataLog(log_level) << _containerName
                         << " flowIO="
                         << _bag->_flowIO
                         << " deltaV="
                         << _bag->_deltaVolume
                         << " accumV="
                         << _bag->_accumulatedVolume
                         << " drawV="
                         << _bag->_drawCycleVolume
                         << " returnV="
                         << _bag->_returnCycleVolume
                         << " drawVI="
                         << _bag->_drawCycleVolumeInfusion
                         << " returnVI="
                         << _bag->_returnCycleVolumeInfusion
                         << " valveIO="
                         << _bag->_valveIO
                         << " valvePosCtl="
                         << _bag->_myValve._positionC
                         << " valvePosSfT="
                         << _bag->_myValve._positionS
                         << endmsg;
   else
      DataLog(log_level) << "No bag" << endmsg;
}

// reset cycle volumes and valve counters
void DefaultContainer::ResetCycleVol (char cCycle)
{
   if (_bag != NULL)
      _bag->ResetCycleVol(cCycle);
}

/* FORMAT HASH e72190f4662f5f6bdeb0c076813b3502 */

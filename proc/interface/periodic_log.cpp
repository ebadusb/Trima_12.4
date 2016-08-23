/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include <taskVarLib.h>
#include <private/memPartLibP.h>
#include <memLib.h>
#include <string>

#include "alarms.h"
#include "datalog_periodic.h"
#include "failure_debug.h"
#include "periodic_log.h"
#include "substatemap.h"

DataLog_SetHandle PeriodicLog::_set      = DATALOG_NULL_SET_HANDLE;
DataLog_SetHandle PeriodicLog::_eorSet   = DATALOG_NULL_SET_HANDLE;

bool              PeriodicLog::m_enabled = true;

float currMemPct ()  { return 100.0 * ((float)memSysPartId->curWordsAllocated)   / ((float)memSysPartId->totalWords); }
float maxMemPct ()   { return 100.0 * ((float)DBG_GetMaxWordsAllocated())        / ((float)memSysPartId->totalWords); }
long  maxContiguousBytes () { return memPartFindMax(memSysPartId); }

void PeriodicLog::init (AbsModalData* data, double interval)
{
   m_enabled = data->isPeriodicLoggingEnabled();

   if (m_enabled)
   {
      if (_set == DATALOG_NULL_SET_HANDLE)
      {
         switch (data->getType())
         {
            case AbsModalData::NMDATA :
               datalog_CreatePeriodicSet("Proc_RunData", &_set);
               datalog_CreatePeriodicSet("Proc_EORData1", &_eorSet);
               break;

            case AbsModalData::SMDATA :
               datalog_CreatePeriodicSet("ServiceMode_Data", &_set);

            /* Add new defined types here */
            case AbsModalData::UNDEFINED :
            default :
               datalog_CreatePeriodicSet("System_Data", &_set);
         }

         // Call the virtual function based on the abstract class
         data->definePeriodicSet(_set);

         // Define the data set for End Of Run Summary info
         data->defineEORSet(_eorSet);


         // Add the standard stuff
         datalog_AddRef(_set, memSysPartId->totalWords,          "MemSize",              "total memory size (words)",        "%u");
         datalog_AddRef(_set, memSysPartId->curWordsAllocated,   "MemUsed",              "current allocated memory (words)", "%u");
         datalog_AddFunc(_set, DBG_GetMaxWordsAllocated,         "MemMax",               "max allocated memory (words)",     "%u");
         datalog_AddFunc(_set, currMemPct,                       "MemUsedPct",           "current allocated memory (%)",     "%.2f");
         datalog_AddFunc(_set, maxMemPct,                        "MemMaxPct",            "max allocated memory (%)",         "%.2f");
         datalog_AddFunc(_set, maxContiguousBytes,               "MemMaxContiguousBytes",            "max available contiguous bytes",           "%u");

#if 0
         datalog_AddRef(_set, memSysPartId->curBlocksAllocated,  "MemBlocksUsed",        "current number of allocated blocks", "%u");
         datalog_AddRef(_set, memSysPartId->cumWordsAllocated,   "CumMemUsed",           "cumulative allocated memory (words)", "%u");
         datalog_AddRef(_set, memSysPartId->cumBlocksAllocated,  "CumMemBlocksUsed",     "cumulative number of allocated blocks", "%u");
#endif

         datalog_AddFunc(_set, DBG_GetIdlePercent,                   "CPUIdle",              "percent idle CPU time", "%d");

         datalog_AddFunc(_set, datalog_GetFreeBufferBytes,       "FreeLogMem",           "free log buffer memory (bytes)",                   "%u");
         datalog_AddFunc(_set, datalog_GetTraceMissedBytes,      "TraceLogMissed",       "cumulative lost trace log (bytes)",                "%u");
         datalog_AddFunc(_set, datalog_GetCriticalMissedBytes,   "CriticalLogMissed",    "cumulative lost critical log (bytes)",                 "%u");

#if 0
         datalog_AddFunc(_set, datalog_GetTraceBufferBytes,      "TraceLogMem",          "used trace log buffer memory (bytes)",             "%u");
         datalog_AddFunc(_set, datalog_GetTraceWrittenBytes,     "TraceLogWritten",      "cumulative data written to trace log (bytes)", "%u");
         datalog_AddFunc(_set, datalog_GetCriticalBufferBytes,   "CriticalLogMem",       "used critical log buffer memory (blocks)",             "%u");
         datalog_AddFunc(_set, datalog_GetCriticalWrittenBytes,  "CriticalLogWritten",   "cumulative data written to critical log (bytes)", "%u");
#endif

#ifdef DATALOG_BUFFER_STATISTICS
         datalog_AddFunc(_set, datalog_GetMinFreeBufferBytes,     "MinFreeLogMem",        "minimum free log buffer memory (blocks)",              "%u");
         datalog_AddFunc(_set, datalog_GetMaxTraceBufferBytes,    "MaxTraceLogMem",   "maximum used trace log buffer memory (blocks)",        "%u");
         datalog_AddFunc(_set, datalog_GetMaxCriticalBufferBytes, "MaxCriticalLogMem", "maximum used critical log buffer memory (blocks)",    "%u");

         datalog_AddFunc(_set, datalog_GetAvgFreeBufferBytes,     "AvgFreeLogMem",        "average free log buffer memory (blocks)",              "%u");
         datalog_AddFunc(_set, datalog_GetAvgTraceBufferBytes,    "AvgTraceLogMem",   "average used trace log buffer memory (blocks)",        "%u");
         datalog_AddFunc(_set, datalog_GetAvgCriticalBufferBytes, "AvgCriticalLogMem", "average used critical log buffer memory (blocks)",    "%u");
#endif   /* ifdef DATALOG_BUFFER_STATISTICS */

         datalog_ForcePeriodicOutput(_set, true);
         datalog_ForcePeriodicOutput(_eorSet, true);

         if (interval > 0)
         {
            datalog_SetPeriodicOutputInterval(_set, (long)(interval * 1000));
            datalog_SetPeriodicOutputInterval(_eorSet, (long)(interval * 1000));
         }
      }
   }
   else
   {
      disablePeriodicity();
   }
}



void PeriodicLog::forceOutput (void)
{
   // only force logging if we're actually enabled
   if (m_enabled)
   {
      datalog_ForcePeriodicOutput(_set);
      datalog_ForcePeriodicOutput(_eorSet);
   }
}

void PeriodicLog::getInterval (double& intervalInSeconds)
{
   long milliSeconds;
   datalog_GetPeriodicOutputInterval(_set, &milliSeconds);
   intervalInSeconds = milliSeconds / 1000.0;
}

void PeriodicLog::enablePeriodicity (double intervalInSeconds)
{
   const long milliSeconds = long(intervalInSeconds * 1000);

   datalog_SetPeriodicOutputInterval(_set, milliSeconds);
   datalog_SetPeriodicOutputInterval(_eorSet, milliSeconds);

   datalog_EnablePeriodicOutput(_set);
   datalog_EnablePeriodicOutput(_eorSet);
}

void PeriodicLog::disablePeriodicity (void)
{
   // Always check for null handle.  datalog code does not check for a null handle.
   if (_set != DATALOG_NULL_SET_HANDLE)
   {
      datalog_DisablePeriodicOutput(_set);
   }

   // Always check for null handle.  datalog code does not check for a null handle.
   if (_eorSet != DATALOG_NULL_SET_HANDLE)
   {
      datalog_DisablePeriodicOutput(_eorSet);
   }
}

const char* PeriodicLog::FieldConverter::convertSystemState (const State_names& systemState)
{
   const char* result = SubstateMap::ssStr(systemState);
   return result;
}

const char* PeriodicLog::FieldConverter::convertAlarmActive (const bool& alarmActive)
{
   const char* result = "";

   switch ( alarmActive )
   {
      case true :  result = "true"; break;
      case false :
      default :    result = "false"; break;
   }

   return result;
}

const char* PeriodicLog::FieldConverter::convertCassetteCommand (const HAL_CASSETTEORDERS& order)
{
   const char* result = "";

   switch ( order )
   {
      case HAL_CASSETTEDOWN :  result         = "L";               break;
      case HAL_CASSETTEUP :     result        = "R";               break;
      default :                        result = "don't move";  break;
   }

   return result;
}

char PeriodicLog::FieldConverter::convertCassetteStatus (const BaseElement<HAL_CASSETTESTATE>& status)
{
   char result;

   switch ( status.Get() )
   {
      case HAL_CASSETTE_DOWN : result         = 'L'; break;
      case HAL_CASSETTE_UP :    result        = 'R'; break;
      default :                        result = 'U'; break;
   }

   return result;
}

int PeriodicLog::FieldConverter::convertCassetteID (const BaseElement<cassette::cassetteType>& id)
{
   return (int)id.Get();
}

float PeriodicLog::FieldConverter::convertCentCurrent (const BaseElement<int>& value)
{
   return 0.002 * value.Get();
}

float PeriodicLog::FieldConverter::convertCurrent (const BaseElement<int>& value)
{
   return 0.001961 * value.Get();
}

int PeriodicLog::FieldConverter::convertDoorCommand (const HAL_DOORORDERS& order)
{
   return (order == HAL_DOORLOCK) ? 1 : 0;
}

const char* PeriodicLog::FieldConverter::convertDoorStatus (const BaseElement<HAL_DOORSTATE>& status)
{
   const char* result = "";

   switch ( status.Get() )
   {
      case HAL_DOOR_OPEN :                 result         = "O+U"; break;
      case HAL_DOOR_LOCKED_STATE :     result             = "O+L"; break;
      case HAL_DOOR_CLOSED_STATE :     result             = "C+U"; break;
      case HAL_DOOR_CLOSED_AND_LOCKED : result            = "C+L"; break;
      default :                                    result = "err"; break;
   }

   return result;
}

int PeriodicLog::FieldConverter::convertDrawCycle (const BaseElement<bool>& drawCycle)
{
   return (drawCycle.Get()) ? 1 : 0;
}

char PeriodicLog::FieldConverter::convertFluidDetected (const BaseElement<HAL_FLUIDDETECT>& status)
{
   char result;

   switch ( status.Get() )
   {
      case HAL_FLUID_DETECTED :    result         = '1'; break;
      case HAL_AIR_DETECTED :      result         = '0'; break;
      default :                            result = 'E'; break;
   }

   return result;
}

char PeriodicLog::FieldConverter::convertReservoirStatus (const BaseElement<HAL_RESERVOIRSTATE>& reservoir)
{
   char result;

   switch ( reservoir.Get() )
   {
      case HAL_RESERVOIR_ERROR :   result         = 'F'; break;
      case HAL_RESERVOIR_EMPTY :  result          = 'E'; break;
      case HAL_RESERVOIR_MIDDLE : result          = 'L'; break;
      case HAL_RESERVOIR_HIGH :    result         = 'H'; break;
      default :                            result = 'U'; break;
   }

   return result;
}

float PeriodicLog::FieldConverter::convertTemperature (const BaseElement<int>& value)
{
   return 0.1 * value.Get() - 50.0;
}

char PeriodicLog::FieldConverter::convertValveOrder (const HAL_VALVESTATE& order)
{
   char result;

   switch ( order )
   {
      case HAL_VALVE_COLLECT : result         = 'C'; break;
      case HAL_VALVE_OPEN :     result        = 'O'; break;
      case HAL_VALVE_RETURN :   result        = 'R'; break;
      default :                        result = 'U'; break;
   }

   return result;
}

char PeriodicLog::FieldConverter::convertValveStatus (const BaseElement<HAL_VALVESTATE>& state)
{
   char result;

   switch ( state.Get() )
   {
      case HAL_VALVE_COLLECT : result         = 'C'; break;
      case HAL_VALVE_OPEN :     result        = 'O'; break;
      case HAL_VALVE_RETURN :   result        = 'R'; break;
      default :                        result = 'U'; break;
   }

   return result;
}

float PeriodicLog::FieldConverter::convertVoltage (const BaseElement<int>& value)
{
   return 0.001 * value.Get();
}



const char* PeriodicLog::FieldConverter::convertCassetteState (const BaseElement<ProcTrimaSet_CDS::CassetteStateType>& state)
{

   const char* result = "";

   switch ( state )
   {
      case ProcTrimaSet_CDS::DRY :                     result = "DRY";          break;
      case ProcTrimaSet_CDS::TESTED :                  result = "TESTED";       break;
      case ProcTrimaSet_CDS::AC :                      result = "AC";           break;
      case ProcTrimaSet_CDS::ACPRIMED :                result = "ACPRIMED";     break;
      case ProcTrimaSet_CDS::BLOOD :                   result = "BLOOD";        break;
      case ProcTrimaSet_CDS::BLOODPRIME :              result = "BLOODPRIME";   break;
      case ProcTrimaSet_CDS::PRIMED :                  result = "PRIMED";       break;
      case ProcTrimaSet_CDS::RINSEBACK :               result = "RINSEBACK";    break;
      case ProcTrimaSet_CDS::USED :                    result = "USED";         break;
      case ProcTrimaSet_CDS::STORAGE :                 result = "STORAGE";      break;
      case ProcTrimaSet_CDS::STORAGE_COMPLETE :        result = "STORAGE COMPLETE";     break;
      default :                                        result = "unknown";      break;
   }

   return result;

}


const char PeriodicLog::FieldConverter::convertSafetyReservoirStatus (const SHW_RESERVOIR_STATES& status)
{

   char result;

   switch ( status  )
   {
      case SHW_RESERVOIR_EMPTY :      result = 'E'; break;
      case SHW_RESERVOIR_LOW :        result = 'L'; break;
      default :                      result  = 'U'; break;
   }

   return result;



}

/* FORMAT HASH 99aa7b692708b87185994bc631c3a9e8 */

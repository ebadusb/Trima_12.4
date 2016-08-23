/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/proc/interface/rcs/periodic_log.h 1.13 2007/08/23 19:42:03Z spriusb Exp dslausb $
 * $Log: periodic_log.h $
 * Revision 1.13  2007/08/23 19:42:03Z  spriusb
 * Fix for 7742: Initialization for periodic logging more flexible.  Now supports service mode.  Also did some refactoring.
 * Revision 1.11  2007/05/11 19:45:15Z  dslausb
 * IT 7957 - Update GUI button press columns in dlog
 * Revision 1.10  2007/04/27 21:56:52Z  dslausb
 * IT 7957 - Add another column to periodic data with additional GUI button push info.
 * Revision 1.9  2007/03/08 21:11:19Z  jheiusb
 * 7907 -- disable leak detector after MSS addition
 * Revision 1.8  2003/08/11 15:42:37Z  ms10234
 * 6177 - add the chamber flow commanded and actual to the periodic log.
 * Revision 1.7  2003/06/19 15:52:43Z  ms10234
 * 6127 - added display function for GUI buttons
 * Revision 1.6  2003/04/24 16:12:06Z  jl11312
 * - corrections/additions to periodic log
 * Revision 1.5  2003/03/31 15:44:52Z  ms10234
 * 5935 - added conversion function to conver the system state to a string instead of an enum value
 * Revision 1.4  2003/02/25 16:15:21Z  jl11312
 * - modified buffering scheme to help prevent buffer overruns
 * Revision 1.3  2003/01/23 18:58:23  ms10234
 * Added periodic logging data
 * Revision 1.2  2002/12/05 18:57:49Z  jl11312
 * - added more periodic log data
 * Revision 1.1  2002/11/18 19:55:13  jl11312
 * Initial revision
 *
 */

#ifndef _PERIODIC_LOG_INCLUDE
#define _PERIODIC_LOG_INCLUDE

#include "trima_datalog.h"
#include "modaldata.hpp"
#include "states.h"
#include "haldefs.h"
#include "proctrimaset_cds.h"
#include "shw.hpp"


class PeriodicLog
{
public:
   static void init (AbsModalData* data, double periodicInterval = 0.0 /* in seconds */);
   static void forceOutput (void);
   static void getInterval (double& interval);   // Interval between logging cycles in seconds
   static void enablePeriodicity (double periodicInterval /* in seconds */);
   static void disablePeriodicity (void);

   class FieldConverter
   {
   public:
      typedef unsigned long convertBytesFunc (void);

      static const char* convertSystemState (const State_names& systemState);
      static const char* convertAlarmActive (const bool& alarmActive);
      static const char* convertCassetteCommand (const HAL_CASSETTEORDERS& order);
      static char        convertCassetteStatus (const BaseElement<HAL_CASSETTESTATE>& status);
      static int         convertCassetteID (const BaseElement<cassette::cassetteType>& id);
      static float       convertCentCurrent (const BaseElement<int>& value);
      static float       convertCurrent (const BaseElement<int>& value);
      static int         convertDoorCommand (const HAL_DOORORDERS& order);
      static const char* convertDoorStatus (const BaseElement<HAL_DOORSTATE>& status);
      static int         convertDrawCycle (const BaseElement<bool>& drawCycle);
      static char        convertFluidDetected (const BaseElement<HAL_FLUIDDETECT>& status);
      static char        convertReservoirStatus (const BaseElement<HAL_RESERVOIRSTATE>& reservoir);
      static float       convertTemperature (const BaseElement<int>& value);
      static char        convertValveOrder (const HAL_VALVESTATE& order);
      static char        convertValveStatus (const BaseElement<HAL_VALVESTATE>& state);
      static float       convertVoltage (const BaseElement<int>& value);
      static const char* convertCassetteState (const BaseElement<ProcTrimaSet_CDS::CassetteStateType>& state);
      static const char  convertSafetyReservoirStatus (const SHW_RESERVOIR_STATES& status);


   };

private:
   static DataLog_SetHandle _set;
   static DataLog_SetHandle _eorSet;

   static bool              m_enabled;
};

#endif /* ifndef _PERIODIC_LOG_INCLUDE */

/* FORMAT HASH 14d788cfe58b61187136ed1de99e45f9 */

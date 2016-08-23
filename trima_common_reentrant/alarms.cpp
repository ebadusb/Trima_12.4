/*******************************************************************************
 *
 * Copyright (c) 2000 by Gambro BCT: Inc.  All rights reserved.
 *
 * TITLE:      alarms.h
 *
 * ABSTRACT:   This file contains definitions that exist throughout the procedure
 *                domain for alarm handling.
 *
 ******************************************************************************/

/**********************************************************************************
 NOTE: This file is now shared with Vista. Take care when adding new dependencies
 **********************************************************************************/

#include <vxworks.h>
#include "alarms.h"
#include "alarmcontainer.h"


DataLog_Stream & operator << (DataLog_Stream& os, ALARM_VALUES id)
{
   if (id >= 0)                     // valid array index
   {
      static AlarmTable alarmTable;

      AlarmStruct&      alarm = alarmTable[id];

      os << alarm.getDescription();
   }
   else
   {
      os << "None";
   }

   os << "(" << dec << (int)id << ")";

   return os;
}

DataLog_Stream& operator << (DataLog_Stream& os, ALARM_STATES id)
{
   switch (id)
   {
      case LATCHED :
         os << "LATCHED";
         break;
      case ACTIVE :
         os << "ACTIVE";
         break;
      case CLEARED :
         os << "CLEARED";
         break;
   }

   os << "(" << dec << (int)id << ")";

   return os;
}

DataLog_Stream& operator << (DataLog_Stream& os, ALARM_TYPES id)
{
   switch (id)
   {
      case A2 :
         os << "A2";
         break;
      case A1 :
         os << "A1";
         break;
      case R2 :
         os << "R2";
         break;
      case R1 :
         os << "R1";
         break;
      case OPERATOR_ATTENTION :
         os << "OPERATOR_ATTENTION";
         break;
   }

   os << "(" << dec << (int)id << ")";

   return os;
}

DataLog_Stream& operator << (DataLog_Stream& os, ALARM_DISPLAY_TYPES id)
{
   switch (id)
   {
      case DISP_A2 :
         os << "DISP_A2";
         break;
      case DISP_A1 :
         os << "DISP_A1";
         break;
      case DISP_R2 :
         os << "DISP_R2";
         break;
      case DISP_FLOATING_HIGHPRI :
         os << "DISP_FLOATING_HIGHPRI";
         break;
      case DISP_FLOATING :
         os << "DISP_FLOATING";
         break;
      case DISP_R1 :
         os << "DISP_R1";
         break;
      case DISP_OPERATOR :
         os << "DISP_OPERATOR";
         break;
      case DISP_CFR11 :
         os << "DISP_CFR11";
         break;
      case DISP_NONE :
         os << "DISP_NONE";
         break;
      case DISP_STATUS_LINE :
         os << "DISP_NONE";
         break;
   }

   os << "(" << dec << (int)id << ")";

   return os;
}

DataLog_Stream& operator << (DataLog_Stream& os, ALARM_ACTIONS id)
{
   switch (id)
   {
      case STOP_ALL :
         os << "STOP_ALL";
         break;
      case SLOW_CENTRIFUGE :
         os << "SLOW_CENTRIFUGE";
         break;
      case STOP_PUMPS :
         os << "STOP_PUMPS";
         break;
      case STOP_PUMPS_CLOSE_RBC :
         os << "STOP_PUMPS_CLOSE_RBC";
         break;
      case STOP_DONOR_ACCESS :
         os << "STOP_DONOR_ACCESS";
         break;
      case HALVE_PUMP_SPEEDS :
         os << "HALVE_PUMP_SPEEDS";
         break;
      case STOP_MSS_RBC :
         os << "STOP_MSS_RBC";
         break;
      case STOP_MSS_PLT :
         os << "STOP_MSS_PLT";
         break;
      case NOTHING :
         os << "NOTHING";
         break;
   }

   os << "(" << dec << (int)id << ")";

   return os;
}

DataLog_Stream& operator << (DataLog_Stream& os, ALARM_RESPONSES id)
{
   switch (id)
   {
      case ABORT :
         os << "ABORT";
         break;
      case ABORT_THRU_RINSEBACK :
         os << "ABORT_THRU_RINSEBACK";
         break;
      case CONTINUE :
         os << "CONTINUE";
         break;
      case CONFIRM :
         os << "CONFIRM";
         break;
      case UNLOAD :
         os << "UNLOAD";
         break;
   }

   os << "(" << dec << (int)id << ")";

   return os;
}

/* FORMAT HASH e94e913ca1a80047e6e5610ae6ecafcd */

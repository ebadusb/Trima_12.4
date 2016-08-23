/*******************************************************************************
 *
 * Copyright (c) 2002 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      trimamessages.h
 *             Trima specific Messages
 *
 *
 ******************************************************************************/
#include <vxworks.h>

#define _ALARM_NAME_LOGGING_   // Get alarm name translations.

#define _TRIMA_MESSAGES_CPP_


#include "alarms.h"
#include "trimamessages.h"      // Needed to define the alarms.
#include "trima_datalog.h"
#include "predict_msgs.h"

DataLog_Stream & operator << (DataLog_Stream& os, GuiToProcPredictRequest_t request)
{
   switch (request)
   {
      case IS_PROCEDURE_VALID :    return os << "IS_PROCEDURE_VALID" << "(" << (int)request << ")";
      case SELECT_NEXT :           return os << "SELECT_NEXT" << "(" << (int)request << ")";
      case SELECT_PREVIOUS :       return os << "SELECT_PREVIOUS" << "(" << (int)request << ")";
      case SELECT_CURRENT :        return os << "SELECT_CURRENT" << "(" << (int)request << ")";
      case REPREDICT_ALL :         return os << "REPREDICT_ALL" << "(" << (int)request << ")";
      case REPREDICT_ALL_OPTIMAL : return os << "REPREDICT_ALL_OPTIMAL" << "(" << (int)request << ")";
   }

   return os << "UNKNOWN" << "(" << (int)request << ")";
}

DataLog_Stream& operator << (DataLog_Stream& os, ProcToGuiPredictStatus_t status)
{
   switch (status)
   {
      case PROCEDURE_VALID :           return os << "PROCEDURE_VALID" << "(" << (int)status << ")";
      case REQUEST_SELECTION_SCREEN :  return os << "REQUEST_SELECTION_SCREEN" << "(" << (int)status << ")";
      case PROCEDURES_SENT :           return os << "PROCEDURES_SENT" << "(" << (int)status << ")";
   }

   return os << "UNKNOWN" << "(" << (int)status << ")";
}

/* FORMAT HASH 0d00d5a0e20eddb0fcc0c542e23723b6 */

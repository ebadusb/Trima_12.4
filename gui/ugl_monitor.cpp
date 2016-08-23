/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 */

#include "ugl_monitor.hpp"

const char* const ugl_status_print (UGL_STATUS status)
{

   switch (status)
   {
      case UGL_STATUS_OK                   : return "UGL_STATUS_OK(0)";
      case UGL_STATUS_FINISHED             : return "UGL_STATUS_FINISHED(1)";
      case UGL_STATUS_ERROR                : return "UGL_STATUS_ERROR(-1)";
      case UGL_STATUS_DROP                 : return "UGL_STATUS_DROP(-2)";
      case UGL_STATUS_MSG_NOT_READY        : return "UGL_STATUS_[MSG|EVENT]_NOT_READY(-3)";
      // case UGL_STATUS_EVENT_NOT_READY      : a duplicate value
      case UGL_STATUS_Q_ACTIVE             : return "UGL_STATUS_Q_ACTIVE(-4)";
      case UGL_STATUS_Q_EMPTY              : return "UGL_STATUS_Q_EMPTY(-5)";
      case UGL_STATUS_Q_FULL               : return "UGL_STATUS_Q_FULL(-6)";
      case UGL_STATUS_BATCH_ERROR          : return "UGL_STATUS_BATCH_ERROR(-7)";
      case UGL_STATUS_MEMORY_ERROR         : return "UGL_STATUS_MEMORY_ERROR(-8)";
      case UGL_STATUS_TRUE_COLOR_SYSTEM    : return "UGL_STATUS_TRUE_COLOR_SYSTEM(-9)";
      case UGL_STATUS_RESOURCES_EXHAUSTED  : return "UGL_STATUS_RESOURCES_EXHAUSTED(-10)";
      case UGL_STATUS_RESOURCES_UNRESERVED : return "UGL_STATUS_RESOURCES_UNRESERVED(-11)";
      case UGL_STATUS_TIMEOUT              : return "UGL_STATUS_TIMEOUT(-12)";
      case UGL_STATUS_PERMISSION_DENIED    : return "UGL_STATUS_PERMISSION_DENIED(-13)";
   }

   static char buf[100]; // should be more than enough for our purposes

   sprintf (buf, "UGL STATUS UNKNOWN( %d )", status);
   return buf;
}

/* FORMAT HASH 623444efc50cedcccb2a74060e77e0ce */

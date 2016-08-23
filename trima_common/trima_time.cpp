////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2002 Gambro BCT, Inc.  All rights reserverd
//
// Author:       Regis McGarry
//
// Class Name:   trima_time
//
// Description:
// This class provides the ability to change time on Trima.
//
// $Log: trima_time.cpp $
// Revision 1.4  2003/04/03 21:35:13Z  ms10234
// 5818 - Changed log statements to use standard log_levels
// Revision 1.3  2002/10/28 15:26:30Z  rm70006
// Change signature for set_time to pass in const ref.
// Revision 1.2  2002/08/26 20:59:29Z  rm70006
// Fixed bug using static datalog constructors.
// Revision 1.1  2002/08/16 16:56:14Z  rm70006
// Initial revision
//
////////////////////////////////////////////////////////////////////////////
#include <vxWorks.h>

#include "trima_time.h"

#include "rtc.h"

#include "error.h"
#include "datalog.h"
#include "trima_datalog.h"



//
// set
//
void trima_time::set (const time_t& t)
{
   struct timespec  clockTime;
   STATUS           status;
   DataLog_Critical _fatal;

   clockTime.tv_sec  = t;
   clockTime.tv_nsec = 0;

   DataLog(log_level_trima_library_info) << "Setting Trima time to " << ctime(&t) << endmsg;

   if (clock_settime(CLOCK_REALTIME, &clockTime) == 0)
   {
// This step is only necessary on the 486 target.
#if CPU==I80486
      if (setRTCFromCurrentTime() != OK)
      {
         DataLog(_fatal) << "set time failed for time " << ctime(&t) << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Call to setRTCFromCurrentTime failed!");
      }
#endif
   }
   else
   {
      DataLog(_fatal) << "set time failed for time " << ctime(&t) << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Call to clock_settime failed!");
   }
}

/* FORMAT HASH cc0541e28b5aeffaec2217d431276674 */

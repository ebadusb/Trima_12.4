#include "guiproc.h"

DataLog_Stream & operator << (DataLog_Stream& os, const PREDICT_INVOKE_TYPE& p)
{
   switch (p)
   {
      case DONINFO_DELTA     : return os << "DONINFO_DELTA"     ;
      case DONINFO_NO_DELTA  : return os << "DONINFO_NO_DELTA"  ;
      case ADJUST_DELTA      : return os << "ADJUST_DELTA"      ;
      case ADJUST_NO_DELTA   : return os << "ADJUST_NO_DELTA"   ;
      case REPFLUID_NO_DELTA : return os << "REPFLUID_NO_DELTA" ;
      case CPS_DISABLES_RBCS : return os << "CPS_DISABLES_RBCS" ;
      case AUTO_FLOW_TIMEOUT : return os << "AUTO_FLOW_TIMEOUT" ;
   }

   return os << "UNKNOWN(" << (int)p << ')';
}

/* FORMAT HASH e5521861b0c24ad25cf03f7733ca4d11 */

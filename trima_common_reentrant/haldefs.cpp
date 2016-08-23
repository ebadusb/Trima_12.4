#include <vxworks.h>
#include "haldefs.h"




DataLog_Stream & operator << (DataLog_Stream& os, HAL_VALVESTATE valve)
{
   switch (valve)
   {
      case HAL_VALVE_UNKNOWN :
         os << "U";
         break;

      case HAL_VALVE_COLLECT :
         os << "C";
         break;

      case HAL_VALVE_OPEN :
         os << "O";
         break;

      case HAL_VALVE_RETURN :
         os << "R";
         break;
   }

   os << "(" << (int)valve << ")";

   return os;
}

/* FORMAT HASH 464455b18fb10c82c8c2a3a3a3174bb1 */

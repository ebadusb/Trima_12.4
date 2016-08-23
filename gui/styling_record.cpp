#include "styling_record.h"




DataLog_Stream & operator << (DataLog_Stream& os, const OSRegion& region)
{
   os << "(x=" << region.x << "; y=" << region.y << "; w="
      << region.width << "; h=" << region.height << ")";
   return os;
}


DataLog_Stream& operator << (DataLog_Stream& os, const UGL_RECT& rect)
{
   return os << "(x=" << rect.left << "; y=" << rect.top << "; w="
             << rect.right - rect.left + 1 << "; h=" << rect.bottom - rect.top + 1
             << "; area=" << (rect.right - rect.left + 1) * (rect.bottom - rect.top + 1)
             << " pixels)";
}

/* FORMAT HASH f3682b8fae3bd21c8a45521d8d8c0fbe */

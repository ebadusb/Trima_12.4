#include "run_defs.h"

inline float LimitPrimeRatio (float r, float hct)
{
   float fret = r;

#define HCT_LIMIT  (  0.37f / ( 1.0f - 1.0f / MAX_RATIO  ))

   if ( hct > HCT_LIMIT )
   {
      float tmp = 1.0f / ( 1.0f - 0.37f / hct );
      if ( fret > tmp )
      {
         fret = tmp;
      }
   }
   if (fret < 5.1f) fret = 5.1f;
   return fret;
}

/* FORMAT HASH d3b57430da1d49daa2f380f0669dd720 */

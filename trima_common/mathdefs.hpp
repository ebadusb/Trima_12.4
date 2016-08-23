/****************************************************************************
 *
 *  Round.hpp
 *
 *  Apparently, the compiler used for the simulator has no 'round' function
 *  defined.  Previously, functions defined their own replacement; these have
 *  been refactored and improved here.
 *
 *
 ***************************************************************************/

#ifndef ROUND_FOR_SIMNT
#define ROUND_FOR_SIMNT

#if (CPU == SIMNT)
// Apparently the "round()" function works on Trima, but not in the
// simulator, so we define it here: symmetric, away-from-0 rounding (which is
// what std::round does).
inline double round (double x)
{
   return
      (x < 0)
      ? ceil  (x - .5)
      : floor (x + .5)  ;
}


// Silly, but necessary,.
inline double trunc (double value) { return (value > 0) ? floor(value) : ceil(value) ; }


#endif

// These are convenience functions that used to be defined in several different places.

// inline double roundToTenth(double x) { return round (x * 10)/10; }
inline double floorToTenth (double x) { return floor (x * 10) / 10; }
inline double ceilToTenth (double x) { return ceil  (x * 10) / 10; }



#endif

/* FORMAT HASH 889a5320b64d1f8b80fe13e0e5bd9fa3 */

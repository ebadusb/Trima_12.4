/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 */

#ifndef UGL_MONTIOR_INCLUDE
#define UGL_MONTIOR_INCLUDE

#include <ugl/ugl.h>
#include "trima_datalog.h"

// remove the next line to enable checking
#define DISABLE_UGL_CHECKING



// #include <ugl/uglwin.h>


//
// The code that follows defines a macro that will print any bad
// return values from UGL.  This is useful information for debugging
// GUI issues (we can trace stuff back to UGL errors more easily).
// - D.S.

// UGL_STATUS isn't even an enum type, it's an int with a bunch of #define'd
// values.  Here's the translation function...
const char* const ugl_status_print(UGL_STATUS);


inline UGL_STATUS ugl_call_checker (const char* file, int line, UGL_STATUS s)
{
   if (s != UGL_STATUS_OK )
   {
      DataLog(log_level_gui_error) << "UGL ERROR in " << file << ':' << line
                                   << ", ugl call returned code " << s
                                   << " == " << ugl_status_print(s)
                                   << endmsg;
      _FATAL_ERROR (file, line, "UGL ERROR: BAD STATUS");
   }
   return s;
}

template<class T>
inline T* ugl_null_checker (const char* file, int line, T* retval)
{
   if (retval == UGL_NULL)
   {
      DataLog(log_level_gui_error) << "UGL ERROR in " << file << ':' << line
                                   << ", ugl call returned UGL_NULL"
                                   << endmsg;
      _FATAL_ERROR (file, line, "UGL ERROR: RETURNED NULL");
   }
   return retval;
}

template<class T>
inline T* ugl_null_id_checker (const char* file, int line, T* retval)
{
   if (retval == UGL_NULL_ID)
   {
      DataLog(log_level_gui_error) << "UGL ERROR in " << file << ':' << line
                                   << ", ugl call returned UGL_NULL_ID"
                                   << endmsg;
      _FATAL_ERROR (file, line, "UGL ERROR: RETURNED NULL ID");
   }
   return retval;
}


#ifdef DISABLE_UGL_CHECKING // or, later, just CPU==SIMNT
# define UGL_STATUS_MONITOR(x) x
# define UGL_NULL_RESULT_MONITOR(x)   x
# define UGL_NULL_ID_MONITOR(x)   x
#else
# define UGL_STATUS_MONITOR(_ugl_call) ugl_call_checker(__FILE__, __LINE__, _ugl_call)
# define UGL_NULL_RESULT_MONITOR(_ugl_call) ugl_null_checker(__FILE__, __LINE__, _ugl_call)
# define UGL_NULL_ID_MONITOR(_ugl_call) ugl_null_id_checker(__FILE__, __LINE__, _ugl_call)
#endif


#endif

/* FORMAT HASH f989d84278ca058c9c381ee50dbaf362 */

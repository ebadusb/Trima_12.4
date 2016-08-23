/* FILENAME: platform.c
 * PURPOSE: encapsulates platform dependencies
 * provides common wrappers where needed or redefinitions
 * so that src code can use unix version.
 * CHANGELOG:
 *   04/29/2002 - dyes - initial version for VXWORKS specific stuff
 */


#include "platform.h"

#ifdef BUILD_FOR_UNIX
#endif
//
// end of UNIX definitions
//

//
// NT definitions
//
#ifdef BUILD_FOR_NT
#endif
//
// end of NT definitions
//


//
// VxWorks definitions
//
#ifdef BUILD_FOR_VXWORKS


// JPH -- change name due to namespace collision with unistd.h!!
// FUNCTION: sleep_unixEmu
// PURPOSE: emulates Unix style sleep
// 
int sleep_unixEmu( int n )
{
    int rval;
    struct timespec request;
    struct timespec remaining;

    request.tv_sec = n;
    request.tv_nsec = 0;

    rval = nanosleep( &request, &remaining );

    // if interrupted by signal, return seconds remaining
    if(rval == -1 && errno == EINTR)
    {
        return remaining.tv_sec;
    }
    else
    {
        return 0;
    }
}

#endif
//
// end of VxWorks definitions
//



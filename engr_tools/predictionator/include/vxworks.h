#ifndef __VXWORKS_H_INC
#define __VXWORKS_H_INC

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm> // for max
#include <io.h>


#define _FATAL_ERROR(file,line,msg) std::cout << "FATAL " << file << " " << line << " " << msg << std::endl

#undef TRUE
#define TRUE true
#undef FALSE
#define FALSE false

#ifdef PREDICT_DEBUG
#include <cassert>
#undef ASSERT
#define ASSERT assert
#endif

#pragma warning(disable:4305) /* truncation from const double to float */
#pragma warning(disable:4355) /* use of 'this' in initialization list */
#pragma warning(disable:4786) /* identifier truncated to '255' chars in debug info. */
//
// Definitions for common error logging levels
#define LOG_ERROR               "log_error"
#define LOG_FATAL               "log_fatal"
#define errnoMsg 1

extern char* PATHNAME;

struct timespec // is defined in time.h; but not the win32 version
    {
    					/* interval = tv_sec*10**9 + tv_nsec */
    time_t tv_sec;			/* seconds */
    long tv_nsec;			/* nanoseconds (0 - 1,000,000,000) */
    };

// hopefully I don't really have to implement these.
//int _cdecl open( const char *fname, const int flags, const int protection);// { return -1; };
//int _cdecl write(const int fd , const char *data , const int length );//{ return -1; };
//Deliberate redef. 
//int _cdecl close(const int fd, ...);// { return -1; };    
static unsigned long trimaSysGetSoftwareSetting(const char * s)
{
	if (strcmp(s, "postcount_platelet_mobilization_algorithm") == 0)
		return 1;
	else
		return 0;
}
#endif /* ifdef __VXWORKS_H_INC */
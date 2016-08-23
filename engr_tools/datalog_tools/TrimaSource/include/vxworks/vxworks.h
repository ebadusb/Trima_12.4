#ifndef __VXWORKS_H_INC
#define __VXWORKS_H_INC

#include <ctime>
#include <cstdlib>
#include <iostream>

//#include <io.h>
#define UNUSED_ARGUMENT(x) (void)x
#ifndef TRUE
#define TRUE true
#endif

#ifndef FALSE
#define FALSE false
#endif

#ifndef ERROR
#define ERROR (-1)
#endif

#define STATUS int
#define OK 0

#define _FATAL_ERROR(file,line,msg) /*cout << "FATAL " << file << " " << line << " " << msg << endl*/
//#pragma warning(disable:4305) /* truncation from const double to float */
//#pragma warning(disable:4355) /* use of 'this' in initialization list */
//
// Definitions for common error logging levels
#define LOG_ERROR               "log_error"
#define LOG_FATAL               "log_fatal"
#define errnoMsg 1

#ifdef PREDICT_DEBUG
#include <cassert>
#define ASSERT assert
#endif

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */
#if defined(_MSC_VER) && !defined(__CYGWIN__)
#include <io.h>
	struct timespec // is defined in time.h; but not the win32 version
		{
							// interval = tv_sec*10**9 + tv_nsec 
		time_t tv_sec;			// seconds 
		long tv_nsec;			// nanoseconds (0 - 1,000,000,000) 
		};
		//int open( const char *fname, const int flags, const int protection);
		//int write(const int fd , const char *data , const int length );
		//Deliberate redef. 
		//int close(const int fd, ...);
#endif
	int trimaSysGetSoftwareSetting(const char *optionName, unsigned long *value);
	int trimaSysLoadOptionFiles(); //reads only sw.dat now

	bool attrib(const char*, const char*);
	
	typedef struct OptionFileDataStruct
	{
		char * name;
		char * setting;
	
		struct OptionFileDataStruct * next;
	} OptionFileData;
	
	int readOptionFile(const char * fileName, const char * sectionName, OptionFileData ** data, int crcRequired);
	int getOption(const char * optionName, const char ** option, OptionFileData * data);
    int taskDelay(int delay);
#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* __VXWORKS_H_INC*/

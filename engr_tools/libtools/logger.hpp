/* Copyright 1998 Cobe BCT, Inc.
 *
 * FILENAME: Logger.hpp
 * $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/libtools/rcs/logger.hpp 6.4 2002/11/21 22:19:51Z td07711 Exp td07711 $
 * PURPOSE: Logger class is a static class that provides
 *   generic logging functions.
 * CHANGELOG:
 *   $Log: logger.hpp $
 *   Revision 6.4  2002/11/21 22:19:51Z  td07711
 *   log output to stdout instead of stderr
 *   Revision 6.3  2002/09/19 21:07:35  td07711
 *   removed couplings to usage class
 *   Revision 6.2  2002/04/30 22:16:49  td07711
 *   porting to vxworks
 *   4/30/2002 - dyes - add Init() for vxworks
 *   Revision 6.1  2002/04/29 23:59:39  td07711
 *   mods for vxworks port
 *   Revision 1.8  2000/08/10 20:50:32  td07711
 *     add LOG_VERBATIM macro
 *   7/25/2000 - dyes - merge NT side changes,
 *       default all output to stderr, add LOG_VERBATIM macro
 *   Revision 1.7  2000/05/18 21:42:53  MS10234
 *   Changed case on filenames to be all lower case
 *   Revision 1.5  2000/03/09 23:38:39  TD07711
 *   IT4580
 *   Revision 1.3  2000/01/13 21:22:07  TD07711
 *   02/23/2000 - dyes - add LOG_OPENFILE for also sending log msgs to a file
 *   01/04/2000 - dyes - allow compile with prior definition of ASSERT
 *      Logger::Init() - default usage arg to NULL
 *   09/01/99 - dyes - init with Usage Text
 *   11/23/98 - dyes - initial version
 */

#ifndef Logger_HPP // prevents multiple inclusion
#define Logger_HPP

#include <stdio.h>
#include "platform.h"

#define LOG_BUF_SIZE 2048

//
// macros
//
#define COMPILE_ASSERT 1 // conditionally compile calls to ASSERT 
#define COMPILE_LOG_DEBUG 1 // conditionally compile calls to LOG_DEBUG


#ifndef ASSERT  // if defined by prior #include, use that definition, not this one.
#ifdef COMPILE_ASSERT
    #define ASSERT(s) ((s)?(void)0:Logger::Assert_failed(#s, __FILE__, __LINE__))
#else 
    #define ASSERT(s) ((void)0)
#endif
#endif


#ifdef COMPILE_LOG_DEBUG
    #define LOG_DEBUG \
        Logger::Set_va_variables(__FILE__, __LINE__), \
        Logger::Log_debug
#else
    #define LOG_DEBUG(s) ((void)0)
#endif 

// logging macros
#define LOG_FATAL \
    Logger::Set_va_variables(__FILE__, __LINE__), \
    Logger::Log_fatal
#define LOG_ERROR \
    Logger::Set_va_variables(__FILE__, __LINE__), \
    Logger::Log_error
#define LOG_WARNING \
    Logger::Set_va_variables(__FILE__, __LINE__), \
    Logger::Log_warning
#define LOG_INFO Logger::Log_info
#define LOG_USAGE Logger::Log_usage
#define LOG_DUMP(buf, size) Logger::Log_dump(buf, size)
#define LOG_OPENFILE Logger::Log_openfile
#define LOG_VERBATIM Logger::Log_verbatim



// CLASSNAME: Logger
// RESPONSIBILITIES:
//   1. Provides printf varargs interfaces for logging info.
//   2. Prefixes error messages with ERROR.
//   3. Prefixes warning messages with WARNING.
//   4. Prefixes fatal error messages with FATAL.
//   5. Supplies errno info if available for warning and error msgs.
//   6. Allows runtime control of debug messages
// COLLABORATIONS:
//   1. Used by all classes for logging info.
// NOTES:
//   1. all data and functions are static.  No instances are wanted 
//      so a pure virtual destructor is used to enforce. 
class Logger
{
    public:

        Logger(); // needed public constructor declared to avoid compile warning
        virtual ~Logger() = 0; // enforce no instances at compile time

        static void Init(int argc, char** argv); // for NT and Unix
        static void Init(const char* programName); // for vxworks

        // called via LOG macro to save file and line of caller
        static void Set_va_variables(char* filename, int line) {
            _Filename = filename; _Line = line; };

        // Logging functions
        static void Log_fatal(char* format,...);
        static void Log_error(char* format,...);
        static void Log_warning(char* format,...);
        static void Log_info(char* format,...); 
        static void Log_debug(char* format,...);
        static void Log_void(char* format,...);
        static void Log_usage(char* format,...);
        static void Log_usage();
        static void Log_dump(const char* buf, int size);
        static void Log_verbatim(char* format,...);

        // Logging control
        static void Enable_debug() { _Debug_enabled = true; };
        static void Disable_debug() { _Debug_enabled = false; };
        static bool Is_debug() { return _Debug_enabled; };
		static const char* Get_program_name() { return _Program_name; };
        static void Log_openfile(const char* filename);
	static void LogToStderr() { _log_file = stderr; }; 
	static void LogToStdout() { _log_file = stdout; }; 

        // assert failure handling
        static void Assert_failed(const char* pCond, const char* file, int line);

        #ifdef BUILD_FOR_NT
        static int outOfMemory(size_t);
        #else
        static void outOfMemory();
        #endif

    protected:

        static const char* _Program_name;
        static bool _Debug_enabled;  // enables debug messages
        static bool _Initialized; // set when initialized
        static const char* _Filename; // caller's filename
        static int _Line; // caller's line number
        static char* _Empty_string;
        static FILE* _log_file;
        static char _Va_buf[LOG_BUF_SIZE]; // space to unpack formatted message
        static char _Log_buf[LOG_BUF_SIZE]; // add header info

        // helper functions
        static const char* Strip_path(const char* filename);
        static void Init_defaults(); // called by Init()
        static void Log_buf( FILE* fp=stdout ); // sends _Log_buf to log
        static char* getLineNum();
        static const char* getFileName();

    private:

//        Logger(); // catch unauthorized use
        Logger(const Logger&); // catch unauthorized use 
        Logger& operator=(const Logger& rhs); // catch unauthorized use
};

#endif // Logger_HPP

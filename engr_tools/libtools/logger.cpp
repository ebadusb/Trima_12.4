/* Copyright 1998 Cobe BCT, Inc.
 *
 * FILENAME: Logger.cpp
 * PURPOSE: encapsulates generic logging functions
 * CHANGELOG:
 *   11/23/98 - dyes - initial version
 *   09/01/99 - dyes - add usage text
 *   11/03/99 - dyes - removed -debug, moved it to application 
 *   11/18/99 - dyes - log filename/linenum only if in debug mode
 *                     except for LOG_FATAL and ASSERT
 *   07/26/2000 - dyes - default all output to stderr, add log_verbatim()
 *   04/29/2002 - dyes - include ctype.h for isprint()
 *   04/30/2002 - dyes - add vxworks compatible Init()
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "platform.h"
#include "logger.hpp"

//
// initialize statics
//
bool Logger::_Debug_enabled = false;
bool Logger::_Initialized = false;
const char* Logger::_Program_name = "unknown";
const char* Logger::_Filename = "unknown"; // caller's filename
int Logger::_Line = 0; // caller's line number
char Logger::_Va_buf[LOG_BUF_SIZE]; // formatted varargs message
char Logger::_Log_buf[LOG_BUF_SIZE]; // formatted message to log
char* Logger::_Empty_string = "";
FILE* Logger::_log_file = 0;


// FUNCTION: Init
// PURPOSE: scan cmd line for Program Name and for 
// any Logger control flags, i.e. -debug and -timestamp
void Logger::Init(int argc, char** argv)
{
    ASSERT(argc);
    ASSERT(argv);

    if (_Initialized) return; // just return if already initialized

    _Program_name = Strip_path(argv[0]);
    _Initialized = true;

#ifdef BUILD_FOR_NT
    errno = 0;
#endif
}


// FUNCTION: Init
// PURPOSE: set program name
void Logger::Init(const char* programName)
{
    ASSERT(programName);

    if (_Initialized) return; // just return if already initialized

    _Program_name = programName;
    ASSERT(_Program_name);

    _Initialized = true;

#ifndef BUILD_FOR_NT
    errno = 0;
#endif
}



// FUNCTION: Assert_failed
// PURPOSE: assertion failure handling - logs message and aborts.
void Logger::Assert_failed(const char* pCond, const char* file, int line)
{
    if (pCond == 0) pCond = "???";
    if (file == 0) file = "???";

    file = Strip_path(file);
    if (_Initialized) {
        sprintf(_Log_buf, "ASSERT %.30s: %.50s@%d (%.1900s)\n", 
            _Program_name, file, line, pCond);
    }
    else {
        sprintf(_Log_buf, "ASSERT %.30s: %.50s@%d (%.1900s)\n", 
            "???", file, line, pCond);
    }

    Log_buf();

	LOG_DEBUG("Assert_failed: calling abort()");

#ifdef BUILD_FOR_NT
    _flushall();
    exit(134); // to avoid NT error popup
#endif
    
    abort(); // dump core
}


// FUNCTION: Strip_path()
// PURPOSE: strips path from filename, using '/' or '\' delimiters
// ARGUMENTS: const char* file - filename where invoked
// RETURNS: const char* stripped_filename
const char* Logger::Strip_path(const char* file)
{
    ASSERT(file);

    const char* p;
    for (p = file + strlen(file) - 1; p > file; p--) {
        if (*p == '/' || *p == '\\') {
            p++;
            break;
        }
    }

    return p;
}


// FUNCTION: getLineNum()
// PURPOSE: converts _Line to string, empty unless in Debug mode
char* Logger::getLineNum()
{
    static char line_string[20];

    if(Is_debug())
    {
        sprintf(line_string, "@%d: ", _Line);
        return line_string;
    }

    return _Empty_string;
}


// FUNCTION: getFileName()
// PURPOSE: returns _File if in debug mode, else empty string
const char* Logger::getFileName()
{
    if(Is_debug())
    {
        return _Filename;
    }
    return _Empty_string;
}


// FUNCTION:  Log_error
// PURPOSE:   logs error message. Adds ERROR prefix, and errno info.
void Logger::Log_error(char* format,...)
{
    va_list arglist;
    va_start(arglist, format);
    vsprintf(_Va_buf, format, arglist);
    va_end(arglist);
    
    _Filename = Strip_path(_Filename);
    if (errno) {
        sprintf(_Log_buf, "ERROR %.30s: %.30s@%d errno=%d: %.1900s",
        _Program_name, _Filename, _Line, errno, _Va_buf);
    }
    else {
        sprintf(_Log_buf, "ERROR %.30s: %.30s@%d %.1900s",
        _Program_name, _Filename, _Line, _Va_buf);
    }
    Log_buf();
}


// FUNCTION:  Log_fatal
// PURPOSE:   logs error message. Adds FATAL prefix, and errno info.
void Logger::Log_fatal(char* format,...)
{
    va_list arglist;
    va_start(arglist, format);
    vsprintf(_Va_buf, format, arglist);
    va_end(arglist);
    
    _Filename = Strip_path(_Filename);
    if (errno) {
        sprintf(_Log_buf, "FATAL %.30s: %.30s@%d errno=%d: %.1900s",
        _Program_name, _Filename, _Line, errno, _Va_buf);
    }
    else {
        sprintf(_Log_buf, "FATAL %.30s: %.30s@%d: %.1900s",
        _Program_name, _Filename, _Line, _Va_buf);
    }
    Log_buf();
	LOG_DEBUG("Log_fatal: calling abort()");
#ifdef BUILD_FOR_NT
    _flushall();
#endif
    exit(1);
}

// FUNCTION: Log_buf
// PURPOSE: sends completely formatted message to its destination
void Logger::Log_buf(FILE* fp)
{
    fprintf(fp, "%s\n", _Log_buf);
    if(_log_file && _log_file != fp)
    {
        fprintf(_log_file, "%s\n", _Log_buf);
    }
#ifndef BUILD_FOR_NT // for UNIX and VXWORKS
    errno = 0;
#endif
}


// FUNCTION:  Log_warning
// PURPOSE:   logs warning message. Adds WARNING prefix, and errno info.
void Logger::Log_warning(char* format,...)
{
    va_list arglist;
    va_start(arglist, format);
    vsprintf(_Va_buf, format, arglist);
    va_end(arglist);
    
    _Filename = Strip_path(_Filename);
    if (errno) {
        sprintf(_Log_buf, "WARNING %.30s: %.30s@%d errno=%d: %.1900s",
        _Program_name, _Filename, _Line, errno, _Va_buf);
    }
    else {
        sprintf(_Log_buf, "WARNING %.30s: %.30s@%d %.1900s",
        _Program_name, _Filename, _Line, _Va_buf);
    }
    Log_buf();
}


// FUNCTION:  Log_info
// PURPOSE:   logs informational message. Does not add a prefix or errno info.
void Logger::Log_info(char* format,...)
{
    va_list arglist;
    va_start(arglist, format);
    vsprintf(_Va_buf, format, arglist);
    va_end(arglist);
    
    sprintf(_Log_buf, "%.30s: %.1900s",
        _Program_name, _Va_buf);
    Log_buf();
}


// FUNCTION:  Log_verbatim
// PURPOSE:   logs message as given
void Logger::Log_verbatim(char* format,...)
{
    va_list arglist;
    va_start(arglist, format);
    vsprintf(_Va_buf, format, arglist);
    va_end(arglist);
    
    sprintf(_Log_buf, "%.1900s", _Va_buf);
    Log_buf();
}


// FUNCTION:  Log_debug
// PURPOSE:   logs debug message. Adds DEBUG prefix and errno info.
void Logger::Log_debug(char* format,...)
{
    va_list arglist;
    va_start(arglist, format);
    vsprintf(_Va_buf, format, arglist);
    va_end(arglist);
    
    if (Is_debug() == false) return;
    
    _Filename = Strip_path(_Filename);
    if (errno) {
        sprintf(_Log_buf, "DEBUG %.30s: %.30s@%d errno=%d: %.1900s",
        _Program_name, _Filename, _Line, errno, _Va_buf);
    }
    else {
        sprintf(_Log_buf, "DEBUG %.30s: %.30s@%d: %.1900s",
        _Program_name, _Filename, _Line, _Va_buf);
    }
    if(_log_file == 0)
        Log_buf();
    else
        Log_buf(_log_file);
}


// FUNCTION:  Log_dump
// PURPOSE:  dumps buffer contents in ascii and hex formats
void Logger::Log_dump(const char* buf, int size)
{
    FILE* fp = stderr;
    if(_log_file)
    {
        fp = _log_file;
    }
    const unsigned char* ptr = (const unsigned char*)buf;
    const unsigned char* cptr = ptr;
    int i;
    int line;
    int n;
    int linesize = 16;
    int bytes_to_dump;
    for(line = 0, n = size; n > 0; n -= linesize, line++)
    {
        fprintf(fp, "  %05d  ", line*linesize);

        if(n >= linesize)
            bytes_to_dump = linesize;
        else
            bytes_to_dump = n;

        //
        // dump hex
        //
        for (i = 0; i < bytes_to_dump; i++, ptr++)
        {
            if ((i%4) == 0)
            {
                fprintf(fp, " ");
            }
            fprintf(fp, "%02x", *ptr);
        }

        // pad with blanks
        for (i = 0; i <= (linesize - bytes_to_dump); i++)
        {
            if ((i%4) == 0)
                fprintf(fp, " ");
            fprintf(fp, "  ");
        }

        //
        // dump ascii
        //
        for (i = 0; i < bytes_to_dump; i++, cptr++)
        {
            if (isprint(*cptr))
                fprintf(fp, "%c", *cptr);
            else
                fprintf(fp, ".");
        }

        fprintf(fp, "\n");
    }
}


#ifdef BUILD_FOR_NT
int Logger::outOfMemory(size_t)
{
    fprintf(stderr, "new failed, out of memory\n");
    _flushall();
    abort();
    return 0;
}
#else
void Logger::outOfMemory()
{
    fprintf(stderr, "new failed, out of memory\n");
    abort();
}
#endif


// FUNCTION:  Log_openfile
// PURPOSE:  opens specified file for logging
// NOTES: exit if can't open
void Logger::Log_openfile(const char* filename)
{
    ASSERT(filename);

    // first close any open currently open file
    if(_log_file)
    {
        fclose(_log_file);
    }

    _log_file = fopen(filename, "w");
    if(_log_file == 0)
    {
        LOG_FATAL("fopen failed to open logfile=%s", filename);
    }
    LOG_INFO("opened logfile=%s", filename);
}

/* FILENAME: KeywordValueParser.cpp
 * PURPOSE: KeywordValueParser class implemenation
 * CHANGELOG:
 *   11/11/99 - dyes - initial version
 *   04/30/2002 - dyes - port for vxworks
 */

#include "platform.h"
#include "logger.hpp"
#include "keywordvalueparser.hpp"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifndef BUILD_FOR_NT // i.e. BUILD_FOR_UNIX or BUILD_FOR_VXWORKS
#include <unistd.h>
#endif




// FUNCTION: KeywordValueParser
// PURPOSE:  constructor
KeywordValueParser::KeywordValueParser(const char* filename, int linesize)
: _keyword(0), _value(0), _file(0), _linesize(linesize), _linenum(0)
{
    ASSERT(filename);
    ASSERT(linesize > 0);
    ASSERT(linesize <= 1024);

    // open std.dat
    _file = fopen(filename, "r");
    if(_file == 0)
    {
        char* cwd = getcwd(0,0);
        LOG_ERROR("fopen failed on file=%s, cwd=%s/std.dat", 
                  filename, cwd);
        free(cwd);
    }

    _linebuf = new char[linesize]; // input buffer
    *_linebuf = 0; // init to empty string
    _tempbuf = new char[linesize]; // parsing buffer
    *_tempbuf = 0; // init to empty string
}


// FUNCTION: ~KeywordValueParser
// PURPOSE:  destructor
KeywordValueParser::~KeywordValueParser()
{
    if(_file) fclose(_file);
    _file = 0;
    _keyword = 0;
    _value = 0;

    delete [] _linebuf;
    _linebuf = 0;
    delete [] _tempbuf;
    _tempbuf = 0;
}


// FUNCTION: getNext
// PURPOSE: parses file valid keyword/value record
// returns true if one is found, also strips comment from the record
bool KeywordValueParser::getNext()
{
    _keyword = 0;
    _value = 0;

    char* p;
    while(1)
    {
        if (fgets(_linebuf, _linesize, _file) == 0)
            return false;

        _linenum++;
        _keyword = 0;
        _value = 0;


        strcpy(_tempbuf, _linebuf);
        // strip trailing comments
        for(p=_tempbuf; *p; p++)
        {
            if(*p == '#')
            {
                *p = 0;
                break;
            }
        }

        // strip trailing whitespace
        for(p=_tempbuf+strlen(_tempbuf)-1; p >= _tempbuf; p--)
        {
            if(isspace(*p))
                *p = 0;
            else
                break;
        }


        // strip leading whitespace
        for(p=_tempbuf; *p; p++)
        {
            if(!isspace(*p))
            {
                _keyword = p;
                break;
            }
        }

        // skip blanks lines
        if(_keyword)
            break;

    } // end while (looking for keyword)

    // _keyword ptr already set
    // determine _value ptr, i.e. first non whitespace following keyword
    for(p=_keyword; *p; p++)
    {
        if(isspace(*p))
        {
            break;
        }
    }
    if(*p == 0)
    {
        // no value, just return
        LOG_DEBUG("keyword=%s", _keyword);
        return true;
    }

    *p = 0; // terminate _keyword
    
    // look for value, strip leading whitespace
    for(++p ; *p; p++)
    {
        if(!isspace(*p))
        {
            _value = p;
            break;
        }
    }

    // trailing whitespace already stripped

    if(_value)
        LOG_DEBUG("keyword=%s value=%s", _keyword, _value);
    else
        LOG_DEBUG("keyword=%s", _keyword);

    return true;

} // end getNext()


// FUNCTION: check
// PURPOSE:  return true if file opened OK, else false
bool KeywordValueParser::check()
{
    if(_file)
        return true;
    else
        return false;
}

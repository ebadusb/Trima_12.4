// FILENAME: Clock.cpp
//   $Header: $
// PURPOSE: provide generic clock functions
// CHANGELOG:
//   $Log: $
//   7/25/2000 - dyes - add stopwatch functions
//   11/10/99 - dyes - initial version


#include <time.h>
#include <string.h>

// Trima includes
#include "auxclock.h"

// engr_tools includes
#include "logger.hpp"
#include "clock.hpp"


Clock::Clock()
: _running( false ), _startMsec( 0 ), _stopMsec( 0 )
{
}

void Clock :: timebaseInit()
{
    // PURPOSE: initialize the timebase

    LOG_DEBUG("timebaseInit: initializing the timebase");
    auxClockInit();
}


void Clock :: getAsciiTime( char* buf )
{
    // PURPOSE: copies to buffer current time in ASCII
    // presumes buf size is big enough.

    time_t newtime = time( 0 );
    strcpy( buf, ctime( &newtime ) );
    buf[ strlen(buf) - 1 ] = 0; // remove line feed
}


unsigned long Clock :: getRawMsec()
{
    // PURPOSE: returns raw msec since boot

    rawTime t;
    auxClockTimeGet( &t );
    return( t.sec*1000 + t.nanosec/1000000 );
}


void Clock :: start()
{
    // PURPOSE: starts elapsed time tracking from 0
    
    _startMsec = getRawMsec();
    _running = true;
}


void Clock :: stop()
{
    // PURPOSE: suspends elapsed time tracking
    
    if( _running )
    {
	_running = false;
	_stopMsec = getRawMsec();
    }
}


void Clock :: resume()
{
    // PURPOSE: resumes elapsed time tracking

    if( !_running )
    {
	_running = true;
	_startMsec += getRawMsec() - _stopMsec; // bump _startMsec by number of msec stopped
    }
}


unsigned long Clock :: getElapsedMsec()
{
    // PURPOSE: returns elapsed number of msec 
    
    unsigned long elapsedMsec; 

    if( _running )
    {
	elapsedMsec = getRawMsec() - _startMsec;
    }
    else
    {
	elapsedMsec = _stopMsec - _startMsec;
    }

    return elapsedMsec;
}

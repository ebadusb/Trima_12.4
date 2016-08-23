// FILENAME: Clock.hpp
//   $ Header: $
// PURPOSE: Clock class is a static class that provides
//   generic clock and time/date functions.
// CHANGELOG:
//   $ Log: $
//   05/01/2002 - dyes - add helper functions for vxworks port
//   04/18/2000 - dyes - add stopwatch functions
//   11/10/99 - dyes - initial version


#ifndef Clock_HPP // prevents multiple inclusion
#define Clock_HPP

#include <platform.h>


// CLASSNAME: Clock
// RESPONSIBILITIES:
//   1. Provides caller with ascii time/date
//   2. Provides call to initialize the timebase
//   3. Provides stopwatch functions
// COLLABORATIONS:
//   1. used common/auxclock.h for timebase
class Clock
{
    public:
        
        Clock(); // clock is initialized stopped
        virtual ~Clock() {};

        static void getAsciiTime(char* buf); // returns ASCII time in caller's buf
	static void timebaseInit(); // initialize the timebase

        void start(); // resets elapsed time to 0 and starts tracking
        void stop(); // stops tracking elapsed time
        void resume(); // resumes tracking elapsed time
        unsigned long getElapsedSec() { return getElapsedMsec()/1000; }; // returns sec elapsed since start
        unsigned long getElapsedMsec(); // returns msec elapsed since start
	unsigned long getRawMsec(); // returns msec since boot
    
    protected:

        unsigned long _startMsec;
	unsigned long _stopMsec; 
	bool _running; // false when stopped, true when started or resumed

    private:

        Clock(const Clock&); // catch unauthorized use 
        Clock& operator=(const Clock& rhs); // catch unauthorized use
};

#endif // Clock_HPP

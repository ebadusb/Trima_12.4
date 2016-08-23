// FILENAME: main.cpp

// PURPOSE: entry point for hog utility used to hog specified cpu and memory resources
// CHANGELOG:

//   Revision 6.6  2003/09/05 17:04:16Z  td07711
//   IT6140 - add datalogging and minor mods
//   Revision 6.5  2003/01/30 21:29:39Z  td07711
//   bugfix - vxworks doesn't free heap on task exit, have to do it explicitly.
//   Revision 6.4  2002/12/20 23:30:22Z  td07711
//   return 1 if any cmdline parsing errors
//   Revision 6.3  2002/12/20 22:05:57Z  td07711
//   add -tb_init option to initialize timebase
//   Revision 6.2  2002/12/19 23:00:26Z  td07711
//   use taskPrioritySet() instead of sched_setparam()
//   Revision 6.1  2002/12/17 01:28:07Z  td07711
//   vx port
//   Revision 1.4  2002/07/02 22:02:53Z  rgx2112
//   Changed to vxworks
//   Revision 1.3  2000/05/18 21:46:29Z  MS10234
//   Changed case of filenames to be all lowercase
//   Revision 1.2  1998/06/10 18:37:51  TD07711
//   06/09/98 - dyes - use new instead of malloc, check setprio success
//   05/19/98 - dyes - initial version


// vxworks includes
#include <vxWorks.h>
#include <usrLib.h>
#include <taskLib.h>

// common includes
#include "optionparser.h"
#include "datalog_levels.h"

// libtools includes
#include "logger.hpp"
#include "clock.hpp"


#ifdef __cplusplus
extern "C" int hog_main( const char* args );
#endif



int hog_main( const char* args )
{
   Logger::Init( "hog_main" );

   OptionParser commandline( "hog_main", "hogs cpu/mem resources" );
   commandline.init( args );

   // storage for command line options
   bool Debug_flag;
   int Priority;
   int Mem_size;
   int Time_interval;
   int Cpu_time;
   int Time_out;
   int Mem_interval;
   int Mem_n;
   bool TimebaseInitFlag;
   bool flag;

   commandline.parse( "hog_main", "allowed as cmd line token for execution at boot", &flag);
   commandline.parse( "-debug", "enables debug messages", &Debug_flag );
   commandline.parse( "-p", "set priority to run hog at", &Priority, 128, 1, 255 );
   commandline.parse( "-i", "set time interval for hog", &Time_interval, 100, 1, 1000 );
   commandline.parse( "-c", "set % for cpu time consumption", &Cpu_time, 10, 1, 100 );
   commandline.parse( "-m", "set memory size (kb) for hog", &Mem_size, 1, 1, 100000 );
   commandline.parse( "-t", "set time out value", &Time_out, 5, 0, 10000 );
   commandline.parse( "-step", "set memory write step size (byte)", &Mem_interval, 100, 1, 1000000 );
   commandline.parse( "-n", "set number of bytes to write at a time", &Mem_n, 100, 1, 1000000 );
   commandline.parse( "-tb_init", "initialized the timebase", &TimebaseInitFlag );
   commandline.done();

   if( commandline.anyErrors() )
   {
      LOG_ERROR( "unparsed stuff on cmd line" );
      return 1;
   }

   if( Debug_flag )
   {
      Logger::Enable_debug();
   }
   else
   {
      Logger::Disable_debug();
   }


   // set process priority
   if( taskPrioritySet( taskIdSelf(), Priority ) != 0 )
   {
      DataLog( log_level_hog_error ) << "taskPrioritySet failed, exiting" << endmsg;
      LOG_ERROR( "taskPrioritySet failed to set priority to %d", Priority );
      return 1;
   }

   // allocate specified memory
   // NOTE:  vxworks does not free memory on task exit, so have to do it explicitly
   int mem_size = Mem_size * 1024;
   char* pMem = new char[ mem_size ];
   if( pMem == 0 )
   {
      DataLog( log_level_hog_error ) << "new failed, exiting" << endmsg;
      LOG_ERROR( "failed to new %ld bytes", mem_size );
      return 1 ;
   }
   LOG_DEBUG("new succeeded on %d bytes", mem_size);

   // touch every 4KB to force page fault
   char* p;
   for( p = pMem; p < (pMem + mem_size); p += 1000 )
   {
      *p = 0x12;
   }
   LOG_DEBUG("new memory written every 1000 bytes");

   DataLog( log_level_hog_info ) << "starting hog loop, " << Cpu_time << "% of cpu at prio=" << Priority
                                 << ", hogging " << Mem_size << "KB of memory" << endmsg;
   LOG_INFO( "running for %d seconds, %d%% of cpu at priority %d, %dKB of memory",
             Time_out, Cpu_time, Priority, Mem_size );

   //
   // run hog loop
   //

   // initialize timebase mechanism if specified on cmdline
   // if router is running, it will have already been initialized)
   if( TimebaseInitFlag )
   {
      Clock::timebaseInit();
   }
   Clock timeoutClock;
   Clock hogtimeClock;
   timeoutClock.start();

   struct timespec sleep_time;
   int sleep_msec;
   int status;
   int hog_time = (int)((float)Time_interval * ((float)Cpu_time/100.));
   DataLog( log_level_hog_info ) << "hogging " << hog_time << " msec every "
                                 << Time_interval << " msec" << endmsg;
   LOG_INFO( "will hog %ld msec per %ld msec interval", hog_time, Time_interval );

   for( p = pMem; Time_out == 0 || timeoutClock.getElapsedSec() < Time_out; )
   {
      hogtimeClock.start();

      // hog cpu for specified time
      // walk through memory until time is up, do n bytes at a time
      int elapsedMsec;
      int numBytes;
      for( elapsedMsec = 0, numBytes = 0; elapsedMsec < hog_time;
           elapsedMsec = hogtimeClock.getElapsedMsec() )
      {
         for( int i = 1; i <= Mem_n; i++, p+= Mem_interval, numBytes++ )
         {
            // avoid mem overrun
            if( p >= ( pMem + mem_size ) )
            {
               p = pMem;
            }

            *p ^= 0xFF; // flip the bits
         }
      }

      LOG_DEBUG( "read/write %d bytes in %d msec", numBytes, elapsedMsec );

      // sleep until time to run again
      sleep_msec = Time_interval - elapsedMsec;
      if( sleep_msec > 0 )
      {
         sleep_time.tv_sec = sleep_msec / 1000;
         sleep_time.tv_nsec = ( sleep_msec % 1000 ) * 1000000;
         LOG_DEBUG( "sleeping for %ld msec", sleep_msec );
         status = nanosleep( &sleep_time, 0 );

         if( status == -1 && errno == EINTR )
         {
            break;
         }
      }
   }

   // free heap memory
   delete [] pMem;

   // exit message
   int run_msec = timeoutClock.getElapsedMsec();
   DataLog( log_level_hog_info ) << "exiting, ran for " << run_msec/1000 << " seconds" << endmsg;
   LOG_INFO( "ran for %d.%d seconds", run_msec / 1000, run_msec % 1000 );

   return 0;
}

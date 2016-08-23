//
// Necessary functions to run simulator
//

#include <stdio.h>
#include <envLib.h>
#include "trima_time.h"
#include "trima_datalog.h"

void start()
{
   // Needed to start up the datalogger
   //putenv("TRIMA_MACHINE_NAME=1T00085");
   //sysinit_main();

#if CPU==SIMNT
   time_t today;

   today = 1026393764;

   trima_time::set(today);

   today = time(0);
   printf ("%s\n", ctime(&today));
#endif

   printf("Start Complete\n");
}

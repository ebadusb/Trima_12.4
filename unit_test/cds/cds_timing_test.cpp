#include <vxWorks.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>


#include "semLib.h" // Needed for semaphores

#include "cds.h"
#include "datalog.h"
#include "trima_datalog.h"
#include "filenames.h"
#include "readdir_stat.h"
#include "auxclock.h"


SEM_ID synchSem;



extern "C" int worker_task(int loop_count, int mode, int jitter_max)
{
   int value;
   int jitter;
   double work = 0;
   CDS *_cds = NULL;

   MessageType msg;


   if (mode == 1)
   {
      _cds = new CDS(ROLE_RO);
   }
   else if (mode == 2)
   {
      _cds = new CDS(ROLE_RW);
   }


   semTake(synchSem, -1);
   semGive(synchSem);

   DataLog( log_level_cds_info ) << "task " << taskName(taskIdSelf()) << " starting" << endmsg;

   for (int i = 0; i < loop_count; i++)
   {
      msg = _cds->BE6.Get();

      if (mode == 2)
      {
         _cds->BE6.Set(msg);

         taskDelay(20);
      }

#if 0
      jitter = int( ( (float)rand() / (float)RAND_MAX) * (float)jitter_max);
      
      DataLog( log_level_cds_info ) << "task " << taskName(taskIdSelf()) << " value is " << value << ".\t"
                      << "Jitter is " << jitter << "." << endmsg;

      taskDelay(jitter);
#else
      taskDelay(jitter_max);
#endif
   }

   return 0;
}



//
// This task reads a directory and increments a cds int for every read
//
extern "C" void read_the_dir(void)
{
   DIR *directory;
   struct dirent *dirEntry;
   struct stat fileStat;
   int myCount = 0;

   CDS _cds(ROLE_RW);

   for (;;)
   {
      directory = opendir (LOG_PATH);

      for (;;)
      {
         CountType localCount = _cds.count;

         dirEntry = readdir_stat(directory, &fileStat);

         if (dirEntry == 0)
         {
            //DataLog(log_level_cds_info) << "End of Reading dir " << endmsg;
            break;
         }

         localCount.count1 = ++myCount;

         //printf("Task %s Count is %d %d\n", taskName(taskIdSelf()), localCount.count1, myCount);

         _cds.count.Set(localCount);
      }

      closedir(directory);
   }
}



//
// This function counts the number of times it is called from a timer interrupt.
//
extern "C" void count_the_interrupts(void)
{
   CDS _cds(ROLE_RW);

   SEM_ID timerSemaphore = auxClockSemaphoreAttach(1200, AuxClockCountingSemaphore);

   int myCount = 0;

   for (;;)
   {
      CountType localCount = _cds.count;
      semTake(timerSemaphore, WAIT_FOREVER);
      localCount.count2 = ++myCount;
      _cds.count.Set(localCount);
      //printf("Task %s Count is %d %d\n", taskName(taskIdSelf()), localCount.count2, myCount);
   }
}



//
// This task reads the CDS just because it can
//
extern "C" void read_the_cds(int jitter_max, int modulus)
{
   CDS _cds(ROLE_RO);

   for (;;)
   {
#if 0
      const int jitter = int( ( (float)rand() / (float)RAND_MAX) * (float)jitter_max);
      
      DataLog( log_level_cds_info ) << "task " << taskName(taskIdSelf()) << " value is " << value << ".\t"
                      << "Jitter is " << jitter << "." << endmsg;

      taskDelay(jitter);
#else
      taskDelay(jitter_max);
#endif

      const CountType count = _cds.count;

      printf("Task %s Count is %d %d\n", taskName(taskIdSelf()), count.count1, count.count2);

      if ( (count.count1 % modulus) == 1)
      {
         printf("Task %s Count is %d\n", taskName(taskIdSelf()), count.count1);
      }

      if ( (count.count2 % modulus) == 1)
      {
         printf("Task %s Count is %d\n", taskName(taskIdSelf()), count.count2);
      }
   }
}



//
// this task starts off one of the timing tests
//
extern "C" void timing_test_a(void)
{
   // Initialization
   synchSem = semBCreate (SEM_Q_PRIORITY, SEM_FULL);
   taskPrioritySet(taskIdSelf(), 10);

   semTake(synchSem, -1);

   taskSpawn("cds_task_1", 30, 0, 0x1000, (FUNCPTR)worker_task, 40, 2, 20, 0, 0, 0, 0, 0, 0, 0);

   DataLog( log_level_cds_info ) << "tasks spawned.  releasing semaphore " << hex << synchSem << dec << "..." << endmsg;

   semGive(synchSem);
   semFlush(synchSem);
}



//
// This tasks starts off one of the timing tests
//
extern "C" void timing_test_b(void)
{
   taskSpawn("read_the_dir_1", 150, 0, 0x1000, (FUNCPTR)read_the_dir, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
   taskSpawn("read_the_dir_2", 150, 0, 0x1000, (FUNCPTR)read_the_dir, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
   
   taskSpawn("count_the_interrupts_1", 150, 0, 0x1000, (FUNCPTR)count_the_interrupts, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
   //taskSpawn("count_the_interrupts_2", 150, 0, 0x1000, (FUNCPTR)count_the_interrupts, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
   
   taskSpawn("read_the_cds_1", 150, 0, 0x1000, (FUNCPTR)read_the_cds, 30, 10000, 0, 0, 0, 0, 0, 0, 0, 0);
   taskSpawn("read_the_cds_2", 150, 0, 0x1000, (FUNCPTR)read_the_cds, 50, 20000, 0, 0, 0, 0, 0, 0, 0, 0);
   taskSpawn("read_the_cds_3", 150, 0, 0x1000, (FUNCPTR)read_the_cds, 70, 40000, 0, 0, 0, 0, 0, 0, 0, 0);
   taskSpawn("read_the_cds_4", 150, 0, 0x1000, (FUNCPTR)read_the_cds, 90, 80000, 0, 0, 0, 0, 0, 0, 0, 0);
}



/*
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      Control Driver 50 ms interrupt handler
 *
 * ABSTRACT:   Processes Air to Donor and Ultrasonics
 *
 */

#include <vxWorks.h>
#include <string.h>
#include <timers.h>
#include "c_ultra.h"
#include "ctl_hw_private.h"
#include "trima_datalog.h"
#include "trimaOs.h"
#include "trima_options.h"



#define MAX_COUNTS (256)               /* fluid/air reg size */
#define MAX_INVALID_REG_ALLOWED (1)    /* threshold for invalid reg readings */
#define US_LOWER_SAMPLES (4)           /* lower sensor filter size (number 50 ms samples) */
#define US_UPPER_SAMPLES (20)          /* upper sensor filter size (number 50 ms samples) */

#define SAMPLE_RATE_IN_MS (50)
#define FLUID_COUNT_TRIGGER (1330)
#define FLUID_COUNT_SAMPLES (6)         /* Current sample plus last 6 = 7 */
#define FLUID_COUNT_MAX_ITERATIONS (20) /* 1 second at 50 ms rate */


typedef struct
{
   unsigned long totalCounts;
   unsigned long totalSamples;
   unsigned long minCounts;
   unsigned long maxCounts;
   short         minAGC;
   short         maxAGC;
   short         secondStageNormalTransition;
   short         secondStageLastChanceTransition;
} US_STATS;

/* struct to hold ultrasonics data */

enum { LOG_SIZE = 20 };                /* number of samples maintained for logging purposes */

typedef struct
{
   short         filterLength;         /* length of filter for air/fluid transitions */

   short         count;                /* count of air/fluid */
   unsigned char result;               /* =1 fluid */
   unsigned char ulsFluidToAir;        /* ULS fluid to air transition.  Don't log */
   unsigned char newResult;            /* 1 if sensor state changed on last scan */
   unsigned char logHoldoff;           /* count of update cycles before logging is allowed again, */
                                       /* used to insure that logging is not excessive */

   long cyclesInSecondStageFilter;            /* number of cycles the first stage filter is true */
                                              /* and second stage filter has not yet triggered */

   unsigned char air;                  /* air reading */
   long          airCounts;            /* air counts */
   unsigned char lastAir;              /* last air count */

   unsigned char fluid;                /* fluid reading */
   long          fluidCounts;          /* fluid counts */
   unsigned char lastFluid;            /* last fluid count */

   short         total;                /* total f+a counts */

   unsigned char registerErrorCounter; /* if 2 in a row, cause fatal    */
   unsigned char fatalError;           /* indicates hardware register error */

   unsigned long accTotal;             /* accumulated total for timing check */

   /*
    * Log information for last LOG_SIZE samples to be dumped after a
    * level sensor event.
    */
   unsigned char  resultLog[LOG_SIZE];
   unsigned char  airCountLog[LOG_SIZE];
   unsigned char  fluidCountLog[LOG_SIZE];
   short          agcLog[LOG_SIZE];
   unsigned short logIdx;

   /*
    * Save a pointer to the AGC data
    */
   short* agcPtr;

   /*
    * Maintain statistics for logging purposes
    */
   US_STATS stats;
} US_DATA;

/* struct to hold data for interrupt handler */

typedef struct
{
   US_DATA lo;                         /* lower sensor */
   US_DATA hi;                         /* upper sensor */

   /* interval data */
   unsigned short minAllowedCountsPerInterval;
   unsigned short maxAllowedCountsPerInterval;

   /* logging flags */
   unsigned char fatalErrorLogged;     /* =1 if information relating to a fatal error has been logged */

   /*
    * The following control when the sensor statistics are logged.  Each
    * sensor is logged only once per draw/return cycle.
    */
   unsigned char logLowSensorOnTransition;
   unsigned char logHighSensorOnTransition;
} intData;

static volatile intData data;
static volatile int     interruptCount;
static VOIDFUNCPTR      timerCallback;

static unsigned long    secondStageULSFiltering = 0; /* default value just in case */

static const char*      UPPER                   = "upper";
static const char*      LOWER                   = "lower";


/*
 * SPECIFICATION:    fluidCountsPerInterval
 *
 * ERROR HANDLING:   none
 *
 * Sum up the last 7 counts.  Since the history array isn't updated until after this call,
 * the last 7 counts is the current count plus the last 6 in the history buffer.  Since
 * the history is stored in a ring buffer, cycle through the buffer to get the current data.
 *
 */
static int fluidCountsPerInterval (const US_DATA* d)
{
   int i;
   int count = d->fluidCounts;   /* seed value with current fluid count */

   for (i = 0; i < FLUID_COUNT_SAMPLES; i++)
   {
      const int index = (LOG_SIZE + d->logIdx - i - 1) % LOG_SIZE;
      count += d->fluidCountLog[index];
   }

   return count;
}


#include <assert.h>

/*
 * SPECIFICATION:    read ultrasonics sensor
 *
 * ERROR HANDLING:   none
 */
static void readSensor (CtlHWUltrasonicSensor sensor, US_DATA* d)
{
   /*
    * read lower ultrasonics counters from gate array
    */
   unsigned short raw_data = chw_ultrasonicSensorGetCounter(sensor);
   d->air   = (unsigned char)(raw_data & 0x00ff) ;
   d->fluid = (unsigned char)((raw_data & 0xff00) >> 8);

   /*
    * process air counts
    */
   d->airCounts = d->air - d->lastAir;
   if ( d->airCounts < 0)
   {
      d->airCounts += MAX_COUNTS;
   }
   d->lastAir = d->air;

   /*
    * process fluid counts
    */
   d->fluidCounts = d->fluid - d->lastFluid;
   if ( d->fluidCounts < 0)
   {
      d->fluidCounts += MAX_COUNTS;
   }
   d->lastFluid = d->fluid;


   /*
    * Compute total count information
    */
   d->total     = d->fluidCounts + d->airCounts;
   d->accTotal += d->total;

   /*
    * Process valid data or process as an error condition
    */
   if ( d->total >= data.minAllowedCountsPerInterval &&
        d->total <= data.maxAllowedCountsPerInterval )
   {
      int firstStageResult;

      if ( d->fluidCounts > d->airCounts)
      {
         d->count++;                               /* bump and limit fluid count */
         if ( d->count > d->filterLength)
         {
            d->count = d->filterLength;
         }
      }
      else                                         /* air */
      {
         d->count--;                               /* decrement and limit air count */
         if (d->count < 0)
         {
            d->count = 0;
         }
      }

      firstStageResult = (d->count > (d->filterLength / 2));     /* set result */

      /* the low level sensor only has the first stage filter applied */
      if ( !secondStageULSFiltering ||
           (sensor == chw_lowerUltrasonicSensor) )
      {
         if (firstStageResult != d->result)
         {
            d->result    = firstStageResult;
            d->newResult = 1;
         }
      }
      else /* Upper level sensor */
      {
         /* If first stage trigger is true, meaning fluid */
         if (firstStageResult)
         {
            d->ulsFluidToAir = 0;

            if (firstStageResult != d->result)
            {
               const int fluidCounts = fluidCountsPerInterval(d);

               d->cyclesInSecondStageFilter++;

               if (fluidCounts > FLUID_COUNT_TRIGGER)
               {
                  d->result    = firstStageResult;
                  d->newResult = 1;
                  d->stats.secondStageNormalTransition++;
               }
               else if (d->cyclesInSecondStageFilter > FLUID_COUNT_MAX_ITERATIONS)
               {
                  d->result    = firstStageResult;
                  d->newResult = 1;
                  d->stats.secondStageLastChanceTransition++;
               }
            }
         }
         else
         {
            /* Zero the max duration counter for the second stage filter.  Must see */
            /* 20 consequitive hits of the first stage to be a bail-out criteria     */
            d->cyclesInSecondStageFilter = 0;
            d->ulsFluidToAir             = 1;

            // If we are transitioning to not fluid, then trigger the transition
            if (firstStageResult != d->result)
            {
               d->result    = firstStageResult;
               d->newResult = 1;
            }
         }
      }

      d->registerErrorCounter = MAX_INVALID_REG_ALLOWED;  /* clear error indicator */
   }
   else                                            /* invalid registers */
   {
      if (d->registerErrorCounter-- <= 0)
         d->fatalError = 1;
   }

   /*
    * Maintain history of readings for logging purposes
    */
   d->resultLog[d->logIdx]     = d->result;
   d->airCountLog[d->logIdx]   = d->airCounts;
   d->fluidCountLog[d->logIdx] = d->fluidCounts;
   d->agcLog[d->logIdx]        = *(d->agcPtr);
   d->logIdx                   = (d->logIdx + 1) % LOG_SIZE;

   /*
    * Update holdoff timer (used to restrict frequency of logging
    */
   if ( d->logHoldoff > 0 )
   {
      d->logHoldoff -= 1;
   }

   /*
    * Maintain statistical data.  Note that the logging routine only needs to set totalSamples to
    * zero, and the re-initialization for the next sampling period is handled here.  That avoids
    * any possible issues with data access within both the interrupt routine and the logging routine.
    * We also reset after 10000 samples (500 seconds) to keep accumulators from overflowing and
    * producing incorrect results.
    */
   if ( d->stats.totalSamples == 0 || d->stats.totalSamples > 10000 )
   {
      d->stats.minAGC       = d->stats.maxAGC = *(d->agcPtr);
      d->stats.minCounts    = d->stats.maxCounts = d->total;
      d->stats.totalCounts  = d->total;
      d->stats.totalSamples = 1;
   }
   else
   {
      d->stats.minAGC        = (*(d->agcPtr) < d->stats.minAGC) ? *(d->agcPtr) : d->stats.minAGC;
      d->stats.maxAGC        = (*(d->agcPtr) > d->stats.maxAGC) ? *(d->agcPtr) : d->stats.maxAGC;
      d->stats.minCounts     = (d->total < d->stats.minCounts) ? d->total : d->stats.minCounts;
      d->stats.maxCounts     = (d->total > d->stats.maxCounts) ? d->total : d->stats.maxCounts;

      d->stats.totalCounts  += d->total;
      d->stats.totalSamples += 1;
   }
}

/*
 * SPECIFICATION:    interrupt handler (periodic at 50 ms)
 *
 * ERROR HANDLING:   none
 */
static void intHandler (timer_t timerID, int arg)
{
   interruptCount += 1;
   readSensor(chw_lowerUltrasonicSensor, (US_DATA*)&data.lo);     /* read lower sensor */
   readSensor(chw_upperUltrasonicSensor, (US_DATA*)&data.hi);     /* read upper sensor */
}

static void LogSensorHistory (CtlHWUltrasonicSensor sensor, volatile US_DATA* d)
{
   static char logBuffer[128 + 5 * LOG_SIZE];

   const char* sensorName = (sensor == chw_lowerUltrasonicSensor) ? LOWER : UPPER;

   int         idx;
   US_DATA     data;
   int         oldInterruptCount;
   int         retryCount = 0;

   /*
    * Make sure we didn't have an interrupt in the middle of copying the data
    * The limit makes sure we don't lock up the system just for logging if
    * something strange is happening.
    */
   do
   {
      oldInterruptCount = interruptCount;
      data              = *d;
   } while ( oldInterruptCount != interruptCount && ++retryCount < 3 );

   memset(logBuffer, 0, sizeof(logBuffer));
   sprintf(logBuffer, "%s sensor result (new to old, 1=fluid) =", sensorName);
   for (idx = 0; idx<LOG_SIZE; idx++)
   {
      int circIdx = (LOG_SIZE + data.logIdx - idx - 1) % LOG_SIZE;
      sprintf(&logBuffer[strlen(logBuffer)], " %d", (int)(data.resultLog[circIdx]));
   }
   datalog_PrintToDefault(__FILE__, __LINE__, logBuffer);

   memset(logBuffer, 0, sizeof(logBuffer));
   sprintf(logBuffer, "%s air counts (new to old) =", sensorName);
   for (idx = 0; idx<LOG_SIZE; idx++)
   {
      int circIdx = (LOG_SIZE + data.logIdx - idx - 1) % LOG_SIZE;
      sprintf(&logBuffer[strlen(logBuffer)], " %d", (int)(data.airCountLog[circIdx]));
   }
   datalog_PrintToDefault(__FILE__, __LINE__, logBuffer);

   memset(logBuffer, 0, sizeof(logBuffer));
   sprintf(logBuffer, "%s fluid counts (new to old) =", sensorName);
   for (idx = 0; idx<LOG_SIZE; idx++)
   {
      int circIdx = (LOG_SIZE + data.logIdx - idx - 1) % LOG_SIZE;
      sprintf(&logBuffer[strlen(logBuffer)], " %d", (int)(data.fluidCountLog[circIdx]));
   }
   datalog_PrintToDefault(__FILE__, __LINE__, logBuffer);

   memset(logBuffer, 0, sizeof(logBuffer));
   sprintf(logBuffer, "%s raw AGC voltage (new to old) =", sensorName);
   for (idx = 0; idx<LOG_SIZE; idx++)
   {
      int circIdx = (LOG_SIZE + data.logIdx - idx - 1) % LOG_SIZE;
      sprintf(&logBuffer[strlen(logBuffer)], " %d", (int)(data.agcLog[circIdx]));
   }
   datalog_PrintToDefault(__FILE__, __LINE__, logBuffer);

   /*/ Don't log low level sensor or the ULS transition from air to fluid */
   if ( (sensor == chw_upperUltrasonicSensor) && !data.ulsFluidToAir)
   {
      datalog_PrintToDefault(__FILE__, __LINE__,
                             "%s sensor second stage statistics: totalSecondStage %d totalSamples %d count %d normalTransition %d lastChanceTransition %d",
                             sensorName, data.cyclesInSecondStageFilter, data.stats.totalSamples, data.count, data.stats.secondStageNormalTransition, data.stats.secondStageLastChanceTransition);
   }
}

static void LogFatalErrorHistory (const char* errorMessage)
{
   datalog_PrintToDefault(__FILE__, __LINE__, "%s: lowerResult=%d upperResult=%d",
                          errorMessage, (int)data.lo.result, (int)data.hi.result);
   LogSensorHistory(chw_lowerUltrasonicSensor, &data.lo);
   LogSensorHistory(chw_upperUltrasonicSensor, &data.hi);

   data.fatalErrorLogged = 1;
}

static void LogSensorStatistics (CtlHWUltrasonicSensor sensor, volatile US_DATA* data)
{
   US_STATS    stats;
   int         oldInterruptCount;
   int         retryCount = 0;

   const char* sensorName = (sensor == chw_lowerUltrasonicSensor) ? LOWER : UPPER;

   /*
    * Make sure we didn't have an interrupt in the middle of copying the data
    */
   do
   {
      oldInterruptCount = interruptCount;
      stats             = data->stats;
   } while ( oldInterruptCount != interruptCount && ++retryCount < 3 );

   /*
    * Force a reset of the statistics data.  Technically, we could could have an
    * interrupt between the while loop and this statement, but it would only mean
    * that we lose one 50 msec sample in the statistics over the draw/return cycle,
    * so it isn't worth the performance hit for disabling the interrupt.
    */
   data->stats.totalSamples = 0;

   if ( stats.totalSamples > 0 )
   {
      datalog_PrintToDefault(__FILE__, __LINE__,
                             "%s ultrasonic statistics: totalMin %ld totalMax %ld totalAve %.2lf",
                             sensorName, stats.minCounts, stats.maxCounts, stats.totalCounts / (double)stats.totalSamples);

      datalog_PrintToDefault(__FILE__, __LINE__,
                             "%s AGC statistics: min %d max %d",
                             sensorName, (int)stats.minAGC, (int)stats.maxAGC);

   }
}

/*
 * SPECIFICATION:    Get Upper Ultrasonic sensor error
 *
 * ERROR HANDLING:   none.
 */
static unsigned char ctl_interruptUpperSensorError (void)
{
   /*
    * Access data only once to avoid problems with interrupt during this function
    */
   unsigned char retVal = data.hi.fatalError;

   if ( retVal && !data.fatalErrorLogged )
   {
      LogFatalErrorHistory("upper level sensor fatal error");
   }

   return retVal;
}

/*
 * SPECIFICATION:    Get Lower Ultrasonic sensor error
 *
 * ERROR HANDLING:   none.
 */
static unsigned char ctl_interruptLowerSensorError (void)
{
   /*
    * Access data only once to avoid problems with interrupt during this function
    */
   unsigned char retVal = data.lo.fatalError;

   if ( retVal && !data.fatalErrorLogged )
   {
      LogFatalErrorHistory("lower level sensor fatal error");
   }

   return retVal;
}

/*
 * SPECIFICATION:    Initializes the ultrasonics data area
 *
 * ERROR HANDLING:   none
 */
static void ctl_initializeUSData (volatile US_DATA* data, int samples, CtlHWUltrasonicSensor sensor, short* agcPtr)
{
   unsigned short rawCounts = chw_ultrasonicSensorGetCounter(sensor);
   data->lastAir       = (unsigned char)(rawCounts & 0xff);
   data->lastFluid     = (unsigned char)((rawCounts >> 8) & 0xff);

   data->filterLength  = samples;
   data->count         = 0;
   data->result        = 0;
   data->result        = 0;
   data->ulsFluidToAir = 0;
   data->logHoldoff    = 0;
   data->cyclesInSecondStageFilter             = 0;
   data->registerErrorCounter                  = MAX_INVALID_REG_ALLOWED;
   data->fatalError                            = 0;
   data->accTotal                              = 0;
   data->logIdx                                = 0;
   data->agcPtr                                = agcPtr;
   data->stats.totalCounts                     = 0;
   data->stats.totalSamples                    = 0;
   data->stats.secondStageNormalTransition     = 0;
   data->stats.secondStageLastChanceTransition = 0;
   data->stats.minCounts                       = 0;
   data->stats.maxCounts                       = 0;
   data->stats.minAGC                          = 0;
   data->stats.maxAGC                          = 0;
}

/*
 * SPECIFICATION:    Attaches 50 ms timer 2.6 HW
 *
 * ERROR HANDLING:   _FATAL_ERROR
 */
void ctl_interruptAttach (short* lowAGCPtr, short* highAGCPtr)
{
   int actualIntervalMSec;
   int nominalCountsPerInterval;

   memset((void*)&data, 0, sizeof(data));

   ctl_initializeUSData(&data.lo, US_LOWER_SAMPLES, chw_lowerUltrasonicSensor, lowAGCPtr);
   ctl_initializeUSData(&data.hi, US_UPPER_SAMPLES, chw_upperUltrasonicSensor, highAGCPtr);

   /* set the mode of the upper level sensor processing.  It is either just the up/down count
    * filter, just like the lower level sensor, or it uses the second stage filter
    */
   ctl_interruptSetUpperSensorMode();

   data.fatalErrorLogged = 0;

   timerCallback         = intHandler;
   actualIntervalMSec    = trimaSysClockConnect(timerCallback, SAMPLE_RATE_IN_MS);

   /*
    * Ultrasonic data is update in hardware once every 256 uSec
    */
   nominalCountsPerInterval         = actualIntervalMSec * 1000 / 256;
   data.minAllowedCountsPerInterval = nominalCountsPerInterval * 9 / 10;
   data.maxAllowedCountsPerInterval = nominalCountsPerInterval * 11 / 10;

   datalog_PrintToDefault(__FILE__, __LINE__, "interrupt connected: interval=%d msec counts=%d-%d", actualIntervalMSec,
                          (int)data.minAllowedCountsPerInterval, (int)data.maxAllowedCountsPerInterval);
}

/*
 * SPECIFICATION:    Deattaches 50 ms timer
 *
 * ERROR HANDLING:   none
 */
void ctl_interruptDeattach (void)
{
   trimaSysClockDisconnect(timerCallback);
}

long ctl_interruptLowerSensorAir (void)
{
   return data.lo.airCounts;
}

long ctl_interruptLowerSensorFluid (void)
{
   return data.lo.fluidCounts;
}

long ctl_interruptUpperSensorAir (void)
{
   return data.hi.airCounts;
}

long ctl_interruptUpperSensorFluid (void)
{
   return data.hi.fluidCounts;
}

/*
 * SPECIFICATION:    Get Lower Ultrasonic sensor
 *
 * ERROR HANDLING:   none.
 */
unsigned char ctl_interruptLowerSensor (unsigned char* error)
{
   if ( data.lo.newResult && !data.lo.logHoldoff )
   {
      datalog_PrintToDefault(__FILE__, __LINE__, "lower level sensor state change");
      LogSensorHistory(chw_lowerUltrasonicSensor, &data.lo);
      data.lo.newResult  = 0;
      data.lo.logHoldoff = LOG_SIZE / 2;
   }

   if ( data.lo.result == 0 )
   {
      if ( data.logHighSensorOnTransition )
      {
         LogSensorStatistics(chw_upperUltrasonicSensor, &data.hi);
         data.logHighSensorOnTransition = 0;
      }

      data.logLowSensorOnTransition = 1;
   }

   *error = ctl_interruptLowerSensorError();
   return data.lo.result;
}

/*
 * SPECIFICATION:    Get Upper Ultrasonic sensor
 *
 * ERROR HANDLING:   none.
 */
unsigned char ctl_interruptUpperSensor (unsigned char* error)
{
   if ( data.hi.newResult && !data.hi.logHoldoff )
   {
      datalog_PrintToDefault(__FILE__, __LINE__, "upper level sensor state change");
      LogSensorHistory(chw_upperUltrasonicSensor, &data.hi);
      data.hi.newResult     = 0;
      data.hi.logHoldoff    = LOG_SIZE / 2;

      data.hi.ulsFluidToAir = 0;
      data.hi.stats.secondStageNormalTransition     = 0;
      data.hi.stats.secondStageLastChanceTransition = 0;
   }

   if ( data.hi.result == 1 )
   {
      if ( data.logLowSensorOnTransition )
      {
         LogSensorStatistics(chw_lowerUltrasonicSensor, &data.lo);
         data.logLowSensorOnTransition = 0;
      }

      data.logHighSensorOnTransition = 1;
   }

   *error = ctl_interruptUpperSensorError();
   return data.hi.result;
}

/*
 * SPECIFICATION:    Get Lower Ultrasonic sensor count total
 *
 * ERROR HANDLING:   none.
 */
unsigned long ctl_interruptLowerSensorAccTotal (void)
{
   unsigned long retValue;

   do
      retValue = data.lo.accTotal;
   while (retValue != data.lo.accTotal);

   return retValue;
}

/*
 * SPECIFICATION:    Get Upper Ultrasonic sensor count total
 *
 * ERROR HANDLING:   none.
 */
unsigned long ctl_interruptUpperSensorAccTotal (void)
{
   unsigned long retValue;

   do
      retValue = data.hi.accTotal;
   while (retValue != data.hi.accTotal);

   return retValue;
}

void ctl_interruptSetUpperSensorMode (void)
{
   char logString[80];

   // No longer use second_stage_uls_filtering config setting.  It is always on.
   secondStageULSFiltering = 1;

   sprintf(logString, "second stage ULS filtering is %s\n", (secondStageULSFiltering ? "ACTIVE" : "INACTIVE"));

   datalog_PrintToDefault(__FILE__, __LINE__, logString);
}

/* FORMAT HASH e3ff9616d21a42ee3de8abc58dca8101 */

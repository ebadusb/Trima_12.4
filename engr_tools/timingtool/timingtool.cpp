/*
 * $Header$
 * $Log$
 * 
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      timingtool.cpp
 * PURPOSE:    debug test tool for monitoring/controlling Trima low-level timing
 */

#include <conio.h>
#include <i86.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/irqinfo.h>
#include <sys/kernel.h>
#include <sys/osinfo.h>

//
// 8254 hardware definitions
//
enum { TMR8254_BASE = 0x40 };
enum { TMR8254_CONTROL = TMR8254_BASE + 0x03 };
enum { TMR8254_OUTPUT_MASK = 0x80 };

//
// RTC hardware definitions
//
enum { RTC_ADDRESS_REG = 0x70 };
enum { RTC_DATA_REG = 0x71 };
enum { RTC_IRQ = 0x08 };

enum { RTC_STATUS_A = 0x0a };
enum { RTC_RATE_MASK = 0x0f };
enum { RTC_RATE_1024HZ = 0x06 };

enum { RTC_STATUS_B = 0x0b };
enum { RTC_PIE = 0x40 };

enum { RTC_STATUS_C = 0x0c };

//
// Stores readback information about 8254 timer
// See Intel 82C54 documentation for details
//
struct TimingInfo
{
   unsigned char  c8254Status[3];      // status byte for each timer
} timingInfo;

//
// Stores information collected through RTC IRQ handler
//
struct IRQInfo
{
   unsigned long  intCount;            // number of RTC interrupts
   unsigned char  outState[3];         // current timer output pin state
   unsigned long  outCount[3];         // number of output pin transitions
} irqInfo;

void Usage(
   char * progName
   )
{
   fprintf(stderr, "Monitor/Control Trima low level timing\n");
   fprintf(stderr, "Usage: %s <options>\n", progName);
   fprintf(stderr, "With no options, information about internal timing will be displayed.\n");
   fprintf(stderr, "The -a [percent] option can be used to adjust timing.  For example\n");
   fprintf(stderr, "  %s -a 1.0\n");
   fprintf(stderr, "will adjust the internal timing control registers so that low_level\n");
   fprintf(stderr, "timing will 1.0%% too slow (i.e. 1 msec of real time will be measured as\n");
   fprintf(stderr, "0.99 msec by low level timing.  Negative values can be used to adjust\n");
   fprintf(stderr, "timing to be too fast.\n");
   fprintf(stderr, "The -d option can be used to suppress timer data collection.  This option\n");
   fprintf(stderr, "is necessary for running on the safety controller.\n");
   fprintf(stderr, "The -q option can be used to suppress status messages during program\n");
   fprintf(stderr, "operation.\n");
   
   exit(1);
}

#pragma off(check_stack)
pid_t far RTC_Handler(void)
{
   //
   // track the number of interrupts
   //
   irqInfo.intCount += 1;

   //
   // latch current counter status bytes and check for output transition
   //
   outp(TMR8254_CONTROL, 0xee);
   for (int i=0; i<3; i++)
   {
      unsigned char status = (unsigned char)inp(TMR8254_BASE+i);
      unsigned char newOutState = (status & TMR8254_OUTPUT_MASK) ? 1 : 0;

      if (irqInfo.outState[i] == 0 && newOutState == 1)
      {
         irqInfo.outCount[i] += 1;
      }

      irqInfo.outState[i] = newOutState;
   }

   //
   // Read RTC_STATUS_C to ack interrupt
   //
   outp(RTC_ADDRESS_REG, RTC_STATUS_C);
   inp(RTC_DATA_REG);

   return 0;
}
#pragma on(check_stack)

void
CollectTimingInfo(
   int quiet        // non-zero if status messages should be suppressed
   )
{
   //
   // Setup RTC for 1024 Hz interrupt rate
   //
   _disable();
   outp(RTC_ADDRESS_REG, RTC_STATUS_A);
   unsigned char  statusA = (unsigned char)inp(RTC_DATA_REG);

   statusA = (unsigned char)(statusA & ~RTC_RATE_MASK);
   statusA = (unsigned char)(statusA | RTC_RATE_1024HZ);
   outp(RTC_ADDRESS_REG, RTC_STATUS_A);
   outp(RTC_DATA_REG, statusA);
   _enable();

   //
   // Latch status bytes for each counter and read back
   //
   _disable();
   outp(TMR8254_CONTROL, 0xee);
   for (int i=0; i<3; i++)
   {
      timingInfo.c8254Status[i] = (unsigned char)inp(TMR8254_BASE+i);
      irqInfo.outState[i] = (timingInfo.c8254Status[i] & TMR8254_OUTPUT_MASK) ? 1 : 0;
      irqInfo.outCount[i] = 0;
   }
   _enable();

   //
   // Attach handler to RTC periodic interrupt and enable interrupt
   //
   irqInfo.intCount = 0;
   int   irqID = qnx_hint_attach(RTC_IRQ, &RTC_Handler, FP_SEG(&irqInfo));

   _disable();
   outp(RTC_ADDRESS_REG, RTC_STATUS_B);
   unsigned char  statusB = (unsigned char)inp(RTC_DATA_REG);
   statusB |= RTC_PIE;
   outp(RTC_DATA_REG, statusB);

   //
   // dummy read of RTC_STATUS_C to ack any previous interrupts
   //
   outp(RTC_ADDRESS_REG, RTC_STATUS_C);
   inp(RTC_DATA_REG);
   _enable();

   if (!quiet) printf("Collecting timer information ...\n");
   unsigned long  totalDelay = 0;
   while (totalDelay < 10000)
   {
      totalDelay += 1000-delay(1000);
   }

   _disable();
   outp(RTC_ADDRESS_REG, RTC_STATUS_B);
   statusB = (unsigned char)inp(RTC_DATA_REG);
   statusB &= ~RTC_PIE;
   outp(RTC_DATA_REG, statusB);
   _enable();

   qnx_hint_detach(irqID);
   if (irqInfo.intCount < 7*1024 || irqInfo.intCount > 13*1024)
   {
      fprintf(stderr, "RTC hardware fault - intCount = %lu\n", irqInfo.intCount);
      exit(1);
   }

   if (!quiet)
   {
      for (int i=0; i<3; i++)
      {
         printf("8254 timer %d rate = %8.2lf Hz\n",
            i, irqInfo.outCount[i]/(irqInfo.intCount/1024.0));
      }
   }
}

void
AdjustTiming(
   double adjustPercent,   // adjustment amount (+=faster, -=slower)
   int quiet               // non-zero if status messages should be suppressed
   )
{
   struct _osinfo osdata;
   qnx_osinfo(0, &osdata);

   if (adjustPercent < -10 || adjustPercent > 10)
   {
      fprintf(stderr, "Specified adjustment %lg is out of range\n", adjustPercent);
      exit(1);
   }

   double   newTickSize = osdata.tick_size * (1.0 + adjustPercent/100.0);
   if (!quiet) printf("Adjust %d usec tick size to %.2lf usec\n", osdata.tick_size, newTickSize);

   double   new8254Count = 1.19318e6 * newTickSize/1.0e6;
   int      new8254Reload = (int)(new8254Count+0.5);
   if (!quiet) printf("New 8254 count reload value = %d\n", new8254Reload);

   if (new8254Reload < 0x100 || new8254Reload > 0xffff)
   {
      fprintf(stderr, "8254 count reload %d out of range\n", new8254Reload);
      exit(1);
   }

   if (timingInfo.c8254Status[0] & 0x3f != 0x36)
   {
      fprintf(stderr, "unexpected 8254 timer 0 mode = 0x%02X\n", timingInfo.c8254Status[0]);
      exit(1);
   }

   _disable();
   outp(TMR8254_CONTROL, 0x36);
   outp(TMR8254_BASE, new8254Reload & 0xff);
   outp(TMR8254_BASE, (new8254Reload & 0xff00) >> 8);
   _enable();
}

int
main(int argc, char * argv[])
{
   int      adjust = 0;
   double   adjustPercent = 0.0;
   int      dataCollectionEnabled = 1;
   int      parseArg = 1;
   int      quiet = 0;

   while (parseArg < argc)
   {
      if (strcmp(argv[parseArg], "-a") == 0)
      {
         adjust = 1;
         if (parseArg+1 >= argc ||
             sscanf(argv[parseArg+1], "%lf", &adjustPercent) != 1)
         {
            Usage(argv[0]);
         }

         parseArg += 2;
      }
      else if (strcmp(argv[parseArg], "-d") == 0)
      {
         dataCollectionEnabled = 0;
         parseArg += 1;
      }
      else if (strcmp(argv[parseArg], "-q") == 0)
      {
         quiet = 1;
         parseArg += 1;
      }
      else
      {
         Usage(argv[0]);
      }
   }

   if (dataCollectionEnabled)
   {
      CollectTimingInfo(quiet);
   }
   else
   {
      timingInfo.c8254Status[0] = 0x36;
   }

   if (adjust)
   {
      AdjustTiming(adjustPercent, quiet);
   }

   return 0;
}

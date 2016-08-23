/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/os/rcs/cpu_speed.c 1.1 2007/11/07 22:38:37Z dslausb Exp $
 * $Log: cpu_speed.c $
 * Revision 1.1  2007/11/07 22:38:37Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:34:08Z  dslausb
 * Initial revision
 * Revision 1.1  2007/11/07 22:28:32Z  dslausb
 * Initial revision
 * Revision 1.4  2007/08/28 18:41:22Z  rhecusb
 * sysCPUSpeed() does not work in vxWorks 5.5.1
 * Revision 1.2  2007/08/01 22:39:25Z  dslausb
 * Add speed test function
 * Revision 1.1  2007/05/22 18:20:08Z  dslausb
 * Initial revision
 * Revision 1.1  2004/03/30 19:31:00Z  jl11312
 * Initial revision
 * Revision 1.1  2003/04/30 15:19:42Z  jl11312
 * Initial revision
 */

/*
 * This variable is declared static to prevent optimizer from
 * removing computations in the cpuSpeedTest function (the optimizer
 * could detect that the value for an automatic was never used).
 */
static volatile unsigned long testValue          = 0;
unsigned long                 cpuSpeedTestResult = 0;
void cpuSpeedTest (void);

void cpuSpeedTest (void)
{
   int           ticksPerSec  = sysClkRateGet();
   int           testDuration = ticksPerSec / 10;
   int           testIter;
   unsigned long startTick;

   if ( testDuration < 2 ) testDuration = 2;
   startTick = tickGet();
   while ( startTick == tickGet() )
      ;

   startTick          = tickGet();
   cpuSpeedTestResult = 0;
   while ( tickGet() < startTick + testDuration )
   {
      cpuSpeedTestResult += 1;
      for ( testIter = 0; testIter<100; testIter += 1 )
      {
         /*
          * An arbitrary expression
          */
         testValue = ((testValue + 12345) << 3) ^ 0x1234;
      }
   }
}

/* CPU TEST DOESN'T WORK IN VX55
#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

unsigned long sysCPUSpeed(void)
{
    return cpuSpeedTestResult;
}

#ifdef __cplusplus
}; // extern "C"
#endif // ifdef __cplusplus
*/

/* FORMAT HASH c47c9ee83d8ccea0757c7216e3abbbbd */

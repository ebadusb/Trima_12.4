#ifndef _CASS_MONITOR_HPP
#define _CASS_MONITOR_HPP

#include "s_monitor.hpp"

static const int HALL_MAGNETS[] = {
   1L,   /* Return */
   1L,   /* AC */
   1L,   /* Inlet */
   1L,   /* Plasma */
   2L    /* Collect */
};

/**
 * CassetteMonitor
 *
 * ROLE: Provide safety mitigation monitoring regarding cassette position and pump action.
 *
 * RESPONSIBILITIES:
 * 1) affect a T1 test for hall sensor
 * 2) alarm if disposible test is reached with any pump reporting less than 5 revs of movement
 **/
class HallT1Monitor
   : public Monitor
{
public:

   HallT1Monitor(void)
      : Monitor("HallT1Monitor", false){}
   virtual ~HallT1Monitor(void) { }

   virtual void check (void);
};

#endif  // _CASS_MONITOR_HPP

/* FORMAT HASH ca659932118e3aa402d99f8f07e9a211 */

#ifndef PUMP_STATUS_HPP  //  Prevents multiple compilations.
#define PUMP_STATUS_HPP

#include "chw.hpp"
#include "s_set.hpp"

typedef struct
{
   long rpm;                  // 0 to ~250 rpm
   long deltaHalls;           // delta counts
   long accumHalls;           // total counts
   long offsetHalls;          // offset from PFR
} SafetyPump;

typedef struct
{
   float revs;               // Delta revs since last update
   long  accum;              // total revs, encoder ticks
   long  offset;             // offset from PFR
} ControlPump;

class ControlStatus_Export
{
public:
   ControlPump     pumps[5];

   HW_VALVE_STATES valves[3];
   short           accessPressure;
   struct timespec timestamp;
};

class SafetyStatus_Export
{
public:
   SafetyPump pumps[5];
};

class ControlCommand_Export
{
public:
   float pumpRpm[5];
};


class SafetyDerived
{
public:
   float stroke[5];          // stroke volume
   float volumes[6];         // calculated volumes for pumps + RBC
};
#endif  // PUMP_STATUS_HPP

/* FORMAT HASH f6c185a37faff5fa29efa75173e7c55b */

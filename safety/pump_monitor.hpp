#ifndef _PUMP_MONITOR_INCLUDE
#define _PUMP_MONITOR_INCLUDE

#include <string>
#include "s_monitor.hpp"
#include "s_names.hpp"
#include "states.h"
#include "timermessage.h"

#define TIME_BUFFER_SIZE    20

typedef struct
{
   long starting;
   long last;
   long movement;
   long whileDisengaged;
} PumpData;

static const int MAGNET_COUNT[] = {
   1L,   /* Return */
   1L,   /* AC */
   1L,   /* Inlet */
   1L,   /* Plasma */
   2L    /* Collect */
};

// limits
static const int          REV_LIMIT[] = {
   7,   /* Return */
   4,   /* AC */
   7,   /* Inlet */
   4,   /* Plasma */
   4    /* Collect */
};

static const ALARM_VALUES PUMP_FAILURE_MODES[][2] = {
   {RETURN_PUMP_HALL_TOO_SMALL, RETURN_PUMP_HALL_TOO_BIG},
   {AC_PUMP_HALL_TOO_SMALL, AC_PUMP_HALL_TOO_BIG},
   {INLET_PUMP_HALL_TOO_SMALL, INLET_PUMP_HALL_TOO_BIG},
   {PLASMA_PUMP_HALL_TOO_SMALL, PLASMA_PUMP_HALL_TOO_BIG},
   {PLATELET_PUMP_HALL_TOO_SMALL, PLATELET_PUMP_HALL_TOO_BIG}
};

enum RESET_REASON
{
   UNKNOWN = 0,
   NO_MOVING_DETECTED_BY_HALLS   = 1,
   NO_MOVING_DETECTED_BY_ENCODER = 2,
   PUMP_0_RPM_COMMAND_SENT       = 4,
   SYSTEM_STATE_CHANGED          = 8,
   PUMP_DIR_CHANGED              = 16
};

DataLog_Stream& operator << (DataLog_Stream& os, RESET_REASON reason);
DataLog_Stream& operator << (DataLog_Stream& os, State_names stateName);

class PumpMonitor
   : public Monitor
{
#if defined(__HALLSTEST__)
   friend class HallsTest;
#endif
public:

   PumpMonitor(void);
   virtual ~PumpMonitor(void) { }

   virtual void check (void){ checkHallsVsEncoders(); }

private:

   void checkHallsVsEncoders (void);
   bool hallsMoving (int pump, long currentHalls);
   bool encMoving (int pump, long currentEnc);
   void zeroError (int pump, State_names state, int reasons);
   void stateReset (SafetyPumps pump, State_names state);
   void logRevError ();

   // telemetry
   enum SOURCES { ENCODERS       = 0, HALLS };

   enum MIN_MOVEMENT { MIN_HALLS = 1, MIN_ENCODERS = 114 };

   int          m_lastControlStatusTimeIndex;
   timespec     m_lastControlStatusTimes[TIME_BUFFER_SIZE];

   bool         m_lastReturnDirectionForward;
   long         m_lastState;

   PumpData     data[5][2];
   double       m_errLimit[5];
   TimerMessage m_logInfoTimer;

   long         m_lastLoggedEncoders[5];
   long         m_lastLoggedHalls[5];
   bool         m_lastEncodersSeen[5];
   bool         m_extraLogging;
};

#endif /* ifndef _MONITOR_INCLUDE */

/* FORMAT HASH 0f1d5df8a99d961f6578bee0f3f98186 */

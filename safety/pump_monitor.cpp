#include <vxWorks.h>
#include <math.h>

#include "pump_monitor.hpp"
#include "state_objects.hpp"
#include "s_exec.hpp"
#include "s_names.hpp"


extern bool acAlarmActive[NUM_SAFETY_ALARMS];

//
//
//  This flag is used to ignore alarm conditions the first pass through the code.  After a PFR, there is
//  a one-frame delay before the control status messages resync, causing spurious additional alarms.
//
static int firstTime = 1;

PumpMonitor::PumpMonitor()
   : Monitor("PumpMonitor", true),
     m_lastControlStatusTimeIndex(0),
     m_lastReturnDirectionForward(false),
     m_lastState((long)FIRST_STATE),
     m_extraLogging(false),
     m_logInfoTimer(6000ul, Callback< PumpMonitor >(this, &PumpMonitor::logRevError), TimerMessage::ARMED)
{
   // initialize buffers
   memset(m_lastControlStatusTimes, 0, sizeof(timespec) * TIME_BUFFER_SIZE);
   memset(data, 0, sizeof(PumpData) * 10);

   for (int pumpIndex = (int)RETURN_PUMP; pumpIndex <= (int)PLATELET_PUMP; pumpIndex++ )
   {
      m_lastLoggedEncoders[pumpIndex] = 0;
      m_lastLoggedHalls[pumpIndex]    = 0;
      m_lastEncodersSeen[pumpIndex]   = false;

      m_errLimit[pumpIndex]           = (REV_LIMIT[pumpIndex] * ENCODER_TICKS_PER_REV);
   }

   FILE* fp = fopen(PUMP_STROKE_LOGGING_ON, "r");
   if (fp)
   {
      m_extraLogging = true;
      fclose(fp);
   }
   DataLog(log_level_safe_exec_info) << "pump_halls_encoders_logging: " << m_extraLogging << endmsg;
}

/**
 * Performs the actual halls vs. encoders check.
 *
 * RETURN: NONE (in case of alarm, we don't return)
 **/
void PumpMonitor::checkHallsVsEncoders ()
{
   for ( int pumpIndex = (int)RETURN_PUMP; pumpIndex <= (int)PLATELET_PUMP; pumpIndex++ )
   {
      long        s_state  = SafetyExec::instance()->SystemState();

      SafetyPumps pumpEnum = static_cast<SafetyPumps>(pumpIndex);

      if ( s_state != m_lastState )
         stateReset( (SafetyPumps)pumpIndex, (State_names)s_state);

      long currentEnc   = m_controlStatus.pumps[pumpIndex].accum;
      long currentHalls = m_safetyStatus.pumps[pumpIndex].accumHalls;

      bool encodersSeen = encMoving(pumpIndex, currentEnc);
      bool hallsSeen    = hallsMoving(pumpIndex, currentHalls);

      if ( !encodersSeen && !hallsSeen )
      {
         // only reset the counters if encoders and halls both appear to be
         // stopped AND the pump was actually commanded to do so
         if ( m_commands.pumpRpm[pumpIndex] == 0 )
         {
            int reason =
               NO_MOVING_DETECTED_BY_HALLS | NO_MOVING_DETECTED_BY_ENCODER | PUMP_0_RPM_COMMAND_SENT;
            zeroError(pumpIndex, static_cast<State_names>(s_state), reason);
         }
      }
      else
      {
         if ((m_controlStatus.timestamp.tv_sec != m_lastControlStatusTimes[m_lastControlStatusTimeIndex].tv_sec) ||
             (m_controlStatus.timestamp.tv_nsec != m_lastControlStatusTimes[m_lastControlStatusTimeIndex].tv_nsec))
         {
            data[pumpIndex][HALLS].movement    = currentHalls - data[pumpIndex][HALLS].starting;
            data[pumpIndex][ENCODERS].movement = currentEnc - data[pumpIndex][ENCODERS].starting;

            // check for bad shaft encoder (halls w/ no encoders)
            if ( !encodersSeen )
            {
               // encoders aren't moving, but halls are...
               long hallsWithNoEnc = labs(currentHalls - data[pumpIndex][HALLS].whileDisengaged);

               if ( hallsWithNoEnc > (REV_LIMIT[pumpIndex] * MAGNET_COUNT[pumpIndex]) )
               {
                  // filter if this is the return pump and we just changed directions
                  bool bSoundAlarm = true;

                  if ( pumpIndex == RETURN_PUMP )
                  {
                     bool forwardDir = (m_commands.pumpRpm[pumpIndex] > 0);

                     if ( forwardDir != m_lastReturnDirectionForward )
                     {
                        bSoundAlarm = false;

                        DataLog(log_level_safe_exec_info) << name() << ": skipping dir change (HnE)." << endmsg;
                     }
                  }

                  if ( bSoundAlarm )
                  {
                     DataLog(log_level_safe_exec_error)
                        <<  name() << ": Halls vs Encoders; (" << pumpEnum
                        << ") Halls with no encoders. "
                        << "SHWNE(" << data[pumpIndex][HALLS].whileDisengaged << ")" << endmsg;

                     alarm(OFF_BOTH, PUMP_FAILURE_MODES[pumpIndex][HALLS]);
                  }
               }
            }
            else        // if (encodersSeen)
            {
               data[pumpIndex][HALLS].whileDisengaged = currentHalls;

               if (m_lastEncodersSeen[pumpIndex] == false )
               {
                  DataLog(log_level_safe_exec_info) << "Reset" << (SafetyPumps)pumpIndex << "hall count to: " \
                                                    << currentHalls << " .Current commanded RPM: " << m_commands.pumpRpm[pumpIndex] << endmsg;
               }
            }

            bool   alarmCondition = false;
            double error          = 0.0;

            // do core halls vs. encoders check (accounting for reset at pump stops)
            error = data[pumpIndex][ENCODERS].movement -
                    ((data[pumpIndex][HALLS].movement / MAGNET_COUNT[pumpIndex]) * ENCODER_TICKS_PER_REV);

            DataLog(log_level_safe_exec_debug) << name() << " (" << pumpEnum
                                               << "): encoders=" << data[pumpIndex][ENCODERS].movement
                                               << ";halls=" << ((data[pumpIndex][HALLS].movement / MAGNET_COUNT[pumpIndex]) * ENCODER_TICKS_PER_REV)
                                               << ";error=" << error
                                               << ";limit=" << m_errLimit[pumpIndex] << endmsg;

            if ( fabs(error) >  m_errLimit[pumpIndex])
            {
               alarmCondition = true;

               DataLog(log_level_safe_exec_info) << name() << " (" << pumpEnum << ") error:" << error << " > " << m_errLimit[pumpIndex] << endmsg;

               // give a grace period to the return pump if we recently
               // changed directions
               if ( pumpIndex == RETURN_PUMP )
               {
                  bool forwardDir = (m_commands.pumpRpm[pumpIndex] > 0);

                  if ( forwardDir != m_lastReturnDirectionForward )
                  {
                     alarmCondition = false;
                     DataLog(log_level_safe_exec_info) <<  name() << ": skipping dir change (HvE)." << endmsg;
                  }
               }
            }

            // keep track of pump 3 dir
            if ( pumpIndex == RETURN_PUMP )
            {
               bool forwardDir = (m_commands.pumpRpm[pumpIndex] > 0);

               if ((forwardDir != m_lastReturnDirectionForward) && (m_commands.pumpRpm[pumpIndex] != 0))
               {
                  zeroError(pumpIndex, static_cast<State_names>(s_state), PUMP_DIR_CHANGED);
               }
               m_lastReturnDirectionForward = forwardDir;
            }

            if ( alarmCondition && (!firstTime))
            {
               ALARM_VALUES mode = NULL_ALARM;

               // alarm
               if ( error < 0 )
                  // generate halls exceed revs alarm for "this" pump
                  mode = PUMP_FAILURE_MODES[pumpIndex][HALLS];
               else
                  // generate revs exceed halls alarm for "this" pump
                  mode = PUMP_FAILURE_MODES[pumpIndex][ENCODERS];

               if (!acAlarmActive[ mode - FIRST_SAFETY_ALARM ] )
               {
                  DataLog(log_level_safety_alarm_detail) << pumpEnum
                                                         << ", Err:"
                                                         << error
                                                         << ", Enc:"
                                                         << currentEnc
                                                         << ", Hall Equiv:"
                                                         << ((currentHalls / MAGNET_COUNT[pumpIndex]) * ENCODER_TICKS_PER_REV)
                                                         << ", Limit = "
                                                         << REV_LIMIT[pumpIndex]
                                                         << endmsg;

                  acAlarmActive[ mode - FIRST_SAFETY_ALARM ] = true;
               }

               alarm(OFF_BOTH, mode);
            }
         }
      }
      if (m_extraLogging)
      {
         double error = 0.0;
         error = data[pumpIndex][ENCODERS].movement -
                 ((data[pumpIndex][HALLS].movement / MAGNET_COUNT[pumpIndex]) * ENCODER_TICKS_PER_REV);

         DataLog(log_level_safe_exec_info) << name() << " (" << pumpEnum
                                           << "): encoders=" << data[pumpIndex][ENCODERS].movement
                                           << ";halls=" << ((data[pumpIndex][HALLS].movement / MAGNET_COUNT[pumpIndex]) * ENCODER_TICKS_PER_REV)
                                           << ";error=" << error
                                           << ";limit=" << m_errLimit[pumpIndex] << endmsg;
      }
      m_lastEncodersSeen[pumpIndex] = encodersSeen;
   }

   m_lastState                  = SafetyExec::instance()->SystemState();

   m_lastControlStatusTimeIndex = ++m_lastControlStatusTimeIndex % TIME_BUFFER_SIZE;

   m_lastControlStatusTimes[m_lastControlStatusTimeIndex].tv_sec  = m_controlStatus.timestamp.tv_sec;
   m_lastControlStatusTimes[m_lastControlStatusTimeIndex].tv_nsec = m_controlStatus.timestamp.tv_nsec;

   firstTime = 0; // for every iteration through the code excepting the first
}

/**
 * Tests current pump encoder data for actual
 * movement above a predefined threshold.
 *
 * RETURN: TRUE if encoders indicate pump is
 * actually moving; otherwise, FALSE
 **/
bool PumpMonitor::encMoving (int pump, long currentEnc)
{
   bool ret = false;

   if ( labs(currentEnc - data[pump][ENCODERS].last) >= MIN_ENCODERS )
   {
      data[pump][ENCODERS].last = currentEnc;
      ret = true;
   }

   return ret;
}

/**
 * Tests current pump hall data for actual
 * movement above a predefined threshold.
 *
 * RETURN: TRUE if halls indicate pump is
 * actually moving; otherwise, FALSE
 **/
bool PumpMonitor::hallsMoving (int pump, long currentHalls)
{
   bool ret = false;

   if ( labs(currentHalls - data[pump][HALLS].last) >= MIN_HALLS )
   {
      data[pump][HALLS].last = currentHalls;
      ret                    = true;
   }

   return ret;
}

void PumpMonitor::stateReset (SafetyPumps pump, State_names state)
{
   DataLog(log_level_safe_exec_debug) << "stateReset Pump " << pump << "state " << state << endmsg;
   switch (pump)
   {
      case RETURN_PUMP :
         if ( (state == DISPOSABLE_TEST) || (state == BLOOD_PRIME) || (state == BLOOD_RUN) )
            zeroError(pump, state, SYSTEM_STATE_CHANGED);

         break;

      case AC_PUMP :
         if ( state == DISPOSABLE_TEST )
            zeroError(pump, state, SYSTEM_STATE_CHANGED);

         break;

      case INLET_PUMP :
         if ( (state == DISPOSABLE_TEST) || (state == BLOOD_PRIME) || (state == BLOOD_RUN) )
            zeroError(pump, state, SYSTEM_STATE_CHANGED);

         break;

      case PLASMA_PUMP :
         if ( (state == DISPOSABLE_TEST) || (state == BLOOD_PRIME) )
            zeroError(pump, state, SYSTEM_STATE_CHANGED);

         break;

      case PLATELET_PUMP :
         if ( (state == DISPOSABLE_TEST) || (state == BLOOD_PRIME) )
            zeroError(pump, state, SYSTEM_STATE_CHANGED);

         break;

      default :
         break;
   }
}

/**
 * Common function used to zero the error.  The error
 * term isn't global. Instead, we simply move the 'start'
 * marker for halls and encoders to last reported.
 *
 * RETURN: NONE
 **/
void PumpMonitor::zeroError (int pump, State_names state, int reasons)
{
   bool changed = false;

   if (data[pump][HALLS].starting != data[pump][HALLS].last)
   {
      data[pump][HALLS].starting = data[pump][HALLS].last;
      changed                    = true;
   }
   if (data[pump][ENCODERS].starting != data[pump][ENCODERS].last)
   {
      data[pump][ENCODERS].starting = data[pump][ENCODERS].last;
      changed = true;
   }
   if (data[pump][HALLS].whileDisengaged != data[pump][HALLS].last)
   {
      data[pump][HALLS].whileDisengaged = data[pump][HALLS].last;
      changed = true;
   }
   if (data[pump][ENCODERS].whileDisengaged != data[pump][ENCODERS].last)
   {
      data[pump][ENCODERS].whileDisengaged = data[pump][ENCODERS].last;
      changed = true;
   }
   if (changed)
   {
      DataLog(log_level_safe_exec_info) << (SafetyPumps)pump << "hall or encoder counter is reset during " \
                                        << state << (RESET_REASON)reasons << endmsg;
   }
}

//
//
// Periodic logging of halls vs encoders information
//
void PumpMonitor::logRevError ()
{
   int  pumpIndex;

   bool changed = false;

   for (pumpIndex = (int)RETURN_PUMP; pumpIndex <= (int)PLATELET_PUMP; pumpIndex++ )
   {
      if ((m_lastLoggedEncoders[pumpIndex] != m_controlStatus.pumps[pumpIndex].accum) ||
          (m_lastLoggedHalls[pumpIndex] != m_safetyStatus.pumps[pumpIndex].accumHalls))
      {
         changed = true;
         break;
      }
   }


   if (changed)
   {
      DataLog(log_level_safe_exec_info) << "HvE: ";
      for (pumpIndex = (int)RETURN_PUMP; pumpIndex <= (int)PLATELET_PUMP; pumpIndex++ )
      {
         SafetyPumps pumpEnum = static_cast<SafetyPumps>(pumpIndex);
         m_lastLoggedEncoders[pumpIndex] = m_controlStatus.pumps[pumpIndex].accum;
         m_lastLoggedHalls[pumpIndex]    = m_safetyStatus.pumps[pumpIndex].accumHalls;
         long encFromHalls = ((m_lastLoggedHalls[pumpIndex] / MAGNET_COUNT[pumpIndex]) * ENCODER_TICKS_PER_REV);

         DataLog(log_level_safe_exec_info) << (SafetyPumps)pumpEnum << ": " << m_lastLoggedEncoders[pumpIndex] << " " << m_lastLoggedHalls[pumpIndex] << " " << encFromHalls << " ; ";
      }
      DataLog(log_level_safe_exec_info) << endmsg;
   }
}

DataLog_Stream& operator << (DataLog_Stream& os, RESET_REASON resetReasons)
{
   int          reasons = (int)resetReasons;

   RESET_REASON reason;
   int          shift = 0;

   while (reasons > 0)
   {
      reason = (RESET_REASON)(reasons % 2);
      shift++;

      if (reason == 1)
      {
         if (shift == 1 )
         {
            os << " No moving detected by halls ";
         }
         else if (shift == 2)
         {
            os << " No moving detected by encoders ";
         }
         else if (shift == 3)
         {
            os << " Command RPM = 0 ";
         }
         else if (shift == 4)
         {
            os << " System state changed ";
         }
         else if (shift == 5)
         {
            os << " Return pump direction changed ";
         }
      }
      reasons /= 2;
   }
   return os;
}

DataLog_Stream& operator << (DataLog_Stream& os, State_names stateName)
{
   switch (stateName)
   {
      case FIRST_STATE :                os << "FIRST_STATE";                break;
      case SELF_TEST :                  os << "SELF_TEST";                  break;
      case STARTUP_TESTS :              os << "STARTUP_TESTS";              break;
      case DISPOSABLE_TEST :            os << "DISPOSABLE_TEST";            break;
      case AC_CONNECTED :               os << "AC_CONNECTED";               break;
      case AC_PRIME :                   os << "AC_PRIME";                   break;
      case DONOR_CONNECTED :            os << "DONOR_CONNECTED";            break;
      case BLOOD_PRIME :                os << "BLOOD_PRIME";                break;
      case BLOOD_RUN :                  os << "BLOOD_RUN";                  break;
      case BLOOD_RINSEBACK :            os << "BLOOD_RINSEBACK";            break;
      case DONOR_DISCONNECT :           os << "DONOR_DISCONNECT";           break;
      case POST_RUN :                   os << "POST_RUN";                   break;
      case METERED_STORAGE :            os << "METERED_STORAGE";            break;
      case METERED_STORAGE_DISCONNECT : os << "METERED_STORAGE_DISCONNECT"; break;
      case SERVICE_MODE :               os << "SERVICE_MODE";               break;
      case DEFAULT_STATE :              os << "DEFAULT_STATE";              break;
      default :                         os << "UNKNOWN";                    break;
   }

   return os;
}

/* FORMAT HASH a158fef143ccef959bf3c403dbb5daf5 */

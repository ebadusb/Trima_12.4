#include "cassette_monitor.hpp"
#include "s_names.hpp"
#include "state_objects.hpp"
#include "s_exec.hpp"

void HallT1Monitor::check ()
{
   static const int MIN_REVS = 5;

   for ( int i = RETURN_PUMP; i <= PLATELET_PUMP; i++ )
   {
      const long accumHalls = labs(m_safetyStatus.pumps[i].accumHalls);
      const int  errorLimit = MIN_REVS * HALL_MAGNETS[i];

      if (accumHalls < errorLimit)  // not direction specific
      {
         // we should have seen more revs by now,
         // so alarm
         DataLog(log_level_safety_alarm_detail) << (SafetyPumps)i
                                                << " failed hall sensor T1 test.  Accum revs " << accumHalls << " < limit of " << errorLimit
                                                << " before disposibles test" << endmsg;

         alarm(OFF_BOTH, SAFETY_T1_FAILURE);

         break;
      }
   }

   // this is a one-time monitor, so disable it
   disable();
}

/* FORMAT HASH 51deb4eb33522239e98df2fcd1d15ae4 */

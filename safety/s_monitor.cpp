#include "s_monitor.hpp"
#include "s_exec.hpp"
#if defined(__HALLSTEST__)
    #include <iostream>
#endif

Monitor::Monitor(const char* name, bool enabled)
   : SafetyModule(),
     m_enabled(enabled),
     m_name(name),
     m_commands(SafetyExec::instance()->data().orders)
{}

/**
 * A layer of abstraction to hide our alarm method.  This
 * was originally needed during Safety refactor for IT 9082
 * until we could get back to redesigning safety
 * all together.
 *
 * RETURN - NONE
 **/
void Monitor::alarm (char cSafeStateAction, ALARM_VALUES alarm_name)
{
#if defined(__HALLSTEST__)
   std::cout << "**ALARM: STATE = " << cSafeStateAction << ", ALARM_VALUE = " << alarm_name << std::endl;
#else
   paSafetyHardwareCommandsExec->respond_to_alarm(cSafeStateAction, alarm_name);
#endif
}

DataLog_Stream& operator << (DataLog_Stream& os, SafetyPumps pump)
{
   switch (pump)
   {
      case RETURN_PUMP :
         os << " Return pump ";
         break;
      case AC_PUMP :
         os << " AC pump ";
         break;
      case INLET_PUMP :
         os << " inlet pump ";
         break;
      case PLASMA_PUMP :
         os << " plasma pump ";
         break;
      case PLATELET_PUMP :
         os << " Platelet or collect pump ";
         break;
   }
   return os;
}

/* FORMAT HASH 29bb533c46e33b3cf38ccaf6e0b96d1c */

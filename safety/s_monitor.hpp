#ifndef _MONITOR_INCLUDE
#define _MONITOR_INCLUDE

#include "alarms.h"
#include "s_names.hpp"
#include "s_module.hpp"

DataLog_Stream & operator << (DataLog_Stream& os, SafetyPumps pump);

class Monitor
   : public SafetyModule
{
#if defined(__HALLSTEST__)
   friend class HallsTest;
#endif

public:

   virtual ~Monitor(){}

   virtual void check () = 0;

   std::string name () const { return m_name; }

   /* accessors for enable switch */
   void enable () { m_enabled = true; DataLog(log_level_safe_exec_info) << "Safety monitor enabled: " << m_name << endmsg; }
   void disable () { m_enabled = false; DataLog(log_level_safe_exec_info) << "Safety monitor disabled: " << m_name << endmsg; }
   bool isEnabled () { return m_enabled; }

protected:

   Monitor(const char* name, bool enabled = true);

   void alarm (char cSafeStateAction, ALARM_VALUES alarm_name);

   bool                   m_enabled;
   std::string            m_name;

   ControlCommand_Export& m_commands;
};

#endif /* ifndef _MONITOR_INCLUDE */

/* FORMAT HASH 46e06e3355b8eee2528432ae0ac1edbe */

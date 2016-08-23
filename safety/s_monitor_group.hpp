#ifndef MONITOR_GROUP_INCLUDE
#define MONITOR_GROUP_INCLUDE

#include <map>
#include <string>

#include "s_proc_group.hpp"
#include "s_monitor.hpp"

class MonitorGroup
   : public ModuleGroup
{

public:

   MonitorGroup(Group type);
   virtual ~MonitorGroup();

   void check ();

   // NOTE: takes ownership of the pointer.  *DO NOT* call delete on pointers that have been add()ed.
   void add (Monitor* monitor);

   void addPreProcessor (Processor* processor) { m_processors.add(processor); }

   /* enable/disable managed monitor */
   void enable (const std::string& name){ if ( m_monitors[name] ) m_monitors[name]->enable(); }
   void disable (const std::string& name){ if ( m_monitors[name] ) m_monitors[name]->disable(); }

   virtual int  size () const { return m_monitors.size(); }
   virtual void RestoreFromPFR ();

protected:

   typedef std::map<std::string, Monitor*> Map_t;

   ProcessorGroup m_processors;
   Map_t          m_monitors;

private:

   void SaveToPFR ();
};

#endif

/* FORMAT HASH 71daed2ed4620b4a73c9d8063407e52b */

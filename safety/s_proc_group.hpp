#ifndef _PROC_GROUP_INCLUDE
#define _PROC_GROUP_INCLUDE

#include <list>

#include "s_mod_group.hpp"
#include "s_processor.hpp"

class ProcessorGroup
   : public ModuleGroup
{
public:

   ProcessorGroup(Group type)
      : ModuleGroup(type),
        m_processors(){}

   virtual ~ProcessorGroup(void){ m_processors.clear(); }

   SafetyModule::Status operator () (void);

   void add (Processor* processor){ m_processors.push_back(processor); }

   virtual int size () const { return m_processors.size(); }

protected:

   std::list<Processor*> m_processors;
};

#endif  // _PROC_GROUP_INCLUDE

/* FORMAT HASH 9c8dffc046a06f3c0b486b0d6520bc49 */

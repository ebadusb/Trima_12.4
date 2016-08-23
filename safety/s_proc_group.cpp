#include "s_proc_group.hpp"

SafetyModule::Status ProcessorGroup::operator () ()
{
   std::list<Processor*>::iterator processor = m_processors.begin();

   SafetyModule::Status            status    = SafetyModule::SUCCESS;

   while ( processor != m_processors.end() )
   {
      if ( (status = (**processor)()) == SafetyModule::FAILURE )
         break;

      processor++;
   }

   return status;
}

/* FORMAT HASH 5e9ff762f6cc5b16b35091dfabc844be */

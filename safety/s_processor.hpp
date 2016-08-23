#ifndef _PROCESSOR_INCLUDE
#define _PROCESSOR_INCLUDE

#include "s_module.hpp"

class Processor
   : public SafetyModule
{

public:

   virtual ~Processor(){}

   virtual SafetyModule::Status operator () (void) = 0;

protected:

   Processor(void)
      : SafetyModule() {}
};

#endif  // _PROCESSOR_INCLUDE

/* FORMAT HASH d2d224e179fe773b31078a7170913e61 */

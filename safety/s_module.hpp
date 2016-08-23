#ifndef _SAFETY_MODULE_INCLUDE
#define _SAFETY_MODULE_INCLUDE

#include <string>
#include "state_objects.hpp"

class SafetyModule
{
#if defined(__HALLSTEST__)
   friend class HallsTest;
#endif

public:

   enum Status
   {
      FAILURE,
      SUCCESS
   };

protected:

   SafetyModule();
   virtual ~SafetyModule(){}

   ControlStatus_Export& m_controlStatus;
   SafetyStatus_Export&  m_safetyStatus;
};

#endif  // _NAMED_SAFETY_MODULE_INCLUDE

/* FORMAT HASH d722427e84737b4d8ca98df83ae371ba */

#include "s_exec.hpp"

SafetyModule::SafetyModule()
   : m_controlStatus(SafetyExec::instance()->data().control),
     m_safetyStatus(SafetyExec::instance()->data().safety)
{}

/* FORMAT HASH 529c86026c03cfc03c019083593c338d */

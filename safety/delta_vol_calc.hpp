#ifndef _DELTA_VOL_CALC_HPP
#define _DELTA_VOL_CALC_HPP

#include "s_processor.hpp"
#include "state_objects.hpp"
#include "s_exec.hpp"

class DeltaVolCalculator
   : public Processor
{
public:

   DeltaVolCalculator(void);
   virtual ~DeltaVolCalculator(void) { }

   virtual SafetyModule::Status operator () (void);

private:

   float calcPumpDelta (int pump) const
   {
      return ( m_controlStatus.pumps[pump].revs * m_derived.stroke[pump] );
   }

   float calcNetRBCDelta () const;

   SafetyDerived& m_derived;
};

#endif  // _DELTA_VOL_CALC_HPP

/* FORMAT HASH 03edb77d51aa27efcfbe93fed5fad660 */

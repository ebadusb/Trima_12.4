#ifndef _STROKE_CALC_HPP
#define _STROKE_CALC_HPP

#include "s_processor.hpp"
#include "state_objects.hpp"
#include "s_set.hpp"

static const float PUMP_COEFF_A[] = {
   0.837f,
   0.00014154f,
   0.00018167f
};

static const float ALLOWABLE_PRESSURE[] = {
   -250.0f, // mmHg
   400.0f   // mmHg
};

class StrokeCalculator
   : public Processor
{
   friend class SafetyExec;

public:

   StrokeCalculator(void);
   virtual ~StrokeCalculator(void) { }

   virtual SafetyModule::Status operator () (void);

private:

   /* per pump delegates */
   float calcReturnPump ();
   float calcACPump ();
   float calcInletPump ();
   float calcPlasmaPump ();
   float calcPlateletPump ();

   // pressure constants
   enum BOUNDS { MIN = 0, MAX };

   ControlCommand_Export& m_commands;
   SafetyDerived&         m_derived;
};

#endif  // _VOLUME_CALC_HPP

/* FORMAT HASH b6cbbffafed737ff29c7bfe23172ee25 */

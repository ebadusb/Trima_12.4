#include "delta_vol_calc.hpp"

DeltaVolCalculator::DeltaVolCalculator()
   : Processor(),
     m_derived(SafetyExec::instance()->data().derived)
{}

SafetyModule::Status DeltaVolCalculator::operator () ()
{
   // get the base volume changes for pumps
   for ( int i = RETURN_PUMP; i <= PLATELET_PUMP; i++ )
      m_derived.volumes[i] = calcPumpDelta(i);

   // now, do the net change for RBC flow (estimated)
   m_derived.volumes[PSEUDO_RBC] = calcNetRBCDelta();

   return SafetyModule::SUCCESS;
}

float DeltaVolCalculator::calcNetRBCDelta () const
{
   float InletMultiplier, PlateletMultiplier, PlasmaMultiplier;

   // The pumps do not know which direction they are pumping so
   // we have to tell them by giving them a multiplier in order
   // to correctly accumulate the RBC flow
   //
   // the inlet pump is always FLOW_INTO_SET so it always adds to
   // the RBC flow
   SET_TYPE settype = SafetyExec::instance()->SetType();

   if (Set[settype].InletFlow == FLOW_INTO_BAG)
      InletMultiplier = -1.0;
   else
      InletMultiplier = 1.0;

   // the platelet pump is FLOW_INTO_BAG in most sets but it is
   // FLOW_INTO_SET for the rbc/pls set when it pumps replacement fluid.
   // the replacement fluid does not affect RBC flow
   if (Set[settype].PlateletBagFlow == FLOW_INTO_BAG)
      PlateletMultiplier = -1.0;
   else
      PlateletMultiplier = 0.0;

   // the plasma pump is FLOW_INTO_BAG in most sets but it is
   // FLOW_INTO_BAG_RC for the single-stage plt/pls/rbc set when
   // it is sometimes used to maintain flow in the collect line.
   // if the plasma valve is collecting, then it affects RBC flow
   // but if it is not collecting, then it does not affect RBC flow
   if (Set[settype].PlasmaBagFlow == FLOW_INTO_BAG)
      PlasmaMultiplier = -1.0;
   else if (Set[settype].PlasmaBagFlow == FLOW_INTO_BAG_RC)
   {
      if (m_controlStatus.valves[PLASMA_VALVE] == HW_VALVE_COLLECT)
         PlasmaMultiplier = -1.0;
      else
         PlasmaMultiplier = 0.0;
   }
   else
      PlasmaMultiplier = 0.0;

   return (InletMultiplier * m_derived.volumes[INLET_PUMP])
          + (PlateletMultiplier * m_derived.volumes[PLATELET_PUMP])
          + (PlasmaMultiplier * m_derived.volumes[PLASMA_PUMP]);
}

/* FORMAT HASH 85bc428b9462d171b7a00a899b830c68 */

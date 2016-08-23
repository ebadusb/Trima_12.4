#include "stroke_calc.hpp"
#include "s_exec.hpp"

StrokeCalculator::StrokeCalculator()
   : Processor(),
     m_commands(SafetyExec::instance()->data().orders),
     m_derived(SafetyExec::instance()->data().derived)
{}

SafetyModule::Status StrokeCalculator::operator () ()
{
   m_derived.stroke[RETURN_PUMP]   = calcReturnPump();
   m_derived.stroke[AC_PUMP]       = calcACPump();
   m_derived.stroke[INLET_PUMP]    = calcInletPump();
   m_derived.stroke[PLASMA_PUMP]   = calcPlasmaPump();
   m_derived.stroke[PLATELET_PUMP] = calcPlateletPump();

   for ( int i = RETURN_PUMP; i <= PLATELET_PUMP; i++ )
      m_derived.volumes[i] = 0.0f;

   return SafetyModule::SUCCESS;
}

float StrokeCalculator::calcReturnPump ()
{
   return 2.73713f;
}

float StrokeCalculator::calcACPump ()
{
   static const float SECOND_ORDER_CONST = -4.0E-05f;
   static const float LINEAR_CONST       = 0.0004f;
   static const float OFFSET             = 0.8199f;

   float              stroke             = 0.0f;

   //
   //
   //   The minimum AC stroke volume was calculated by using lab data at
   //   60RPM, which gave a stroke volume of 0.749.  Working the SV equation
   //   backwards gives a stroke volume of 0.755.  The stroke volume equation
   //   does not match actual data above 60RPM; at 140RPM the theoretical SV
   //   is approximately 0.31, while the measured SV is about 0.71.  This
   //   difference becomes more exaggerated as the speed increases.  Therefore
   //   we assume a minimum AC stroke volume to avoid problems in case of a
   //   bogus command or pump runaway.
   //
   static const float MINIMUM_AC_STROKE_VOLUME = 0.75f;

   float              fQ;

   fQ     = m_commands.pumpRpm[AC_PUMP] * OFFSET;

   stroke = ( SECOND_ORDER_CONST * fQ * fQ )
            + ( LINEAR_CONST * fQ ) + OFFSET;

   //
   //
   //   The stroke volume equation is validated only for low flow rates.
   //   To avoid problems with runaway pump commands limit the stroke
   //   volume at higher flows (higher flow = smaller SV).
   //
   if (stroke < MINIMUM_AC_STROKE_VOLUME )
      stroke = MINIMUM_AC_STROKE_VOLUME;

   return stroke;
}

float StrokeCalculator::calcInletPump ()
{
   static const float INLET_CONSTANT = 0.812 * 0.985;      // IT5240

   float              fQ;
   float              fP = 0.0f;

   fQ = m_commands.pumpRpm[INLET_PUMP] * INLET_CONSTANT;

   fP = calc_APS_mmhg_3_pt(m_controlStatus.accessPressure);

   // 04/18/97 msm
   // move range test for pressure correction to inlet pump sv from
   // calc_APS_mmhg ( s_glob.cpp ) to here.
   // inlet pump is only pump with pressure corrected sv and the pressure sued for
   // sv correction needs to be range tested as per a requirement identified by dave shouldice
   // during 2.2 software development.
   // range test moved here to address bug in failed donor disconnect test where if
   // the starting pressure was less than -150 mmHg, the pressure test could not be passed because
   // the range test clamps pressure on ( -250, +400 ) (MIN_ALLOWABLE_PRESSURE,MAX_ALLOWABLE_PRESSURE ).
   //
   // 05/14/97 pressure from calc_APS_mmhg_3_pt now
   //
   if ( fP > ALLOWABLE_PRESSURE[MAX] )
      fP = ALLOWABLE_PRESSURE[MAX];
   else if ( fP < ALLOWABLE_PRESSURE[MIN] )
      fP = ALLOWABLE_PRESSURE[MIN];

   return INLET_CONSTANT + fQ * PUMP_COEFF_A[1] + fP * PUMP_COEFF_A[2];
}

float StrokeCalculator::calcPlasmaPump ()
{
   static const float PLASMA_CONST_A0        = 0.794; // .837 * ( 100% - 5.15% )
   static const float PLASMA_CONST_MULT      = 1.043; // Mark Brierton factor added 1/15/97

   static const float PLASMA_PUMP_INLET_PRES = 250.0;

   float              fQ;

   fQ = m_commands.pumpRpm[PLASMA_PUMP] * PUMP_COEFF_A[0];

   return ( PLASMA_CONST_A0
            + fQ * PUMP_COEFF_A[1]
            + PLASMA_PUMP_INLET_PRES * PUMP_COEFF_A[2] )
          * PLASMA_CONST_MULT;
}

float StrokeCalculator::calcPlateletPump ()
{
   static const float PLATELET_CONST_A0        = 0.794f; // .837 * ( 100% - 5.15% )

   static const float PLATELET_PUMP_INLET_PRES = 250.0f;

   float              stroke                   = 0.0f;

   float              fQ;

   if ( SafetyExec::instance()->SetType() != SET_RBCPLS )
   {
      fQ     = m_commands.pumpRpm[PLATELET_PUMP] * (float)PUMP_COEFF_A[0];

      stroke = PLATELET_CONST_A0 + fQ * PUMP_COEFF_A[1]
               + PLATELET_PUMP_INLET_PRES * PUMP_COEFF_A[2];

   }
   else
   {
      static const float SECOND_ORDER_CONST = -4.0E-05f;
      static const float LINEAR_CONST       = 0.0004f;
      static const float OFFSET             = 0.8199f;

      // for rbc/pls sets, we have replacement fluid at the platelet pump and a filter.
      // use the same algorithm as is used on the AC pump
      static const float MINIMUM_PLATELET_STROKE_VOLUME = 0.75f;

      fQ = m_commands.pumpRpm[PLATELET_PUMP] * OFFSET;


      stroke = ( SECOND_ORDER_CONST * fQ * fQ )
               + ( LINEAR_CONST * fQ ) + OFFSET;

      //
      //
      //   The stroke volume equation is validated only for low flow rates.
      //   To avoid problems with runaway pump commands limit the stroke
      //   volume at higher flows (higher flow = smaller SV).
      //
      if (stroke < MINIMUM_PLATELET_STROKE_VOLUME )
         stroke = MINIMUM_PLATELET_STROKE_VOLUME;
   }

   return stroke;
}

/* FORMAT HASH 23978fc55a307347ab296231b58c6665 */

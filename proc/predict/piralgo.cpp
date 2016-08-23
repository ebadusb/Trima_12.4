#include "piralgo.h"
#include "procdetpltplsrbc.h"
#include "trima_datalog_levels.h"
#include "software_cds.h"
#include <cmath>

PirAlgo::PirAlgo(procedureDetailPltPlsRBC* pd)
   : IPirAlgo(),
     _FirstMaxPIRChamberFlow(_cc.FirstMaxPIRChamberFlowStandard),
     _FirstPIRFlowLimit(_cc.FirstPIRFlowLimitStandard),
     _SecondMaxPIRChamberFlow(_cc.SecondMaxPIRChamberFlowStandard),
     _pd(pd)
{
   _PIRLockin = _CollectMaximumFlow = _cc.CollectMaximumFlowStandard;
   _YtargLoss = Software_CDS::GetInstance().getFeature(PIRPreferred) ? _cc.PIRYtargLossPreferPIR : _cc.PIRYtargLossNormal;
}

// PIR Ramp calculations. Qcho -> Qc at time t=0.
// Qcho could be at the start of a single needle cycle or substate.
PirAlgoLR::PirAlgoLR(procedureDetailPltPlsRBC* pd)
   : IPirAlgo(),
     _pd(pd)
{
   _PIRLockin = _CollectMaximumFlow = _cc.CollectMaximumFlowHigh;
   _YtargLoss = _cc.PIRYtargLossNormal;
}

const float IPirAlgo::CalculatePIRQchdot (float Qc) const
{
   if (Qc >= _cc.PIRLinearRampQchLimit)
      return _cc.PIRRampRateCoefficient * Qc;
   else
      return _cc.PIRLinearRampRate;
}

const float IPirAlgo::CalculatePIRRampTime (float Qcho, float endQc) const
{
   if (Qcho >= _cc.PIRLinearRampQchLimit )
      return (float)log(endQc / Qcho) / _cc.PIRRampRateCoefficient;
   else
   {
      if (endQc > _cc.PIRLinearRampQchLimit )
         return (_cc.PIRLinearRampQchLimit  - Qcho) / _cc.PIRLinearRampRate
                + CalculatePIRRampTime(_cc.PIRLinearRampQchLimit, endQc);
      else
         return (endQc - Qcho) / _cc.PIRLinearRampRate;
   }
}

const float IPirAlgo::CalculatePIRQch (float timeInSubstate, float Qcho) const
{
   if (Qcho >= _cc.PIRLinearRampQchLimit )
      return Qcho * exp(timeInSubstate * _cc.PIRRampRateCoefficient);
   else
   {
      float linearRampTime = CalculatePIRRampTime(Qcho, _cc.PIRLinearRampQchLimit);
      if (linearRampTime > timeInSubstate)
         return Qcho + _cc.PIRLinearRampRate * timeInSubstate;
      else
         return _cc.PIRLinearRampQchLimit + CalculatePIRQch(timeInSubstate - linearRampTime, _cc.PIRLinearRampQchLimit);
   }
}

const float IPirAlgo::CalculatePIRRampVolume (float Qcho, float endQc) const
{
   if (Qcho >= _cc.PIRLinearRampQchLimit )
      return (endQc - Qcho) / _cc.PIRRampRateCoefficient;
   else
   {
      if (endQc > _cc.PIRLinearRampQchLimit )
         return (_cc.PIRLinearRampQchLimit * _cc.PIRLinearRampQchLimit - Qcho * Qcho) / _cc.PIRQcdot
                + CalculatePIRRampVolume(_cc.PIRLinearRampQchLimit, endQc);
      else
         return (endQc * endQc - Qcho * Qcho) / _cc.PIRQcdot;
   }
}

float PirAlgoLR::calcLockinBracket (const float qch)
{
   float Npsat = 1.0f / (qch / _cc.PlateletSaturationAp + _cc.PlateletSaturationBp);
   _PIRLockin = ((1.0f / ((1 - _cc.PIRPercentOffload) * Npsat)) - _cc.PlateletSaturationBp)
                * _cc.PlateletSaturationAp;
   if (_PIRLockin > _CollectMaximumFlow)
      _PIRLockin = _CollectMaximumFlow;
   return _PIRLockin;
}
//
// -----------------------------------------------------------------------------
//
float PirAlgo::calcLockinBracket (const float qch)
{

   float minQch      =  _pd->_procState.QchMin();

   float Csf         = _pd->PirCsf();
   float qchmax      =  _CollectMaximumFlow * Csf;
   float minCollFlow =  _cc.CollectMinimumFlow;
   if (minQch >= minCollFlow)
   {
      if (minQch < _cc.SecondPIRFlowLimit * Csf)
         qchmax = _SecondMaxPIRChamberFlow * Csf;

      if (minQch < _FirstPIRFlowLimit * Csf )
         qchmax = _FirstMaxPIRChamberFlow * Csf;
   }

   float neededbracket = qchmax;

   //
   //  Find new needed PIR lockin bracket
   //
   if (qch < _cc.SecondPIRFlowLimit * Csf)
      neededbracket = _SecondMaxPIRChamberFlow * Csf;

   if (qch < _FirstPIRFlowLimit * Csf)
      neededbracket = _FirstMaxPIRChamberFlow * Csf;

   if (neededbracket > qchmax)
      neededbracket = qchmax;

   _PIRLockin = neededbracket;
   return neededbracket;
}

float IPirAlgo::averagePIRQc (const IntermediateResults& currentsubstate, float& PIRVcAtMax, const float Qch)
{
   //  Set defaults assuming we have completed ramping
   float startQc = CalculatePIRQch(currentsubstate.T, Qch);
   float endQc   = CalculatePIRQch(currentsubstate.T + currentsubstate.Tcn, Qch);
   if (endQc > _PIRLockin) endQc = _PIRLockin;
   float vc      = endQc * currentsubstate.Tcn;

   if (startQc < endQc)   // not all steady state
   {
      endQc = CalculatePIRQch(currentsubstate.Tcn, startQc);
      if (endQc < _PIRLockin)   // this cycle is all ramping
      {
         vc = CalculatePIRRampVolume(startQc, endQc); // (startQc + endQc)*currentsubstate.Tcn*0.5f;
      }
      else     // it's a mix of ramping and steady state
      {
         endQc      = _PIRLockin;
         float rampTime = CalculatePIRRampTime(startQc, endQc);
         PIRVcAtMax = endQc * (currentsubstate.Tcn - rampTime);
         vc         = CalculatePIRRampVolume(startQc, endQc) // ((endQc + startQc)*0.5f)*rampTime // Qcc during ramp
                      + PIRVcAtMax;                          // Qcc after ramp
      }
   }
   else
   {
      PIRVcAtMax += vc;
   }
   return vc;
}

int PirAlgoLR::getConvCriteria (const float qch)
{
   calcLockinBracket(qch);
   return 0;
}


int PirAlgo::getConvCriteria (const float qch)
{
   int convCtr = 0;
   //
   //
   //   Recalculate PIR lockin bracket.  This is only done at intervals
   //   to allow stability and convergence.
   //
   if (_PIRConv == 0)
   {
      float oldlockin = _PIRLockin;
      float newlockin = calcLockinBracket(qch);

      //
      //
      //   Note for convergence there are two cases: where CSF is 1 and the bracket shifts; and where
      //   CSF is <1 and the shift is minor and due to changes in CSF itself.  We use 1ml to decide
      //   this (since the brackets for non-LR are more than 1ml apart).
      //
      if (fabs(newlockin - oldlockin) > 1.0)
      {
         _PIRConv = 2;
      }
   }
   //
   //   Ensure iterations for stability after PIR lockin bracket changes
   //
   if (_PIRConv > 0)
   {
      convCtr = 7;
      _PIRConv--;
   }
   return convCtr;
}

bool PirAlgo::calcPirRequired ()
{
   //
   //
   //  Normally this would be logged in the constructor but the logging gets confusing since we don't
   //  have the procedure number at construction time.  Logged here once for QA verification purposes.
   //
   if (!_YtargLossLogged)
   {
      DataLog(log_level_predict_info) << "Procedure " << _pd->getProcedureNumber() << " Ytarg loss set to " << _YtargLoss << endmsg;
      _YtargLossLogged = true;
   }

   //   PIR determination
   int   lastPIRReqd  = _PIRRequired;
   _PIRRequired = 1;
   float Vintarg      = _pd->getVintarg();
   float DYdch        = fabs(_pd->Ydch(Vintarg, 0) - _pd->Ydch(Vintarg, 1));
   // If either of the conditions are true, we do NOT do a PIR.
   bool  bQchGTLockin = (_pd->_Qch >= _PIRLockin);
   // One of the two conditions for a PIR determination: - DYdch < _cc.PIRMinFractionYtargLoss
   // should be true.
   bool bPirCond2 = (DYdch < _YtargLoss);
   //
   //  Convergence cusp fix: if we are close to the cutoff for determining PIR,
   //  and we predicted a PIR last iteration, keep it for this iteration.
   //  This is a one-sided convergence fix which preferentially prefers PIR.
   //
   if (fabs(DYdch - _YtargLoss) < 0.03)
   {
      if (lastPIRReqd) bPirCond2 = false; // keep this one if still within deadband
   }

   if (bQchGTLockin || bPirCond2)
      _PIRRequired = 0;

   //
   //
   //   PIR is required if we are already in that substate
   //
   if (_pd->InPir()) _PIRRequired = 1;

   return _PIRRequired;
}

bool PirAlgoLR::calcPirRequired ()
{
   return false; // IT13447
}

// TODO: Make this a friend of pltplsrbc. so we don't have to pass structures around.

/* FORMAT HASH 56269a0fa1b946832b371b72a0f6946d */

/*
 * TITLE:      flows.hpp
*/


#include <vxworks.h>
#include <cmath>
#include <cstdlib>

#include "flows.h"
#include "sharedstruct.h"
#include "trima_datalog_levels.h"

// Needed for predictionator
#ifndef WIN32
#include "error.h"
#endif

#ifndef __SKIPMSGSYS__
#include "an_alarm.h"
#endif

static const float MINIMUM_DC = 0.4f;
int                Calculate_Flows::_numCalculations = 0;

float upperLimit (float& x, const float limit)
{
   if (x > limit)
      x = limit;

   return x;
}


float lowerLimit (float& x, const float limit)
{
   if (x < limit)
      x = limit;

   return x;
}

#if (CPU!=SIMNT) // Static construction of this object kills the simulator.
const CobeConfig::CobeConfigStruct& Calculate_Flows::_cc = CobeConfig::data();
#endif

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

Calculate_Flows::Calculate_Flows()
   : //  My status flag.  0=success nonzero=failure
     _failure(0),
     //  These members are copies of the corresponding initialization values.
     _hct(0.0f),                 // Donor Hematocrit (Hin)
     _precount(0.0f),
     _TBV(0.0f),
     _IR(0.0f),
     _ratio(0.0f),
     _QinLimit(0.0f),
     _QrpLimit(0.0f),
     _QnLimit(0.0f),
     _DCLimit(0.0f),              // Upper Limit and Init value
     //   Precalculated values
     _oneM1Ratio(0.0f),
     _facp(0.0f),
     _facb(0.0f),
     _Vsvnr(0.0f),
     _Vres(0.0f),
     _CollectMaxFlow(0.0f) // set from cobe configs based on concentration.
    #if (CPU==SIMNT)       // Static construction of the cobeConfig object kills the simulator.
     , _cc(CobeConfig::data())
    #endif
{}

Calculate_Flows::~Calculate_Flows()
{}



void Calculate_Flows::Initialize (const float hct,
                                  const float precount,
                                  const float TBV,
                                  const float IR,
                                  const float ratio,
                                  const float QinLimit,
                                  const float QrpLimit,
                                  const float QnLimit,
                                  const float DCLimit)
{

   _hct        = hct;
   _precount   = precount;
   _TBV        = TBV / 1000.0f;
   _IR         = IR;
   _ratio      = ratio;
   _QinLimit   = QinLimit;
   _QrpLimit   = QrpLimit;
   _QnLimit    = QnLimit;
   _oneM1Ratio = (1.0f - 1.0f / ratio);
   _facp       = (1.0f / ((ratio - 1.0f) * (1.0f - hct)));
   _facb       = _facp / (1.0f + _facp);
   _DCLimit    = DCLimit;
   _Vsvnr      = _cc.RecirculateSnDrawVol;
   _Vres       = _cc.ReservoirVolume;
}


const int Calculate_Flows::calculate (struct IntermediateResults* substate)
{
#ifdef PREDICT_DEBUG
   ASSERT (++_numCalculations < 200);
#endif

   static const int MAX_ITERATIONS = 20;
   int              status         = 0;

   _failure    = 0;
   substate->D = _DCLimit;

   int iter = 0;
   for (iter = 0; iter < MAX_ITERATIONS; iter++)
   {

      status = get_QinQrp(substate->Qininstd, substate->Qrinst, substate->D);

      if (_failure || (!status))
         break;

#ifdef PREDICT_DEBUG
      ASSERT(iter < 15);
#endif
   }

   //  Note iterative code allows 0.4ml/min fudge on return flow convergence constraint.
   //  In the case where we've allowed the inaccuracy restrict Qrinst here (this will
   //  cause a corresponding inaccuracy in DC, but we won't violate flow constraints).
   //  Specific limits enforced to allow offine checks below to succeed for gross
   //  errors.  Occasionally a procedure restricted by Qn causes the max to change via
   //  this process (for example, the platelet+plasma phases where the Qnlimit is
   //  itself iterative).  Therefore additional margin is subtracted to ensure
   //  we actually get below the limit.
   //
   float maxQn  = get_QnLimit(substate->Qininstd, substate->D);
   float maxQrp = get_maxQrp(substate->Qininstd, maxQn, _QrpLimit, substate->D);

#ifdef PREDICT_DEBUG
   ASSERT (substate->Qrinst < (maxQrp + 0.5f));
   int numfix = 0;
#endif

   int fixstatus = 1;
   while (fixstatus)
   {
      fixstatus = 0;

      if (substate->Qrinst > maxQrp)
      {
         substate->Qrinst = maxQrp;
         fixstatus        = 1;
      }

      float Qn = get_Qn(substate->Qininstd, substate->Qrinst);

      if (Qn > maxQn)
      {
         substate->Qrinst -= (Qn - maxQn + 0.01f);
         fixstatus         = 1;
      }

#ifdef PREDICT_DEBUG
      ASSERT (++numfix < 10);
#endif
   }

#ifdef PREDICT_DEBUG
   float qn = get_Qn(substate->Qininstd, substate->Qrinst);
   ASSERT(qn <= (maxQn + 0.5f));

   //
   //
   //    Debug checking on flow parameters
   //
   ASSERT(iter < MAX_ITERATIONS);
   ASSERT(substate->Qininstd <= _QinLimit);

   if (!_failure)
      ASSERT(substate->Qininstd > 0.0f);

   ASSERT(substate->Qrinst <= maxQrp);

   float DC = get_DC (substate->Qininstd, substate->Qrinst);
   // ASSERT(fabs(DC - substate->D) < 0.01);

   qn = get_Qn(substate->Qininstd, substate->Qrinst);
   ASSERT(qn <= (maxQn + 0.004f));
#endif

   if (_failure)
   {
      status = _failure;
      DataLog(log_level_predict_error) << "Error " << _failure << " while attempting to calculate flows" << endmsg;
#ifndef __SKIPMSGSYS__
      anAlarmMsg badflows(FLOW_CALCULATION_ERROR);
      badflows.setAlarm();
#endif
   }

   return status;
}

const int Calculate_Flows::get_QinQrp (float& qin, float& qrp, float& DC)
{

   int status = 0;

   qin = get_Qin(DC);

   if (_failure)
   {

      return _failure;
   }

   if (qin > _QinLimit)
   {

      qin = _QinLimit;
   }

   qrp = get_Qrp(DC, qin);

   if (_failure)
   {

      return _failure;
   }

   //
   //
   //   Limit return flow to maximum (0.4f fudge used since 0.5ml is "perceptive pain threshold")
   //
   const float maxQrp = get_maxQrp(qin, get_QnLimit(qin, DC), _QrpLimit, DC);

   if (qrp > (maxQrp + 0.4f))
   {

      status = 1;
      qrp    = maxQrp;

      DC     = get_DC(qin, qrp);

      if (_failure)
      {
         return _failure;
      }
   }

   if (DC > _DCLimit)
   {
      _failure = 25;
      return _failure;

   }

   //
   //
   //   If our duty cycle calcs bombed (or ended up below the minimum DC)
   //   then we also need to lower the inlet flow.
   //
   if (DC < (MINIMUM_DC - 0.001f))
   {
      DC     = MINIMUM_DC;
      status = 0;

      qin    = get_Qin(DC, qrp);

      if (_failure)
      {
         return _failure;
      }

      upperLimit(qin, _QinLimit);

      //
      //
      //  If the needle flow is greater than our maximum it means that
      //  we could not meet our needle flow limits using the minimum
      //  duty cycle.  In this case we need to calculate the optimum
      //  Qin and Qrp based upon the DC and needle flow.
      //
      if (get_Qn(qin, qrp) > (get_QnLimit(qin, DC) + 0.4f))
      {
         get_Qin(qin, DC, get_QnLimit(qin, DC));

         qrp = get_maxQrp(qin, get_QnLimit(qin, DC), _QrpLimit, DC);

         if (_failure)
         {
            return _failure;
         }
      }
   }

   return status;
}



//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//         No collect flows assume 100% recirculation
//
Calc_No_collection::Calc_No_collection()
   : Calculate_Flows()
{}

Calc_No_collection::~Calc_No_collection()
{}

void Calc_No_collection::Initialize (const float hct,
                                     const float precount,
                                     const float TBV,
                                     const float IR,
                                     const float ratio,
                                     const float QinLimit,
                                     const float QrpLimit,
                                     const float QnLimit)
{

   Calculate_Flows::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, _cc.DutyCycleLimit);
}

// ----------------------------------------------------------------------------
//
const int Calc_No_collection::calculate (struct IntermediateResults* substate)
{
   int status = Calculate_Flows::calculate(substate);
   substate->Qininstr = substate->Qininstd;
   //  purposely do not set Qp/Qc so it can be overridden at will
   substate->Qnret    = get_Qn(substate->Qininstd, substate->Qrinst);
   substate->Qacinst  = substate->Qininstd / _ratio;
   substate->Vsvn     = _Vres;
   substate->Tr       = _Vres / substate->Qnret;
   substate->Td       = _Vres / substate->Qininstd;
   substate->Tcn      = substate->Tr + substate->Td;
   substate->Facb     = _facb;
   substate->Facp     = _facp;
   substate->Csf      = 1.0f; // By definition
   return status;

}

// ----------------------------------------------------------------------------
//
float Calc_No_collection::get_Qin (float DC)
{
   float dcf = 1.0f - _Vsvnr / _Vres;
   float qin = (_IR * _TBV * _ratio / (DC * dcf));

   if (qin <= 0.0f)
   {
      DataLog(log_level_predict_error) << "Error. "
                                       << ", _Vsvnr= " << _Vsvnr
                                       << ", _Vres= " << _Vres
                                       << ", _IR=" << _IR
                                       << ", _TBV=" << _TBV
                                       << ", _ratio=" << _ratio
                                       << ", DC= " << DC
                                       << ", dcf= " << dcf
                                       << endmsg;

      _failure = 5;
      return 0.0f;
   }

   return qin;
}

//
// ----------------------------------------------------------------------------
//
float Calc_No_collection::get_Qin (float DC, float Qrp)
{
   float qin = Qrp * (1.0f - DC);
   if (qin <= 0.0f)
   {
      _failure = 6;
      return 0.0f;
   }
   return qin;
}
//
// ----------------------------------------------------------------------------
//
float Calc_No_collection::get_Qrp (float DC, float Qin)
{
   if ((DC < (MINIMUM_DC - 0.01f)) || (DC > (_DCLimit + 0.01f)))
   {
      _failure = 4;
      return 0.0f;
   }

   return (Qin / (1.0f - DC));
}

//
// ----------------------------------------------------------------------------
float Calc_No_collection::get_DC (const float Qin, const float Qrp)
{
   float DC = 1.0f - Qin / Qrp;
   return DC;
}

//
// ----------------------------------------------------------------------------
//
void Calc_No_collection::get_Qin (float& Qin, const float DC, const float Qneedle)
{
   Qin = Qneedle * (1.0f / DC - 1.0f);
   if (Qin <= 0.0f)
   {
      _failure = 8;
      Qin      = 0.0f;
   }
}
//
// ----------------------------------------------------------------------------
//
const float Calc_No_collection::get_Qn (const float Qin, const float Qrp)
{
   return (Qrp - Qin); // generic 100% recirculation equation
}

//
// ----------------------------------------------------------------------------
//
const float Calc_No_collection::get_maxQrp (const float Qin,
                                            const float Qneedle,
                                            const float QrpLimit,
                                            const float)
{
   float max = Qin + Qneedle;
   upperLimit(max, QrpLimit);
   return max;
}

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//         RBC/Plasma set RBC Setup Phase. No collect flows assume 0% recirculation
//
Calc_Rbcpl_RBC_Setup::Calc_Rbcpl_RBC_Setup()
   : Calculate_Flows()
{}

Calc_Rbcpl_RBC_Setup::~Calc_Rbcpl_RBC_Setup()
{}

void Calc_Rbcpl_RBC_Setup::Initialize (const float hct,
                                       const float precount,
                                       const float TBV,
                                       const float IR,
                                       const float ratio,
                                       const float QinLimit,
                                       const float QrpLimit,
                                       const float QnLimit,
                                       const float Hrbc)
{
   _hrbc = Hrbc;

   Calculate_Flows::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, _cc.RBCDutyCycleLimit);
}

// ----------------------------------------------------------------------------
//
const int Calc_Rbcpl_RBC_Setup::calculate (struct IntermediateResults* substate)
{
   int status = Calculate_Flows::calculate(substate);

   substate->Qininstr = 0.0f;  // No Recirc

   substate->Qcinstd  = 0.0f;  // No replacement fluid
   substate->Qcinstr  = 0.0f;
   substate->Qrep     = 0.0f;

   substate->Qpinstd  = get_Qp(substate->Qininstd);

   if (substate->Qpinstd < 0)
   {
      substate->Qpinstd = 0;
   }

   substate->Qpinstr = 0.0f;  // ?

   substate->Qnret   = substate->Qrinst; // Qn = Qr ?
   substate->Tr      = _cc.ReservoirVolume / substate->Qrinst;
   substate->Td      = _cc.ReservoirVolume / substate->Qininstd;

   // substate->Vsvn = _cc.ReservoirVolume/(1.0f-(substate->Qininstd-substate->Qpinstd-substate->Qcinstd)/substate->Qininstd);
   // substate->Td = _cc.ReservoirVolume/(substate->Qpinstd+substate->Qcinstd);

   substate->Qacinst = substate->Qininstd / _ratio;
   substate->Vsvn    = _cc.ReservoirVolume;

   substate->Tcn     = substate->Tr + substate->Td;
   substate->Facb    = _facb;
   substate->Facp    = _facp;

   return status;
}

// ----------------------------------------------------------------------------
//
float Calc_Rbcpl_RBC_Setup::get_Qin (float DC)
{
   const float DCF = 1.0f - _cc.RecirculateSnDrawVol / _cc.ReservoirVolume;

   float       qin = (_IR * _TBV * _ratio / (DC * DCF));

   if (qin <= 0.0f)
   {
      DataLog(log_level_predict_error) << "Error. "
                                       << ", _IR=" << _IR
                                       << ", _TBV=" << _TBV
                                       << ", _ratio=" << _ratio
                                       << ", DC= " << DC
                                       << ", dcf= " << DCF
                                       << endmsg;
      _failure = 20;
      return 0.0f;
   }

   return qin;
}

//
// ----------------------------------------------------------------------------
//
float Calc_Rbcpl_RBC_Setup::get_Qin (float DC, float Qrp)
{
   const float DCF = 1.0f - (_cc.RecirculateSnDrawVol / _cc.ReservoirVolume);

   return (Qrp * (1.0f - DC) ) / (DC * DCF);
}

//
// ----------------------------------------------------------------------------
//
void Calc_Rbcpl_RBC_Setup::get_Qin (float& Qin, const float DC, const float Qneedle)
{
   Qin = get_Qin(DC, Qneedle);
}

//
// ----------------------------------------------------------------------------
//
float Calc_Rbcpl_RBC_Setup::get_Qrp (float DC, float Qin)
{
   const float DCF = 1.0f - (_cc.RecirculateSnDrawVol / _cc.ReservoirVolume);

   // IT 8242 reduce aps on returns in setup
   float Qrp = _cc.PtfSetupReturnPumpMultiple * (Qin * DC * DCF / (1.0f - DC));

   // IT 9227 second time.  pin the flow to this max
   if (_cc.PtfSetupReturnMax < Qrp)
      Qrp = _cc.PtfSetupReturnMax;

   return Qrp;

}

//
// ----------------------------------------------------------------------------
//
float Calc_Rbcpl_RBC_Setup::get_DC (const float Qin, const float Qrp)
{
   const float DCF = 1.0f - (_cc.RecirculateSnDrawVol / _cc.ReservoirVolume);

   return Qrp / (Qrp + (Qin * DCF));
}

//
// ----------------------------------------------------------------------------
//
const float Calc_Rbcpl_RBC_Setup::get_Qn (const float, const float Qrp)
{
   return Qrp;
}

//
// ----------------------------------------------------------------------------
const float Calc_Rbcpl_RBC_Setup::get_maxQrp (const float,
                                              const float Qneedle,
                                              const float QrpLimit,
                                              const float)
{
   return (Qneedle > QrpLimit) ? QrpLimit : Qneedle;
}

// RKM
// ----------------------------------------------------------------------------
//
// From 5.1 RBC / Plasma procedures (3.1.3-2)
//
// ----------------------------------------------------------------------------
float Calc_Rbcpl_RBC_Setup::get_Qp (const float Qin)
{
   const float Hin = _hct * _oneM1Ratio;

   float       Qp  = Qin * ( 1.0f - Hin / _hrbc );

   if (Qp < 0.2f)
   {
      Qp = 0.0f;
   }

   return Qp;
}

#if 0
//
// ----------------------------------------------------------------------------
float Calc_Rbcpl_RBC_Setup::get_Qrbc (const float Qin)
{
   return Qin - get_Qp(Qin);
}
#endif

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//         RBC/Plasma set - plasma collection
//
Calc_Rbcpl_plasma_collection::Calc_Rbcpl_plasma_collection()
   : Calculate_Flows()
{}

Calc_Rbcpl_plasma_collection::~Calc_Rbcpl_plasma_collection()
{}

void Calc_Rbcpl_plasma_collection::Initialize (const float hct,
                                               const float precount,
                                               const float TBV,
                                               const float IR,
                                               const float ratio,
                                               const float QinLimit,
                                               const float QrpLimit,
                                               const float QnLimit,
                                               const int replacementRequired,
                                               const float replacementPercent)
{
   _replacementRequired = replacementRequired;
   _replacementPercent  = replacementPercent;
   Calculate_Flows::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, _cc.DutyCycleLimit);
}

// ----------------------------------------------------------------------------
//
const int Calc_Rbcpl_plasma_collection::calculate (struct IntermediateResults* substate)
{
   int status = Calculate_Flows::calculate(substate);
   substate->Qininstr = 0.0f;
   substate->Qnret    = substate->Qrinst;
   substate->Qacinst  = substate->Qininstd / _ratio;
   substate->Qpinstd  = get_Qp(substate->Qininstd);
   substate->Qpinstr  = 0.0f;
   substate->Qcinstd  = get_Qrep(substate->Qininstd);
   substate->Qcinstr  = 0.0f;
   substate->Vsvn     = _Vres / (1.0f - (substate->Qpinstd - substate->Qcinstd) / substate->Qininstd);
   substate->Tr       = _Vres / substate->Qrinst;
   substate->Td       = _Vres / (substate->Qininstd - substate->Qpinstd + substate->Qcinstd);
   substate->Tcn      = substate->Tr + substate->Td;
   substate->Facb     = _facb;
   substate->Facp     = _facp;
   return status;

}

// ----------------------------------------------------------------------------
//
float Calc_Rbcpl_plasma_collection::get_Qin (float DC)
{
   float Hin   = _hct * _oneM1Ratio;
   float H     = 1.0f - Hin / _cc.MaxRbcPlasmaHrbc;
   float denom = 1.0f - _facb * _ratio * H;

   //
   //
   //   As Vsvn is a function of Qin, and vice-versa, we iterate.  In practice this
   //   is usually only one iteration.
   //
   float Vsvn     = 65.0f;
   float lastVsvn = 0.0f;
   float qin      = 0.0f;

   while (fabs(Vsvn - lastVsvn) > 0.1)
   {
      lastVsvn = Vsvn;
      float dcf = 1.0f - _Vsvnr / Vsvn;
      qin      = (_IR * _ratio * _TBV) / (DC * denom * dcf);
      Vsvn     = _Vres / (1.0f - (get_Qp(qin) - get_Qrep(qin)) / qin);
   }

   if (qin < 0.0f)
   {
      _failure = 11;
      qin      = 0.0f;
   }

   return qin;
}

//
// ----------------------------------------------------------------------------
//
float Calc_Rbcpl_plasma_collection::get_Qin (float DC, float Qrp)
{
   /// This is V4
   float qin;
   if (_replacementRequired)
   {
      qin = Qrp * (1.0f / DC - 1.0f) /
            ((_replacementPercent - 1.0f) * (1.0f - _hct / _cc.MaxRbcPlasmaHrbc * _oneM1Ratio) +
             (1.0f - _replacementPercent / _ratio));
   }
   else
   {
      qin = Qrp * (1.0f / DC - 1.0f) / (_hct / _cc.MaxRbcPlasmaHrbc * _oneM1Ratio);
   }
   if (qin < 0.0f)
   {
      _failure = 12;
      qin      = 0.0f;
   }
   return qin;
}

//
// ----------------------------------------------------------------------------
//
float Calc_Rbcpl_plasma_collection::get_Qrp (float DC, float Qin)
{
   float Td = _Vres / (Qin - get_Qp(Qin) + get_Qrep(Qin));
   return (DC * _Vres) / (Td - Td * DC);
}

//
// ----------------------------------------------------------------------------
float Calc_Rbcpl_plasma_collection::get_DC (const float Qin, const float Qrp)
{
   float Tr = _Vres / Qrp;
   float Td = _Vres / (Qin - get_Qp(Qin) + get_Qrep(Qin));
   return 1.0f - Tr / (Tr + Td);
}

//
// ----------------------------------------------------------------------------
void Calc_Rbcpl_plasma_collection::get_Qin (float& Qin, const float DC, const float Qneedle)
{
   Qin = get_Qin(DC, Qneedle); // Qn = Qrp for this phase
   if (Qin < 0.0f)
   {
      _failure = 14;
      Qin      = 0.0f;
   }
}

//
// ----------------------------------------------------------------------------
float Calc_Rbcpl_plasma_collection::get_Qrep (const float Qin)
{
   return (_replacementRequired ? (_replacementPercent * (get_Qp(Qin) - Qin / _ratio)) : 0.0f);
}

//
// ----------------------------------------------------------------------------
float Calc_Rbcpl_plasma_collection::get_Qp (const float Qin)
{
   return Qin * (1.0f - (_hct * _oneM1Ratio) / _cc.MaxRbcPlasmaHrbc);
}
//
// ----------------------------------------------------------------------------
const float Calc_Rbcpl_plasma_collection::get_maxQrp (const float qin,
                                                      const float Qneedle,
                                                      const float QrpLIMIT,
                                                      const float DC)
{
   float max = Qneedle;
   upperLimit(max, QrpLIMIT);
   upperLimit(max, get_QnLimit(qin, DC));
   return max;
}

//
// ----------------------------------------------------------------------------
//
const float Calc_Rbcpl_plasma_collection::get_Qn (const float, const float Qrp)
{
   return Qrp;
}
//
// ----------------------------------------------------------------------------
//
const float Calc_Rbcpl_plasma_collection::get_QnLimit (const float Qin, const float /*DC*/)
{
   //
   //
   //  This is the fix for IT5782.
   //
   float flowIntoRes = Qin - get_Qp(Qin);
   float rbcsIntoRes = flowIntoRes * _cc.MaxRbcPlasmaHrbc;
   float hctRes      = 1.0f - rbcsIntoRes / (flowIntoRes + get_Qrep(Qin));
   float temp        = 0.91f * _cc.QneedleLimitMax * hctRes;

   return ((temp < _QnLimit) ? temp : _QnLimit);
}

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//         RBC/Plasma set - rbc collection
//
Calc_Rbcpl_rbc_collection::Calc_Rbcpl_rbc_collection()
   : Calculate_Flows(),
     _DCLimit(_cc.DutyCycleLimit) // Default DC Limit.  Overridded by PTF.
{}

Calc_Rbcpl_rbc_collection::~Calc_Rbcpl_rbc_collection()
{}

void Calc_Rbcpl_rbc_collection::Initialize (const float hct,
                                            const float precount,
                                            const float TBV,
                                            const float IR,
                                            const float ratio,
                                            const float QinLimit,
                                            const float QrpLimit,
                                            const float QnLimit,
                                            const bool replacementRequired,
                                            const float replacementPercent,
                                            const float collectionCrit,
                                            const float,
                                            const float)   // Not used here.
{
   _replacementRequired = replacementRequired;
   _replacementPercent  = replacementPercent;
   _collectionCrit      = collectionCrit;

   Calculate_Flows::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, _DCLimit);
}

// ----------------------------------------------------------------------------
//
const int Calc_Rbcpl_rbc_collection::calculate (struct IntermediateResults* substate)
{
   int status = Calculate_Flows::calculate(substate);
   substate->Qininstr = 0.0f;
   substate->Qnret    = substate->Qrinst;
   substate->Qacinst  = substate->Qininstd / _ratio;
   substate->Qpinstd  = get_Qp(substate->Qininstd);
   substate->Qpinstr  = 0.0f;
   substate->Qcinstd  = get_Qrep(substate->Qininstd);
   substate->Qcinstr  = 0.0f;
   substate->Vsvn     = _Vres / (1.0f - (substate->Qininstd - substate->Qpinstd - substate->Qcinstd) / substate->Qininstd);
   substate->Tr       = _Vres / substate->Qrinst;
   substate->Td       = _Vres / (substate->Qpinstd + substate->Qcinstd);
   substate->Tcn      = substate->Tr + substate->Td;
   substate->Facb     = _facb;
   substate->Facp     = _facp;

   return status;
}

// ----------------------------------------------------------------------------
//
float Calc_Rbcpl_rbc_collection::get_Qin (float DC)
{
   //
   //
   //  Start by calling Qp to get Vsvn and dcf.  Since Qp and Qin are proportional,
   //  Vsvn doesn't change as a function of Qin.
   //
   get_Qp(50.0f); // can be any "valid" flow rate

   float denom = (1.0f - (_hct * _oneM1Ratio) * (1.0f / _collectionCrit - 1.0f) * _facb * _ratio);
   float Qin   = _IR * _ratio * _TBV / (DC * denom * _dcf);

   if (Qin < 0.0f)
   {
      _failure = 15;
      Qin      = 0.0f;
   }

   return Qin;
}

//
// ----------------------------------------------------------------------------
//
float Calc_Rbcpl_rbc_collection::get_Qin (float DC, float Qrp)
{
   //
   //
   //  Start by calling Qp to get Vsvn and dcf.  Since Qp and Qin are proportional,
   //  Vsvn doesn't change as a function of Qin.
   //
   get_Qp(50.0f); // can be any "valid" flow rate

   float Hin = _hct * _oneM1Ratio;
   float Tr  = _Vres / Qrp;
   float qin = 0.0f;

   if (_replacementRequired)
   {
      qin = (_Vres * (1.0f - DC)) /
            ((Tr - Tr * (1.0f - DC)) * (1.0f - _replacementPercent / _ratio + (Hin / _collectionCrit / _dcf) * (_replacementPercent - 1.0f)));
   }
   else
   {
      qin = ((1.0f - DC) * _Vres) /
            ((Tr - Tr * (1.0f - DC)) * (1.0f - (Hin / _collectionCrit / _dcf)));
   }

   if (qin < 0.0f)
   {
      _failure = 16;
      qin      = 0.0f;
   }

   return qin;
}


//
// ----------------------------------------------------------------------------
//
float Calc_Rbcpl_rbc_collection::get_Qrp (float DC, float Qin)
{
   float Td = _Vres / (get_Qp(Qin) + get_Qrep(Qin));
   return (DC * _Vres) / (Td - Td * DC);
}

//
// ----------------------------------------------------------------------------
float Calc_Rbcpl_rbc_collection::get_DC (const float Qin, const float Qrp)
{
   float Tr = _Vres / Qrp;
   float Td = _Vres / (get_Qp(Qin) + get_Qrep(Qin));
   return 1.0f - Tr / (Tr + Td);
}

//
// ----------------------------------------------------------------------------
void Calc_Rbcpl_rbc_collection::get_Qin (float& Qin, const float DC, const float Qneedle)
{
   Qin = get_Qin(DC, Qneedle); // Qn = Qrp for this phase
}

//
// ----------------------------------------------------------------------------
float Calc_Rbcpl_rbc_collection::get_Qrep (const float Qin)
{
   return (_replacementRequired ? (_replacementPercent * (get_Qrbc(Qin) - Qin / _ratio)) : 0.0f);
}

//
// ----------------------------------------------------------------------------
float Calc_Rbcpl_rbc_collection::get_Qp (const float Qin)
{
   float Qp     = 20.0f;
   float lastQp = 0.0f;

   while (fabs(Qp - lastQp) > 0.01)  // Force convergence at resolution
   {
      lastQp = Qp;
      float qcinstd = (_replacementRequired ? (_replacementPercent * (Qin - Qp - Qin / _ratio)) : 0.0f);
      _Vsvn  = Qin * _Vres / (Qp + qcinstd);
      _dcf   = 1.0f - _Vsvnr / _Vsvn;
      Qp     = Qin * (1.0f - _hct * _oneM1Ratio * _dcf / _collectionCrit);
   }
   return Qp;
}
//
// ----------------------------------------------------------------------------
float Calc_Rbcpl_rbc_collection::get_Qrbc (const float Qin)
{
   return Qin - get_Qp(Qin);
}
//
// ----------------------------------------------------------------------------
const float Calc_Rbcpl_rbc_collection::get_maxQrp (const float qin,
                                                   const float Qneedle,
                                                   const float QrpLIMIT,
                                                   const float DC)
{
   float max = Qneedle;
   upperLimit(max, QrpLIMIT);
   upperLimit(max, get_QnLimit(qin, DC));
   return max;
}

//
// ----------------------------------------------------------------------------
//
const float Calc_Rbcpl_rbc_collection::get_Qn (const float, const float Qrp)
{
   return Qrp;
}
//
// ----------------------------------------------------------------------------
//
const float Calc_Rbcpl_rbc_collection::get_QnLimit (const float /*Qin*/, const float /*DC*/)
{
   float temp = 0.91f * _cc.QneedleLimitMax;
   return ((temp < _QnLimit) ? temp : _QnLimit);
}



//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//         RBC/Plasma PTF set - rbc collection
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
Calc_Rbcpl_ptf_rbc_collection::Calc_Rbcpl_ptf_rbc_collection()
   : Calc_Rbcpl_rbc_collection()
{}

Calc_Rbcpl_ptf_rbc_collection::~Calc_Rbcpl_ptf_rbc_collection()
{}


void Calc_Rbcpl_ptf_rbc_collection::Initialize (const float hct,
                                                const float precount,
                                                const float TBV,
                                                const float IR,
                                                const float ratio,
                                                const float QinLimit,
                                                const float QrpLimit,
                                                const float QnLimit,
                                                const bool replacementRequired,
                                                const float replacementPercent,
                                                const float collectionCrit,
                                                const float qrbcMax,
                                                const float HCT_Delta)
{


   _qrbcmax  = qrbcMax;
   _DCLimit  = _cc.RBCDutyCycleLimit;  // Override DC limit for PTF.
   _hctDelta = HCT_Delta;


   Calc_Rbcpl_rbc_collection::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, replacementRequired, replacementPercent, collectionCrit, _qrbcmax, HCT_Delta);
}

//
// Use the base class function and just limit Qin
//
float Calc_Rbcpl_ptf_rbc_collection::get_Qin (float DC)
{
   const float Qin        = Calc_Rbcpl_rbc_collection::get_Qin(DC);

   const float loweredQin = limit_Qin(Qin);

   return loweredQin;
}



//
// Use the base class function and just limit Qin
//
float Calc_Rbcpl_ptf_rbc_collection::get_Qin (float DC, float Qrp)
{
   const float Qin        = Calc_Rbcpl_rbc_collection::get_Qin(DC, Qrp);

   const float loweredQin = limit_Qin(Qin);

   return loweredQin;

}



//
// Use the base class function and just limit Qin
//
void Calc_Rbcpl_ptf_rbc_collection::get_Qin (float& Qin, const float DC, const float Qneedle)
{

   Qin = Calc_Rbcpl_rbc_collection::get_Qin(DC, Qneedle);

   const float loweredQin = limit_Qin(Qin);

   Qin = loweredQin;
}



float Calc_Rbcpl_ptf_rbc_collection::limit_Qin (const float Qin)
{

   float Qrbc       = Qin - get_Qp(Qin);
   float loweredQin = Qin;

   while (Qrbc > _qrbcmax)
   {
      loweredQin -= 0.5f;
      Qrbc        = loweredQin - get_Qp(loweredQin);
   }

   return loweredQin ;
}


float Calc_Rbcpl_ptf_rbc_collection::get_Qp (const float Qin)
{

   float Qp     = 20.0f;
   float lastQp = 0.0f;

   while (fabs(Qp - lastQp) > 0.01)  // Force convergence at resolution
   {
      lastQp = Qp;
      float qcinstd = (_replacementRequired ? (_replacementPercent * (Qin - Qp - Qin / _ratio)) : 0.0f);
      _Vsvn  = Qin * _Vres / (Qp + qcinstd);
      _dcf   = 1.0f - _Vsvnr / _Vsvn;
      Qp     = Qin * (1.0f - _hct * _oneM1Ratio * _dcf / (_collectionCrit + _hctDelta));
   }
   return Qp;



}

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
Calc_Channel_Setup::Calc_Channel_Setup()
   : Calculate_Flows()
{}

Calc_Channel_Setup::~Calc_Channel_Setup()
{}

void Calc_Channel_Setup::Initialize (const float hct,
                                     const float precount,
                                     const float TBV,
                                     const float IR,
                                     const float ratio,
                                     const float QinLimit,
                                     const float QrpLimit,
                                     const float QnLimit,
                                     const float Qch)
{
   _Qch = Qch;

   //
   //
   //   Instantaneous Qp / Qc can be computed now
   //
   if (_Qch < GetCollectMaxFlow())
   {
      _Qcinstd = _Qch;
      _Qpinstd = 0.0f;
   }
   else
   {
      _Qpinstd = GetCollectMaxFlow();
      _Qcinstd = 2.0f * GetCollectMaxFlow();
   }

   Calculate_Flows::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, _cc.DutyCycleLimit);
}

// ----------------------------------------------------------------------------
//
const int Calc_Channel_Setup::calculate (struct IntermediateResults* substate)
{
   //
   //
   //   Instantaneous Qp / Qc are fixed by input Qch
   //
   substate->Qpinstd = _Qpinstd;
   substate->Qcinstd = _Qcinstd;
   substate->Qpinstr = substate->Qpinstd;
   substate->Qcinstr = substate->Qcinstd;

   //
   //
   //   Perform Qin / Qrp calculations
   //
   int status = Calculate_Flows::calculate(substate);

   substate->Qininstr = get_Qinr(substate->Qininstd);
   substate->Qnret    = get_Qn(substate->Qininstd, substate->Qrinst);
   substate->Csf      = MAX(MIN(substate->Qininstd / _cc.CsfQin, _cc.CsfMax), _cc.CsfMin);
   substate->Qacinst  = substate->Qininstd / _ratio;
   substate->Vsvn     = _Vres;
   substate->Tr       = _Vres / substate->Qnret;
   substate->Td       = _Vres / substate->Qininstd;
   substate->Tcn      = substate->Tr + substate->Td;
   substate->Facb     = _facb;
   substate->Facp     = _facp;

   return status;

}

// ----------------------------------------------------------------------------
//
float Calc_Channel_Setup::get_Qin (float DC)
{
   float dcf = 1.0f - _Vsvnr / _Vres;
   float qin = (_IR * _TBV * _ratio / (DC * dcf));
   return qin;
}

//
// ----------------------------------------------------------------------------
//
float Calc_Channel_Setup::get_Qin (float DC, float Qrp)
{
   float       Qin;

   const float f = (_Vres * (1.0f - DC)) / DC;


   Qin = (f * (Qrp - _Qpinstd - _Qch )) / (_Vres + f * _cc.MinRecirculatePlateletCollect);

   //
   //
   //  Ensure we did not violate recirculation maximums
   //
   float cr = get_Qinr(Qin) / Qin;

   if (cr > _cc.MaxRecirculatePlateletCollect)
   {
      Qin = (f * Qrp) / (_Vres + f * _cc.MaxRecirculatePlateletCollect);
   }

   // Check Qinr against the min
   const float qinr    = get_Qinr(Qin);
   const float qinrmin = (_cc.MinRecirculatePlateletCollect * Qin + _Qpinstd + _Qch );

   if (qinr != qinrmin)
   {
      Qin = (f * Qrp) / (_Vres + f * _cc.MaxRecirculatePlateletCollect);
   }

   return Qin;
}



//
// ----------------------------------------------------------------------------
//
float Calc_Channel_Setup::get_Qrp (float DC, float Qin)
{
   float Td = _Vres / Qin;
   return (_Vres * DC) / (Td * (1.0f - DC)) + get_Qinr(Qin);
}

//
// ----------------------------------------------------------------------------
float Calc_Channel_Setup::get_DC (const float Qin, const float Qrp)
{
   float Tr = _Vres / (Qrp - get_Qinr(Qin));
   float Td = _Vres / Qin;
   return 1.0f - Tr / (Tr + Td);
}

//
// ----------------------------------------------------------------------------
//
void Calc_Channel_Setup::get_Qin (float& Qin, const float DC, const float Qneedle)
{
   Qin = (((_Vres * (1.0f - DC)) / DC) * Qneedle) / _Vres;
}

//
// ----------------------------------------------------------------------------
//
const float Calc_Channel_Setup::get_Qn (const float Qin, const float Qrp)
{
   return (Qrp - get_Qinr(Qin));
}
//
// ----------------------------------------------------------------------------
//
const float Calc_Channel_Setup::get_maxQrp (const float Qin,
                                            const float Qneedle,
                                            const float QrpLimit,
                                            const float)
{
   float max = get_Qinr(Qin) + Qneedle;
   upperLimit (max, QrpLimit);

   return max;
}

//
// ----------------------------------------------------------------------------
//
const float Calc_Channel_Setup::get_Qinr (const float Qin)
{
   float Qinr;

   Qinr = _cc.MinRecirculatePlateletCollect * Qin + _Qpinstd + _Qch;
   const float max = Qin * _cc.MaxRecirculatePlateletCollect;
   upperLimit(Qinr, max);

   return Qinr;
}


//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
Calc_Preplatelet_Plasma::Calc_Preplatelet_Plasma()
   : Calculate_Flows()
{}

Calc_Preplatelet_Plasma::~Calc_Preplatelet_Plasma()
{}

void Calc_Preplatelet_Plasma::Initialize (const float hct,
                                          const float precount,
                                          const float TBV,
                                          const float IR,
                                          const float ratio,
                                          const float QinLimit,
                                          const float QrpLimit,
                                          const float QnLimit,
                                          const float Qch)
{
   Calculate_Flows::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, _cc.DutyCycleLimit);
   _Qch       = Qch;
   _generalfc = (1.0f - (_hct * _oneM1Ratio) / _cc.MaxPlateletPlasmaHrbc);
}

// ----------------------------------------------------------------------------
//
const int Calc_Preplatelet_Plasma::calculate (struct IntermediateResults* substate)
{
   int status = Calculate_Flows::calculate(substate);

   substate->Qininstr = get_Qinr(substate->Qininstd, substate->D);
   substate->Qpinstd  = get_Qp(substate->Qininstd, substate->D);
   substate->Qcinstd  = _Qch; // get_Qch(substate->Qininstd);
   substate->Qpinstr  = substate->Qpinstd;
   substate->Qcinstr  = substate->Qcinstd;
   substate->Qnret    = get_Qn(substate->Qininstd, substate->Qrinst);
   substate->Csf      = MAX(MIN(substate->Qininstd / _cc.CsfQin, _cc.CsfMax), _cc.CsfMin);
   substate->Qacinst  = substate->Qininstd / _ratio;
   substate->Vsvn     = substate->Qininstd * _Vres / (substate->Qininstd - substate->Qpinstd);
   substate->Tr       = _Vres / (substate->Qrinst - substate->Qininstr + substate->Qpinstr);
   substate->Td       = _Vres / (substate->Qininstd - substate->Qpinstr);
   substate->Tcn      = substate->Tr + substate->Td;
   substate->Facb     = _facb;
   substate->Facp     = _facp;

   return status;

}

// ----------------------------------------------------------------------------
//
float Calc_Preplatelet_Plasma::get_Qin (float DC)
{
   const float A       = (1.0f / (1.0f - DC) + _cc.MinRecirculatePlateletCollect / DC) / 2.0f;

   float       Qin     = 100.0f;
   float       lastQin = 0.0f;


   while (fabs(Qin - lastQin)>0.01)
   {
      lastQin = Qin;

      float       Qp   = get_Qp(Qin, DC);

      const float Vsvn = Qin * _Vres / (Qin - Qp);
      const float dcf  = 1.0f - _Vsvnr / Vsvn;

#ifdef PREDICT_DEBUG
      ASSERT(A * A + dcf * _generalfc / (1.0f - DC) > 0.0);
#endif

      const float fcmax = (float)(sqrt(A * A + dcf * _generalfc / (1.0f - DC)) - A) / dcf;

      Qin = (_IR * _ratio * _TBV - _facb * _ratio * get_Qch(Qin)) / (1.0f - _facb * _ratio * fcmax) / DC / dcf;
   }

   return Qin;
}

//
// ----------------------------------------------------------------------------
//
float Calc_Preplatelet_Plasma::get_Qin (float DC, float Qrp)
{
   float Qin;

   Qin = 100.0f;
   float lastQin = 0.0f;

   while (fabs(Qin - lastQin)>0.01)
   {
      lastQin = Qin;
      const float Qp = get_Qp(Qin, DC);
      const float Tr = _Vres / (Qrp - get_Qinr(Qin, DC) + Qp);

      Qin = 0.8f * ((_Vres * (1.0f - DC)) / (DC * Tr) + Qp) + 0.2f * lastQin;
   }

   return Qin;
}


//
// ----------------------------------------------------------------------------
//
float Calc_Preplatelet_Plasma::get_Qrp (float DC, float Qin)
{
   float Qp  = get_Qp(Qin, DC);
   float Td  = _Vres / (Qin - Qp);
   float Qrp = (_Vres * DC) / (Td * (1.0f - DC)) + get_Qinr(Qin, DC) - Qp;

   return Qrp;
}

//
// ----------------------------------------------------------------------------
float Calc_Preplatelet_Plasma::get_DC (const float Qin, const float Qrp)
{
   float DC     = _cc.DutyCycleLimit;
   float lastDC = 0.0f;

   while (fabs(DC - lastDC) > 0.001)
   {
      lastDC = DC;
      float Qp = get_Qp(Qin, DC);
      float Tr = _Vres / (Qrp - get_Qinr(Qin, DC) + Qp);
      float Td = _Vres / (Qin - Qp);
      DC = 1.0f - Tr / (Tr + Td);
   }

   return DC;
}

//
// ----------------------------------------------------------------------------
//
void Calc_Preplatelet_Plasma::get_Qin (float& Qin, const float DC, const float Qneedle)
{
   Qin = 100.0f;
   float lastQin = 0.0f;

   while (fabs(Qin - lastQin)>0.01)
   {
      lastQin = Qin;
      float Qp = get_Qp(Qin, DC);
      Qin     = ((Qneedle + Qp) * (1.0f - DC)) / DC + Qp;
   }
}

//
// ----------------------------------------------------------------------------
//
const float Calc_Preplatelet_Plasma::get_Qn (const float Qin, const float Qrp)
{
   float DC = get_DC(Qin, Qrp);
   return (Qrp - get_Qinr(Qin, DC));
}

const float Calc_Preplatelet_Plasma::get_QnLimit (const float Qin, const float DC)
{
   float Qnret = 0.0f;
   float Qp    = get_Qp(Qin, DC);
   float Vsvn  = Qin * _Vres / (Qin - Qp);
   float dcf   = (1.0f - _Vsvnr / Vsvn);
   float Hnret = (_hct * _oneM1Ratio) / ( 1.0f -  Qp / (Qin * DC * dcf) ) ;
   Qnret = 1.69f * _cc.QneedleLimitMax * ( 1.0f - Hnret);

   return MIN(Qnret, _QnLimit);
}

//
// ----------------------------------------------------------------------------
//
const float Calc_Preplatelet_Plasma::get_maxQrp (const float Qin,
                                                 const float Qneedle,
                                                 const float QrpLimit,
                                                 const float DC)
{
   //
   //
   //   case 1: Qrp limited
   //
   if (get_Qn(Qin, QrpLimit) < _QnLimit) return QrpLimit;

   //
   //
   //   case 2: Qneedle limited
   //
   return Qneedle + get_Qinr(Qin, DC);
}



//
// ----------------------------------------------------------------------------
//
const float Calc_Preplatelet_Plasma::get_Qinr (const float Qin, const float DC)
{
   float Qinr;

   Qinr = _cc.MinRecirculatePlateletCollect * Qin + get_Qp(Qin, DC) + get_Qch(Qin);
   float max = Qin * _cc.MaxRecirculatePlateletCollect;
   upperLimit(Qinr, max);

   return Qinr;
}
//
// ----------------------------------------------------------------------------
//
const float Calc_Preplatelet_Plasma::get_Qp (const float Qin, const float DC)
{
   float       Qp;

   const float bobsA = (1.0f / (1.0f - DC) + _cc.MinRecirculatePlateletCollect / DC) / 2.0f;

   //
   //
   //    Calculate Qp as function of (Vsvn, fcmax) using quadratic formula
   //
   const float k = Qin * Qin * DC * DC;
   const float g = _generalfc / (1.0f - DC);
   const float h = Qin * DC * bobsA + get_Qch(Qin);

   const float A = 1.0f;
   const float B = 2.0f * h - (_Vsvnr * k * g) / (_Vres * Qin);
   const float C = (k * g * _Vsvnr * Qin) / (_Vres * Qin) - k * g + h * h - k * bobsA * bobsA;

   Qp = (-B + (float)sqrt(B * B - 4.0f * A * C)) / (2.0f * A);

#ifdef PREDICT_DEBUG
   ASSERT((B * B - 4.0f * A * C) > 0.0);
#endif
   lowerLimit(Qp, _cc.PlasmaMinimumFlow);
   return Qp;
}

const float Calc_Preplatelet_Plasma::get_Qch (float Qin) const
{
   (void)Qin;
   return _Qch; // MIN(_Qch, _CollectMaxFlow * MAX( MIN(Qin / _cc.CsfQin, _cc.CsfMax), _cc.CsfMin ));
}


//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
Calc_Platelet_Plasma::Calc_Platelet_Plasma()
   : Calc_Preplatelet_Plasma()
{}

Calc_Platelet_Plasma::~Calc_Platelet_Plasma()
{}

void Calc_Platelet_Plasma::Initialize (const float hct,
                                       const float precount,
                                       const float TBV,
                                       const float IR,
                                       const float ratio,
                                       const float QinLimit,
                                       const float QrpLimit,
                                       const float QnLimit,
                                       const float Qcmr)
{
   Calc_Preplatelet_Plasma::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, Qcmr);
}


// ----------------------------------------------------------------------------
//
const int Calc_Platelet_Plasma::calculate (struct IntermediateResults* substate)
{
   int status = Calc_Preplatelet_Plasma::calculate(substate);
   substate->Qininstr = get_Qinr(substate->Qininstd, substate->D);
   substate->Qpinstd  = get_Qp(substate->Qininstd, substate->D);
   substate->Qcinstd  = get_Qch(substate->Qininstd);
   substate->Qpinstr  = substate->Qpinstd;
   substate->Qcinstr  = substate->Qcinstd;
   substate->Qnret    = get_Qn(substate->Qininstd, substate->Qrinst);
   substate->Csf      = MAX(MIN(substate->Qininstd / _cc.CsfQin, _cc.CsfMax), _cc.CsfMin);
   substate->Qacinst  = substate->Qininstd / _ratio;
   substate->Vsvn     = substate->Qininstd * _Vres / (substate->Qininstd - substate->Qpinstd - get_Qch(substate->Qininstd));
   substate->Tr       = _Vres / (substate->Qrinst - substate->Qininstr + substate->Qpinstr + substate->Qcinstr);
   substate->Td       = _Vres / (substate->Qininstd - substate->Qpinstd - get_Qch(substate->Qininstd));
   substate->Tcn      = substate->Tr + substate->Td;
   substate->Facb     = _facb;
   substate->Facp     = _facp;

   return status;

}

const float Calc_Platelet_Plasma::get_Qch (float Qin) const
{
   return MIN(_Qch, _CollectMaxFlow * MAX(MIN(Qin / _cc.CsfQin, _cc.CsfMax), _cc.CsfMin));
}

// ----------------------------------------------------------------------------
//
float Calc_Platelet_Plasma::get_Qin (float DC)
{
   float Qin     = 100.0f;
   float lastQin = 0.0f;

   while (fabs(Qin - lastQin)>0.01)
   {
      lastQin = Qin;
      float Qp   = get_Qp(Qin, DC);

      float Vsvn = Qin * _Vres / (Qin - Qp - get_Qch(Qin));
      float dcf  = 1.0f - _Vsvnr / Vsvn;
      Qin = ((_IR * _TBV + (Qp + get_Qch(Qin)) * _facb) * _ratio) / (DC * dcf);
   }

   return Qin;
}
//
// ----------------------------------------------------------------------------
//
float Calc_Platelet_Plasma::get_Qin (float DC, float Qrp)
{
   float Qin;

   Qin = 100.0f;
   float lastQin = 0.0f;

   while (fabs(Qin - lastQin)>0.01)
   {
      lastQin = Qin;
      const float Qp = get_Qp(Qin, DC);
      const float Tr = _Vres / (Qrp - get_Qinr(Qin, DC) + Qp + get_Qch(Qin));
      Qin = 0.8f * ((_Vres * (1.0f - DC)) / (DC * Tr) + Qp + get_Qch(Qin)) + 0.2f * lastQin;
   }

   return Qin;
}
//
// ----------------------------------------------------------------------------
//
float Calc_Platelet_Plasma::get_Qrp (float DC, float Qin)
{
   float Qp  = get_Qp(Qin, DC);
   float Td  = _Vres / (Qin - Qp - get_Qch(Qin));
   float Qrp = (_Vres * DC) / (Td * (1.0f - DC)) + get_Qinr(Qin, DC) - Qp - get_Qch(Qin);

   return Qrp;
}
//
// ----------------------------------------------------------------------------
float Calc_Platelet_Plasma::get_DC (const float Qin, const float Qrp)
{
   float DC     = _cc.DutyCycleLimit;
   float lastDC = 0.0f;

   while (fabs(DC - lastDC) > 0.001)
   {
      lastDC = DC;
      const float Qp = get_Qp(Qin, DC);
      const float Tr = _Vres / (Qrp - get_Qinr(Qin, DC) + Qp + get_Qch(Qin));
      const float Td = _Vres / (Qin - Qp - get_Qch(Qin));
      DC = 1.0f - Tr / (Tr + Td);

#ifdef PREDICT_DEBUG
      ASSERT(DC > 0.0f);
      ASSERT(Tr > 0.0f);
      ASSERT(Td > 0.0f);
#endif
   }

   return DC;
}

//
// ----------------------------------------------------------------------------
//
void Calc_Platelet_Plasma::get_Qin (float& Qin, const float DC, const float Qneedle)
{
   Qin = 100.0f;
   float lastQin = 0.0f;

   while (fabs(Qin - lastQin)>0.01)
   {
      lastQin = Qin;
      float Qp = get_Qp(Qin, DC);
      Qin     = ((Qneedle + Qp + get_Qch(Qin)) * (1.0f - DC)) / DC + Qp + get_Qch(Qin);
   }

}

const float Calc_Platelet_Plasma::get_QnLimit (const float Qin, const float DC)
{
   float Qnret = 0.0f;
   float Qp    = get_Qp(Qin, DC);
   float Qch   = get_Qch(Qin);
   float Vsvn  = Qin * _Vres / (Qin - Qp - Qch);
   float dcf   = (1.0f - _Vsvnr / Vsvn);
   float Hnret = (_hct * _oneM1Ratio) / ( 1.0f - (Qp + Qch) / (Qin * DC * dcf) ) ;
   Qnret = 1.69f * _cc.QneedleLimitMax * ( 1.0f - Hnret);

   return MIN(Qnret, _QnLimit);
}


//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
Calc_PCA::Calc_PCA()
   : Calculate_Flows()
{}

Calc_PCA::~Calc_PCA()
{}

void Calc_PCA::Initialize (const float hct,
                           const float precount,
                           const float TBV,
                           const float IR,
                           const float ratio,
                           const float QinLimit,
                           const float QrpLimit,
                           const float QnLimit,
                           const float Qcmr)
{
   Calculate_Flows::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, _cc.DutyCycleLimit);
   _Qcmr      = Qcmr;
   _generalfc = (1.0f - (_hct * _oneM1Ratio) / _cc.MaxPlateletPlasmaHrbc);
}


// ----------------------------------------------------------------------------
//
const int Calc_PCA::calculate (struct IntermediateResults* substate)
{
   int status = Calculate_Flows::calculate(substate);

   substate->Qininstr = get_Qinr(substate->Qininstd, substate->D);
   substate->Qcinstd  = get_Qc(substate->Qininstd, substate->D);
   substate->Qpinstd  = substate->Qcinstd - get_Qch(substate->Qininstd);
   if ((substate->Qpinstd < 0.5) && (substate->Qcinstd == _Qcmr))    // IT 9685
   {
      substate->Qpinstd = 0.0f;
   }
   lowerLimit(substate->Qpinstd, 0.0f);
   substate->Qpinstr = substate->Qpinstd;
   substate->Qcinstr = substate->Qcinstd;
   substate->Qnret   = get_Qn(substate->Qininstd, substate->Qrinst);
   substate->Csf     = MAX(MIN(substate->Qininstd / _cc.CsfQin, _cc.CsfMax), _cc.CsfMin);
   substate->Qacinst = substate->Qininstd / _ratio;
   substate->Vsvn    = substate->Qininstd * _Vres / (substate->Qininstd - substate->Qcinstd);
   substate->Tr      = _Vres / (substate->Qrinst - substate->Qininstr + substate->Qcinstd);
   substate->Td      = _Vres / (substate->Qininstd - substate->Qcinstd);
   substate->Tcn     = substate->Tr + substate->Td;
   substate->Facb    = _facb;
   substate->Facp    = _facp;

   return status;

}



// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
float Calc_PCA::get_Qin (float DC)
{
   const float A = (1.0f / (1.0f - DC) + _cc.MinRecirculatePlateletCollect / DC) / 2.0f;

   float       Qc;
   float       Qin     = 100.0f;
   float       lastQin = 0.0f;

   while (fabs(Qin - lastQin)>0.01)
   {
      lastQin = Qin;

      Qc      = get_Qc(Qin, DC);
      const float Vsvn  = Qin * _Vres / (Qin - Qc);
      const float dcf   = 1.0f - _Vsvnr / Vsvn;

      const float fcmax = (float)(sqrt(A * A + dcf * _generalfc / (1.0f - DC)) - A) / dcf;

#ifdef PREDICT_DEBUG
      ASSERT(A * A + dcf * _generalfc / (1.0f - DC) > 0.0);
#endif

      Qin = (_IR * _ratio * _TBV / (1.0f - _facb * _ratio * fcmax)) / DC / dcf;
   }

   return Qin;
}

const float Calc_PCA::get_Qch (float Qin) const
{
   return MIN(_Qcmr, _CollectMaxFlow * MAX(MIN(Qin / _cc.CsfQin, _cc.CsfMax), _cc.CsfMin) );
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
float Calc_PCA::get_Qin (float DC, float Qrp)
{
   float Qin;

   Qin = 100.0f;
   float lastQin = 0.0f;

   while (fabs(Qin - lastQin)>0.01)
   {
      lastQin = Qin;
      const float Qc = get_Qc(Qin, DC);
      const float Tr = _Vres / (Qrp - get_Qinr(Qin, DC) + Qc);
      Qin = 0.8f * ((_Vres * (1.0f - DC)) / (DC * Tr) + Qc) + 0.2f * lastQin;
   }

// Internal consistency check
#if PREDICT_DEBUG
   const float DC1 = get_DC(Qin, Qrp);
   ASSERT(fabs(DC1 - DC) < 0.01);
#endif

   return Qin;
}


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
float Calc_PCA::get_Qrp (float DC, float Qin)
{
   float Qc  = get_Qc(Qin, DC);
   float Td  = _Vres / (Qin - Qc);
   float Qrp = (_Vres * DC) / (Td * (1.0f - DC)) + get_Qinr(Qin, DC) - Qc;

   return Qrp;
}
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
float Calc_PCA::get_DC (const float Qin, const float Qrp)
{
   float DC     = _DCLimit;

   float lastDC = 0.0f;

   while (fabs(DC - lastDC) > 0.001)
   {
      lastDC = DC;
      const float Qc = get_Qc(Qin, DC);
      const float Tr = _Vres / (Qrp - get_Qinr(Qin, DC) + Qc);
      const float Td = _Vres / (Qin - Qc);
      DC = 1.0f - Tr / (Tr + Td);
   }

   return DC;
}


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
void Calc_PCA::get_Qin (float& Qin, const float DC, const float Qneedle)
{
   Qin = 100.0f;
   float lastQin = 0.0f;

   while (fabs(Qin - lastQin)>0.01)
   {
      lastQin = Qin;

      const float Qc = get_Qc(Qin, DC);
      Qin = ((Qneedle + Qc) * (1.0f - DC)) / DC + Qc;
   }
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
const float Calc_PCA::get_Qc (const float Qin, const float DC)
{
   float       Qc;

   const float A = (1.0f / (1.0f - DC) + _cc.MinRecirculatePlateletCollect / DC) / 2.0f;

   //
   //
   //    Calculate Qc as function of (Vsvn, fcmax) using quadratic formula
   //
   const float k = Qin * Qin * DC * DC;
   const float g = _generalfc / (1.0f - DC);
   const float v = Qin * DC * A;

   const float B = 2.0f * v - (_Vsvnr * k * g) / (_Vres * Qin);
   const float C = (k * g * _Vsvnr * Qin) / (_Vres * Qin) - k * g + v * v - k * A * A;

   Qc = (-B + (float)sqrt(B * B - 4.0f * C)) / 2.0f;
#ifdef PREDICT_DEBUG
   ASSERT(B * B - 4.0f * C > 0.0);
#endif

   //
   //
   //   Ensure minimum flow limits are enforced
   //
   lowerLimit(Qc, _cc.CollectMinimumFlow);

   if ((Qc > GetCollectMaxFlow()) &&
       (Qc < (GetCollectMaxFlow() + _cc.CollectMinimumFlow)))
   {
      Qc = GetCollectMaxFlow();
   }
   // Ensure Qp runs >= 0.5ml/min
   /*const float Qch = get_Qch(Qin);
   if ( (Qc > Qch) && (Qc < (Qch + _cc.CollectMinimumFlow)) ) {
       Qc = Qch;
   }*/
   return Qc;
}


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
const float Calc_PCA::get_Qinr (const float Qin, const float DC)
{
   float Qinr;

   Qinr = _cc.MinRecirculatePlateletCollect * Qin + get_Qc(Qin, DC);
   const float max = Qin * _cc.MaxRecirculatePlateletCollect;
   upperLimit(Qinr, max);
   return Qinr;
}


//
// ----------------------------------------------------------------------------
//
const float Calc_PCA::get_Qn (const float Qin, const float Qrp)
{
   float DC = get_DC(Qin, Qrp);
   return (Qrp - get_Qinr(Qin, DC));
}

const float Calc_PCA::get_QnLimit (const float Qin, const float DC)
{
   float Qnret = 0.0f;
   float Qc    = get_Qc(Qin, DC);
   float Vsvn  = Qin * _Vres / (Qin - Qc);
   float dcf   = (1.0f - _Vsvnr / Vsvn);
   float Hnret = (_hct * _oneM1Ratio) / ( 1.0f -  Qc / (Qin * DC * dcf) ) ;
   Qnret = 1.69f * _cc.QneedleLimitMax * ( 1.0f - Hnret);
   return MIN(Qnret, _QnLimit);
}

//
// ----------------------------------------------------------------------------
//
const float Calc_PCA::get_maxQrp (const float Qin,
                                  const float Qneedle,
                                  const float QrpLimit,
                                  const float DC)
{
   float maxQrp;

   //   case 1: Qrp limited
   //
   if (get_Qn(Qin, QrpLimit) < get_QnLimit(Qin, DC))
      maxQrp = QrpLimit;
   else
      //   case 2: Qneedle limited
      //
      maxQrp = Qneedle + get_Qinr(Qin, DC);

   return maxQrp;
}



//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
Calc_Midrun::Calc_Midrun()
   : Calc_PCA()
{}

Calc_Midrun::~Calc_Midrun()
{}

void Calc_Midrun::Initialize (const float hct,
                              const float precount,
                              const float TBV,
                              const float IR,
                              const float ratio,
                              const float QinLimit,
                              const float QrpLimit,
                              const float QnLimit,
                              const float Qcmr)
{
   _Qcmr = Qcmr;
   // _Qch = MIN(_Qcmr, _CollectMaxFlow * MAX( MIN(QinLimit / _cc.CsfQin, _cc.CsfMax), _cc.CsfMin ) );

   Calc_PCA::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, _Qcmr);
}


// ----------------------------------------------------------------------------
//
const int Calc_Midrun::calculate (struct IntermediateResults* substate)
{
   return Calc_PCA::calculate(substate);
}


// ----------------------------------------------------------------------------
//
float Calc_Midrun::get_Qin (float DC)
{
   float Qin     = 100.0f;
   float lastQin = 0.0f;

   while (fabs(Qin - lastQin)>0.01)
   {
      lastQin = Qin;
      float Qc   = get_Qc(Qin, DC);
      float Vsvn = Qin * _Vres / (Qin - Qc);
      float dcf  = 1.0f - _Vsvnr / Vsvn;
      Qin = (_IR * _ratio * _TBV + _facb * _ratio * Qc) / DC / dcf;
   }

   return Qin;
}


//
// ----------------------------------------------------------------------------
//
const float Calc_Midrun::get_Qc (const float Qin, const float DC)
{
   float maxqc = Calc_PCA::get_Qc(Qin, DC);

   if (maxqc < _Qcmr)
      return maxqc;

   return _Qcmr;

}


//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
Calc_PIR_Plasma::Calc_PIR_Plasma()
   : Calc_Platelet_Plasma()
{}

Calc_PIR_Plasma::~Calc_PIR_Plasma()
{}

void Calc_PIR_Plasma::Initialize (const float hct,
                                  const float precount,
                                  const float TBV,
                                  const float IR,
                                  const float ratio,
                                  const float QinLimit,
                                  const float QrpLimit,
                                  const float QnLimit,
                                  const float Qch)
{
   Calc_Platelet_Plasma::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, Qch);
}
// ----------------------------------------------------------------------------
//
const int Calc_PIR_Plasma::calculate (struct IntermediateResults* substate)
{
   int status = Calc_Platelet_Plasma::calculate(substate);

   substate->Qininstr = get_Qinr(substate->Qininstd, substate->D);
   substate->Qpinstd  = get_Qp(substate->Qininstd, substate->D);
   substate->Qcinstd  = get_Qch(substate->Qininstd);
   substate->Qpinstr  = substate->Qpinstd;
   substate->Qcinstr  = substate->Qcinstd;
   substate->Qnret    = get_Qn(substate->Qininstd, substate->Qrinst);
   substate->Qacinst  = substate->Qininstd / _ratio;
   substate->Vsvn     = substate->Qininstd * _Vres / (substate->Qininstd - substate->Qpinstd - get_Qch(substate->Qininstd));
   substate->Tr       = _Vres / (substate->Qrinst - substate->Qininstr + substate->Qpinstr + substate->Qcinstr);
   substate->Td       = _Vres / (substate->Qininstd - substate->Qpinstd - get_Qch(substate->Qininstd));
   substate->Tcn      = substate->Tr + substate->Td;
   substate->Facb     = _facb;
   substate->Facp     = _facp;

   return status;

}
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
Calc_PIR_NoPlasma::Calc_PIR_NoPlasma()
   : Calc_Midrun()
{}

Calc_PIR_NoPlasma::~Calc_PIR_NoPlasma()
{}

void Calc_PIR_NoPlasma::Initialize (const float hct,
                                    const float precount,
                                    const float TBV,
                                    const float IR,
                                    const float ratio,
                                    const float QinLimit,
                                    const float QrpLimit,
                                    const float QnLimit,
                                    const float Qcmr)
{
   Calc_Midrun::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, Qcmr);
}
// ----------------------------------------------------------------------------
//
const int Calc_PIR_NoPlasma::calculate (struct IntermediateResults* substate)
{
   int status = Calc_Midrun::calculate(substate);

   substate->Qininstr = get_Qinr(substate->Qininstd, substate->D);
   substate->Qpinstd  = 0.0f;
   substate->Qcinstd  = _Qcmr;
   substate->Qpinstr  = substate->Qpinstd;
   substate->Qcinstr  = substate->Qcinstd;
   substate->Qnret    = get_Qn(substate->Qininstd, substate->Qrinst);
   substate->Qacinst  = substate->Qininstd / _ratio;
   substate->Vsvn     = substate->Qininstd * _Vres / (substate->Qininstd - substate->Qpinstd - _Qcmr);
   substate->Tr       = _Vres / (substate->Qrinst - substate->Qininstr + substate->Qcinstr);
   substate->Td       = _Vres / (substate->Qininstd - _Qcmr);
   substate->Tcn      = substate->Tr + substate->Td;
   substate->Facb     = _facb;
   substate->Facp     = _facp;

   return status;

}
//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//         Plt/Plasma/RBC set - rbc collection
//
Calc_RBC::Calc_RBC()
   : Calculate_Flows(),
     _DCLimit(_cc.DutyCycleLimit)  // Default DC limit.  Overridden by PTF
{}

Calc_RBC::~Calc_RBC()
{}

void Calc_RBC::Initialize (const float hct,
                           const float precount,
                           const float TBV,
                           const float IR,
                           const float ratio,
                           const float QinLimit,
                           const float QrpLimit,
                           const float QnLimit,
                           const float collectionCrit,
                           const float Qch, //  Not used here.
                           const float)     //  Not used here.
{
   _collectionCrit = collectionCrit;

   Calculate_Flows::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, _DCLimit);
}

// ----------------------------------------------------------------------------
//
const int Calc_RBC::calculate (struct IntermediateResults* substate)
{
   int status = Calculate_Flows::calculate(substate);
   substate->Qininstr = 0.0f;
   substate->Qnret    = substate->Qrinst;
   substate->Qacinst  = substate->Qininstd / _ratio;
   substate->Qcinstd  = get_Qc(substate->Qininstd);
   substate->Qpinstd  = substate->Qcinstd;

   substate->Qpinstr  = 0.0f;
   substate->Qcinstr  = 0.0f;
   substate->Vsvn     = _Vres / (1.0f - (substate->Qininstd - substate->Qcinstd) / substate->Qininstd);
   substate->Tr       = _Vres / substate->Qrinst;
   substate->Td       = _Vres / substate->Qcinstd;
   substate->Tcn      = substate->Tr + substate->Td;
   substate->Facb     = _facb;
   substate->Facp     = _facp;

   return status;
}

// ----------------------------------------------------------------------------
//
float Calc_RBC::get_Qin (float DC)
{
   const float Vsvn = (_Vres * _collectionCrit - _Vsvnr * _hct * _oneM1Ratio) / (_collectionCrit - _hct * _oneM1Ratio);
   const float dcf  = 1.0f - _Vsvnr / Vsvn;
   const float Hinr = _hct * _oneM1Ratio * dcf;

   float       Qin  = _IR * _ratio * _TBV / (DC * (dcf - (_facb * (1.0f - _collectionCrit) * _ratio) * Hinr / _collectionCrit));

   //
   // Ensure we don't violate average flow limits
   //
   const float avgQin = Qin * DC * dcf;

   if (avgQin > _cc.MaxQinRbcCollection)
      Qin = _cc.MaxQinRbcCollection / DC / dcf;

   return Qin;
}
//
// ----------------------------------------------------------------------------
//
float Calc_RBC::get_Qin (float DC, float Qrp)
{
   const float Vsvn = (_Vres * _collectionCrit - _Vsvnr * _hct * _oneM1Ratio) / (_collectionCrit - _hct * _oneM1Ratio);
   const float dcf  = 1.0f - _Vsvnr / Vsvn;
   const float Hinr = _hct * _oneM1Ratio * dcf;

   float       Tr   = _Vres / Qrp;
   float       Qin  = ((1.0f - DC) * _Vres) / (Tr * DC * (1.0f - Hinr / _collectionCrit));

   return Qin;
}
//
// ----------------------------------------------------------------------------
//
float Calc_RBC::get_Qrp (float DC, float Qin)
{
   float Td = _Vres / get_Qc(Qin);
   return (DC * _Vres) / (Td * (1.0f - DC));
}

//
// ----------------------------------------------------------------------------
float Calc_RBC::get_DC (const float Qin, const float Qrp)
{
   float Tr = _Vres / Qrp;
   float Td = _Vres / get_Qc(Qin);
   return (1.0f - Tr / (Tr + Td));
}

//
// ----------------------------------------------------------------------------
void Calc_RBC::get_Qin (float& Qin, const float DC, const float Qneedle)
{
   Qin = get_Qin(DC, Qneedle); // Qn = Qrp for this phase
}

//
// ----------------------------------------------------------------------------
float Calc_RBC::get_Qc (const float Qin)
{
   float K  = _hct * _oneM1Ratio / _collectionCrit;
   float Qc = (Qin - K * Qin) / (1.0f - K * _Vsvnr / _Vres);
   return Qc;
}
//
// ----------------------------------------------------------------------------
const float Calc_RBC::get_maxQrp (const float qin,
                                  const float Qneedle,
                                  const float QrpLIMIT,
                                  const float DC)
{
   float max = Qneedle;
   upperLimit(max, QrpLIMIT);
   upperLimit(max, get_QnLimit(qin, DC));
   return max;
}

//
// ----------------------------------------------------------------------------
//
const float Calc_RBC::get_Qn (const float, const float Qrp)
{
   return Qrp;
}



//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//         Plt/Plasma/RBC set - rbc collection
//
Calc_RBC_PTF::Calc_RBC_PTF()
   : Calc_RBC()
{}

Calc_RBC_PTF::~Calc_RBC_PTF()
{}

void Calc_RBC_PTF::Initialize (const float hct,
                               const float precount,
                               const float TBV,
                               const float IR,
                               const float ratio,
                               const float QinLimit,
                               const float QrpLimit,
                               const float QnLimit,
                               const float collectionCrit,
                               const float Qch,
                               const float QrbcMax)
{
   _Qch     = Qch;
   _qrbcmax = QrbcMax;
   _DCLimit = _cc.RBCDutyCycleLimit;   // Override DC

   Calc_RBC::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, collectionCrit, Qch, QrbcMax);
}



// ----------------------------------------------------------------------------
//
const int Calc_RBC_PTF::calculate (struct IntermediateResults* substate)
{
   int status = Calculate_Flows::calculate(substate);
   substate->Qininstr = 0.0f;
   substate->Qnret    = substate->Qrinst;
   substate->Qacinst  = substate->Qininstd / _ratio;
   substate->Qcinstd  = get_Qc(substate->Qininstd);
   substate->Qpinstd  = substate->Qcinstd - _Qch;

   if (substate->Qpinstd < 0.0f)
      substate->Qpinstd = 0.0f;

   substate->Qpinstr = 0.0f;
   substate->Qcinstr = 0.0f;
   substate->Vsvn    = _Vres / (1.0f - (substate->Qininstd - substate->Qcinstd) / substate->Qininstd);
   substate->Tr      = _Vres / substate->Qrinst;
   substate->Td      = _Vres / substate->Qcinstd;
   substate->Tcn     = substate->Tr + substate->Td;
   substate->Facb    = _facb;
   substate->Facp    = _facp;

   return status;
}



// ----------------------------------------------------------------------------
//
float Calc_RBC_PTF::get_Qin (float DC)
{
   const float Vsvn = (_Vres * _collectionCrit - _Vsvnr * _hct * _oneM1Ratio) / (_collectionCrit - _hct * _oneM1Ratio);
   const float dcf  = 1.0f - _Vsvnr / Vsvn;
   const float Hinr = _hct * _oneM1Ratio * dcf;

   float       Qin  = _IR * _ratio * _TBV / (DC * (dcf - (_facb * (1.0f - _collectionCrit) * _ratio) * Hinr / _collectionCrit));

   return limit_qin(Qin, DC);
}



//
// ----------------------------------------------------------------------------
//
float Calc_RBC_PTF::get_Qin (float DC, float Qrp)
{
   const float Vsvn = (_Vres * _collectionCrit - _Vsvnr * _hct * _oneM1Ratio) / (_collectionCrit - _hct * _oneM1Ratio);
   const float dcf  = 1.0f - _Vsvnr / Vsvn;
   const float Hinr = _hct * _oneM1Ratio * dcf;

   float       Tr   = _Vres / Qrp;
   float       Qin  = ((1.0f - DC) * _Vres) / (Tr * DC * (1.0f - Hinr / _collectionCrit));

   return limit_qin(Qin, DC);
}



/*  Derivation of above Qin as f(Qrbc) :

  Qrbc = Qin - Qc
  from Qc eqn:
   float K = _hct*_oneM1Ratio/_collectionCrit;
   float Qc = (Qin - K*Qin)/(1.0f - K*_cc.RecirculateSnDrawVol/_cc.ReservoirVolume);

    let dcffactor = _cc.RecirculateSnDrawVol/_cc.ReservoirVolume

  Qrbc = Qin - (Qin - K*Qin)/(1.0f - K*dcffactor)

  Normalize:
    Qrbc = Qin*(1-K*dcffactor) - (Qin - K*Qin)
           ------------------------------------
           (1-K*dcffactor)

    Qrbc*(1-K*dcffactor) = Qin - Qin*K*dcffactor - Qin + K*Qin
    Qrbc*(1-K*dcffactor) = -Qin*K*dcffactor + K*Qin
    Qrbc*(1-K*dcffactor) = Qin(-K*dcffactor + K)

*/
//
// ----------------------------------------------------------------------------
//
float Calc_RBC_PTF::limit_qin (const float Qin, const float DC)
{
   float       limitedQin = Qin;

   const float Vsvn       = (_Vres * _collectionCrit - _Vsvnr * _hct * _oneM1Ratio) / (_collectionCrit - _hct * _oneM1Ratio);
   const float dcf        = 1.0f - _Vsvnr / Vsvn;

   //
   //
   //   Ensure we don't violate average flow limits
   //
   const float avgQin = Qin * DC * dcf;
   if (avgQin > _cc.MaxQinRbcCollection)
      limitedQin = _cc.MaxQinRbcCollection / DC / dcf;

   //
   //
   //  Ensure we don't violate Qrbc flow limits
   //
   float qrbc = limitedQin - get_Qc(limitedQin);
   if (qrbc > _qrbcmax)
   {
      const float K         = _hct * _oneM1Ratio / _collectionCrit;
      const float dcffactor = _Vsvnr / _Vres;
      limitedQin = (_qrbcmax * (1.0f - K * dcffactor)) / (-K * dcffactor + K);
   }
   return limitedQin;
}



//
// ----------------------------------------------------------------------------
//         RBC setup flows assume 0% recirculation
// ----------------------------------------------------------------------------
//  Base Equations
//
//            draw              Vsvn / Qin               Qn
//  DC = --------------- = ------------------------- = ---------------------
//        draw + return     Vsvn / Qin + Vsvn / Qrp      Qn + Qin
//
//
//        Qn (1 - DC)
//  Qin = -------------
//             DC
//
//
//         DC * Qin
//  Qn =  ----------
//         1 - DC
//
//
// where:
//     Qrp = Qn (no - recirc)
//
//
// Qac = Qin / R
//
// ----------------------------------------------------------------------------
//
Calc_RBC_Setup::Calc_RBC_Setup()
   : Calculate_Flows()
{}

Calc_RBC_Setup::~Calc_RBC_Setup()
{}

void Calc_RBC_Setup::Initialize (const float hct,
                                 const float precount,
                                 const float TBV,
                                 const float IR,
                                 const float ratio,
                                 const float QinLimit,
                                 const float QrpLimit,
                                 const float QnLimit,
                                 const float Qch,
                                 const float Hrbc)
{
   _Qch  = Qch;
   _hrbc = Hrbc;
   Calculate_Flows::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, _cc.RBCDutyCycleLimit);
}

// ----------------------------------------------------------------------------
//
const int Calc_RBC_Setup::calculate (struct IntermediateResults* substate)
{
   substate->Qininstr = 0;
   substate->Qpinstr  = 0;
   substate->Qcinstr  = 0;

   int status = Calculate_Flows::calculate(substate);

   substate->Qcinstd = get_Qc(substate->Qininstd);
   substate->Qpinstd = substate->Qcinstd - _Qch;

   if (substate->Qpinstd < 0)
   {
      substate->Qpinstd = 0;
   }

   substate->Qnret   = substate->Qrinst;

   substate->Qacinst = substate->Qininstd / _ratio;
   substate->Vsvn    = _Vres;

   substate->Tr      = _Vres / substate->Qrinst;
   substate->Td      = _Vres / substate->Qininstd;
   substate->Tcn     = substate->Tr + substate->Td;
   substate->Facb    = _facb;
   substate->Facp    = _facp;

   return status;
}



//
// ----------------------------------------------------------------------------
const float Calc_RBC_Setup::get_Qc (const float Qin)
{
   const float DCF  = 1.0f - (_Vsvnr / _Vres);
   const float Hinr = _hct * _oneM1Ratio * DCF;

   float       Qc   = Qin * (1 - Hinr / _hrbc);

   return Qc;
}


float Calc_RBC_Setup::get_Qin (float DC)
{
   const float DCF = 1.0f - _Vsvnr / _Vres;
   const float qin = (_IR * _TBV * _ratio / (DC * DCF));

   if (qin <= 0.0f)
   {
      _failure = 21;
      return 0.0f;
   }

   return qin;
}

float Calc_RBC_Setup::get_Qin (float DC, float Qrp)
{
   const float DCF = 1.0f - (_Vsvnr / _Vres);

   return (Qrp * (1.0f - DC) ) / (DC * DCF);
}


float Calc_RBC_Setup::get_Qrp (float DC, float Qin)
{
   const float DCF = 1.0f - (_Vsvnr / _Vres);

   // IT 8242 reduce aps on returns in setup
   float Qrp = _cc.PtfSetupReturnPumpMultiple * (Qin * DC * DCF / (1.0f - DC));

   return Qrp;
}


float Calc_RBC_Setup::get_DC (const float Qin, const float Qrp)
{
   const float DCF = 1.0f - (_Vsvnr / _Vres);

   return Qrp / (Qrp + (Qin * DCF));
}


void Calc_RBC_Setup::get_Qin (float& Qin, const float DC, const float Qneedle)
{
   Qin = get_Qin(DC, Qneedle);
}



const float Calc_RBC_Setup::get_Qn (const float, const float Qrp)
{
   return Qrp;
}



const float Calc_RBC_Setup::get_maxQrp (const float, const float Qneedle, const float QrpLimit, const float)
{
   return (Qneedle > QrpLimit) ? QrpLimit : Qneedle;
}



//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//         Plt/Plasma/RBC set - rbc Prime
//
Calc_RBC_PTF_PRIME::Calc_RBC_PTF_PRIME()
   : Calc_RBC_PTF()
{}

Calc_RBC_PTF_PRIME::~Calc_RBC_PTF_PRIME()
{}

void Calc_RBC_PTF_PRIME::Initialize (const float hct,
                                     const float precount,
                                     const float TBV,
                                     const float IR,
                                     const float ratio,
                                     const float QinLimit,
                                     const float QrpLimit,
                                     const float QnLimit,
                                     const float collectionCrit,
                                     const float Qch,
                                     const float QrbcMax)
{
   _Qch     = Qch;
   _qrbcmax = QrbcMax;
   _DCLimit = _cc.RBCDutyCycleLimit;   // Override DC
   _hrbc    = collectionCrit;

   Calc_RBC_PTF::Initialize(hct, precount, TBV, IR, ratio, QinLimit, QrpLimit, QnLimit, collectionCrit, Qch, QrbcMax);
}


// PTF PRIME
// ----------------------------------------------------------------------------
//

const int Calc_RBC_PTF_PRIME::calculate (struct IntermediateResults* substate)
{
   int status = Calc_RBC_PTF::calculate(substate);

   substate->Qininstr = 0.0f;
   substate->Qnret    = substate->Qrinst;
   substate->Qacinst  = substate->Qininstd / _ratio;
   substate->Qcinstd  = get_Qc(substate->Qininstd);
   substate->Qpinstd  = substate->Qcinstd - _Qch;

   if (substate->Qpinstd < 0.0f)
      substate->Qpinstd = 0.0f;

   substate->Qpinstr = 0.0f;
   substate->Qcinstr = 0.0f;
   substate->Vsvn    = _Vres / (1.0f - (substate->Qininstd - substate->Qcinstd) / substate->Qininstd);
   substate->Tr      = _Vres / substate->Qrinst;
   substate->Td      = _Vres / substate->Qcinstd;
   substate->Tcn     = substate->Tr + substate->Td;
   substate->Facb    = _facb;
   substate->Facp    = _facp;

   return status;
}

/* FORMAT HASH fce37564bb1b29ef4c665394c9276853 */

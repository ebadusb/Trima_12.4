/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      acinfusc.cpp, the cycle-based AC infusion rate monitoring object.
 *
 * ABSTRACT:   Safety's monitoring for per cycle AC infusion rate.
 *             Monitoring is done at the end of the draw cycle and covers the
 *             draw cycle and the previous return cycle.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "trima_assert.h"
#include "alarms.h"

#include "trimamessages.h"

#include "chw.hpp"

#include "acinfus.hpp"
#include "s_donor.hpp"
#include "s_pfr.hpp"
#include "s_exec.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  ACInfusion Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety cycle-based AC Infusion rate monitoring constructor
//
// ERROR HANDLING:   none.

ACInfusion::ACInfusion(InletContainer* InletCont,
                       ReturnContainer* ReturnCont,
                       ACContainer* ACCont,
                       PlateletContainer* PlateletCont,
                       PlasmaContainer* PlasmaCont,
                       RBCContainer* RBCCont,
                       aDonor* Donor,
                       HypovolemiaDonor* HypoDonorMon)
   : _Inlet(InletCont), _Return(ReturnCont), _AC(ACCont),
     _Platelet(PlateletCont), _Plasma(PlasmaCont), _RBC(RBCCont),
     _Donor(Donor), _HypovolemiaDonorMonitor(HypoDonorMon)
{
   _faccumDrawCycleInletRBCCollect   = 0.0f;
   _faccumReturnCycleInletRBCCollect = 0.0f;
   _faccumCollectedRBCs              = 0.0f;
   _faccumACToPlateletBag            = 0.0f;
   _faccumACToPlasmaBag              = 0.0f;
   _faccumACToRBCBag                 = 0.0f;
   _faccumSetVolume                  = 0.0f;
   _fadjustedDonorHCT                = 0.0f;
   _bACInfusionMonitoringActive      = FALSE;
   _monType  = MON_INACTIVE;
   _dumpFlag = TRUE;

   clock_gettime(CLOCK_REALTIME, &_drawCycleTimer);
   clock_gettime(CLOCK_REALTIME, &_returnCycleTimer);
   _fdrawCycleTime      = 0.0f;
   _freturnCycleTime    = 0.0f;
   _fACInfusionPerTBV   = 0.0f;
   _fACInfusionPerTBV_2 = 0.0f;
   _fACInfusionPerTBV_3 = 0.0f;

   // fatal if containers are not initialized
   trima_assert(_Inlet);
   trima_assert(_Return);
   trima_assert(_AC);
   trima_assert(_Platelet);
   trima_assert(_Plasma);
   trima_assert(_RBC);
   trima_assert(_Donor);
   trima_assert(_HypovolemiaDonorMonitor);

   SaveToPFR();  // parasoft-suppress PB-26 "Object is fully constructed"
}

// SPECIFICATION:    Safety cycle-based AC Infusion rate monitoring destructor
//
// ERROR HANDLING:   none.

ACInfusion::~ACInfusion()
{
   _Inlet                   = NULL;
   _Return                  = NULL;
   _AC                      = NULL;
   _Platelet                = NULL;
   _Plasma                  = NULL;
   _RBC                     = NULL;
   _Donor                   = NULL;
   _HypovolemiaDonorMonitor = NULL;
}

// SPECIFICATION:    Initialize cycle-based AC infusion rate monitoring for appropriate state
//
// ERROR HANDLING:   none
void ACInfusion::InitializeForState (long newState)
{
   switch (newState)
   {
      case DONOR_CONNECTED :
      case BLOOD_PRIME :
      case BLOOD_RUN :
      case BLOOD_RINSEBACK :
         _bACInfusionMonitoringActive = TRUE;
         break;
      case DONOR_DISCONNECT :
      case POST_RUN :
         _bACInfusionMonitoringActive = FALSE;
         break;
      default :
         break;

   }

   SaveToPFR();
}

// SPECIFICATION:    Safety AC Infusion set type set.
//
//                   Set type is used to determine if we need
//                   to use the adjusted donor hematocrit
//                   while calculating AC Infusion.
//                   The donor's HCT changes substantially if
//                   we are in a RBC/Plasma procedure
//
// ERROR HANDLING:   none.
void ACInfusion::SetSetType (SET_TYPE setType)
{
   // log use of adjusted HCT
   // double check the set type
   if ( (setType == SET_RBCPLS) )
   {
      DataLog(log_level_safe_exec_info) << "Set " << (int)setType << ": RBC/Plasma procedure: Safety will use adjusted donor HCT." << endmsg;
   }

   SaveToPFR();
}


// SPECIFICATION:    Updates the inlet accumulators in the AC infusion rate
//                   monitoring object.  Inlet accumulators are used in
//                   calculating the AC in the RBC bag
//
// ERROR HANDLING:   none.

void ACInfusion::UpdateAccumulators (char cCycle)
{
   // if the RBC bag is collecting, accumulate the volume that
   // the inlet pump adds to the set

   // Fix for IT8057.
   const bool isCollectingRBCs = _RBC->_bag->_myValve.GetSafetyValve() == HW_VALVE_COLLECT;

   if (isCollectingRBCs)
   {
      if (cCycle == DRAW_CYCLE)
         _faccumDrawCycleInletRBCCollect += _Inlet->GetVolume(VOL_DELTA);
      else
         _faccumReturnCycleInletRBCCollect += _Inlet->GetVolume(VOL_DELTA);
   }

   // if this is an RBC/Plasma procedure and we are in first draw
   // accumulate the blood in the set which will be used to
   // determine the adjusted donor hematocrit.  This volume will
   // include the AC volume pumped so we want to subtract it
   if ( (SafetyExec::instance()->SetType() == SET_RBCPLS) &&
        (_monType == MON_FIRST_DRAW) )
   {
      _faccumSetVolume += ( _Inlet->GetVolume(VOL_DELTA)
                            - _Return->GetVolume(VOL_DELTA)
                            - _AC->GetVolume(VOL_DELTA));
   }

   SaveToPFR();
}


// SPECIFICATION:    Updates the cycle timers in the AC infusion rate
//                   monitoring object.
//
// ERROR HANDLING:   none.

void ACInfusion::UpdateTimers (CHW_RESERVOIR_STATES newReservoir)
{
   // if the reservoir just hit empty, read the return cycle timer
   // and reset the draw cycle timer
   if (newReservoir == CHW_RESERVOIR_EMPTY)
   {
      _freturnCycleTime = calc_elapsed_time(&_returnCycleTimer);
      clock_gettime(CLOCK_REALTIME, &_drawCycleTimer);
      DataLog(log_level_safe_exec_info) <<  "Return cycle time: " << _freturnCycleTime << endmsg;
   }
   else if (newReservoir == CHW_RESERVOIR_HIGH)
   // read the draw cycle timer and reset the return cycle timer
   {
      _fdrawCycleTime = calc_elapsed_time(&_drawCycleTimer);
      clock_gettime(CLOCK_REALTIME, &_returnCycleTimer);
      DataLog(log_level_safe_exec_info) << "Draw cycle time: " << _fdrawCycleTime << endmsg;
   }

   SaveToPFR();
}


// SPECIFICATION:    Calculates the adjusted donor hematocrit
//                   for RBC/Plasma procedures.  For other cases
//                   use the donor's initial hematocrit
//
// ERROR HANDLING:   none.
void ACInfusion::CalculateDonorHCT (float avgACRatio)
{
   float TBV_mls, lastHCT;
   float deltaV;

   if (SafetyExec::instance()->SetType() == SET_RBCPLS)
   {
      // first time through, set to entered HCT
      if (_fadjustedDonorHCT == 0.0f)
         _fadjustedDonorHCT = _Donor->GetHCT_decimal();

      // the adjusted donor's HCT can be calculated by
      // HCTadj = ( (HCTdon * TBV) - (collected RBCs) - (RBCs in set))
      //          / (TBV - net change in TBV)
      //
      // Let's break this equation apart:
      //
      // HCTdon is the entered donor HCT.
      //
      // delta Collected RBCs can be calculated HCTrbc(t) * deltaVOLrbc(t)
      // where HCTrbc(t) is the hematocrit of the RBC bag and deltaVOLrbc(t)
      // is the collected volume in the RBC bag for this cycle.
      // Hematocrit of the RBC bag can be expressed as
      // HCTrbc = (VOLin * HCTdon * (1-1/R))/VOLrbc.
      // (See derivation for HCTrbc equation in the function CalculateACtoRBCBag.)
      //
      // For the current cycle HCTrbc(t) can be expressed as
      // HCTrbc(t) = deltaVOLin(t) * HCTadj(t-1) * (1-1/R(t)) / deltaVOLrbc(t)
      //
      // In this case, deltaVOLin(t) and deltaVOLrbc(t) are for current cycle.
      // deltaVOLin(t) is the input volume pumped while the RBC valve is open.
      // deltaVOLrbc(t) is the RBC volume collected.
      // delta Collected RBCs= delatVOLin(t) * HCTadj(t-1) * (1-1/R(t))
      //
      // RBCs in the set = HCTadj(t-1) * VOLset where VOLset is a constant that
      // is measured during first draw.
      // RBCset = HCTadj(t-1) * _faccumSetVolume
      //
      // Net change in TBV is the extra-corporeal volume of the donor.  The
      // monitor for EC volume is already tracking this so we will just ask
      // it for the volume.
      //
      // Substituting the above quantities in the original equation we get
      // HCTadj(t) = ( (HCTinit * TBV)
      //             - summation(deltaVOLin(t) * HCTadj(t-1) * (1-1/R(t)))
      //             - (HCTadj(t-1) * _faccumSetVolume) )
      //           / (TBV - EC)


      TBV_mls = _Donor->GetTBV_liters() * 1000.0f;
      lastHCT = _fadjustedDonorHCT;
      deltaV  = (   _faccumDrawCycleInletRBCCollect
                    + _faccumReturnCycleInletRBCCollect
                    - MANIFOLD_VOLUME );

      if ( deltaV > 0.0f )
         _faccumCollectedRBCs += (   deltaV
                                     * lastHCT
                                     * (1.0f - (1.0f / avgACRatio) ) );

      // TBV should never be zero when we have a donor connected, but just in case..
      if (TBV_mls != 0)
      {
         _fadjustedDonorHCT = ( (TBV_mls * _Donor->GetHCT_decimal())
                                - _faccumCollectedRBCs
                                - (lastHCT * _faccumSetVolume) )
                              / ( TBV_mls - _HypovolemiaDonorMonitor->GetPrivateFloatData(HYPPD_EC) );

         DataLog(log_level_safe_exec_info) << "Adjusted Donor HCT: "
                                           << _fadjustedDonorHCT << ", VolumeDelta: " << deltaV
                                           << "(" << _faccumDrawCycleInletRBCCollect << "+"
                                           << _faccumReturnCycleInletRBCCollect << "-"
                                           << MANIFOLD_VOLUME << "), CollRBC: " << _faccumCollectedRBCs
                                           << ", SetVol: " << _faccumSetVolume << endmsg;
      }
      else
      {
         // tell procedure b4 go out
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);

         if (_dumpFlag)
         {
            _dumpFlag = false;
            Dump(log_level_safety_alarm_detail);
         }
      }
   }
   else
      // always use the entered donor HCT if this is not an RBC/Plasma procedure
      _fadjustedDonorHCT = _Donor->GetHCT_decimal();
}



// SPECIFICATION:    Calculates the AC delivered to the Platelet bag in the
//                   current draw or return cycle
//
// ERROR HANDLING:   none.
float ACInfusion::CalculateACToPlateletBag (char cCycle, float fraction)
{
   float ACToBag;

   // a negative volume indicates the flow is into the bag so if the flow is
   // positive, there is no AC going to the bag
   if (cCycle == DRAW_CYCLE)
   {
      ACToBag = fraction * (_Platelet->GetVolume(VOL_DRAW_CYCLE_INFUSION));

      if (ACToBag > 0.0f)
         ACToBag = 0.0f;

      DataLog(log_level_safe_exec_info) << "AC to Platelet in Draw: ";
   }
   else
   {
      ACToBag = fraction * (_Platelet->GetVolume(VOL_RETURN_CYCLE_INFUSION));

      if (ACToBag > 0.0f)
         ACToBag = 0.0f;

      DataLog(log_level_safe_exec_info) << "AC to Platelet in Return: ";
   }

   DataLog(log_level_safe_exec_info) << ACToBag << endmsg;

   _faccumACToPlateletBag += ACToBag;

   return ACToBag;
}


// SPECIFICATION:    Calculates the AC delivered to the Plasma bag in the
//                   current draw or return cycle
//
// ERROR HANDLING:   none.
float ACInfusion::CalculateACToPlasmaBag (char cCycle, float fraction)
{
   float ACToBag;

   if (cCycle == DRAW_CYCLE)
   {
      ACToBag = fraction * (_Plasma->GetVolume(VOL_DRAW_CYCLE_INFUSION));

      if (ACToBag > 0.0f)
         ACToBag = 0.0f;

      DataLog(log_level_safe_exec_info) << "AC to Plasma in Draw: ";
   }
   else
   {
      ACToBag = fraction * (_Plasma->GetVolume(VOL_RETURN_CYCLE_INFUSION));

      if (ACToBag > 0.0f)
         ACToBag = 0.0f;

      DataLog(log_level_safe_exec_info) << "AC to Plasma in Return: ";
   }

   DataLog(log_level_safe_exec_info) << ACToBag << endmsg;

   _faccumACToPlasmaBag += ACToBag;

   return ACToBag;
}


// SPECIFICATION:    Calculates the AC delivered to the RBC bag in the
//                   current draw or return cycle
//
// ERROR HANDLING:   none.
float ACInfusion::CalculateACToRBCBag (char cCycle, float fraction, float avgACRatio)
{
   float ACToBag;
   float HCTrbc;

   // DataLog(log_level_safe_exec_info) << " cCycle =" << (int)cCycle << endmsg;
   // DataLog(log_level_safe_exec_info) << " fraction =" << fraction << endmsg;
   // DataLog(log_level_safe_exec_info) << " avgACRatio =" << avgACRatio << endmsg;
   // We need the HCT of the RBC bag because we need to figure out how
   // much fluid (which carries AC) is in the bag.
   //
   // The hematocrit of the inlet flow is HCT of the donor times whole blood
   // divided by the inlet flow or Hin = (HCT * Qwb)/Qin.
   // Remember Qin = Qwb + Qac and R = Qin/Qac.
   // So Hin = (HCT * (Qin - Qac))/Qin = (HCT * (Qin - Qin/R))/Qin
   // Hin = HCT * (1-1/R)
   //
   // The RBCs flowing into the system are Qin * Hin
   // The fluid flowing into the RBC bag is Qrbc
   // The HCT of the RBC bag is the RBCs flowing in the the bag divided by the
   // fluid flowing in to the bag.
   // So the HCT of the RBC bag is (Qin * Hin) / Qrbc
   // HCTrbc = (Qin * HCT * (1-1/R))/Qrbc


   if ( ( cCycle == DRAW_CYCLE) && (_RBC->GetVolume(VOL_DRAW_CYCLE_INFUSION) != 0) ||
        ( cCycle == RETURN_CYCLE) && (_RBC->GetVolume(VOL_RETURN_CYCLE_INFUSION) != 0) )
   {
      if (cCycle == DRAW_CYCLE)
      {
         HCTrbc = (   (   _faccumDrawCycleInletRBCCollect
                          - MANIFOLD_VOLUME )
                      *  _fadjustedDonorHCT
                      *  (1.0 - (1.0 / avgACRatio)))
                  /  fabs(_RBC->GetVolume(VOL_DRAW_CYCLE_INFUSION));
      }
      else
      {
         HCTrbc = (_faccumReturnCycleInletRBCCollect
                   *  _fadjustedDonorHCT
                   *  (1.0 - (1.0 / avgACRatio)))
                  /  fabs(_RBC->GetVolume(VOL_RETURN_CYCLE_INFUSION));
      }

      if (HCTrbc <= 0.0f)
      {
         HCTrbc = 0.01f;
      }
      else if (HCTrbc > 1.0f)
      {
         HCTrbc = 0.99f;
      }

      // We know the HCT of the RBC bag so we can get the percentage of
      // fluid in the RBC bag = 1-HCTrbc
      //
      // AC travels with the fluid.  The AC in the bag is the
      // bag fraction * fluid factor * bag volume.  The bag fraction
      // was calculated earlier and passed to this function.
      if (cCycle == DRAW_CYCLE)
      {
         ACToBag = fraction
                   * (1.0 - HCTrbc)
                   * _RBC->GetVolume(VOL_DRAW_CYCLE_INFUSION);

         if (ACToBag > 0.0f)
            ACToBag = 0.0f;

         DataLog(log_level_safe_exec_info) << "AC to RBC in Draw: " << ACToBag
                                           << " (HCTrbc=" << HCTrbc << "), AccumV: "
                                           << _faccumDrawCycleInletRBCCollect << "-" << MANIFOLD_VOLUME << endmsg;
      }
      else
      {
         ACToBag = fraction
                   * (1.0 - HCTrbc)
                   * _RBC->GetVolume(VOL_RETURN_CYCLE_INFUSION);

         if (ACToBag > 0.0f)
            ACToBag = 0.0f;

         DataLog(log_level_safe_exec_info) << "AC to RBC in Return: " << ACToBag
                                           << "(HCTrbc=" << HCTrbc << "), AccumV: " << _faccumReturnCycleInletRBCCollect
                                           << "-" << MANIFOLD_VOLUME << endmsg;
      }
   }
   else
   {
      ACToBag = 0.0f;
   }

   _faccumACToRBCBag += ACToBag;

   return ACToBag;
}

// SPECIFICATION:    Calculates the AC infusion rate for the just-ending
//                   draw cycle and the previous return cycle.  Tests
//                   the infusion rate against the alarm values
//
// ERROR HANDLING:   AC Infusion High Alarm.
void ACInfusion::UpdateInfusionRate (float avgACRatio)
{
   float bagFraction;
   float ACToDonorDrawCycle, ACToDonorReturnCycle;
   float DonorTBVLiters, CycleTime;
   float f2cycleInfusionPerTBV, f3cycleInfusionPerTBV;

   // if AC infusion rate monitoring is not active,
   // just return
   if ( (!_bACInfusionMonitoringActive) ||
        (_monType == MON_INACTIVE) ||
        (_monType == MON_FIRST_DRAW) )
   {
      SaveToPFR();
      return;
   }

   // Compute the FACB bag fraction denominator for divide-by-zero check
   float bagFracDenom = ( 1.0f + ( (avgACRatio - 1.0f) * (1.0f - _fadjustedDonorHCT) ) );

   // Protect against divide-by-zero for subsequent calculations using acRatio
   if ( (avgACRatio == 0.0f) || (bagFracDenom == 0.0f) )
   {
      DataLog(log_level_safe_exec_info) << " INVALID: avgACRatio = " << avgACRatio << endmsg;

      // tell procedure b4 go out
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);

      if (_dumpFlag)
      {
         _dumpFlag = false;
         Dump(log_level_safety_alarm_detail);
      }
      return;
   }

   // If we are doing RBC/Plasma procedures, we need to use the adjusted donor HCT
   // because the HCT is changing as the procedure progresses but if this is not
   // RBC/Plasma, use the initial hematocrit
   CalculateDonorHCT(avgACRatio);


   // The Inlet fluid is made up of whole blood and AC because the AC travels past
   // the inlet pump after being pumped by the AC pump. Qin = Qwb+Qac.
   // The whole blood is plasma and RBC.  The HCT is the percentage of RBC in the whole
   // blood.  (1-HCT) is the percentage of plasma in the whole blood.
   // By definition, the ACRatio R = Qin/Qac.
   //
   // We want to figure out the amount of AC in the bags.  AC travels with the fluid.
   // The bag has three things in it -- RBCs, Plasma and AC. The fluid is made of AC
   // and Plasma.
   // The fraction of AC in the bag is Qac/(Qac+Qp) = (Qin/R)/((Qin/R)+Qp)
   // The RBCs in the bag are Qwb*HCT = (Qin-Qac)*HCT
   // The plasma in the bag is Qwb(1-HCT) = (Qin-Qac)(1-HCT) = (Qin-Qin/R)(1-HCT)
   // = Qin(1-1/R)(1-HCT) = Qp
   // Remember the fraction of AC in the bag = FACB = (Qin/R)/((Qin/R)+Qp)
   // FACB = (Qin/R)/((Qin/R)+(Qin(1-1/R)(1-HCT)) = (1/R)/((1/R) + (1-1/R)(1-HCT))
   // FACB = 1/(1+(R-1)(1-HCT))

   bagFraction = 1.0f / bagFracDenom;
   // DataLog(log_level_safe_exec_info) << " bagFraction =" << bagFraction << endmsg;


   // AC to donor for the return cycle is the AC delivered in the
   // return minus the AC collected in the product in each bag
   ACToDonorReturnCycle = _AC->GetVolume(VOL_RETURN_CYCLE)
                          + CalculateACToPlateletBag(RETURN_CYCLE, bagFraction)
                          + CalculateACToPlasmaBag(RETURN_CYCLE, bagFraction)
                          + CalculateACToRBCBag(RETURN_CYCLE, bagFraction, avgACRatio);

   DataLog(log_level_safe_exec_info) << " ACToDonorReturnCycle =" << ACToDonorReturnCycle << endmsg;

   // AC to donor for the draw cycle is the AC delivered in the
   // draw minus the AC collected in the product in each bag
   ACToDonorDrawCycle = _AC->GetVolume(VOL_DRAW_CYCLE)
                        + CalculateACToPlateletBag(DRAW_CYCLE, bagFraction)
                        + CalculateACToPlasmaBag(DRAW_CYCLE, bagFraction)
                        + CalculateACToRBCBag(DRAW_CYCLE, bagFraction, avgACRatio);

   DataLog(log_level_safe_exec_info) << " ACToDonorDrawCycle =" << ACToDonorDrawCycle << endmsg;

   // Infusion rate is calculated over the just-ending draw cycle and the previous return
   CycleTime = _freturnCycleTime + _fdrawCycleTime;

   // debug
   DataLog(log_level_safe_exec_info) << "AC in Return: "
                                     << _AC->GetVolume(VOL_RETURN_CYCLE)
                                     << ", AC in Draw: "
                                     << _AC->GetVolume(VOL_DRAW_CYCLE)
                                     << ", Return Time: "
                                     << _freturnCycleTime
                                     << ", Draw Time: "
                                     << _fdrawCycleTime
                                     << endmsg;


   // AC infusion rate is calculated in units of mls/min per liter of TBV for the donor
   // so make sure we have valid donor TBV
   DonorTBVLiters = _Donor->GetTBV_liters();

   if (DonorTBVLiters != 0.0f)
   {
      float ACToDonor = ACToDonorDrawCycle + ACToDonorReturnCycle;

      ACToDonor          = ( ACToDonor >= 0.0f ? ACToDonor : 0.0f );

      _fACInfusionPerTBV = ( ACToDonor / CycleTime * 60.0 ) / DonorTBVLiters;
   }
   else
   {
      // tell procedure b4 go out
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);

      if (_dumpFlag)
      {
         _dumpFlag = false;
         Dump(log_level_safety_alarm_detail);
      }
   }

   // AC infusion rate is tested on a one, two and three cycle average
   f2cycleInfusionPerTBV = (_fACInfusionPerTBV + _fACInfusionPerTBV_2) / 2.0f;
   f3cycleInfusionPerTBV = (_fACInfusionPerTBV + _fACInfusionPerTBV_2 + _fACInfusionPerTBV_3) / 3.0f;

   if ( (_fACInfusionPerTBV > MAX_ALLOWABLE_AC_INFUSION_SINGLE_CYCLE ) ||
        (f2cycleInfusionPerTBV > MAX_ALLOWABLE_AC_INFUSION_TWO_CYCLE_AVG ) ||
        (f3cycleInfusionPerTBV > MAX_ALLOWABLE_AC_INFUSION_THREE_CYCLE_AVG ) )
   {
      // AC infusion shutdown
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, AC_INFUSION_HIGH_ALARM);

      if (_dumpFlag)
      {
         _dumpFlag = false;
         Dump(log_level_safety_alarm_detail);
      }
   }

   // always log infusion value
   DataLog(log_level_safe_exec_info) << "Safety AC Infusion = " << _fACInfusionPerTBV
                                     << " 2 avg:" << f2cycleInfusionPerTBV << ", 3 avg:" << f3cycleInfusionPerTBV << endmsg;

   // save infusion rates for next cycle's averages
   _fACInfusionPerTBV_3 = _fACInfusionPerTBV_2;
   _fACInfusionPerTBV_2 = _fACInfusionPerTBV;

   // save to PFR
   SaveToPFR();
}

// SPECIFICATION:    Cycle-based AC infusion monitoring method to
//                   return private data for logging
//
// ERROR HANDLING:   FATAL ERROR for unknown type.

float ACInfusion::GetPrivateFloatData (INF_PDATA_TYPE type)
{
   float result = FLT_MAX;    // To keep GNU compiler happy for production builds

   switch (type)
   {
      case INFPD_PLATELET :
         result = _faccumACToPlateletBag;
         break;
      case INFPD_PLASMA :
         result = _faccumACToPlasmaBag;
         break;
      case INFPD_RBC :
         result = _faccumACToRBCBag;
         break;
      case INFPD_INF :
         result = _fACInfusionPerTBV;
         break;
      default :
      {
         DataLog(log_level_safety_alarm_detail) << "Type: " << type << "Request for unknown data" << endmsg;
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);
      }
      break;
   }

   return result;
}

// SPECIFICATION:    Cycle-based AC infusion monitoring method to reset
//                   volumes at the start of the draw or return cycle
//
// ERROR HANDLING:   none.
void ACInfusion::ResetCycleVol (char cCycle)
{
   if (cCycle == DRAW_CYCLE)
   {
      _faccumDrawCycleInletRBCCollect = 0.0f;
      _fdrawCycleTime                 = 0.0f;
      clock_gettime(CLOCK_REALTIME, &_drawCycleTimer);
   }
   else if (cCycle == RETURN_CYCLE)
   {
      _faccumReturnCycleInletRBCCollect = 0.0f;
      _freturnCycleTime                 = 0.0f;
      clock_gettime(CLOCK_REALTIME, &_returnCycleTimer);
   }

   SaveToPFR();
}

// SPECIFICATION:    Dump container data
//
// ERROR HANDLING:   none.
void ACInfusion::Dump (DataLog_Level& log_level)
{
   _Inlet->DumpContainer(log_level);
   _Return->DumpContainer(log_level);
   _AC->DumpContainer(log_level);
   _Platelet->DumpContainer(log_level);
   _Plasma->DumpContainer(log_level);
   _RBC->DumpContainer(log_level);
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.
void ACInfusion::SaveToPFR ()
{
   SafetyPFRDataToSave.PFR_faccumDrawCycleInletRBCCollect       = _faccumDrawCycleInletRBCCollect;
   SafetyPFRDataToSave.PFR_faccumReturnCycleInletRBCCollect     = _faccumReturnCycleInletRBCCollect;
   SafetyPFRDataToSave.PFR_faccumCollectedRBCs                  = _faccumCollectedRBCs;
   SafetyPFRDataToSave.PFR_faccumACToPlateletBag                = _faccumACToPlateletBag;
   SafetyPFRDataToSave.PFR_faccumACToPlasmaBag                  = _faccumACToPlasmaBag;
   SafetyPFRDataToSave.PFR_faccumACToRBCBag                     = _faccumACToRBCBag;
   SafetyPFRDataToSave.PFR_faccumSetVolume                      = _faccumSetVolume;
   SafetyPFRDataToSave.PFR_fadjustedDonorHCT                    = _fadjustedDonorHCT;
   SafetyPFRDataToSave.PFR_drawCycleTimer                       = _drawCycleTimer;
   SafetyPFRDataToSave.PFR_returnCycleTimer                     = _returnCycleTimer;
   SafetyPFRDataToSave.PFR_fdrawCycleTime                       = _fdrawCycleTime;
   SafetyPFRDataToSave.PFR_freturnCycleTime                     = _freturnCycleTime;
   SafetyPFRDataToSave.PFR_fACInfusionPerTBV                    = _fACInfusionPerTBV;
   SafetyPFRDataToSave.PFR_fACInfusionPerTBV_2                  = _fACInfusionPerTBV_2;
   SafetyPFRDataToSave.PFR_fACInfusionPerTBV_3                  = _fACInfusionPerTBV_3;
   SafetyPFRDataToSave.PFR_acInfusC_bACInfusionMonitoringActive = _bACInfusionMonitoringActive;
   SafetyPFRDataToSave.PFR_acInfusC_monType                     = _monType;
}


// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.
void ACInfusion::RestoreFromPFR ()
{
   _faccumDrawCycleInletRBCCollect   = SafetyPFRDataFromRestore.PFR_faccumDrawCycleInletRBCCollect;
   _faccumReturnCycleInletRBCCollect = SafetyPFRDataFromRestore.PFR_faccumReturnCycleInletRBCCollect;
   _faccumCollectedRBCs              = SafetyPFRDataFromRestore.PFR_faccumCollectedRBCs;
   _faccumACToPlateletBag            = SafetyPFRDataFromRestore.PFR_faccumACToPlateletBag;
   _faccumACToPlasmaBag              = SafetyPFRDataFromRestore.PFR_faccumACToPlasmaBag;
   _faccumACToRBCBag                 = SafetyPFRDataFromRestore.PFR_faccumACToRBCBag;
   _faccumSetVolume                  = SafetyPFRDataFromRestore.PFR_faccumSetVolume;
   _fadjustedDonorHCT                = SafetyPFRDataFromRestore.PFR_fadjustedDonorHCT;
   _drawCycleTimer                   = SafetyPFRDataFromRestore.PFR_drawCycleTimer;
   _returnCycleTimer                 = SafetyPFRDataFromRestore.PFR_returnCycleTimer;
   _fdrawCycleTime                   = SafetyPFRDataFromRestore.PFR_fdrawCycleTime;
   _freturnCycleTime                 = SafetyPFRDataFromRestore.PFR_freturnCycleTime;
   _fACInfusionPerTBV                = SafetyPFRDataFromRestore.PFR_fACInfusionPerTBV;
   _fACInfusionPerTBV_2              = SafetyPFRDataFromRestore.PFR_fACInfusionPerTBV_2;
   _fACInfusionPerTBV_3              = SafetyPFRDataFromRestore.PFR_fACInfusionPerTBV_3;
   _bACInfusionMonitoringActive      = SafetyPFRDataFromRestore.PFR_acInfusC_bACInfusionMonitoringActive;
   _monType = SafetyPFRDataFromRestore.PFR_acInfusC_monType;

   DataLog(log_level_safe_exec_info) << "ACInfusion::RestoreFromPFR(), "
                                     << "faccumDrawCycleInletRBCCollect: " << _faccumDrawCycleInletRBCCollect
                                     << " faccumReturnCycleInletRBCCollect: " << _faccumReturnCycleInletRBCCollect
                                     << " faccumCollectedRBCs: " << _faccumCollectedRBCs
                                     << " faccumACToPlateletBag: " << _faccumACToPlateletBag
                                     << " faccumACToPlasmaBag: " << _faccumACToPlasmaBag
                                     << " faccumSetVolume: " << _faccumSetVolume
                                     << " fadjustedDonorHCT: " << _fadjustedDonorHCT
                                     << " drawCycleTimer: " << _drawCycleTimer.tv_sec << "." << _drawCycleTimer.tv_nsec
                                     << " returnCycleTimer: " << _returnCycleTimer.tv_sec << "." << _returnCycleTimer.tv_nsec
                                     << " fdrawCycleTime: " << _fdrawCycleTime
                                     << " freturnCycleTime: " << _freturnCycleTime
                                     << " fACInfusionPerTBV: " << _fACInfusionPerTBV
                                     << " fACInfusionPerTBV_2: " << _fACInfusionPerTBV_2
                                     << " fACInfusionPerTBV_3: " << _fACInfusionPerTBV_3
                                     << " bACInfusionMonitoringActive: " << _bACInfusionMonitoringActive
                                     << " monType: " << (MON_TYPE)_monType
                                     << endmsg;

   SaveToPFR();
}

/* FORMAT HASH bbd7d34b7089cfdc4e80e27946be1e1e */

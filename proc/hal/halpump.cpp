/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/proc/hal/rcs/halpump.cpp 1.7 2007/10/18 19:33:04Z jheiusb Exp dslausb $
 * $Log: halpump.cpp $
 * Revision 1.7  2007/10/18 19:33:04Z  jheiusb
 * 8256 -- remove the SV changes for plasma pump due to Autosplit
 * Revision 1.6  2007/10/08 16:14:31Z  jheiusb
 * 8194 -- SV pressure changes for the PTF on plasma pump
 * Revision 1.5  2007/08/23 19:24:47Z  spriusb
 * Fix for IT 7742:  Additional interfaces for stroke volumes to support service mode
 * Revision 1.4  2007/04/02 20:25:22Z  jheiusb
 * update 5.R metering stroke volumes per 5.R spec
 * Revision 1.3  2006/03/03 17:28:45Z  jheiusb
 * init merge
 * Revision 1.2  2002/10/28 21:49:05Z  jl11312
 * - modified HAL to be part of proc instead of a separate task
 * - modified to handle the HAL status common data store
 *
 */

#include <vxWorks.h>

#include "halpump.h"
#include "procData.h"
#include "filenames.h"

#include "cobeconfig.h"    // for all the cobe config values

// and yet more constants
const float CDRBCPumpSet::PLASMA_PUMP_INLET_PRESSURE_RBC1 = 250.0f;
const float CDRBCPumpSet::PLASMA_PUMP_INLET_PRESSURE_RBC2 = 250.0f;



CPump::CPump(float MaxRPM)
{
   Init();
   m_fMaxRPM = MaxRPM;


}

CPump::~CPump()
{}

void CPump::Init ()
{
   m_fStrokeVolume = 1.0f;
   m_fMaxRPM       = 0.0f;
}

float CPump::RPM (float flowrate) const
{
   float temp;

   temp = flowrate / StrokeVolume();

   return ( temp > m_fMaxRPM ) ? m_fMaxRPM : temp;
}

float CPump::Flow (float RPM) const
{
   return RPM * StrokeVolume();
}

float CPump::StrokeVolume () const
{
   return m_fStrokeVolume;
}

float CPump::StrokeVolume (float sv)
{
   return m_fStrokeVolume = sv;
}

// Call this after after the HwStatus has been copied into our
// HalStatus object and the stroke volume equations have been
// updated.

void CPump::UpdatePump (HalPump_CDS& stat)
{
   // Calculate Accumulated Delta Volume from the delta Revs
   // and current stroke volume per rev
   stat._dV.Set(StrokeVolume() * stat._Revs.Get());

   // calculate the actual flow rate from the provided RPM and
   // current stroke volume
   stat._ActFlow.Set(StrokeVolume() * stat._ActRPM.Get());
}

////////////////////////////////////////////////////////////////
// Inlet Pump

CPumpInlet::CPumpInlet()
   : CPump(MAX_INLET_RPM)
{}

CPumpInlet::~CPumpInlet()
{}

float CPumpInlet::StrokeVolume (float InletP, float CmdFlow)
{
   float tmp;
   tmp = INLET_PUMP_A0 + PUMP_A1 * CmdFlow
         + PUMP_A2 * InletP;


   return CPump::StrokeVolume(tmp);
}

////////////////////////////////////////////////////////////////
// Plasma Pump

CPumpPlasma::CPumpPlasma()
   : CPump(MAX_PLASMA_RPM)
{}

CPumpPlasma::~CPumpPlasma()
{}

float CPumpPlasma::StrokeVolume (float InletP, float CmdFlow)
{
   float tmp = (PLASMA_PUMP_A0 + PUMP_A1 * CmdFlow +
                PUMP_A2 * InletP ) * PLASMA_PUMP_MULT;


   ////////////////////////////////////////////////
   //  Change Stroke volm for MS
   ProcData p;
   if (p.SystemState() == METERED_STORAGE)
   {



      float mss_sv =
         CobeConfig::data().MssPlsSecondOrderConst * CmdFlow * CmdFlow
         + CobeConfig::data().MssPlsLinearConst * CmdFlow
         + CobeConfig::data().MssPlsOffset;

      return CPump::StrokeVolume(mss_sv);

   }
   ////////////////////////////////////////////////

   return CPump::StrokeVolume(tmp);
}

////////////////////////////////////////////////////////////////
// Collect Pump

CPumpCollect::CPumpCollect()
   : CPump(MAX_COLLECT_RPM)
{}

CPumpCollect::~CPumpCollect()
{}

float CPumpCollect::StrokeVolume (float InletP, float CmdFlow)
{
   float tmp = COLLECT_PUMP_A0 + PUMP_A1 * CmdFlow +
               PUMP_A2 * InletP;


   ////////////////////////////////////////////////
   //  Change Stroke volm for MS
   ProcData p;
   if (p.SystemState() == METERED_STORAGE)
   {

      float mss_sv =
         CobeConfig::data().MssPltSecondOrderConst * CmdFlow * CmdFlow
         + CobeConfig::data().MssPltLinearConst * CmdFlow
         + CobeConfig::data().MssPltOffset;


      return CPump::StrokeVolume(mss_sv);

   }
   ////////////////////////////////////////////////


   return CPump::StrokeVolume(tmp);
}

////////////////////////////////////////////////////////////////
// AC Pump

CPumpAC::CPumpAC()
   : CPump(MAX_AC_RPM)
{}

CPumpAC::~CPumpAC()
{}

float CPumpAC::StrokeVolume (float InletP, float CmdFlow)
{

   float dummy = InletP;
/*
 *  The AC pump uses equations developed by Muriel Keller 6/3/97:
 *
 *  SV = Q/RPM = AQ^2 + BQ + C
 *     where A = -4*10^-5
 *           B = .0004
 *           C = .8199
 *
 */

   static const float SECOND_ORDER_CONST = -4.0E-05f;
   static const float LINEAR_CONST       = 0.0004f;
   static const float OFFSET             = 0.8199f;

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

   float              tmp = ( SECOND_ORDER_CONST * CmdFlow * CmdFlow )
                            + ( LINEAR_CONST * CmdFlow )
                            + OFFSET;

   //
   //
   //   The stroke volume equation is validated only for low flow rates.
   //   To avoid problems with runaway pump commands limit the stroke
   //   volume at higher flows (higher flow = smaller SV).
   //
   if (tmp < MINIMUM_AC_STROKE_VOLUME )
   {
      tmp = MINIMUM_AC_STROKE_VOLUME;
   }


   ////////////////////////////////////////////////
   //  Change Stroke volm for MS
   ProcData p;
   if (p.SystemState() == METERED_STORAGE)
   {


      float mss_sv =
         CobeConfig::data().MssACSecondOrderConst * CmdFlow * CmdFlow
         + CobeConfig::data().MssACLinearConst * CmdFlow
         + CobeConfig::data().MssACOffset;

      // max sv
      if (mss_sv < 0.7 )
      {
         mss_sv = 0.7;
      }

      return CPump::StrokeVolume(mss_sv);

   }
   ////////////////////////////////////////////////

   return CPump::StrokeVolume(tmp);
}

////////////////////////////////////////////////////////////////
// Return Pump

CPumpReturn::CPumpReturn()
   : CPump(MAX_RETURN_RPM)
{}

CPumpReturn::~CPumpReturn()
{}

float CPumpReturn::StrokeVolume (float InletP, float CmdFlow)
{
   /*
  *  The return pump uses the same formulation as Spectra:
  *
  *  Simply an empirical constant
  */
   float dummy = InletP = CmdFlow;

   return CPump::StrokeVolume(RETURN_PUMP_STROKE_VOLUME);
}


////////////////////////////////////////////////////////////////
// Return Pump

CPumpReplace::CPumpReplace()
   : CPump(MAX_REPLACEMENT_RPM)
{}

CPumpReplace::~CPumpReplace()
{}

float CPumpReplace::StrokeVolume (float InletP, float CmdFlow)
{
   float dummy = InletP;
/*
 *  The Replacment pump equations are modelled after the AC pump
 *
 */

   static const float SECOND_ORDER_CONST                = -4.0E-05f;
   static const float LINEAR_CONST                      = 0.0004f;
   static const float OFFSET                            = 0.8199f;

   static const float MINIMUM_REPLACEMENT_STROKE_VOLUME = 0.75f;

   float              tmp = ( SECOND_ORDER_CONST * CmdFlow * CmdFlow )
                            + ( LINEAR_CONST * CmdFlow )
                            + OFFSET;

   if (tmp < MINIMUM_REPLACEMENT_STROKE_VOLUME )
   {
      tmp = MINIMUM_REPLACEMENT_STROKE_VOLUME;
   }

   return CPump::StrokeVolume(tmp);
}

//////////////////////////////////////////////////////////////////////
// CPumpSet
bool CPumpSet::pump_stroke_volume_logging     = false;
bool CPumpSet::pump_stroke_volume_logging_set = false;

CPumpSet::CPumpSet()
{
   if (!pump_stroke_volume_logging_set)
   {
      FILE* fp = fopen(PUMP_STROKE_LOGGING_ON, "r");
      if (fp)
      {
         pump_stroke_volume_logging = true;
         fclose(fp);
      }
      else
      {
         pump_stroke_volume_logging = false;
      }
      DataLog(log_level_proc_info) << "pump_stroke_volume_logging: " << pump_stroke_volume_logging << endmsg;

      pump_stroke_volume_logging_set = true;
   }
}

CPumpSet::~CPumpSet()
{}

//////////////////////////////////////////////////////////////////////
// CNormPumpSet
// Platelet, Plasma, Inlet, Return and AC normal pump set

CNormPumpSet::CNormPumpSet()
{}

CNormPumpSet::~CNormPumpSet()
{}

void CNormPumpSet::UpdatePumps (HalStatus_CDS& stat)
{
   // First step is to update the stroke volume equations
   UpdateStrokeVolumes(stat);

   // Update the remaining pump info
   m_pmpInlet.UpdatePump(stat.InletPump);
   m_pmpPlasma.UpdatePump(stat.PlasmaPump);
   m_pmpCollect.UpdatePump(stat.CollectPump);
   m_pmpAC.UpdatePump(stat.ACPump);
   m_pmpReturn.UpdatePump(stat.ReturnPump);

   if (pump_stroke_volume_logging)
   {
      DataLog(log_level_proc_info) << "control_pump_stroke_volume sV (Stroke Volume) CF (Command Flow) "
                                   << "APS: " << stat._aps << " mmHg"
                                   << " Return sV: " << m_pmpReturn.StrokeVolume() << " CF: " << stat.ReturnPump._CmdFlow
                                   << " AC sV: " << m_pmpAC.StrokeVolume() << " CF: " << stat.ACPump._CmdFlow
                                   << " Inlet sV: " << m_pmpInlet.StrokeVolume() << " CF: " << stat.InletPump._CmdFlow
                                   << " Plasma sV: " << m_pmpPlasma.StrokeVolume() << " CF: " << stat.PlasmaPump._CmdFlow
                                   << " Platelet sV: " << m_pmpCollect.StrokeVolume() << " CF: " << stat.CollectPump._CmdFlow
                                   << endmsg;
   }
}

void CNormPumpSet::UpdateStrokeVolumes (HalStatus_CDS& stat)
{
   m_pmpInlet.StrokeVolume(stat._aps, stat.InletPump._CmdFlow);
   m_pmpPlasma.StrokeVolume(PLASMA_PUMP_INLET_PRESSURE, stat.PlasmaPump._CmdFlow);
   m_pmpCollect.StrokeVolume(COLLECT_PUMP_INLET_PRESSURE, stat.CollectPump._CmdFlow);
   m_pmpAC.StrokeVolume(AC_PUMP_INLET_PRESSURE, stat.ACPump._CmdFlow);
   m_pmpReturn.StrokeVolume(0, stat.ReturnPump._CmdFlow);
}

void CNormPumpSet::ObtainRPMs (HalStatus_CDS& stat)
{
   // First step is to update the stroke volume equations
   UpdateStrokeVolumes(stat);

   stat.ACPump._CmdRPM      = m_pmpAC.RPM(stat.ACPump._CmdFlow);
   stat.InletPump._CmdRPM   = m_pmpInlet.RPM(stat.InletPump._CmdFlow);
   stat.ReturnPump._CmdRPM  = m_pmpReturn.RPM(stat.ReturnPump._CmdFlow);
   stat.CollectPump._CmdRPM = m_pmpCollect.RPM(stat.CollectPump._CmdFlow);
   stat.PlasmaPump._CmdRPM  = m_pmpPlasma.RPM(stat.PlasmaPump._CmdFlow);
}

void CNormPumpSet::TransferPumpOrders (const CHwOrders& ctrl, HalOrders& orders)
{
   orders.CmdFlow.Ac      =      m_pmpAC.Flow(ctrl.acRPM);
   orders.CmdFlow.Inlet   =   m_pmpInlet.Flow(ctrl.inletRPM);
   orders.CmdFlow.Return  =  m_pmpReturn.Flow(ctrl.returnRPM);
   orders.CmdFlow.Collect = m_pmpCollect.Flow(ctrl.plateletRPM);
   orders.CmdFlow.Plasma  =  m_pmpPlasma.Flow(ctrl.plasmaRPM);
}

//////////////////////////////////////////////////////////////////////
// CDRBCPumpSet
// Replacement, Plasma, Inlet, Return and AC  pump set

CDRBCPumpSet::CDRBCPumpSet()
{}

CDRBCPumpSet::~CDRBCPumpSet()
{}

void CDRBCPumpSet::UpdatePumps (HalStatus_CDS& stat)
{

   // First step is to update the stroke volume equations
   UpdateStrokeVolumes(stat);

   // Update the remaining pump info
   m_pmpInlet.UpdatePump(stat.InletPump);
   m_pmpPlasma.UpdatePump(stat.PlasmaPump);
   m_pmpReplace.UpdatePump(stat.CollectPump);
   m_pmpAC.UpdatePump(stat.ACPump);
   m_pmpReturn.UpdatePump(stat.ReturnPump);

   if (pump_stroke_volume_logging)
   {
      DataLog(log_level_proc_info) << "control_pump_stroke_volume sV (Stroke Volume) CF (Command Flow) "
                                   << " APS: " << stat._aps << "mmHg"
                                   << " Return sV: " << m_pmpReturn.StrokeVolume() << " CF: " << stat.ReturnPump._CmdFlow
                                   << " AC sV: " << m_pmpAC.StrokeVolume() << " CF: " << stat.ACPump._CmdFlow
                                   << " Inlet sV: " << m_pmpInlet.StrokeVolume() << " CF: " << stat.InletPump._CmdFlow
                                   << " Plasma sV: " << m_pmpPlasma.StrokeVolume() << " CF: " << stat.PlasmaPump._CmdFlow
                                   << " Collect sV: " << m_pmpReplace.StrokeVolume() << " CF: " << stat.CollectPump._CmdFlow
                                   << endmsg;
   }
}

void CDRBCPumpSet::ObtainRPMs (HalStatus_CDS& stat)
{
   // First step is to update the stroke volume equations
   UpdateStrokeVolumes(stat);

   stat.ACPump._CmdRPM      = m_pmpAC.RPM(stat.ACPump._CmdFlow);
   stat.InletPump._CmdRPM   = m_pmpInlet.RPM(stat.InletPump._CmdFlow);
   stat.ReturnPump._CmdRPM  = m_pmpReturn.RPM(stat.ReturnPump._CmdFlow);
   stat.CollectPump._CmdRPM = m_pmpReplace.RPM(stat.CollectPump._CmdFlow);
   stat.PlasmaPump._CmdRPM  = m_pmpPlasma.RPM(stat.PlasmaPump._CmdFlow);

}

void CDRBCPumpSet::TransferPumpOrders (const CHwOrders& ctrl, HalOrders& orders)
{
   orders.CmdFlow.Ac      =       m_pmpAC.Flow(ctrl.acRPM);
   orders.CmdFlow.Inlet   =    m_pmpInlet.Flow(ctrl.inletRPM);
   orders.CmdFlow.Return  =   m_pmpReturn.Flow(ctrl.returnRPM);
   orders.CmdFlow.Collect =  m_pmpReplace.Flow(ctrl.plateletRPM);
   orders.CmdFlow.Plasma  =   m_pmpPlasma.Flow(ctrl.plasmaRPM);
}

void CDRBCPumpSet::UpdateStrokeVolumes (HalStatus_CDS& stat)
{
   m_pmpInlet.StrokeVolume(stat._aps, stat.InletPump._CmdFlow);


   // Plasma pump has variable pressure due to check valves in PTF sets IT8194
   ProcData p;

   // with out autosplit we dont have the checkvalves... so everything is same pressure
   switch ( 0 /*p.Run().rbcCollectionPhase.Get()*/)
   {
      // non-PTF:
      case 0 :
      {
         m_pmpPlasma.StrokeVolume(PLASMA_PUMP_INLET_PRESSURE, stat.PlasmaPump._CmdFlow);
      }
      break;
      // first product
      case 1 :  // collect
      case 3 :  // mss
      {
         m_pmpPlasma.StrokeVolume(PLASMA_PUMP_INLET_PRESSURE_RBC1, stat.PlasmaPump._CmdFlow);
      }
      break;
      // second product
      case 2 :  // collect
      case 4 :  // mss
      {
         m_pmpPlasma.StrokeVolume(PLASMA_PUMP_INLET_PRESSURE_RBC2, stat.PlasmaPump._CmdFlow);
      }
      break;
      default :
      {
         m_pmpPlasma.StrokeVolume(PLASMA_PUMP_INLET_PRESSURE, stat.PlasmaPump._CmdFlow);
      }
      break;
   }


   m_pmpReplace.StrokeVolume(REPLACEMENT_PUMP_INLET_PRESSURE, stat.CollectPump._CmdFlow);
   m_pmpAC.StrokeVolume(AC_PUMP_INLET_PRESSURE, stat.ACPump._CmdFlow);
   m_pmpReturn.StrokeVolume(0, stat.ReturnPump._CmdFlow);
}

/* FORMAT HASH 6ac73dec860f82aca80ff6dd953cadeb */

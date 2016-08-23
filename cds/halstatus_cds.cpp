/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.1x/Trima/cds/rcs/halstatus_cds.cpp 1.3 2002/11/18 18:31:04Z jl11312 Exp rm70006 $
 * $Log: halstatus_cds.cpp $
 * Revision 1.3  2002/11/18 18:31:04Z  jl11312
 * - modifications to improve compile speed, reduce inline function sizes
 * Revision 1.2  2002/10/29 21:17:26  jl11312
 * - removed test code that should not have been checked in
 * Revision 1.1  2002/10/28 22:09:09  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>

#pragma implementation "halstatus_cds.h"
#include "halstatus_cds.h"


HalPump_CDS::HalPump_CDS(const char* name, Role role)
   : SingleWriteDataStore(name, role)
{
   _error.Register(this, NO_PFR, PumpError_NoFault);
   _CmdRPM.Register(this, NO_PFR, 0.0);
   _ActRPM.Register(this, NO_PFR, 0.0);
   _CmdFlow.Register(this, NO_PFR, 0.0);
   _ActFlow.Register(this, NO_PFR, 0.0);
   _Revs.Register(this, NO_PFR, 0.0);
   _dV.Register(this, NO_PFR, 0.0);
   _Accum.Register(this, NO_PFR, 0);
   _Integral.Register(this, NO_PFR, 0);
   _AccumIntegral.Register(this, NO_PFR, 0);
}

HalPump_CDS::~HalPump_CDS()
{}

HalPowerSupply_CDS::HalPowerSupply_CDS(BaseElement<int>& value, int low, int high, int acceptable)
   : _nValue(value), _nLow(low), _nHigh(high), _nAcceptable(acceptable)
{}

HalPowerSupply_CDS::~HalPowerSupply_CDS()
{}

HalValve_CDS::HalValve_CDS(const char* name, Role role)
   : SingleWriteDataStore(name, role)
{
   _order.Register(this, NO_PFR, HAL_VALVE_UNKNOWN);
   _status.Register(this, NO_PFR, HAL_VALVE_UNKNOWN);
}

HalValve_CDS::~HalValve_CDS()
{}

HalStatus_CDS::HalStatus_CDS(Role role)
   : SingleWriteDataStore("HalStatus", role),
     ACPump("ACPumpStatus", role),
     InletPump("InletPumpStatus", role),
     PlasmaPump("PlasmaPumpStatus", role),
     CollectPump("CollectPumpStatus", role),
     ReturnPump("ReturnPumpStatus", role),
     RBCValve("RBCValveStatus", role),
     PlasmaValve("PlasmaValveStatus", role),
     CollectValve("CollectValveStatus", role),
     PS24V(_twentyFourV, HL_LOW_24V_POWER, HL_HIGH_24V_POWER, HL_ACCEPTABLE_24V_POWER),
     PS24Vsw(_twentyFourSwV, HL_LOW_24V_POWER, HL_HIGH_24V_POWER, HL_ACCEPTABLE_24V_POWER),
     PS64V(_sixtyFourV, HL_LOW_64V_POWER, HL_HIGH_64V_POWER, HL_ACCEPTABLE_64V_POWER),
     PS64Vsw(_sixtyFourSwV, HL_LOW_64V_POWER, HL_HIGH_64V_POWER, HL_ACCEPTABLE_64V_POWER),
     PSp12V(_twelveV, HL_LOW_12V_POWER, HL_HIGH_12V_POWER, HL_ACCEPTABLE_12V_POWER),
     PSm12V(_minus12V, HL_LOW_minus12V_POWER, HL_HIGH_minus12V_POWER, HL_ACCEPTABLE_minus12V_POWER),
     PS5V(_plus5V, HL_LOW_5V_POWER, HL_HIGH_5V_POWER, HL_ACCEPTABLE_5V_POWER)
{
   EventFlags flags;
   memset(&flags, 0, sizeof(flags));

   _msgCount.Register(this, NO_PFR, 0);
   _Event.Register(this, NO_PFR, flags);
   _aps.Register(this, NO_PFR, 0.0);
   _apsLow.Register(this, NO_PFR, 0);
   _apsHigh.Register(this, NO_PFR, 0);
   _cps.Register(this, NO_PFR, 0.0);
   _centRPM.Register(this, NO_PFR, 0.0);
   _centCmdRPM.Register(this, NO_PFR, 0.0);
   _acDetect.Register(this, NO_PFR, HAL_ERROR_DETECTED);
   _reservoir.Register(this, NO_PFR, HAL_RESERVOIR_ERROR);
   _lowAGC.Register(this, NO_PFR, 0);
   _highAGC.Register(this, NO_PFR, 0);
   _cassettePos.Register(this, NO_PFR, HAL_CASSETTE_UNKNOWN);
   _cassetteId.Register(this, NO_PFR, cassette::UNKNOWN);
   _doorOrders.Register(this, NO_PFR, HAL_DOORUNLOCK);
   _doorStatus.Register(this, NO_PFR, HAL_DOOR_OPEN);
   _LeakValue.Register(this, NO_PFR, 0);
   _Leak.Register(this, NO_PFR, HAL_ERROR_DETECTED);
   _redValue.Register(this, NO_PFR, 0);
   _redDriveValue.Register(this, NO_PFR, 0);
   _greenValue.Register(this, NO_PFR, 0);
   _greenDriveValue.Register(this, NO_PFR, 0);
   _newDriveCounts.Register(this, NO_PFR, 0);
   _newReflectanceCounts.Register(this, NO_PFR, 0);
   _SoundLevel.Register(this, NO_PFR, 0);
   _centI.Register(this, NO_PFR, 0);
   _plus5V.Register(this, NO_PFR, 0);
   _minus12V.Register(this, NO_PFR, 0);
   _sixtyFourVI.Register(this, NO_PFR, 0);
   _sixtyFourV.Register(this, NO_PFR, 0);
   _sixtyFourSwV.Register(this, NO_PFR, 0);
   _twelveV.Register(this, NO_PFR, 0);
   _twentyFourVI.Register(this, NO_PFR, 0);
   _twentyFourV.Register(this, NO_PFR, 0);
   _twentyFourSwV.Register(this, NO_PFR, 0);
   _EMITemp.Register(this, NO_PFR, 0);
}

HalStatus_CDS::~HalStatus_CDS()
{}



//
// Provide readable dlog output
//
DataLog_Stream& operator << (DataLog_Stream& os, HalPump_CDS::PumpError id)
{
   std::string output;


   if (id & HalPump_CDS::PumpError_NoFault)
   {
      output += " No Fault;";
   }

   if (id & HalPump_CDS::PumpError_FailureToMaintainSpeed)
   {
      output += " Failure To Maintain Speed;";
   }

   if (id & HalPump_CDS::PumpError_IntegrationError)
   {
      output += " Integration Error;";
   }

   if (id & HalPump_CDS::PumpError_VolumeError)
   {
      output += " Volume Error;";
   }

   if (id & HalPump_CDS::PumpError_Disabled)
   {
      output += " Disabled;";
   }

   // Peel off last seperator
   output.erase(output.rfind(";"));

   os << output.c_str() << " (" << (int)id << ")";

   return os;
}

/* FORMAT HASH 55a92957456c8632989b1b924a190c3d */

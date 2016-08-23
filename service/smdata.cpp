/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      smdata.cpp
 *
 */

#include <vxWorks.h>
#include <taskLib.h>

#include "smdata.hpp"
#include "datalog_periodic.h"
#include "failure_debug.h"
#include "error.h"
#include "periodic_log.h"

SMData::SMDataStruct* SMData::_TheData         = 0;
unsigned long         SMData::_TheOwningTaskId = 0;

// #define RETURN_CDS( __cds__ ) if ( _TheData->_##__cds__##DataWriter == this ) return _TheData->_RW##__cds__##Data; else return _TheData->_RO##__cds__##Data
// #define MAKEWRITABLE( __cds__) if ( _TheData->_##__cds__##DataWriter == 0 ) { _TheData->_##__cds__##DataWriter = this; return true; } else return false
// #define MAKEREADONLY( __cds__) if ( _TheData->_##__cds__##DataWriter == this ) _TheData->_##__cds__##DataWriter = 0

SMData::SMDataStruct::SMDataStruct()
   : _ConfigurationData        (ROLE_RO),
     _StatusData               (ROLE_RO),
     _Orders                   (),
     _ROTrimaSetData           (ROLE_RO)
{}

SMData::SMData()
   : AbsModalData(AbsModalData::SMDATA)
{
   if (    _TheOwningTaskId != taskIdSelf()
           && _TheOwningTaskId != 0 )
   {
      //
      // Notify the system of the error ...
      DataLog_Critical criticalLog;
      DataLog(criticalLog) << "SMData object may already in use by taskId->" << taskIdSelf() << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "SMData initialization failed");
      return;
   }

   if ( !_TheData )
   {
      //
      // Create the task wide object
      _TheData         = new SMDataStruct();
      _TheOwningTaskId = taskIdSelf();
   }
}

SMData::~SMData()
{
   // MakeTrimaSetReadable();
}

void SMData::definePeriodicSet (DataLog_SetHandle pset)
{
   if ( m_periodicLoggingEnabled )
   {
      // First, setup some quick references to make are lives easier
      short (BaseElement<short>::* short_func) (void) const = &BaseElement<short>::Get;
      int (BaseElement<int>::* int_func) (void) const       = &BaseElement<int>::Get;
      float (BaseElement<float>::* float_func) (void) const = &BaseElement<float>::Get;
      const char* (string::* string_func)(void)const        = &string::c_str;

      HalStatus_CDS&   hal_status = Status();
      const HalOrders& hal_orders = Orders().cData();

      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertReservoirStatus, hal_status._reservoir,       "ReservoirStatus",  "reservoir status",                     "%c");

      datalog_AddRef(pset, hal_orders.CmdFlow.Inlet,                                          "InletCmd",            "inlet pump command (ml/min)",   "%.1f");
      datalog_AddMemberFunc(pset, &hal_status.InletPump._ActFlow, float_func,         "InletAct",            "inlet pump actual (ml/min)",    "%.1f");

      datalog_AddRef(pset, hal_orders.CmdFlow.Ac,                                             "ACCmd",            "AC pump command (ml/min)",     "%.1f");
      datalog_AddMemberFunc(pset, &hal_status.ACPump._ActFlow, float_func,                "ACAct",            "AC pump actual (ml/min)",      "%.1f");

      datalog_AddRef(pset, hal_orders.CmdFlow.Plasma,                                        "PlasmaCmd",         "plasma pump command (ml/min)",  "%.1f");
      datalog_AddMemberFunc(pset, &hal_status.PlasmaPump._ActFlow, float_func,        "PlasmaAct",        "plasma pump actual (ml/min)",   "%.1f");

      datalog_AddRef(pset, hal_orders.CmdFlow.Collect,                                        "CollectCmd",      "collect pump command (ml/min)",  "%.1f");
      datalog_AddMemberFunc(pset, &hal_status.CollectPump._ActFlow, float_func,       "CollectAct",      "collect pump actual (ml/min)",   "%.1f");

      datalog_AddMemberFunc(pset, &TrimaSet().CmdQch, float_func,                 "ChamberCmd",      "chamber flow command (ml/min)", "%.1f");
      datalog_AddMemberFunc(pset, &TrimaSet().ActQch, float_func,                 "ChamberAct",      "chamber flow actual (ml/min)",  "%.1f");

      datalog_AddRef(pset, hal_orders.CmdFlow.Return,                                        "ReturnCmd",         "return pump command (ml/min)",  "%.1f");
      datalog_AddMemberFunc(pset, &hal_status.ReturnPump._ActFlow, float_func,        "ReturnAct",        "return pump actual (ml/min)",   "%.1f");

      datalog_AddRef(pset, hal_orders.CmdCent.RPM,                                                "CentCmd",          "centrifuge command (RPM)",         "%.0f");
      datalog_AddMemberFunc(pset, &hal_status._centRPM, float_func,                       "CentAct",          "centrifuge actual (RPM)",          "%.0f");

      datalog_AddMemberFunc(pset, &hal_status._redValue, short_func,                      "RedReflectance",       "red reflectance",                      "%d");
      datalog_AddMemberFunc(pset, &hal_status._greenValue, short_func,                    "GreenReflectance",     "green reflectance",                "%d");

      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveStatus, hal_status.CollectValve._status, "CollectValvePos",      "collect valve position",       "%c");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveOrder, hal_orders.CmdValve.collect,          "CollectValveCmd",      "collect valve command",            "%c");

      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveStatus, hal_status.PlasmaValve._status,      "PlasmaValvePos",   "plasma valve position",        "%c");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveOrder, hal_orders.CmdValve.plasma,           "PlasmaValveCmd",   "plasma valve command",             "%c");

      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveStatus, hal_status.RBCValve._status,         "RBCValvePos",          "RBC valve position",               "%c");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertValveOrder, hal_orders.CmdValve.rbc,              "RBCValveCmd",          "RBC valve command",                "%c");

      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCassetteStatus, hal_status._cassettePos,          "CassettePos",          "Cassette position",                "%c");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCassetteCommand, hal_orders.cassettePosition, "CassetteCmd",          "Cassette command",                 "%s");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCassetteID, hal_status._cassetteId,                   "CassetteID",       "Cassette type",                        "%d");


      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCassetteState, TrimaSet().CassetteState,     "CassetteState",         "Cassette State",    "%s");

      datalog_AddMemberFunc(pset, &hal_status._cps, float_func,                           "CPS",                  "CPS (mm Hg)",                      "%.1f");
      datalog_AddMemberFunc(pset, &hal_status._apsLow, short_func,                        "APSLow",               "APS low limit",                        "%d");
      datalog_AddMemberFunc(pset, &hal_status._aps, float_func,                           "APS",                  "APS (mm Hg)",                      "%.1f");
      datalog_AddMemberFunc(pset, &hal_status._apsHigh, short_func,                       "APSHigh",              "APS high limit",                   "%d");

      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertFluidDetected, hal_status._acDetect,          "ACDetected",           "1=fluid 0=air E=error",            "%c");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertFluidDetected, hal_status._Leak,                  "LeakDetected",     "1=fluid 0=air E=error",            "%c");

      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertDoorCommand, hal_orders.doorCmd,                  "DoorCommand",          "1=door commanded locked",          "%d");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertDoorStatus, hal_status._doorStatus,               "DoorStatus",           "O/C=open/close L/U=lock/unlock", "%s");

      datalog_AddMemberFunc(pset, &hal_status._lowAGC, short_func,                        "LowAGC",                   "lower level sensor raw reading",   "%d");
      datalog_AddMemberFunc(pset, &hal_status._highAGC,   short_func,                 "HighAGC",                  "upper level sensor raw reading",   "%d");

      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertTemperature, hal_status._EMITemp,         "EMITemp",              "EMI box temperature (celsius)",    "%.1f");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCentCurrent, hal_status._centI,               "CentI",                    "centrifuge current (amps)",        "%.3f");

      datalog_AddMemberFunc(pset, &hal_status._LeakValue, short_func,             "LeakValue",            "raw leak sensor reading",          "%d");

      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._plus5V,                  "PS+5V",                    "+5 supply (volts)",                    "%.3f");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._minus12V,                "PS-12V",               "-12 supply (volts)",               "%.3f");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._twelveV,                 "PS+12V",               "+12 supply (volts)",               "%.3f");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._twentyFourV,         "PS+24V",               "+24 supply (volts)",               "%.3f");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._twentyFourSwV,           "PS+24V/Switched",  "+24 switched supply (volts)",  "%.3f");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCurrent, hal_status._twentyFourVI,            "PS+24VI",              "+24 supply current (amps)",        "%.3f");

      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._sixtyFourV,              "PS+64V",               "+64 supply (volts)",               "%.3f");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertVoltage, hal_status._sixtyFourSwV,            "PS+64V/Switched",  "+64 switched supply (volts)",  "%.3f");
      datalog_AddArgFunc(pset, &PeriodicLog::FieldConverter::convertCurrent, hal_status._sixtyFourVI,         "PS+64VI",              "+64 supply current (amps)",        "%.3f");
   }
}

// ProcTrimaSet_CDS      &SMData::TrimaSet()        { RETURN_CDS( TrimaSet ); }

// void SMData::MakeTrimaSetReadable()              { MAKEREADONLY( TrimaSet ); }

/* FORMAT HASH 52d2335e194a713752b139c986590c63 */

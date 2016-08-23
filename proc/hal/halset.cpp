/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include "error.h"

#include "halset.h"
#include "trima_datalog.h"
#include "ostime.hpp"
#include "caldat.h"


const int ERROR_FILTER = 10;


CHalSet::CHalSet()
   : m_CentErrorCount(0),
     m_CentIntErrorCount(0),
     m_ValveErrorCount(0),
     m_MotorErrorCount(0),
     m_SendCount(0),
     m_psM12V(PSGN_M12V, PSOF_M12V),
     m_psP5V(PSGN_P5V, PSOF_P5V),
     m_psP12V(PSGN_P12V, PSOF_P12V),
     m_ps24V(PSGN_24V, PSOF_24V),
     m_psSW24V(PSGN_SW24V, PSOF_SW24V),
     m_ps64V(PSGN_64V, PSOF_64V),
     m_psSW64V(PSGN_SW64V, PSOF_SW64V),
     m_statuscds(NULL)
{}

CHalSet::~CHalSet()
{}

void CHalSet::Initialize (HalStatus_CDS* statuscds)
{
   m_statuscds = statuscds;
}

void CHalSet::PerformStatusUpdate (const CHwStates& hw)
{
   // Copy hardware status info to our local status
   // and convert units where necessary.
   TransferStatus(hw);

   // Update the pump stroke volume equations
   UpdatePumps();
}

void CHalSet::PerformAPSReadingUpdate (FASTFILTER& data)
{
   // Store raw value and convert mmHg for APS
   m_statuscds->_aps.Set(m_APS.mmHg(data.raw[0]));
}

void CHalSet::PerformAPSAutoZero (float mmHg)
{
   // Restore initial calibration data
   m_APS.ResetZero();

   // Apply requested offset
   const int rawZero = m_APS.Raw(mmHg);
   m_APS.SetZero(rawZero);
}

CHalSet::HalSetType CHalSet::SetType (void) const
{
   return HST_Unknown;
}

void CHalSet::InitSensors ()
{
   const CalDat::CalDatStruct& Cal = CalDat::data();

   m_APS.SetCal(Cal.APSRawNegative, Cal.APSmmHgNegative, Cal.APSRawReference, Cal.APSmmHgReference, Cal.APSRawPositive, Cal.APSmmHgPositive);
   m_CPS.SetCal(Cal.CPSGain, Cal.CPSOffset);

   datalog_Print(log_handle_proc_info, __FILE__, __LINE__, "APS Neg Raw: %f  mmHg: %f", Cal.APSRawNegative, Cal.APSmmHgNegative);
   datalog_Print(log_handle_proc_info, __FILE__, __LINE__, "APS Ref Raw: %f  mmHg: %f", Cal.APSRawReference, Cal.APSmmHgReference);
   datalog_Print(log_handle_proc_info, __FILE__, __LINE__, "APS Pos Raw: %f  mmHg: %f", Cal.APSRawPositive, Cal.APSmmHgPositive);
   datalog_Print(log_handle_proc_info, __FILE__, __LINE__, "CPS Gain: %f  Offset: %f", Cal.CPSGain, Cal.CPSOffset);
}

// Converts units to put orders into control driver format
// HalOrders ---> CHwOrders
void CHalSet::MakeOrders (const HalOrders& orders, CHwOrders& ctrl)
{
   // Lock the CDS for consistent update
   m_statuscds->Lock();

   // Copy flow commanded rates to HalStatus for future stroke eq.
   m_statuscds->ACPump._CmdFlow.Set(orders.CmdFlow.Ac);
   m_statuscds->InletPump._CmdFlow.Set(orders.CmdFlow.Inlet);
   m_statuscds->PlasmaPump._CmdFlow.Set(orders.CmdFlow.Plasma);
   m_statuscds->ReturnPump._CmdFlow.Set(orders.CmdFlow.Return);
   m_statuscds->CollectPump._CmdFlow.Set(orders.CmdFlow.Collect);
   m_statuscds->_cassetteId.Set(orders.cassetteId);

   // Copy other orders into status message for those who need it.
   m_statuscds->_centCmdRPM.Set(orders.CmdCent.RPM);
   m_statuscds->RBCValve._order.Set(orders.CmdValve.rbc);
   m_statuscds->PlasmaValve._order.Set(orders.CmdValve.plasma);
   m_statuscds->CollectValve._order.Set(orders.CmdValve.collect);
   m_statuscds->_doorOrders.Set(orders.doorCmd);
   m_statuscds->_apsHigh.Set(orders.APSHighLimit);
   m_statuscds->_apsLow.Set(orders.APSLowLimit);

   // Obtain commanded RPM values
   ObtainOrders();

   ctrl.acRPM       = m_statuscds->ACPump._CmdRPM.Get();
   ctrl.inletRPM    = m_statuscds->InletPump._CmdRPM.Get();
   ctrl.plateletRPM = m_statuscds->CollectPump._CmdRPM.Get();
   ctrl.plasmaRPM   = m_statuscds->PlasmaPump._CmdRPM.Get();
   ctrl.returnRPM   = m_statuscds->ReturnPump._CmdRPM.Get();

   // Unlock the CDS for consistent update
   m_statuscds->Unlock();

   //
   // Don't allow centrifuge to start until the door is closed and locked
   //
   if ( orders.CmdCent.RPM <= 0.0 )
   {
      ctrl.centrifugeRPM = orders.CmdCent.RPM;
   }
   else if ( m_statuscds->_doorStatus != HW_DOOR_CLOSED_AND_LOCKED )
   {
      ctrl.centrifugeRPM = 0;
   }
   else
   {
      ctrl.centrifugeRPM          = orders.CmdCent.RPM;
      ctrl.centrifugeRampUpRate   = orders.CmdCent.RampUp;
      ctrl.centrifugeRampDownRate = orders.CmdCent.RampDn;
   }

   // Update valve and cassette position commands
   ctrl.cassettePosition = convertHalCassetteOrder(orders.cassettePosition);
   ctrl.plasmaValve      = convertHalValveOrder(orders.CmdValve.plasma);
   ctrl.plateletValve    = convertHalValveOrder(orders.CmdValve.collect);
   ctrl.rbcValve         = convertHalValveOrder(orders.CmdValve.rbc);

   ctrl.sequenceNumber   = m_SendCount++; // Increment the send message counter
   ctrl.openLoop         = 0;

   // Red blood cell detector drive values
   ctrl.redDrive   = orders.redDrive;
   ctrl.greenDrive = orders.greenDrive;

   // Activate miscellaneous controls
   ctrl.alarmLight = convertHalAlarmOrder(orders.alarmLight);

   // Send the new alarm sound level.
   ctrl.soundLevel = (CHW_SOUND_LEVELS)orders.soundLevel;

   // raw pressure limits
   if ( orders.APSLowLimit == 0 )
   {  // this disables pressure monitoring
      ctrl.lowAPSLimit = 0;
   }
   else
   {
      ctrl.lowAPSLimit = m_APS.Raw(orders.APSLowLimit);
   }

   if ( orders.APSHighLimit == 0 )
   {  // this disables pressure monitoring
      ctrl.highAPSLimit = 0;
   }
   else
   {
      ctrl.highAPSLimit = m_APS.Raw(orders.APSHighLimit);
   }

   // Door command
   ctrl.doorDirection = convertHalDoorOrder(orders.doorCmd);
   ctrl.doorLatches   = HW_ENABLE;

   // Snapshot order creation time to debug soft watchdog errors
   osTime::snapshotRawTime(ctrl.timeOrdersCreated);
}



// Converts units to put orders into control driver formatted
// orders into HAL orders format
// CHwOrders ---> HalOrders
void CHalSet::MakeOrders (const CHwOrders& ctrl, HalOrders& orders)
{
   // Convert RPM commands to flow rates
   TransferPumpOrders(ctrl, orders);
   m_statuscds->ACPump._CmdFlow      = orders.CmdFlow.Ac;
   m_statuscds->InletPump._CmdFlow   = orders.CmdFlow.Inlet;
   m_statuscds->PlasmaPump._CmdFlow  = orders.CmdFlow.Plasma;
   m_statuscds->ReturnPump._CmdFlow  = orders.CmdFlow.Return;
   m_statuscds->CollectPump._CmdFlow = orders.CmdFlow.Collect;

   // Store centrifuge speed
   m_statuscds->_centCmdRPM = ctrl.centrifugeRPM;

   // Store valve commanded positions
   m_statuscds->RBCValve._order     = convertDriverValveOrder(ctrl.rbcValve);
   m_statuscds->PlasmaValve._order  = convertDriverValveOrder(ctrl.plasmaValve);
   m_statuscds->CollectValve._order = convertDriverValveOrder(ctrl.plateletValve);

   // Store addition values
   m_statuscds->_doorOrders = convertDriverDoorOrder(ctrl.doorDirection);
   m_statuscds->_apsHigh    = ctrl.highAPSLimit;
   m_statuscds->_apsLow     = ctrl.lowAPSLimit;
   orders.cassetteId        = m_statuscds->_cassetteId;
   orders.cassettePosition  = convertDriverCassetteOrder(ctrl.cassettePosition);

   // Copy value positions
   orders.CmdValve.rbc     = m_statuscds->RBCValve._order;
   orders.CmdValve.plasma  = m_statuscds->PlasmaValve._order;
   orders.CmdValve.collect = m_statuscds->CollectValve._order;

   // Red blood cell detector drive values
   orders.redDrive   = ctrl.redDrive;
   orders.greenDrive = ctrl.greenDrive;

   // Activate miscellaneous controls
   orders.alarmLight = convertDriverAlarmOrder(ctrl.alarmLight);
   orders.soundLevel = (short)ctrl.soundLevel;

   // Centrifuge speed and ramps
   orders.CmdCent.RPM    = m_statuscds->_centCmdRPM;
   orders.CmdCent.RampUp = ctrl.centrifugeRampUpRate;
   orders.CmdCent.RampDn = ctrl.centrifugeRampDownRate;

   // Raw pressure limits
   orders.APSLowLimit  = (short)m_APS.mmHg(m_statuscds->_apsLow);
   orders.APSHighLimit = (short)m_APS.mmHg(m_statuscds->_apsHigh);

   // Door command
   orders.doorCmd = convertDriverDoorOrder(ctrl.doorDirection);
}

// This will transfer all hardware status info to our local
// format except for pump info.
void CHalSet::TransferStatus (const CHwStates& hw)
{
   // send status
   struct timespec now; // time msg sent
   clock_gettime(CLOCK_REALTIME, &now);

   const long deltaTime = ( (now.tv_sec - hw.timestamp.tv_sec) * 1000) +     // sec to ms
                          ( (now.tv_nsec - hw.timestamp.tv_nsec) / 1000000); // nsec to ms

   //
   // Check to see if there is a back-up from the message being generated by the control driver
   // to being sent by the ctl_msg task
   //
   // If the delay exceed 750 ms, log it.
   //
   if ( (deltaTime > 750) || (deltaTime < 0) )
   {
      DataLog(log_level_proc_error) << "control driver status messages are backing up. deltaTime=" << deltaTime << " ms" << endmsg;
   }

   // Update header
   m_statuscds->_msgCount = hw.msgCount;

   // Store raw value and convert mmHg for APS
   m_statuscds->_aps = m_APS.mmHg(hw.accessPressure);

   // Centrifuge info
   // Store raw value and convert mmHg for CPS
   m_statuscds->_cps     = m_CPS.mmHg(hw.centrifugePressure);
   m_statuscds->_centRPM = hw.centrifugeRPM;

   if ( hw.centrifugeError & HW_CENT_HARDWARE_ERROR ||
        hw.centrifugeError & HW_CENT_REVERSE_BIT )
   {
      if ( m_CentErrorCount < ( 2 * ERROR_FILTER) )
      {
         m_CentErrorCount++;
      }
   }
   else
   {
      if ( m_CentErrorCount > 0 )
      {
         m_CentErrorCount--;
      }
   }


   if ( hw.centrifugeError & HW_CENT_INTEGRAL_ERROR )
   {
      if ( m_CentIntErrorCount < ( 2 * ERROR_FILTER) )
      {
         m_CentIntErrorCount++;
      }
   }
   else
   {
      if ( m_CentIntErrorCount > 0 )
      {
         m_CentIntErrorCount--;
      }
   }

   m_statuscds->_acDetect  = convertDriverACState(hw.acDetector);
   m_statuscds->_reservoir = convertDriverReservoirState(hw.reservoir);
   m_statuscds->_lowAGC    = hw.lowAGC;
   m_statuscds->_highAGC   = hw.highAGC;

   // Transfer Valve position information
   m_statuscds->RBCValve._status     = convertDriverValveState(hw.rbcValve);
   m_statuscds->PlasmaValve._status  = convertDriverValveState(hw.plasmaValve);
   m_statuscds->CollectValve._status = convertDriverValveState(hw.plateletValve);

   if ( hw.valveFault )
   {
      if ( m_ValveErrorCount < ERROR_FILTER )
      {
         m_ValveErrorCount++;
      }
   }
   else
   {
      if ( m_ValveErrorCount > 0 )
      {
         m_ValveErrorCount--;
      }
   }


   if ( hw.motorFault )
   {
      if ( m_MotorErrorCount < ERROR_FILTER )
      {
         m_MotorErrorCount++;
      }
   }
   else
   {
      if ( m_MotorErrorCount > 0 )
      {
         m_MotorErrorCount--;
      }
   }

   // Transfer cassette info
   m_statuscds->_cassettePos = convertDriverCassettePosition(hw.cassettePosition);

   // Door and basin status
   m_statuscds->_doorStatus = convertDriverDoorStatus(hw.doorLocks);
   m_statuscds->_LeakValue  = hw.leakDetector;

   if ( m_statuscds->_LeakValue > LEAK_FAULT_VALUE )
      m_statuscds->_Leak = HAL_ERROR_DETECTED;
   else if ( m_statuscds->_LeakValue >= NO_LEAK_VALUE )
      m_statuscds->_Leak = HAL_AIR_DETECTED;
   else
      m_statuscds->_Leak = HAL_FLUID_DETECTED;

   // system power status
   m_statuscds->_centI.Set(hw.centrifugeCurrent);
   m_statuscds->_twentyFourVI.Set(hw.twentyFourVoltCurrent);
   m_statuscds->_minus12V.Set((int)m_psM12V.mVolts(hw.minusTwelvePS));
   m_statuscds->_twelveV.Set((int)m_psP12V.mVolts(hw.twelveVoltSupply));
   m_statuscds->_plus5V.Set((int)m_psP5V.mVolts(hw.fiveVoltSupply));
   m_statuscds->_sixtyFourV.Set((int)m_ps64V.mVolts(hw.sixtyFourVoltSupply));
   m_statuscds->_sixtyFourVI.Set(hw.sixtyFourVoltCurrent);
   m_statuscds->_sixtyFourSwV.Set((int)m_psSW64V.mVolts(hw.sixtyFourVoltSwitched));
   m_statuscds->_twentyFourV.Set((int)m_ps24V.mVolts(hw.twentyFourVoltSupply));
   m_statuscds->_twentyFourSwV.Set((int)m_psSW24V.mVolts(hw.twentyFourVoltSwitched));
   m_statuscds->_EMITemp.Set(hw.emiTemperature);

   // RBC Detector values
   m_statuscds->_redValue             = hw.red;
   m_statuscds->_greenValue           = hw.green;
   m_statuscds->_newReflectanceCounts = hw.newReflectanceCounts;
   m_statuscds->_newDriveCounts       = hw.newDriveCounts;

   // sound level
   m_statuscds->_SoundLevel = (short)hw.soundLevel;

   // Transfer the Pump Status Info from HwStatus
   TransferPumpStatus(m_statuscds->ACPump, hw.acPump);
   TransferPumpStatus(m_statuscds->CollectPump, hw.plateletPump);
   TransferPumpStatus(m_statuscds->InletPump, hw.inletPump);
   TransferPumpStatus(m_statuscds->PlasmaPump, hw.plasmaPump);
   TransferPumpStatus(m_statuscds->ReturnPump, hw.returnPump);

   EventFlags event = m_statuscds->_Event.Get();
   event.Update           = convertDriverEvent(hw.event);
   event.CentErr          = (m_CentErrorCount > ERROR_FILTER) ? 1 : 0;
   event.CentIntErr       = ( m_CentIntErrorCount > ERROR_FILTER ) ? 1 : 0;
   event.Vibration        = ( hw.vibration ) ? 1 : 0;
   event.ValveFault       = ( m_ValveErrorCount > ( ERROR_FILTER / 2 ) ) ? 1 : 0;
   event.Cassette         =  ( hw.cassetteError ) ? 1 : 0;
   event.MotorErr         = ( m_MotorErrorCount > ( ERROR_FILTER / 2 ) ) ? 1 : 0;
   event.SwPause          = ( hw.pauseSwitch ) ? 1 : 0;
   event.SwStop           = ( hw.stopSwitch ) ? 1 : 0;
   event.CentErrNoReverse = ( hw.centrifugeError & (HW_CENT_HARDWARE_ERROR ) ) ? 1 : 0;

   m_statuscds->_Event.Set(event);
}

void CHalSet::TransferPumpStatus (HalPump_CDS& pmp, const PumpStatus& hw)
{
   pmp._Accum         = hw.Accum;
   pmp._ActRPM        = hw.RPM;
   pmp._AccumIntegral = hw.AcummIntegral;
   pmp._error         = (HalPump_CDS::PumpError)hw.errors;
   pmp._Integral      = hw.Integral;
   pmp._Revs          = hw.Revs;
}

CHW_CASSETTE_ORDERS CHalSet::convertHalCassetteOrder (HAL_CASSETTEORDERS order)
{
   CHW_CASSETTE_ORDERS result = CHW_CASSETTE_DONTMOVE;

   switch ( order )
   {
      case HAL_NOCHANGE :
         result = CHW_CASSETTE_DONTMOVE;
         break;

      case HAL_CASSETTEDOWN :
         result = CHW_CASSETTE_DOWN;
         break;

      case HAL_CASSETTEUP :
         result = CHW_CASSETTE_UP;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "cassette order=" << (int)order << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad cassette order");
      }
   }

   return result;
}

CHW_VALVE_ORDERS CHalSet::convertHalValveOrder (HAL_VALVESTATE order)
{
   CHW_VALVE_ORDERS result = (CHW_VALVE_ORDERS)0;

   switch ( order )
   {
      case HAL_VALVE_UNKNOWN :
         result = (CHW_VALVE_ORDERS)0;
         break;

      case HAL_VALVE_COLLECT :
         result = CHW_VALVE_COLLECT;
         break;

      case HAL_VALVE_OPEN :
         result = CHW_VALVE_OPEN;
         break;

      case HAL_VALVE_RETURN :
         result = CHW_VALVE_RETURN;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "valve order=" << (int)order << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad valve order");
      }
   }

   return result;
}

HW_ORDERS CHalSet::convertHalAlarmOrder (HAL_ALARMLIGHTORDERS order)
{
   HW_ORDERS result = HW_ENABLE;

   switch ( order )
   {
      case HAL_ALARMLIGHTON :
         result = HW_ENABLE;
         break;

      case HAL_ALARMLIGHTOFF :
         result = HW_DISABLE;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "light order=" << (int)order << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad alarm light order");
      }
   }

   return result;
}

CHW_DOOR_ORDERS CHalSet::convertHalDoorOrder (HAL_DOORORDERS order)
{
   CHW_DOOR_ORDERS result = CHW_DOOR_LOCK;

   switch ( order )
   {
      case HAL_DOORUNLOCK :
         result = CHW_DOOR_UNLOCK;
         break;

      case HAL_DOORLOCK :
         result = CHW_DOOR_LOCK;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "door lock order=" << (int)order << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad door lock order");
      }
   }

   return result;
}

//
// For CHwOrders ---> HalOrders conversion
//
HAL_CASSETTEORDERS CHalSet::convertDriverCassetteOrder (CHW_CASSETTE_ORDERS order)
{
   HAL_CASSETTEORDERS result = HAL_NOCHANGE;

   switch ( order )
   {
      case CHW_CASSETTE_DONTMOVE :
         result = HAL_NOCHANGE;
         break;

      case CHW_CASSETTE_DOWN :
         result = HAL_CASSETTEDOWN;
         break;

      case CHW_CASSETTE_UP :
         result = HAL_CASSETTEUP;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "cassette order=" << (int)order << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad cassette order");
      }
   }

   return result;
}

HAL_VALVESTATE CHalSet::convertDriverValveOrder (CHW_VALVE_ORDERS order)
{
   HAL_VALVESTATE result = (HAL_VALVESTATE)0;

   switch ( order )
   {
      case (CHW_VALVE_ORDERS)0 :
         result = HAL_VALVE_UNKNOWN;
         break;

      case CHW_VALVE_COLLECT :
         result = HAL_VALVE_COLLECT;
         break;

      case CHW_VALVE_OPEN :
         result = HAL_VALVE_OPEN;
         break;

      case CHW_VALVE_RETURN :
         result = HAL_VALVE_RETURN;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "valve order=" << (int)order << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad valve order");
      }
   }

   return result;
}

HAL_ALARMLIGHTORDERS CHalSet::convertDriverAlarmOrder (HW_ORDERS order)
{
   HAL_ALARMLIGHTORDERS result = HAL_ALARMLIGHTON;

   switch ( order )
   {
      case HW_ENABLE :
         result = HAL_ALARMLIGHTON;
         break;

      case HW_DISABLE :
         result = HAL_ALARMLIGHTOFF;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "light order=" << (int)order << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad alarm light order");
      }
   }

   return result;
}

HAL_DOORORDERS CHalSet::convertDriverDoorOrder (CHW_DOOR_ORDERS order)
{
   HAL_DOORORDERS result = HAL_DOORLOCK;

   switch ( order )
   {
      case CHW_DOOR_UNLOCK :
         result = HAL_DOORUNLOCK;
         break;

      case CHW_DOOR_LOCK :
         result = HAL_DOORLOCK;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "door lock order=" << (int)order << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad door lock order");
      }
   }

   return result;
}

HAL_FLUIDDETECT CHalSet::convertDriverACState (CHW_AC_DETECTOR_STATES state)
{
   HAL_FLUIDDETECT result = HAL_AIR_DETECTED;

   switch ( state )
   {
      case CHW_AC_DETECTOR_FLUID :
         result = HAL_FLUID_DETECTED;
         break;

      case CHW_AC_DETECTOR_AIR :
         result = HAL_AIR_DETECTED;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "AC state=" << (int)state << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad AC state");
      }
   }

   return result;
}

HAL_RESERVOIRSTATE CHalSet::convertDriverReservoirState (CHW_RESERVOIR_STATES state)
{
   HAL_RESERVOIRSTATE result = HAL_RESERVOIR_EMPTY;
   switch ( state )
   {
      case CHW_RESERVOIR_ERROR :
         result = HAL_RESERVOIR_ERROR;
         break;

      case CHW_RESERVOIR_EMPTY :
         result = HAL_RESERVOIR_EMPTY;
         break;

      case CHW_RESERVOIR_MIDDLE :
         result = HAL_RESERVOIR_MIDDLE;
         break;

      case CHW_RESERVOIR_HIGH :
         result = HAL_RESERVOIR_HIGH;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "Reservoir state=" << (int)state << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad reservoir state");
      }
   }

   return result;
}

HAL_VALVESTATE CHalSet::convertDriverValveState (HW_VALVE_STATES state)
{
   HAL_VALVESTATE result = HAL_VALVE_UNKNOWN;

   switch ( state )
   {
      case HW_VALVE_UNKNOWN :
         result = HAL_VALVE_UNKNOWN;
         break;

      case HW_VALVE_COLLECT :
         result = HAL_VALVE_COLLECT;
         break;

      case HW_VALVE_OPEN :
         result = HAL_VALVE_OPEN;
         break;

      case HW_VALVE_RETURN :
         result = HAL_VALVE_RETURN;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "Valve state=" << (int)state << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad valve state");
      }
   }

   return result;
}

HAL_CASSETTESTATE CHalSet::convertDriverCassettePosition (HW_CASSETTE_STATES state)
{
   HAL_CASSETTESTATE result = HAL_CASSETTE_UNKNOWN;

   switch ( state )
   {
      case HW_CASSETTE_UNKNOWN :
         result = HAL_CASSETTE_UNKNOWN;
         break;

      case HW_CASSETTE_DOWN :
         result = HAL_CASSETTE_DOWN;
         break;

      case HW_CASSETTE_UP :
         result = HAL_CASSETTE_UP;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "Cassette position=" << (int)state << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad cassette position");
      }
   }

   return result;
}

HAL_DOORSTATE CHalSet::convertDriverDoorStatus (HW_DOOR_STATES state)
{
   HAL_DOORSTATE result = HAL_DOOR_OPEN;

   switch ( state )
   {
      case HW_DOOR_OPEN :
         result = HAL_DOOR_OPEN;
         break;

      case HW_DOOR_LOCKED_STATE :
         result = HAL_DOOR_LOCKED_STATE;
         break;

      case HW_DOOR_CLOSED_STATE :
         result = HAL_DOOR_CLOSED_STATE;
         break;

      case HW_DOOR_CLOSED_AND_LOCKED :
         result = HAL_DOOR_CLOSED_AND_LOCKED;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "Door status=" << (int)state << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad door status");
      }
   }

   return result;
}

int CHalSet::convertDriverEvent (CHW_EVENTS event)
{
   int result = (int)HAL_NORMAL_UPDATE;

   switch ( event )
   {
      case CHW_NORMAL_UPDATE :
         result = (int)HAL_NORMAL_UPDATE;
         break;

      case CHW_AC_EVENT :
         result = (int)HAL_AC_EVENT;
         break;

      case CHW_DOOR_EVENT :
         result = (int)HAL_DOOR_EVENT;
         break;

      case CHW_PAUSE_EVENT :
         result = (int)HAL_PAUSE_EVENT;
         break;

      case CHW_STOP_EVENT :
         result = (int)HAL_STOP_EVENT;
         break;

      case CHW_RESERVOIR_EVENT :
         result = (int)HAL_RESERVOIR_EVENT;
         break;

      default :
      {
         DataLog_Critical critical;
         DataLog(critical) << "event type=" << (int)event << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Bad event type");
      }
   }

   return result;
}

///////////////////////////////////////////////////////////////
// CHalNormSet

CHalNormSet::CHalNormSet()
{}

CHalNormSet::~CHalNormSet()
{}

void CHalNormSet::UpdatePumps (void)
{
   m_pmp.UpdatePumps(*m_statuscds);
}

void CHalNormSet::ObtainOrders (void)
{
   m_pmp.ObtainRPMs(*m_statuscds);
}

void CHalNormSet::TransferPumpOrders (const CHwOrders& ctrl, HalOrders& orders)
{
   m_pmp.TransferPumpOrders(ctrl, orders);
}

CHalSet::HalSetType CHalNormSet::SetType (void) const
{
   return HST_Normal;
}


///////////////////////////////////////////////////////////////
// CHalDRBCSet

CHalDRBCSet::CHalDRBCSet()
{}

CHalDRBCSet::~CHalDRBCSet()
{}

void CHalDRBCSet::UpdatePumps (void)
{
   m_pmp.UpdatePumps(*m_statuscds);
}

void CHalDRBCSet::ObtainOrders (void)
{
   m_pmp.ObtainRPMs(*m_statuscds);
}

void CHalDRBCSet::TransferPumpOrders (const CHwOrders& ctrl, HalOrders& orders)
{
   m_pmp.TransferPumpOrders(ctrl, orders);
}

CHalSet::HalSetType CHalDRBCSet::SetType (void) const
{
   return HST_DRBC;
}

/* FORMAT HASH 48d32da1b58ce40f2eebd4b439909d91 */

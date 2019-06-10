/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/proc/interface/rcs/halorders.cpp 1.4 2006/03/03 17:29:12Z jheiusb Exp $
 * $Log: halorders.cpp $
 * Revision 1.4  2006/03/03 17:29:12Z  jheiusb
 * initial merge
 * Revision 1.3  2003/11/14 20:19:44Z  ms10234
 * 5316 - rework centrifuge test and door alarm logic
 * Revision 1.2  2002/11/18 22:16:10Z  ms10234
 * Initialized data to 0
 * Revision 1.1  2002/10/28 22:17:41Z  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>
#include <math.h>
#include "halorders.h"
#include "procdata.h"
#include "cobeconfig.h"

/////////////////////////////////////////////////////////
// CHalOrders
// This object is generally only used by the main Proc task.
// After creation, each value should be set to a desired default
// state.

CHalOrders::CHalOrders(void)
   : m_override(false),
     m_safetyReturnCheck(false) 
{
   memset(&m_Orders, 0, sizeof( HalOrders ) );
}

CHalOrders::~CHalOrders()
{}

void CHalOrders::ResetOrders (void)
{
   CassetteCmd(HAL_NOCHANGE);
}

HAL_CASSETTEORDERS CHalOrders::CassetteCmd (void) const
{
   return m_Orders.cassettePosition;
}

HAL_CASSETTEORDERS CHalOrders::CassetteCmd (HAL_CASSETTEORDERS cmd)
{
   return m_Orders.cassettePosition = cmd;
}

cassette::cassetteType CHalOrders::CassetteId (void) const
{
   return m_Orders.cassetteId;
}

cassette::cassetteType CHalOrders::CassetteId (cassette::cassetteType id)
{
   return m_Orders.cassetteId = id;
}

float CHalOrders::Qac (void) const
{
   return m_Orders.CmdFlow.Ac;
}

float CHalOrders::Qac (float flw)
{
   ProcData pd;
   if (pd.SystemState() == METERED_STORAGE)
      pd.LogMSSPumps(true);
   else pd.LogMSSPumps(false);

   return m_Orders.CmdFlow.Ac = flw;
}

float CHalOrders::Qin (void) const
{
   return m_Orders.CmdFlow.Inlet;
}

float CHalOrders::Qin (float flw)
{

   ProcData pd;
   if (pd.SystemState() == METERED_STORAGE)
      pd.LogMSSPumps(true);
   else pd.LogMSSPumps(false);

   return m_Orders.CmdFlow.Inlet = flw;
}

float CHalOrders::Qrp (void) const
{
   return m_Orders.CmdFlow.Return;
}

float CHalOrders::Qrp (float flw)
{
   ProcData pd;
   if (pd.SystemState() == METERED_STORAGE)
      pd.LogMSSPumps(true);
   else pd.LogMSSPumps(false);

   if ( m_safetyReturnCheck )
   {
      if ( !pd.Status().ReservoirIsEmpty() && 
            (pd.SafetyStatus().reservoir != SHW_RESERVOIR_EMPTY)
      )
      {   
         return m_Orders.CmdFlow.Return = flw;
      } 
      else 
      {
         if (flw < 0.0f)  // negative direction blood prime excusion
         {
            return m_Orders.CmdFlow.Return = flw;
         }
         else
         {
            return m_Orders.CmdFlow.Return = 0.0f;
         }
      }
   } 
   else 
   {
      return m_Orders.CmdFlow.Return = flw;
   }
}

float CHalOrders::Qplasma (void) const
{
   return m_Orders.CmdFlow.Plasma;
}

float CHalOrders::Qplasma (float flw)
{

   ProcData pd;
   if (pd.SystemState() == METERED_STORAGE)
      pd.LogMSSPumps(true);
   else pd.LogMSSPumps(false);

   return m_Orders.CmdFlow.Plasma = flw;
}

float CHalOrders::Qcollect (void) const
{
   return m_Orders.CmdFlow.Collect;
}

float CHalOrders::Qcollect (float flw)
{
   ProcData pd;
   if (pd.SystemState() == METERED_STORAGE)
      pd.LogMSSPumps(true);
   else pd.LogMSSPumps(false);

   return m_Orders.CmdFlow.Collect = flw;
}

float CHalOrders::RPM (void) const
{
   return m_Orders.CmdCent.RPM;
}

float CHalOrders::RPM (float rpm)
{
   // if override exists, do not change
   // the speed
   if ( m_override )
      return m_Orders.CmdCent.RPM;
   return m_Orders.CmdCent.RPM = rpm;
}

float CHalOrders::RPM (float rpm, bool overrideCmd)
{
   m_override                  = overrideCmd;
   return m_Orders.CmdCent.RPM = rpm;
}

void CHalOrders::RPMClearOverride ()
{
   m_override = false;
}

float CHalOrders::getScaledRPM ()
{
   ProcData    pd;
   State_names substate = pd.Run().Substate.Get();
   float       newRPM   = pd.Predict().Rpm(substate);
   if (newRPM > CobeConfig::data().RPMCollect)
      newRPM = CobeConfig::data().RPMCollect;
   return newRPM;
}

float CHalOrders::scaleRPM ()
{
   ProcData pd;
   float    newRPM = getScaledRPM();
   pd.Orders().RPM(newRPM);
   return newRPM;
}

float CHalOrders::RampUp (void) const
{
   return m_Orders.CmdCent.RampUp;
}

float CHalOrders::RampUp (float rp)
{
   return m_Orders.CmdCent.RampUp = rp;
}


float CHalOrders::RampDn (void) const
{
   return m_Orders.CmdCent.RampDn;
}

float CHalOrders::RampDn (float rp)
{
   return m_Orders.CmdCent.RampDn = rp;
}

HAL_ALARMLIGHTORDERS CHalOrders::AlarmLight (void) const
{
   return m_Orders.alarmLight;
}

HAL_ALARMLIGHTORDERS CHalOrders::AlarmLight (HAL_ALARMLIGHTORDERS order)
{
   m_Orders.alarmLight = order;
   return m_Orders.alarmLight;
}

HAL_LEAKTESTORDERS CHalOrders::LeakTest (void) const
{
   return m_Orders.leakTest;
}

HAL_LEAKTESTORDERS CHalOrders::LeakTest (HAL_LEAKTESTORDERS order)
{
   m_Orders.leakTest = order;
   return m_Orders.leakTest;
}

void CHalOrders::LockDoor (void)
{
   m_Orders.doorCmd = HAL_DOORLOCK;
}

void CHalOrders::UnlockDoor (void)
{
   m_Orders.doorCmd = HAL_DOORUNLOCK;
}

bool CHalOrders::DoorCommandedToLock (void) const
{
   return (m_Orders.doorCmd == HAL_DOORLOCK);
}


HAL_VALVESTATE CHalOrders::ValveCmd (HAL_VALVEID id) const
{
   HAL_VALVESTATE tmp;

   switch ( id )
   {
      case HAL_VRBC :
         tmp = m_Orders.CmdValve.rbc;
         break;
      case HAL_VPLASMA :
         tmp = m_Orders.CmdValve.plasma;
         break;
      case HAL_VCOLLECT :
         tmp = m_Orders.CmdValve.collect;
         break;
      default :
         tmp = HAL_VALVE_UNKNOWN;
         break;
   }
   return tmp;
}

HAL_VALVESTATE CHalOrders::ValveCmd (HAL_VALVEID id, HAL_VALVESTATE cmd)
{
   HAL_VALVESTATE tmp;

   switch ( id )
   {
      case HAL_VRBC :
         tmp = m_Orders.CmdValve.rbc = cmd;
         break;
      case HAL_VPLASMA :
         tmp = m_Orders.CmdValve.plasma = cmd;
         break;
      case HAL_VCOLLECT :
         tmp = m_Orders.CmdValve.collect = cmd;
         break;
      default :
         tmp = HAL_VALVE_UNKNOWN;
         break;
   }
   return tmp;
}

unsigned char CHalOrders::RBCRed (void) const
{
   return m_Orders.redDrive;
}

unsigned char CHalOrders::RBCRed (unsigned char val)
{
   return m_Orders.redDrive = val;
}

unsigned char CHalOrders::RBCGreen () const
{
   return m_Orders.greenDrive;
}

unsigned char CHalOrders::RBCGreen (unsigned char val)
{
   return m_Orders.greenDrive = val;
}

short CHalOrders::SoundLevel () const
{
   return m_Orders.soundLevel;
}

short CHalOrders::SoundLevel (short lv)
{
   return m_Orders.soundLevel = lv;
}

float CHalOrders::APSLow (void) const
{
   return m_Orders.APSLowLimit;
}

float CHalOrders::APSLow (float v)
{
   return m_Orders.APSLowLimit = (short)v;
}

float CHalOrders::APSHigh (void) const
{
   return m_Orders.APSHighLimit;
}

float CHalOrders::APSHigh (float v)
{
   return m_Orders.APSHighLimit = (short)v;
}

bool CHalOrders::APSLowDisabled (void) const
{
   return (m_Orders.APSLowLimit == 0);
}

bool CHalOrders::APSHighDisabled (void) const
{
   return (m_Orders.APSHighLimit == 0);
}

void CHalOrders::safeReturnCommands (bool on)
{
   m_safetyReturnCheck = on;
}

/* FORMAT HASH 5817a88c590f1666acda1ac2433f135f */

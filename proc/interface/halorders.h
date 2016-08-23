/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/proc/interface/rcs/halorders.h 1.2 2003/11/14 20:19:47Z ms10234 Exp $
 * $Log: halorders.h $
 * Revision 1.2  2003/11/14 20:19:47Z  ms10234
 * 5316 - rework centrifuge test and door alarm logic
 * Revision 1.1  2002/10/28 22:17:41Z  jl11312
 * Initial revision
 *
 */

#ifndef _HALORDERS_INCLUDE
#define _HALORDERS_INCLUDE

#include "haldata.h"
#include "cassette.h"
#include "message.h"

////////////////////////////////////////////////
// The Status Message
class CHalOrders
{
public:
   CHalOrders(void);
   virtual ~CHalOrders();

   // reset orders that should not be persistent
   void ResetOrders (void);

   // Cassette Commands
   HAL_CASSETTEORDERS CassetteCmd (void) const;
   HAL_CASSETTEORDERS CassetteCmd (HAL_CASSETTEORDERS cmd);

   cassette::cassetteType CassetteId (void) const;
   cassette::cassetteType CassetteId (cassette::cassetteType id);

   // pump flows in ml/min
   float Qac (void) const;
   float Qac (float flow);
   float Qin (void) const;
   float Qin (float flow);
   float Qrp (void) const;
   float Qrp (float flow);
   float Qplasma (void) const;
   float Qplasma (float flow);
   float Qcollect (void) const;
   float Qcollect (float flow);

   // Centrifuge
   // Ramp rates are in RPM per second when above 1000RPM
   float RPM (void) const;
   float RPM (float rpm);
   float RPM (float rpm, bool overrideCmd);
   void  RPMClearOverride ();

   // Scale the centrifuge speed to qin.
   float scaleRPM ();
   // Just get the scaled value, don't actual command centrifuge
   float getScaledRPM ();

   float RampUp (void) const;
   float RampUp (float rate);
   float RampDn (void) const;
   float RampDn (float rate);

   // alarm light
   HAL_ALARMLIGHTORDERS AlarmLight (void) const;
   HAL_ALARMLIGHTORDERS AlarmLight (HAL_ALARMLIGHTORDERS oder);

   // leak test hardware
   HAL_LEAKTESTORDERS LeakTest (void) const;
   HAL_LEAKTESTORDERS LeakTest (HAL_LEAKTESTORDERS order);

   // Door
   void LockDoor (void);
   void UnlockDoor (void);
   bool DoorCommandedToLock (void) const;

   // Valve commands
   HAL_VALVESTATE ValveCmd (HAL_VALVEID id) const;
   HAL_VALVESTATE ValveCmd (HAL_VALVEID id, HAL_VALVESTATE cmd);

   // rbc detector drive signal
   unsigned char RBCRed (void) const;
   unsigned char RBCRed (unsigned char val);
   unsigned char RBCGreen (void) const;
   unsigned char RBCGreen (unsigned char grn);

   // sound
   short SoundLevel (void) const;
   short SoundLevel (short lv);

   // Pressure thresholds (mmHg)
   // NB: if APSLow = 0, this will disable monitoring.
   // NB: if APSHigh = 0, this will disable monitoring.
   float APSLow (void) const;
   float APSLow (float lw);
   float APSHigh (void) const;
   float APSHigh (float hg);
   bool  APSLowDisabled (void) const;
   bool  APSHighDisabled (void) const;

   //
   //   Access to my data
   //
   HalOrders&       Data () { return m_Orders; }
   const HalOrders& cData () const { return m_Orders; }

private:
   // copy of current orders
   HalOrders m_Orders;
   bool      m_override;
};

#endif /* ifndef _HALORDERS_INCLUDE */

/* FORMAT HASH 26e650d6ba27177e40b176274b1e4dbe */

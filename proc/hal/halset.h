/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef _HALSET_INCLUDE
#define _HALSET_INCLUDE

#include "chw.hpp"
#include "haldata.h"
#include "halsense.h"
#include "halpower.h"
#include "halpump.h"
#include "fastdata.h"

// One of these objects is created.  When the hardware status is received
// from the control driver, perform the following:
// 1) Lock CDS Semaphore, take a snapshot of the CDS into
//    this object. - pCDS()
// 2) Transfer the HWSTATUS info - TransferStatus(..)
// 3) Update Pump stuff. - UpdatePumps(..)
// 4) Accumulate pump volumes - AccumulateVolumes(..)
// 5) Transfer data to local CDS - TransferToCDS( .. )
// 6) Update global CDS and release semaphore

// When the message from Proc is received to update the hardware.
// 1) Take a snapshot of the CDS
// 2) *** Temporary - Update needed config and cal values - InitSensors()
// 3) *** Temporary - Limit flow commands depending on state - CheckOrders(..)
// 4) convert to HwOrders message - MakeOrders( .. )
// 5) Send to CtrlDrv


class CHalSet
{

public:
   CHalSet();
   virtual ~CHalSet();

   void Initialize (HalStatus_CDS* statuscds);

   // Initialize sensors with calibration values.
   void InitSensors ();

   //////////////////////////////////////////////////////////
   // On HwStatus Receive
   // This will guide through a hwstatus receive.
   void PerformStatusUpdate (const CHwStates& hw);

   //////////////////////////////////////////////////////////
   // On APS Fast Filter Data Receive
   void PerformAPSReadingUpdate (FASTFILTER& data);

   //////////////////////////////////////////////////////////
   // Handle APS auto-zero
   void PerformAPSAutoZero (float mmHg);

   // Converts units to put orders into control driver format
   // HalOrders ---> CHwOrders
   void MakeOrders (const HalOrders& orders, CHwOrders& ctrl);

   // Converts units to put orders into control driver formatted
   // orders into HAL orders format
   // CHwOrders ---> HalOrders
   void MakeOrders (const CHwOrders& ctrl, HalOrders& orders);

   inline unsigned long sendCount () { return m_SendCount; }
   inline void          SetSendCount (unsigned long count) { m_SendCount = count; }

   //
   // Provide a method to allow determination of set type
   //
   enum HalSetType
   {
      HST_Unknown,      // not a known set type
      HST_Normal,       // normal set type (CHalNormSet)
      HST_DRBC          // DRBC set type (CHalDRBCSet)
   };

   virtual HalSetType SetType (void) const;

protected:
   // Transfer and convert data and store in our local halstatus area.
   void TransferStatus (const CHwStates& hw);

   // Update pump information in halstatus.  Cassette specific.
   virtual void UpdatePumps (void)  = 0;
   // Obtain RPM orders given commanded flow rates
   virtual void ObtainOrders (void) = 0;
   // Obtain RPM orders given commanded flow rates
   virtual void TransferPumpOrders (const CHwOrders& ctrl, HalOrders& orders) = 0;

   // Transfer HwStatus pump info into our pump status format
   void TransferPumpStatus (HalPump_CDS& pmp, const PumpStatus& hw);

   // Conversion routines for Normal Mode
   CHW_CASSETTE_ORDERS convertHalCassetteOrder (HAL_CASSETTEORDERS order);
   CHW_VALVE_ORDERS    convertHalValveOrder (HAL_VALVESTATE order);
   HW_ORDERS           convertHalAlarmOrder (HAL_ALARMLIGHTORDERS order);
   CHW_DOOR_ORDERS     convertHalDoorOrder (HAL_DOORORDERS order);
   HAL_FLUIDDETECT     convertDriverACState (CHW_AC_DETECTOR_STATES state);
   HAL_RESERVOIRSTATE  convertDriverReservoirState (CHW_RESERVOIR_STATES state);
   HAL_VALVESTATE      convertDriverValveState (HW_VALVE_STATES state);
   HAL_CASSETTESTATE   convertDriverCassettePosition (HW_CASSETTE_STATES state);
   HAL_DOORSTATE       convertDriverDoorStatus (HW_DOOR_STATES state);
   int                 convertDriverEvent (CHW_EVENTS event);

   // Conversion routines for Service Mode
   HAL_CASSETTEORDERS   convertDriverCassetteOrder (CHW_CASSETTE_ORDERS order);
   HAL_VALVESTATE       convertDriverValveOrder (CHW_VALVE_ORDERS order);
   HAL_ALARMLIGHTORDERS convertDriverAlarmOrder (HW_ORDERS order);
   HAL_DOORORDERS       convertDriverDoorOrder (CHW_DOOR_ORDERS order);

protected:
   HalStatus_CDS* m_statuscds;

private:
   CSenseAPS     m_APS;
   CSenseCPS     m_CPS;
   CPwrSupply    m_psM12V;
   CPwrSupply    m_psP5V;
   CPwrSupply    m_psP12V;
   CPwrSupply    m_ps24V;
   CPwrSupply    m_psSW24V;
   CPwrSupply    m_ps64V;
   CPwrSupply    m_psSW64V;

   int           m_CentErrorCount;
   int           m_CentIntErrorCount;
   int           m_ValveErrorCount;
   int           m_MotorErrorCount;
   unsigned long m_SendCount;
};

// This class has a replacement pump instead of a collect pump.
// There is no platetlet collection into a bag.
// The collect valve is always in the return position.
class CHalDRBCSet
   : public CHalSet
{
public:
   CHalDRBCSet();
   virtual ~CHalDRBCSet();

   virtual void UpdatePumps (void);
   virtual void ObtainOrders (void);
   virtual void TransferPumpOrders (const CHwOrders& ctrl, HalOrders& orders);

   virtual HalSetType SetType (void) const;

private:
   CDRBCPumpSet m_pmp;
};

// This class has the normal 5 pumps.
// There can be platetlet collection into a bag.
// The collect valve can be in the collect position.
class CHalNormSet
   : public CHalSet
{

public:
   CHalNormSet();
   virtual ~CHalNormSet();

   virtual void UpdatePumps (void);
   virtual void ObtainOrders (void);
   virtual void TransferPumpOrders (const CHwOrders& ctrl, HalOrders& orders);

   virtual HalSetType SetType (void) const;

private:
   CNormPumpSet m_pmp;
};

#endif /* ifndef _HALSET_INCLUDE */

/* FORMAT HASH 9e688afd699dbfdf64375b2962cdb3b6 */

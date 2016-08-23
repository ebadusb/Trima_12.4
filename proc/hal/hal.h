/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef _HAL_INCLUDE
#define _HAL_INCLUDE

#include "alarms.h"
#include "callback.h"
#include "chw.hpp"
#include "fastdata.h"
#include "halorders.h"
#include "halset.h"
#include "halstatus_cds.h"
#include "trimamessages.h"

class CHal
{
public:
   // Constructor
   CHal(const CallbackBase& statusUpdateCallback);

   // Destructor
   virtual ~CHal();

   //
   // Initialize the object
   //
   ALARM_VALUES Init ();

   //
   // Convert HAL orders to HW orders and send
   //
   void SendOrders (CHalOrders* orders);

   //
   // Convert HW orders to HAL orders, returning the
   // resulting HAL orders for future record
   //
   void SendOrders (const CHwOrders& chworders, HalOrders& orders);

   //
   // Convert HW orders to HAL orders, but don't do
   // anything with them.  This is for service mode logging.
   //
   void MakeOrders (const CHwOrders& chworders, HalOrders& orders);

private:
   CHal(const CHal&);                    // not implemented
   CHal& operator = (const CHal& rhs);

protected:
   //
   // for processing when the new status msg is
   // received from the control driver
   //
   void NewStatus (void);

   //
   // for processing the APS reading out of bounds
   // message
   //
   void APSOutOfBounds (void);

   //
   // for processing APS auto-zero setting
   //
   void APSZero (void);

protected:
   // Select the proper HAL set
   void setSelect (cassette::cassetteType setType);

   // link to HAL status common data store
   HalStatus_CDS m_statuscds;
   HalStatus_CDS m_RWstatuscds;

   // to send HwOrders to the control driver
   CHwOrders          m_hwOrders;
   Message<CHwOrders> m_msgHwOrders;

   // to receive hardware status updates from the control driver
   Message<CHwStates> m_msgHwStates;

   // to receive APS readings out of limit boundary message from the
   //  control driver
   Message<FASTFILTER> m_msgAPS;

   // to send proc the APS reading that stopped the pumps
   APSOutOfBoundsMsg m_msgAPSStoppedPumps;

   // to receive APS auto-zero setting
   APSZeroMsg m_msgAPSZero;

   // Pointer to our current HalSet object
   CHalSet* m_pHalSet;
   CHalSet* NormalHalSet;
   CHalSet* DRBCHalSet;

   // Last reported cassette type - used to select appropriate CHalSet derived class
   cassette::cassetteType m_lastCassetteId;

private:
   CallbackBase m_statusUpdateCallback;
};

#endif /* ifndef _HAL_INCLUDE */

/* FORMAT HASH a112d6fb6ddabbc0678ff7951bca5c6e */

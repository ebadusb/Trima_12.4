/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>

#include "hal.h"

#include "chw.hpp"
#include "trima_datalog.h"


CHal::CHal(const CallbackBase& statusUpdateCallback)
   : m_msgHwOrders(MessageBase::SEND_GLOBAL),
     m_msgHwStates(Callback<CHal>(this, &CHal::NewStatus), MessageBase::SNDRCV_RECEIVE_ONLY),
     m_msgAPS(Callback<CHal>(this, &CHal::APSOutOfBounds), MessageBase::SNDRCV_RECEIVE_ONLY),
     m_msgAPSZero(Callback<CHal>(this, &CHal::APSZero), MessageBase::SNDRCV_RECEIVE_ONLY),
     m_msgAPSStoppedPumps(MessageBase::SEND_GLOBAL),
     m_statuscds(ROLE_RO),
     m_RWstatuscds(ROLE_RW),
     m_pHalSet(NULL),
     m_lastCassetteId(cassette::UNKNOWN),
     m_statusUpdateCallback(statusUpdateCallback)
{
   NormalHalSet = new CHalNormSet;
   DRBCHalSet   = new CHalDRBCSet;
}

CHal::~CHal()
{
   m_pHalSet = NULL;

   delete  NormalHalSet; NormalHalSet = NULL;
   delete  DRBCHalSet;   DRBCHalSet   = NULL;
}

ALARM_VALUES CHal::Init ()
{
   ALARM_VALUES alr = NULL_ALARM;

   // Create a normal set by default until a cassette is loaded.
   m_pHalSet = NormalHalSet;
   m_pHalSet->Initialize(&m_RWstatuscds);
   m_pHalSet->InitSensors();

   return alr;
}

void CHal::SendOrders (const CHwOrders& chworders, HalOrders& orders)
{
   m_hwOrders = chworders;

   // Update Sensors with config / calibration info
   m_pHalSet->MakeOrders(m_hwOrders, orders);

   // Send the orders to the control driver
   m_msgHwOrders.send(m_hwOrders);
}

void CHal::MakeOrders (const CHwOrders& chworders, HalOrders& orders)
{
   m_pHalSet->MakeOrders(chworders, orders);
}


// Signal from Procedure to update hardware
void CHal::SendOrders (CHalOrders* orders)
{
   setSelect(orders->CassetteId());

   // Update Sensors with config / calibration info
   m_pHalSet->MakeOrders(orders->cData(), m_hwOrders);

   // Send the orders to the control driver
   m_msgHwOrders.send(m_hwOrders);
}

// Message of CHwStatus from control driver
void CHal::NewStatus ()
{
   const CHwStates status = m_msgHwStates.getData();

   // This updates our local CDS copy in halset
   m_pHalSet->PerformStatusUpdate(status);

   m_statusUpdateCallback();
}

void CHal::APSOutOfBounds ()
{
   FASTFILTER filterData = m_msgAPS.getData();

   // This updates our local CDS copy in halset
   m_pHalSet->PerformAPSReadingUpdate(filterData);

   // Do this here because above function sets the CDS.
   const float apsReading = m_statuscds._aps.Get();

   // send out hal APS stopped pumps message
   m_msgAPSStoppedPumps.send( (int)apsReading);

   DataLog(log_level_proc_info) << "APSOutOfBounds: Pumps stopped by the control driver (aps=" << apsReading << ")" << endmsg;
}

void CHal::APSZero ()
{
   const float zero = (float)m_msgAPSZero.getData();
   m_pHalSet->PerformAPSAutoZero(zero);
}

void CHal::setSelect (cassette::cassetteType setType)
{
   if (m_lastCassetteId != setType)
   {
      DataLog(log_level_proc_info) << "Cassette type changed from " << (int)m_lastCassetteId << " to " << (int)setType << endmsg;

      m_lastCassetteId = setType;

      switch (m_lastCassetteId)
      {
         case cassette::PLTPLSRBC :
            if (m_pHalSet->SetType() != CHalSet::HST_Normal)
            {
               const unsigned long oldSendCount = m_pHalSet->sendCount();

               m_pHalSet = NormalHalSet;
               m_pHalSet->Initialize(&m_RWstatuscds);
               m_pHalSet->InitSensors();
               m_pHalSet->SetSendCount(oldSendCount);

               DataLog(log_level_proc_info) << "Switched to CHalNormSet" << endmsg;
            }

            break;

         case cassette::RBCPLS :
            if (m_pHalSet->SetType() != CHalSet::HST_DRBC)
            {
               const unsigned long oldSendCount = m_pHalSet->sendCount();

               m_pHalSet = DRBCHalSet;
               m_pHalSet->Initialize(&m_RWstatuscds);
               m_pHalSet->InitSensors();
               m_pHalSet->SetSendCount(oldSendCount);

               DataLog(log_level_proc_info) << "Switched to CHalDRBCSet" << endmsg;
            }

            break;

         case cassette::NOT_LOADED :
         case cassette::UNKNOWN :
            break;

         default :
            //
            // Fatal error here to be sure that if we add a new cassette type - we know
            // that it is not yet supported by hal
            //
         {
            DataLog_Critical critical;
            DataLog(critical) << "cassette type=" << (int)m_lastCassetteId << endmsg;
            _FATAL_ERROR(__FILE__, __LINE__, "No HAL set available for cassette type");
         }
      }
   }
}

/* FORMAT HASH 0a275b12f088898048a2f572efd46295 */

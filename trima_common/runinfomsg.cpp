/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      runinfomsg.cpp
 *             Run Info message object and container.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/
#include <vxworks.h>

#include "runinfomsg.h"
#include "time.h"
#include "trima_datalog.h"


CRunInfoMsg::CRunInfoMsg()
   : m_message(),
     _timecds(ROLE_RO),
     _volumescds(ROLE_RO),
     _runcds(ROLE_RO),
     _configcds(ROLE_RO),
     _isPtfDrbc(false)
{
   memset(&m_Data, 0, sizeof( CURRENT_PROCEDURE_STATUS_STRUCT ) );
}

CRunInfoMsg::~CRunInfoMsg()
{}

CURRENT_PROCEDURE_STATUS_STRUCT& CRunInfoMsg::Data ()
{
   return m_Data;
}

const CURRENT_PROCEDURE_STATUS_STRUCT& CRunInfoMsg::cData () const
{
   return m_Data;
}

void CRunInfoMsg::SendMsg ()
{
   m_message.send(m_Data);
}


void CRunInfoMsg::SetStartTime (BaseElement<float>& startTime)
{
   if (startTime.Get() == 0.0f)
   {
      // We haven't recorded a procStartTime yet.
      time_t     tm;
      time(&tm);
      struct tm* tmstruct = localtime(&tm);

      if (startTime.isWritable()) startTime.Set((float)tmstruct->tm_hour * 60.0f + tmstruct->tm_min);
      else _FATAL_ERROR(__FILE__, __LINE__, "start time base element is not writable");

      DataLog (log_level_gui_info) << "Initialize fresh procStartTime: " << (float)tmstruct->tm_hour * 60.0f + tmstruct->tm_min << endmsg;
   }
   else DataLog (log_level_gui_info) << "Load PFR procStartTime: " << startTime.Get() << endmsg;


}

void CRunInfoMsg::Initialize ()
{
   //
   // Initialize the message for sending ...
   m_message.init(MessageBase::SEND_LOCAL);

   m_Data.current_inlet_pressure_min = _configcds.Procedure.Get().key_draw_press;
   m_Data.current_inlet_pressure_max = _configcds.Procedure.Get().key_return_press;
   m_Data.current_inlet_pressure     = 0;
   m_Data.use_startup_pressure_meter = 1;
}

void CRunInfoMsg::SetTargets (float plat, float plas, float rbc)
{
   m_Data.estimated_platelets = plat;
   m_Data.estimated_plasma    = plas;
   m_Data.estimated_rbcs      = rbc;
}

void CRunInfoMsg::Update ()
{
   m_Data.return_pump_cycle           = (char)( _runcds.DrawCycle.Get() ? 0 : 1 );
   m_Data.current_platelets_collected = _volumescds.PlateletYield.Get() / 1.0e11f;
   m_Data.current_plasma_collected    = _volumescds.PlasmaVolume.Get();
   m_Data.current_rbc_collected       = _volumescds.RBCVolume.Get();
   m_Data.current_rbcs_prod1          = _volumescds.RBCProduct_A_FinalVolume.Get();
   m_Data.current_rbcs_prod2          = _volumescds.RBCProduct_B_FinalVolume.Get();
}

void CRunInfoMsg::Pressure (const float APSLow, const float APSHigh,
                            const float APSCur, const int meter)
{
   m_Data.current_inlet_pressure     = APSCur;
   m_Data.current_inlet_pressure_min = APSLow;
   m_Data.current_inlet_pressure_max = APSHigh;
   m_Data.use_startup_pressure_meter = meter;
}

/* FORMAT HASH 690274564604b7196e00c0d4d0cb5d8d */

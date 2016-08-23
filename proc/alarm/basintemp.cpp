/*
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * File: BasinTemp.cpp
 *
 */

#include "basintemp.h"

#include <float.h>
#include <stdio.h>
#include <unistd.h>

#include "hwdat.h"
#include "procdata.h"

// Never to exceed safety limit temperature
static const float ABSOLUTE_MAXIMUM_TEMPERATURE = 42.0f;

DEFINE_OBJ(BasinTempAlert);

BasinTempAlert::BasinTempAlert()
   : m_msgNewData(Callback<BasinTempAlert>(this, &BasinTempAlert::newTempData)),
     m_msgBasinTemp(MessageBase::SEND_LOCAL),
     m_AlmOvertempWarning(OVERTEMP_ALARM),
     m_AlmOvertempFailure(OVERTEMP_FAILURE),
     m_bValid(0),
     m_fAverage(0.0f),
     m_nNumSamples(0),
     m_pHistory(0),
     m_nCurIndex(0),
     m_fCurrentTime(0.0f),
     m_DataSeen(true)
{}

BasinTempAlert::~BasinTempAlert()
{
   cleanup();
}

void BasinTempAlert::Initialize ()
{
   m_msgBasinTemp.send(0);

   m_pHistory = new sample_t[BT_MAXIMUM_SAMPLES];
   if ( m_pHistory != 0 )
   {
      ClearHistory();
   }
   else
   {
      DataLog(log_level_proc_alarm_monitor_error) <<  "BasinTemp : Unable to Allocate Storage" << endmsg;
   }

   DataLog(log_level_proc_alarm_monitor_info) << "Max basin temp configured to: " << HWDat::data().MaximumBasinTemperature << endmsg;
}

void BasinTempAlert::Monitor ()
{
   ProcData pd;
   //
   // Get the current time ...
   //
   m_fCurrentTime = pd.AbsTime().getMins();

   if (m_fAverage >= ABSOLUTE_MAXIMUM_TEMPERATURE)
   {
      m_AlmOvertempFailure.setAlarm();
   }

   else if (m_fAverage >= HWDat::data().MaximumBasinTemperature )
   {
      m_AlmOvertempWarning.setAlarm();
   }
}

void BasinTempAlert::newTempData (void)
{
   static const float MINIMUM_VALID_TEMPERATURE = 10.0f;
   static const float MAXIMUM_VALID_TEMPERATURE = 60.0f;

   basinTemp          data = m_msgNewData.getData();

   if (data.dataType == NO_BASIN_DATA)
   {
      m_msgBasinTemp.send(0);
      if ( m_DataSeen )
      {
         DataLog(log_level_proc_alarm_monitor_info) << "BasinTemp: no data flag received" << endmsg;
         m_DataSeen = false;
      }
   }
   else
   {
      m_DataSeen = true;
   }

   if (data.dataType == BASIN_DATA_CORRUPT)
   {
      m_msgBasinTemp.send(-1);
      DataLog(log_level_proc_alarm_monitor_info) << "BasinTemp: bad sample received" << endmsg;
   }

   // periodic update to trace log for debugging
   static int count = 0;
   count++;
   if ( count > 60 )
   {
      count = 0;
      DataLog(log_level_proc_alarm_monitor_info) << "Basin Temp : Type: " << data.dataType
                                                 <<             " Temp: " << data.temp
                                                 <<              " Ave: " << m_fAverage
                                                 <<                " N: " << m_nNumSamples << endmsg;

   }

   if (data.dataType == BASIN_TEMP)
   {
      m_msgBasinTemp.send( (long)data.temp);

      if (
         (data.temp >= MINIMUM_VALID_TEMPERATURE) &&
         (data.temp <= MAXIMUM_VALID_TEMPERATURE)
         )
      {

         //
         //
         //   Recalculate the average temperature
         //
         m_fAverage = movingAverage(data.temp);

      }
      else
      {
         DataLog(log_level_proc_alarm_monitor_info) << "BasinTemp: temperature out of range ignored: " <<  (int)data.temp << endmsg;
      }
   }
}

float BasinTempAlert::movingAverage (float fTemp)
{
   float fAve = 0.0f;

   // Insert the new data
   m_pHistory[m_nCurIndex].temperature = fTemp;
   m_pHistory[m_nCurIndex].time        = m_fCurrentTime;
   m_pHistory[m_nCurIndex].bValid      = 1;
   m_nCurIndex = IncIndex(m_nCurIndex);

   // remove old data from our history
   RemoveSamples(m_fCurrentTime);

   // calculate the current average
   fAve = CalAverage();

   // check if we have enough data in this period for an average
   if ( m_nNumSamples >= BT_MINIMUM_SAMPLES )
   {
      m_bValid = 1;  // we have a valid mean
   }
   else
   {    // if we had data and lost it, then log the error
      if ( m_bValid )
      {
         ClearHistory();
         DataLog(log_level_proc_alarm_monitor_info) << "BasinTemp: period elapsed, not enough readings" << endmsg;
      }
   }

   // if we have not yet got enough good historical samples
   // then let average be 0
   if ( !m_bValid )
   {
      fAve = 0.0f;
   }


   return fAve;
}

void BasinTempAlert::cleanup ()
{
   delete [] m_pHistory;
}

void BasinTempAlert::ClearHistory ()
{
   for ( int i = 0 ; i < BT_MAXIMUM_SAMPLES ; i++ )
   {
      m_pHistory[i].temperature = 0.0f;
      m_pHistory[i].time        = 0.0f;
      m_pHistory[i].bValid      = 0;
   }
   m_fAverage    = 0.0f;
   m_nNumSamples = 0;
   m_bValid      = 0;
   m_nCurIndex   = 0;
}

int BasinTempAlert::IncIndex (int nIndex)
{
   nIndex++;
   if ( nIndex >= BT_MAXIMUM_SAMPLES ) nIndex = 0;
   return nIndex;
}

void BasinTempAlert::RemoveSamples (float ct)
{
   for ( int i = 0 ; i < BT_MAXIMUM_SAMPLES ; i++ )
   {
      if (
         m_pHistory[i].bValid &&
         ( ( ct - m_pHistory[i].time ) > BT_SAMPLE_PERIOD )
         )
         m_pHistory[i].bValid = 0;
   }
}

float BasinTempAlert::CalAverage ()
{
   float fAve = 0;
   m_nNumSamples = 0;
   for ( int i = 0 ; i < BT_MAXIMUM_SAMPLES ; i++ )
   {
      if ( m_pHistory[i].bValid )
      {
         fAve += m_pHistory[i].temperature;
         m_nNumSamples++;
      }
   }

   if ( m_nNumSamples > 0 )
      fAve /= (float)m_nNumSamples;

   return fAve;
}

/* FORMAT HASH 2172a4e673e605b084dd4d4e1663f12b */

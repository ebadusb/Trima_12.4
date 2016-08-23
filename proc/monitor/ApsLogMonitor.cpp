/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ApsLogMonitor.cpp
 *
 * ABSTRACT:  This is the source file for the product quality monitors' base class.  It is derived of class MonitorBase.
 *
 */

#include "ApsLogMonitor.h"
#include "procdata.h"
#include "datalog_periodic.h"
#include "halstatus_cds.h"
#include "software_cds.h"

#define LOG_INTERVAL    500L /*msecs*/


DEFINE_OBJ(ApsLogMonitor);

// Constructor, called at the time the "states" file is read in.
ApsLogMonitor::ApsLogMonitor()
   : _logNow(false),
     _wasNotLogging(true),
     _currentAPS(0.0f),
     _apsHighLimit(0.0f),
     _apsLowLimit(0.0f),
     _DataLogSet(DATALOG_NULL_SET_HANDLE),
     _FeatureChanged(Callback<ApsLogMonitor>(this, &ApsLogMonitor::featureChanged)),
     _featureEnabled(Software_CDS::GetInstance().getFeature(ApsHighSpeedLogEnable) )
{}



// Destructor
ApsLogMonitor::~ApsLogMonitor()
{}


// Initialization
void ApsLogMonitor::Initialize ()
{
   // Do nothing if feature disabled
   if ( !_featureEnabled )
   {
      return;
   }

   ProcData       pd;
   HalStatus_CDS& hal_status(pd.Status() );

   datalog_CreatePeriodicSet("Proc_APS_Log", &_DataLogSet);

   float (BaseElement<float>::* float_func) (void) const =
      &BaseElement<float>::Get;

   datalog_AddMemberFunc(_DataLogSet,
                         &hal_status._aps,
                         float_func,
                         "NAMAPS",
                         "High Speed APS",
                         "%f");

   datalog_ForcePeriodicOutput(_DataLogSet);

   datalog_SetPeriodicOutputInterval(_DataLogSet, LOG_INTERVAL);

   datalog_DisablePeriodicOutput(_DataLogSet);    // Initialize it to off

   // Get these values out of sw.dat... unfortunately, they are currently
   // missing. That being the case, we are using default values.
   _apsHighLimit = 150; // Software_CDS::GetInstance().aps_high_limit;
   _apsLowLimit  = 0;   // Software_CDS::GetInstance().aps_low_limit;
}


void ApsLogMonitor::Monitor ()
{
   // Do nothing if the feature is disabled
   if ( !_featureEnabled )
   {
      return;
   }

   ProcData pd;
   _currentAPS = pd.Status().APS();

   if ( ( _currentAPS <= _apsLowLimit )
        ||
        ( _currentAPS >= _apsHighLimit ) )
   {

#if 0
      DataLog(log_level_proc_info)
         << "Current APS = " << _currentAPS << endmsg;

      DataLog(log_level_proc_info)
         << "APS Low Limit = " << _apsLowLimit << endmsg;

      DataLog(log_level_proc_info)
         << "APS High Limit = " << _apsHighLimit << endmsg;

      DataLog(log_level_proc_info)
         << "System State = " << pd.SystemState() << endmsg;

      DataLog(log_level_proc_info)
         << "Substate Name = " << pd.SubstateName() << endmsg;

      DataLog(log_level_proc_info)
         << "Event = " <<  pd.Status().Event() << endmsg;
#endif

      do
      {

         if ( ( ( pd.SystemState() == BLOOD_PRIME )
                ||
                ( pd.SystemState() == DONOR_CONNECTED ) )
              &&
              ( _logNow == false ) )
         {

            _logNow = true;
#if 0
            if ( pd.SystemState() == BLOOD_PRIME )
            {

               DataLog(log_level_proc_info)
                  << "SystemState BLOOD_PRIME" << endmsg;
            }
            else if ( pd.SystemState() == DONOR_CONNECTED )
            {

               DataLog(log_level_proc_info)
                  << "SystemState DONOR_CONNECTED" << endmsg;
            }
#endif
            break;
         }

         if ( ( ( pd.SystemState() == BLOOD_RINSEBACK )
                ||
                ( pd.Status().Event() == HAL_PAUSE_EVENT ) )
              &&
              ( _logNow == true ) )
         {
#if 0

            DataLog(log_level_proc_info)
               << "SystemState() = "
               << pd.SystemState() << endmsg;

            DataLog(log_level_proc_info)
               << "Event() = "
               << pd.Status().Event() << endmsg;
#endif

            _logNow = false;

            break;
         }
      } while ( false );

      if ( _logNow && _wasNotLogging )
      {

         DataLog(log_level_proc_info)
            << "Enabling APS High Speed logging" << endmsg;

         datalog_EnablePeriodicOutput(_DataLogSet);

         datalog_ForcePeriodicOutput(_DataLogSet);

         _wasNotLogging = false;
      }
      else if ( !_logNow && !_wasNotLogging )
      {

         DataLog(log_level_proc_info)
            << "Disabling APS High Speed logging" << endmsg;

         datalog_ForcePeriodicOutput(_DataLogSet);

         datalog_DisablePeriodicOutput(_DataLogSet);

         _wasNotLogging = true;
      }
   }
}


void ApsLogMonitor::featureChanged ()
{
   // Has this feature been toggled?
   if ( Software_CDS::GetInstance().getFeature(ApsHighSpeedLogEnable) != _featureEnabled )
   {
      // Yes. Toggle it and turn things ON/OFF accordingly
      _featureEnabled = !_featureEnabled;
      DataLog(log_level_proc_info) << "feature ApsHighSpeedLogEnable is now "
                                   << (_featureEnabled ? "ON" : "OFF") << endmsg;

      if ( _featureEnabled && _DataLogSet == DATALOG_NULL_SET_HANDLE )
      {
         // setup the logging
         Initialize();
      }
      else if ( !_featureEnabled && _DataLogSet != DATALOG_NULL_SET_HANDLE )
      {
         // turn off the logging
         datalog_DisablePeriodicOutput(_DataLogSet);
      }
   }
}

/* FORMAT HASH 0cd7aa51e76f27a8f167542f64e986e8 */

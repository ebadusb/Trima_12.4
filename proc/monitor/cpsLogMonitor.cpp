/*
 * Copyright (c) 2004 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cpsLogMonitor.cpp
 *
 * ABSTRACT:  This is the source file for the product quality monitors' base class.  It is derived of class MonitorBase.
 *
 */


#define  CPS_MONITOR_BASE
#include "cpsLogMonitor.h"
#include "procdata.h"
#include "datalog_periodic.h"
#include "software_cds.h"

#define LOG_INTERVAL    500L /*msecs*/


DEFINE_OBJ(cpsLogMonitor);

// Constructor, called at the time the "states" file is read in.
cpsLogMonitor::cpsLogMonitor()
   : _WasNotLogging(true),
     _DataLogSet(DATALOG_NULL_SET_HANDLE),
     _FeatureChanged(Callback<cpsLogMonitor>(this, &cpsLogMonitor::featureChanged)),
     _featureEnabled(Software_CDS::GetInstance().getFeature(CpsHighSpeedLogEnable))
{}



// Destructor
cpsLogMonitor::~cpsLogMonitor()
{}



// Initialization
void cpsLogMonitor::Initialize ()
{
   // Do nothing if feature disabled
   if ( !_featureEnabled )
   {
      return;
   }

   ProcData       pd;
   HalStatus_CDS& hal_status = pd.Status();

   datalog_CreatePeriodicSet("Proc_CPS_Log", &_DataLogSet);

   float (BaseElement<float>::* float_func) (void) const = &BaseElement<float>::Get;

   datalog_AddMemberFunc(_DataLogSet, &hal_status._cps, float_func, "PTFCPS", "High Speed CPS", "%f");

   datalog_ForcePeriodicOutput(_DataLogSet);
   datalog_SetPeriodicOutputInterval(_DataLogSet, LOG_INTERVAL);

   datalog_DisablePeriodicOutput(_DataLogSet);  // Initialize it to off
}




void cpsLogMonitor::Monitor ()
{
   // Do nothing if feature disabled
   if ( !_featureEnabled )
   {
      return;
   }

   ProcData pd;
   bool     logNow = false;
   int      spot   = 0;

   do
   {
      // Only activate on PTF sets
      if (pd.RunTargets().MeterRbcs.Get())
      {
         if ( (pd.VistaSubstateName() == "LowerCassette") || (pd.VistaSubstateName() == "CassetteID") )
         {
            logNow = true;
            spot   = 1;
            break;
         }

         if ( (pd.VistaSubstateName() == "PrimeChannel")   ||
              (pd.VistaSubstateName() == "PrimeChannel2A") ||
              (pd.VistaSubstateName() == "PrimeChannel3")  ||
              (pd.VistaSubstateName() == "PrimeChannel4")  ||
              (pd.VistaSubstateName() == "PrimeVent")
              )
         {
            logNow = true;
            spot   = 2;
            break;
         }

         if ( (pd.VistaSubstateName() == "RBCPTFSetup1") ||
              (pd.VistaSubstateName() == "RBCPTFSetup2") ||
              (pd.VistaSubstateName() == "RBCPTFPrime") ||
              (pd.VistaSubstateName() == "RBCCollection") ||
              (pd.VistaSubstateName() == "RBCPTFSetup1Double") ||
              (pd.VistaSubstateName() == "RBCPTFSetup2Double") ||
              (pd.VistaSubstateName() == "RBCCollectionDouble")

              )
         {
            logNow = true;
            spot   = 3;
            break;
         }
      }
   } while (false);

   if (logNow && _WasNotLogging)
   {
      DataLog(log_level_proc_info) << "Enabling CPS High Speed logging during ";

      switch (spot)
      {
         case 1 :
            DataLog(log_level_proc_info) << "LowerCassette" << endmsg;
            break;

         case 2 :
            DataLog(log_level_proc_info) << "Blood Prime" << endmsg;
            break;

         case 3 :
            DataLog(log_level_proc_info) << "RBC Collection." << endmsg;
            break;
      }


      datalog_EnablePeriodicOutput(_DataLogSet);
      datalog_ForcePeriodicOutput(_DataLogSet);

      _WasNotLogging = false;

   }
   else if (!logNow && !_WasNotLogging)
   {
      DataLog(log_level_proc_info) << "Disabling CPS High Speed logging" << endmsg;

      datalog_ForcePeriodicOutput(_DataLogSet);
      datalog_DisablePeriodicOutput(_DataLogSet);

      _WasNotLogging = true;
   }
}


void cpsLogMonitor::featureChanged ()
{
   // Has this feature been toggled?
   if ( Software_CDS::GetInstance().getFeature(CpsHighSpeedLogEnable) != _featureEnabled )
   {
      // Yes. Toggle it and turn things ON/OFF accordingly
      _featureEnabled = !_featureEnabled;
      DataLog(log_level_proc_info) << "feature CpsHighSpeedLogEnable is now "
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

/* FORMAT HASH 85b8fea3cafce58a0208656b850daa6b */

#include "s_monitor_group.hpp"
#include "state_objects.hpp"
#include "s_exec.hpp"
#include "s_pfr.hpp"
#include "alarms.h"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;

MonitorGroup::MonitorGroup(Group type)
   : ModuleGroup(type),
     m_processors(type),
     m_monitors()
{}

MonitorGroup::~MonitorGroup()
{

   // Iterate through all stored monitors and call delete
   Map_t::iterator i = m_monitors.begin();
   Map_t::iterator e = m_monitors.end();

   for (; i != e; ++i)
      delete i->second; ;

}



void MonitorGroup::check ()
{
   // first, run the pre-processors
   if ( m_processors() == SafetyModule::SUCCESS )
   {
      // we'll persist state to PFR file consistently after prepocessors,
      // but before we get to the monitors.
      SaveToPFR();

      std::map<std::string, Monitor*>::iterator iter = m_monitors.begin();

      while ( iter != m_monitors.end() )
      {
         if ( iter->second == NULL )
         {
            // no need to call delete, it's NULL (obviously, but code like this
            // always needs a second look, and probably a third and fourth)
            m_monitors.erase(iter++);
         }
         else
         {
            if ( iter->second->isEnabled() )
               iter->second->check();

            iter++;
         }
      }
   }
   else
   {
      // pre-conditions aren't guaranteed, so bail
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SAFETY_T1_FAILURE);
   }
}

void MonitorGroup::add (Monitor* monitor)
{
   // add (or replace) this monitor
   const std::string& monitorName(monitor->name() );

   std::map<std::string, Monitor*>::iterator iter = m_monitors.find(monitorName);

   if ( iter != m_monitors.end() )
   {
      delete iter->second;      // clean up the old one...
      iter->second = monitor;   // ... and replace with new one
   }
   else
   {
      m_monitors.insert(std::pair<std::string, Monitor*>(monitorName, monitor) );
   }
}

void MonitorGroup::SaveToPFR ()
{
   ControlStatus_Export& cs      = SafetyExec::instance()->data().control;
   SafetyStatus_Export&  ss      = SafetyExec::instance()->data().safety;
   SafetyDerived&        derived = SafetyExec::instance()->data().derived;

   // update PFR data
   SafetyPFRDataToSave.PFRaReturn_lAccumRevs     = cs.pumps[RETURN_PUMP].accum;
   SafetyPFRDataToSave.PFRanAC_lAccumRevs        = cs.pumps[AC_PUMP].accum;
   SafetyPFRDataToSave.PFRanInlet_lAccumRevs     = cs.pumps[INLET_PUMP].accum;
   SafetyPFRDataToSave.PFRaPlasma_lAccumRevs     = cs.pumps[PLASMA_PUMP].accum;
   SafetyPFRDataToSave.PFRaPlatelet_lAccumRevs   = cs.pumps[PLATELET_PUMP].accum;

   SafetyPFRDataToSave.PFRaReturn_fDeltaVolSet   = derived.volumes[RETURN_PUMP];
   SafetyPFRDataToSave.PFRanAC_fDeltaVolSet      = derived.volumes[AC_PUMP];
   SafetyPFRDataToSave.PFRanInlet_fDeltaVolSet   = derived.volumes[INLET_PUMP];
   SafetyPFRDataToSave.PFRaPlasma_fDeltaVolSet   = derived.volumes[PLASMA_PUMP];
   SafetyPFRDataToSave.PFRaPlatelet_fDeltaVolSet = derived.volumes[PLATELET_PUMP];

   SafetyPFRDataToSave.PFRanAC_fSV               = derived.stroke[AC_PUMP];
   SafetyPFRDataToSave.PFRanInlet_fSV            = derived.stroke[INLET_PUMP];
   SafetyPFRDataToSave.PFRaPlasma_fSV            = derived.stroke[PLASMA_PUMP];
   SafetyPFRDataToSave.PFRaPlatelet_fSV          = derived.stroke[PLATELET_PUMP];
   SafetyPFRDataToSave.PFRaReturn_fSV            = derived.stroke[RETURN_PUMP];

   SafetyPFRDataToSave.PFRaReturn_lAccumHalls    = ss.pumps[RETURN_PUMP].accumHalls;
   SafetyPFRDataToSave.PFRanAC_lAccumHalls       = ss.pumps[AC_PUMP].accumHalls;
   SafetyPFRDataToSave.PFRanInlet_lAccumHalls    = ss.pumps[INLET_PUMP].accumHalls;
   SafetyPFRDataToSave.PFRaPlasma_lAccumHalls    = ss.pumps[PLASMA_PUMP].accumHalls;
   SafetyPFRDataToSave.PFRaPlatelet_lAccumHalls  = ss.pumps[PLATELET_PUMP].accumHalls;
}

void MonitorGroup::RestoreFromPFR ()
{
   DataLog(log_level_safe_exec_info) << "Restoring monitor group data" << endmsg;

   ControlStatus_Export& cs      = SafetyExec::instance()->data().control;
   SafetyStatus_Export&  ss      = SafetyExec::instance()->data().safety;
   SafetyDerived&        derived = SafetyExec::instance()->data().derived;

   cs.pumps[RETURN_PUMP].accum         = SafetyPFRDataFromRestore.PFRaReturn_lAccumRevs;
   cs.pumps[AC_PUMP].accum             = SafetyPFRDataFromRestore.PFRanAC_lAccumRevs;
   cs.pumps[INLET_PUMP].accum          = SafetyPFRDataFromRestore.PFRanInlet_lAccumRevs;
   cs.pumps[PLASMA_PUMP].accum         = SafetyPFRDataFromRestore.PFRaPlasma_lAccumRevs;
   cs.pumps[PLATELET_PUMP].accum       = SafetyPFRDataFromRestore.PFRaPlatelet_lAccumRevs;

   cs.pumps[RETURN_PUMP].offset        = SafetyPFRDataFromRestore.PFRaReturn_lAccumRevs;
   cs.pumps[AC_PUMP].offset            = SafetyPFRDataFromRestore.PFRanAC_lAccumRevs;
   cs.pumps[INLET_PUMP].offset         = SafetyPFRDataFromRestore.PFRanInlet_lAccumRevs;
   cs.pumps[PLASMA_PUMP].offset        = SafetyPFRDataFromRestore.PFRaPlasma_lAccumRevs;
   cs.pumps[PLATELET_PUMP].offset      = SafetyPFRDataFromRestore.PFRaPlatelet_lAccumRevs;

   ss.pumps[RETURN_PUMP].accumHalls    = SafetyPFRDataFromRestore.PFRaReturn_lAccumHalls;
   ss.pumps[AC_PUMP].accumHalls        = SafetyPFRDataFromRestore.PFRanAC_lAccumHalls;
   ss.pumps[INLET_PUMP].accumHalls     = SafetyPFRDataFromRestore.PFRanInlet_lAccumHalls;
   ss.pumps[PLASMA_PUMP].accumHalls    = SafetyPFRDataFromRestore.PFRaPlasma_lAccumHalls;
   ss.pumps[PLATELET_PUMP].accumHalls  = SafetyPFRDataFromRestore.PFRaPlatelet_lAccumHalls;

   ss.pumps[RETURN_PUMP].offsetHalls   = SafetyPFRDataFromRestore.PFRaReturn_lAccumHalls;
   ss.pumps[AC_PUMP].offsetHalls       = SafetyPFRDataFromRestore.PFRanAC_lAccumHalls;
   ss.pumps[INLET_PUMP].offsetHalls    = SafetyPFRDataFromRestore.PFRanInlet_lAccumHalls;
   ss.pumps[PLASMA_PUMP].offsetHalls   = SafetyPFRDataFromRestore.PFRaPlasma_lAccumHalls;
   ss.pumps[PLATELET_PUMP].offsetHalls = SafetyPFRDataFromRestore.PFRaPlatelet_lAccumHalls;

   derived.stroke[RETURN_PUMP]         = SafetyPFRDataFromRestore.PFRaReturn_fSV;
   derived.stroke[AC_PUMP]             = SafetyPFRDataFromRestore.PFRanAC_fSV;
   derived.stroke[INLET_PUMP]          = SafetyPFRDataFromRestore.PFRanInlet_fSV;
   derived.stroke[PLASMA_PUMP]         = SafetyPFRDataFromRestore.PFRaPlasma_fSV;
   derived.stroke[PLATELET_PUMP]       = SafetyPFRDataFromRestore.PFRaPlatelet_fSV;

   derived.volumes[RETURN_PUMP]        = SafetyPFRDataFromRestore.PFRaReturn_fDeltaVolSet;
   derived.volumes[AC_PUMP]            = SafetyPFRDataFromRestore.PFRanAC_fDeltaVolSet;
   derived.volumes[INLET_PUMP]         = SafetyPFRDataFromRestore.PFRanInlet_fDeltaVolSet;
   derived.volumes[PLASMA_PUMP]        = SafetyPFRDataFromRestore.PFRaPlasma_fDeltaVolSet;
   derived.volumes[PLATELET_PUMP]      = SafetyPFRDataFromRestore.PFRaPlatelet_fDeltaVolSet;

   SaveToPFR();


   DataLog(log_level_safe_exec_info) << "Pump Offsets:";

   for ( int i = RETURN_PUMP; i <= PLATELET_PUMP; i++ )
   {
      SafetyPumps pumpEnum = static_cast<SafetyPumps>(i);

      DataLog(log_level_safe_exec_info) << " " << pumpEnum
                                        << " = " << cs.pumps[i].offset << " (ENC), "
                                        << ss.pumps[i].offsetHalls << " (HALL);";
   }

   DataLog(log_level_safe_exec_info) << endmsg;
}

/* FORMAT HASH f3b25f3f03a2c5609ac6c0f5e8c62568 */

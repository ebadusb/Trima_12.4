///////////////////////////////////////////////////////////////////////////////
#include "ClinicalDataLogMonitor.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "datalog_periodic.h"


#define  DLOG   DataLog(log_level_proc_info)

DEFINE_OBJ(ClinicalDataLogMonitor);

// Constructor, called at the time the "states" file is read in.
ClinicalDataLogMonitor::ClinicalDataLogMonitor()
   : lastSubstate("NONE"),
     lastState("NONE"),
     lastCollectSubstate(NOCOLLECT_STATE)
{}

// Destructor
ClinicalDataLogMonitor::~ClinicalDataLogMonitor()
{}

// Initialization
void ClinicalDataLogMonitor::Initialize ()
{}


void ClinicalDataLogMonitor::Monitor ()
{

   ProcData pd;


   if (!pd.RunTargets().MeterRbcs.Get())
      return;

   State_names currCollectState = pd.Run().Substate.Get();
   string      currState (state_name_table[pd.SystemState()].state_text);
   string      currSubState     =  pd.SubstateName();
   if (currState == "BloodRinseback")
   {
      currCollectState = INVALID_SUBSTATE;
   }

   if (currSubState == "DisconnectPrompt")
      return;

   /////////////////////////////////////////////////////////////////////////////////
   //  Write this stuff on the state change //
   ///////////////////////////////////////////////////////////////////////////////////
   if ((lastSubstate != currSubState)
       &&  (lastSubstate != "NONE"))
   {
      writeData(CPSStats, lastSubstate, "CPS");
      writeData(APSStats, lastSubstate, "APS");
      writeData(ACPumpStats, lastSubstate, "AC Pump");
      writeData(InletPumpStats, lastSubstate, "Inlet Pump");
      writeData(ReturnPumpStats, lastSubstate, "Return Pump");

   }

   if ((lastState != currState)
       &&  (lastState != "NONE"))
   {
      writeData(CPSStatsState, lastState, "CPS");
      writeData(APSStatsState, lastState, "APS");

   }

   if ( (lastCollectSubstate != currCollectState) )
   {
      /*
     DataLog( log_level_proc_info ) << "substate transitioning -> "
          <<  lastCollectSubstate
          << "/"
          << currCollectState
          << endmsg;
     */
      writeData(CPSStatsCollectState, SubstateMap::ssStr(lastCollectSubstate), "CPS");
      writeData(APSStatsCollectState, SubstateMap::ssStr(lastCollectSubstate), "APS");

   }

   /////////////////////////////////////////////////////////////////////////////
   //  Gather the data for collect states
   /////////////////////////////////////////////////////////////////////////////
   if (currCollectState == SS_RBC_PTF_SETUP_1)
   {
      CPSStatsCollectState.addValue(pd.Status().CPS());
      APSStatsCollectState.addValue(pd.Status().APS());
      lastCollectSubstate = currCollectState;
   }

   if (currCollectState == SS_RBC_PTF_SETUP_2)
   {
      CPSStatsCollectState.addValue(pd.Status().CPS());
      APSStatsCollectState.addValue(pd.Status().APS());
      lastCollectSubstate = currCollectState;
   }
   if (currCollectState == SS_RBC_PTF_PRIME)
   {
      CPSStatsCollectState.addValue(pd.Status().CPS());
      APSStatsCollectState.addValue(pd.Status().APS());
      lastCollectSubstate = currCollectState;
   }
   if ((currCollectState == SS_RBC) && (currState != "BloodRinseback"))
   {


      CPSStatsCollectState.addValue(pd.Status().CPS());
      APSStatsCollectState.addValue(pd.Status().APS());
      lastCollectSubstate = currCollectState;
   }
   if (currCollectState == SS_RP_RBC_PTF_SETUP_1)
   {
      CPSStatsCollectState.addValue(pd.Status().CPS());
      APSStatsCollectState.addValue(pd.Status().APS());
      lastCollectSubstate = currCollectState;
   }
   if (currCollectState == SS_RP_RBC_PTF_SETUP_2)
   {
      CPSStatsCollectState.addValue(pd.Status().CPS());
      APSStatsCollectState.addValue(pd.Status().APS());
      lastCollectSubstate = currCollectState;
   }
   if ((currCollectState == SS_RP_RBC) && (currState != "BloodRinseback") )
   {

      CPSStatsCollectState.addValue(pd.Status().CPS());
      APSStatsCollectState.addValue(pd.Status().APS());
      lastCollectSubstate = currCollectState;
   }
   if (currCollectState == SS_RP_RBC_PTF_SETUP_1_DOUBLE)
   {
      CPSStatsCollectState.addValue(pd.Status().CPS());
      APSStatsCollectState.addValue(pd.Status().APS());
      lastCollectSubstate = currCollectState;
   }
   if (currCollectState == SS_RP_RBC_PTF_SETUP_2_DOUBLE)
   {
      CPSStatsCollectState.addValue(pd.Status().CPS());
      APSStatsCollectState.addValue(pd.Status().APS());
      lastCollectSubstate = currCollectState;
   }
   if ((currCollectState == SS_RP_RBC_DOUBLE) && (currState != "BloodRinseback") )
   {


      CPSStatsCollectState.addValue(pd.Status().CPS());
      APSStatsCollectState.addValue(pd.Status().APS());

      lastCollectSubstate = currCollectState;
   }

   /////////////////////////////////////////////////////////////////////////////
   //  Gather the data for some main states
   /////////////////////////////////////////////////////////////////////////////
   if (currState == "DisposableTest")
   {

      CPSStatsState.addValue(pd.Status().CPS());
      APSStatsState.addValue(pd.Status().APS());
      InletPumpStatsState.addValue(pd.Volumes().VinTotal.Get());
      lastState = currState;
   }

   if (currState == "MeteredStorage")
   {

      CPSStatsState.addValue(pd.Status().CPS());
      APSStatsState.addValue(pd.Status().APS());
      InletPumpStatsState.addValue(pd.Volumes().VinTotal.Get());
      lastState = currState;
   }

   /////////////////////////////////////////////////////////////////////////////
   //  Gather the data for select substates
   /////////////////////////////////////////////////////////////////////////////
   if (currSubState == "PrimeChannel")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());
      lastSubstate = currSubState;
   }
   if (currSubState == "PrimeChannel2A")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());
      lastSubstate = currSubState;
   }
   if (currSubState == "PrimeChannel2")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());
      lastSubstate = currSubState;
   }
   if (currSubState == "PrimeChannel3")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());
      lastSubstate = currSubState;
   }
   if (currSubState == "PrimeChannel4")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());
      lastSubstate = currSubState;
   }
   if (currSubState == "PrimeVent")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());
      lastSubstate = currSubState;
   }
   if (currSubState == "RampCentrifuge")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());
      lastSubstate = currSubState;
   }


   if (currSubState == "CheckSampleBag")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());

      ACPumpStats.addValue(pd.Volumes().VacTotal.Get());
      ReturnPumpStats.addValue(pd.Volumes().Vreturn.Get());

      lastSubstate = currSubState;
   }

   if (currSubState == "PressInletLine")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());

      ACPumpStats.addValue(pd.Volumes().VacTotal.Get());

      lastSubstate = currSubState;
   }

   if (currSubState == "InletPressTest2")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());
      lastSubstate = currSubState;

   }
   if (currSubState == "NegativePressRelief")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());


      ACPumpStats.addValue(pd.Volumes().VacTotal.Get());

      lastSubstate = currSubState;
   }

   if (currSubState == "NegPressRelief2")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());

      ReturnPumpStats.addValue(pd.Volumes().Vreturn.Get());
      lastSubstate = currSubState;
   }

   if (currSubState == "NegativePressTest")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());


      InletPumpStats.addValue(pd.Volumes().VinTotal.Get());
      lastSubstate = currSubState;
   }

   if (currSubState == "InletDecayTest2")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());


      ACPumpStats.addValue(pd.Volumes().VacTotal.Get());
      ReturnPumpStats.addValue(pd.Volumes().Vreturn.Get());
      InletPumpStats.addValue(pd.Volumes().VinTotal.Get());
      lastSubstate = currSubState;
   }

   if (currSubState == "PressInletLine2")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());

      ReturnPumpStats.addValue(pd.Volumes().Vreturn.Get());
      lastSubstate = currSubState;
   }


   if (currSubState == "PressInletLine3")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());

      ACPumpStats.addValue(pd.Volumes().VacTotal.Get());
      lastSubstate = currSubState;
   }

   if (currSubState == "InletPressTest")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());
      lastSubstate = currSubState;

   }

   if (currSubState == "InletDecayTest")
   {

      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());


      ACPumpStats.addValue(pd.Volumes().VacTotal.Get());
      ReturnPumpStats.addValue(pd.Volumes().Vreturn.Get());
      InletPumpStats.addValue(pd.Volumes().VinTotal.Get());

      lastSubstate = currSubState;
   }

   if (currSubState == "MSSRBCPrime1")
   {
      CPSStats.addValue(pd.Status().CPS());
      APSStats.addValue(pd.Status().APS());
      lastSubstate = currSubState;
   }





//      if (currSubState == "StartPumps")
//      {
//        APSStats.addValue(pd.Status().APS());
//        lastSubstate = currSubState;
//      }
//      if (currSubState == "DonorClampClosureCheck")
//      {
//        APSStats.addValue(pd.Status().APS());
//        lastSubstate = currSubState;
//      }
//      if (currSubState == "EvacuateSetValves")
//      {
//        APSStats.addValue(pd.Status().APS());
//        lastSubstate = currSubState;
//      }
//      if (currSubState == "EvacuateBags")
//      {
//        APSStats.addValue(pd.Status().APS());
//        lastSubstate = currSubState;
//      }
//
//      if (currSubState == "EvacPlasmaBag")
//      {
//        APSStats.addValue(pd.Status().APS());
//        lastSubstate = currSubState;
//      }
//


   // this circus below is because of the parallel state names
   // i just want MSSRBCAddition, but it comes in different flavors
   if (
      (currSubState == " || VacPurge || MSSRBCAddition")
      || (currSubState == " || VacPurgeFinal || MSSRBCAddition")
      || (currSubState == " || MSSPltAdditionPause || MSSRBCAddition")
      || (currSubState == " || MSSPLTAddition || MSSRBCAddition")
      || (currSubState == " || MSSRBCAddition")
      || (currSubState == "MSSRBCAddition")
      )
   {
      APSStats.addValue(pd.Status().APS());
      CPSStats.addValue(pd.Status().CPS());
      lastSubstate = currSubState;
   }

}


////////////////////////////////////////////////////////////////////
//  Generic Write for all data sets
///////////////////////////////////////////////////////////////////
void ClinicalDataLogMonitor::writeData (Stats& theStats,
                                        const string& theSubstate,
                                        const string& DataExplaination)
{

   ProcData pd;
   bool     black = true;

   if (pd.TrimaSet().Cassette.Get()==cassette::PLTPLSRBC)
      black = false;



   if (theStats.numOfValues() != 0)
   {

      DLOG
         << " STATISTICS FOR " << theSubstate  << " ... "
         << (black ? " (black stamp)... " : " (white stamp)... ")
         << DataExplaination << " avg    -> "  << theStats.average()    << "; "
         << DataExplaination << " high   -> "  << theStats.high()       << "; "
         << DataExplaination << " low    -> "  << theStats.low()        << "; "
         << DataExplaination << " @Entry -> "  << theStats.entryValue() << "; "
         << DataExplaination << " @Exit  -> "  << theStats.exitValue()  << "; "
         << DataExplaination << " number -> "  << theStats.numOfValues() << "; "
         << endmsg;

      theStats.cleanData();
   }
}

void ClinicalDataLogMonitor::enable ()
{
   MonitorBase::enableMonitoring(1);
}


void ClinicalDataLogMonitor::disable ()
{

   ProcData pd;

   MonitorBase::enableMonitoring(0);
} //////////////////////////////////////////////////////////////////////////////////////

/* FORMAT HASH 86e10e8517083024a8990c3e05b0093a */

#include <vxworks.h>
#include <math.h>
#include <float.h>
#include <algorithm>


#include "WBCPurgeMonitor.h"
#include "datalog_periodic.h"
#include "cobeconfig.h"
#include "procdata.h"
#include "rbcdat.h"

// IT 11513 is to remove the settings from cobe_config.dat
// here are the default values so they aren't forgotten
// [WBC_PURGE_MONITOR]
/*
_data.MaxRedThreshold     = 2000.0;
_data.MinDeltaQch         = 0.1;
_data.RedPlsUpperOffset   = 200.0;
_data.RedPlsLowerOffset   = 200.0;
_data.DirtyPlsMultiplier  = 1.0;
_data.DirtyPltSdevMinimum = 10.0;
_data.DirtyPltSdevMaximum = 100.0;
_data.DirtyPurgeTriggerWindowMinimum = 120.0;
_data.DirtyPurgeTriggerWindowMaximum = 1000.0;
_data.PostAPSBlackoutTime = 3.0;
_data.CleanPlsBaseMinimum = 1000.0;
_data.CleanPlsBaseMaximum = 1600.0;
*/


DEFINE_OBJ(WBCPurgeMonitor);


WBCPurgeMonitor::WBCPurgeMonitor()
   : aPQMonitor(),
     _APSAlarmTime(0.0f),
     _apsActive(false),
     _myState(WBCPurgeMonitor::GATHERING_DATA),
     _currentSubstate(INVALID_SUBSTATE),
     _lastsubstate(INVALID_SUBSTATE),
     _collectingData(true),
     _blockJudgement(false),
     _myAirFinder(),
     _myDirtyPlasmaFinder(),
     _productFlagged(false),
     _RedPlt(),
     _vinDuringPurge(0.0)
{
   _RedPlt.cleanData();

   //  set the recoveries to judge here:   add more or delete some if we need to....
   ////////////////////////////////////////////////////////////////////////////////////////////
   _recoverysToJudge.push_back(RecoveryTypes::RecoveryStr(RecoveryTypes::WBCChamberPurge));
   _recoverysToJudge.push_back(RecoveryTypes::RecoveryStr(RecoveryTypes::SettleChannel));
   _recoverysToJudge.push_back(RecoveryTypes::RecoveryStr(RecoveryTypes::ClearLine) );
}

WBCPurgeMonitor::~WBCPurgeMonitor()
{}

void WBCPurgeMonitor::Initialize ()
{}

void WBCPurgeMonitor::Monitor ()
{
   static bool logged = false;

   ProcData    pd;
   if (    !Software_CDS::isPPC(pd.RunTargets().PlateletConcenTarget.Get())
           || !pd.TrimaSet().RBCDetectorCalibrated.Get()
           )
   {

      if (!logged)
      {
         logged = true;
         DataLog(log_level_proc_info)
            << "WBCPurgeMonitor disabled, product not hyperconcentrate "
            << pd.RunTargets().PlateletConcenTarget.Get()
            << endmsg;

      }

      disable();
   }



   State_names currentSubstate = pd.Run().Substate.Get();
   if (   currentSubstate < SS_PLATELET_PLASMA
          || currentSubstate > SS_EXTENDED_PLASMA)
      return;

   clearDataOnStateChange();
   setAPSStatus();

   switch (setMyState())
   {
      case WBCPurgeMonitor::GATHERING_DATA :
         // get the red values prior to a WBC purge event.
         getFilteredRed();
         break;

      case WBCPurgeMonitor::JUDGING_PURGE :
         // check if the purge should flag....
         judgeTheWBCPurge();
         break;

      case WBCPurgeMonitor::BLACKEDOUT :
      case WBCPurgeMonitor::DISABLED :
         break;

      default :
         break;
   }


}
///////////////////////////////////////////////////////////////////////////////

WBCPurgeMonitor::WBCPurgeMonitor_states WBCPurgeMonitor::setMyState ()
{

   ProcData           pd;
   const static float SUFFICIENTLY_SMALL_FLOAT =  0.0001f;


   //  assume data gathering state
   ////////////////////////////////////////////////////////////////////////////////////
   WBCPurgeMonitor::WBCPurgeMonitor_states localState = WBCPurgeMonitor::GATHERING_DATA;


   // check if in blackout for APS state
   /////////////////////////////////////////////////////////////////
   if (_apsActive)
   {
      localState = WBCPurgeMonitor::BLACKEDOUT;
   }

   if (  _APSAlarmTime > SUFFICIENTLY_SMALL_FLOAT  // small delta to avoid float problem
         && pd.ProcRunTime().getMins() - _APSAlarmTime > CobeConfig::data().PostAPSBlackoutTime
         )
   {
      localState = WBCPurgeMonitor::BLACKEDOUT;
   }

   // check if in we are in out recovery to JUDGE
   /////////////////////////////////////////////////////////////
   string localCurrentRecovery = pd.RecoveryName();
   for (int idx = 0; idx < _recoverysToJudge.size(); idx++)
   {
      if (localCurrentRecovery == _recoverysToJudge[idx])
      {
         localState = WBCPurgeMonitor::JUDGING_PURGE;

         if (localCurrentRecovery != _currentRecovery)
         {
            // zero the volume blackout at entry to this state
            if (localCurrentRecovery == RecoveryTypes::RecoveryStr(RecoveryTypes::WBCChamberPurge))
            {
               DataLog(log_level_proc_info) << "entering WBCChamberPurge reset volume counter" << endmsg;
               _vinDuringPurge = 0.0f;
            }

            DataLog(log_level_proc_info)
               << "Recovery to check found; "
               << localCurrentRecovery
               << " was "
               << _currentRecovery
               << endmsg;
         }
      }
   }
   _currentRecovery = localCurrentRecovery;
   ///////////////////////////////////////////////////////////////

   if (_blockJudgement)
   {
      localState = WBCPurgeMonitor::DISABLED;
   }


   if (_productFlagged)
   {

      localState = WBCPurgeMonitor::DISABLED;
      DataLog(log_level_proc_info) << "product flagged, disabling monitor. " << endmsg;
      disable();   // turn us off
   }


   if (localState != _myState)
      DataLog(log_level_proc_info) << "new state: changing wbcpurge_monitor state from "
                                   << dumpMyState(_myState)
                                   << "to "
                                   << dumpMyState(localState)
                                   << endmsg;



   _myState = localState;
   return _myState;
}


///////////////////////////////////////////////////////////////////////////////
void WBCPurgeMonitor::clearDataOnStateChange ()
{

   ProcData pd;

   _currentSubstate = pd.Run().Substate.Get();
   if (_currentSubstate != _lastsubstate)
   {
      DataLog(log_level_proc_info) << "clearing REDplt Data, substate switch from " << _lastsubstate << "to " << _currentSubstate << endmsg;

      DataLog(log_level_proc_info)
         << "Red_plt Data collected was: Average = "
         << (double)_RedPlt.average() << " ("
         << (double)_RedPlt.sum() << "/" << (double)_RedPlt.numOfValues() <<  " ) ;"
         << " Sigma = " << (double)_RedPlt.sigma() <<  "; "
         << " High = " << (double)_RedPlt.high() <<  "; "
         << " Low = " << (double)_RedPlt.low() <<  "; "
         << " Entry = " << (double)_RedPlt.entryValue() <<  "; "
         << " Exit = " << (double)_RedPlt.exitValue() <<  "; "
         << endmsg;

      _RedPlt.cleanData();
      _blockJudgement = false;
      _lastsubstate   = _currentSubstate;
      _vinDuringPurge = 0.0f;
   }

}

///////////////////////////////////////////////////////////////////////////////

void WBCPurgeMonitor::setAPSStatus ()
{

   ProcData pd;

   // mark APS alarms for filtering
   if (pd.AlarmQ().isActive(APS_TOO_HIGH_ALERT)
       || pd.AlarmQ().isActive(APS_TOO_LOW) )
   {
      _APSAlarmTime = pd.ProcRunTime().getMins();

      if (!_apsActive)
         DataLog(log_level_proc_info) << "in aps alarm (blackout RED data collection)" << endmsg;

      _apsActive = true;
   }
   else
   {

      if (_apsActive)
         DataLog(log_level_proc_info)
            << "aps alarm blackout until about "
            << pd.ProcRunTime().getMins() + CobeConfig::data().PostAPSBlackoutTime
            << "  (procRunTime minutes)"
            << endmsg;

      _apsActive = false;
   }

}

///////////////////////////////////////////////////////////////////////////////

void WBCPurgeMonitor::getFilteredRed ()
{
   ProcData    pd;
   double      red             = (double)pd.Status().RedValue();

   State_names currentSubstate = pd.Run().Substate.Get();

   // we dont want all the red values but filtered based on the below conditions:
   if (currentSubstate >= SS_PLATELET_PLASMA && currentSubstate <= SS_EXTENDED_PLASMA)
   {
      if (red < CobeConfig::data().MaxRedThreshold)
      {
         if (pd.TrimaSet().CmdQch.Get() > CobeConfig::data().MinDeltaQch)
         {
            if (!_apsActive)
            {
               if (red > 1.0)
               {
                  _RedPlt.addValue(red);

               } // else  DataLog( log_level_proc_info ) << "Not collecting: RED is Zero" << endmsg;
            }    // else  DataLog( log_level_proc_info ) << "Not collecting: aps alarm blackout" << endmsg;
         }       // else  DataLog( log_level_proc_info ) << "Not collecting: CmdQch too small" << endmsg;
      }          // else   DataLog( log_level_proc_info ) << "Not collecting: above MaxRedThreshold" << endmsg;
   }  // else  DataLog( log_level_proc_info ) << "Not collecting:  Outside state" << endmsg;


}
///////////////////////////////////////////////////////////////////////////////

void WBCPurgeMonitor::judgeTheWBCPurge ()
{

   ProcData pd;



   const float REDpls_base   =  pd.Run().RedPl.Get();
   const float REDpls_upper  =  REDpls_base + CobeConfig::data().RedPlsUpperOffset;
   const float REDpls_lower  =  REDpls_base - CobeConfig::data().RedPlsLowerOffset;
   const float REDplat_base  =  _RedPlt.average();
   const float REDplat_lower =  REDplat_base - (_RedPlt.sigma() * CobeConfig::data().DirtyPlsMultiplier);


   if ( shouldWeSkipJudging(REDpls_base, _RedPlt.sigma(), REDpls_lower, REDplat_lower) )
      return;


   // IT 9747 do check only after we've cleared some or we will always flag!!
   _vinDuringPurge = _vinDuringPurge + pd.Status().InletPump.dV();
   if (_vinDuringPurge < 18.0f )
      return;

   float red = (float)pd.Status().RedValue();


   // requirement:
   // flag the PLT product for WBC if dirty plasma signal detected without
   // detecting the air during purge signal.
   _myAirFinder.setLimits(REDpls_upper, REDplat_lower, false /* outside the window*/);
   _myAirFinder.finderFunc(red);

   _myDirtyPlasmaFinder.setLimits(REDpls_lower, REDplat_lower, true /* inside the window*/);
   _myDirtyPlasmaFinder.finderFunc(red);

   if (_myDirtyPlasmaFinder.flag() && !_myAirFinder.flag())
   {
      DataLog(log_level_proc_info) << "flagging PLT product for WBC contamination because of dirty plasma signal in purge." << endmsg;

      DataLog(log_level_proc_info) << "REDpls_base   = " <<  REDpls_base <<
         ", REDpls_upper  = " <<  REDpls_upper <<
         ", REDpls_lower  = " <<  REDpls_lower <<
         ", REDplat_base  = " <<  REDplat_base <<
         ", REDplat_lower = " <<  REDplat_lower << endmsg;

      DataLog(log_level_proc_info) << "Red value Dirty Plasma dump: " << endmsg;
      _myDirtyPlasmaFinder.dumpArray();
      DataLog(log_level_proc_info) << "Red value Air dump: " << endmsg;
      _myAirFinder.dumpArray();


      DataLog(log_level_proc_info)
         << "Red_plt Data collected was: Average = "  << (double)_RedPlt.average()
         << " (" << (double)_RedPlt.sum() << "/" << (double)_RedPlt.numOfValues() <<  " ) ;"
         << ", Sigma = " << (double)_RedPlt.sigma() <<  "; "
         << endmsg;


      _sout << "COUNT PLATELET PRODUCT because WBC contamination detected";
      LogIt(PLATELET_CONTAMINANT_DETECTION);
      _pqi.PutReason(PLATELET_CONTAMINANT_DETECTION);

      _productFlagged = true;

   }
   else if (_myDirtyPlasmaFinder.flag() && _myAirFinder.flag())
   {
      DataLog(log_level_proc_info) << "dirty plasma signal seen with air in purge signal, no flagging!" << endmsg;


      DataLog(log_level_proc_info) << "REDpls_base   = " <<  REDpls_base <<
         ", REDpls_upper  = " <<  REDpls_upper <<
         ", REDpls_lower  = " <<  REDpls_lower <<
         ", REDplat_base  = " <<  REDplat_base <<
         ", REDplat_lower = " <<  REDplat_lower << endmsg;

      DataLog(log_level_proc_info) << "Red value Dirty Plasma dump: " << endmsg;
      _myDirtyPlasmaFinder.dumpArray();
      DataLog(log_level_proc_info) << "Red value Air dump: " << endmsg;
      _myAirFinder.dumpArray();


      DataLog(log_level_proc_info)
         << "Red_plt Data collected was: Average = "  << (double)_RedPlt.average()
         << " (" << (double)_RedPlt.sum() << "/" << (double)_RedPlt.numOfValues() <<  " ) ;"
         << ", Sigma = " << (double)_RedPlt.sigma() <<  "; "
         << endmsg;

   }
   else if (!_myDirtyPlasmaFinder.flag() && _myAirFinder.flag())
   {
      DataLog(log_level_proc_info) << "air in purge signal seen with NO dirty plasma signal seen, no flagging!" << endmsg;


      DataLog(log_level_proc_info) << "REDpls_base   = " <<  REDpls_base <<
         ", REDpls_upper  = " <<  REDpls_upper <<
         ", REDpls_lower  = " <<  REDpls_lower <<
         ", REDplat_base  = " <<  REDplat_base <<
         ", REDplat_lower = " <<  REDplat_lower << endmsg;

      DataLog(log_level_proc_info) << "Red value Dirty Plasma dump: " << endmsg;
      _myDirtyPlasmaFinder.dumpArray();
      DataLog(log_level_proc_info) << "Red value Air dump: " << endmsg;
      _myAirFinder.dumpArray();


      DataLog(log_level_proc_info)
         << "Red_plt Data collected was: Average = "  << (double)_RedPlt.average()
         << " (" << (double)_RedPlt.sum() << "/" << (double)_RedPlt.numOfValues() <<  " ) ;"
         << ", Sigma = " << (double)_RedPlt.sigma() <<  "; "
         << endmsg;
   }

}

///////////////////////////////////////////////////////////////////////////////

bool WBCPurgeMonitor::shouldWeSkipJudging (float redpls_base,
                                           float redplt_sigma,
                                           float redpls_low,
                                           float redplt_low)
{

   bool result = false;

   if ( !inWindow(redpls_base,
                  CobeConfig::data().CleanPlsBaseMinimum,
                  CobeConfig::data().CleanPlsBaseMaximum) )
   {

      DataLog(log_level_proc_info) << "Disabling monitor because redpls_base ("
                                   << redpls_base
                                   << ") is outside window"
                                   << endmsg;
      disable();
      result = true;
   }



   if ( !inWindow(redplt_sigma,
                  CobeConfig::data().DirtyPltSdevMinimum,
                  CobeConfig::data().DirtyPltSdevMaximum) )
   {


      DataLog(log_level_proc_info) << "Skipping WBCPurge Judgement because redplt_sigma ("
                                   << redplt_sigma
                                   << ") is outside window"
                                   << endmsg;

      result = true;
   }


   float dirtytrigger = (redpls_low - redplt_low);
   if ( !inWindow(dirtytrigger,
                  CobeConfig::data().DirtyPurgeTriggerWindowMinimum,
                  CobeConfig::data().DirtyPurgeTriggerWindowMaximum) )
   {


      DataLog(log_level_proc_info) << "Skipping WBCPurge Judgement because dirtytrigger ("
                                   << dirtytrigger  << "= [" << redpls_low << " - " << redplt_low << "]"
                                   << ") is outside window"
                                   << endmsg;

      result = true;
   }

   if (result == true)
   {
      _blockJudgement = true;
   }
   return result;


}

///////////////////////////////////////////////////////////////////////////////
bool WBCPurgeMonitor::inWindow (float value, float boundry1, float boundry2)
{

   float highend = max(boundry1, boundry2);
   float lowend  = min(boundry1, boundry2);

   if ((value > lowend) && (value < highend))
   {
      return true;
   }
   else
   {
      return false;
   }


}


///////////////////////////////////////////////////////////////////////////////
string WBCPurgeMonitor::dumpMyState (WBCPurgeMonitor_states s)
{
   switch (s)
   {
      case GATHERING_DATA :
         return "GATHERING_DATA";
         break;
      case JUDGING_PURGE :
         return "JUDGING_PURGE";
         break;
      case BLACKEDOUT :
         return "BLACKEDOUT";
         break;
      case DISABLED :
         return "DISABLED";
         break;
      default :
         return "unknown";
         break;
   }
   return "unknown";
}


///////////////////////////////////////////////////////////////////////////////

void WBCPurgeMonitor::enable ()
{
   MonitorBase::enableMonitoring(1);
}

void WBCPurgeMonitor::disable ()
{
   MonitorBase::enableMonitoring(0);
}

/////////////////////////////////////////////////////////////
//  find 5 in a moving 20 window that violate the limits
///////////////////////////////////////////////////
finder::finder()
   : _totalCount(0),
     _FLAGGED(false),
     _vileCount(0),
     _boundry1(0.0),
     _boundry2(0.0),
     _inWindow(true)
{
   for (int i = 0; i < WINDOW; i++ )
   {
      _violatorArray[i] = 0.0f;
   }
}


void finder::setLimits (float boundry1, float boundry2, bool inWindow)
{
   _inWindow = inWindow;
   _boundry1 = boundry1;
   _boundry2 = boundry2;
}


bool finder::CheckVal (float redValue)
{
   bool  res     = false;
   float highend = max(_boundry1, _boundry2);
   float lowend  = min(_boundry1, _boundry2);


   if (     (redValue > lowend)
            && (redValue < highend)
            )
      res = true;
   else
      res = false;

   if (_inWindow)
      return res;
   else
      return (!res);
}

////////////////////////////////////////////////////
void finder::finderFunc (float redvalue)
{

   _totalCount++;

   if (_totalCount < WINDOW)
   {
      _violatorArray[_totalCount] = redvalue;
   }
   else
   {
      shiftArray();
      _violatorArray[WINDOW - 1] = redvalue;
   }

   _vileCount = 0;

   int LIMIT = (_totalCount < WINDOW) ? _totalCount : WINDOW;

   for (int i = 0; i < LIMIT; i++ )
   {
      if (CheckVal(_violatorArray[i]))
         _vileCount++;
   }

   if (_vileCount > MAX_VIOLATORS)
   {
      _FLAGGED = true;
   }
   else
   {
      _FLAGGED = false;
   }



}

void finder::shiftArray ()
{
   for (int i = 1; i < WINDOW; i++ )
      _violatorArray[i - 1] = _violatorArray[i];
   _violatorArray[WINDOW - 1] = 0;
}


void finder::dumpArray ()
{

   DataLog(log_level_proc_info) << "Array Dump -> (collected values=" << ( (_totalCount < WINDOW) ? _totalCount : WINDOW ) << ")";

   for (int i = 1; i < WINDOW; i++ )
      DataLog(log_level_proc_info) << _violatorArray[i]  << ", ";
   DataLog(log_level_proc_info) << endmsg;

}
/////////////////////////////////////////////////////////////////////////

LocalStats::LocalStats()
   : mySum(0.0),
     myEntryValue(0.0),
     myExitValue(0.0),
     myNumber(0),
     myHigh(-10000.0),
     myLow(10000.0),
     mySumSquared(0.0),
     mySigma(0.0)
{}

/////////////////////////////////////////////////////
LocalStats::~LocalStats()
{}

/////////////////////////////////////////////////////
void LocalStats::addValue (double val)
{

   myNumber++;
   // first and last.
   if (myNumber == 1)
      myEntryValue = val;

   myExitValue = val;

   // high and low..
   if (val > myHigh)
      myHigh = val;

   if (val < myLow)
      myLow = val;

   mySum        = mySum + val;
   mySumSquared = mySumSquared + (val * val);

}


/////////////////////////////////////////////////////
double LocalStats::average ()  const
{

   if (myNumber < 1)
   {
      return 0.0;
   }

   return (mySum / (double)myNumber);
}


/////////////////////////////////////////////////////
void LocalStats::cleanData ()
{

   mySum        = 0.0;
   myEntryValue = 0.0;
   myExitValue  = 0.0;
   myNumber     = 0;
   myHigh       = -10000.0;
   myLow        = 10000.0;
   mySumSquared = 0.0;
   mySigma      = 0.0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


double LocalStats::sigma ()
{

   if (numOfValues() < 2)
      return 0.0;

   if (mySumSquared <= (mySum * mySum) / (double)numOfValues())
   {
      mySigma =  0.0;
   }
   else
   {
      mySigma = sqrt( (mySumSquared - (mySum * mySum) / (double)numOfValues() ) / ((double)numOfValues() - 1)  );
   }

   return mySigma;
}

/* FORMAT HASH 53251456ac14ab2ad58ad78d560da96d */

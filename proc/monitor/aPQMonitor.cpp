/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      aPQMonitor.cpp
 *
 * ABSTRACT:  This is the source file for the product quality monitors' base class.  It is derived of class MonitorBase.
 *
 */


#define  PQMONITOR_BASE
#include "aPQMonitor.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "procplsrinseback_cds.h"
#include "pqi_constants.h"


PQControl     aPQMonitor::_pqi;
char          aPQMonitor::_logBuffer[BUFFER_SIZE] = {};
ostrstream    aPQMonitor::_sout(_logBuffer, BUFFER_SIZE, ios::out); // string stream object
DataLog_Level aPQMonitor::_pqLog(log_level_proc_pqi_info);          // DataLog object
bool          aPQMonitor::_Initialized(false);

// Constructor, called at the time the "states" file is read in.
aPQMonitor::aPQMonitor()
   : MonitorBase()
{}

// Destructor
aPQMonitor::~aPQMonitor()
{}

// Monitor function, called each dispatcher cycle
// void Monitor( ) {;}

// Initialization
void aPQMonitor::Initialize ()
{
   // Set static "CDS initialized" flag for the other methods of this and derived classes.
   _Initialized = true;
}

// Function to enable monitoring ...
// void enable() {;}

// Function to disable monitoring ...
// void disable() {;}

// Maps a given flag enumeration value to a string containing the enumeration value's name.
const char* aPQMonitor::theFlagName (ENUMERATED_FLAG theFlag)
{
   return ENUMERATED_FLAG_NAME(theFlag);
}

// Maps a given reason enumeration value to a string containing the enumeration value's name.
const char* aPQMonitor::theReasonName (ENUMERATED_REASON theReason)
{
   return ENUMERATED_REASON_NAME(theReason);
}

// Logs the given reason only if not logged before.
void aPQMonitor::LogReason (ENUMERATED_REASON theReason, const char* file, int line)
{
   _sout << ends;
   if (!_pqi.isReason(theReason))
   {
      _pqLog(file, line) <<  _sout.str()
                         << " Reason="  << theReasonName(theReason) << "(" << (int)theReason << ") "
                         << endmsg;
   }
   _sout.seekp(0); // Clear the log buffer.
}

// Logs the given reason for the given flag only if not logged before.
void aPQMonitor::LogReason (ENUMERATED_REASON theReason, ENUMERATED_FLAG theFlag, const char* file, int line)
{
   _sout << ends;
   if (!_pqi.isReason(theReason, theFlag))
   {
      _pqLog(file, line) <<  _sout.str()
                         << " Flag="  << theFlagName(theFlag) << "(" << (int)theFlag << ") "
                         << " Reason="  << theReasonName(theReason) << "(" << (int)theReason << ") "
                         <<  endmsg;
   }
   _sout.seekp(0); // Clear the log buffer.
}


// This method computes and returns the flow that has been commanded through the LRS chamber
//
float aPQMonitor::LRSChamberCmdFlow () const
{
   ProcData pd;
   if (pd.Status().PlasmaValve.CmdCollect())
      return( pd.Status().CollectPump.CmdFlow() );
   else
      return( pd.Status().CollectPump.CmdFlow() - pd.Status().PlasmaPump.CmdFlow() );

}

// This method computes and returns the flow that has been commanded through the RBC line
//
float aPQMonitor::RbcLineCmdFlow () const
{
   ProcData pd;
   if (pd.TrimaSet().Cassette.Get() != cassette::RBCPLS)
      return( pd.Status().InletPump.CmdFlow() - LRSChamberCmdFlow() - pd.Status().PlasmaPump.CmdFlow() );
   else
      return( pd.Status().InletPump.CmdFlow() - pd.Status().PlasmaPump.CmdFlow() );
}

// This method returns "true" if the procedure has been directing fluid into the platelet bags.  Otherwise, it returns "false".
//
bool aPQMonitor::flowingIntoPlateletBag () const
{
   ProcData pd;
   return( pd.TrimaSet().Cassette.Get() != cassette::RBCPLS
           && pd.Status().CollectValve.CmdCollect()
           );
}

// This method returns "true" if the procedure has been collecting platelets actively.  Otherwise, it returns "false".
//
bool aPQMonitor::nowCollectingPlatelets () const
{
   ProcData pd;
   switch (pd.Run().Substate.Get())
   {
      case SS_PLATELET_PLASMA :
      case SS_PCA :
      case SS_MIDRUN :
      case SS_PIR_PLASMA :
      case SS_PIR_NOPLASMA :
         return( flowingIntoPlateletBag() );
      default :
         return( false );
   }
}

// This method returns "true" if the procedure has been diluting platelets actively.  Otherwise, it returns "false".
//
bool aPQMonitor::nowAdjustingPlateletConcentration () const
{
   ProcData pd;
   switch (pd.Run().Substate.Get())
   {
      case SS_PCA :
      case SS_EXTENDED_PCA :
         return( flowingIntoPlateletBag() );
      default :
         return( false );
   }

}

// This method returns "true" if the procedure has been collecting plasma actively.  Otherwise, it returns "false".
//
bool aPQMonitor::nowCollectingPlasma () const
{
   ProcData    pd;
   State_names currentSubstate = pd.Run().Substate.Get();  // Get the current Collection substate or INVALID_SUBSTATE if not in Collect
   return( ( (currentSubstate > SS_CHANNEL_SETUP && currentSubstate <= SS_RBC)
             || (currentSubstate > SS_RP_CHANNEL_SETUP && currentSubstate <= SS_RP_RBC) )
           && pd.Status().PlasmaValve.CmdCollect()
           );
}

// This method returns "true" if the procedure has been collecting RBCs actively.  Otherwise, it returns "false".
//
bool aPQMonitor::nowCollectingRbc () const
{
   ProcData pd;

   bool     isCmdCollectRBCs = false;
   if ( pd.Status().RBCValve.CmdCollect())
   {
      isCmdCollectRBCs = true;
   }
   else
   {
      isCmdCollectRBCs = false;
   }


   State_names currentSubstate = pd.Run().Substate.Get();  // Get the current Collection substate or INVALID_SUBSTATE if not in Collect
   return( ( (currentSubstate > SS_CHANNEL_SETUP && currentSubstate <= SS_RBC)
             || (currentSubstate > SS_RP_CHANNEL_SETUP && currentSubstate <= SS_RP_RBC) )
           && isCmdCollectRBCs
           );
}



// This method returns "true" if all pumps have been commanded paused (0.0 ml/min).
//
bool aPQMonitor::nowPaused () const
{
   ProcData pd;
   return( ( pd.Status().ACPump.CmdFlow() == 0.0f )
           && ( pd.Status().InletPump.CmdFlow() == 0.0f )
           && ( pd.Status().ReturnPump.CmdFlow() == 0.0f )
           && ( pd.Status().PlasmaPump.CmdFlow() == 0.0f )
           && ( pd.Status().CollectPump.CmdFlow() == 0.0f ) );
}

// Calculate the current instantaneous Red/Green ratio of the RBC detector,
// making sure not to commit a "divide by 0" error.
float aPQMonitor::RGratio () const
{
   ProcData pd;

   // Local constants
   const float red   = (float)pd.Status().RedValue();
   const float green = (float)pd.Status().GreenValue();

   float       ratio;

   // Ensure green value is not zero.
   if (green != 0.0f)
   {
      ratio = red / green;
   }
   // If both green and red are zero then ratio is one.
   else if (red == 0.0f)
   {
      ratio = 1.0f;
   }
   // Otherwise the ratio is very large.
   else
   {
      ratio = VERY_LARGE_RED_GREEN_RATIO;
   }

   return ratio;
}

// Upon being called, processes the particular reason "Run Ended Early" by setting reasons for verify flag(s).
void aPQMonitor::RunEndedEarly ()
{
   ProcData    pd;

   const float PLTfactor     = CobeConfig::data().Vplt1 / 100.0f;
   const float VcolTarget    = pd.RunTargets().SelectedPlateletVolume.Get();
   const float VcolBag       = pd.Volumes().PlateletVolume.Get();

   const float VplasmaTarget = pd.RunTargets().SelectedPlasmaVolume.Get();
   const float plasmaTarget  = plasmaRinsebackEnabled() ? (VplasmaTarget - CobeConfig::data().RinsebackVplrb) : VplasmaTarget;

   const float VplasmaBag    = pd.Volumes().PlasmaVolume.Get();

   const float VrbcTarget    = pd.RunTargets().SelectedRbcVolume.Get();
   const float VrbcBag       = pd.Volumes().RBCVolume.Get();

   const float VrbcBag1of2   = pd.Volumes().RBCProduct_A_FinalVolume.Get();
   const float VrbcBag2of2   = pd.Volumes().RBCProduct_B_FinalVolume.Get();


   if (pd.TrimaSet().Cassette.Get() != cassette::RBCPLS
       && pd.PQI().PlateletsWereCollected.Get()
       && (( VcolTarget - VcolBag ) > (VcolTarget * PLTfactor) ))
   {
      DataLog(_pqLog) <<  (int)RUN_ENDED_EARLY
                      << " VERIFY PLATELET VOLUME because Run Ended Early with Vcollect=" << VcolBag
                      << " which deviates >= " << CobeConfig::data().Vplt1
                      << "% from VcolTarget=" << VcolTarget
                      << endmsg;
      _pqi.AddReasonToFlag(RUN_ENDED_EARLY, VERIFY_PLATELET_YIELD);   // Yield & Volume are the same flag
   }

   DataLog(_pqLog) << "Using Plasma Rinseback Configuration: " << plasmaRinsebackEnabled()
                   << " / " << plasmaTarget << " / " << VplasmaTarget << "." << endmsg;

   if (((plasmaTarget - VplasmaBag) > CobeConfig::data().Vplasma1) &&
       pd.PQI().PlasmaWasCollected.Get())
   {
      DataLog(_pqLog) <<  (int)RUN_ENDED_EARLY
                      << " VERIFY PLASMA VOLUME because Run Ended Early with Vplasma=" << VplasmaBag
                      << " which deviates >= " << CobeConfig::data().Vplasma1
                      << " from VplasmaTarget=" << plasmaTarget
                      << endmsg;
      _pqi.AddReasonToFlag(RUN_ENDED_EARLY, VERIFY_PLASMA_VOL);
   }

   if (VrbcTarget > 0.0f && pd.PQI().RbcWereCollected.Get())
   {
      // we've detected a short RBC single or double somewhere
      if (( VrbcTarget - VrbcBag ) > CobeConfig::data().Vrbc1)
      {
         // IT 8306 - Always determine whether this is prompted split based on configured
         // rbc dose with no scaling or adjustments
         bool doubleTargeted = pd.Config().isPromptedDrbcSplit(pd.RunTargets().ProcNumber.Get());

         bool isRAS          =  pd.RunTargets().MeterRbcs.Get();

         if (doubleTargeted && isRAS)
         {
            // bag one is short blame it:
            if ( (VrbcTarget * 0.5f) - VrbcBag1of2 > CobeConfig::data().Vrbc1)
            {
               DataLog(_pqLog) <<  (int)RUN_ENDED_EARLY
                               << " VERIFY RBC VOLUME because Run Ended Early with Vrbc1of2=" << VrbcBag1of2
                               << " which deviates >= " << CobeConfig::data().Vrbc1
                               << " from VrbcTarget1of2=" << (VrbcTarget * 0.5f)
                               << endmsg;
               _pqi.AddReasonToFlag(RUN_ENDED_EARLY, VERIFY_RBC_VOL);
            }

            // bag two is short blame it
            if ( (VrbcTarget * 0.5f) - VrbcBag2of2 > CobeConfig::data().Vrbc1)
            {
               DataLog(_pqLog) <<  (int)RUN_ENDED_EARLY
                               << " VERIFY RBC VOLUME (PROD2) because Run Ended Early with Vrbc2of2=" << VrbcBag2of2
                               << " which deviates >= " << CobeConfig::data().Vrbc1
                               << " from VrbcTarget2of2=" << (VrbcTarget * 0.5f)
                               << endmsg;
               _pqi.AddReasonToFlag(RUN_ENDED_EARLY, VERIFY_RBC_VOL_PROD2);
            }

         }
         else
         {
            // single so blame only bag one:
            DataLog(_pqLog) <<  (int)RUN_ENDED_EARLY
                            << " VERIFY RBC VOLUME because Run Ended Early with Vrbc=" << VrbcBag
                            << " which deviates >= " << CobeConfig::data().Vrbc1
                            << " from VrbcTarget=" << VrbcTarget
                            << endmsg;
            _pqi.AddReasonToFlag(RUN_ENDED_EARLY, VERIFY_RBC_VOL);
         }
      }
   }
}



// plasmaRinsebackEnabled
bool aPQMonitor::plasmaRinsebackEnabled () const
{
   ProcPlsRinseback_CDS plasmaRinsebackCds(ROLE_RO);

   return (plasmaRinsebackCds.Status != ProcPlsRinseback_CDS::None);  // If state is not none, plasma rinseback occurred
}

/* FORMAT HASH f26438559ba0ddeda4a971c9ac3032b9 */

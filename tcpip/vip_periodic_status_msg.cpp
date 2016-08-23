/*******************************************************************************
 *
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      vip_periodic_status_msg
 *             Vista Interface
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file sends out the Vista Periodic Status Message
 *
 *******************************************************************************/

#include "vip_periodic_status_msg.h"
#include "display_formatter.h"
#include "cassette.h"
#include "catalogchecker.h"

//
// Default Constructor
//
VistaPeriodicStatusMsg::VistaPeriodicStatusMsg()
   : _procStateMsg         (Callback<VistaPeriodicStatusMsg> (this, &VistaPeriodicStatusMsg::HandleProcStateMsg),         MessageBase::SNDRCV_RECEIVE_ONLY),
     _cassettePositionMsg  (Callback<VistaPeriodicStatusMsg> (this, &VistaPeriodicStatusMsg::HandleCassettePositionMsg),  MessageBase::SNDRCV_RECEIVE_ONLY),
     _systemStateMsg       (Callback<VistaPeriodicStatusMsg> (this, &VistaPeriodicStatusMsg::HandleSystemStateChangeMsg), MessageBase::SNDRCV_RECEIVE_ONLY),
     _RefSelectedMsg       (Callback<VistaPeriodicStatusMsg> (this, &VistaPeriodicStatusMsg::HandleRefSelectedMsg),       MessageBase::SNDRCV_RECEIVE_ONLY),

// Datastore instances
     _config_CDS(ROLE_RO),
     _procCycleCds(ROLE_RO),
     _procDonorCds(ROLE_RO),
     _procRunCds(ROLE_RO),
     _procRunTargetsCds(ROLE_RO),
     _procTimeCds(ROLE_RO),
     _procTrimaSetCds(ROLE_RO),
     _procVolumeCds(ROLE_RO),
     _procPlsRinsebackCds(ROLE_RO),

// Init variables
     _cassettePosition((CASSETTE_POSITION_STATUS)-1),
     _systemState((State_names) - 1),
     _procedureNumber (-1),
     _isPtfDrbc(false),
     _isRasSet(false),
     _isDrbcSet(false)
{}



//
// Default Destructor
//
VistaPeriodicStatusMsg::~VistaPeriodicStatusMsg()
{}


//
// Update the Trima Periodic Status Msg
//
void VistaPeriodicStatusMsg::Update (trimaPeriodicStatusMsg& msg)
{
   const float PlateletYieldFactor = 1.0e11;

   // Get values
   msg.targetProcedureTime    = _procRunTargetsCds.ProcTimeTarget.Get();
   msg.currentProcedureTime   = _procTimeCds.ProcRunTime();

   msg.remainingProcedureTime = DisplayFormatter::CalculateRemainingProcedureTime(msg.currentProcedureTime, msg.targetProcedureTime);

   msg.targetPlateletYield    = DisplayFormatter::FormatPlateletYield(_procRunTargetsCds.PlateletYieldTarget.Get());
   msg.currentPlateletYield   = DisplayFormatter::FormatPlateletYield(_procVolumeCds.PlateletYield.Get() / PlateletYieldFactor);

   msg.currentPlasmaVolume    = (int)_procVolumeCds.PlasmaVolume.Get();

   //
   // Set the Plasma Target to the correct value if using plasma rinseback
   //
   msg.targetPlasmaVolume = (int)(_procPlsRinsebackCds.PlasmaRinsebackTarget
                                     (_procPlsRinsebackCds.VPlasmaTarget.Get(),
                                     _procRunTargetsCds.PlasmaTarget.Get(),
                                     _procVolumeCds.PlasmaVolume.Get(),
                                     _procTrimaSetCds.CassetteState.Get()) + 0.5f); // Round up given the integer truncation.


   msg.targetRbcCollectHct = _procRunTargetsCds.RbcCritTarget.Get();

   // Cap the collect crit to 80 to hide crit fudge
   const float hrbc = _procCycleCds.Hrbc.Get();
   msg.currentRbcCollectHct  = (hrbc > 0.8f) ? 0.8f : hrbc;

   msg.targetPlateletVolume  = (int)_procRunTargetsCds.PlateletVolumeTarget.Get();
   msg.currentPlateletVolume = (int)_procVolumeCds.PlateletVolume.Get();

   msg.targetPASVolume       = (int)_procRunTargetsCds.PltSolutionTarget.Get();
   msg.currentPASVolume      = (int)_procVolumeCds.VstoragePltBag.Get();

   // Product Bag AC volumes are truncated by PROC
   msg.currentPlateletAcVolume = (int)_procVolumeCds.VacPlateletBag.Get();
   msg.currentPlasmaAcVolume   = (int)_procVolumeCds.VacPlasmaBag.Get();

   msg.currentRbc1AcVolume     = (int)_procVolumeCds.RBCProduct_A_FinalVac.Get();
   msg.currentRbc2AcVolume     = (int)_procVolumeCds.RBCProduct_B_FinalVac.Get();

   msg.currentRAS1Volume       = (int)_procVolumeCds.RBCProduct_A_FinalVss.Get();
   msg.currentRAS2Volume       = (int)_procVolumeCds.RBCProduct_B_FinalVss.Get();

   msg.totalBloodProcessed     = _procVolumeCds.BloodVolumeProcessed();
   msg.donorTbv                = _procDonorCds.DonorTBV();
   msg.trimaSystemState        = _systemState;

   strcpy(msg.subState,      _procState.stateName());
   strcpy(msg.recoveryState, _procState.recoveryState());

   msg.cassetteType      = _procTrimaSetCds.Cassette.Get();
   msg.cassettePosition  = _cassettePosition;

   msg.selectedProcedure = _procRunTargetsCds.ProcNumber.Get();
   msg.catalogNumber     = _Ref.ref;

   // Set the DRBC split variable
   if (msg.selectedProcedure != _procedureNumber)
   {
      _procedureNumber = msg.selectedProcedure;
      _isPtfDrbc       = _config_CDS.isPromptedDrbcSplit(_procedureNumber);
   }

   DisplayFormatter::CalculateRbcs(_isRasSet,
                                   _isDrbcSet,
                                   _isPtfDrbc,
                                   _procVolumeCds.RBCVolume.Get(),
                                   _procRunTargetsCds.RbcVolumeTarget.Get(),
                                   _procVolumeCds.RBCProduct_A_FinalVolume.Get(),
                                   _procVolumeCds.RBCProduct_B_FinalVolume.Get(),
                                   msg.currentRbcVolume,
                                   msg.targetRbcVolume);

   if (msg.cassetteType == cassette::RBCPLS && _isPtfDrbc)
   {
      // Devide the solution in half.
      const int targetRasVolume = (int)_procRunTargetsCds.RBCSolutionTarget.Get() / 2;
      msg.targetRAS1Volume = targetRasVolume;
      msg.targetRAS2Volume = targetRasVolume;
   }
   else
   {
      msg.targetRAS1Volume = (int)_procRunTargetsCds.RBCSolutionTarget.Get();
      msg.targetRAS2Volume = 0;
   }
}



//
// Callbacks for Trima messages
//
void VistaPeriodicStatusMsg::HandleProcStateMsg (void)
{
   _procState = _procStateMsg.getData();
}



//
// HandleCassettePositionMsg
//
void VistaPeriodicStatusMsg::HandleCassettePositionMsg (void)
{
   _cassettePosition = (CASSETTE_POSITION_STATUS)(int)_cassettePositionMsg.getData();
}



//
// HandleSystemStateChangeMsg
//
void VistaPeriodicStatusMsg::HandleSystemStateChangeMsg ()
{
   _systemState = (State_names)(int)_systemStateMsg.getData();
}



//
// HandleRefSelectedMsg
//
void VistaPeriodicStatusMsg::HandleRefSelectedMsg ()
{
   _Ref       = _RefSelectedMsg.getData();

   _isRasSet  = CatalogChecker::checkBit(_Ref.functBits, CatalogChecker::MSS_RBC);
   _isDrbcSet = CatalogChecker::checkBit(_Ref.functBits, CatalogChecker::DRBC_BAG);
}

/* FORMAT HASH 455f916dec2924450413ca083fba18e0 */

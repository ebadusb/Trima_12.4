/*******************************************************************************
 *
 * Copyright (c) 2002 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      trimamessages.h
 *             Trima specific Messages
 *
 *
 ******************************************************************************/


#if !defined( INCLUDE_TRIMAMESSAGES )
#define INCLUDE_TRIMAMESSAGES

#include <vxworks.h>

#include "message.h"
#include "objdictionary.h"

#define INT_MESSAGE(__name__) \
   class __name__\
      : public Message< long > \
   { \
      DECLARE_OBJ(__name__); \
   public: \
      __name__()\
         : Message<long>() { } \
      __name__(MessageBase::SendType t)\
         : Message<long>() { init(t); } \
      __name__(const CallbackBase &cb, \
               MessageBase::SendReceiveType t = MessageBase::SNDRCV_RECEIVE_ONLY)   \
         : Message<long>() { init(cb, t); } \
      virtual ~__name__() { }; \
      inline operator long () const { return getData(); };\
   }

//
// Messages are listed in alphabetical order...
//
//           MESSAGE NAME                               FROM         TO          DESCRIPTION
INT_MESSAGE(AasAdditionVolumeUpdate);                // PROC -> GUI  Proc tells GUI how much SS (RAS+PAS) has been added.
INT_MESSAGE(ACConnectedMsg);                         // GUI       -> Procedure   Operator confirms connection of AC
INT_MESSAGE(AckRunCompleteMsg);                      // GUI       -> Procedure   Acknowledge run complete condition at end of BLOOD RINSEBACK
INT_MESSAGE(AckTestCompletedMsg);                    // GUI       -> Procedure   Acknowledge TestCompleted message
INT_MESSAGE(ACPrimeCompletedMsg);                    // Procedure -> GUI         AC Prime is completed
INT_MESSAGE(AirInPlasmaLineMsg);                     // GUI       -> Procedure   Operator presses 'Air in Plasma Line' button
INT_MESSAGE(AKOMsg);                                 // proc      -> proc:  message to request ThreeMinutePause action of Proc
INT_MESSAGE(AlarmHandlerStatusMsg);                  // Procedure -> Procedure   Check status of the alarm handler process
INT_MESSAGE(AlarmResponseMsg);                       // GUI       -> Procedure   Operator responds to an alarm condition (ABORT/CONTINUE for 2.2)
INT_MESSAGE(AlarmStateChangeMsg);                    // GUI       -> GUI         Alarm handler to alarm screen state change information
INT_MESSAGE(AnswerGUIAliveMsg);                      // GUI       -> Procedure   Respond to a Query GUI Alive message
INT_MESSAGE(AnswerSafetyAliveMsg);                   // Safety    -> Procedure   Respond to a Query Safety Alive message
INT_MESSAGE(APSDecoupledPauseMsg);                   // Proc                     APS decoupled 6 second pause
INT_MESSAGE(APSOutOfBoundsMsg);                      // Hal -> Proc sending the APS value that was out of range
INT_MESSAGE(APSZeroMsg);                             // Proc            -> Hal, Safety New APS zero value
INT_MESSAGE(BarcodeScannedMsg);                      // requestor ->
INT_MESSAGE(BasinTempMsg);                           // Proc (basintemp process) -> anyone who want our current
INT_MESSAGE(BroadcastStartMsg);                      // TCPIP     -> TrimaBroadcast  Start external Broadcast messages
INT_MESSAGE(BroadcastStopMsg);                       // TCPIP     -> TrimaBroadcast  Stop external Broadcast messages
INT_MESSAGE(CancelProcedureMsg);                     // GUI       -> Procedure   Operator requests an abort of the procedure
INT_MESSAGE(CancelMssMsg);                           // GUI       -> Procedure   Operator requests an abort of the procedure
INT_MESSAGE(CassettePositionStatusMsg);              // proc -> GUI current cassette action
INT_MESSAGE(CentrifugePressureStatusMsg);            // Procedure -> GUI         Centrifuge pressure reading in mmHg
INT_MESSAGE(CentrifugeStopMsg);                      // requestor -> Proc for a Centrifuge Stop recovery
INT_MESSAGE(ClampedForTestingMsg);                   // GUI       -> Procedure   Operator confirms that the disposable is clamped and disposable test may start
INT_MESSAGE(ClearLineCompleteMsg);                   // Proc -> anyone waiting for a ClearLine recovery to complete
INT_MESSAGE(ClearLineMsg);                           // requestor -> Proc for a ClearLine recovery (0 = Unused, 1 = Spillover, 2 = Standard)
INT_MESSAGE(ClearRefNumberMsg);                      // GUI -> Proc to clear the ref Number and function bits in the CDS
INT_MESSAGE(ClearStatusLineMsg);                     // Alarm Handler -> GUI  Clears a status line in an alarm condition (without the alarm screen)
INT_MESSAGE(ConfirmBoxConfirmMsg);                   // GUI -> GUI  Confirm button chosen on confirm box.
INT_MESSAGE(ConfirmBoxCancelMsg);                    // GUI -> GUI  Cancel button chosen on confirm box.
INT_MESSAGE(ConfigChangedMsg);                       // GUI or EOT config.dat changes complete.
INT_MESSAGE(ConfirmDisposableLoadedMsg);             // GUI       -> Procedure   Operator has confirmed that the disposable is loaded
INT_MESSAGE(ConfirmDonorDisconnectedMsg);            // Procedure -> GUI         Procedure confirms donor disconnected condition
INT_MESSAGE(ConfirmEnableLoadSystemMsg);             // GUI       -> Procedure   GUI confirms that the Enable Load System has been accepted
INT_MESSAGE(CrossoverClampedMsg);                    // GUI       -> Proc  the crossover line on MSS sets is closed for disp test
INT_MESSAGE(CloseCrossoverClampMsg);                 // Proc -> GUI  put up the close crossover clamp msg.
INT_MESSAGE(DateTimeSetMsg);                         // anyone    -> time setting task.  Data is seconds since 1/1/1970 UTC.
INT_MESSAGE(DeleteProductMsg);                       // anyone    -> proc delete specified product
INT_MESSAGE(DisconnectCompleteMsg);                  // GUI       -> Procedure   Operator confirms disconnect complete
INT_MESSAGE(DisposableIDMsg);                        // proc      -> safety current cassette type (enum cast to int)
INT_MESSAGE(DisposableLoweredMsg);                   // Procedure -> GUI         The disposable is lowered
INT_MESSAGE(DisposableRaisedMsg);                    // Procedure -> GUI         The disposable is Raised.
INT_MESSAGE(DonorConnectedMsg);                      // GUI       -> Procedure   Operator confirms donor is connected
INT_MESSAGE(DonorDataStatusMsg);                     // GUI       -> GUI from donor data object to donor information screens
INT_MESSAGE(DonorDisconnectedMsg);                   // GUI       -> Procedure   Operator confirms donor is disconnected
INT_MESSAGE(DoPredictionMsg);                        // Procedure -> Procedure   A parameter has changed, do the prediction again.
INT_MESSAGE(EnableFastDataMsg);                      // requestor --> drivers to request fastdata message
INT_MESSAGE(EnableLoadSystemMsg);                    // Procedure -> GUI         Procedure has completed startup tests - allows load system button
INT_MESSAGE(ErasePinFileMsg);                        // VIP       -> Boimetric   Erase PIN file.
INT_MESSAGE(FeatureChanged);                         // Software CDS --> Anyone: Notify of a software cds feature value being changed
INT_MESSAGE(FinishedClampFromARMMsg);                // gui -> proc     showed the PLT/RBC clamping for ARM
INT_MESSAGE(FinishedViewingStatsMsg);                // GUI       -> Procedure   Operator has completed viewing end run summary
INT_MESSAGE(GetPointerServiceMsg);                   // TED       -> Service     EMS sends this message to get the pointer coordinates during the touch screen calibration
INT_MESSAGE(GUIRequestBeginRunMsg);                  // GUI       -> Procedure   GUI requests run transition after AC Prime is completed
INT_MESSAGE(GUIRequestRaiseCassetteMsg);             // GUI       -> Procedure   GUI requests the procedure to raise the cassette
INT_MESSAGE(GUIRequestStartDrawMsg);                 // GUI       -> Procedure   Operator presses "Start Draw" button indicating blood sample complete and ready for Blood Prime
INT_MESSAGE(GUISysDonorStateMsg);                    // proc --> GUI notify for the sys donor screen status at PFR
INT_MESSAGE(GuiProceduresUpdated);                   // PREDICT --> GUI: tell gui that predict has updated the procedure info.  Payload=same as GuiToProcPredictRequest
INT_MESSAGE(GuiToProcDonorTimeMsg);                  // GUI       -> proc operator has changed donor time
INT_MESSAGE(GuiToProcFlowAdjustmentMsg);             // GUI       -> proc operator has adjusted flow rate on predict screen
INT_MESSAGE(GuiToProcPredictRequestMsg);             // GUI       -> proc request for optimization
INT_MESSAGE(Gui2ProcRequestRemainingPasVolMsg);      // PROC --> GUI  Hey, proc: can I get some remaining pas volume data?
INT_MESSAGE(InitializeInternalStatesMsg);            // GUI       -> GUI
INT_MESSAGE(InletPressureStatusMsg);                 // Procedure -> GUI         Inlet pressure reading in mmHg
INT_MESSAGE(MidCycleSwitchMsg);                      // proc      -> safety indicates a cycle switch initiated by proc code instead
INT_MESSAGE(MSSBeginGuiMsg);                         // proc -> GUI  starts the Metered storage GUI path
INT_MESSAGE(MSSPFRForceClampScreen);                 // Proc -> GUI  proc forces gui to mss ClampClosure screen screen on PFR
INT_MESSAGE(MSSSetupMsg);                            // GUI -> Proc  Tells proc to begin mss cassette tests
INT_MESSAGE(MSSSetupCompleteMsg);                    // proc -> GUI  MSS tests complete message to GUI to change screens
INT_MESSAGE(MSSConnectedMsg);                        // GUI -> Proc  Operator indicates solutions are connected
INT_MESSAGE(MSSPFRtoSealAndDetach);                  // Proc -> GUI  proc forces gui to mss disconnect screen on PFR
INT_MESSAGE(MSSUnloadMsg);                           // Proc -> GUI  proc tells GUI the cassette is unloaded
INT_MESSAGE(MSSConfirmUnloadMsg);                    // GUI -> proc  operator confirms  unload
INT_MESSAGE(MSSVacPurgeMsg);                         // proc -> GUI  MSS has entered Vac Purge
INT_MESSAGE(MSSVacPurgeCompleteMsg);                 // Proc -> GUI MSS has completed Vac Purge (update GUI)
INT_MESSAGE(MSSReadyPltAdditionMsg);
INT_MESSAGE(MSSPASAdditionMsg);                     // Proc -> GUI update GUI to reflect MSS addition has started in proc.
INT_MESSAGE(MSSRASAdditionMsg);                     // Proc -> GUI update GUI to reflect MSS addition has started in proc.
INT_MESSAGE(MSSCompletionMsg);                      // Proc -> GUI metering has been completed
INT_MESSAGE(MSSAckCompletionMsg);                   // GUI -> proc Operator confirms MSS Complete
INT_MESSAGE(MSSEndRunMsg);                          // GUI -> Proc Operator presses the endRun for Metered Storage
INT_MESSAGE(MSSPltAddedMsg);                        // PROC -----> GUI : Tell gui how much platelet solution has been added so we can keep the meter up to date.
INT_MESSAGE(MSSRbcAddedMsg);                        // PROC -----> GUI : Tell gui how much RBC solution has been added so we can keep the meter up to date.
INT_MESSAGE(MSSWriteBagVolPLT);                     // GUI -----> Proc : Write override default PLT bag volume.
INT_MESSAGE(MSSWriteBagVolRBC);                     // GUI -----> Proc : Write override default RBC bag volume.
INT_MESSAGE(MovePointerServiceMsg);                 // TED       -> Service     EMS sends this message to move the pointer during the touch screen calibration
INT_MESSAGE(NewControlDataMsg);                     // Procedure -> Procedure   Internal command (algos->VD)
INT_MESSAGE(NotifySafetyOfA2Msg);                   // Alarm handler ---> safety when an A2 alarm is serviced
INT_MESSAGE(OperatorSetIdentificationMsg);          // GUI       -> Procedure: relay operator's ID of a machine-unreadable set.
INT_MESSAGE(OptimizerAliveMsg);                     // optimizer -> task, opt task is alive
INT_MESSAGE(OptimizerAreYouOkMsg);                  // predict   -> optimizer: notify me that you are okay....
INT_MESSAGE(OptimizerSelectedMsg);                  // optimizer -> predict: I have been selected successfully (IT4080)
INT_MESSAGE(PartialCancellationMsg);                // GUI --> PROC GUI tells proc to cancel the current procedure (int enum in guiproc.h).
INT_MESSAGE(PendingCassetteAlarmMsg);               // proc -> proc raise alarm after cassette raised
INT_MESSAGE(PFRForceAirToDonorMonitorMsg);          // proc      -> control driver, force control driver to start air to donor monitoring
INT_MESSAGE(PFRSubstateMsg);                        // Proc -> predict_jr - notify of substate at time of power fail
INT_MESSAGE(PFRToOccurMsg);                         // proc      -> AP2 proc will powerfail recover
INT_MESSAGE(PFRStatus);                             // proc      -> any.   Trima PFR status (PFR happened, PFR didn't happen, PFR failed)
INT_MESSAGE(PredictDebugMsg);                       // anyone -> predict... Turn on/off any debug logging
INT_MESSAGE(PredictNextProcedureMsg);               // predict -> predict wake up and predict next procedure
INT_MESSAGE(PredictRespondedMsg);                   // predict -> proc  ready for next command
INT_MESSAGE(ProcAlarmMsg);                          // Any domain-> Procedure   Command procedure to set specified alarm
INT_MESSAGE(ProcDescEndMsg);                        // Vista --> Predict, Sending Procedures End
INT_MESSAGE(ProcDescStartMsg);                      // Vista --> Predict, Sending Procedures Start
INT_MESSAGE(ProcedureAdjustmentMsg);                // GUI       -> Procedure   Operator requests adjustment (pressures, clumping, tingling)
INT_MESSAGE(ProcedureAdjustmentStatusMsg);          // Procedure -> GUI         Adjustment screen status update request
INT_MESSAGE(ProcedureSelectedMsg);                  // Predict -> GUI         Indicate which procedure has been selected by operator.  Used for CFR 11
INT_MESSAGE(ProcedureDebugMsg);                     // anyone -> proc...Turn on/off any debug logging
INT_MESSAGE(ProcStateMsg);                          // Any domain-> Procedure   Command procedure to go to specified state
INT_MESSAGE(ProcSubstateChangeMsg);                 // proc      -> anyone new substate now in effect
INT_MESSAGE(ProcToGuiAasPfrMsg);                    // GUI->PROC	Proc tells GUI which AAS screen it should go to.
INT_MESSAGE(ProcToGuiPredictStatusMsg);             // procedure -> GUI optimization status information
INT_MESSAGE(Proc2GuiPfrSetFuncBits);                // PROC -> GUI  Proc tells GUI about the functionality of the disposable set.
INT_MESSAGE(Proc2GuiSendRemainingPasVolDataMsg);    // GUI --> PROC  Here's that pas volume remaining data you requested, GUI.
INT_MESSAGE(ProductSelectionMsg);                   // proc      -> AP2 - Product Selection
INT_MESSAGE(QueryGUIAliveMsg);                      // Procedure -> GUI         Procedure queries GUI for a ready response at power-up
INT_MESSAGE(QuerySafetyAliveMsg);                   // Procedure -> Safety      Procedure queries Safety for a ready response at power-up
INT_MESSAGE(RasConnectedPasToBeConnected);          // GUI->PROC   GUI tells proc that RAS is connected, and PAS connection is still to come.
INT_MESSAGE(RBCCalibrationRequestMsg);              // Proc --> RBC initialer object
INT_MESSAGE(RBCChamberPurgeMsg);                    // requestor -> Proc for an RBC chamber purge
INT_MESSAGE(RBCDetectorUncalibratedMsg);            // Proc -> anyone saying the RBC Detector is out of cal
INT_MESSAGE(RBCRestoreCalibrationMsg);              // proc -> RBC initialization object - restore after power fail
INT_MESSAGE(RBCSetupNeededMsg);                     // requestor -> proc perform an RBC setup before collection continues
INT_MESSAGE(RbcSpilloverRequestMsg);                // Proc --> Proc: Do a wbc chamber purge as part of the Rbc spillover recovery initiated by GUI
INT_MESSAGE(RemoveAllChildScreensMsg);              // GUI       -> GUI         remove all child screens from child stack
INT_MESSAGE(RemoveAllStatelessScreensMsg);          // GUI       -> GUI         remove all stateless screens from the stateless stack
INT_MESSAGE(RemoveMostRecentChildScreenMsg);        // GUI       -> GUI         remove the last displayed child from stack
INT_MESSAGE(RemoveMostRecentStatelessScreenMsg);    // GUI       -> GUI         remove the last displayed stateless screen from stack
INT_MESSAGE(RepFluidConnectStatusMsg);              // GUI       -> Procedure: notify proc whether operator connected Rep. Fluid or not
INT_MESSAGE(RequestCancelMssPfr);                   // GUI --> PROC  Operator hit "end run" on PFR alarm.  Deal with that, mister.
INT_MESSAGE(RequestEndRunStatsMsg);                 // GUI       -> Procedure   Request procedure to send end run statistics
INT_MESSAGE(RequestGenericWbcAlarm);                // PROC --> PROC  Tell the alarm handler that we want to display our generic WBC alarm.
INT_MESSAGE(RequestHardwareVersionMsg);             // anyone    -> drivers Request causes drivers to respond with hardware version structure
INT_MESSAGE(RequestSafetyDriverStatusMsg);          // Safety    -> SAFE_DRV    request status
INT_MESSAGE(RequestSafetyPowerMsg);                 // Procedure -> Safety      Request the change of some Power status.
INT_MESSAGE(RequestScreenCal);                      // TED -->  Service: tell service mode to initiate a screen calibration.
INT_MESSAGE(RetOccMonitorStatusMsg);                // Alarm handler <-> retoccmonitor - check status of return line occlusion monitor
INT_MESSAGE(RinsebackCompleteMsg);                  // proc      -> vista:
INT_MESSAGE(RinsebackRequestMsg);                   // GUI       -> proc:  message to request transition to rinseback
INT_MESSAGE(RinsebackStatusMsg);                    // proc<     -> GUI is rinseback available handshake
INT_MESSAGE(RunCompleteMsg);                        // Procedure -> GUI         Indicates run is finished (end of Blood Rinseback)
INT_MESSAGE(SafetyPFRFileStatusMsg);                // Safety    -> Procedure   Reports status of safety power fail save file
INT_MESSAGE(SafetyServiceModeMsg);                  // Service   -> Safety sends this message to the control side once the safety service mode boot-up is complete.
INT_MESSAGE(SafetyTestDoneMsg);                     // Safety    -> Procedure
INT_MESSAGE(SafetyDisconnectStatMsg);               // Saftey    -> Proc:   reports safety donor disconnect status to proc.
INT_MESSAGE(SalineBolusMsg);                        // requestor -> Proc for a Saline Bolus recovery
INT_MESSAGE(SalineDrawRecoveryMsg);                 // Proc      -> Proc for to refill the reservoir after draw too long alert
INT_MESSAGE(SendOrdersMsg);                         // requestor -> Proc to request an orders message be sent
INT_MESSAGE(SetCtlStatusSentTimestamp);             // TED -->  Driver: tell the control driver that we've sent a control status message from TED.
INT_MESSAGE(SetListboxItemChosen);                  // GUI -> GUI  Cassette entry lisbox data stored in guiglobs.
INT_MESSAGE(SetStatusLineMsg);                      // Alarm Handler -> GUI  Sets a status line in an alarm condition (without the alarm screen)
INT_MESSAGE(SettleChannelMsg);                      // requestor -> Proc for a SettleChannel recovery
INT_MESSAGE(ShowClampFromARMMsg);                   // proc -> gui    show the PLT/RBC clamping for ARM
INT_MESSAGE(ShowTimeoutPredictScreen);              // proc -> GUI: autoflow .                                                           //
INT_MESSAGE(SnapScreenshotMsg);                     // anyone    -> GUI         Snap a screenshot of currently displayed window(s)
INT_MESSAGE(SoundMsg);                              // Safety    -> SafetyHardware (audio)
INT_MESSAGE(SpilloverDetectedMsg);                  // requestor -> Proc  Inform that a spillover was detected by SW
INT_MESSAGE(SpilloverRecoveryMsg);                  // GUI       -> Procedure   Operator presses 'Spillover Recovery" button
INT_MESSAGE(StackMonitorTaskEndMsg);                // Proc -->  SUMO on Control and Safety: tell Sumo to exit.
INT_MESSAGE(StackMonitorTaskStartMsg);              // Proc -->  Safety: tell Safety to run the Stack Monitor task.
INT_MESSAGE(StatLineRequestMsg);                    // Proc-> GUI to get a stat line change for a no-display alarm IT11825
INT_MESSAGE(StateChangeCompleteMsg);                // proc      -> proc  state manager to task manager sync for power fail
INT_MESSAGE(StopGoodDogMsg);                        // TED       -> GoodDog     Tell Gooddog to stop pumping the watchdogs effectively rebooting the machine
INT_MESSAGE(STSConnectedMsg);                       // TED --> Meter:  STS is connected so start listening for the Hardware Status messages.
INT_MESSAGE(SwitchToPreviousScreenMsg);             // GUI       -> GUI         switch to the previously displayed procedure screen
INT_MESSAGE(SystemStateChangeMsg);                  // Procedure -> any domain  Procedure updates system state status
INT_MESSAGE(SystemStateChangeRequestMsg);           // GUI       -> Procedure,Safety    GUI requests a change to specified system state
INT_MESSAGE(TaskManAliveMsg);                       // Procedure -> Procedure   Tells when the task manager is alive.
INT_MESSAGE(TellGuiThatDisconnectPassed);           // PROC --> GUI Tell GUI that the donor disconnect has passed.
INT_MESSAGE(TellProcToTransitionFromDisconnect);    // GUI -->  PROC Tell PROC that the operator says we're ready to transition.
INT_MESSAGE(TestCompletedMsg);                      // Procedure -> GUI         Disposable tests have completed (passed)
INT_MESSAGE(TimeJumpControlMsg);                    // timeset task -> drivers.  Temporary override control-loop calculations during timeset.
INT_MESSAGE(TransitionValidationMsg);               // Safety    -> Procedure
INT_MESSAGE(TriggerSetRunTypeMsg);                  // GUI -----> Proc : Ask proc to send the SetRunTypeMsg.
INT_MESSAGE(UnsuccessfulCassetteLoweringMsg);       // procedure -> GUI         unable to load cassette
INT_MESSAGE(UpdateAutoFlowTimerMsg);                // Predict -> proc: reset the autoflow alert timer.
INT_MESSAGE(VerifyAcClosureCompleteMsg);            // PROC --> GUI The pre-mss verify AC closure test completed sucessfully
INT_MESSAGE(VistaDonorCommittedMsg);                // GUI -> VIP  Donor Committed.
INT_MESSAGE(VistaDonorRejectedMsg);                 // GUI -> VIP  Donor Rejected
INT_MESSAGE(VistaNumberOfNewPinsMsg);               // VIP CMD -> GUI  New PIN list size
INT_MESSAGE(VistaWritePinFileMsg);                  // VIP CMD -> GUI  New PIN list is completed
INT_MESSAGE(VistaConnectionStatusMsg);              // VIP CMD -> GUI  Vista is connected or disconnected

// No need to add to the bottom on this list.  Add alphabetically.


#if (CPU==SIMNT) // This include file has defs for sim-only messages.
#include "../unit_test/gui/h/simIntMessages.hpp"
#endif // #if (CPU==SIMNT)

#endif // #if !defined( INCLUDE_TRIMAMESSAGES )

#ifdef _TRIMA_MESSAGES_CPP_

DEFINE_OBJ(ACConnectedMsg);
DEFINE_OBJ(AckRunCompleteMsg);
DEFINE_OBJ(AckTestCompletedMsg);
DEFINE_OBJ(ACPrimeCompletedMsg);
DEFINE_OBJ(AirInPlasmaLineMsg);
DEFINE_OBJ(AKOMsg);
DEFINE_OBJ(AlarmHandlerStatusMsg);
DEFINE_OBJ(AlarmResponseMsg);
DEFINE_OBJ(AlarmStateChangeMsg);
DEFINE_OBJ(AnswerGUIAliveMsg);
DEFINE_OBJ(AnswerSafetyAliveMsg);
DEFINE_OBJ(APSDecoupledPauseMsg);
DEFINE_OBJ(APSOutOfBoundsMsg);
DEFINE_OBJ(APSZeroMsg);
DEFINE_OBJ(BarcodeScannedMsg);
DEFINE_OBJ(BasinTempMsg);
DEFINE_OBJ(BroadcastStartMsg);
DEFINE_OBJ(BroadcastStopMsg);
DEFINE_OBJ(CancelProcedureMsg);
DEFINE_OBJ(CancelMssMsg);
DEFINE_OBJ(CassettePositionStatusMsg);
DEFINE_OBJ(CentrifugePressureStatusMsg);
DEFINE_OBJ(CentrifugeStopMsg);
DEFINE_OBJ(ClampedForTestingMsg);
DEFINE_OBJ(ClearLineCompleteMsg);
DEFINE_OBJ(ClearLineMsg);
DEFINE_OBJ(ClearRefNumberMsg);
DEFINE_OBJ(ClearStatusLineMsg);
DEFINE_OBJ(ConfigChangedMsg);
DEFINE_OBJ(ConfirmBoxCancelMsg);
DEFINE_OBJ(ConfirmBoxConfirmMsg);
DEFINE_OBJ(ConfirmDisposableLoadedMsg);
DEFINE_OBJ(ConfirmDonorDisconnectedMsg);
DEFINE_OBJ(ConfirmEnableLoadSystemMsg);
DEFINE_OBJ(CrossoverClampedMsg);
DEFINE_OBJ(CloseCrossoverClampMsg);
DEFINE_OBJ(DateTimeSetMsg);
DEFINE_OBJ(DeleteProductMsg);
DEFINE_OBJ(DisconnectCompleteMsg);
DEFINE_OBJ(DisposableIDMsg);
DEFINE_OBJ(DisposableLoweredMsg);
DEFINE_OBJ(DisposableRaisedMsg);
DEFINE_OBJ(DonorConnectedMsg);
DEFINE_OBJ(DonorDataStatusMsg);
DEFINE_OBJ(DonorDisconnectedMsg);
DEFINE_OBJ(DoPredictionMsg);
DEFINE_OBJ(EnableFastDataMsg);
DEFINE_OBJ(EnableLoadSystemMsg);
DEFINE_OBJ(ErasePinFileMsg);
DEFINE_OBJ(FeatureChanged);
DEFINE_OBJ(FinishedClampFromARMMsg);
DEFINE_OBJ(FinishedViewingStatsMsg);
DEFINE_OBJ(Gui2ProcRequestRemainingPasVolMsg);
DEFINE_OBJ(GUIRequestBeginRunMsg);
DEFINE_OBJ(GUIRequestRaiseCassetteMsg);
DEFINE_OBJ(GUIRequestStartDrawMsg);
DEFINE_OBJ(GUISysDonorStateMsg);
DEFINE_OBJ(GuiToProcDonorTimeMsg);
DEFINE_OBJ(GuiProceduresUpdated);
DEFINE_OBJ(GuiToProcFlowAdjustmentMsg);
DEFINE_OBJ(GuiToProcPredictRequestMsg);
DEFINE_OBJ(InitializeInternalStatesMsg);
DEFINE_OBJ(InletPressureStatusMsg);
DEFINE_OBJ(MidCycleSwitchMsg);
DEFINE_OBJ(MovePointerServiceMsg);
DEFINE_OBJ(MSSBeginGuiMsg);
DEFINE_OBJ(MSSSetupMsg);
DEFINE_OBJ(MSSSetupCompleteMsg);
DEFINE_OBJ(MSSConnectedMsg);
DEFINE_OBJ(MSSPFRtoSealAndDetach);
DEFINE_OBJ(MSSUnloadMsg);
DEFINE_OBJ(MSSConfirmUnloadMsg);
DEFINE_OBJ(MSSVacPurgeMsg);
DEFINE_OBJ(MSSVacPurgeCompleteMsg);
DEFINE_OBJ(MSSReadyPltAdditionMsg);
DEFINE_OBJ(MSSPASAdditionMsg);
DEFINE_OBJ(MSSRASAdditionMsg);
DEFINE_OBJ(MSSCompletionMsg);
DEFINE_OBJ(MSSAckCompletionMsg);
DEFINE_OBJ(MSSEndRunMsg);
DEFINE_OBJ(MSSPltAddedMsg);
DEFINE_OBJ(MSSRbcAddedMsg);
DEFINE_OBJ(MSSWriteBagVolPLT);
DEFINE_OBJ(MSSWriteBagVolRBC);
DEFINE_OBJ(NewControlDataMsg);
DEFINE_OBJ(NotifySafetyOfA2Msg);
DEFINE_OBJ(OperatorSetIdentificationMsg);
DEFINE_OBJ(OptimizerAliveMsg);
DEFINE_OBJ(OptimizerAreYouOkMsg);
DEFINE_OBJ(OptimizerSelectedMsg);
DEFINE_OBJ(PartialCancellationMsg);
DEFINE_OBJ(PendingCassetteAlarmMsg);
DEFINE_OBJ(PFRForceAirToDonorMonitorMsg);
DEFINE_OBJ(PFRSubstateMsg);
DEFINE_OBJ(PFRToOccurMsg);
DEFINE_OBJ(PFRStatus);
DEFINE_OBJ(PredictDebugMsg);
DEFINE_OBJ(PredictNextProcedureMsg);
DEFINE_OBJ(PredictRespondedMsg);
DEFINE_OBJ(Proc2GuiPfrSetFuncBits);
DEFINE_OBJ(Proc2GuiSendRemainingPasVolDataMsg);
DEFINE_OBJ(ProcAlarmMsg);
DEFINE_OBJ(ProcDescEndMsg);
DEFINE_OBJ(ProcDescStartMsg);
DEFINE_OBJ(ProcedureAdjustmentMsg);
DEFINE_OBJ(ProcedureAdjustmentStatusMsg);
DEFINE_OBJ(ProcedureSelectedMsg);
DEFINE_OBJ(ProcedureDebugMsg);
DEFINE_OBJ(ProcStateMsg);
DEFINE_OBJ(ProcSubstateChangeMsg);
DEFINE_OBJ(ProcToGuiAasPfrMsg);
// DEFINE_OBJ(ProcToGuiCassetteInfoMsg);
DEFINE_OBJ(ProcToGuiPredictStatusMsg);
DEFINE_OBJ(ProductSelectionMsg);
DEFINE_OBJ(QueryGUIAliveMsg);
DEFINE_OBJ(QuerySafetyAliveMsg);
DEFINE_OBJ(RasConnectedPasToBeConnected);
DEFINE_OBJ(RBCCalibrationRequestMsg);
DEFINE_OBJ(RBCChamberPurgeMsg);
DEFINE_OBJ(RBCDetectorUncalibratedMsg);
DEFINE_OBJ(RBCRestoreCalibrationMsg);
DEFINE_OBJ(RBCSetupNeededMsg);
DEFINE_OBJ(RbcSpilloverRequestMsg);
DEFINE_OBJ(RemoveAllChildScreensMsg);
DEFINE_OBJ(RemoveAllStatelessScreensMsg);
DEFINE_OBJ(RemoveMostRecentChildScreenMsg);
DEFINE_OBJ(RemoveMostRecentStatelessScreenMsg);
DEFINE_OBJ(RepFluidConnectStatusMsg);
DEFINE_OBJ(RequestCancelMssPfr);
DEFINE_OBJ(RequestEndRunStatsMsg);
DEFINE_OBJ(RequestGenericWbcAlarm);
DEFINE_OBJ(RequestHardwareVersionMsg);
DEFINE_OBJ(RequestScreenCal);
DEFINE_OBJ(RequestSafetyDriverStatusMsg);
DEFINE_OBJ(RequestSafetyPowerMsg);
DEFINE_OBJ(RetOccMonitorStatusMsg);
DEFINE_OBJ(RinsebackCompleteMsg);
DEFINE_OBJ(RinsebackRequestMsg);
DEFINE_OBJ(RinsebackStatusMsg);
DEFINE_OBJ(RunCompleteMsg);
DEFINE_OBJ(SafetyPFRFileStatusMsg);
DEFINE_OBJ(SafetyServiceModeMsg);
DEFINE_OBJ(SafetyTestDoneMsg);
DEFINE_OBJ(SafetyDisconnectStatMsg);
DEFINE_OBJ(SalineBolusMsg);
DEFINE_OBJ(SalineDrawRecoveryMsg);
DEFINE_OBJ(SendOrdersMsg);
DEFINE_OBJ(SetCtlStatusSentTimestamp);
DEFINE_OBJ(SetListboxItemChosen);
DEFINE_OBJ(SetStatusLineMsg);
DEFINE_OBJ(SettleChannelMsg);
DEFINE_OBJ(ShowClampFromARMMsg);
DEFINE_OBJ(ShowTimeoutPredictScreen);
DEFINE_OBJ(SnapScreenshotMsg);
DEFINE_OBJ(SoundMsg);
DEFINE_OBJ(SpilloverDetectedMsg);
DEFINE_OBJ(SpilloverRecoveryMsg);
DEFINE_OBJ(StackMonitorTaskEndMsg);
DEFINE_OBJ(StackMonitorTaskStartMsg);
DEFINE_OBJ(StatLineRequestMsg);
DEFINE_OBJ(StateChangeCompleteMsg);
DEFINE_OBJ(StopGoodDogMsg);
DEFINE_OBJ(STSConnectedMsg);
DEFINE_OBJ(SwitchToPreviousScreenMsg);
DEFINE_OBJ(SystemStateChangeMsg);
DEFINE_OBJ(SystemStateChangeRequestMsg);
DEFINE_OBJ(TaskManAliveMsg);
DEFINE_OBJ(TellGuiThatDisconnectPassed);
DEFINE_OBJ(TellProcToTransitionFromDisconnect);
DEFINE_OBJ(TestCompletedMsg);
DEFINE_OBJ(TimeJumpControlMsg);
DEFINE_OBJ(TransitionValidationMsg);
DEFINE_OBJ(TriggerSetRunTypeMsg);
DEFINE_OBJ(UnsuccessfulCassetteLoweringMsg);
DEFINE_OBJ(UpdateAutoFlowTimerMsg);
DEFINE_OBJ(VerifyAcClosureCompleteMsg);
DEFINE_OBJ(VistaDonorCommittedMsg);
DEFINE_OBJ(VistaDonorRejectedMsg);
DEFINE_OBJ(VistaNumberOfNewPinsMsg);
DEFINE_OBJ(VistaWritePinFileMsg);
DEFINE_OBJ(VistaConnectionStatusMsg);

#if (CPU==SIMNT) // This include file has defs for sim-only messages.
#include "../unit_test/gui/h/simIntMessages.hpp"
#endif // #if (CPU==SIMNT)

#endif // #ifdef _TRIMA_MESSAGES_CPP_

/* FORMAT HASH c407497168a2fbcbcc68426d4030c1b4 */

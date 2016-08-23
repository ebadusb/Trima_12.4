using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Windows;

using BCT.Common;
using BCT.Common.Utilities;
using BCT.Device.Trima.V6_3;

namespace TrimaTestTool.ViewModel
{
    public class Trima63ViewModel : TrimaViewModelBase
    {
        private const string floatNumberFormat = "0.0";
        private const string float2NumberFormat = "0.00";
        /// <summary>
        /// This is the version of trima
        /// </summary>
        public string Version { get; set; }
        public string IP { get; set; }
        public string SerialNumber { get; set; }

        public string SafetyRBCValve { get; set; }
        public string SafetyPlasmaValve { get; set; }
        public string SafetyPlateletValve { get; set; }

        public string ControlRBCValve { get; set; }
        public string ControlPlasmaValve { get; set; }
        public string ControlPlateletValve { get; set; }

        public string mmHgAccessPressure { get; set; }
        public string mmHgCentrifugePressure { get; set; }
        public string RawAccessPressure { get; set; }
        public string RawCentrifugePressure { get; set; }

        public string ControlReservoirLevel { get; set; }
        public string SafetyReservoirLevel { get; set; }

        public string AGCHigh { get; set; }
        public string AGCLow { get; set; }

        public string RedDetector { get; set; }
        public string GreenDetector { get; set; }
        public string RedGreenRatio { get; set; }

        /// <summary>
        /// VIF
        /// </summary>
        
        //States
        public string SystemState { get; set; }
        public string Substate { get; set; }
        public string RecoveryState { get; set; }


        //referenced
        public string CassetteType { get; set; }
        public string CassettePosition { get; set; }
        public string CatalogRefNumber { get; set; }
        public string SelectedProcedure { get; set; }
        public string SequenceNumber { get; set; }
        public string MessageTime { get; set; }

        //collect values
        public string CurrentPlateletYield { get; set; }
        public string TargetPlateletYield { get; set; }
        public string CurrentPlateletVolume { get; set; }
        public string TargetPlateletVolume { get; set; }
        public string CurrentPlasmaVolume { get; set; }
        public string TargetPlasmaVolume { get; set; }
        public string CurrentRBCVolume { get; set; }
        public string TargetRBCVolume { get; set; }
        public string CurrentRBCHCT { get; set; }
        public string TargetRBCHCT { get; set; }
        public string CurrentProcedureTime { get; set; }
        public string TargetProcedureTime { get; set; }
        public string CurrentPASVolme { get; set; }
        public string TargetPASVolume { get; set; }
        public string CurrentRAS1Volume { get; set; }
        public string TargetRAS1Volume { get; set; }
        public string CurrentRAS2Volume { get; set; }
        public string TargetRAS2Volume { get; set; }

        public string PlateletAC { get; set; }
        public string PlasmaAC { get; set; }
        public string RBC1AC { get; set; }
        public string RBC2AC { get; set; }
        public string TotalBloodProcessed { get; set; }
        public string TimeRemaining { get; set; }
        public string DonorTBV { get; set; }

        //pumps
        public string CommandedACPump { get; set; }
        public string CommandedPlasmaPump { get; set; }
        public string CommandedPlateletPump { get; set; }
        public string CommandedReturnPump { get; set; }
        public string CommandedReplacementPump { get; set; }
        public string CommandedInletPump { get; set; }

        public string ActualACPump { get; set; }
        public string ActualPlasmaPump { get; set; }
        public string ActualPlateletPump { get; set; }
        public string ActualReturnPump { get; set; }
        public string ActualReplacementPump { get; set; }
        public string ActualInletPump { get; set; }

        public string AccumulatedACPump { get; set; }
        public string AccumulatedPlasmaPump { get; set; }
        public string AccumulatedPlateletPump { get; set; }
        public string AccumulatedReturnPump { get; set; }
        public string AccumulatedReplacementPump { get; set; }
        public string AccumulatedInletPump { get; set; }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="address"></param>
        public Trima63ViewModel(IPAddress address)
            : base(address)
        {
            Trima trima = new Trima(address);
            trima.ControlHwUpdate = new TedControlStatusUpdate(ControlHwUpdate);
            trima.SafetyHwUpdate = new TedSafetyStatusUpdate(SafetyHwUpdate);
            trima.VifPeriodicStatusUpdate = new VifPeriodicStatusUpdate(VifPeriodicUpdate);
            trima.VifMachineStatusUpdate = new VifMachineStatusUpdate(VifMachineStatusUpdateCallback);
            trima.BroadcastUpdate = new BroadcastUpdate(BroadcastUpdateCallback);
            Trima = trima;

            IP = address.ToString();
            SerialNumber = SerialNumberConverter.GetSerialNumber(IP);
        }

        public void StoppingConnection(object sender, RoutedEventArgs e)
        {
            Trima.CloseConnection();
        }

        private void BroadcastUpdateCallback(TEDIf.TED_BroadcastMsg msg)
        {
            this.Version = NullTerminatedString.GetASCIIString(msg.trimaRelease);
            RaisePropertyChanged(this.GetPropertyName(()=>this.Version));

            this.SerialNumber = NullTerminatedString.GetASCIIString(msg.trimaSerialNumber);
            RaisePropertyChanged(this.GetPropertyName(() => this.SerialNumber));
        }

        private void ControlHwUpdate(TEDIf.TED_ControlHwStatusMsg msg)
        {
            //valves
            ControlRBCValve = TranslateValveState(msg.rbcValve);
            RaisePropertyChanged(this.GetPropertyName(() => this.ControlRBCValve));

            ControlPlasmaValve = TranslateValveState(msg.plasmaValve);
            RaisePropertyChanged(this.GetPropertyName(() => this.ControlPlasmaValve));

            ControlPlateletValve = TranslateValveState(msg.plateletValve);
            RaisePropertyChanged(this.GetPropertyName(() => this.ControlPlateletValve));
            var pressure = CPSConverter.To_mmHg((uint)msg.centrifugePressure);
            if(pressure.HasValue )
                mmHgCentrifugePressure = string.Format("{0:f3}", pressure.Value);
            RaisePropertyChanged(this.GetPropertyName(() => this.mmHgCentrifugePressure));

            pressure = APSConverter.To_mmHg((uint)msg.accessPressure);
            if(pressure.HasValue)
                mmHgAccessPressure = string.Format("{0:f3}", pressure.Value );
            RaisePropertyChanged(this.GetPropertyName(() => this.mmHgAccessPressure));

            RawCentrifugePressure = msg.centrifugePressure.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.RawCentrifugePressure));

            RawAccessPressure = msg.accessPressure.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.RawAccessPressure));

            ControlReservoirLevel = this.TranslateReservoirLevel(msg.reservoir);
            RaisePropertyChanged(this.GetPropertyName(() => this.ControlReservoirLevel));

            AGCLow = (msg.lowAGC * 0.001).ToString(float2NumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.AGCLow));

            AGCHigh = (msg.highAGC * 0.001).ToString(float2NumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.AGCHigh));

            RedDetector = msg.red.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.RedDetector));

            GreenDetector = msg.green.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.GreenDetector));

            if (msg.green != 0)
            {
                double ratio = (double)msg.red / (double)msg.green;
                RedGreenRatio = string.Format("{0:f3}", ratio);
            }
            else
                RedGreenRatio = string.Empty;
            RaisePropertyChanged(this.GetPropertyName(() => this.RedGreenRatio));
        }

        private void SafetyHwUpdate(TEDIf.TED_SafetyHwStatusMsg msg)
        {
            SafetyRBCValve = TranslateValveState(msg.rbcValve);
            RaisePropertyChanged(this.GetPropertyName(() => this.SafetyRBCValve));

            SafetyPlasmaValve = TranslateValveState(msg.plasmaValve);
            RaisePropertyChanged(this.GetPropertyName(()=>this.SafetyPlasmaValve));

            SafetyPlateletValve = TranslateValveState(msg.plateletValve);
            RaisePropertyChanged(this.GetPropertyName(() => this.SafetyPlateletValve));

            SafetyReservoirLevel = this.TranslateReservoirLevel(msg.reservoir);
            RaisePropertyChanged(this.GetPropertyName(() => this.SafetyReservoirLevel));
        }

        private void VifPeriodicUpdate(VISIf.trimaPeriodicStatusMsg msg)
        {
            //States
            SystemState = TranslateSystemState(msg.trimaSystemState);
            RaisePropertyChanged(this.GetPropertyName(() => this.SystemState));

            string test = Encoding.ASCII.GetString(msg.subState);

            Substate = NullTerminatedString.GetASCIIString(msg.subState);
            RaisePropertyChanged(this.GetPropertyName(() => this.Substate));

            RecoveryState = NullTerminatedString.GetASCIIString(msg.recoveryState);
            RaisePropertyChanged(this.GetPropertyName(() => this.RecoveryState));

            //CollectValues
            CurrentPlateletYield = (msg.currentPlateletYield / 10.0f).ToString(float2NumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.CurrentPlateletYield));

            TargetPlateletYield = (msg.targetPlateletYield / 10.0f).ToString(float2NumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.TargetPlateletYield));


            CurrentPlateletVolume = msg.currentPlateletVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.CurrentPlateletVolume));

            TargetPlateletVolume = msg.targetPlateletVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.TargetPlateletVolume));

            CurrentPlasmaVolume = msg.currentPlasmaVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.CurrentPlasmaVolume));

            TargetPlasmaVolume = msg.targetPlasmaVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.TargetPlasmaVolume));

            CurrentRBCVolume = msg.currentRbcVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.CurrentRBCVolume));

            TargetRBCVolume = msg.targetRbcVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.TargetRBCVolume));

            CurrentRBCHCT = msg.currentRbcCollectHct.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.CurrentRBCHCT));

            TargetRBCHCT = msg.targetRbcCollectHct.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.TargetRBCHCT));

            CurrentProcedureTime = msg.currentProcedureTime.ToString(float2NumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.CurrentProcedureTime));

            TargetProcedureTime = msg.targetProcedureTime.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.TargetProcedureTime));

            CurrentPASVolme = msg.currentPASVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.CurrentPASVolme));

            TargetPASVolume = msg.targetPASVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.TargetPASVolume));

            CurrentRAS1Volume = msg.currentRAS1Volume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.CurrentRAS1Volume));

            TargetRAS1Volume = msg.targetRAS1Volume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.TargetRAS1Volume));

            CurrentRAS2Volume = msg.currentRAS2Volume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.CurrentRAS2Volume));

            TargetRAS2Volume = msg.targetRAS2Volume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.TargetRAS2Volume));

            PlateletAC = msg.currentPlateletAcVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.PlateletAC));

            PlasmaAC = msg.currentPlasmaAcVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.PlasmaAC));

            RBC1AC = msg.currentRbc1AcVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.RBC1AC));
            
            RBC2AC = msg.currentRbc2AcVolume.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.RBC2AC));

            TotalBloodProcessed = msg.totalBloodProcessed.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.TotalBloodProcessed));

            TimeRemaining = msg.remainingProcedureTime.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.TimeRemaining));

            DonorTBV = msg.donorTbv.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.DonorTBV));

            //Reference 
            CassetteType = msg.cassetteType.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.CassetteType));

            CassettePosition = msg.cassettePosition.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.CassettePosition));

            CatalogRefNumber = msg.catalogNumber.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.CatalogRefNumber));

            SelectedProcedure = msg.selectedProcedure.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.SelectedProcedure));

            SequenceNumber = msg.hdr.sequenceNumber.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.SequenceNumber));

            MessageTime = EpochTime.GetDateTime(msg.hdr.timestamp).ToLongTimeString();
            RaisePropertyChanged(this.GetPropertyName(() => this.MessageTime));

            UpdateHeader(msg.hdr);

            if (Substate != null && Substate.Contains("LowerCassette"))
            {
                Trima.GetAPSAutoZero();
            }
        }

        private void UpdateHeader(VISIf.trimaVistaHeader header)
        {
            SequenceNumber = header.sequenceNumber.ToString();
            RaisePropertyChanged(this.GetPropertyName(() => this.SequenceNumber));

            MessageTime = EpochTime.GetDateTime(header.timestamp).ToLongTimeString();
            RaisePropertyChanged(this.GetPropertyName(() => this.MessageTime));
        }

        private void VifMachineStatusUpdateCallback(VISIf.trimaMachineStatusMsg msg)
        {
            CommandedACPump = msg.acPump.commandedRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.CommandedACPump));

            CommandedPlasmaPump = msg.plasmaPump.commandedRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.CommandedPlasmaPump));

            CommandedPlateletPump = msg.plateletPump.commandedRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.CommandedPlateletPump));

            CommandedReturnPump = msg.returnPump.commandedRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.CommandedReturnPump));

            CommandedReplacementPump = msg.replacementPump.commandedRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.CommandedReplacementPump));

            CommandedInletPump = msg.inletPump.commandedRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.CommandedInletPump));

            ActualACPump = msg.acPump.actualRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.ActualACPump));

            ActualPlasmaPump = msg.plasmaPump.actualRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.ActualPlasmaPump));

            ActualPlateletPump = msg.plateletPump.actualRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.ActualPlateletPump));

            ActualReturnPump = msg.returnPump.actualRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.ActualReturnPump));

            ActualReplacementPump = msg.replacementPump.actualRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.ActualReplacementPump));

            ActualInletPump = msg.inletPump.actualRate.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.ActualInletPump));

            AccumulatedACPump = msg.acPump.accumulatedVolume.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.AccumulatedACPump));

            AccumulatedPlasmaPump = msg.plasmaPump.accumulatedVolume.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.AccumulatedPlasmaPump));

            AccumulatedPlateletPump = msg.plateletPump.accumulatedVolume.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.AccumulatedPlateletPump));

            AccumulatedReturnPump = msg.returnPump.accumulatedVolume.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.AccumulatedReturnPump));

            AccumulatedReplacementPump = msg.replacementPump.accumulatedVolume.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.AccumulatedReplacementPump));

            AccumulatedInletPump = msg.inletPump.accumulatedVolume.ToString(floatNumberFormat);
            RaisePropertyChanged(this.GetPropertyName(() => this.AccumulatedInletPump));

            UpdateHeader(msg.hdr);
        }

        private string TranslateSystemState(int state)
        {
            VISIf.State_names enumState = (VISIf.State_names)state;
            return string.Format( "({0}) - {1}", state, enumState.ToString());
        }

        private string TranslateValveState(TEDIf.HW_VALVE_STATES state)
        {
            string stateStr = string.Empty;
            switch (state)
            {
                case TEDIf.HW_VALVE_STATES.HW_VALVE_COLLECT:
                    stateStr = "Collect";
                    break;
                case TEDIf.HW_VALVE_STATES.HW_VALVE_OPEN:
                    stateStr = "Open";
                    break;
                case TEDIf.HW_VALVE_STATES.HW_VALVE_RETURN:
                    stateStr = "Return";
                    break;
                case TEDIf.HW_VALVE_STATES.HW_VALVE_UNKNOWN:
                default:
                    stateStr = "Unknown";
                    break;
            }
            return stateStr;
        }

        private string TranslateReservoirLevel(TEDIf.CHW_RESERVOIR_STATES state)
        {
            string level = string.Empty;
            switch (state)
            {
                case TEDIf.CHW_RESERVOIR_STATES.CHW_RESERVOIR_EMPTY:
                    level = "Empty";
                    break;
                case TEDIf.CHW_RESERVOIR_STATES.CHW_RESERVOIR_ERROR:
                    level = "Error";
                    break;
                case TEDIf.CHW_RESERVOIR_STATES.CHW_RESERVOIR_HIGH:
                    level = "High";
                    break;
                case TEDIf.CHW_RESERVOIR_STATES.CHW_RESERVOIR_MIDDLE:
                    level = "Middle";
                    break;
            }
            return level;
        }
        private string TranslateReservoirLevel(TEDIf.SHW_RESERVOIR_STATES state)
        {
            string level = string.Empty;
            switch (state)
            {
                case TEDIf.SHW_RESERVOIR_STATES.SHW_RESERVOIR_EMPTY:
                    level = "Empty";
                    break;
                case TEDIf.SHW_RESERVOIR_STATES.SHW_RESERVOIR_LOW:
                    level = "Low";
                    break;
            }
            return level;
        }
    
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BCT.Device.Trima.V6_3
{
    public partial class TEDIf
    {
        public const string Version = "4.9";

        public enum MessageId
        {
            TED_FIRST_MSG = 11000,
            TED_CONNECT_REQUEST = 11001,
            TED_HW_PERIODIC_STATUS_REQUEST = 11002,
            TED_CONTROL_HARDWARE_CMD = 11003,
            TED_SAFETY_HARDWARE_CMD = 11004,
            TED_END_SERVICE_MODE = 11005,
            TED_MOVE_CURSOR_CMD = 11006,
            TED_SET_TIME = 11007,
            TED_END_CONNECTION = 11008,
            TED_CONTROL_HARDWARE_STATUS = 11009,
            TED_SAFETY_HARDWARE_STATUS = 11010,
            TED_REPLY = 11011,
            TED_REQUEST_CURSOR_POSITION = 11012,
            TED_CURRENT_CURSOR_POSITION = 11013,
            TED_BROADCAST_REQUEST = 11014,
            TED_BROADCAST = 11015,
            TED_REQUEST_APS_AUTOZERO_VALUE = 11016,
            TED_APS_AUTOZERO_VALUE = 11017,
            // IDs 11018-11019 reserved
            TED_SCREEN_CALIBRATION_MESSAGE = 11020,
            // IDs 11021-11027 reserved
            TED_CONTROL_OVP_TEST_REQUEST = 11028,
            TED_LOGGING_MESSAGE = 11029,
            TED_LAST_MSG = TED_LOGGING_MESSAGE
        }

        public enum TED_Status
        {
            TED_CONNECTION_ALLOWED_OPERATIONAL = 0,
            TED_CONNECTION_ALLOWED_SERVICE = 1,
            TED_CONNECTION_ALLOWED_SINGLE_STEP = 2,
            TED_CONNECTION_DENIED = 3,
            TED_REQUEST_ALLOWED = 4,
            TED_REQUEST_ALLOWED_OVERRIDE = 5,
            TED_REQUEST_DENIED_NOT_IN_SERVICE_MODE = 6,
            TED_REQUEST_DENIED_NOT_CLIENT = 7,
            TED_BAD_MESSAGE_RECEIVED = 8,
            TED_OPERATION_FAILED = 9,
            TED_SAFETY_NOT_STARTED = 10
        }

        public class TED_Header
        {
            public uint som;
            public uint msgId;
            public uint length;
            public int time;
            public uint bodyCRC;
            public uint headerCRC;
        }

        public class TED_ReplyMsg
        {
            public TED_Header hdr;
            public TED_Status status;
        }

        public class TED_ConnectRequestMsg
        {
            public TED_Header hdr;
        };

        public enum TED_BoardType
        {
            CONTROL = 0,
            SAFETY = 1
        }

        public class TED_HwPeriodicStatusRequestMsg
        {
           public TED_Header    hdr;
           public TED_BoardType board	;           // Status being requested from TED.
           public uint  period	;                   // Period of requested status in ms.
           public uint  port	;                   // Port Response message goes to.
        }

        public class CHWPumpStatus
        {
            //#pragma pack(push,1)
            public byte   errors;        // CHW_PUMP_NO_FAULT,
                                                    // CHW_PUMP_FAIL_TO_MAINTAIN_SPEED = 1,
                                                    // CHW_PUMP_INTEGRAL_ERROR = 2,                                             // CHW_PUMP_VOLUME_ERROR = 4
           // pump actual rpm
           public float RPM			; // ~-250 to ~250, for return pump                                             // 0 to 250 for all other pumps
           // pump revolutions
           public float Revs			; // Delta revs since last update
           // pump accumulated revs, encoder ticks
           public int  Accum			; // total revs, encoder ticks
           // error terms
           public int  Integral		; // snap-shot of the integration error
           public int  AcummIntegral	; // cummulative integration error for the                                             // course of the entire run
            //#pragma pack(pop)
        }

        public class TED_ControlHwStatusMsg
        {
           public TED_Header          hdr;
            //#pragma pack(push,1)
        // A/D channels, raw sensor readings, uncalibrated, 0-4095 counts
           public short accessPressure			;	// access (inlet) pressure
           public short emiTemperature			;	// temperature of EMI box
           public short centrifugeCurrent		;	// centrifuge current
           public short centrifugePressure		;	// centrifuge pressure sensor
           public short fiveVoltSupply			;	// + 5 volt supply
           public short leakDetector			;	// leak detector
           public short minusTwelvePS			;	// -12 volt supply
           public short sixtyFourVoltCurrent	;	// +64 volt (centrifuge) current
           public short sixtyFourVoltSupply	;	// +64 volt (centrifuge) before switch
           public short sixtyFourVoltSwitched	;	// +64 volt (centrifuge) after switch
           public short twelveVoltSupply		;	// +12 volt supply
           public short twentyFourVoltCurrent	;	// +24 volt supply current
           public short twentyFourVoltSupply	;	// +24 volt before switch
           public short twentyFourVoltSwitched	;	// +24 volt after switch

            // cassette position
            public HW_CASSETTE_STATES  cassettePosition	;	// HW_CASSETTE_UNKNOWN,
																	        // HW_CASSETTE_DOWN,
																	        // HW_CASSETTE_UP
   
            public HW_ERROR            cassetteError	;	// HW_NO_FAULT, HW_FAULT

	        // centrifuge
            public float               centrifugeRPM	;	// 0 to ~3000 rpm
            public byte                 centrifugeError;           					// HW_CENT_NO_FAULT,
																	        // HW_CENT_HARDWARE_ERROR=1,
																	        // HW_CENT_INTEGRAL_ERROR=2,
	        // Misc
            public HW_DOOR_STATES      doorLocks		;	// HW_DOOR_OPEN,
																	        // HW_DOOR_LOCKED_STATE,
																	        // HW_DOOR_CLOSED_STATE,
																	        // HW_DOOR_CLOSED_AND_LOCKED

           public HW_SWITCH_STATES    pauseSwitch		;	// HW_SWITCH_DISABLED,
																	        // HW_SWITCH_ENABLED
           public HW_SWITCH_STATES    stopSwitch		;             
           public HW_SWITCH_STATES    pauseSwitchLatch	;       
           public HW_SWITCH_STATES    stopSwitchLatch	;        
           public HW_ERROR            vibration		;	// HW_NO_FAULT, HW_FAULT
           public HW_ERROR            valveFault		;	// HW_NO_FAULT, HW_FAULT
           public HW_ERROR            motorFault		;	// HW_NO_FAULT, HW_FAULT
           public HW_ERROR            solenoidFault	;	// HW_NO_FAULT, HW_FAULT

           // rbc detector
           public short               red				;	// raw red value, 0-~32000 counts
           public short               green			;	// raw green value, 0-~32000 counts
           public HW_ERROR            rbcCommError		;	// HW_NO_FAULT, HW_FAULT

           // pumps
           public CHWPumpStatus       acPump			;	// AC
           public CHWPumpStatus       inletPump		;	// Inlet
           public CHWPumpStatus       plasmaPump		;	// Plasma
           public CHWPumpStatus       plateletPump		;	// Platelet
           public CHWPumpStatus       returnPump		;	// Return

        // ultrasonics
           public CHW_AC_DETECTOR_STATES acDetector	;	// CHW_AC_DETECTOR_FLUID = 1,
																	        // CHW_AC_DETECTOR_AIR = 2

           public CHW_RESERVOIR_STATES   reservoir		;	// CHW_RESERVOIR_ERROR,
																	        // CHW_RESERVOIR_EMPTY,
																	        // CHW_RESERVOIR_MIDDLE,
																	        // CHW_RESERVOIR_HIGH
                                             

           public HW_VALVE_STATES        plasmaValve	;	// HW_VALVE_UNKNOWN,
																	        // HW_VALVE_COLLECT,
																	        // HW_VALVE_OPEN,
																	        // HW_VALVE_RETURN
           public HW_VALVE_STATES        plateletValve	;           
           public HW_VALVE_STATES        rbcValve		;                
                                             
           public HW_ERROR               fanSense0		;
           public HW_ERROR               fanSense1		;                 
           public HW_ERROR               fanSense2		;	// Not used in any HW
           public HW_ERROR               tempError		;

           public CHW_PS_OT_ERRORS       psTempWarning ;

           public CHW_SOUND_LEVELS       soundLevel	;

        // test modes
           public CHW_EVENTS             chwevent			;	// CHW_NORMAL_UPDATE,
																	        // CHW_AC_EVENT,
																	        // CHW_DOOR_EVENT,
																	        // CHW_PAUSE_EVENT,
																	        // CHW_STOP_EVENT,
																	        // CHW_RESERVOIR_EVENT

           public int                   msgCount		;	// accum message counter
           public short                  lowAGC		;	// low AGC voltage
           public short                  highAGC		;	// high AGC voltage

           public CHW_SEAL_SAFE_OPERATION       sealSafeOperating	;	// Only on Artysan power supplies
           public CHW_SEAL_SAFE_OVER_TEMP_WARN  sealSafeOvertemp	;	// Only on Artysan power supplies
           public CHW_PS_OVP_TEST_RESULT        ovpTestResult		;	// Only on Artysan power supplies
           public CHW_PS_TEST_STATUS			 ovpTestStatus		;

           public short				mxFpgaInstalled				;	// 0 or 1 depending on whether or not
																				        // the MX FPGA is installed
           public byte		fpgaRevision;			// firmware revision for control FPGA
           public byte      fpgaIdRevision;			// hw revision for control FPGA
           public byte      fpgaInterfaceRevision;	// sw interface revision for control FPGA
           public byte      ccaRevision;			// CCA revision from control FPGA
           public HW_WATCHDOG_STATUS watchDogStatus; // HW Watchdog Status
           public HW_PS_OUTPUT_STATUS powerSupplyOutputStatus; // PS Output Status
        //#pragma pack(pop)
        };

        public class TED_SafetyHwStatusMsg
        {
           public TED_Header          hdr;
            //#pragma pack(push, 1)
            // pumps
           public int                inletRPM				;	// 0 to ~250 rpm
           public int                inletRevs			;	// delta counts
           public int                inletAccumRevs		;	// total counts
                                           
           public int                plateletRPM			;	// 0 to ~250 rpm
           public int                plateletRevs			;	// delta counts
           public int                plateletAccumRevs	;	// total counts
                                           
           public int                plasmaRPM			;	// 0 to ~250 rpm
           public int                plasmaRevs			;	// delta counts
           public int                plasmaAccumRevs		;	// total counts
                                           
           public int                acRPM				;	// 0 to ~250 rpm
           public int                acRevs				;	// delta counts
           public int                acAccumRevs			;	// total counts
                                           
           public int                returnRPM			;	// ~-250 to ~250 rpm
           public int                returnRevs			;	// delta counts
           public int                returnAccumRevs		;	// total counts
           public SHW_DIRECTION       returnDirection		;	// SHW_RETURN, SHW_DRAW

	        // valve status    0 = HW_VALVE_UNKNOWN
	        //                 1 = HW_VALVE_COLLECT
	        //                 2 = HW_VALVE_OPEN
	        //                 3 = HW_VALVE_RETURN

           public HW_VALVE_STATES     rbcValve				;
           public HW_VALVE_STATES     plasmaValve			;
           public HW_VALVE_STATES     plateletValve		;

           public byte       valveLedTest;        // 1=ok, all others =error

           // cassette position
           public HW_CASSETTE_STATES  cassettePosition		;	// HW_CASSETTE_UNKNOWN, HW_CASSETTE_DOWN,
																		        // HW_CASSETTE_UP

           // ultrasonics
           public SHW_RESERVOIR_STATES reservoir			;	// 1=SHW_RESERVOIR_EMPTY, SHW_RESERVOIR_LOW

	        // centrifuge
           public float               centrifugeRPM		;	// RPM
           public byte       centrifugeError;									// HW_CENT_NO_FAULT, HW_CENT_HARDWARE_ERROR=1,
																		        // HW_CENT_INTEGRAL_ERROR=2

	        // misc
           public HW_DOOR_STATES      doorLocks			;	// HW_DOOR_OPEN, HW_DOOR_LOCKED_STATE,
																		        // HW_DOOR_CLOSED,
																		        // HW_DOOR_CLOSED_AND_LOCKED

           public bool                pauseSwitch;										// HW_SWITCH_DISABLED, HW_SWITCH_ENABLED
           public bool                stopSwitch;										// HW_SWITCH_DISABLED, HW_SWITCH_ENABLED

           public SHW_EVENTS          hwEvent				;	// SHW_NORMAL_UPDATE,
																		        // SHW_PAUSE_EVENT, SHW_STOP_EVENT,
																		        // SHW_DOOR_EVENT,
																		        // SHW_RESERVOIR_EVENT,
																		        // SHW_RETURN_EVENT, SHW_AIR_EVENT


           public int                msgCount				;	// accum message counter

           public byte      returnPumpDirChgTime	;	// time from low level sensor
																		        // indicating air to return
																		        // pump in draw direction,
																		        // milliseconds

           // test modes
           public int                test1				;
           public short               test2				;

           public HW_ORDERS           serviceEnableMode	;	// HW_DISABLE=!servicemode,
																		        // HW_ENABLE=servicemode
           public HW_ORDERS           donorConnectMode		;	// HW_DISABLE=!donorconnect,
																		        // HW_ENABLE=donorconnect
            //#pragma pack(pop)
        }

        public const int TED_HW_REV_SIZE          = 64;
        public const int TED_IP_ADDRESS_SIZE = 32;
        public const int TED_LOG_FILENAME_SIZE = 32;
        public const int TED_SERIAL_NUMBER_SIZE = 16;
        public const int TED_CRC_SIZE = 16;
        public const int TED_RELEASE_SIZE = 16;
        public const int TED_BUILD_SIZE = 16;
        public const int TED_PORT_NUMBER_SIZE = 8;
        public const int TED_SEQUENCE_NUMBER_SIZE = 7;
        public const int TED_VIPIF_VERSION_SIZE = 32;
        public const int TED_LOG_STRING_SIZE = 256;
        public const int TED_INTERFACE_REVISION_SIZE = 16;

        public class TED_BroadcastMsg
        {
           public TED_Header   hdr;
           public int broadcastCount;                             // count of packets broadcasted
           public byte[] trimaHWRev = new byte[TED_HW_REV_SIZE];                // Control & Safety Hdw Revisions
           public byte[] trimaIP = new byte[TED_IP_ADDRESS_SIZE];               // 172.80.90.34
           public byte[] trimaLogFile = new byte[TED_LOG_FILENAME_SIZE];        // T00015_0504_0033
           public byte[] trimaSerialNumber = new byte[TED_SERIAL_NUMBER_SIZE];  // T00015
           public byte[] trimaCRC = new byte[TED_CRC_SIZE];                     // 0xadcef73
           public byte[] trimaRelease = new byte[TED_RELEASE_SIZE];             // Trima 3.3a
           public byte[] trimaBuild = new byte[TED_BUILD_SIZE];                 // 1.191
           public byte[] trimaPort = new byte[TED_PORT_NUMBER_SIZE];            // 7001
           public byte[] sequenceNumber = new byte[TED_SEQUENCE_NUMBER_SIZE];   // 000000
           public byte[] vipifVersionNumber = new byte[TED_VIPIF_VERSION_SIZE]; // $Revision: 1.24 $
           public uint biometricDeviceList;                        // Bit field containing Biometric ID's.
           public byte[] tedifVersionNumber = new byte[TED_INTERFACE_REVISION_SIZE]; // TED_InterfaceRevision  
        };


        public class TED_ApsAutoZeroValueMsg
        {
           public TED_Header hdr;
           public float      rawValue;	        // Raw value from cal.dat
           public float      referenceValue;	// Auto zero value computed by proc
        };
    }
}

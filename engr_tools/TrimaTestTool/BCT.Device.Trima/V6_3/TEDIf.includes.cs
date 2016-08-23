using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BCT.Device.Trima.V6_3
{
    public partial class TEDIf
    {
        //-----------------------------------
        //hw.h
        public enum HW_CASSETTE_STATES
        {
            HW_CASSETTE_UNKNOWN = 0,
            HW_CASSETTE_DOWN = 1,
            HW_CASSETTE_UP = 2
        };

        public enum HW_VALVE_STATES
        {
            HW_VALVE_UNKNOWN = 0,
            HW_VALVE_COLLECT = 1,
            HW_VALVE_OPEN = 2,
            HW_VALVE_RETURN = 3
        };

        public enum HW_DOOR_STATES
        {
            HW_DOOR_OPEN = 0,
            HW_DOOR_LOCKED_STATE = 1,
            HW_DOOR_CLOSED_STATE = 2,
            HW_DOOR_CLOSED_AND_LOCKED = 3
        };

        public enum HW_ORDERS
        {
            HW_DISABLE = 0,
            HW_ENABLE = 1
        };

        public enum HW_SWITCH_STATES
        {
            HW_SWITCH_DISABLED = 0,
            HW_SWITCH_ENABLED = 1
        };

        public enum HW_ERROR
        {
            HW_NO_FAULT = 0,
            HW_FAULT = 1
        };

        // bit masks for centrifuge errors
        public enum HW_CENT_ERRORS
        {
            HW_CENT_NO_FAULT = HW_ERROR.HW_NO_FAULT,
            HW_CENT_HARDWARE_ERROR = 1,         // could be any: reverse, over current,
            // under voltage or FPGA-sensed commutation 
            HW_CENT_INTEGRAL_ERROR = 2          // means integration component of the
            // control loop is not sufficient to make
            // desired speed
        };
        //hw.h----------------------------------------------

        //chw.hpp
        public enum CHW_AC_DETECTOR_STATES
        {
            CHW_AC_DETECTOR_FLUID = 1,
            CHW_AC_DETECTOR_AIR = 2
        };

        public enum CHW_CASSETTE_ORDERS
        {
            CHW_CASSETTE_FIRST_ORDER = 0,
            CHW_CASSETTE_DONTMOVE = CHW_CASSETTE_FIRST_ORDER,
            CHW_CASSETTE_DOWN = HW_CASSETTE_STATES.HW_CASSETTE_DOWN,
            CHW_CASSETTE_UP = HW_CASSETTE_STATES.HW_CASSETTE_UP,
            CHW_CASSETTE_LAST_ORDER = CHW_CASSETTE_UP
        };

        public enum CHW_DOOR_ORDERS
        {
            CHW_DOOR_UNLOCK = 0,
            CHW_DOOR_LOCK = 1
        };

        public enum CHW_VALVE_ORDERS
        {
            CHW_VALVE_FIRST = HW_VALVE_STATES.HW_VALVE_COLLECT,
            CHW_VALVE_COLLECT = CHW_VALVE_FIRST,
            CHW_VALVE_OPEN = HW_VALVE_STATES.HW_VALVE_OPEN,
            CHW_VALVE_RETURN = HW_VALVE_STATES.HW_VALVE_RETURN,
            CHW_VALVE_LAST = CHW_VALVE_RETURN
        };

        public enum CHW_EVENTS
        {
            CHW_NORMAL_UPDATE = 0,
            CHW_AC_EVENT = 1,
            CHW_DOOR_EVENT = 2,
            CHW_PAUSE_EVENT = 3,
            CHW_STOP_EVENT = 4,
            CHW_RESERVOIR_EVENT = 5
        };

        public enum CHW_RESERVOIR_STATES
        {
            CHW_RESERVOIR_ERROR = 0,
            CHW_RESERVOIR_EMPTY = 1,
            CHW_RESERVOIR_MIDDLE = 2,
            CHW_RESERVOIR_HIGH = 3
        };

        public enum CHW_SOUND_LEVELS
        {
            CHW_SOUND_LEVEL_FIRST = 0,
            CHW_SOUND_LOUD = CHW_SOUND_LEVEL_FIRST,
            CHW_SOUND_MEDIUM_LOUD = 1,
            CHW_SOUND_MEDIUM_SOFT = 2,
            CHW_SOUND_SOFT = 3,
            CHW_SOUND_LEVEL_LAST = CHW_SOUND_SOFT
        };

        // bit masks for pump errors
        public enum CHW_PUMP_ERRORS
        {
            CHW_PUMP_NO_FAULT = HW_ERROR.HW_NO_FAULT,
            CHW_PUMP_FAIL_TO_MAINTAIN_SPEED = 1,
            CHW_PUMP_INTEGRAL_ERROR = 2,
            CHW_PUMP_VOLUME_ERROR = 4,
            CHW_MOTOR_FAULT = 8
        };

        // bit mask for power supply overtemp errors
        public enum CHW_PS_OT_ERRORS
        {
            CHW_PS_NO_FAULT = HW_ERROR.HW_NO_FAULT,
            CHW_DC_NOT_OK = 1,                   // DC output is not OK
            CHW_POWER_FAIL = 2,                  // Power Fail interrupt
            CHW_PS_OTW = 4,                      // Power supply overtemp warning
            CHW_SS_OTW = 8                       // Seal-safe overtemp warning
        };

        // SEAL SAFE OVER TEMP WARNING
        public enum CHW_SEAL_SAFE_OPERATION
        {
            CHW_SEAL_SAFE_OPERATION_NOT_AVAILABLE = 0,
            CHW_SEAL_SAFE_ON = 1,
            CHW_SEAL_SAFE_OFF = 2
        };

        // SEAL SAFE OVER TEMP WARNING
        public enum CHW_SEAL_SAFE_OVER_TEMP_WARN
        {
            CHW_SEAL_SAFE_OTW_NOT_AVAILABLE = 0,
            CHW_SEAL_SAFE_OTW = 1,
            CHW_SEAL_SAFE_TEMP_OK = 2
        };


        // POWER SUPPLY OVER VOLTAGE PROTECT TEST RESULT
        public enum CHW_PS_OVP_TEST_RESULT
        {
            CHW_OVP_TEST_RESULT_NOT_AVAILABLE = 0,
            CHW_OVP_TEST_RESULT_FAIL = 1,
            CHW_OVP_TEST_RESULT_PASS = 2,
        };

        public enum CHW_PS_TEST_STATUS
        {
            CHW_PS_TEST_STATUS_NOT_AVAILABLE = 0,
            CHW_PS_TEST_STATUS_OFF = 1,
            CHW_PS_TEST_STATUS_ON = 2
        };

        ///
        //added to minimize scope
        ///----------------------
        public enum HW_WATCHDOG_STATUS
        {
            HW_WATCHDOG_STATUS_NO_FAILURE,
            HW_WATCHDOG_STATUS_WRONG_A5_BYTE,
            HW_WATCHDOG_STATUS_WRONG_F0_BYTE,
            HW_WATCHDOG_STATUS_F0_WRITE_WAITING,
            HW_WATCHDOG_STATUS_A5_WRITE_WAITING,
            HW_WATCHDOG_STATUS_TIMEOUT,
            HW_WATCHDOG_STATUS_POWER_UP,
            HW_WATCHDOG_STATUS_UNKNOWN
        };

        public enum HW_PS_OUTPUT_STATUS
        {
            HW_PS_OUTPUT_STATUS_FIRST,
            HW_PS_OUTPUT_STATUS_UNKNOWN = HW_PS_OUTPUT_STATUS_FIRST,
            HW_PS_OUTPUT_STATUS_ENABLED,
            HW_PS_OUTPUT_STATUS_DISABLED,
            HW_PS_OUTPUT_STATUS_LAST = HW_PS_OUTPUT_STATUS_DISABLED
        };
        //-----------------------------------


        // sh.hpp
        public enum SHW_RESERVOIR_STATES
        {
            SHW_RESERVOIR_EMPTY = 1,
            SHW_RESERVOIR_LOW = 2
        };

        public enum SHW_EVENTS
        {
            SHW_NORMAL_UPDATE = 0,
            SHW_PAUSE_EVENT = 1,     // event
            SHW_STOP_EVENT = 2,      // event
            SHW_DOOR_EVENT = 3,      // event
            SHW_RESERVOIR_EVENT = 4, // event
            SHW_RETURN_EVENT = 5,    // event
            SHW_AIR_EVENT = 6,       // event
            SHW_RETURN_PUMP_TOO_FAST_EVENT = 7, //event
            SHW_AC_PUMP_TOO_FAST_EVENT = 8   //event
        };

        public enum SHW_DIRECTION
        {
            SHW_RETURN = 0,
            SHW_DRAW = 1
        };
    }
}

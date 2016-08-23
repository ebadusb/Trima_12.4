/*
 * Copyright (c) 1995, 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/trima_common/rcs/hw.h 1.2 2009/07/08 16:59:22Z dslausb Exp dslausb $
 * $Log: hw.h $
 * Revision 1.2  2009/07/08 16:59:22Z  dslausb
 * STS Updates
 * Revision 1.1  2000/04/06 21:50:15Z  BD10648
 * Initial revision
 * Revision 1.2  1997/05/09 21:53:19  bs04481
 * Defines for centrifuge errors
 * Revision 1.1  1997/02/11 18:01:36  bs04481
 * Initial revision
 *
 * TITLE:      hw.hpp, Focussed System universal hardware
 *             structure defines
 *
 * ABSTRACT:   Defines strucures used with both hardware drivers
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS8.DOC
 * Test:             I:\ieee1498\STD8.DOC
 */

#ifndef HW_H
#define HW_H

// typedefs
// states

enum HW_CASSETTE_STATES
{
   HW_CASSETTE_UNKNOWN = 0,
   HW_CASSETTE_DOWN    = 1,
   HW_CASSETTE_UP      = 2
};

enum HW_VALVE_STATES
{
   HW_VALVE_UNKNOWN = 0,
   HW_VALVE_COLLECT = 1,
   HW_VALVE_OPEN    = 2,
   HW_VALVE_RETURN  = 3
};

enum HW_DOOR_STATES
{
   HW_DOOR_OPEN              = 0,
   HW_DOOR_LOCKED_STATE      = 1,
   HW_DOOR_CLOSED_STATE      = 2,
   HW_DOOR_CLOSED_AND_LOCKED = 3
};

enum HW_ORDERS
{
   HW_ORDERS_FIRST=0,
   HW_DISABLE     = HW_ORDERS_FIRST,
   HW_ENABLE      = 1,
   HW_ORDERS_LAST =HW_ENABLE
};

enum HW_SWITCH_STATES
{
   HW_SWITCH_DISABLED = 0,
   HW_SWITCH_ENABLED  = 1
};

enum HW_ERROR
{
   HW_NO_FAULT = 0,
   HW_FAULT    = 1
};

// bit masks for centrifuge errors
enum HW_CENT_ERRORS
{
   HW_CENT_NO_FAULT       = HW_NO_FAULT,
   HW_CENT_HARDWARE_ERROR = 1,         // could be any: reverse, over current,
                                       // under voltage or FPGA-sensed commutation
   HW_CENT_INTEGRAL_ERROR = 2,         // means integration component of the
                                       // control loop is not sufficient to make
                                       // desired speed

   HW_CENT_REVERSE_BIT = 4           // means that the centrifuge had a reverse fault

};

enum HW_WATCHDOG_STATUS
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

enum HW_PS_OUTPUT_STATUS
{
   HW_PS_OUTPUT_STATUS_FIRST,
   HW_PS_OUTPUT_STATUS_UNKNOWN=HW_PS_OUTPUT_STATUS_FIRST,
   HW_PS_OUTPUT_STATUS_ENABLED,
   HW_PS_OUTPUT_STATUS_DISABLED,
   HW_PS_OUTPUT_STATUS_LAST=HW_PS_OUTPUT_STATUS_DISABLED
};





#endif // HW_H

/* FORMAT HASH 50198f64675bb25a0438c1b5283b429a */

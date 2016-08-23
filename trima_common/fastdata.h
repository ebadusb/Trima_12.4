/*
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/trima_common/rcs/fastdata.h 1.3 2000/10/11 22:11:21Z bs04481 Exp spriusb $
 * $Log: fastdata.h $
 * Revision 1.3  2000/10/11 22:11:21Z  bs04481
 * Add Speed Order to 10ms data.  Rearrange enumeration.
 * Revision 1.2  2000/08/07 23:03:41  bs04481
 * Add type enumerations for fastdata messages
 * Revision 1.1  2000/05/24 19:53:43  HR10414
 * Initial revision
 * Revision 1.6  1998/02/04 21:27:20  bs04481
 * Implementation of fast response to APS violations in the control
 * driver.
 * Revision 1.5  1996/07/24 19:49:39  SS03309
 * fix MKS
 * Revision 1.4  1996/07/02 14:14:43  SS03309
 * Fixed comment spelling
 * Revision 1.3  1996/06/27 15:57:59  SS03309
 * Comments
 *
 * TITLE:      fastdata.hpp, Structures and enum for fast data log
 *             messages.
 *
 * ABSTRACT:   These defines are used with buffmsg.hpp to
 *             send messages containing arrays of high speed sampled
 *             data.  This is used for access pressure and ultrasonics.
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 */

#ifndef FASTDATA_HPP                   // prevent multiple includes
#define FASTDATA_HPP

// array length
#define FASTDATALENGTH  (100)          // array size

// source defines
enum DATA_SOURCE
{
   CONTROL_ACCESS_PRESSURE,
   CONTROL_UPPER_US_AIR,
   CONTROL_UPPER_US_FLUID,
   CONTROL_LOWER_US_AIR,
   CONTROL_LOWER_US_FLUID,
   SAFETY_LOWER_US_AIR,
   SAFETY_LOWER_US_FLUID,
   CENT_RAW_ENCODER,
   CENT_AVG_ENCODER,
   CENT_INTERNAL_ORDER,
   CENT_DELTA_RPM,
   CENT_INTEGRAL,
   CENT_CONTROL,
   CENT_ERRORS,
   AC_PUMP_ENCODER,
   INLET_PUMP_ENCODER,
   PLASMA_PUMP_ENCODER,
   PLATELET_PUMP_ENCODER,
   RETURN_PUMP_ENCODER,
   AC_PUMP_ORDER,
   INLET_PUMP_ORDER,
   PLASMA_PUMP_ORDER,
   PLATELET_PUMP_ORDER,
   RETURN_PUMP_ORDER,
   AC_PUMP_INTEGRATION_ERROR,
   INLET_PUMP_INTEGRATION_ERROR,
   PLASMA_PUMP_INTEGRATION_ERROR,
   PLATELET_PUMP_INTEGRATION_ERROR,
   RETURN_PUMP_INTEGRATION_ERROR,
   AC_PUMP_VOLUME_ERROR,
   INLET_PUMP_VOLUME_ERROR,
   PLASMA_PUMP_VOLUME_ERROR,
   PLATELET_PUMP_VOLUME_ERROR,
   RETURN_PUMP_VOLUME_ERROR,
   AC_PUMP_DELTA_SPEED,
   INLET_PUMP_DELTA_SPEED,
   PLASMA_PUMP_DELTA_SPEED,
   PLATELET_PUMP_DELTA_SPEED,
   RETURN_PUMP_DELTA_SPEED,
   AC_PUMP_ERRORS,
   INLET_PUMP_ERRORS,
   PLASMA_PUMP_ERRORS,
   PLATELET_PUMP_ERRORS,
   RETURN_PUMP_ERRORS,
   AC_PUMP_HALL_TIME,
   INLET_PUMP_HALL_TIME,
   PLASMA_PUMP_HALL_TIME,
   PLATELET_PUMP_HALL_TIME,
   RETURN_PUMP_HALL_TIME,
   AC_PUMP_SPEED_ORDER,
   INLET_PUMP_SPEED_ORDER,
   PLASMA_PUMP_SPEED_ORDER,
   PLATELET_PUMP_SPEED_ORDER,
   RETURN_PUMP_SPEED_ORDER,
   DATA_SOURCE_LAST     // Do not include new entries after this
};

// message structure
typedef struct
{
   DATA_SOURCE source;                 // source of data
   short       raw[FASTDATALENGTH];    // raw data readings
   int         time[FASTDATALENGTH];   // relative timestamp of raw data reading
} FASTDATA;

#define FASTFILTERLENGTH (1)
// message structure
typedef struct
{
   short raw[FASTFILTERLENGTH];          // raw data readings
} FASTFILTER;

#define FASTDATALENGTHFORLONGS  (50)          // array size
// message structure
typedef struct
{
   DATA_SOURCE source;                       // source of data
   long        raw[FASTDATALENGTHFORLONGS];  // raw data readings
   int         time[FASTDATALENGTHFORLONGS]; // relative timestamp of raw data reading
} FASTDATALONG;

#endif

/* FORMAT HASH c7c9d851f54445e178705010493fae5f */

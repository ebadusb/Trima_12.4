/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/low_level/ctl_drv/rcs/c_ultra.h 1.2 2003/04/24 16:43:27Z jl11312 Exp jl11312 $
 * $Log: c_ultra.h $
 * Revision 1.2  2003/04/24 16:43:27Z  jl11312
 * - removed redundant code in ultrasonics interrupt handler for setting return pump direction (see IT 5987)
 * Revision 1.1  2002/06/13 17:07:37Z  jl11312
 * Initial revision
 * Revision 1.7  2001/07/20 20:06:14  jl11312
 * - additional logging for reservoir sensor events and fatal errors
 * Revision 1.6  2001/07/16 21:23:42  jl11312
 * - added extra logging for ultrasonic level sensors (IT 5202)
 * Revision 1.5  2001/04/05 20:15:54  jl11312
 * - internal timer handling changes required for versa logic CPU board
 * Revision 1.4  2000/08/07 23:00:17  bs04481
 * Allow visibility to 10ms data used/created by the drivers.
 * See IT 4741
 * Revision 1.3  1997/11/21 19:35:33  bs04481
 * Cause fatal error in control for bad ultrasonics hardware
 * Revision 1.2  1997/03/10 22:10:07  bs04481
 * I/O changes to additionally support 2.6 HW.  Initial pass.
 * Revision 1.1  1996/07/30 18:47:56  SS03309
 * Initial revision
 *
 * TITLE:      Control Driver 50 ms interrupt handler
 *
 * ABSTRACT:   Processes Air to Donor and Ultrasonics
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 */

#ifndef _C_ULTRA_INCLUDE
#define _C_ULTRA_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPECIFICATION:    Attaches 50 ms timer interrupt
 *
 * ERROR HANDLING:   _FATAL_ERROR
 */
void ctl_interruptAttach (short* lowAGCPtr, short* highAGCPtr);

/*
 * SPECIFICATION:    Deattaches 50 ms timer
 *
 * ERROR HANDLING:   _FATAL_ERROR
 */
void ctl_interruptDeattach (void);

/*
 * SPECIFICATION:    Get Lower Ultrasonic sensor
 *
 * ERROR HANDLING:   none.
 */
unsigned char ctl_interruptLowerSensor (unsigned char* error);

/*
 * SPECIFICATION:    Get Upper Ultrasonic sensor
 *
 * ERROR HANDLING:   none.
 */
unsigned char ctl_interruptUpperSensor (unsigned char* error);

long ctl_interruptLowerSensorAir (void);
long ctl_interruptLowerSensorFluid (void);
long ctl_interruptUpperSensorAir (void);
long ctl_interruptUpperSensorFluid (void);

unsigned long ctl_interruptLowerSensorAccTotal (void);
unsigned long ctl_interruptUpperSensorAccTotal (void);


void ctl_interruptSetUpperSensorMode (void);


#ifdef __cplusplus
};
#endif

#endif /* #ifndef _C_ULTRA_INCLUDE */

/* FORMAT HASH e95ef390fcd329f51a09d83f261950bb */

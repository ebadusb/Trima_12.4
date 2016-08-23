/*
 * Copyright (c) 1995, 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      s_type.h, Focussed System universal typdefs
 *
 * ABSTRACT:   Defines universal types
 *
 */

#ifndef S_TYPE_H
#define S_TYPE_H

// typedefs

// timer state defines
#define TIMER_ARMED          1     // arm a given timer "channel"
#define TIMER_DISARMED       2     // disarm a given timer channel
#define TIMER_LIMIT_EXPIRED  3     // soft time timer interval HAS expired

// rb valves must be in return position allow 20 sample fault tolerant "time"
// during rb monitoring, testing indicates that proc switches state to rb before moving
// valves and valves take at least five samples to get into return position
// 20 samples is approximately 10s the same fault tolerant time allowed for valve position
// monitoring suring run.
// it 9885 change this value to match MAX_SAMPLES_BAD_VALVE in s_valve.cpp. keeps from alarming too early.
#define RB_VALVE_FAULT_TOLERANT_THRESHOLD   40

#define MAX_CENT_SPD_DOOR_NOT_LOCKED_AND_CLOSED     60.0f

// 10/01/96 mod pre tim gordon for door fault tolerance
// alarm for door if fault condition persists for 4 consecutive samples.
//
#define DOOR_FAULT_TOLERANCE                   4

#define BILLION          1000000000L                    // clock_gettime time conversion factor
#define MAX_CHAR 256

typedef unsigned char T1STATUS;

#endif // S_TYPE_H

/* FORMAT HASH 607d4ccfee675823c94877b89235b42d */

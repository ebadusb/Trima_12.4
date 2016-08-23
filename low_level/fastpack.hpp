/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      fastpack.hpp, Fast data from aps, pumps, etc
 *
 * ABSTRACT:   Data collection for fata data types
 *
 */

#ifndef _FASTPACK_INCLUDE
#define _FASTPACK_INCLUDE

#include "fastdata.h"
#include "msginfo.hpp"
#include "ostime.hpp"

// SPECIFICATION:    used to collect access pressure data,
//                   ultrasonic data, and pump data.
//
// ERROR HANDLING:   none.

class fastpack
{
public:
   fastpack(DATA_SOURCE src, const char* sharename);
   ~fastpack();
   void dataPoint (short data);

private:
   FASTPAK* _FPtr;
   rawTime  startTime;
};

// SPECIFICATION:    used to collect access pressure data,
//                   ultrasonic data, and pump data.
//
// ERROR HANDLING:   none.

class fastpacklong
{
public:
   fastpacklong(DATA_SOURCE src, const char* sharename);
   ~fastpacklong();
   void dataPoint (long data);

private:
   FASTPAKLONG* _FPtr;
   rawTime      startTime;
};

#endif /* ifndef _FASTPACK_INCLUDE */

/* FORMAT HASH 018b439393a177f2b3e283beb8fed3f2 */

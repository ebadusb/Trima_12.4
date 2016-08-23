/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: M:/BCT_Development/vxWorks/Trima/trima_common/rcs/trima_assert.cpp 1.2 2002/06/13 18:23:42 jl11312 Exp $
 * $Log: trima_assert.cpp $
 * Revision 1.2  2002/06/13 18:23:42  jl11312
 * - modifications for initial control driver port to vxWorks
 * Revision 1.1  2002/04/05 17:00:08  pn02526
 * Initial revision
 *
 *
 * TITLE:      trima_assert.c - used by the trima_assert macro to shut system down.
 *
 * ABSTRACT:
 *
 */
#include <vxworks.h>

#include "trima_assert.h"
#include "trima_datalog.h"

/* SPECIFICATION:  trima_assert_failure() */
/*   handles assertion failure via _FATAL_ERROR() */
/* ERROR HANDLING: none */
void trima_assert_failed (const char* expr, const char* file, int line)
{
   DataLog_Critical critical;

   critical(file, line) << "assertion failed: " << expr << ")" << endmsg;
   _FATAL_ERROR(__FILE__, __LINE__, "TRIMA_ASSERT");
}

/* FORMAT HASH 8537fdabd0467f3ebaebae36d6fe1782 */

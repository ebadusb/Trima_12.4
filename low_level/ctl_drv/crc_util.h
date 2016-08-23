/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 * $Log$
 *
 */

#ifndef _CRC_UTIL_INCLUDE
#define _CRC_UTIL_INCLUDE

#include "crcgen.h"

/*
 *	The following macro handles crc generation for a structure
 * which follows the format:
 *
 *	struct name
 * {
 *		data items to be included in crc ...
 *		unsigned long crc;
 *
 *      data items not included in crc ...
 * };
 *
 */
#define  CRCGEN32_STRUCT(pResult, pStruct) \
   crcgen32((pResult), (unsigned char*)(pStruct), (char*)&((pStruct)->crc) - (char*)(pStruct))

#endif /* ifndef _CRC_UTIL_INCLUDE */

/* FORMAT HASH c05a15d2b3a2e8aa71c76a5cf03dd1cc */

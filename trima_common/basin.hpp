/*
 * Copyright (c) 1995-1999 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: I:/Trima_Project/Source/router/rcs/basin.hpp 1.1 1999/07/23 01:42:11 BS04481 Exp BS04481 $
 * $Log: basin.hpp $
 * Revision 1.1  1999/07/23 01:42:11  BS04481
 * Initial revision
 *
 * TITLE:      basin.hpp, definition of basin data structure from
 *             the driver to higher level code.
 *
 * ABSTRACT:   Takes basin data from the getByte object.  Validates and
 *             sends to higher level code.
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 * PUBLIC FUNCTIONS:
 *
 *
 */

#ifndef BASIN_HPP
#define BASIN_HPP

enum BASIN_DATA
{
   NO_BASIN_DATA = 0,
   BASIN_DATA_CORRUPT,
   BASIN_TEMP,
   BASIN_FAN_SPEED
};

typedef struct
{
   BASIN_DATA dataType;
   float      temp;         // temp is degrees C
} basinTemp;

#endif // BASIN_HPP

/* FORMAT HASH a3ea2be4d078cc6ee0c47bbfac2dda8d */

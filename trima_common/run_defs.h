/*
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      run_defs.h
 *
 * AUTHOR:     Tom Minyard
 *
 * ABSTRACT:   This header file contains constant definitions that are used
 *              throughout the algorithm procedure run.
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 */


#ifndef RUN_DEFS_H  //  Prevents multiple compilations.
#define RUN_DEFS_H

#define MINIMUM_PROCEDURE_TIME 10.0f           // Minimum procedure time allowed
#define ABSOLUTE_MAXIMUM_PROCEDURE_TIME 150.0f // Minimum procedure time allowed

#define MAX_RATIO 13.7f                 // Max ratio in general.
#define MIN_RATIO 6.0f                  // Min ratio in general.

#define MAX_INFUSION_RATE      1.2f     // Maximum allowed infusion rate.
#define MIN_INFUSION_RATE      0.6f     // Minimum allowed infusion rate.

#endif

/* FORMAT HASH 2210297ba2dea961adaa6228fc55f0e4 */

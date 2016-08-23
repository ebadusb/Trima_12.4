/*
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      Power Fail Handling
 *
 */

#ifndef PFW_INT_H
#define PFW_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/* SPECIFICATION:    Initializes power fail handling
 *
 *                   Parameters:
 *                   powerFailFlag =0, normal, =1 if power fail
 *
 * ERROR HANDLING:   _FATAL_ERROR
 */
void pfw_attach (unsigned char* powerFailFlag);

/* SPECIFICATION:    Called every 10 ms to check power fail status
 *
 * ERROR HANDLING:   none.
 */
void pfw_timer (void);

#ifdef __cplusplus
};
#endif

#endif

/* FORMAT HASH d7129296761d71773e7f6188665b54d1 */

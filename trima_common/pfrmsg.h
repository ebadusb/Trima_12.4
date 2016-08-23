/*******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pfrmsg.h
 *             enumerations for PFR related messages
 *
 ******************************************************************************/


#ifndef INCLUDE_PFRMSG
#define INCLUDE_PFRMSG


//
// This enum used by Barcode, Biometric Manager (GUI) and Vista for
// PFR decision making
//
enum PFR_Status
{
   NoRestore,      // PFR will not be performed
   RestorePFRData  // PFR is in progress
};

#endif // ifndef INCLUDE_PFRMSG

/* FORMAT HASH 8e6f0521250e62d9d8fefacebba8edb5 */

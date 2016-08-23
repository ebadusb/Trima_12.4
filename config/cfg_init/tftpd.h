/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //depot/main/embedded/Trima/Main/config/cfg_init/tftpd.h#1 $
 * $Log$
 *
 */

#ifndef _TFTPD_INCLUDE
#define _TFTPD_INCLUDE

#include <semLib.h>

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/* Sem to use to block on waiting for task to start */
static SEM_ID bootSyncSem = semMCreate(SEM_Q_FIFO);


typedef enum
{
   TFTPD_FileTransferRequest,
   TFTPD_FileTransferCompleted
} TFTPD_CallBackStatus;

typedef bool tftpdStatusCallBack (TFTPD_CallBackStatus status, struct in_addr& clientAddr, const char* fileName);

void startTFTPD (tftpdStatusCallBack* statusCallBack);
void endTFTPD (void);

#ifdef __cplusplus
}  // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _TFTPD_INCLUDE */

/* FORMAT HASH 4e03d9045e0d0bb041b916fb81d5d0ee */

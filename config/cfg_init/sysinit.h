/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#ifndef _SYSINIT_INCLUDE
#define _SYSINIT_INCLUDE

#include <setjmp.h>
#include "trima_tasks.h"

typedef enum
{
   NormalBootMode  = TTP_NormalBoot,
   ServiceBootMode = TTP_ServiceBoot,
   AltBootMode     = TTP_AltBoot
} BootMode;

extern jmp_buf  sysinit_abort;
extern BootMode sysinit_boot_mode;

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

void sysinit_datalog (void);
void sysinit_main (void);
void usrShellInit (void);

void sysinit_waitServiceModeAcknowledge (void);
void sysinit_setup_environment_variables (void);
void sysinit_prestart_node (void);
void sysinit_logTrapWrite (void);
void sysinit_telnet_configuration (void);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _SYSINIT_INCLUDE */

/* FORMAT HASH cc7a2bf9d76ec20f32d524c5cac8ed6a */

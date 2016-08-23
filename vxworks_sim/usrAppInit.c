/* usrAppInit.c - stub application initialization routine */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,02jun98,ms   written
*/

/*
DESCRIPTION
Initialize user application code.
*/ 

#include <vxWorks.h>
#include <tasklib.h>
#include <loadlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <sysLib.h>
#include <sysSymTbl.h>
#include <ioLib.h>
#include <sys/fcntlcom.h>

#include "junkSymbols.c"

/******************************************************************************
*
* usrAppInit - initialize the users application
*/ 

void usrAppInit (void)
    {
		struct timespec data;

#ifdef	USER_APPL_INIT
	USER_APPL_INIT;		/* for backwards compatibility */
#endif

#ifdef CONTROL_BUILD
	int	partNum;

	for ( partNum=FirstPartition; partNum<EndOfPartitionEntries; partNum+=1 )
	{
		trimaPartitionData[partNum].bytesTotal = (long long)50*1024*1024;
	}

#endif /* ifdef CONTROL_BUILD */

	trimaSysLoadOptionFiles();

	memDrv();
	
   /* add application specific code here */
   hostAdd("control", "90.0.0.1");

	putenv("MACHINE=1TVXSIM");
    /* add application specific code here */
    }



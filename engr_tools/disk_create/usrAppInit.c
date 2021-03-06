
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

/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * Partition and format a hard disk for use on a Trima/vxWorks system
 *
 * $Header$
 * $Log$
 *
 */

#include <vxWorks.h>
#include <cbioLib.h>
#include <dcacheCbio.h>
#include <dosFsLib.h>
#include <dpartCbio.h>
#include <rawFsLib.h>
#include <stdio.h>
#include <string.h>

#include <drv/hdisk/ataDrv.h>
#include <sys/stat.h>

/*
 * These functions aren't included in the standard DosFS2 header files,
 * so prototypes are defined here
 */
STATUS usrFdiskPartRead
    (
    CBIO_DEV_ID dev,      			/* device from which to read blocks */
    PART_TABLE_ENTRY *pPartTab, 	/* table where to fill results */
    int nPart               		/* # of entries in <pPartTable> */
    );

STATUS usrFdiskPartCreate
    (
    CBIO_DEV_ID dev, 				/* device representing the entire disk */
    int		nPart,					/* how many partitions needed, default=1, max=4 */
    ...
    );

STATUS chkdsk
    (
    char *	devName,					/* device name */
    unsigned int	repairLevel,	/* how to fix errors */
    unsigned int	verbose			/* verbosity level */
    );

/*
 * Local functions
 */
static int vxbootOptions(DOS_VOL_CONFIG * cfg);
static int trimaOptions(DOS_VOL_CONFIG * cfg);
static int configOptions(DOS_VOL_CONFIG * cfg);
static int machineOptions(DOS_VOL_CONFIG * cfg);
static int copyFile(const char * from, const char * to);
void buildTrimaDisk(void);

/*
 *	Boot image for the hard disk master boot record (MBR).
 *
 * The MBR is formatted as follows:
 *
 *	0x000 - 0x1bd	: master boot image (executes in x86 real mode)
 * 0x1be - 0x1cd  : disk partition 1 information  
 * 0x1ce - 0x1dd  : disk partition 2 information
 * 0x1de - 0x1ed  : disk partition 3 information
 * 0x1ee - 0x1fd  : disk partition 4 information
 * 0x1fe - 0x1ff  : boot sector signature (0x55 0xaa)
 */
static unsigned char masterBootImage[] =
{
	0xFA, 0x33, 0xC0, 0x8E, 0xD0, 0xBC, 0x00, 0x7C, 0x8B, 0xF4, 0x50, 0x07, 0x50, 0x1F, 0xFB, 0xFC, /* 00 */
	0xBF, 0x00, 0x06, 0xB9, 0x00, 0x01, 0xF2, 0xA5, 0xEA, 0x1D, 0x06, 0x00, 0x00, 0xBE, 0xBE, 0x07, /* 10 */
	0xB3, 0x04, 0x80, 0x3C, 0x80, 0x74, 0x0E, 0x80, 0x3C, 0x00, 0x75, 0x1C, 0x83, 0xC6, 0x10, 0xFE, /* 20 */
	0xCB, 0x75, 0xEF, 0xCD, 0x18, 0x8B, 0x14, 0x8B, 0x4C, 0x02, 0x8B, 0xEE, 0x83, 0xC6, 0x10, 0xFE, /* 30 */
	0xCB, 0x74, 0x1A, 0x80, 0x3C, 0x00, 0x74, 0xF4, 0xBE, 0x8B, 0x06, 0xAC, 0x3C, 0x00, 0x74, 0x0B, /* 40 */
	0x56, 0xBB, 0x07, 0x00, 0xB4, 0x0E, 0xCD, 0x10, 0x5E, 0xEB, 0xF0, 0xEB, 0xFE, 0xBF, 0x05, 0x00, /* 50 */
	0xBB, 0x00, 0x7C, 0xB8, 0x01, 0x02, 0x57, 0xCD, 0x13, 0x5F, 0x73, 0x0C, 0x33, 0xC0, 0xCD, 0x13, /* 60 */
	0x4F, 0x75, 0xED, 0xBE, 0xA3, 0x06, 0xEB, 0xD3, 0xBE, 0xC2, 0x06, 0xBF, 0xFE, 0x7D, 0x81, 0x3D, /* 70 */
	0x55, 0xAA, 0x75, 0xC7, 0x8B, 0xF5, 0xEA, 0x00, 0x7C, 0x00, 0x00, 0x49, 0x6E, 0x76, 0x61, 0x6C, /* 80 */
	0x69, 0x64, 0x20, 0x70, 0x61, 0x72, 0x74, 0x69, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x74, 0x61, 0x62, /* 90 */
	0x6C, 0x65, 0x00, 0x45, 0x72, 0x72, 0x6F, 0x72, 0x20, 0x6C, 0x6F, 0x61, 0x64, 0x69, 0x6E, 0x67, /* a0 */
	0x20, 0x6F, 0x70, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6E, 0x67, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, /* b0 */
	0x6D, 0x00, 0x4D, 0x69, 0x73, 0x73, 0x69, 0x6E, 0x67, 0x20, 0x6F, 0x70, 0x65, 0x72, 0x61, 0x74, /* c0 */
	0x69, 0x6E, 0x67, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* d0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* e0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* f0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 100 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 110 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 120 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 130 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 140 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 150 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 160 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 170 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 180 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 190 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 1a0 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* 1b0 */
};

/*
 *	vxLoader image for the active partition boot sector
 *
 *	The vxLoader image is placed in the boot sector of the active
 * disk partition.  The format of this sector is as follows:
 *
 * 0x000 - 0x002 : relative jump to offset 0x03e
 * 0x003 - 0x00a : 8 byte OS identifier ("WINDRIVR")
 * 0x03e - 0x1fd : vxLoader image (byte 0x1d2 must be set to 0x80 +
 *							disk controller number - 0x80 = first disk,
 *							0x81 = second disk, etc.)
 *	0x1fe - 0x1ff  : boot sector signature (0x55 0xaa)
 */
static unsigned char vxLoaderImage[] =
{
																													0xFA, 0x88, /* 3e */
   0x16, 0x24, 0x7C, 0xB4, 0x08, 0xCD, 0x13, 0x72, 0x11, 0x83, 0xE1, 0x3F, 0x89, 0x0E, 0x18, 0x7C, /* 40 */
   0xFE, 0xC6, 0x32, 0xD2, 0x86, 0xF2, 0x89, 0x16, 0x1A, 0x7C, 0xFA, 0x8C, 0xC8, 0x8E, 0xD8, 0x8E, /* 50 */
   0xC0, 0xFC, 0x8E, 0xD0, 0xBC, 0xFE, 0x7B, 0xFB, 0xBE, 0xE1, 0x7D, 0xE8, 0x14, 0x01, 0x33, 0xDB, /* 60 */
   0x8B, 0xC3, 0x8B, 0xD3, 0xA0, 0x10, 0x7C, 0xF7, 0x26, 0x16, 0x7C, 0x8B, 0x0E, 0x0E, 0x7C, 0x03, /* 70 */
   0x0E, 0x1C, 0x7C, 0x13, 0x16, 0x1E, 0x7C, 0x03, 0xC8, 0x13, 0xD3, 0x89, 0x0E, 0x00, 0x7B, 0x89, /* 80 */
   0x16, 0x02, 0x7B, 0xBE, 0x03, 0x7C, 0xBF, 0xF9, 0x7D, 0xC7, 0x06, 0x36, 0x7C, 0x20, 0x00, 0xB9, /* 90 */
   0x05, 0x00, 0xF3, 0xA6, 0x75, 0x06, 0xC7, 0x06, 0x36, 0x7C, 0x40, 0x00, 0xA1, 0x36, 0x7C, 0xF7, /* a0 */
   0x26, 0x11, 0x7C, 0x8B, 0x36, 0x0B, 0x7C, 0x03, 0xC6, 0x48, 0xF7, 0xF6, 0x8B, 0xC8, 0x51, 0xA1, /* b0 */
   0x00, 0x7B, 0x8B, 0x16, 0x02, 0x7B, 0xBB, 0x00, 0x7E, 0xE8, 0xD0, 0x00, 0x73, 0x03, 0xE9, 0xA5, /* c0 */
   0x00, 0x8B, 0x16, 0x0B, 0x7C, 0xBF, 0x00, 0x7E, 0xBE, 0xEB, 0x7D, 0x57, 0xB9, 0x0B, 0x00, 0xF3, /* d0 */
   0xA6, 0x5F, 0x74, 0x1C, 0x03, 0x3E, 0x36, 0x7C, 0x2B, 0x16, 0x36, 0x7C, 0x75, 0xEA, 0x83, 0x06, /* e0 */
   0x00, 0x7B, 0x01, 0x83, 0x16, 0x02, 0x7B, 0x00, 0x59, 0xE2, 0xC3, 0xBE, 0xEA, 0x7D, 0xEB, 0x79, /* f0 */
   0x59, 0x01, 0x0E, 0x00, 0x7B, 0x83, 0x16, 0x02, 0x7B, 0x00, 0x33, 0xC9, 0xBB, 0x1C, 0x00, 0x83, /* 100 */
   0x3E, 0x36, 0x7C, 0x40, 0x75, 0x03, 0x83, 0xC3, 0x20, 0x8B, 0x01, 0x43, 0x43, 0x8B, 0x11, 0xF7, /* 110 */
   0x36, 0x0B, 0x7C, 0x40, 0xA3, 0x04, 0x7B, 0x83, 0xEB, 0x04, 0x8B, 0x01, 0x48, 0x48, 0x8A, 0x0E, /* 120 */
   0x0D, 0x7C, 0xF7, 0xE1, 0x03, 0x06, 0x00, 0x7B, 0x13, 0x16, 0x02, 0x7B, 0xBB, 0x00, 0x08, 0x8E, /* 130 */
   0xC3, 0x33, 0xDB, 0x50, 0x52, 0xE8, 0x54, 0x00, 0x5A, 0x58, 0x72, 0x2A, 0xBE, 0xF7, 0x7D, 0xE8, /* 140 */
   0x30, 0x00, 0xFF, 0x0E, 0x04, 0x7B, 0x74, 0x0D, 0x83, 0xC0, 0x01, 0x83, 0xD2, 0x00, 0x8C, 0xC3, /* 150 */
   0x83, 0xC3, 0x20, 0xEB, 0xDA, 0xA0, 0x24, 0x7C, 0x24, 0x80, 0x75, 0x06, 0xBA, 0xF2, 0x03, 0x32, /* 160 */
   0xC0, 0xEE, 0xFF, 0x2E, 0x7E, 0x7D, 0xBE, 0xE6, 0x7D, 0xE8, 0x06, 0x00, 0xEB, 0xFE, 0x00, 0x00, /* 170 */
   0x00, 0x08, 0x53, 0x50, 0x80, 0x3E, 0xE0, 0x7D, 0x00, 0x75, 0x0E, 0xAC, 0x0A, 0xC0, 0x74, 0x09, /* 180 */
   0xB4, 0x0E, 0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB, 0xF2, 0x58, 0x5B, 0xC3, 0x89, 0x1E, 0x06, 0x7B, /* 190 */
   0x8B, 0x1E, 0x18, 0x7C, 0xF7, 0xF3, 0x42, 0x8B, 0xDA, 0x33, 0xD2, 0xF7, 0x36, 0x1A, 0x7C, 0x86, /* 1a0 */
   0xE0, 0xB1, 0x06, 0xD2, 0xE0, 0x91, 0x0A, 0xCB, 0x8A, 0xF2, 0xBB, 0x05, 0x00, 0x53, 0x8B, 0x1E, /* 1b0 */
   0x06, 0x7B, 0x8A, 0x16, 0x24, 0x7C, 0xB8, 0x01, 0x02, 0x51, 0x52, 0xCD, 0x13, 0x5A, 0x59, 0x72, /* 1c0 */
   0x03, 0x5B, 0xF8, 0xC3, 0x33, 0xC0, 0xCD, 0x13, 0x5B, 0xFE, 0xCF, 0x75, 0xE0, 0xF9, 0xEB, 0xF3, /* 1d0 */
   0x00, 0x56, 0x31, 0x2E, 0x36, 0x00, 0x21, 0x52, 0x64, 0x00, 0x21, 0x42, 0x4F, 0x4F, 0x54, 0x52, /* 1e0 */
   0x4F, 0x4D, 0x20, 0x53, 0x59, 0x53, 0x00, 0x2B, 0x00, 0x56, 0x58, 0x45, 0x58, 0x54, 0x55, 0xAA  /* 1f0 */
};

/*
 * Call back functions used to modify the volume parameters for each partition
 * before they are formatted
 */
static int vxbootOptions(DOS_VOL_CONFIG * cfg)
{
	cfg->nFats = 2;
	cfg->volSerial = 0x5aa50001;
	strncpy(cfg->volLabel, "VXBOOT     ", 11);
	return 1;
}

static int trimaOptions(DOS_VOL_CONFIG * cfg)
{
	cfg->nFats = 2;
	cfg->secPerClust = 128;
	cfg->volSerial = 0x5aa50002;
	strncpy(cfg->volLabel, "TRIMA      ", 11);
	return 1;
}

static int configOptions(DOS_VOL_CONFIG * cfg)
{
	cfg->nFats = 2;
	cfg->secPerClust = 128;
	cfg->volSerial = 0x5aa50003;
	strncpy(cfg->volLabel, "CONFIG     ", 11);
	return 1;
}

static int machineOptions(DOS_VOL_CONFIG * cfg)
{
	cfg->nFats = 2;
	cfg->secPerClust = 128;
	cfg->volSerial = 0x5aa50004;
	strncpy(cfg->volLabel, "MACHINE    ", 11);
	return 1;
}

static int copyFile(const char * from, const char * to)
{
	int 			fromFD = open(from, O_RDONLY, 0644);
	int 			toFD = open(to, O_CREAT|O_RDWR, 0644);
	struct stat fileStat;
	char 			buffer[256];
	int  			result = 0; 

	if ( fromFD >= 0 && toFD >= 0 )
	{
		long	bytesCopied = 0;
		int	bytesRead;

		fstat(fromFD, &fileStat);
		printf("copying %s to %s: file length = %ld bytes\n", from, to, fileStat.st_size);

		/*
		 * Make destination a contiguous file (required for boot image)
		 */
		ioctl(toFD, FIOCONTIG, fileStat.st_size);
		while ( (bytesRead = read(fromFD, buffer, 256)) > 0 )
	   {
			bytesCopied += write(toFD, buffer, bytesRead);
	   }

		if ( bytesCopied != fileStat.st_size)
		{
			printf("Copy failed (%ld bytes written)\n", bytesCopied);
		}
		else
		{
			result = 1;
		}
	}
	else
	{
		if ( fromFD < 0 ) perror(from);
		if ( toFD < 0 ) perror(to);
	}

	if ( fromFD >= 0 ) close(fromFD);
	if ( toFD >= 0 ) close(toFD);
	return result;
}


/*
 * Main function to configure a Trima hard disk
 */
void usrAppInit (void)
{
   CBIO_DEV_ID partDev;
   ATA_RAW ataRaw;
   PART_TABLE_ENTRY partData[4];
   RAW_VOL_DESC * rawDev;
   int rawFD;
   FILE * fp;

	/*
	 * Create a device for reading/writing entire disk
	 */
	BLK_DEV * hdDev = ataDevCreate(0, 0, 0, 0);
   CBIO_DEV_ID cbio = dcacheDevCreate((CBIO_DEV_ID)hdDev, 0, 0x10000, "hd");

	ULONG	bytesPerBlock = hdDev->bd_bytesPerBlk;
	ULONG	totalBlocks = hdDev->bd_nBlocks;
   char bootSec[512];
 
	/*
	 * Partition sizes are specified in MBytes, and converted to percentages
	 * for usrFdiskPartCreate
	 */
	ULONG	vxbootPartSize = 10;
	ULONG	trimaPartSize = 300;
	ULONG	configPartSize = 20;
	ULONG	minMachineSize = 500;

	double	vxbootFraction = vxbootPartSize*1024.0*1024.0/bytesPerBlock/totalBlocks;
	double	trimaFraction = trimaPartSize*1024.0*1024.0/bytesPerBlock/totalBlocks;
	double	configFraction = configPartSize*1024.0*1024.0/bytesPerBlock/totalBlocks;

	int	vxbootPercent = (int)(vxbootFraction*100.0+1.0);
	int	trimaPercent = (int)(trimaFraction*100.0+1.0);
	int	configPercent = (int)(configFraction*100.0+1.0);
	int	machinePercent;
	double  	machineSize;

	int	machineNumber = -1;

	machinePercent = (100-vxbootPercent-trimaPercent-configPercent);
	machineSize = machinePercent/100.0 * totalBlocks * bytesPerBlock/1024.0/1024.0;

	printf("vxboot  : %d%% of disk (%d MBytes)\n", vxbootPercent, (int)(vxbootPercent/100.0 * totalBlocks * bytesPerBlock/1024.0/1024.0 + 0.5));
	printf("trima   : %d%% of disk (%d MBytes)\n", trimaPercent, (int)(trimaPercent/100.0 * totalBlocks * bytesPerBlock/1024.0/1024.0 + 0.5));
	printf("config  : %d%% of disk (%d MBytes)\n", configPercent, (int)(configPercent/100.0 * totalBlocks * bytesPerBlock/1024.0/1024.0 + 0.5));
	printf("machine : %d%% of disk (%d MBytes)\n", machinePercent, (int)(machinePercent/100.0 * totalBlocks * bytesPerBlock/1024.0/1024.0 + 0.5));
	if ( machineSize < minMachineSize )
	{
		printf("machine partition is too small.  Can't build Trima disk\n");
		return;
	}

	if ( bytesPerBlock != 512 )
	{
		printf("Only 512 byte disk blocks are supported\n");
		return;
	}

	usrFdiskPartCreate(cbio, 4, trimaPercent, configPercent, machinePercent);
   partDev = dpartDevCreate(cbio, 4, usrFdiskPartRead);
   dosFsDevCreate("/vxboot", dpartPartGet(partDev, 0), 0x100, NONE);
	dosFsDevCreate("/trima", dpartPartGet(partDev, 1), 0x100, NONE);
	dosFsDevCreate("/config", dpartPartGet(partDev, 2), 0x100, NONE);
	dosFsDevCreate("/machine", dpartPartGet(partDev, 3), 0x100, NONE);

	dosFsVolFormat("/vxboot", DOS_OPT_BLANK | DOS_OPT_FAT16, vxbootOptions);
	dosFsVolFormat("/trima", DOS_OPT_BLANK | DOS_OPT_FAT32, trimaOptions);
	dosFsVolFormat("/config", DOS_OPT_BLANK | DOS_OPT_FAT32, configOptions);
	dosFsVolFormat("/machine", DOS_OPT_BLANK | DOS_OPT_FAT32, machineOptions);

	/*
	 * Copy bootrom and vxWorks image to the vxboot partition
	 */
   printf("\nInsert vxWorks disk build boot 1 disk and press enter\n");
   fgets(bootSec, bytesPerBlock, stdin);
	if ( !copyFile("/fd0/bootrom.sys", "/vxboot/bootrom.sys") ) return;

   printf("\nInsert vxWorks disk build image disk and press enter\n");
   fgets(bootSec, bytesPerBlock, stdin);
	if ( !copyFile("/fd0/vxWorks", "/vxboot/vxWorks") ) return;
	if ( !copyFile("/fd0/hw.dat", "/config/hw.dat") ) return;
	if ( !copyFile("/fd0/sw.dat", "/config/sw.dat") ) return;
	if ( !copyFile("/fd0/config.dat", "/config/config.dat") ) return;

	mkdir("/trima/low_level");
	if ( !copyFile("/fd0/ctl_hw.out", "/trima/low_level/ctl_hw.out") ) return;

	/*
	 * Read back partition information to get block offsets
	 */
	if ( usrFdiskPartRead(cbio, partData, 4) != OK )
	{
		printf("Error reading partition information\n");
		return;
	}

	/*
	 *	Create a raw file system for the disk to allow writing the master boot
	 * record and vxboot partition boot sector
	 */
	rawDev = rawFsDevInit("RAW_HD:", hdDev);
	rawFD = open("RAW_HD:", O_RDWR, 0644);
	if ( rawFD < 0 )
	{
		perror("Error opening raw disk device for write\n");
		return;
	}

	/*
	 * Sector 0 must be modified to include the code for the
	 * master boot record, and to have the proper 55aa signature
	 */
	ioctl(rawFD, FIOSEEK, 0);
	read(rawFD, bootSec, bytesPerBlock);

	bcopy(masterBootImage, bootSec, sizeof(masterBootImage));
	bootSec[510] = 0x55;
	bootSec[511] = 0xaa;
	ioctl(rawFD, FIOSEEK, 0);
	write(rawFD, bootSec, bytesPerBlock);

	/*
	 * Read back boot sector for vxboot partition
	 */
	ioctl(rawFD, FIOSEEK, partData[0].offset * bytesPerBlock);
	read(rawFD, bootSec, bytesPerBlock);

	/*
	 * Setup boot sector appropriately and write it back to the disk
	 */
	bootSec[0] = 0xeb;
	bootSec[1] = 0x3c;
	bootSec[2] = 0x90;
   bcopy ("WINDRIVR",  (char *)&bootSec[0x03], 8);
   bcopy (vxLoaderImage, (char *)&bootSec[0x3e], sizeof(vxLoaderImage));

	ioctl(rawFD, FIOSEEK, partData[0].offset * bytesPerBlock);
	write(rawFD, bootSec, bytesPerBlock);
	rawFsVolUnmount(rawDev);
	cbioIoctl(cbio, CBIO_CACHE_FLUSH, 0);

   printf("\nPress enter to start disk check for /vxboot\n");
   fgets(bootSec, bytesPerBlock, stdin);
   chkdsk("/vxboot", DOS_CHK_ONLY, DOS_CHK_VERB_1);

   printf("\nPress enter to start disk check for /trima\n");
   fgets(bootSec, bytesPerBlock, stdin);
   chkdsk("/trima", DOS_CHK_ONLY, DOS_CHK_VERB_1);

   printf("\nPress enter to start disk check for /config\n");
   fgets(bootSec, bytesPerBlock, stdin);
   chkdsk("/config", DOS_CHK_ONLY, DOS_CHK_VERB_1);

   printf("\nPress enter to start disk check for /machine\n");
   fgets(bootSec, bytesPerBlock, stdin);
   chkdsk("/machine", DOS_CHK_ONLY, DOS_CHK_VERB_1);

	mkdir("/config/crc");
	mkdir("/machine/tmp");
	mkdir("/machine/savedata");

	fp = fopen("/config/globvars", "w");
	if ( !fp )
	{
		printf("Can't create /config/globvars - disk must be configured manually\n");
		return;
	}

  	while ( machineNumber < 0 )
	{
		printf("\nEnter machine number:\n");
		fgets(bootSec, bytesPerBlock, stdin);
		if ( sscanf(bootSec, "%d", &machineNumber) != 1 ||
			  machineNumber <= 0 ||
			  machineNumber > 99999 )
		{
			machineNumber = -1;
		}
	}

	fprintf(fp, "MACHINE=1T%05d\n", machineNumber);

	printf("\nEnter safety network MAC address\n");
	fgets(bootSec, bytesPerBlock, stdin);
	fprintf(fp, "SENPID=%s", bootSec);

	fprintf(fp, "EXTERNALIP=172.21.%d.%d\n", (int)(machineNumber/256), machineNumber%256);
	fprintf(fp, "EXTERNALBIP=172.21.255.255\n");
	fclose(fp);

	bootSec[0] = '?';
	while ( bootSec[0] != 'n' && bootSec[0] != 'N' && bootSec[0] != 'y' && bootSec[0] != 'Y' )
	{
		printf("\nIf you have a floppy disk available with V5 cal.dat and absf.2,\n"
				 "  insert the disk and type 'Y'.  Otherwise, type 'N' and default files\n"
				 "  will be copied to the hard disk\n");
      fgets(bootSec, bytesPerBlock, stdin);
	}

	if ( bootSec[0] == 'Y' || bootSec[0] == 'y' )
	{
		FILE	* fp1;
		FILE  * fp2;

		if ( !copyFile("/fd0/cal.dat", "/config/cal.dat") ) return;
		fp1 = fopen("/config/cal.dat", "a");
		fp2 = fopen("/fd0/absf.2", "r");

		if ( fp1 && fp2 )
		{
			int	x1, x2, y1, y2;

			x1 = 90;
			x2 = 850;
			y1 = 130;
			y2 = 830;
			fscanf(fp2, "%s:%s:%d %d %d %d", bootSec, bootSec, &x1, &x2, &y1, &y2);

			fprintf(fp1, "[TOUCHSCREEN]\n"
							 "#\n"
							 "# Touch screen calibration values\n"
							 "#\n"
							 "# These values specify the logical screen size in pixels.\n"
							 "#\n"
							 "screen_horizontal_size=640\n"
							 "screen_vertical_size=480\n"
							 "\n"
							 "#\n"
							 "# These values correspond to the raw touchscreen readings for the specified\n"
							 "# screen edges.  They are used along with the screen size specified above\n"
							 "# to convert raw touch screen readings to pixel based coordinates.\n"
							 "#\n");

			fprintf(fp1, "tsraw_left_edge=%d\n", x1);
			fprintf(fp1, "tsraw_right_edge=%d\n", x2);
			fprintf(fp1, "tsraw_top_edge=%d\n", y1);
			fprintf(fp1, "tsraw_bottom_edge=%d\n", y2);
			fclose(fp2);
			fclose(fp1);
		}
	}
	else
	{
		if ( !copyFile("/fd0/cal.dat", "/config/cal.dat") ) return;
	}

	cbioIoctl(cbio, CBIO_CACHE_FLUSH, 0);
	printf("\nDisk build complete\n");
}


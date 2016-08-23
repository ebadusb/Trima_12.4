/*
 * Copyright (c) 2001 by Gamrbo BCT, Inc.  All rights reserved.
 *
 * computehwcrc.cpp - compute CRC values for use in hw.dat
 *
 * See the proc state CPUIDTest, and the program drivercheck
 * for details on usage.
 *
 * $Header$
 * $Log$
 */

#include "crcgen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Usage(const char * progName)
{
   fprintf(stderr, "Usage 1: %s <value1> <value2> ...\n", progName);
   fprintf(stderr, "where values are as specified in the tracelog (in hexadecimal)\n\n");

   fprintf(stderr, "Usage 2: %s -string <string>\n", progName);
   fprintf(stderr, "where string is the command line to be used in hw.dat\n");
   exit(1);
}

void main(int argc, char *argv[])
{
   if (argc <= 1)
   {
      Usage(argv[0]);
   }

   if ( strcmp(argv[1], "-string") == 0 )
   {
      if ( argc != 3 )
      {
         Usage(argv[0]);
      }

      unsigned long  crc = 0;
      crcgen32(&crc, argv[2], strlen(argv[2]));
      printf("String CRC=%08lx\n", crc);
   }
   else
   {
      unsigned long  * data = (unsigned long *)alloca((argc-1)*sizeof(unsigned long));
   
      for (int i=1; i<argc; i++)
      {
         if (sscanf(argv[i], "%x", &data[i-1]) != 1)
         {
            Usage(argv[0]);
         }
      }
   
      unsigned long  crc = 0;
      crcgen32(&crc, (char *)data, (argc-1)*sizeof(unsigned long));
      printf("HW CRC=%08lx\n", crc);
   }

   exit(0);
}

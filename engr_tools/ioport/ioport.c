/*
 * $Header$
 * $Log$
 * 
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ioport.c
 * PURPOSE:    debug test tool for performing raw I/O port read/write operations
 */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

void Usage(char * progName)
{
   fprintf(stderr, "Read/Write I/O ports\n");
   fprintf(stderr, "Usage: %s port <value>\n", progName);
   fprintf(stderr, "With only a port argument specified, the specified port will be read\n");
   fprintf(stderr, "and the result displayed.  If the value argument is specified, that\n");
   fprintf(stderr, "value will be written to the port.  All values must be specified in hex.\n");
   exit(1);
}

int main(int argc, char * argv[])
{
   if (argc == 2)
   {
      int   iPort;

      if (sscanf(argv[1], "%x", &iPort) != 1)
      {
         Usage(argv[0]);
      }

      printf("Reading port 0x%x = 0x%x\n", iPort, inp(iPort));
   }
   else if (argc == 3)
   {
      int   iPort, iValue;

      if (sscanf(argv[1], "%x", &iPort) != 1 ||
          sscanf(argv[2], "%x", &iValue) != 1)
      {
         Usage(argv[0]);
      }

      printf("Writing port 0x%x = 0x%x\n", iPort, iValue);
      outp(iPort, iValue);
   }
   else
   {
      Usage(argv[0]);
   }

   return 0;
}

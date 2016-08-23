/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      System ID Logger
 *
 * ABSTRACT:   Logs system information to the trace logs.  This information
 *             includes:
 *
 *                - CPU information (speed, type)
 *                - system ROM information (ID, copyright strings)
 *                - expansion ROM information (ID, copyright strings)
 *                - control board revision, FPGA revision
 *
 *	USAGE:		sysidlog("safety");  - on safety processor
 *					sysidlog("control"); - on control processor
 *
 * DOCUMENTS:  Developed in response to IT 4739.
 */

#include <vxWorks.h>
#include <bspVersion.h>
#include <ctype.h>
#include <dosFsLib.h>
#include <envLib.h>
#include <fcntl.h>
#include <ioLib.h>
#include <kernelLib.h>
#include <stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common_hw.h"
#include "crcgen.h"
#include "datfile.h"
#include "filenames.h"
#include "trima_datalog.h"
#include "sys_info.h"
#include "file_system.h"
#include "node_data.h"
#include "trimaOs.h"

//
// Local assembly language support functions for the CPUID instruction,
// supported on late model 486 and later processors.
//
static unsigned int CPU_GetFamily (void);
static unsigned int CPU_GetGenericIDInfo (void);
static unsigned int CPU_GetModel (void);
static void         CPU_GetRawIDData (unsigned long* buffer);
static unsigned int CPU_GetSteppingID (void);
static unsigned int CPU_GetType (void);
static void         CPU_GetVendor (char* buffer);
static bool         CPU_TestIDFlag (void);

//
// Local function prototypes
//
static const char*   FindIDString (const char* memPtr, unsigned int memLength, const char* pattern, unsigned int& memStart, unsigned int& stringLength);
static unsigned long GetROMChecksum (const char* memPtr, unsigned int memLength);
static void          LogAMDCPUInfo (void);
static void          LogBIOSInfo (void);
static void          LogBoardRev (void);
static void          LogCPUInfo (void);
static void          LogDiskSpaceInfo (void);
static void          LogExpansionROMInfo (unsigned int romAddr);
static void          LogHWDat (void);
static void          LogIntelCPUInfo (void);
static void          LogItem (const char* file, int line, const char* variableName, const char* value);
static void          LogItem (const char* file, int line, const char* variableName, int value);
static void          LogItem (const char* file, int line, const char* variableName, unsigned long value);
static void          LogROMChecksum (const char* memPtr, unsigned int memLength, const char* itemName);

//
// Static data used for logging
//
static const char* systemID  = NULL;
static const char* controlID = "control";
static const char* safetyID  = "safety";

//
// CPU_TestIDFlag
//    Tests if the ID bit in EFLAGS can be set.  If it can, the CPUID
//    instruction is supported by the current processor.
//
static bool CPU_TestIDFlag (void)
{
   unsigned int flagTest;     // must be a 32 bit value

   __asm__ volatile ("pushfl"                                 );  //
   __asm__ volatile ("pop %%eax"             : : : "eax"       ); // eax = CPU flag settings
   __asm__ volatile ("orl $0x200000, %%eax"  : : : "eax"       ); // set ID bit in flags
   __asm__ volatile ("pushl %eax"                             );  //
   __asm__ volatile ("popfl"                                  );  // set CPU flags
   __asm__ volatile ("pushfl"                                 );  //
   __asm__ volatile ("pop %%eax"             : : : "eax"       ); // eax = CPU flag settings
   __asm__ volatile ("andl $0x200000, %%eax" : : : "eax"       ); // test if CPU allowed ID bit to be set
   __asm__ volatile ("movl %%eax, %0"        : "=m" (flagTest) ); // flagTest = result

   //
   // If the CPU supports the CPUID instruction, the ID bit will
   // be supported.  Otherwise, the CPU will force the ID bit to 0
   //
   return(flagTest != 0);
}

//
// CPU_GetRawIDData
//    Get 4 unsigned longs from CPUID instruction.  The
//    input parameter must be an unsigned long array of
//    length 4.
//
static void CPU_GetRawIDData (unsigned long* buffer)
{
   __asm__ volatile ("xorl %%eax, %%eax" : : : "eax"                      );  // eax = 0
   __asm__ volatile ("cpuid"             : : : "eax", "ebx", "ecx", "edx" );  // get CPU ID information
   __asm__ volatile ("movl %%eax, %0"    : "=m" (buffer[0])              );
   __asm__ volatile ("movl %%ebx, %0"    : "=m" (buffer[1])              );
   __asm__ volatile ("movl %%ecx, %0"    : "=m" (buffer[2])              );
   __asm__ volatile ("movl %%edx, %0"    : "=m" (buffer[3])              );   // save CPU ID information in buffer
}

//
// CPU_GetVendor
//    Get 12 byte vendor ID string.  The input parameter buffer
//    must be at least 13 bytes long (12 byte vendor string +
//    null terminator).
//
static void CPU_GetVendor (char* buffer)
{
   __asm__ volatile ("xorl %%eax, %%eax" : : : "eax"                      );  // eax = 0
   __asm__ volatile ("cpuid"             : : : "eax", "ebx", "ecx", "edx" );  // get CPU ID information
   __asm__ volatile ("movl %%ebx, %0"    : "=m" (buffer[0])               );
   __asm__ volatile ("movl %%edx, %0"    : "=m" (buffer[4])               );
   __asm__ volatile ("movl %%ecx, %0"    : "=m" (buffer[8])               );  // save CPU vendor information in buffer
   __asm__ volatile ("xorb %%al, %%al"   : : : "eax"                      );  // al = 0
   __asm__ volatile ("movb %%al, %0"     : "=m" (buffer[12])              );  // NULL terminate vendor string

}

//
// CPU_GetGenericIDInfo
//    Generic CPU ID information - supported by any CPU with CPUID instruction
//
static unsigned int CPU_GetGenericIDInfo (void)
{
   unsigned int result;

   __asm__ volatile ("movl $1, %%eax"    : : : "eax"                      );  // eax = 1
   __asm__ volatile ("cpuid"             : : : "eax", "ebx", "ecx", "edx" );  // get CPU ID information
   __asm__ volatile ("movl %%eax, %0"    : "=m" (result)                  );

   return result;
}

//
// CPU_GetSteppingID
//    Supported for Intel and AMD processors - returns processor stepping number
//
static unsigned int CPU_GetSteppingID (void)
{
   unsigned int result;

   __asm__ volatile ("movl $1, %%eax"    : : : "eax"                      );  // eax = 1
   __asm__ volatile ("cpuid"             : : : "eax", "ebx", "ecx", "edx" );  // get CPU ID information
   __asm__ volatile ("andl $0x0f, %%eax" : : : "eax"                      );  // mask out all but CPU stepping information
   __asm__ volatile ("movl %%eax, %0"    : "=m" (result)                  );

   return result;
}

//
// CPU_GetModel
//    Supported for Intel and AMD processors - returns processor model number
//
static unsigned int CPU_GetModel (void)
{
   unsigned int result;

   __asm__ volatile ("movl $1, %%eax"    : : : "eax"                      );  // eax = 1
   __asm__ volatile ("cpuid"             : : : "eax", "ebx", "ecx", "edx" );  // get CPU ID information
   __asm__ volatile ("movl %%eax, %0"    : "=m" (result)                  );

   result = (result << 4) & 0x0f;
   return result;
}

//
// CPU_GetFamily
//    Supported for Intel and AMD processors - returns processor family
//
static unsigned int CPU_GetFamily (void)
{
   unsigned int result;

   __asm__ volatile ("movl $1, %%eax"    : : : "eax"                      );  // eax = 1
   __asm__ volatile ("cpuid"             : : : "eax", "ebx", "ecx", "edx" );  // get CPU ID information
   __asm__ volatile ("movl %%eax, %0"    : "=m" (result)                  );

   result = (result << 8) & 0x0f;
   return result;
}

//
// CPU_GetType
//    Supported only for Intel processors - returns processor type
//
static unsigned int CPU_GetType (void)
{
   unsigned int result = 0;

   __asm__ volatile ("movl $1, %%eax"    : : : "eax"                      );  // eax = 1
   __asm__ volatile ("cpuid"             : : : "eax", "ebx", "ecx", "edx" );  // get CPU ID information
   __asm__ volatile ("movl %%eax, %0"    : "=m" (result)                  );

   result = (result << 12) & 0x03;
   return result;
}

//
// The following was entered from AMD application note 20734R, and
// is used to indentify AMD processors from the CPUID instruction
//
static const char* amdVendorID = "AuthenticAMD";
struct AMDProcessorData
{
   unsigned char family;
   unsigned char model;
   const char*   idString;
};

static AMDProcessorData amdProcessorData[] =
{
   { 4,  0, "AM486 or AM5x86, model 0"},
   { 4,  1, "AM486 or AM5x86, model 1"},
   { 4,  2, "AM486 or AM5x86, model 2"},
   { 4,  3, "AM486 or AM5x86, model 3"},
   { 4,  4, "AM486 or AM5x86, model 4"},
   { 4,  5, "AM486 or AM5x86, model 5"},
   { 4,  6, "AM486 or AM5x86, model 6"},
   { 4,  7, "AM486 or AM5x86, model 7"},
   { 4,  8, "AM486 or AM5x86, model 8"},
   { 4,  9, "AM486 or AM5x86, model 9"},
   { 4, 10, "AM486 or AM5x86, model 10"},
   { 4, 11, "AM486 or AM5x86, model 11"},
   { 4, 12, "AM486 or AM5x86, model 12"},
   { 4, 13, "AM486 or AM5x86, model 13"},
   { 4, 14, "AM486 or AM5x86, model 14"},
   { 4, 15, "AM486 or AM5x86, model 14"},
   { 5,  0, "AMD-K5, model 0"},
   { 5,  1, "AMD-K5, model 1"},
   { 5,  2, "AMD-K5, model 2"},
   { 5,  6, "AMD-K6, model 6"},
   { 5,  7, "AMD-K6, model 6"},
   { 5,  8, "AMD-K6-2, model 8"},
   { 5,  9, "AMD-K6-III, model 9"},
   { 6,  1, "AMD Athlon, model 1"},
   { 6,  2, "AMD Athlon, model 2"},
   { 6,  3, "AMD Duron"},
   { 6,  4, "AMD Athlon, model 4"},
   { 0, 15, NULL}   // must be last entry
};

//
// The following was entered from Intel application note AP-485, and
// is used to indentify Intel processors from the CPUID instruction
//
static const char* intelVendorID = "GenuineIntel";
struct IntelProcessorData
{
   unsigned char type;
   unsigned char family;
   unsigned char model;
   const char*   idString;
};

static IntelProcessorData intelProcessorData[] =
{
   { 0, 4,  4, "Intel486 SL"},
   { 0, 4,  7, "Intel486 DX2"},
   { 0, 4,  8, "Intel486 DX4"},
   { 1, 4,  8, "Intel486 DX4 OverDrive"},
   { 0, 5,  1, "Intel Pentium 60-66"},
   { 1, 5,  1, "Intel Pentium 60-66 OverDrive"},
   { 0, 5,  2, "Intel Pentium 75-200"},
   { 1, 5,  2, "Intel Pentium 75-133 OverDrive"},
   { 1, 5,  3, "Intel Pentium OverDrive for 486"},
   { 0, 5,  4, "Intel Pentium 166-200 w/MMX"},
   { 1, 5,  4, "Intel Pentium 75-133 w/MMX OverDrive"},
   { 0, 6,  1, "Intel Pentium Pro"},
   { 0, 6,  3, "Intel Pentium II, model 3"},
   { 0, 6,  5, "Intel Pentium II or Celeron, model 5"},
   { 0, 6,  6, "Intel Celeron, model 6"},
   { 0, 6,  7, "Intel Pentium III, model 7"},
   { 0, 6,  8, "Intel Pentium III or Celeron, model 8"},
   { 0, 6, 10, "Intel Pentium III, model A"},
   { 0, 15, 0, "Intel Pentium 4"},
   { 0, 0,  0, NULL}   // must be last entry
};

//
// The following strings are used to identify ID/copyright
// strings within ROM data
//
enum { MaxIDStringLength = 256 };

static const char* romIDString[] =
{ "copyright",
  "Copyright",
  "COPYRIGHT",
  "BIOS",
  NULL        // must be last entry
};


//
// Log an ID item.  There are several versions depending on the type of value
//
static void LogItem (const char* file,
                     int line,
                     const char* variableName,
                     const char* value
                     )
{
   DataLog_Default << "sysidlog(" << systemID << "): " << variableName << "=" << value << endmsg;
}



static void LogItem (const char* file,
                     int line,
                     const char* variableName,
                     int value
                     )
{
   //
   // Decimal numbers require 2.4 characters per byte (rounded up to 3),
   // plus a position for the - sign and one for the terminator.
   //
   char temp[3 * sizeof(int) + 2];

   sprintf(temp, "%d", value);
   LogItem(file, line, variableName, temp);
}



static void LogItem (const char* file,
                     int line,
                     const char* variableName,
                     unsigned long value
                     )
{
   //
   // Unsigned longs are logged as HEX values, and require 2 characters
   // per byte plus two for a leading "0x" plus one for the terminating \0
   //
   char temp[2 * sizeof(unsigned long) + 3];

   sprintf(temp, "0x%08lX", value);
   LogItem(file, line, variableName, temp);
}



//
// Handle CPU indentification for AMD processors
//
static void LogAMDCPUInfo (void)
{
   const char* modelString = NULL;
   int         family      = CPU_GetFamily();
   int         model       = CPU_GetModel();
   int         steppingID  = CPU_GetSteppingID();

   int         idx         = 0;
   bool        found       = false;
   while (amdProcessorData[idx].idString && !found)
   {
      if (family == amdProcessorData[idx].family &&
          model == amdProcessorData[idx].model)
      {
         modelString = amdProcessorData[idx].idString;
         found       = true;
      }
      else
      {
         idx += 1;
      }
   }

   if (!found)
   {
      LogItem(__FILE__, __LINE__, "CPU family ID", family);
      LogItem(__FILE__, __LINE__, "CPU model ID", model);
   }
   else
   {
      LogItem(__FILE__, __LINE__, "CPU model", modelString);
   }

   LogItem(__FILE__, __LINE__, "CPU stepping ID", steppingID);
}



//
// Handle CPU indentification for Intel processors
//
static void LogIntelCPUInfo (void)
{
   const char* modelString = NULL;
   int         type        = CPU_GetType();
   int         family      = CPU_GetFamily();
   int         model       = CPU_GetModel();
   int         steppingID  = CPU_GetSteppingID();

   int         idx         = 0;
   bool        found       = false;
   while (intelProcessorData[idx].idString && !found)
   {
      if (type == intelProcessorData[idx].type &&
          family == intelProcessorData[idx].family &&
          model == intelProcessorData[idx].model)
      {
         modelString = intelProcessorData[idx].idString;
         found       = true;
      }
      else
      {
         idx += 1;
      }
   }

   if (!found)
   {
      LogItem(__FILE__, __LINE__, "CPU type ID", type);
      LogItem(__FILE__, __LINE__, "CPU family ID", family);
      LogItem(__FILE__, __LINE__, "CPU model ID", model);
   }
   else
   {
      LogItem(__FILE__, __LINE__, "CPU model", modelString);
   }

   LogItem(__FILE__, __LINE__, "CPU stepping ID", steppingID);
}



//
// Handle CPU identification
//
static void LogCPUInfo (void)
{
   //
   // Log information about the OS and processor speed
   //
   LogItem(__FILE__, __LINE__, "BSP version", sysBspRev());
   LogItem(__FILE__, __LINE__, "kernel version", sysBspRev());

/***** RJH_08_28_07: NOTE: sysCPUSpeed() does not work with vxWorks 5.5.1 for now *****/
// LogItem(__FILE__, __LINE__, "OS reported CPU speed", sysCPUSpeed());

   //
   // Log information about the processor type reported by CPUID,
   // if the instruction is supported.  See Intel application note
   // AP-485 for information on INTEL_??? definitions and usage of
   // the CPUID instruction for Intel processors.
   //
   if (CPU_TestIDFlag())
   {
      char vendor[13];
      CPU_GetVendor(vendor);

      LogItem(__FILE__, __LINE__, "CPU vendor", vendor);
      LogItem(__FILE__, __LINE__, "CPU ID word", (unsigned long)CPU_GetGenericIDInfo());

      if (strcmp(vendor, amdVendorID) == 0)
      {
         LogAMDCPUInfo();
      }
      else if (strcmp(vendor, intelVendorID) == 0)
      {
         LogIntelCPUInfo();
      }
   }
   else
   {
      LogItem(__FILE__, __LINE__, "CPU ID", "not supported by processor");
   }
}



static unsigned long GetROMChecksum (const char* memPtr,      // start of memory block
                                     unsigned int memLength   // length of memory block (in bytes)
                                     )
{
   unsigned long crc = 0;
   crcgen32(&crc, (const unsigned char*)memPtr, memLength);
   return crc;
}



//
// Compute and then log the checksum for a given memory block
//
static void LogROMChecksum (const char* memPtr,      // start of memory block
                            unsigned int memLength,  // length of memory block (in bytes)
                            const char* itemName     // item name for log file
                            )
{
   LogItem(__FILE__, __LINE__, itemName, GetROMChecksum(memPtr, memLength));
}



//
// Scan memory for specified character pattern.  Used to find ID strings
// in BIOS ROM and expansion ROM memory areas.
//
static const char* FindIDString (const char* memPtr,          // start of memory block
                                 unsigned int memLength,      // length of memory block (in bytes)
                                 const char* pattern,         // pattern to search for
                                 unsigned int& memStart,      // starting byte within memory block for search
                                 unsigned int& stringLength   // length of found string
                                 )
{
   int         patternLength = strlen(pattern);
   const char* resultString  = NULL;

   while (memStart < memLength - patternLength &&
          !resultString)
   {
      if (memPtr[memStart] == pattern[0] &&
          memcmp(&memPtr[memStart], pattern, patternLength) == 0)
      {
         resultString = &memPtr[memStart];
      }
      else
      {
         memStart += 1;
      }
   }

   if (resultString)
   {
      //
      // Found the specified pattern.  Extend the string to get the printable
      // characters surrounding it.
      //
      unsigned int stringStart = memStart;
      while (stringStart > 0 &&
             isprint(memPtr[stringStart - 1]) &&
             (memPtr[stringStart - 1] & 0x80) == 0)
      {
         stringStart -= 1;
      }

      stringLength = patternLength + (memStart - stringStart);
      while (stringStart + stringLength < memLength &&
             stringLength < MaxIDStringLength &&
             isprint(memPtr[stringStart + stringLength]) &&
             (memPtr[stringStart + stringLength] & 0x80) == 0)
      {
         stringLength += 1;
      }

      resultString = &memPtr[stringStart];
      memStart     = stringStart + stringLength;

      if (stringStart + stringLength < memLength &&
          resultString[stringLength] != '\0' &&
          resultString[stringLength] != '\r' &&
          resultString[stringLength] != '\n')
      {
         //
         // Reject strings that do not end at the end of the ROM, or do not
         // end with \0, \n, or \r.
         //
         resultString = NULL;
      }
      else
      {
         //
         // Strip trailing spaces from string
         //
         while (resultString[stringLength] == ' ' && stringLength > 0)
         {
            stringLength -= 1;
         }
      }
   }

   return resultString;
}



//
// Scan system BIOS memory area for ID strings
//
static void LogBIOSInfo (void)
{
   const unsigned int BIOSAddress = 0xf0000;
   const unsigned int BIOSLength  = 0x10000;
   char               logData[MaxIDStringLength];

   const char*        romPtr = (const char*)(BIOSAddress);
   LogROMChecksum(romPtr, BIOSLength, "BIOS CRC");

   int stringIdx = 0;
   while (romIDString[stringIdx])
   {
      unsigned int searchStartIdx = 0;
      unsigned int stringLength;
      const char*  stringInfo     = FindIDString(romPtr, BIOSLength, romIDString[stringIdx], searchStartIdx, stringLength);

      while (stringInfo)
      {
         if (stringLength < MaxIDStringLength)
         {
            strncpy(logData, stringInfo, stringLength);
            logData[stringLength] = '\0';
            LogItem(__FILE__, __LINE__, "BIOS ID", logData);
         }

         stringInfo = FindIDString(romPtr, BIOSLength, romIDString[stringIdx], searchStartIdx, stringLength);
      }

      stringIdx += 1;
   }
}



//
// Scan specified expansion ROM memory area for ID strings
//
static void LogExpansionROMInfo (unsigned int romAddr)
{
   const unsigned int ROMLength = 0x8000;
   char               logData[MaxIDStringLength];

   //
   // Valid expansion ROMs start with the two byte pattern 0x55 0xaa
   //
   const char* romPtr = (const char*)romAddr;
   if (romPtr[0] == (char)0x55 && romPtr[1] == (char)0xaa)
   {
      char romAddrString[20];
      sprintf(romAddrString, "0x%X", romAddr);
      LogItem(__FILE__, __LINE__, "Expansion ROM", romAddrString);

      char romCRCString[20];
      sprintf(romCRCString, "ROM(0x%X) CRC", romAddr);
      LogROMChecksum(romPtr, ROMLength, romCRCString);

      sprintf(romAddrString, "ROM(0x%X) ID", romAddr);
      int stringIdx = 0;
      while (romIDString[stringIdx])
      {
         unsigned int searchStartIdx = 0;
         unsigned int stringLength;
         const char*  stringInfo     = FindIDString(romPtr, ROMLength, romIDString[stringIdx], searchStartIdx, stringLength);

         while (stringInfo)
         {
            if (stringLength < MaxIDStringLength)
            {
               strncpy(logData, stringInfo, stringLength);
               logData[stringLength] = '\0';
               LogItem(__FILE__, __LINE__, romAddrString, logData);
            }

            stringInfo = FindIDString(romPtr, ROMLength, romIDString[stringIdx], searchStartIdx, stringLength);
         }

         stringIdx += 1;
      }
   }
}



//
// Log revision information for the Trima control hardware
//
static void LogBoardRev (void)
{
   hw_init();

   unsigned short version;
   char           versionString[10];

   if (hw_mxFpgaInstalled())
   {

      if (hw_getBoardType() == hw_controlBoard)
      {

         LogItem(__FILE__, __LINE__, "Control1 FPGA Firmware Revision", hw_fpgaFwRevision(hw_control1Fpga) );
         LogItem(__FILE__, __LINE__, "Control1 FPGA Interface Revision", hw_fpgaIntfRevision(hw_control1Fpga));
         LogItem(__FILE__, __LINE__, "Control1 FPGA ID Revision", hw_fpgaIdRevision(hw_control1Fpga));
         LogItem(__FILE__, __LINE__, "Control2 FPGA Firmware Revision", hw_fpgaFwRevision(hw_control2Fpga) );
         LogItem(__FILE__, __LINE__, "Control2 FPGA Interface Revision", hw_fpgaIntfRevision(hw_control2Fpga));
         LogItem(__FILE__, __LINE__, "Control2 FPGA ID Revision", hw_fpgaIdRevision(hw_control2Fpga));
         LogItem(__FILE__, __LINE__, "Control CCA Revision", hw_ccaRevision());
      }
      else if (hw_getBoardType() == hw_safetyBoard)
      {

         LogItem(__FILE__, __LINE__, "Safety FPGA Firmware Revision", hw_fpgaFwRevision(hw_safetyFpga) );
         LogItem(__FILE__, __LINE__, "Safety FPGA Interface Revision", hw_fpgaIntfRevision(hw_safetyFpga));
         LogItem(__FILE__, __LINE__, "Safety FPGA ID Revision", hw_fpgaIdRevision(hw_safetyFpga));
         LogItem(__FILE__, __LINE__, "Safety CCA Revision", hw_ccaRevision());
      }
      else
      {

         LogItem(__FILE__, __LINE__, "PROCESSOR BOARD TYPE:", "UNKNOWN");
      }
   }
   else
   {

      version = hw_hardwareVersion();
      sprintf(versionString, "%d.%d", (version & 0xff00) >> 8, version & 0xff);
      LogItem(__FILE__, __LINE__, "Hardware version", versionString);
      LogItem(__FILE__, __LINE__, "Board revision", hw_boardVersion());
      LogItem(__FILE__, __LINE__, "FPGA revision", hw_fpgaVersion());
   }
}



//
// Log disk space information
//
static void LogDiskSpaceInfo (void)
{
   TrimaPartitionData data;
   TrimaPartitionID   id;

   for (id = FirstPartition; id < EndOfPartitionEntries; id = (TrimaPartitionID)(id + 1))
   {
      if (trimaSysGetPartitionData(id, &data) >= 0)
      {
         char itemString[80];

         sprintf(itemString, "Partition %d name", (int)id);
         LogItem(__FILE__, __LINE__, itemString, data.name);

         sprintf(itemString, "Partition %d size (MB)", (int)(id));
         LogItem(__FILE__, __LINE__, itemString, (int)(data.bytesTotal / 1024 / 1024));

         sprintf(itemString, "Partition %d offset (MB)", (int)(id));
         LogItem(__FILE__, __LINE__, itemString, (int)(data.bytesOffset / 1024 / 1024));

         int fd = open(data.name, O_RDONLY, 0644);
         if (fd >= 0)
         {
            long long freeBytes;
            ioctl(fd, FIONFREE64, (int)&freeBytes);

            sprintf(itemString, "Partition %d free space (MB)", (int)(id));
            LogItem(__FILE__, __LINE__, itemString, (int)(freeBytes / 1024 / 1024));
            close(fd);
         }
      }
   }
}



// Called by trima tasks
extern "C" void sysidlog (void)
{
   //
   // Connect to log file
   //
   datalog_SetDefaultLevel(log_handle_startup_detail);

   //
   // Check incoming argument and assign file name for hardware CRC information
   //
   const char* checksumFileName = NULL;

   if (sysGetLocalNodeID() == ControlNode)
   {
      checksumFileName = PNAME_CONTROL_HW_CRC;
      systemID         = controlID;
   }
   else
   {
      checksumFileName = PNAME_SAFETY_HW_CRC;
      systemID         = safetyID;
   }

   FILE* fp = fopen(checksumFileName, "w");
   if (fp)
   {
      unsigned long cpuData[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};

      /***** RJH_08_28_07: NOTE: sysCPUSpeed() does not work with vxWorks 5.5.1 fornow *****/
      // unsigned long  cpuSpeed = RoundCPUSpeed (sysCPUSpeed());

      if (CPU_TestIDFlag())
      {
         CPU_GetRawIDData(cpuData);
      }

      DataLog_Default << "sysidlog: cpuData0 = " << cpuData[0] << endmsg;
      DataLog_Default << "sysidlog: cpuData1 = " << cpuData[1] << endmsg;
      DataLog_Default << "sysidlog: cpuData2 = " << cpuData[2] << endmsg;
      DataLog_Default << "sysidlog: cpuData3 = " << cpuData[3] << endmsg;

      /***** RJH_08_28_07: NOTE: sysCPUSpeed() does not work with vxWorks 5.5.1 fornow *****/
      // DataLog_Default << "sysidlog: cpuSpeed = " << cpuSpeed << endmsg;

      //
      // Write the file used by the proc state CPUIDTest.
      //
      fprintf (fp, "%8lX %8lX %8lX %8lX\n", cpuData[0], cpuData[1], cpuData[2], cpuData[3]);

      /***** RJH_08_28_07: NOTE: sysCPUSpeed() does not work with vxWorks 5.5.1 fornow *****/
      // fprintf (fp, "%8lX\n", cpuSpeed);

      fclose(fp);
   }

   LogCPUInfo();
   LogBIOSInfo();

   //
   // Valid expansion ROM locations are on 0x8000 byte boundaries from 0xc0000
   // to 0xe8000
   //
   LogExpansionROMInfo(0xc0000);
   LogExpansionROMInfo(0xc8000);
   LogExpansionROMInfo(0xd0000);
   LogExpansionROMInfo(0xd8000);
   LogExpansionROMInfo(0xe0000);
   LogExpansionROMInfo(0xe8000);

   //
   // Board and gate array revision information
   //
   LogBoardRev();

   //
   // Disk space information (only on control side)
   //
   if (sysGetLocalNodeID() == ControlNode)
   {
      LogDiskSpaceInfo();
   }

   return;
}

/* FORMAT HASH 63fb25a14e3975823f9eab0855016912 */
